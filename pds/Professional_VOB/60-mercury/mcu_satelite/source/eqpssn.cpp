/*****************************************************************************
   模块名      : mcu
   文件名      : eqpssn.cpp
   相关文件    : eqpssn.h
   文件实现功能: 外设会话应用类实现
   作者        : 薛新文
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/08/19  0.9         薛新文      创建
   2002/08/20  0.9         LI Yi       添加代码
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "eqpssn.h"
#include "mcuerrcode.h"
#include "mpmanager.h"
#include "mcuver.h"

CEqpSsnApp	g_cEqpSsnApp;	//外设会话应用实例

CEqpSsnInst::CEqpSsnInst()
{
	m_dwEqpNode = INVALID_NODE;
	m_byEqpId = 0;
	memset( m_achAlias, 0, MAXLEN_EQP_ALIAS );
}

CEqpSsnInst::~CEqpSsnInst()
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
    02/08/21    1.0         LI Yi        创建
====================================================================*/
void CEqpSsnInst::InstanceDump( u32 param )
{
	char	achType[32];
	memset( achType, 0, sizeof(achType) );

	switch( m_byEqpType )
	{
	case EQP_TYPE_TVWALL:
		strncpy( achType, "TVWALL", sizeof(achType) );
		break;
	case EQP_TYPE_MIXER:
		strncpy( achType, "MIXER", sizeof(achType)  );
		break;
	case EQP_TYPE_RECORDER:
		strncpy( achType, "RECORD", sizeof(achType)  );
		break;
	case EQP_TYPE_DCS:
		strncpy( achType, "DCS", sizeof(achType)  );
		break;
	case EQP_TYPE_BAS:
		strncpy( achType, "BAS", sizeof(achType)  );
		break;
	case EQP_TYPE_VMP:
		strncpy( achType, "VMP", sizeof(achType)  );
		break;
    case EQP_TYPE_PRS:
        strncpy( achType, "PRS", sizeof(achType)  );
        break;
    case EQP_TYPE_VMPTW:
        strncpy( achType, "MPW", sizeof(achType)  );
        break;
	case EQP_TYPE_HDU:
        strncpy( achType, "HDU", sizeof(achType)  );
        break; 
	default:
		strncpy( achType, "N/A", sizeof(achType)  );
		break;
	}
    
	OspPrintf( TRUE, FALSE, "%3u %5u %5u %6s   %s\n", GetInsID(), m_byEqpId, m_dwEqpNode, achType,m_achAlias );
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
    02/08/19    1.0         薛新文        创建
====================================================================*/
void CEqpSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[EqpSsn]: The received msg's pointer in the msg entry is NULL!");
		return;
	}

//	EqpLog("[Eqpssn]: %u(%s) passed! Inst.%d\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), GetInsID() );

	switch( pcMsg->event )
	{
	case TVWALL_MCU_REG_REQ:			//电视墙登记请求
	case REC_MCU_REG_REQ:				//录像机登记请求
	case MIXER_MCU_REG_REQ:				//混音器登记请求
	case BAS_MCU_REG_REQ:				//适配器登记请求
	case VMP_MCU_REGISTER_REQ:          //画面合成器登记请求
	case VMPTW_MCU_REGISTER_REQ:        //复合电视墙登记请求
	case PRS_MCU_REGISTER_REQ:			//PRS向MCU注册请求
    case HDU_MCU_REG_REQ:               //HDU注册登记
		ProcEqpMcuRegMsg( pcMsg );
		break;

	case OSP_DISCONNECT:				//控制台断链通知
		ProcEqpDisconnect( pcMsg );
		break;

	//MCU到外设的消息
	case MCU_TVWALL_START_PLAY_REQ:	    //外设开始播放通知
	case MCU_TVWALL_STOP_PLAY_REQ:		//外设通知播放通知

    case MCU_HDU_START_PLAY_REQ:        //HDU播放
    case MCU_HDU_STOP_PLAY_REQ:         //HDU停止播放

	case MCU_REC_STARTREC_REQ:			//开始录像
	case MCU_REC_PAUSEREC_REQ:			//暂停录像
	case MCU_REC_RESUMEREC_REQ:			//恢复录像
	case MCU_REC_STOPREC_REQ:			//停止录像
	case MCU_REC_STARTPLAY_REQ:			//开始放像
	case MCU_REC_PAUSEPLAY_REQ:			//暂停放像
	case MCU_REC_RESUMEPLAY_REQ:		//恢复放像
	case MCU_REC_STOPPLAY_REQ:			//停止放像
	case MCU_REC_FFPLAY_REQ:			//会议放像快进
	case MCU_REC_FBPLAY_REQ:			//会议放像快退
	case MCU_REC_SEEK_REQ:				//会议放像进度调整
	case MCU_REC_LISTALLRECORD_REQ:		//查询录像机记录
	case MCU_REC_DELETERECORD_REQ:      //删除记录
	case MCU_REC_PUBLISHREC_REQ:        //发布录像请求
	case MCU_REC_CANCELPUBLISHREC_REQ:  //取消发布录像请求
    case MCU_REC_RENAMERECORD_REQ:      //录像文件改名
    case MCU_REC_GETRECPROG_CMD:
    case MCU_REC_GETPLAYPROG_CMD:		
	
	case MCU_MIXER_STARTMIX_REQ:        //MCU开始混音请求
	case MCU_MIXER_STOPMIX_REQ:         //MCU停止混音请求
	case MCU_MIXER_ADDMEMBER_REQ:       //加入成员请求
	case MCU_MIXER_REMOVEMEMBER_REQ:    //删除成员请求
	case MCU_MIXER_FORCEACTIVE_REQ:     //强制成员混音
	case MCU_MIXER_CANCELFORCEACTIVE_REQ:    //取消成员强制混音
	case MCU_MIXER_SETCHNNLVOL_CMD:     //设置某通道音量消息
	case MCU_MIXER_SETMIXDEPTH_REQ:     //设置混音深度请求消息
	case MCU_MIXER_SEND_NOTIFY:         //MCU给混音器的是否要发送码流通知
	case MCU_MIXER_VACKEEPTIME_CMD:     //设置语音激励延时保护时长消息
//	case MCU_MIXER_SETMEMBER_CMD:       //设置混音成员
    case MCU_MIXER_CHANGEMIXDELAY_CMD:
	
	case MCU_BAS_STARTADAPT_REQ:		//启动适配请求
	case MCU_BAS_STOPADAPT_REQ:			//停止适配请求
	case MCU_BAS_SETADAPTPARAM_CMD:		//改变适配参数命令
    case MCU_BAS_FASTUPDATEPIC_CMD:     //
    case MCU_HDBAS_STARTADAPT_REQ:      //启动高清适配请求
    case MCU_HDBAS_STOPADAPT_REQ:       //停止高清适配请求
    case MCU_HDBAS_SETADAPTPARAM_CMD:   //改变高清适配参数命令

	case MCU_VMP_STARTVIDMIX_REQ:       //MCU请求VMP开始工作
	case MCU_VMP_STOPVIDMIX_REQ:        //MCU请求VMP停止工作
	case MCU_VMP_CHANGEVIDMIXPARAM_REQ: //MCU请求VMP改变复合参数
	case MCU_VMP_GETVIDMIXPARAM_REQ:    //MCU请求VMP回送复合参数
	case MCU_VMP_SETCHANNELBITRATE_REQ: //改变画面合成的码率
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
    case MCU_VMP_FASTUPDATEPIC_CMD:
        
    case MCU_VMPTW_STARTVIDMIX_REQ:
	case MCU_VMPTW_STOPVIDMIX_REQ:
	case MCU_VMPTW_CHANGEVIDMIXPARAM_REQ:
	case MCU_VMPTW_GETVIDMIXPARAM_REQ:
	case MCU_VMPTW_SETCHANNELBITRATE_REQ:
    case MCU_VMPTW_UPDATAVMPENCRYPTPARAM_REQ:
    case MCU_VMPTW_FASTUPDATEPIC_CMD:
		
	case MCU_PRS_SETSRC_REQ:			//MCU请求PRS开始保存信息源
//	case MCU_PRS_ADDRESENDCH_REQ:		//MCU请求PRS添加一个重传通道
//	case MCU_PRS_REMOVERESENDCH_REQ:	//MCU请求PRS移出一个重传通道
	case MCU_PRS_REMOVEALL_REQ:			//MCU请求PRS移出所有的重传通道

	case MCU_EQP_SWITCHSTART_NOTIF:		//MCU通知外设交换通道已打开

	case MCU_HDU_CHANGEMODEPORT_NOTIF:  //HDU输入输出制式改变后通知外设
		
    case MCU_HDU_CHANGEVOLUME_CMD:      // 设置音量或静音
		ProcMcuToEqpMsg( pcMsg );
		break;

	case REC_MCU_STARTREC_ACK:			//开始录像应答
	case REC_MCU_STARTREC_NACK:			//开始录像应答
	case REC_MCU_PAUSEREC_ACK:			//暂停录像应答
	case REC_MCU_PAUSEREC_NACK:			//暂停录像应答
	case REC_MCU_RESUMEREC_ACK:			//录像文件改名应答
	case REC_MCU_RESUMEREC_NACK:		//录像文件改名否定应答
	case REC_MCU_STOPREC_ACK:			//停止录像应答
	case REC_MCU_STOPREC_NACK:			//停止录像应答
	case REC_MCU_STARTPLAY_ACK:			//开始放像应答
	case REC_MCU_STARTPLAY_NACK:		//开始放像应答
	case REC_MCU_PAUSEPLAY_ACK:			//暂停放像应答
	case REC_MCU_PAUSEPLAY_NACK:		//暂停放像应答
	case REC_MCU_RESUMEPLAY_ACK:		//恢复放像应答
	case REC_MCU_RESUMEPLAY_NACK:		//恢复放像应答
	case REC_MCU_CHANGERECMODE_ACK:		//改变录像模式
	case REC_MCU_CHANGERECMODE_NACK:	//改变录像模式
	case REC_MCU_STOPPLAY_ACK:			//停止放像应答
	case REC_MCU_STOPPLAY_NACK:			//停止放像应答
	case REC_MCU_FFPLAY_ACK:			//放像快进应答
	case REC_MCU_FFPLAY_NACK:			//放像快进应答
	case REC_MCU_FBPLAY_ACK:			//放像快退应答
	case REC_MCU_FBPLAY_NACK:			//放像快退应答
	case REC_MCU_SEEK_ACK:				//放像进度调整应答
	case REC_MCU_SEEK_NACK:				//放像进度调整像应答
	case REC_MCU_NEEDIFRAME_CMD:         //REC请求关键帧

	case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //混音激励成员改变通知
	case MIXER_MCU_CHNNLVOL_NOTIF:      //某通道音量通知消息
		
//	case MIXER_MCU_SETMIXDEPTH_ACK:     //设置混音深度应答消息
//	case MIXER_MCU_SETMIXDEPTH_NACK:    //设置混音深度拒绝消息
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
	case MIXER_MCU_GRPSTATUS_NOTIF:     //混音组状态通知
	
	case BAS_MCU_STARTADAPT_ACK:		//启动适配应答
	case BAS_MCU_STARTADAPT_NACK:		//启动适配拒绝
	case BAS_MCU_STOPADAPT_ACK:         //停止适配应答
	case BAS_MCU_STOPADAPT_NACK:        //停止适配拒绝
    case BAS_MCU_NEEDIFRAME_CMD:        //适配器请求I帧
    case HDBAS_MCU_STARTADAPT_ACK:      //启动高清适配应答
    case HDBAS_MCU_STARTADAPT_NACK:     //启动高清适配拒绝
    case HDBAS_MCU_STOPADAPT_ACK:       //停止高清适配应答
    case HDBAS_MCU_STOPADAPT_NACK:      //停止高清适配拒绝
		
	case VMP_MCU_STARTVIDMIX_ACK:       //VMP给MCU开始工作确认
	case VMP_MCU_STARTVIDMIX_NACK:      //VMP给MCU开始工作拒绝
	case VMP_MCU_STARTVIDMIX_NOTIF:     //VMP给MCU开始工作通知
	case VMP_MCU_STOPVIDMIX_ACK:        //VMP给MCU停止工作确认
	case VMP_MCU_STOPVIDMIX_NACK:       //VMP给MCU停止工作拒绝
	case VMP_MCU_STOPVIDMIX_NOTIF:      //VMP给MCU停止工作通知
	case VMP_MCU_CHANGEVIDMIXPARAM_ACK: //VMP给MCU改变复合参数确认
	case VMP_MCU_CHANGEVIDMIXPARAM_NACK://VMP给MCU改变复合参数拒绝
	case VMP_MCU_CHANGESTATUS_NOTIF:    //VMP给MCU改变复合参数通知
	case VMP_MCU_GETVIDMIXPARAM_ACK:    //VMP给MCU回送复合参数确认
	case VMP_MCU_GETVIDMIXPARAM_NACK:   //VMP给MCU回送复合参数拒绝
	case VMP_MCU_SETCHANNELBITRATE_ACK: //改变码率应答
	case VMP_MCU_SETCHANNELBITRATE_NACK://改变码率拒绝
	case VMP_MCU_NEEDIFRAME_CMD:        //画面合成请求I帧

    case VMPTW_MCU_STARTVIDMIX_ACK:       //VMPTW给MCU开始工作确认
	case VMPTW_MCU_STARTVIDMIX_NACK:      //VMPTW给MCU开始工作拒绝
	case VMPTW_MCU_STARTVIDMIX_NOTIF:     //VMPTW给MCU开始工作通知
	case VMPTW_MCU_STOPVIDMIX_ACK:        //VMPTW给MCU停止工作确认
	case VMPTW_MCU_STOPVIDMIX_NACK:       //VMPTW给MCU停止工作拒绝
	case VMPTW_MCU_STOPVIDMIX_NOTIF:      //VMPTW给MCU停止工作通知
	case VMPTW_MCU_CHANGEVIDMIXPARAM_ACK: //VMPTW给MCU改变复合参数确认
	case VMPTW_MCU_CHANGEVIDMIXPARAM_NACK://VMPTW给MCU改变复合参数拒绝
	case VMPTW_MCU_CHANGESTATUS_NOTIF:    //VMPTW给MCU改变复合参数通知
	case VMPTW_MCU_GETVIDMIXPARAM_ACK:    //VMPTW给MCU回送复合参数确认
	case VMPTW_MCU_GETVIDMIXPARAM_NACK:   //VMPTW给MCU回送复合参数拒绝
	case VMPTW_MCU_SETCHANNELBITRATE_ACK: //VMPTW给MCU改变码率应答
	case VMPTW_MCU_SETCHANNELBITRATE_NACK://VMPTW给MCU改变码率拒绝
	case VMPTW_MCU_NEEDIFRAME_CMD:        //VMPTW给MCU请求I帧

	case PRS_MCU_SETSRC_ACK:			//PRS给MCU保存信息源确认
	case PRS_MCU_SETSRC_NACK:			//PRS给MCU保存信息源拒绝
//	case PRS_MCU_ADDRESENDCH_ACK:		//PRS给MCU停止工作确认
//	case PRS_MCU_ADDRESENDCH_NACK:		//PRS给MCU停止工作拒绝
//	case PRS_MCU_REMOVERESENDCH_ACK:	//PRS给MCU改变复合参数确认
//	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS给MCU改变复合参数拒绝
	case PRS_MCU_REMOVEALL_ACK:			//PRS给MCU改变复合参数确认
	case PRS_MCU_REMOVEALL_NACK:		//PRS给MCU改变复合参数拒绝
		
	case HDU_MCU_NEEDIFRAME_CMD:        // HDU请求关键帧
		ProcEqpToMcuMsg( pcMsg );
		break;

	case TVWALL_MCU_STATUS_NOTIF:	    //电视墙状态通知
    case HDU_MCU_STATUS_NOTIF:          //高清电视墙状态通知
	case REC_MCU_RECSTATUS_NOTIF:		//录像机状态通知
	case REC_MCU_RECORDCHNSTATUS_NOTIF:	//录像机录像信道状态通知
	case REC_MCU_PLAYCHNSTATUS_NOTIF:	//录像机放像信道状态通知
	case REC_MCU_RECORDPROG_NOTIF:		//当前录像进度通知
	case REC_MCU_PLAYPROG_NOTIF:		//当前放像进度通知
	case REC_MCU_EXCPT_NOTIF:			//录像机异常通知消息
	
	case MIXER_MCU_MIXERSTATUS_NOTIF:   //混音器状态通知
	
	case BAS_MCU_BASSTATUS_NOTIF:		//适配器状态通知
	case BAS_MCU_CHNNLSTATUS_NOTIF:     //适配通道状态通知
    case HDBAS_MCU_BASSTATUS_NOTIF:     //高清适配器状态通知
    case HDBAS_MCU_CHNNLSTATUS_NOTIF:   //高清适配器通道状态通知
    case HDU_MCU_CHNNLSTATUS_NOTIF:     //高清电视墙通道状态通知
    case VMP_MCU_VMPSTATUS_NOTIF:       //画面合成器状态通知
    case VMPTW_MCU_VMPTWSTATUS_NOTIF:   //复合电视墙状态通知
	case PRS_MCU_PRSSTATUS_NOTIF:		//VMP给MCU的状态上报
	case PRS_MCU_SETSRC_NOTIF:			//PRS给MCU的保存信息源结果
//	case PRS_MCU_ADDRESENDCH_NOTIF:		//PRS给MCU的停止结果
//	case PRS_MCU_REMOVERESENDCH_NOTIF:	//PRS给MCU的状态改变结果
	case PRS_MCU_REMOVEALL_NOTIF:		//PRS给MCU的状态改变结果

	case REC_MCU_LISTALLRECORD_ACK:     //录像机列应答(完毕)消息
	case REC_MCU_LISTALLRECORD_NACK:    //录像机拒绝录像列表请求
	case REC_MCU_LISTALLRECORD_NOTIF:	//录像机记录通知
    case REC_MCU_DELETERECORD_ACK:      //删除记录应答消息
	case REC_MCU_DELETERECORD_NACK:     //删除记录拒绝消息
    case REC_MCU_RENAMERECORD_ACK:      //更改记录应答消息
	case REC_MCU_RENAMERECORD_NACK:     //更改记录拒绝消息
	case REC_MCU_PUBLISHREC_ACK:		//发布录像应答
	case REC_MCU_PUBLISHREC_NACK:		//发布录像拒绝
	case REC_MCU_CANCELPUBLISHREC_ACK:  //取消发布录像应答
	case REC_MCU_CANCELPUBLISHREC_NACK: //取消发布录像拒绝

		ProcEqpToMcuDaemonConfMsg( pcMsg );
		break;

	case TVWALL_MCU_START_PLAY_ACK:
	case TVWALL_MCU_START_PLAY_NACK:
	case TVWALL_MCU_STOP_PLAY_ACK:
	case TVWALL_MCU_STOP_PLAY_NACK:

	case HDU_MCU_START_PLAY_ACK:        //hdu播放应答
	case HDU_MCU_START_PLAY_NACK:       //hdu播放拒绝
	case HDU_MCU_STOP_PLAY_ACK:         //停止播放应答
    case HDU_MCU_STOP_PLAY_NACK:        //停止播放应答
		break;
	
    //设置qos值
    case MCU_EQP_SETQOS_CMD:
        {
            CServMsg cMsg(pcMsg->content, pcMsg->length);                    
            ProcMcuEqpSetQos(cMsg);
        }        
        break;
	// 取主备倒换状态
	case EQP_MCU_GETMSSTATUS_REQ:
    case MCU_MSSTATE_EXCHANGE_NTF:
		{
			ProcEqpGetMsStatusReq(pcMsg);
		}
		break;
    //设置画面合成风格
    case MCU_VMP_SETSTYLE_CMD:
        {
            if(EQP_TYPE_VMP == m_byEqpType)
            {
                SendMsgToPeriEqp(pcMsg->event, pcMsg->content, pcMsg->length);
                EqpLog("[EqpSsn] send vmp style msg to eqp<%d> success\n", m_byEqpId);
            }
        }
        break;

	case TEST_MCU_DISCONNECTEQP_CMD:
		ProcMcuDisconnectEqp(pcMsg);
		break;
	default:
		OspPrintf( TRUE, FALSE, "[EqpSsn]: Wrong message %u(%s) received! Inst.%d\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ), GetInsID() );
		break;
	}
}

/*=============================================================================
  函 数 名： ProcMcuEqpSetQos
  功    能： qos msg
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CEqpSsnInst::ProcMcuEqpSetQos(CServMsg &cMsg)
{
    switch( m_byEqpType )
    {
    case EQP_TYPE_TVWALL:        
    case EQP_TYPE_MIXER:        
    case EQP_TYPE_RECORDER:    
    case EQP_TYPE_BAS:      
    case EQP_TYPE_VMP:        
    case EQP_TYPE_VMPTW:
        SendMsgToPeriEqp(cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen());
        EqpLog("[ProcMcuMtSetQos] send qos msg to eqp<%d> success\n", m_byEqpId);
        break;
        
    default:        
        break;
    }
}

/*=============================================================================
  函 数 名： ProcEqpGetMsStatusReq
  功    能： 取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CEqpSsnInst::ProcEqpGetMsStatusReq(CMessage* const pcMsg)
{
    // guzh [4/11/2007] 如果没有通过注册就发送获取请求，很有可能是重新启动前的连接，不理睬
    if ( CurState() == STATE_IDLE )
    {
        OspPrintf(TRUE, FALSE, "[ProcEqpGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }

    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // 如果同步失败,断开相应的连接
        u8 byIsSwitchOk = *pcMsg->content;
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwEqpNode );
            return;
        }        
    }
    
	if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
    {
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
        
        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToPeriEqp(MCU_EQP_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        EqpLog("[ProcEqpGetMsStatusReq] IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }
    return;
}
/*=============================================================================
  函 数 名： ProcMcuDisconnectEqp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CEqpSsnInst::ProcMcuDisconnectEqp(CMessage * const pcMsg)
{
	u8 EqpId = *(u8*)pcMsg->content;
	if ( EqpId == m_byEqpId )
	{
		if ( OspIsValidTcpNode( m_dwEqpNode ) ) 
		{
			if ( OspDisconnectTcpNode( m_dwEqpNode ) )
				OspPrintf( TRUE, FALSE, "[Int EqpSsn]: The Eqp%u disconnected!\n" );	
			else
				OspPrintf( TRUE, FALSE, "[Int EqpSsn]: Disconnect the Eqp%u error!\n" );
			return;
		}
		else
		{
			OspPrintf( TRUE, FALSE, "[Int EqpSsn]: The Eqp%u disconnected before!\n" );
			return ;
		}
	}
}
		
/*====================================================================
    函数名      ：DaemonInstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  , CApp * pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/29    1.0         
====================================================================*/
void CEqpSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp * pcApp )
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "EqpSsn: The received msg's pointer in the msg entry is NULL!" );
		return;
	}
	
	switch( pcMsg->event )
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		OspPrintf( TRUE, FALSE, "EqpSsn: Wrong message %u(%s) received!\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ) );
		break;
	}
}


/*====================================================================
    函数名      ：ProcEqpMcuRegMsg
    功能        ：处理外设的登记请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/21    1.0         LI Yi         创建
====================================================================*/
void CEqpSsnInst::ProcEqpMcuRegMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tRegEqp;

    u8  byEqpId;   //MCU 设备列表中的ID
    u8  byEqpType; //MCU 设备列表中的类型
    u32 dwEqpIP;   //MCU 设备列表中的IP

    u32 dwEqpRcvIP;
    u16 wEqpRcvPort;

    TPeriEqpRegAck tRegAck;
    TPrsTimeSpan tPrsTimeSpan;

    memcpy(&tRegEqp, cServMsg.GetMsgBody(), min(sizeof(TPeriEqpRegReq), cServMsg.GetMsgBodyLen()));

//    EqpLog("[EqpSsn]: Rcv Msg %u(%s) from PeriEqp.%u Type.%u, InsId.%u, Ver.%d\n", 
//                  pcMsg->event, ::OspEventDesc(pcMsg->event),
//                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), GetInsID(), tRegEqp.GetVersion());

    dwEqpRcvIP  = tRegEqp.GetPeriEqpIpAddr();
    wEqpRcvPort = tRegEqp.GetStartPort();

    byEqpId  = tRegEqp.GetEqpId();
    m_dwEqpIpAddr = dwEqpRcvIP;

    EqpLog("EqpSsn: Eqp.%u Type%u EqpIp.%s:%u reg request received!\n", 
                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(),
                  StrOfIP(tRegEqp.GetPeriEqpIpAddr()), tRegEqp.GetStartPort());

    u16 wRet = 0;
    dwEqpIP = 0;
    if (byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX && tRegEqp.IsHDEqp())
    {
        EqpLog("EqpSsn: GetPeriInfo Eqp.%u Type%u EqpIp.%s reg as HDVMP!\n", 
                tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), StrOfIP(dwEqpIP) );
    }
    else
    {
        //检测设备列表中是否有该注册外设ID
        wRet = g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIP, &byEqpType);
        if (SUCCESS_AGENT != wRet)
        {
            EqpLog("EqpSsn: Reg Eqp.%d is invalid Eqp id ,Nack it, ret:%d\n", tRegEqp.GetEqpId(), wRet);

            //Nack 
            cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
            post(pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(),
                 cServMsg.GetServMsgLen(), pcMsg->srcnode);

            ::OspDisconnectTcpNode(pcMsg->srcnode);
            return;
        }
        else
        {
            EqpLog("EqpSsn: GetPeriInfo Eqp.%u Type%u EqpIp.%s reg info!\n", 
                          tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), StrOfIP(dwEqpIP) );
        }
    }


    //对Eqp的Id进行判断
    if (byEqpId != (u8)GetInsID())
    {
        //NACK
        OspPrintf(TRUE, FALSE, "EqpId = % , GetInsId() = %d,not equal to InsId, nack!\n", byEqpId, GetInsID());
        cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
        post(pcMsg->srcid, pcMsg->event + 2,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
        return;
    }

    
    switch(CurState())
    {
    case STATE_IDLE:
    //zbq[12/11/2007]状态机严格翻转
    //case STATE_NORMAL://?重复注册
        {
        if (byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX && tRegEqp.IsHDEqp())
        {
        }
        else
        {
            //检测合法性
            if (LOCAL_MCUID != tRegEqp.GetMcuId() ||
                byEqpType  != tRegEqp.GetEqpType() ||
                dwEqpRcvIP != dwEqpIP)
            {
                OspPrintf(TRUE, FALSE, "EqpSsn: MCU%uEQP%uType%u reg and was refused!\n",
                    tRegEqp.GetMcuId(),
                    tRegEqp.GetEqpId(),
                    tRegEqp.GetEqpType());

                if ( dwEqpRcvIP != dwEqpIP )
                {
					OspPrintf(TRUE, FALSE, "EqpSsn: dwEqpRcvIp(0x%x) is not equal to dwEqpIp(0x%x)\n",
						dwEqpRcvIP, dwEqpIP);
                }
                
                cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
                post(pcMsg->srcid, pcMsg->event + 2,
                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
                
                //断链
                if (m_dwEqpIpAddr != g_cMcuAgent.GetMpcIp())
                {
                    ::OspDisconnectTcpNode(pcMsg->srcnode);
                }				
				
                return;
            }
        }

        u32 dwRcvIP = INADDR_NONE;
        u16 wRcvPort = 0;
        BOOL32 bGetSwitchSuccess = TRUE;
        // MCU 要求的外设版本号
        u16 wEqpVersion = 0;

        //获取交换地址和版本号
        switch(tRegEqp.GetEqpType())
        {
        //录像机	
        case EQP_TYPE_RECORDER:
            if (!g_cMpManager.GetRecorderSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                OspPrintf(TRUE, FALSE, "EqpSsn: Get Rec.%d Switch addr failed!\n", byEqpId);
            }
            wEqpVersion = DEVVER_RECORDER;
            break;

        //混音器
        case EQP_TYPE_MIXER:
            if (!g_cMpManager.GetMixerSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                OspPrintf(TRUE, FALSE, "EqpSsn: Get Mix.%d Switch addr failed!\n", byEqpId);
            }
            wEqpVersion = DEVVER_MIXER;
            break;

        //码流适配器
        case EQP_TYPE_BAS:
            if (!g_cMpManager.GetBasSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                OspPrintf(TRUE, FALSE, "EqpSsn: Get BAS.%d Switch addr failed!\n", byEqpId);
            }              
			// xliang [1/8/2009] BAS Develop Version defers:
			if(g_cMcuAgent.IsMpuBas(byEqpId) || g_cMcuAgent.IsEBap(byEqpId))
			{
				wEqpVersion = DEVVER_MPU;
			}
            else
			{
				wEqpVersion = g_cMcuAgent.IsEqpBasHD( byEqpId ) ? DEVVER_HDBAS : DEVVER_BAS;
            }
            break;

        //画面合成器
        case EQP_TYPE_VMP:
            if (!g_cMpManager.GetVmpSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                OspPrintf(TRUE, FALSE, "EqpSsn: Get VMP.%d Switch addr failed!\n", byEqpId);
            }
			// xliang [1/8/2009] VMP Develop Version defers:
			if( g_cMcuAgent.IsSVmp(byEqpId) 
				|| g_cMcuAgent.IsDVmp(byEqpId) 
				|| g_cMcuAgent.IsEVpu(byEqpId) )
			{
				wEqpVersion = DEVVER_MPU;
			}
			else
			{
				wEqpVersion = DEVVER_VMP;
			}	
            break;

        //丢包重传
        case EQP_TYPE_PRS:
            if (!g_cMpManager.GetPrsSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                OspPrintf( TRUE, FALSE, "EqpSsn: Get Prs.%d Switch addr failed!\n", byEqpId);
            }
            wEqpVersion = DEVVER_PRS;
            break;
        //电视墙
        case EQP_TYPE_TVWALL:
            wEqpVersion = DEVVER_TW;    
            break;
        //多画面电视墙
        case EQP_TYPE_VMPTW:
            wEqpVersion = DEVVER_MPW;    
            break;
		//高清电视墙
		case EQP_TYPE_HDU://HDU // xliang [12/31/2008] 
			wEqpVersion = DEVVER_HDU;
			break;

        default:
            break;
        }

        //获取交换地址失败，拒绝注册
        if (!bGetSwitchSuccess)
        {
            cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
            post(pcMsg->srcid, pcMsg->event + 2,
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

            //断链
            if (m_dwEqpIpAddr != g_cMcuAgent.GetMpcIp())
            {
                ::OspDisconnectTcpNode(pcMsg->srcnode);
            }
            return;
        }

        // 版本验证失败，拒绝注册
        if ( tRegEqp.GetVersion() != wEqpVersion )
        {
            cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
            post(pcMsg->srcid, pcMsg->event + 2,
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

            OspPrintf(TRUE, FALSE, "EqpSsn: Eqp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
                      GetInsID(),
                      tRegEqp.GetPeriEqpIpAddr(),
                      tRegEqp.GetVersion(),
                      wEqpVersion );


            // 非内置，断链
            if (m_dwEqpIpAddr != g_cMcuAgent.GetMpcIp())
            {
                ::OspDisconnectTcpNode(pcMsg->srcnode);
            }
            return;
        }

        //ACK
        tRegAck.SetMcuIpAddr(dwRcvIP);
        tRegAck.SetMcuStartPort(wRcvPort);
        if (g_cMSSsnApp.IsDoubleLink())
        {
            tRegAck.SetAnotherMpcIp(g_cMSSsnApp.GetAnotherMcuIP());
        }
        else
        {
            tRegAck.SetAnotherMpcIp(0);
        }
		u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
        EqpLog("[Ssn: ProcEqpMcuRegMsg] Reg Ack with System SSRC.%u.\n", dwSysSSrc);
        tRegAck.SetMSSsrc(dwSysSSrc);

        
        GetPrsTimeSpan(&tPrsTimeSpan);
        tPrsTimeSpan.m_wFirstTimeSpan = htons(tPrsTimeSpan.m_wFirstTimeSpan);
        tPrsTimeSpan.m_wSecondTimeSpan = htons(tPrsTimeSpan.m_wSecondTimeSpan);
        tPrsTimeSpan.m_wThirdTimeSpan = htons(tPrsTimeSpan.m_wThirdTimeSpan);
        tPrsTimeSpan.m_wRejectTimeSpan = htons(tPrsTimeSpan.m_wRejectTimeSpan);
        cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
        cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));

        // 如果是录像机，通知MCU别名
        if ( tRegEqp.GetEqpType() == EQP_TYPE_RECORDER )
        {
            s8 szMcuAlias[32] = {0};
            g_cMcuAgent.GetMcuAlias( szMcuAlias, 32 );
            szMcuAlias[31] = '\0';
            cServMsg.CatMsgBody( (u8*)szMcuAlias, 32 );
        }
        
		// 增加通知MTU的长度, zgc, 2007-04-02
		if( tRegEqp.GetEqpType() == EQP_TYPE_VMP
		 || tRegEqp.GetEqpType() == EQP_TYPE_BAS
		 || tRegEqp.GetEqpType() == EQP_TYPE_MIXER)
		{
			TNetWorkInfo tTempInfo;
			g_cMcuAgent.GetNetWorkInfo( &tTempInfo );
			u16 wMTUSize = tTempInfo.GetMTUSize();
			wMTUSize = htons(wMTUSize);
			cServMsg.CatMsgBody( (u8*)&wMTUSize, sizeof(wMTUSize) );
		}
		// 增加通知是否允许冗余发送, zgc, 2007-07-25
		if ( tRegEqp.GetEqpType() == EQP_TYPE_MIXER )
		{
			TEqpMixerInfo tTempMixerInfo;
			g_cMcuAgent.GetEqpMixerCfg( byEqpId, &tTempMixerInfo );
			u8 byIsSendRedundancy = tTempMixerInfo.IsSendRedundancy() ? 1 : 0;
			cServMsg.CatMsgBody( (u8*)&byIsSendRedundancy, sizeof(byIsSendRedundancy) );
		}

        // BAS-HD 接收和发送数据起始端口、别名等信息。一方面同步到注册信息中,另一方面通知外设。guzh 2008-08-05
        if( tRegEqp.GetEqpType() == EQP_TYPE_BAS &&
            tRegEqp.IsHDEqp() )
        {
            TEqpBasHDInfo tInfo;
            TEqpBasHDCfgInfo tCfgInfo;
            g_cMcuAgent.GetEqpBasHDCfg(tRegEqp.GetEqpId(), &tInfo);
            tCfgInfo.SetAlias(tInfo.GetAlias());
            tCfgInfo.SetEqpStartPort(tInfo.GetEqpRecvPort());
            tCfgInfo.SetType(tInfo.GetHDBasType());

            tRegEqp.SetEqpAlias( (s8*)tCfgInfo.GetAlias() );
            tRegEqp.SetStartPort( tCfgInfo.GetEqpStartPort() );
            cServMsg.CatMsgBody((u8*)&tCfgInfo, sizeof(tCfgInfo));
        }
        else if (tRegEqp.GetEqpType() == EQP_TYPE_VMP &&
                 tRegEqp.IsHDEqp())
        {
            TEqpBasHDCfgInfo tCfgInfo;
            tCfgInfo.SetAlias("vmphd");
            tCfgInfo.SetEqpStartPort(VMP_EQP_STARTPORT);
            cServMsg.CatMsgBody((u8*)&tCfgInfo, sizeof(tCfgInfo));
            g_cMcuVcApp.SetPeriEqpIsValid( tRegEqp.GetEqpId() );
        }
		
        if (tRegEqp.GetEqpType() == EQP_TYPE_HDU)
        {
			TEqpHduInfo tInfo;
			TEqpHduCfgInfo tCfgInfo;
			g_cMcuAgent.GetEqpHduCfg(tRegEqp.GetEqpId(), &tInfo);
			tCfgInfo.SetAlias( tInfo.GetAlias() );
			tCfgInfo.SetEqpStartPort( tInfo.GetEqpRecvPort() );
            THduChnlModePortAgt tHduChnModePortAgt;
			THduChnlModePort tHduChnModePort;
			memset(&tHduChnModePortAgt, 0x0, sizeof(tHduChnModePortAgt));
			memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
            tInfo.GetHduChnlModePort(0, tHduChnModePortAgt);
            tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
			tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
			tHduChnModePort.SetZoomRate(tHduChnModePortAgt.GetZoomRate());
			tCfgInfo.SetHduChnlModePort(0, tHduChnModePort);
            tInfo.GetHduChnlModePort(1, tHduChnModePortAgt);
            tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
			tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
			tHduChnModePort.SetZoomRate( tHduChnModePortAgt.GetZoomRate() );
			tCfgInfo.SetHduChnlModePort(1, tHduChnModePort);

            tRegEqp.SetEqpAlias( (s8*)tCfgInfo.GetAlias() );
            tRegEqp.SetStartPort( tCfgInfo.GetEqpStartPort() );
            cServMsg.CatMsgBody((u8*)&tCfgInfo, sizeof(tCfgInfo));

        }


        post(pcMsg->srcid, pcMsg->event + 1,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
                
        //save information
        m_dwEqpNode = pcMsg->srcnode;
        m_dwEqpIId  = pcMsg->srcid;
        m_byEqpId   = byEqpId;
        m_byEqpType = tRegEqp.GetEqpType();
        memcpy(m_achAlias, tRegEqp.GetEqpAlias(), MAXLEN_EQP_ALIAS);
        m_achAlias[MAXLEN_EQP_ALIAS-1] = 0;

        // set qos info        
        TQosInfo tAgentQos;
        wRet = g_cMcuAgent.GetQosInfo(&tAgentQos);
        if(SUCCESS_AGENT == wRet)
        {
            TMcuQosCfgInfo tMcuQosCfg;
            tMcuQosCfg.SetQosType(tAgentQos.GetQosType());
            tMcuQosCfg.SetAudLevel(tAgentQos.GetAudioLevel());
            tMcuQosCfg.SetVidLevel(tAgentQos.GetVideoLevel());
            tMcuQosCfg.SetDataLevel(tAgentQos.GetDataLevel());
            tMcuQosCfg.SetSignalLevel(tAgentQos.GetSignalLevel());
            tMcuQosCfg.SetIpServiceType(tAgentQos.GetIpServiceType());
            
            CServMsg cQosMsg;
            cQosMsg.SetEventId(MCU_EQP_SETQOS_CMD);
            cQosMsg.SetMsgBody((u8*)&tMcuQosCfg, sizeof(tMcuQosCfg));            
            ProcMcuEqpSetQos(cQosMsg);            
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[ProcEqpMcuRegMsg] get qos info failed, ret:%d!\n", wRet);
        }             
                
        //注册断链接收实例
        ::OspNodeDiscCBRegQ(pcMsg->srcnode, GetAppID(), GetInsID());

        //通知 VC daemon
        cServMsg.SetMsgBody((u8*)&tRegEqp, sizeof(tRegEqp));
        g_cMcuVcApp.SendMsgToDaemonConf(MCU_EQPCONNECTED_NOTIF, 
                                        cServMsg.GetServMsg(),
                                        cServMsg.GetServMsgLen());

        NEXTSTATE(STATE_NORMAL);

        break;
        }

    default:
        OspPrintf(TRUE, FALSE, "EqpSsn: [ProcEqpMcuRegMsg]Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}

/*====================================================================
    函数名      ：ProcEqpDisconnect
    功能        ：外设断链处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/21    1.0         LI Yi        创建    
====================================================================*/
void CEqpSsnInst::ProcEqpDisconnect( const CMessage * pcMsg )
{
    EqpLog( "EqpSsn: Rcv Msg %u(%s) ! Eqp%u.Type%u disconnected!\n", 
        pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byEqpId, m_byEqpType );
    
	CServMsg	cServMsg;
	TMt	tEqp;
	TPeriEqpStatus	tEqpStatus;

	switch( CurState() )
	{
	case STATE_NORMAL:
		if( *( u32 * )pcMsg->content == m_dwEqpNode )	//本实例对应连接断
		{		
            if (INVALID_NODE != m_dwEqpNode)
            {
                OspDisconnectTcpNode(m_dwEqpNode);
            }
            
			//send notification to VC Daemon
			tEqp.SetMcuEqp( LOCAL_MCUID, m_byEqpId, m_byEqpType );
			cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );
			g_cMcuVcApp.SendMsgToDaemonConf( MCU_EQPDISCONNECTED_NOTIF, cServMsg.GetServMsg(), 
				cServMsg.GetServMsgLen() );

			//clear
			DeleteAlias();
			m_dwEqpNode = INVALID_NODE;
			m_byEqpId   = 0;
			m_byEqpType = 0;

			NEXTSTATE( STATE_IDLE );
		}
		break;
	default:
		OspPrintf( TRUE, FALSE, "CEqpInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcEqpToMcuMsg
    功能        ：处理外设到MCU的一般消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/19    1.0         薛新文        创建
    02/08/25    1.0         LI Yi         修改
====================================================================*/
void CEqpSsnInst::ProcEqpToMcuMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg;

//    EqpLog("[Eqpssn]: Recv %u(%s) from eqp.%d!\n",
//                  pcMsg->event, ::OspEventDesc(pcMsg->event), m_byEqpId);

	switch( CurState() )
	{
	case STATE_NORMAL:
		cServMsg.SetServMsg( pcMsg->content, pcMsg->length );
		if(PRS_MCU_SETSRC_ACK == pcMsg->event)
		{
			cServMsg.SetMsgBody(&m_byEqpId, sizeof(m_byEqpId));
		}
		if( !cServMsg.GetConfId().IsNull() )
		{
			g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfId(), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}	
		break;
	default:
		OspPrintf( TRUE, FALSE, "EqpSsn: [ProcEqpToMcuMsg]Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcuToEqpMsg
    功能        ：处理业务对外设的一般命令
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/19    1.0         薛新文        创建
====================================================================*/
void CEqpSsnInst::ProcMcuToEqpMsg(const CMessage *pcMsg)
{
    EqpLog("[Eqpssn]: Send %u(%s) to eqp.%d!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), m_byEqpId);

	switch( CurState() )
	{
	case STATE_NORMAL:
		SendMsgToPeriEqp( pcMsg->event, pcMsg->content, pcMsg->length );
		break;
	default:

		OspPrintf( TRUE, FALSE, "EqpSsn:[ProcMcuToEqpMsg] Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );

		break;
	}
}


/*====================================================================
    函数名      ：ProcEqpToMcuDaemonConfMsg
    功能        ：处理外设到MCU Daemon实例的一般消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/10    1.0         LI Yi         创建
====================================================================*/
void CEqpSsnInst::ProcEqpToMcuDaemonConfMsg( const CMessage * pcMsg )
{
	switch( CurState() )
	{
	case STATE_NORMAL:
		g_cMcuVcApp.SendMsgToDaemonConf( pcMsg->event, pcMsg->content, pcMsg->length );
		break;
	default:
		OspPrintf( TRUE, FALSE, "EqpSsn: [ProcEqpToMcuDaemonConfMsg]Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：SendMsgToPeriEqp
    功能        ：发消息给本实例对应的外设
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若该外设未登记返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/20    1.0         LI Yi         创建
====================================================================*/
BOOL32 CEqpSsnInst::SendMsgToPeriEqp( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if( OspIsValidTcpNode( m_dwEqpNode ) || m_dwEqpIpAddr == g_cMcuAgent.GetMpcIp() )
	{
		post( m_dwEqpIId, wEvent, pbyMsg, wLen, m_dwEqpNode );
		return( TRUE );
	}
	else
	{
		log( LOGLVL_IMPORTANT, "CEqpSsnInst: PeriEqp%u is offline now: Node=%u, CurState=%u, InstId=%u\n", 
			m_byEqpId, m_dwEqpNode, CurState(), GetInsID() );
		return( FALSE );
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
void CEqpSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEqpConfig::CEqpConfig()
{

}

CEqpConfig::~CEqpConfig()
{
}

/*====================================================================
    函数名      ：SendMsgToPeriEqpSsn
    功能        ：发消息给指定的外设对应的会话实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byPeriEqpId, 指定的外设号
				  u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若该外设未登记返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/19    1.0         薛新文        创建
====================================================================*/
BOOL32 CEqpConfig::SendMsgToPeriEqpSsn( u8 byPeriEqpId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if (byPeriEqpId >= MAXNUM_MCU_PERIEQP || 0 == byPeriEqpId)
	{
#ifdef _DEBUG
		::OspLog( LOGLVL_IMPORTANT, "CEqpConfig: PeriEqp%u invalid, not exist!\n", byPeriEqpId );
#endif
		return FALSE;
	}
	else
	{
		//本端主用则允许向外部各App投递消息，否则丢弃
		if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
		{
			return TRUE;
		}

		::OspPost( MAKEIID( AID_MCU_PERIEQPSSN, byPeriEqpId), wEvent, pbyMsg, wLen );
		return TRUE;
	}
}

/*====================================================================
    函数名      ：BroadcastToAllPeriEqpSsn
    功能        ：发消息给所有已经登记连接的外设对应的会话实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/19    1.0         薛新文         创建
====================================================================*/
void CEqpConfig::BroadcastToAllPeriEqpSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端主用则允许向外部各App投递消息，否则丢弃
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

	::OspPost(MAKEIID( AID_MCU_PERIEQPSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);

	return;
}


//END OF FILE
