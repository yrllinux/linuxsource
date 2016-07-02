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
#include "mcuver.h"

/*lint -save -esym(641, emMcuErrorCode)*/

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
		{
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_byEqpId,byMixerSubtype);
			switch (byMixerSubtype)
			{
			case APU_MIXER:
				strncpy( achType, "MIXER_APU", sizeof(achType)  );
				break;
			case EAPU_MIXER:
				strncpy( achType, "MIXER_EAPU", sizeof(achType)  );
				break;
			case APU2_MIXER:
				strncpy( achType, "MIXER_APU2", sizeof(achType)  );
				break; 
			case MIXER_8KG:
				strncpy( achType, "MIXER_8KG", sizeof(achType)  );
				break;
			case MIXER_8KH:
				strncpy( achType, "MIXER_8KH", sizeof(achType)  );
				break;
			case MIXER_8KI:
				strncpy( achType, "MIXER_8KI", sizeof(achType)  );
				break; 
			default:
				strncpy( achType, "MIXER_UNKNOWN", sizeof(achType)  );
				break;
			}
		}
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
		{
			u8 byHduSubtype = HDU_SUBTYPE_HDU_M;
			if(g_cMcuAgent.GetHduSubTypeByEqpId(m_byEqpId,byHduSubtype))
			{
				switch (byHduSubtype)
				{
				case HDU_SUBTYPE_HDU_M:
					strncpy( achType, "HDU", sizeof(achType)  );
					break; 
				case HDU_SUBTYPE_HDU_H:
					strncpy( achType, "HDU_H", sizeof(achType)  );
					break; 
				case HDU_SUBTYPE_HDU_L:
					strncpy( achType, "HDU_L", sizeof(achType)  );
					break;
				case HDU_SUBTYPE_HDU2:
					strncpy( achType, "HDU2", sizeof(achType)  );
					break;
				case HDU_SUBTYPE_HDU2_L:
					strncpy( achType, "HDU2_L", sizeof(achType)  );
					break;
				case HDU_SUBTYPE_HDU2_S:
					strncpy( achType, "HDU2_S", sizeof(achType)  );
					break;
				default:
					strncpy( achType, "N/A", sizeof(achType)  );
					break;
				}
			}
			else
			{
				strncpy( achType, "N/A", sizeof(achType)  );
			}

		}
		break;
	default:
		strncpy( achType, "N/A", sizeof(achType)  );
		break;
	}
    
	StaticLog("%3u %5u %5u %6s   %s param:%u\n", GetInsID(), m_byEqpId, m_dwEqpNode, achType,m_achAlias,param);
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

#define EQP_EV_MSG(level)	MCU_EV_LOG(level, MID_MCU_EQP, "Eqp")

void CEqpSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[EqpSsn]: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch( pcMsg->event )
	{
		// [2/25/2010 xliang] 8ke eqp reg
/*	case MIXER_MCU_REG_REQ:				//混音器登记请求
#if defined(_8KE_) || defined(_8KH_)  || defined(_8KI_)
	#ifdef _8KI_
			Proc8KiMixerMcuRegMsg( pcMsg );
	#else
			Proc8KEEqpMcuRegMsg( pcMsg );
	#endif                        		
#else
		ProcEqpMcuRegMsg( pcMsg );
#endif

		break;*/
	case MIXER_MCU_REG_REQ:				//混音器登记请求
	case VMP_MCU_REGISTER_REQ:          //画面合成器登记请求
	case BAS_MCU_REG_REQ:				//适配器登记请求
		//[nizhijun 2011/02/17] prs 8ke外置化
	case PRS_MCU_REGISTER_REQ:			//PRS向MCU注册请求
		{
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			Proc8KEEqpMcuRegMsg( pcMsg );
#else
			ProcEqpMcuRegMsg( pcMsg );
#endif
			break;
		}

	case TVWALL_MCU_REG_REQ:			//电视墙登记请求
	case REC_MCU_REG_REQ:				//录像机登记请求
	case VMPTW_MCU_REGISTER_REQ:        //复合电视墙登记请求
    case HDU_MCU_REG_REQ:               //HDU注册登记
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		ProcEqpMcuRegMsg( pcMsg );
		break;

	case OSP_DISCONNECT:				//控制台断链通知
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		ProcEqpDisconnect( pcMsg );
		break;

	//MCU到外设的消息
	case MCU_TVWALL_START_PLAY_REQ:	    //外设开始播放通知
	case MCU_TVWALL_STOP_PLAY_REQ:		//外设通知播放通知

    case MCU_HDU_START_PLAY_REQ:        //HDU播放
    case MCU_HDU_STOP_PLAY_REQ:         //HDU停止播放
	case MCU_HDU_CHGHDUVMPMODE_REQ:		//切换HDU通道风格
	case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD://刷新音频载荷

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
/*	case MCU_REC_ADJUST_BACKRTCP_CMD:    //调整录像机backrtcp[liu lijiu][20100727]*/
	
	case MCU_MIXER_STARTMIX_REQ:        //MCU开始混音请求
	case MCU_MIXER_STOPMIX_REQ:         //MCU停止混音请求
	//tianzhiyong 2010/05/19 添加语音激励开启与停止命令（供EAPU混音器用）
	case MCU_MIXER_STARTVAC_CMD:        //MCU开始语音激励
	case MCU_MIXER_STOPVAC_CMD:         //MCU停止语音激励
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
	case MCU_BAS_CHANGEAUDPARAM_REQ:    //音频开启改变参数请求
    case MCU_BAS_FASTUPDATEPIC_CMD:     //
    case MCU_HDBAS_STARTADAPT_REQ:      //启动高清适配请求
    case MCU_HDBAS_STOPADAPT_REQ:       //停止高清适配请求
    case MCU_HDBAS_SETADAPTPARAM_CMD:   //改变高清适配参数命令

	case MCU_VMP_STARTVIDMIX_REQ:       //MCU请求VMP开始工作
	case MCU_VMP_STOPVIDMIX_REQ:        //MCU请求VMP停止工作
	case MCU_VMP_CHANGEVIDMIXPARAM_CMD: //MCU请求VMP改变复合参数
	case MCU_VMP_GETVIDMIXPARAM_REQ:    //MCU请求VMP回送复合参数
	case MCU_VMP_SETCHANNELBITRATE_REQ: //改变画面合成的码率
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
    case MCU_VMP_FASTUPDATEPIC_CMD:
	case MCU_VMP_CHANGEMEMALIAS_CMD:
	case MCU_VMP_ADDREMOVECHNNL_CMD:
        
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
	case MCU_HDU_CHANGEPLAYPOLICY_NOTIF://通道空闲显示策略修改
    case MCU_HDU_CHANGEVOLUME_CMD:      // 设置音量或静音

	case MCU_EQP_MODSENDADDR_CMD:
		
	case MCU_VMP_STARTSTOPCHNNL_CMD:	//MCU通知VMP停或开某一输出通道命令
	case MCU_HDU_CHANGEMODE_CMD:
	case MCU_REC_SETRTCP_NFY:
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
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
	case BAS_MCU_CHANGEAUDPARAM_ACK:    //开启改变参数应答
	case BAS_MCU_CHANGEAUDPARAM_NACK:    //开启改变参数拒绝
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
	case HDU_MCU_START_PLAY_ACK:        //hdu播放应答
	case HDU_MCU_START_PLAY_NACK:       //hdu播放拒绝
	case HDU_MCU_STOP_PLAY_ACK:         //停止播放应答
    case HDU_MCU_STOP_PLAY_NACK:        //停止播放应答
	case HDU_MCU_CHGHDUVMPMODE_ACK:		//切换HDU通道应答
	case HDU_MCU_CHGHDUVMPMODE_NACK:	//切换HDU通道应答
		EQP_EV_MSG(LOG_LVL_DETAIL);
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
	case AUDBAS_MCU_CHNNLSTATUS_NOTIF:
		EQP_EV_MSG(LOG_LVL_DETAIL);
		ProcEqpToMcuDaemonConfMsg( pcMsg );
		break;

	case TVWALL_MCU_START_PLAY_ACK:
	case TVWALL_MCU_START_PLAY_NACK:
	case TVWALL_MCU_STOP_PLAY_ACK:
	case TVWALL_MCU_STOP_PLAY_NACK:
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		break;
	
    //设置qos值
    case MCU_EQP_SETQOS_CMD:
        {
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
            CServMsg cMsg(pcMsg->content, pcMsg->length);                    
            ProcMcuEqpSetQos(cMsg);
        }        
        break;
	// 取主备倒换状态
	case EQP_MCU_GETMSSTATUS_REQ:
    case MCU_MSSTATE_EXCHANGE_NTF:
		{
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
			ProcEqpGetMsStatusReq(pcMsg);
		}
		break;
    //设置画面合成风格
    case MCU_VMP_SETSTYLE_CMD:
        {
			EQP_EV_MSG(LOG_LVL_KEYSTATUS);
            if(EQP_TYPE_VMP == m_byEqpType)
            {
                SendMsgToPeriEqp(pcMsg->event, pcMsg->content, pcMsg->length);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[EqpSsn] send vmp style msg to eqp<%d> success\n", m_byEqpId);
            }
        }
        break;

	case TEST_MCU_DISCONNECTEQP_CMD:
		EQP_EV_MSG(LOG_LVL_KEYSTATUS);
		ProcMcuDisconnectEqp(pcMsg);
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[EqpSsn]: Wrong message %u(%s) received! Inst.%d\n", pcMsg->event, 
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
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMcuMtSetQos] send qos msg to eqp<%d> success\n", m_byEqpId);
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
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[ProcEqpGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
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

        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcEqpGetMsStatusReq] IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
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
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Int EqpSsn]: The Eqp%u disconnected!\n" );	
			else
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Int EqpSsn]: Disconnect the Eqp%u error!\n" );
			return;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Int EqpSsn]: The Eqp%u disconnected before!\n" );
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
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: The received msg's pointer in the msg entry is NULL!" );
		return;
	}

	if(pcMsg->event != MCU_APPTASKTEST_REQ)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Eqpssn Daem]: %u(%s) in\n", pcMsg->event, ::OspEventDesc( pcMsg->event ));
	}
	
	switch( pcMsg->event )
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;

		// [2/25/2010 xliang] 8ke 外设注册先走daemon,其他外设注册不走daemon,直接进普通实例
	case MIXER_MCU_REG_REQ:				//混音器登记请求
#if defined(_8KE_) || defined(_8KH_)|| defined(_8KI_)//保护起见，还是加了编译宏
        DaemonProc8KMixerMcuRegMsg( pcMsg );
#endif
		break;
    case BAS_MCU_REG_REQ:				//适配器登记请求
    case VMP_MCU_REGISTER_REQ:          //画面合成器登记请求
		//[nizhijun 2011/02/17]prs 8ke外置化
	case PRS_MCU_REGISTER_REQ:			//PRS向MCU注册请求
#if defined(_8KE_) || defined(_8KH_)|| defined(_8KI_)//保护起见，还是加了编译宏
        DaemonProcEqpMcuRegMsg( pcMsg );
#endif
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Wrong message %u(%s) received! AppId.%d\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ),pcApp!=NULL?pcApp->appId:0 );
		break;
	}
}

/*=============================================================================
函 数 名： IsEqpReged
功    能： 
算法实现： 
全局变量： 
参    数： const u8 &byEqpType
           const u32 &dwEqpIp
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/2/16   4.0		    周广程                  创建
=============================================================================*/
BOOL32 CEqpSsnInst::IsEqpReged( const u8 &byEqpType, const u32 &dwEqpIp )
{
    u8 byMinId = 0;
    u8 byMaxId = 0;

    if( EQP_TYPE_MIXER == byEqpType)
    {
        byMinId = MIXERID_MIN;
        byMaxId = MIXERID_MAX;       
    }

    else if( EQP_TYPE_BAS == byEqpType )
    {
        byMinId = BASID_MIN;
        byMaxId = BASID_MAX; 
    }
    
    else  if( EQP_TYPE_VMP == byEqpType )
    {
        byMinId = VMPID_MIN;
        byMaxId = VMPID_MAX; 
    }

    else if( EQP_TYPE_PRS == byEqpType )
    {
        byMinId = PRSID_MIN;
        byMaxId = PRSID_MAX; 
    }

    else
    {
        return FALSE;
    }
    
    u8 byInstID = 0;
    CEqpSsnInst * pcEqpInst = NULL;
    CApp *pcApp = &g_cEqpSsnApp;
    for (byInstID = byMinId; byInstID <= byMaxId; byInstID++)
    {
        pcEqpInst = (CEqpSsnInst *)pcApp->GetInstance(byInstID);
        if (NULL != pcEqpInst)
        {
            if( STATE_NORMAL == pcEqpInst->CurState() &&
                pcEqpInst->m_dwEqpIpAddr == dwEqpIp )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*=============================================================================
函 数 名： GetIdleInst
功    能： 
算法实现： 
全局变量： 
参    数： const u8 &byEqpType
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/2/16   4.0		    周广程                  创建
=============================================================================*/
u8 CEqpSsnInst::GetIdleInst( const u8 &byEqpType,u8 byIsAudBas )
{   
    u8 byMinId = 0;
    u8 byMaxId = 0;

    if( EQP_TYPE_MIXER == byEqpType )
    {
        byMinId = MIXERID_MIN;
        byMaxId = MIXERID_MAX;       
    }
    
    if( EQP_TYPE_BAS == byEqpType )
    {
        byMinId = BASID_MIN;
        byMaxId = BASID_MAX; 
		if( 1 == byIsAudBas )
		{
			byMinId = BASID_MIN + 3;
		}
    }
    
    if( EQP_TYPE_VMP == byEqpType )
    {
        byMinId = VMPID_MIN;
        byMaxId = VMPID_MAX; 
    }
    
    if( EQP_TYPE_PRS == byEqpType )
    {
        byMinId = PRSID_MIN;
        byMaxId = PRSID_MAX; 
    }

    u8 byInstID = 0;
    CEqpSsnInst * pcEqpInst = NULL;
    CApp *pcApp = &g_cEqpSsnApp;
    for (byInstID = byMinId; byInstID <= byMaxId; byInstID++)
    {
        pcEqpInst = (CEqpSsnInst *)pcApp->GetInstance(byInstID);
        if (NULL != pcEqpInst)
        {
            if( STATE_IDLE == pcEqpInst->CurState() )
            {
				pcEqpInst->m_curState = STATE_NORMAL;//[03/01/2010] zjl 这里先将实例状态机置normal
                return byInstID;
            }
        }
    }
    return 0;
}


/*=============================================================================
函 数 名： DaemonProcEqpMcuRegMsg
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/2/16   4.0		周广程                  创建
=============================================================================*/
void CEqpSsnInst::DaemonProcEqpMcuRegMsg( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u32 dwEqpNetId = pcMsg->srcid;
    u32 dwEqpNetNode = pcMsg->srcnode;
	
    TEqpRegReq tRegEqp;    

	const u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
    memcpy(&tRegEqp, cServMsg.GetMsgBody(), min(sizeof(TEqpRegReq), wMsgBodyLen));
	
    u8  byEqpType = tRegEqp.GetEqpType(); 
    u32 dwEqpIP = 0;
    if ( 0 == dwEqpNetNode )
    {
        dwEqpIP = tRegEqp.GetEqpIpAddr();
        if (dwEqpIP == 0)
        {
            dwEqpIP = ntohl( g_cMcuAgent.GetMpcIp() );      
        }
        
        if (dwEqpIP == 0)
        {
            dwEqpIP = ntohl(INET_ADDR("127.0.0.1"));
        }
    }
    else
    {
        dwEqpIP = ntohl( OspNodeIpGet(dwEqpNetNode) );
    }
    
	tRegEqp.SetEqpIpAddr( dwEqpIP );
    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: [DaemonProcEqpMcuRegMsg] Eqp.<%s> reg! \n", StrOfIP( dwEqpIP) );

	/* [03/01/2010] 8000-e pc机外设都为同一ip,这里不做同一ip校验，
	                由GetIdleInst直接分配eqpid，并立即将状态机置为normal,保证唯一性

    if ( IsEqpReged( byEqpType, dwEqpIP ) )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: Reg Eqp.%s<Ip: 0x%x> has been reged ,Nack it\n", 
			tRegEqp.GetEqpAlias(), dwEqpIP );
        
        //Nack 
        cServMsg.SetErrorCode(ERR_MCU_PERIEQP_REGISTERED);
        post(pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(),
            cServMsg.GetServMsgLen(), pcMsg->srcnode);
        
        ::OspDisconnectTcpNode(pcMsg->srcnode);
        return;
    }
	*/

	u8 byIsAudBas = 0;
	if( EQP_TYPE_BAS == byEqpType && cServMsg.GetMsgBodyLen() > sizeof(TEqpRegReq) )
	{
		byIsAudBas = *(u8*)(cServMsg.GetMsgBody() + sizeof(TEqpRegReq) );
	}

    u8 byIdleInstId = GetIdleInst( byEqpType,byIsAudBas );
    if ( 0 == byIdleInstId )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: No idle ssn for Eqp.%s<Ip: 0x%x>, Nack it\n", 
            tRegEqp.GetEqpAlias(), dwEqpIP );
        
        //Nack 
        cServMsg.SetErrorCode(ERR_MCU_NOIDLEEQPSSN);
        post(pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(),
            cServMsg.GetServMsgLen(), pcMsg->srcnode);
        
        ::OspDisconnectTcpNode(pcMsg->srcnode);
        return;
    }
    tRegEqp.SetEqpId( byIdleInstId );

	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: [DaemonProcEqpMcuRegMsg] GetIdleInst.%d, set it to EqpId!\n", byIdleInstId);
	
    cServMsg.SetMsgBody( (u8*)&tRegEqp, sizeof(tRegEqp) );
    cServMsg.CatMsgBody( (u8*)&dwEqpNetId, sizeof(dwEqpNetId) );
    cServMsg.CatMsgBody( (u8*)&dwEqpNetNode, sizeof(dwEqpNetNode) );
    post( MAKEIID(AID_MCU_PERIEQPSSN, byIdleInstId), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    return;
}
/*=============================================================================
函 数 名： DaemonProc8KMixerMcuRegMsg
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/5/17   4.7		    田志勇                  创建
=============================================================================*/
void CEqpSsnInst::DaemonProc8KMixerMcuRegMsg( const CMessage * pcMsg )
{
	//8K系列混音器通过向DAEMON注册来获取混音器配置，以便外设进行相应初始化工作
	CServMsg cMixerCfgToEqp;
	TEqpMixerInfo tTempMixerInfo;
	u8 byMixerId = 0;
	u8 byMixMemNum = 0;
	char achAlias [ MAXLEN_ALIAS + 1 ] = {0};
	u16 wEqpRcvStartPort = 0;
	u8 byMixerIdx = 0;
	BOOL32 bIsHasMixer = FALSE;
	for(byMixerIdx = MIXERID_MIN;byMixerIdx <= MIXERID_MAX;byMixerIdx++)
	{
		if (SUCCESS_AGENT != g_cMcuAgent.GetEqpMixerCfg( byMixerIdx , &tTempMixerInfo ))
		{
			LogPrint( LOG_LVL_WARNING, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]GetEqpMixerCfg(%d) failed!\n", byMixerIdx);
			break;
		}
		if (tTempMixerInfo.GetMaxChnInGrp() == 0)
		{
			LogPrint( LOG_LVL_WARNING, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]GetEqpMixerCfg(%d) MixNum(0)!\n",byMixerIdx);
			continue;
		}
		bIsHasMixer = TRUE;
		byMixerId = tTempMixerInfo.GetEqpId();
		byMixMemNum = tTempMixerInfo.GetMaxChnInGrp();
		wEqpRcvStartPort = tTempMixerInfo.GetEqpRecvPort();
		strcpy(achAlias, tTempMixerInfo.GetAlias());
		LogPrint( LOG_LVL_DETAIL, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]GetEqpMixerCfg(%d) MixMemNum(%d) RcvStartPort(%d) Alias(%s)!\n", 
			byMixerId,byMixMemNum,wEqpRcvStartPort,achAlias);
		if (byMixerIdx == MIXERID_MIN) 
		{
			cMixerCfgToEqp.SetMsgBody( (u8*)&byMixerId, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&byMixMemNum, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&wEqpRcvStartPort, sizeof(u16) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&achAlias[0], sizeof(achAlias) );
		}
		else
		{
			cMixerCfgToEqp.CatMsgBody( (u8*)&byMixerId, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&byMixMemNum, sizeof(u8) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&wEqpRcvStartPort, sizeof(u16) );
			cMixerCfgToEqp.CatMsgBody( (u8*)&achAlias[0], sizeof(achAlias) );
		}
/*#ifndef _8KI_*/
		//对应实例状态要反转，以便混音器外设那边普通INSTANCE可以正常向MCU注册，而8KI混音器走普通流程，状态反转在普通INS注册时
		CEqpSsnInst * pcEqpInst = NULL;
		CApp *pcApp = &g_cEqpSsnApp;
		pcEqpInst = (CEqpSsnInst *)pcApp->GetInstance(byMixerId);
		if (NULL != pcEqpInst)
		{
			if( STATE_IDLE == pcEqpInst->CurState() )
			{
				pcEqpInst->m_curState = STATE_NORMAL;
			}
		}
		else
		{
			LogPrint( LOG_LVL_DETAIL, MID_MCU_MIXER,"[DaemonProc8KMixerMcuRegMsg]NULL != pcEqpInst EqpId(%d)!\n", byMixerId);
		}
/*#endif*/
	}
	if (bIsHasMixer)
	{
		post(pcMsg->srcid, pcMsg->event + 1, cMixerCfgToEqp.GetServMsg(),cMixerCfgToEqp.GetServMsgLen(), pcMsg->srcnode);
	}
	else
	{
		post(pcMsg->srcid, pcMsg->event + 2, cMixerCfgToEqp.GetServMsg(),cMixerCfgToEqp.GetServMsgLen(), pcMsg->srcnode);
	}
    return;
}

/*=============================================================================
函 数 名： Proc8KEEqpMcuRegMsg
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/12/22   		    薛亮                  创建
=============================================================================*/
void CEqpSsnInst::Proc8KEEqpMcuRegMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqpRegReq tRegEqp = *(TEqpRegReq *)cServMsg.GetMsgBody();

	//save information to member variable
    m_dwEqpIId = *(u32*)(cServMsg.GetMsgBody()+sizeof(TEqpRegReq));
	//8KG以及8KH混音器注册直接从外设侧普通INSTANCE发送过来，不再过DAEMON实例。
	if (MIXER_MCU_REG_REQ == pcMsg->event) 
	{
		m_dwEqpIId = pcMsg->srcid;
		m_dwEqpNode = pcMsg->srcnode;
	}
	else
	{
		m_dwEqpIId = *(u32*)(cServMsg.GetMsgBody()+sizeof(TEqpRegReq));
		m_dwEqpNode = *(u32*)(cServMsg.GetMsgBody()+sizeof(TEqpRegReq)+sizeof(u32));
	}
    m_dwEqpIpAddr  = tRegEqp.GetEqpIpAddr();
    m_byEqpType    = tRegEqp.GetEqpType();
    strncpy(m_achAlias, tRegEqp.GetEqpAlias(), sizeof(m_achAlias)-1);
    m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';

	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[EqpSsn]: Rcv Msg %u(%s) from PeriEqp.%s<0x%x Type%u> , InstId=%u, version %d\n", 
		pcMsg->event, ::OspEventDesc(pcMsg->event), tRegEqp.GetEqpAlias(),
                  tRegEqp.GetEqpIpAddr(), tRegEqp.GetEqpType(), GetInsID(), tRegEqp.GetVersion());
    
    switch(CurState())
    {
    case STATE_NORMAL://STATE_IDLE:
        {
            // 抢先修改状态，防止重入
            //NEXTSTATE(STATE_NORMAL);

            m_byEqpId = (u8)GetInsID();
#if defined(_8KH_) || defined(_8KI_)
            if (m_byEqpType == EQP_TYPE_MIXER && tRegEqp.GetEqpId() != m_byEqpId )
            {
                LogPrint(LOG_LVL_WARNING, MID_MCU_EQP,"[Proc8KEEqpMcuRegMsg] tRegEqp.GetEqpId()(%d) != m_byEqpId(%d)\n", 
					tRegEqp.GetEqpId(), m_byEqpId);
                NEXTSTATE(STATE_IDLE);
                return;
            }
#endif
            u32 dwMcuRecvIP = 0;
            u16 wMcuStartPort = 0;
            u16 wEqpStartPort = 0;
            g_cMpManager.GetSwitchInfo( m_byEqpId, dwMcuRecvIP, wMcuStartPort, wEqpStartPort );

			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KEEqpMcuRegMsg] GetSwitchInfo--wMcuStartPort: %d, wEqpStartPort: %d\n", 
				wMcuStartPort, wEqpStartPort);

            // 为了满足外设堆叠的需求，这里需要对127.0.0.1的本机地址进行转换，转换到对注册上的外设有意义的MCU IP
            if ( dwMcuRecvIP == ntohl( INET_ADDR("127.0.0.1") ) )
            {
                if ( 0 != m_dwEqpNode )
                {
                    dwMcuRecvIP = ntohl(OspNodeLocalIpGet(m_dwEqpNode));
                }
            }      
          
            //获取版本号 [12/17/2009 xliang] 不校验8000E外设版本号
			/*
			u16 wEqpVersion = 0;
            switch(tRegEqp.GetEqpType())
            {
            //混音器
            case EQP_TYPE_MIXER:
                wEqpVersion = DEVVER_MIXER;
                break;

            //码流适配器
            case EQP_TYPE_BAS:
                wEqpVersion = DEVVER_BAS;
                break;

            //画面合成器
            case EQP_TYPE_VMP:
                wEqpVersion = DEVVER_VMP;
                break;

            //丢包重传
            case EQP_TYPE_PRS:
                wEqpVersion = DEVVER_PRS;
                break;

            default:
                break;
            }

 
			// 版本验证失败，拒绝注册
        
			if ( tRegEqp.GetVersion() != 0 &&
				 tRegEqp.GetVersion() != wEqpVersion )
			{
				cServMsg.SetErrorCode(ERR_MCU_EQPVER);
				post(pcMsg->srcid, pcMsg->event + 2,
					 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Eqp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
						  m_byEqpId,
						  tRegEqp.GetEqpIpAddr(),
						  tRegEqp.GetVersion(),
						  wEqpVersion );

				// 非内置，断链
				if (m_dwEqpIpAddr != dwMcuRecvIP)
				{
					::OspDisconnectTcpNode(pcMsg->srcnode);
				}

				NEXTSTATE(STATE_IDLE);
				return;
			}
			*/

			// 日志记录
			//g_cMcuLogMgr.InsertLogItem( "%s.%d<%s> online!\n", GetEqpName(m_byEqpType), m_byEqpId, strofip(m_dwEqpIpAddr) );

			//通知 VC daemon // [12/17/2009 xliang] 消息体包含: 外设注册信息, mcu接收ip,port, 外设接收port
			cServMsg.SetMsgBody( (u8*)&tRegEqp, sizeof(tRegEqp) );
			u32 dwIp = htonl(dwMcuRecvIP);
			cServMsg.CatMsgBody( (u8*)&dwIp, sizeof(dwIp) );  // 网络序
			u16 wPort = htons(wMcuStartPort);
			cServMsg.CatMsgBody( (u8*)&wPort, sizeof(wPort) );  // 网络序
			wPort = htons(wEqpStartPort);
			cServMsg.CatMsgBody( (u8*)&wPort, sizeof(wPort) );  // 网络序
			g_cMcuVcApp.SendMsgToDaemonConf(MCU_EQPCONNECTED_NOTIF, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			//ACK
			TEqpRegAck tRegAck;
			tRegAck.SetMcuIpAddr(dwMcuRecvIP);  // 这里只考虑了MCU本地的MP
			tRegAck.SetMcuStartPort(wMcuStartPort);
			tRegAck.SetEqpStartPort(wEqpStartPort);
			tRegAck.SetEqpId( m_byEqpId );
			if (tRegEqp.GetEqpType() == EQP_TYPE_MIXER && !g_cMcuVcApp.Set8kxMixerCfg(tRegAck))
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_EQP,"[Proc8KEEqpMcuRegMsg]Mixer(%d) Set8kxMixerCfg() Failed!\n",m_byEqpId);
				NEXTSTATE(STATE_IDLE);
				return;
			}
			//[nizhijun 2011/02/17]8000E外设获得丢包重传参数，根据mcucfg.ini配置的参数获得，
			//即使没有prs上线，也将mcucfg.ini中配置的参数传给其他外设
			TPrsTimeSpan tPrsTimeSpan;
			TEqpPrsInfo tEqpPrsInfo;
			//这里prsid写死，因为对于8000E来说，取配置文件中丢包重传参数时，不需要感知prsid。
			//这里写死，主要是为了适应接口中，对于非prsid的过滤。
			if ( SUCCESS_AGENT == g_cMcuAgent.GetEqpPrsCfg( PRSID_MIN, tEqpPrsInfo ) )
			{
				tPrsTimeSpan.m_wFirstTimeSpan  = htons( tEqpPrsInfo.GetFirstTimeSpan() );
				tPrsTimeSpan.m_wSecondTimeSpan =  htons( tEqpPrsInfo.GetSecondTimeSpan() );
				tPrsTimeSpan.m_wThirdTimeSpan  =  htons( tEqpPrsInfo.GetThirdTimeSpan() );
				tPrsTimeSpan.m_wRejectTimeSpan =  htons( tEqpPrsInfo.GetRejectTimeSpan() );
				cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
				cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KEEqpMcuRegMsg] eqp:%d GetEqpPrsCfg Error!\n",tRegEqp.GetEqpId());
			}
			
			// 增加通知MTU的长度, zgc, 2007-04-02
			if( tRegEqp.GetEqpType() == EQP_TYPE_VMP
				|| tRegEqp.GetEqpType() == EQP_TYPE_BAS
				|| tRegEqp.GetEqpType() == EQP_TYPE_MIXER)
			{
				TNetWorkInfo tTempInfo;
				g_cMcuAgent.GetNetWorkInfo( &tTempInfo );
				u16 wMTUSize = DEFAULT_MTU_SIZE; //tTempInfo.GetMTUSize();
				wMTUSize = htons(wMTUSize);
				cServMsg.CatMsgBody( (u8*)&wMTUSize, sizeof(wMTUSize) );
			}
			// 增加通知是否允许冗余发送, zgc, 2007-07-25
			if ( tRegEqp.GetEqpType() == EQP_TYPE_MIXER )
			{
				//TEqpMixerInfo tTempMixerInfo;
				//g_cMcuAgent.GetEqpMixerCfg( m_byEqpId, &tTempMixerInfo );
				u8 byIsSendRedundancy = 0;
				cServMsg.CatMsgBody( (u8*)&byIsSendRedundancy, sizeof(byIsSendRedundancy) );
			}
			
			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KEEqpMcuRegMsg] Send Ack to Eqp.Id%d!\n",tRegEqp.GetEqpId());
			SendMsgToPeriEqp( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			
			// set qos info        
			TQosInfo tAgentQos;
			u16 wRet = g_cMcuAgent.GetQosInfo(&tAgentQos);
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
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[ProcEqpMcuRegMsg] get qos info failed, ret:%d!\n", wRet);
			}             
			
			//设置断链检测时间，同外设侧断链检测设置
			::OspSetHBParam(m_dwEqpNode, 10, 0);
			//注册断链接收实例
			::OspNodeDiscCBRegQ(m_dwEqpNode, GetAppID(), GetInsID());
			
			
			break;
        }

    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
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
    u8  byEqpType = 0; //MCU 设备列表中的类型
    u32 dwEqpIP = 0;   //MCU 设备列表中的IP

    u32 dwEqpRcvIP;
    //u16 wEqpRcvPort;

    TPeriEqpRegAck tRegAck;
    TPrsTimeSpan tPrsTimeSpan;

	u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
    memcpy(&tRegEqp, cServMsg.GetMsgBody(), min(sizeof(TPeriEqpRegReq), wMsgBodyLen));

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[EqpSsn]: Rcv Msg %u(%s) from PeriEqp.%u Type.%u, InsId.%u, Ver.%d\n", 
                  pcMsg->event, ::OspEventDesc(pcMsg->event),
	              tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), GetInsID(), tRegEqp.GetVersion());

	dwEqpRcvIP  = tRegEqp.GetPeriEqpIpAddr();
    //wEqpRcvPort = tRegEqp.GetStartPort();

    byEqpId  = tRegEqp.GetEqpId();
    m_dwEqpIpAddr = dwEqpRcvIP;

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: Eqp.%u Type%u EqpIp.%s:%u reg request received!\n", 
                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(),
                  StrOfIP(tRegEqp.GetPeriEqpIpAddr()), tRegEqp.GetStartPort());

    u16 wRet = 0;
    dwEqpIP = 0;
    if (byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX && tRegEqp.IsHDEqp())
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: GetPeriInfo Eqp.%u Type%u EqpIp.%s reg as HDVMP!\n", 
                tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), StrOfIP(dwEqpIP) );
    }
    else
    {
        //检测设备列表中是否有该注册外设ID
        wRet = g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIP, &byEqpType);
        if (SUCCESS_AGENT != wRet)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Reg Eqp.%d is invalid Eqp id ,Nack it, ret:%d\n", tRegEqp.GetEqpId(), wRet);

            //Nack 
            cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
            post(pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(),
                 cServMsg.GetServMsgLen(), pcMsg->srcnode);

            ::OspDisconnectTcpNode(pcMsg->srcnode);
            return;
        }
        else
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "EqpSsn: GetPeriInfo Eqp.%u Type%u EqpIp.%s reg info!\n", 
                          tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), StrOfIP(dwEqpIP) );
        }
    }


    //对Eqp的Id进行判断
    if (byEqpId != (u8)GetInsID())
    {
        //NACK
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpId = % , GetInsId() = %d,not equal to InsId, nack!\n", byEqpId, GetInsID());
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
			if( tRegEqp.IsLocal() 
				&& tRegEqp.GetEqpType() == 12 //原EQP_TYPE_EMIXER值为12，兼容V4R7以前EAPU[2/15/2012 chendaiwei]
				&& byEqpType == EQP_TYPE_MIXER)
			{
				tRegEqp.SetEqpType(EQP_TYPE_MIXER);
			}
			else if (tRegEqp.IsLocal()
					&& byEqpType == EQP_TYPE_HDU
					&& (tRegEqp.GetEqpType() == 13 || //原定义，兼容老版本HDU版本卡#define EQP_TYPE_HDU_H 13
					    tRegEqp.GetEqpType() == 14 || //原定义，兼容老版本HDU版本卡#define EQP_TYPE_HDU_L 14
						tRegEqp.GetEqpType() == 15 || //原定义，兼容老版本HDU版本卡#define EQP_TYPE_HDU2 15
						tRegEqp.GetEqpType() == 12))  //原定义，兼容老版本HDU版本卡#define EQP_TYPE_HDU2_L 12
			{
				tRegEqp.SetEqpType(EQP_TYPE_HDU);
			}
            //检测合法性
            else if (!tRegEqp.IsLocal() ||
                byEqpType  != tRegEqp.GetEqpType() ||
                dwEqpRcvIP != dwEqpIP)
            {
				
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: MCU%uEQP%uType%u reg and was refused!\n",
                    tRegEqp.GetMcuId(),
                    tRegEqp.GetEqpId(),
                    tRegEqp.GetEqpType());

                if ( dwEqpRcvIP != dwEqpIP )
                {
					LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: dwEqpRcvIp(0x%x) is not equal to dwEqpIp(0x%x)\n",
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
		u8 byRecCodeFormat = emenCoding_GBK;

        //获取交换地址和版本号
        switch(tRegEqp.GetEqpType())
        {
        //录像机	
        case EQP_TYPE_RECORDER:
            if (!g_cMpManager.GetRecorderSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get Rec.%d Switch addr failed!\n", byEqpId);
            }
            wEqpVersion = DEVVER_RECORDER;

			//  [5/17/2013 guodawei] 获取编码类型并保存
			if (wMsgBodyLen >= sizeof(TPeriEqpRegReq) + sizeof(u8))
			{
				byRecCodeFormat = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegReq));
			}
			g_cMcuVcApp.SetEqpCodeFormat(byEqpId, byRecCodeFormat);

            break;

        //混音器
        case EQP_TYPE_MIXER:
			{
				if (!g_cMpManager.GetMixerSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
				{
					bGetSwitchSuccess = FALSE;
					LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get Mix.%d Switch addr failed!\n", byEqpId);
				}
				u8 byMixerSubtype = UNKONW_MIXER;
				g_cMcuAgent.GetMixerSubTypeByEqpId(byEqpId,byMixerSubtype);
				switch (byMixerSubtype)
				{
				case APU_MIXER:
					wEqpVersion = DEVVER_MIXER;
					break;
				case EAPU_MIXER:
					wEqpVersion = DEVVER_EMIXER;
					break;
				case APU2_MIXER:
					wEqpVersion = DEVVER_APU2;
					break; 
				default:
					wEqpVersion = 0;
					break;
				}
				break;
			}

        //码流适配器
        case EQP_TYPE_BAS:
            if (!g_cMpManager.GetBasSwitchAddr(byEqpId, dwRcvIP, wRcvPort))
            {
                bGetSwitchSuccess = FALSE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get BAS.%d Switch addr failed!\n", byEqpId);
            }              
			// xliang [1/8/2009] BAS Develop Version defers:
			//APU2 Bas进该IF条件[3/22/2013 chendaiwei]
			if(g_cMcuAgent.IsMpuBas(byEqpId) )
			{
				//wEqpVersion = DEVVER_MPU;
				wEqpVersion = g_cMcuAgent.GetBasEqpVersion(byEqpId);
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
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get VMP.%d Switch addr failed!\n", byEqpId);
            }
			// xliang [1/8/2009] VMP Develop Version defers:
			if( g_cMcuAgent.IsSVmp(byEqpId) 
				//|| g_cMcuAgent.IsDVmp(byEqpId) 
				//|| g_cMcuAgent.IsEVpu(byEqpId) 
				)
			{
				//wEqpVersion = DEVVER_MPU;
				wEqpVersion = g_cMcuAgent.GetVmpEqpVersion(byEqpId);
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
                LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Get Prs.%d Switch addr failed!\n", byEqpId);
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
			wEqpVersion = g_cMcuAgent.GetHDUEqpVersion(byEqpId);
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

            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: Eqp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
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
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Ssn: ProcEqpMcuRegMsg] Reg Ack with System SSRC.%u.\n", dwSysSSrc);
        tRegAck.SetMSSsrc(dwSysSSrc);

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		if ( tRegEqp.GetEqpType() == EQP_TYPE_HDU ||
			 tRegEqp.GetEqpType() == EQP_TYPE_RECORDER ||
			 tRegEqp.GetEqpType() == EQP_TYPE_MIXER
			)
		{
			//[nizhijun 2011/02/17]8000H外设获得丢包重传参数，根据mcucfg.ini配置的参数获得，
			//即使没有prs上线，也将mcucfg.ini中配置的参数传给其他外设
			TEqpPrsInfo tEqpPrsInfo;
			//这里prsid写死，因为对于8000h来说，取配置文件中丢包重传参数时，不需要感知prsid。
			//这里写死，主要是为了适应接口中，对于非prsid的过滤。
			if ( SUCCESS_AGENT == g_cMcuAgent.GetEqpPrsCfg( PRSID_MIN, tEqpPrsInfo ) )
			{
				tPrsTimeSpan.m_wFirstTimeSpan  = htons( tEqpPrsInfo.GetFirstTimeSpan() );
				tPrsTimeSpan.m_wSecondTimeSpan =  htons( tEqpPrsInfo.GetSecondTimeSpan() );
				tPrsTimeSpan.m_wThirdTimeSpan  =  htons( tEqpPrsInfo.GetThirdTimeSpan() );
				tPrsTimeSpan.m_wRejectTimeSpan =  htons( tEqpPrsInfo.GetRejectTimeSpan() );
				cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
				cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
			}
			else
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_EQP,"[Proc8KEEqpMcuRegMsg] eqp:%d GetEqpPrsCfg Error!\n",tRegEqp.GetEqpId());
			}
		}
#else      
        GetPrsTimeSpan(&tPrsTimeSpan);
        tPrsTimeSpan.m_wFirstTimeSpan = htons(tPrsTimeSpan.m_wFirstTimeSpan);
        tPrsTimeSpan.m_wSecondTimeSpan = htons(tPrsTimeSpan.m_wSecondTimeSpan);
        tPrsTimeSpan.m_wThirdTimeSpan = htons(tPrsTimeSpan.m_wThirdTimeSpan);
        tPrsTimeSpan.m_wRejectTimeSpan = htons(tPrsTimeSpan.m_wRejectTimeSpan);
        cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
        cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
#endif
        // 如果是录像机，通知MCU别名
        if ( tRegEqp.GetEqpType() == EQP_TYPE_RECORDER )
        {
			/*guoidawei*/
			s8 szMcuAlias[MAX_ALIAS_LENGTH] = {0};
			s8 szMcuGBKAlias[MAX_ALIAS_LENGTH] = {0};
			g_cMcuAgent.GetMcuAlias( szMcuAlias, MAX_ALIAS_LENGTH );
			szMcuAlias[MAX_ALIAS_LENGTH - 1] = '\0';
#ifdef _UTF8
			if (byRecCodeFormat == emenCoding_GBK)
			{
				//UTF8-GBK转换
				utf8_to_gb2312(szMcuAlias, szMcuGBKAlias, sizeof(szMcuGBKAlias) - 1);
				memcpy(szMcuAlias, szMcuGBKAlias, sizeof(szMcuGBKAlias));
			}
#endif
			cServMsg.CatMsgBody( (u8*)szMcuAlias, MAX_ALIAS_LENGTH );
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
			tCfgInfo.SetEqpId( tInfo.GetEqpId() );

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
		
        if (tRegEqp.GetEqpType() == EQP_TYPE_HDU   /*||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU_H ||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU_L ||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU2  ||
			tRegEqp.GetEqpType() == EQP_TYPE_HDU2_L*/)
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


        if ( tRegEqp.GetEqpType() == EQP_TYPE_VMP )
        {
#ifdef _UTF8
            //[4/8/2013 liaokang] 编码方式
            u8 byEncoding = emenCoding_Utf8;
            cServMsg.CatMsgBody( &byEncoding, sizeof(u8));
#endif
        }

        post(pcMsg->srcid, pcMsg->event + 1,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
                
        //save information
        m_dwEqpNode = pcMsg->srcnode;
        m_dwEqpIId  = pcMsg->srcid;
        m_byEqpId   = byEqpId;
        m_byEqpType = tRegEqp.GetEqpType();

		//  [5/16/2013 guodawei] 录像机别名的UTF8转换
#ifdef _UTF8
		if (tRegEqp.GetEqpType() == EQP_TYPE_RECORDER && byRecCodeFormat == emenCoding_GBK)
		{
			s8 achUTF8Alias[MAXLEN_EQP_ALIAS] = {0};

			//做GBK-UTF8转换
			gb2312_to_utf8(tRegEqp.GetEqpAlias(), achUTF8Alias, sizeof(achUTF8Alias) - 1);
			memcpy(m_achAlias, achUTF8Alias, MAXLEN_EQP_ALIAS);
		}
		else
		{
			memcpy(m_achAlias, tRegEqp.GetEqpAlias(), MAXLEN_EQP_ALIAS);
		}
#else
        memcpy(m_achAlias, tRegEqp.GetEqpAlias(), MAXLEN_EQP_ALIAS);
#endif

        m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';

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
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[ProcEqpMcuRegMsg] get qos info failed, ret:%d!\n", wRet);
        }             
             
		//::OspSetHBParam(pcMsg->srcnode, 20, 5);
		
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
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: [ProcEqpMcuRegMsg]Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}
/*====================================================================
    函数名      Proc8KiMixerMcuRegMsg
    功能        ：处理8KI混音器外设的登记请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/05/18    4.7         田志勇         创建
====================================================================*/
void CEqpSsnInst::Proc8KiMixerMcuRegMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tRegEqp;
    TPeriEqpRegAck tRegAck;
    TPrsTimeSpan tPrsTimeSpan;

	const u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
    memcpy(&tRegEqp, cServMsg.GetMsgBody(), min(sizeof(TPeriEqpRegReq), wMsgBodyLen));

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]: Rcv Msg %u(%s) from PeriEqp.%u Type.%u, InsId.%u, Ver.%d\n", 
                  pcMsg->event, ::OspEventDesc(pcMsg->event),
                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(), GetInsID(), tRegEqp.GetVersion());

    m_dwEqpIpAddr = tRegEqp.GetPeriEqpIpAddr();

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]: Eqp.%u Type%u EqpIp.%s:%u reg request received!\n", 
                  tRegEqp.GetEqpId(), tRegEqp.GetEqpType(),
                  StrOfIP(tRegEqp.GetPeriEqpIpAddr()), tRegEqp.GetStartPort());


    //对Eqp的Id进行判断
    if (tRegEqp.GetEqpId() != (u8)GetInsID())
    {
        //NACK
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]EqpId = % , GetInsId() = %d,not equal to InsId, nack!\n", tRegEqp.GetEqpId(), GetInsID());
        cServMsg.SetErrorCode(ERR_MCU_REGEQP_INVALID);
        post(pcMsg->srcid, pcMsg->event + 2,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
        return;
    }

    
    switch(CurState())
    {
    case STATE_IDLE:
        {
		u16 wEqpVersion = 0;
		u8 byMixerSubtype = UNKONW_MIXER;
		g_cMcuAgent.GetMixerSubTypeByEqpId(tRegEqp.GetEqpId(),byMixerSubtype);
		switch (byMixerSubtype)
		{
		case APU_MIXER:
			wEqpVersion = DEVVER_MIXER;
			break;
		case EAPU_MIXER:
			wEqpVersion = DEVVER_EMIXER;
			break;
		case APU2_MIXER:
			wEqpVersion = DEVVER_APU2;
			break; 
		default:
			wEqpVersion = 0;
			break;
		}
		if (!tRegEqp.IsLocal() || tRegEqp.GetVersion() != wEqpVersion)
        {
			
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]: MCU(%u)EQP(%u) Ver(%d) LocalVer(%d) reg and was refused!\n",
                tRegEqp.IsLocal(),tRegEqp.GetEqpId(),tRegEqp.GetVersion(),wEqpVersion);
            post(pcMsg->srcid, pcMsg->event + 2,cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
            return;
        }
        //ACK
		TEqpMixerInfo tTempMixerInfo;
		if (SUCCESS_AGENT != g_cMcuAgent.GetEqpMixerCfg( tRegEqp.GetEqpId() , &tTempMixerInfo ))
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"[Proc8KiMixerMcuRegMsg] GetEqpMixerCfg(%d) failed!\n", tRegEqp.GetEqpId());
			return;	
		}
        tRegAck.SetMcuIpAddr(m_dwEqpIpAddr);
        tRegAck.SetMcuStartPort(tTempMixerInfo.GetMcuRecvPort());
        if (g_cMSSsnApp.IsDoubleLink())
        {
            tRegAck.SetAnotherMpcIp(g_cMSSsnApp.GetAnotherMcuIP());
        }
        else
        {
            tRegAck.SetAnotherMpcIp(0);
        }
		u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg] Reg Ack with System SSRC.%u.\n", dwSysSSrc);
        tRegAck.SetMSSsrc(dwSysSSrc);
  
        GetPrsTimeSpan(&tPrsTimeSpan);
        tPrsTimeSpan.m_wFirstTimeSpan = htons(tPrsTimeSpan.m_wFirstTimeSpan);
        tPrsTimeSpan.m_wSecondTimeSpan = htons(tPrsTimeSpan.m_wSecondTimeSpan);
        tPrsTimeSpan.m_wThirdTimeSpan = htons(tPrsTimeSpan.m_wThirdTimeSpan);
        tPrsTimeSpan.m_wRejectTimeSpan = htons(tPrsTimeSpan.m_wRejectTimeSpan);
        cServMsg.SetMsgBody((u8 *)&tRegAck, sizeof(tRegAck));
        cServMsg.CatMsgBody((u8 *)&tPrsTimeSpan, sizeof(tPrsTimeSpan));
        
		// 增加通知MTU的长度
		TNetWorkInfo tTempInfo;
		g_cMcuAgent.GetNetWorkInfo( &tTempInfo );
		u16 wMTUSize = tTempInfo.GetMTUSize();
		wMTUSize = htons(wMTUSize);
		cServMsg.CatMsgBody( (u8*)&wMTUSize, sizeof(wMTUSize) );
		// 增加通知是否允许冗余发送
		u8 byIsSendRedundancy = tTempMixerInfo.IsSendRedundancy() ? 1 : 0;
		cServMsg.CatMsgBody( (u8*)&byIsSendRedundancy, sizeof(byIsSendRedundancy) );

        post(pcMsg->srcid, pcMsg->event + 1,
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
                
        //save information
        m_dwEqpNode = pcMsg->srcnode;
        m_dwEqpIId  = pcMsg->srcid;
        m_byEqpId   = tRegEqp.GetEqpId();
        m_byEqpType = tRegEqp.GetEqpType();
        memcpy(m_achAlias, tRegEqp.GetEqpAlias(), MAXLEN_EQP_ALIAS);
        m_achAlias[MAXLEN_EQP_ALIAS-1] = 0;

        // set qos info        
        TQosInfo tAgentQos;
        u16 wRet = g_cMcuAgent.GetQosInfo(&tAgentQos);
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
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg] get qos info failed, ret:%d!\n", wRet);
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
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Proc8KiMixerMcuRegMsg]Wrong message %u(%s) received in current state %u!\n",
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
    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "EqpSsn: Rcv Msg %u(%s) ! Eqp%u.Type%u disconnected!\n", 
        pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byEqpId, m_byEqpType );
    
	CServMsg	cServMsg;
	TMt	tEqp;

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
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "CEqpInst: Wrong message %u(%s) received in state %u!\n", 
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

    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Eqpssn]: Recv %u(%s) from eqp.%d!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), m_byEqpId);

	switch( CurState() )
	{
	case STATE_NORMAL:
		cServMsg.SetServMsg( pcMsg->content, pcMsg->length );
		if(PRS_MCU_SETSRC_ACK == pcMsg->event ||
		   PRS_MCU_REMOVEALL_ACK == pcMsg->event)
		{
			cServMsg.SetMsgBody(&m_byEqpId, sizeof(m_byEqpId));
		}
		if( !cServMsg.GetConfId().IsNull() )
		{
			// [1/7/2010 xliang] Set srcSsnId value manually for BAS_MCU_NEEDIFRAME_CMD
			if(BAS_MCU_NEEDIFRAME_CMD == pcMsg->event  || 
			   HDU_MCU_START_PLAY_ACK == pcMsg->event  ||
			   HDU_MCU_STOP_PLAY_ACK  == pcMsg->event  ||
			   VMP_MCU_STARTVIDMIX_NOTIF == pcMsg->event ||
			   VMP_MCU_STOPVIDMIX_NOTIF == pcMsg->event ||
			   VMP_MCU_CHANGESTATUS_NOTIF == pcMsg->event ||
			   VMP_MCU_NEEDIFRAME_CMD == pcMsg->event
			   )
			{
// 				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[Eqpssn]: recv BAS_MCU_NEEDIFRAME_CMD in EqpSsnInst.%d ---- cServMsg.GetSrcSsnId() is %d !\n",
// 					GetInsID(), cServMsg.GetSrcSsnId());
				cServMsg.SetSrcSsnId((u8)GetInsID());	
			}
			g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfId(), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}	
		break;
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: [ProcEqpToMcuMsg]Wrong message %u(%s) received in current state %u!\n", 
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
    LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[Eqpssn]: Send %u(%s) to eqp.%d!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), m_byEqpId);

	switch( CurState() )
	{
	case STATE_NORMAL:
		SendMsgToPeriEqp( pcMsg->event, pcMsg->content, pcMsg->length );
		break;
	default:

		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn:[ProcMcuToEqpMsg] Wrong message %u(%s) received in current state %u!\n", 
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
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "EqpSsn: [ProcEqpToMcuDaemonConfMsg]Wrong message %u(%s) received in current state %u!\n", 
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
	if (byPeriEqpId > MAXNUM_MCU_PERIEQP || 0 == byPeriEqpId)
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
