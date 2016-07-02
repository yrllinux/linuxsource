/*****************************************************************************
   模块名      : 新方案业务
   文件名      : evmcueqp.h
   相关文件    : 
   文件实现功能: 新方案业务MCU和外设业务接口消息定义
                 *_REQ: 需应答请求
				 *_ACK, *_NACK: 请求之答复
				 *_CMD: 不需应答命令
				 *_NOTIF: 状态通知
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/05/15  1.0         LI Yi       从evsvc.h中分出来
******************************************************************************/
#ifndef _EV_MCUEQP_H_
#define _EV_MCUEQP_H_

#include "osp.h"
#include "eventid.h"

//注：为保证程序正确运行，所有成功和失败应答比原消息号分别加1，2

/**********************************************************
 以下为MCU和外设之间业务接口 (26001-27000)
**********************************************************/

//前面100为内部保留

/*MCU和REC间业务接口消息(26101-26300)*/
#define EV_MCUREC_BGN			EV_MCUEQP_BGN + 100
#define EV_MCUREC_END			EV_MCUREC_BGN + 199

/*MCU和MIXER间业务接口消息(26301-26400)*/
#define EV_MCUMIXER_BGN			EV_MCUREC_END	+ 1
#define EV_MCUMIXER_END			EV_MCUMIXER_BGN + 99

/*MCU和TVWALL间业务接口消息(26401-26440)*/
#define EV_MCUTVWALL_BGN		EV_MCUMIXER_END	 + 1
#define EV_MCUTVWALL_END		EV_MCUTVWALL_BGN + 39

/*MCU和HDU间业务接口消息(26441-26500)*/
#define EV_MCUHDU_BGN		EV_MCUTVWALL_END + 1
#define EV_MCUHDU_END		EV_MCUHDU_BGN + 59

/*MCU和DCS会话间业务接口消息(26501-26600)*/
#define EV_MCUDCS_BGN			EV_MCUHDU_END + 1
#define EV_MCUDCS_END			EV_MCUDCS_BGN + 99

/*MCU和BAS间业务接口消息(26601-26700)*/
#define EV_MCUBAS_BGN			EV_MCUDCS_END + 1
#define EV_MCUBAS_END			EV_MCUBAS_BGN + 99

/*画面复合器业务消息(26701-26800)*/
#define EV_MCUVMP_BGN			EV_MCUBAS_END + 1
#define EV_MCUVMP_END			EV_MCUVMP_BGN + 99

/*复合电视墙业务消息(26801-26900)*/
#define EV_MCUVMPTW_BGN			EV_MCUVMP_END   + 1
#define EV_MCUVMPTW_END			EV_MCUVMPTW_BGN + 99

/*丢包重传器业务消息(26901-27000)*/
#define EV_MCUPRS_BGN			EV_MCUVMPTW_END + 1
#define EV_MCUPRS_END			EV_MCUPRS_BGN   + 99

/*MpuBas业务消息(39601-39650)*/
#define EV_MCUMPUBAS_BGN	    EV_MPU_BGN
#define EV_MCUMPUBAS_END	    EV_MCUMPUBAS_BGN + 49

/*MpuEBap业务消息(39651-39700)*/
#define EV_MCUMPUEBAP_BGN       EV_MCUMPUBAS_END + 1
#define EV_MCUMPUEBAP_END       EV_MCUMPUEBAP_BGN + 49

/*MpuEVpu业务消息(39701-39800)*/
#define EV_MCUMPUEVPU_BGN       EV_MCUMPUEBAP_END + 1
#define EV_MCUMPUEVPU_END       EV_MCUMPUEVPU_BGN + 99

/*MpuSVmp业务消息(39801-39900)*/
#define EV_MCUMPUSVMP_BGN       EV_MCUMPUEVPU_END + 1
#define EV_MCUMPUSVMP_END       EV_MCUMPUSVMP_BGN + 99

/*MpuDVmp业务消息(39901-40000)*/
#define EV_MCUMPUDVMP_BGN       EV_MCUMPUSVMP_END + 1
#define EV_MCUMPUDVMP_END       EV_MCUMPUDVMP_BGN + 99

/*文件解码器业务消息(26901-27000)*/
//#define EV_MCUFILEENC_BGN	EV_MCUPRS_END     + 1
//#define EV_MCUFILEENC_END	EV_MCUFILEENC_BGN + 1

//以下为MCU和外设之间会议操作，所有和信道相关消息，信道号均位于消息头中


/*-------- 以下为MCU和新版REC消息号定义(26101-26300) --------*/

//录像机向MCU发出注册请求，REC->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( REC_MCU_REG_REQ,					EV_MCUREC_BGN );
//MCU接受应答，MCU->REC，消息体为消息体为TPeriEqpRegAck
OSPEVENT( MCU_REC_REG_ACK,					EV_MCUREC_BGN + 1 );
//MCU拒绝应答，MCU->REC，无消息体
OSPEVENT( MCU_REC_REG_NACK,					EV_MCUREC_BGN + 2 );

//MCU请求获得录像机状态请求, MCU->REC，消息体为TEqp
OSPEVENT( MCU_REC_GETRECSTATUS_REQ,			EV_MCUREC_BGN + 5 );
//回馈录像机状态，REC->MCU，消息体TPeriEqpStatus
OSPEVENT( REC_MCU_GETRECSTATUS_ACK,			EV_MCUREC_BGN + 6 );
//拒绝回馈录像机状态，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_GETRECSTATUS_NACK,		EV_MCUREC_BGN + 7 );
//录像机状态通知，REC->MCU，消息体为TPeriEqpStatus
OSPEVENT( REC_MCU_RECSTATUS_NOTIF,			EV_MCUREC_BGN + 8 );

/*
 *录像列表请求消息：
 *录像机收到请求后分配发送录像文件通知，全部发送完毕后发送ACK消息
 *若录像机拒绝发送列表，则仅发送NACK消息。
 */
//MCU请求录像机给出录像列表命令，MCU->REC，消息体为TEqp + 文件组Id（u32） + u32第几页 + u32单页个数
OSPEVENT( MCU_REC_LISTALLRECORD_REQ,		EV_MCUREC_BGN + 10 );
//录像机列应答(完毕)消息，REC->MCU,消息体为TEqp
OSPEVENT( REC_MCU_LISTALLRECORD_ACK,		EV_MCUREC_BGN + 11 );
//录像机拒绝录像列表请求，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_LISTALLRECORD_NACK,		EV_MCUREC_BGN + 12 );
//录像机列表通知消息，REC->MCU, 消息体为TEqp+TRecFileListNotify
// 新录播文件列表上报，TEqp + TMtAlias（新录播）+ 组Id(u32)+文件个数n(u32)+(文件显示名(s8*129)+文件Id(u32)+文件地址IP(u32)+文件路径(s8*129)+文件真实名(s8*129))* n
OSPEVENT( REC_MCU_LISTALLRECORD_NOTIF,		EV_MCUREC_BGN + 13 );
// 新录播节目组列表上报，TEqp + TMtAlias（新录播）+ 总的组个数n(u32)+(组名(s8*49)+组Id(u32)+本组文件个数(u32))* n 
OSPEVENT( REC_MCU_FILEGROUPLIST_NOTIF,		EV_MCUREC_BGN + 14 );


//MCU请求获得录像机录像通道状态请求, MCU->REC, 消息体为TEqp，信道号位于消息头中
OSPEVENT( MCU_REC_GETRECORDCHNSTATUS_REQ,	EV_MCUREC_BGN + 15 );
//回馈录像机信道状态，REC->MCU，消息体TEqp+TRecChnnlStatus
OSPEVENT( REC_MCU_GETRECORDCHNSTATUS_ACK,	EV_MCUREC_BGN + 16 );
//拒绝回馈录像机信道状态，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_GETRECORDCHNSTATUS_NACK,	EV_MCUREC_BGN + 17 );
//录像机录像信道状态通知，REC->MCU，消息体为TMt+TEqp+TRecChnnlStatus
OSPEVENT( REC_MCU_RECORDCHNSTATUS_NOTIF,	EV_MCUREC_BGN + 18 );

//MCU请求获得录像机放像通道状态请求, MCU->REC, 消息体为TEqp，信道号位于消息头中
OSPEVENT( MCU_REC_GETPLAYCHNSTATUS_REQ,		EV_MCUREC_BGN + 20 );
//回馈录像机信道状态，REC->MCU，消息体TEqp+TRecChnnlStatus
OSPEVENT( REC_MCU_GETPLAYCHNSTATUS_ACK,		EV_MCUREC_BGN + 21 );
//拒绝回馈录像机信道状态，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_GETPLAYCHNSTATUS_NACK,	EV_MCUREC_BGN + 22 );
//录像机放像信道状态通知，REC->MCU，消息体为TEqp+TRecChnnlStatus
OSPEVENT( REC_MCU_PLAYCHNSTATUS_NOTIF,		EV_MCUREC_BGN + 23 );

//获取录相进度，消息体：无 (信道号位于消息头)
OSPEVENT( MCU_REC_GETRECPROG_CMD,           EV_MCUREC_BGN + 24 );
//录像机当前录像进度通知消息，REC->MCU，消息体为TMt+TEqp+TRecProg，信道号位于消息头
OSPEVENT( REC_MCU_RECORDPROG_NOTIF,			EV_MCUREC_BGN + 25 );

//获取放相进度，消息体：无 (信道号位于消息头)
OSPEVENT( MCU_REC_GETPLAYPROG_CMD,          EV_MCUREC_BGN + 26 );
//录像机当前放像进度通知消息，REC->MCU，消息体为TEqp+TRecProg，信道号位于消息头
OSPEVENT( REC_MCU_PLAYPROG_NOTIF,			EV_MCUREC_BGN + 27 );
//录像机异常通知消息，REC->MCU，消息体为TEqp，信道号和错误码位于消息头
OSPEVENT( REC_MCU_EXCPT_NOTIF,				EV_MCUREC_BGN + 28 );

/////////////////
//放像消息

//MCU请求播放，MCU->REC，消息体为TEqp + TMediaEncrypt(video) + TDoublePayload(main video) +
//                TMediaEncrypt(audio) + TDoublePayload(main audio) + 
//                TDoublePayload(second video) + TDoublePayload(second audio) +
//				  u8(byDStreamMediaType) +
//                TRecProg(放像进度) + 录像名(s8*128) + TCapSupportEx
//             MCU->VRSREC,消息内容：TEqp(标识录像设备) + 开启放像相关数据[文件地址IP(u32)+文件路径(s8*129)]+ 文件名（129*s8） + 是否加密放像（u8）
OSPEVENT( MCU_REC_STARTPLAY_REQ,			EV_MCUREC_BGN + 30 );
//同意MCU播放请求应答，REC->MCU，消息体为TEqp + TPlayFileAttrib(放像文件属性) + TPlayFileMediaInfo(放象文件媒体信息)
//返回的信道号位于消息头
OSPEVENT( REC_MCU_STARTPLAY_ACK,			EV_MCUREC_BGN + 31 );
//拒绝MCU播放应答，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_STARTPLAY_NACK,			EV_MCUREC_BGN + 32 );

//MCU请求暂停，MCU->REC，消息体为TEqp
OSPEVENT( MCU_REC_PAUSEPLAY_REQ,			EV_MCUREC_BGN + 35 );
//REC同意暂停请求，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_PAUSEPLAY_ACK,			EV_MCUREC_BGN + 36 );
//REC拒绝暂停请求，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_PAUSEPLAY_NACK,			EV_MCUREC_BGN + 37 );

//继续播放请求，MCU->REC，消息体为TEqp
OSPEVENT( MCU_REC_RESUMEPLAY_REQ,			EV_MCUREC_BGN + 40 );
//同意继续播放，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_RESUMEPLAY_ACK,			EV_MCUREC_BGN + 41 );
//拒绝继续播放，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_RESUMEPLAY_NACK,			EV_MCUREC_BGN + 42 );

//停止播放请求，MCU->REC，消息体为TEqp
OSPEVENT( MCU_REC_STOPPLAY_REQ,				EV_MCUREC_BGN + 45 );
//同意停止播放应答,REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_STOPPLAY_ACK,				EV_MCUREC_BGN + 46 );
//拒绝停止播放应答,REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_STOPPLAY_NACK,			EV_MCUREC_BGN + 47 );

//请求以快进方式播放，MCU->REC，消息体为TEqp+快进倍数(u8)
OSPEVENT( MCU_REC_FFPLAY_REQ,				EV_MCUREC_BGN + 50 );
//同意快进方式播放应答，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_FFPLAY_ACK,				EV_MCUREC_BGN + 51 );
//拒绝快进方式播放应答,REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_FFPLAY_NACK,				EV_MCUREC_BGN + 52 );

//请求以快退方式播放，MCU->REC，消息体为TEqp+快退倍数(u8)
OSPEVENT( MCU_REC_FBPLAY_REQ,				EV_MCUREC_BGN + 55 );
//同意快退方式播放应答，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_FBPLAY_ACK,				EV_MCUREC_BGN + 56 );
//拒绝快退方式播放应答,REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_FBPLAY_NACK,				EV_MCUREC_BGN + 57 );

//MCU请求将播放进度调整到新的位置，消息体为TEqp+TRecProg
OSPEVENT( MCU_REC_SEEK_REQ,					EV_MCUREC_BGN + 60 );
//同意调整应答，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_SEEK_ACK,					EV_MCUREC_BGN + 61 );
//调整拒绝，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_SEEK_NACK,				EV_MCUREC_BGN + 62 );
//调整录像机RTCP参数，MCU->REC，消息体：TEqp+要设置第几路参数(u8)+TRecRtcpBack
OSPEVENT( MCU_REC_SETRTCP_NFY,				EV_MCUREC_BGN + 29 );

////////////////
//录像消息
//消息体为TMt(被录像的终端，如果MCU号为空，表示对会议录像。以下请求同)
// + TEqp( 所要使用得录像机) + TRecStartPara + TMediaEncrypt(video) +
//TDoublePayload(video) + TMediaEncrypt(audio) + TDoublePayload(video) +
//TMediaEncrypt(double video) + TDoublePayload(double video) + 录像名 + TCapSupportEx
// vrs新录播消息体：TMt+TEqp+录像文件名(s8*129)+发布方式(u8)+是否直播(u8)
OSPEVENT( MCU_REC_STARTREC_REQ,				EV_MCUREC_BGN + 70 );
//同意MCU录像请求应答，REC->MCU，消息体为TEqp，返回的信道号位于消息头
OSPEVENT( REC_MCU_STARTREC_ACK,				EV_MCUREC_BGN + 71 );
//拒绝MCU录像应答，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_STARTREC_NACK,			EV_MCUREC_BGN + 72 );

//MCU请求暂停录像，MCU->REC，消息体为TEqp
OSPEVENT( MCU_REC_PAUSEREC_REQ,				EV_MCUREC_BGN + 75 );
//REC同意暂停录像请求，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_PAUSEREC_ACK,				EV_MCUREC_BGN + 76 );
//REC拒绝暂停录像请求，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_PAUSEREC_NACK,			EV_MCUREC_BGN + 77 );

//继续录像请求，MCU->REC，消息体为TEqp
OSPEVENT( MCU_REC_RESUMEREC_REQ,			EV_MCUREC_BGN + 80 );
//同意继续录像，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_RESUMEREC_ACK,			EV_MCUREC_BGN + 81 );
//拒绝继续录像,REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_RESUMEREC_NACK,			EV_MCUREC_BGN + 82 );

//停止录像请求，MCU->REC，消息体为TEqp
OSPEVENT( MCU_REC_STOPREC_REQ,				EV_MCUREC_BGN + 85 );
//同意停止录像应答，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_STOPREC_ACK,				EV_MCUREC_BGN + 86 );
//拒绝停止录像应答，REC->MCU，消息体为TEqp
OSPEVENT( REC_MCU_STOPREC_NACK,				EV_MCUREC_BGN + 87 );
//会商差异,停止录像或切文件
OSPEVENT( REC_MCU_ADDVIDFILE_NTF,			EV_MCUREC_BGN + 88 );

//改变录像模式请求, 消息体TMt ＋ u8( REC_MODE_NORMAL, REC_MODE_SKIPFRAME )
OSPEVENT( MCU_REC_CHANGERECMODE_REQ,		EV_MCUREC_BGN + 90 );
//同意，MCU->MCS，消息体为 TMt 
OSPEVENT( REC_MCU_CHANGERECMODE_ACK,		EV_MCUREC_BGN + 91 );
//反对，MCU->MCS，消息体为 TMt 
OSPEVENT( REC_MCU_CHANGERECMODE_NACK,		EV_MCUREC_BGN + 92 );

//MCU 请求删除录像机中的记录，MCU->REC ,消息体为TEqp+记录名(0结尾字符串)
OSPEVENT( MCU_REC_DELETERECORD_REQ,			EV_MCUREC_BGN + 93 );
//录像机同意并成功删除录像文件应答
//消息体为TEqp
OSPEVENT( REC_MCU_DELETERECORD_ACK,			EV_MCUREC_BGN + 94);
//录像机拒绝删除录像 消息体TEqp
OSPEVENT( REC_MCU_DELETERECORD_NACK,		EV_MCUREC_BGN + 95 );

//MCU请求更改记录文件, MCU->REC
//消息体为TEqp + u8(旧记录名长度) + 旧记录名 + u8(新记录名长度) + 新记录名
OSPEVENT( MCU_REC_RENAMERECORD_REQ,			EV_MCUREC_BGN + 96 );
//同意，消息体为TEqp
OSPEVENT( REC_MCU_RENAMERECORD_ACK,			EV_MCUREC_BGN + 97 );
//反对，消息体为TEqp
OSPEVENT( REC_MCU_RENAMERECORD_NACK,		EV_MCUREC_BGN + 98 );

//发布录像请求 MCU->REC，消息体为TEqp + u8(PUBLISH_LEVEL) + 录像名
OSPEVENT( MCU_REC_PUBLISHREC_REQ,			EV_MCUREC_BGN + 100 );
//同意，REC->MCU，消息体为 TEqp
OSPEVENT( REC_MCU_PUBLISHREC_ACK,			EV_MCUREC_BGN + 101 );
//反对，REC->MCU，消息体为 TEqp
OSPEVENT( REC_MCU_PUBLISHREC_NACK,			EV_MCUREC_BGN + 102 );

//取消发布录像请求 MCU->REC，消息体为TEqp +  录像名
OSPEVENT( MCU_REC_CANCELPUBLISHREC_REQ,		EV_MCUREC_BGN + 105 );
//同意，REC->MCU，消息体为 TEqp
OSPEVENT( REC_MCU_CANCELPUBLISHREC_ACK,		EV_MCUREC_BGN + 106 );
//反对，REC->MCU，消息体为 TEqp
OSPEVENT( REC_MCU_CANCELPUBLISHREC_NACK,	EV_MCUREC_BGN + 107 );
//录像机请求关键帧
OSPEVENT( REC_MCU_NEEDIFRAME_CMD,	        EV_MCUREC_BGN + 108 );
//设置接收参数，MCU->REC,消息体：u8(stream number)+TDoublePayload+TMediaEncrypt
OSPEVENT( MCU_REC_SETRECV_NTF,				EV_MCUREC_BGN + 120 );
//调整录像源的BACKRTCP
/*OSPEVENT( MCU_REC_ADJUST_BACKRTCP_CMD,	        EV_MCUREC_BGN + 109 );*/


/*----------  MCU和REC消息定义结束(26101-26300)   ------------*/


/*----------  以下为混音器消息号定义(26301-26400)  -----------*/

//混音器向MCU发出注册请求，MIXER->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( MIXER_MCU_REG_REQ,				EV_MCUMIXER_BGN );
//MCU接受应答，MCU->MIXER，消息体为TPeriEqpRegAck + TPrsTimeSpan + u16(MTU size) + u8(IsSendRedundancy)
OSPEVENT( MCU_MIXER_REG_ACK,				EV_MCUMIXER_BGN + 1 );
//MCU拒绝应答，MCU->MIXER，无消息体
OSPEVENT( MCU_MIXER_REG_NACK,				EV_MCUMIXER_BGN + 2 );
//混音器状态通知，MIXER->MCU，消息体为TPeriEqpStatus
OSPEVENT( MIXER_MCU_MIXERSTATUS_NOTIF,		EV_MCUMIXER_BGN + 3 );

//MCU开始混音请求，MCU->MIXER 消息体: TMixerStart+TDoublePayload+TCapSupportEx
OSPEVENT( MCU_MIXER_STARTMIX_REQ,			EV_MCUMIXER_BGN + 5 );
//同意开始混音应答，MIXER->MCU，消息体: 1 byte(混音组Id)+ 1 byte(混音深度)
OSPEVENT( MIXER_MCU_STARTMIX_ACK,			EV_MCUMIXER_BGN + 6 );
//拒绝开始混音应答，MIXER->MCU，无消息体
OSPEVENT( MIXER_MCU_STARTMIX_NACK,			EV_MCUMIXER_BGN + 7 );

//混音组状态通知，MIXER->MCU，消息体为TMixerGrpStatus
OSPEVENT( MIXER_MCU_GRPSTATUS_NOTIF,		EV_MCUMIXER_BGN + 10 );

//MCU停止混音请求，MCU->MIXER，消息体: 1 byte(混音组Id)
OSPEVENT( MCU_MIXER_STOPMIX_REQ,			EV_MCUMIXER_BGN + 15 );
//同意停止混音应答，MIXER->MCU，消息体: 1 byte(混音组Id)
OSPEVENT( MIXER_MCU_STOPMIX_ACK,			EV_MCUMIXER_BGN + 16 );
//拒绝停止混音应答，MIXER->MCU，无消息体
OSPEVENT( MIXER_MCU_STOPMIX_NACK,			EV_MCUMIXER_BGN + 17 );

//加入成员请求，MCU->MIXER，消息体:1 byte(混音组Id)+ TMixMember + 1 byte(成员语音原始格式)
//  m_tMember,m_tAddr,有效
OSPEVENT( MCU_MIXER_ADDMEMBER_REQ,			EV_MCUMIXER_BGN + 20 );
//加入成员应答消息，MIXER->MCU，消息体: 1 byte(混音组Id)+ TMixMember
OSPEVENT( MIXER_MCU_ADDMEMBER_ACK,			EV_MCUMIXER_BGN + 21 );
//加入成员应答消息，MIXER->MCU，无消息体
OSPEVENT( MIXER_MCU_ADDMEMBER_NACK,			EV_MCUMIXER_BGN + 22 );


//删除成员请求，MCU->MIXER，消息体:1 byte(混音组Id)+ TMixMember	
//  m_tMember, m_byGrpId 有效
OSPEVENT( MCU_MIXER_REMOVEMEMBER_REQ,		EV_MCUMIXER_BGN + 25 );
//删除成员应答消息，MIXER->MCU，消息体: 1 byte(混音组Id)+ TMixMember
OSPEVENT( MIXER_MCU_REMOVEMEMBER_ACK,		EV_MCUMIXER_BGN + 26 );
//删除成员应答消息，MIXER->MCU，无消息体
OSPEVENT( MIXER_MCU_REMOVEMEMBER_NACK,		EV_MCUMIXER_BGN + 27 );

//混音激励成员改变通知,MIXER->MCU ,消息体 u8    abyActiveMmb[MAXNUM_MIXER_GROUP]
//0表示无效成员
OSPEVENT( MIXER_MCU_ACTIVEMMBCHANGE_NOTIF,	EV_MCUMIXER_BGN + 30 );

//设置混音成员，MCU->MIXER，消息体:1 byte(混音组Id)+ 192 byte(每字节对应一终端)	
OSPEVENT( MCU_MIXER_SETMEMBER_CMD,			EV_MCUMIXER_BGN + 32 );

//强制成员混音 ,MCU->MIXER，消息体:1 byte(混音组Id)+ TMixMember
OSPEVENT( MCU_MIXER_FORCEACTIVE_REQ,		EV_MCUMIXER_BGN + 35 );
//强制成员混音应答，MIXER->MCU，消息体:1 byte(混音组Id)+ TMixMember
OSPEVENT( MIXER_MCU_FORCEACTIVE_ACK,		EV_MCUMIXER_BGN + 36 );
//强制成员混音拒绝，MIXER->MCU，无消息体
OSPEVENT( MIXER_MCU_FORCEACTIVE_NACK,		EV_MCUMIXER_BGN + 37 );

//取消成员强制混音 ,MCU->MIXER，消息体:1 byte(混音组Id)+ TMixMember
OSPEVENT( MCU_MIXER_CANCELFORCEACTIVE_REQ,	EV_MCUMIXER_BGN + 40 );
//取消成员强制混音应答，MIXER->MCU，消息体:1 byte(混音组Id)+ TMixMember
OSPEVENT( MIXER_MCU_CANCELFORCEACTIVE_ACK,	EV_MCUMIXER_BGN + 41 );
//取消成员强制混音拒绝，MIXER->MCU，无消息体
OSPEVENT( MIXER_MCU_CANCELFORCEACTIVE_NACK,	EV_MCUMIXER_BGN + 42 );

//设置某通道音量消息，MCU->MIXER，消息体1 byte(混音组Id)+ TMixMember
//  m_tMember, m_byGrpId ,m_byVolume 有效
OSPEVENT( MCU_MIXER_SETCHNNLVOL_CMD,		EV_MCUMIXER_BGN + 45 );
//某通道音量通知消息，MIXER->MCU，消息体为TMixMember
OSPEVENT( MIXER_MCU_CHNNLVOL_NOTIF,			EV_MCUMIXER_BGN + 50 );

//设置混音深度请求消息，MCU->MIX，消息体: 1 byte(混音组Id) + 1 byte(混音深度)
OSPEVENT( MCU_MIXER_SETMIXDEPTH_REQ,		EV_MCUMIXER_BGN + 55 );
//设置混音深度应答消息，MIXER->MCU，消息体: 1 byte(混音组Id) + 1 byte(混音深度)
OSPEVENT( MIXER_MCU_SETMIXDEPTH_ACK,		EV_MCUMIXER_BGN + 56 );
//设置混音深度拒绝消息，MIXER->MCU，
OSPEVENT( MIXER_MCU_SETMIXDEPTH_NACK,		EV_MCUMIXER_BGN + 57 );

//MCU给混音器的是否要发送码流通知,消息体: 1 byte(混音组Id) + 1 byte(0:不发,1发)
OSPEVENT( MCU_MIXER_SEND_NOTIFY,			EV_MCUMIXER_BGN + 60 );

//设置语音激励延时保护时长消息, MCU->MIXER, 消息体: 1 byte(混音组Id) + 4 byte(u32 保护时长 网络序)
OSPEVENT( MCU_MIXER_VACKEEPTIME_CMD,		EV_MCUMIXER_BGN + 65 );

//混音延时设置,消息体：u8（混音组id） ＋ u16（毫秒）
OSPEVENT( MCU_MIXER_CHANGEMIXDELAY_CMD,		EV_MCUMIXER_BGN + 66 );

// stop mixer map, zgc, 2007/04/24
OSPEVENT( MCU_MIXER_STOPEQP_CMD,			EV_MCUMIXER_BGN + 67 );
// start mixer map, zgc, 2007/04/26
OSPEVENT( MCU_MIXER_STARTEQP_CMD,			EV_MCUMIXER_BGN + 68 );

//tianzhiyong 2010/05/19  增加开启和关闭EAPU混音器语音激励功能命令
//开启EAPU混音器语音激励功能命令，MCU->MIXER，
//消息体: 单开语音激励时为TMixerStart+TDoublePayload+TCapSupportEx 否则为空
OSPEVENT( MCU_MIXER_STARTVAC_CMD,		EV_MCUMIXER_BGN + 69 );
//关闭EAPU混音器语音激励功能命令，MCU->MIXER，消息体: 空
OSPEVENT( MCU_MIXER_STOPVAC_CMD,		EV_MCUMIXER_BGN + 70 );
/*----------  MCU和混音器消息号定义结束(26301-26400)  -----------*/


/*---------  MCU和TVWALL间业务消息号定义(26401-26440)  ----------*/

//电视墙向MCU发出注册请求，PERIEQP->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( TVWALL_MCU_REG_REQ,				EV_MCUTVWALL_BGN );
//MCU接受应答，MCU->PERIEQP，无消息体
OSPEVENT( MCU_TVWALL_REG_ACK,				EV_MCUTVWALL_BGN + 1 );
//MCU拒绝应答，MCU->PERIEQP，无消息体
OSPEVENT( MCU_TVWALL_REG_NACK,				EV_MCUTVWALL_BGN + 2 );

//MCU向外设发出开始播放视频图像命令，其中通道
//索引设置在消息头中，MCU->PERIEQP，
//    消息体为TTvWallStartPlay + TDoublePayload(video) + TDoublePayload(audio) + TCapSupportEx
OSPEVENT( MCU_TVWALL_START_PLAY_REQ,		EV_MCUTVWALL_BGN + 5 );
//电视墙接受应答，PERIEQP->MCU，无消息体
OSPEVENT( TVWALL_MCU_START_PLAY_ACK,		EV_MCUTVWALL_BGN + 6 );
//电视墙拒绝应答，PERIEQP->MCU，无消息体	
OSPEVENT( TVWALL_MCU_START_PLAY_NACK,		EV_MCUTVWALL_BGN + 7 );
//跨MCU音频或视频数据改变通知，MCU->PERIEQP，消息体为TSwitchInfo
//OSPEVENT( MCU_PERIEQP_MCUSWITCH_NOTIF,	EV_MCUTVWALL_BGN + 8 );

//MCU向电视墙发出停止播放视频图像命令，其中通道
//索引设置在消息头中，MCU->PERIEQP，无消息体
OSPEVENT( MCU_TVWALL_STOP_PLAY_REQ,			EV_MCUTVWALL_BGN + 10 );
//电视墙接受应答，PERIEQP->MCU，无消息体
OSPEVENT( TVWALL_MCU_STOP_PLAY_ACK,			EV_MCUTVWALL_BGN + 11 );
//电视墙拒绝应答，PERIEQP->MCU，无消息体
OSPEVENT( TVWALL_MCU_STOP_PLAY_NACK,		EV_MCUTVWALL_BGN + 12 );


//MCU发给电视墙查询状态请求，MCU->PERIEQP，无消息体
OSPEVENT( MCU_TVWALL_GETSTATUS_REQ,			EV_MCUTVWALL_BGN + 15 );	
//成功应答，PERIEQP->MCU，消息体为TPeriEqpStatus
OSPEVENT( TVWALL_MCU_GETSTATUS_ACK,			EV_MCUTVWALL_BGN + 16 );
//拒绝应答，PERIEQP->MCU，无消息体
OSPEVENT( TVWALL_MCU_GETSTATUS_NACK,		EV_MCUTVWALL_BGN + 17 );
//电视墙状态改变通知，PERIEQP->MCU，消息体为TPeriEqpStatus
OSPEVENT( TVWALL_MCU_STATUS_NOTIF,			EV_MCUTVWALL_BGN + 18 );

/*---------- MCU和TVWALL业务消息定义结束 (26401-26440) ----------*/

/*--------  MCU和HDU业务消息号定义 (26441-26500) --------------*/
//HDU注册
OSPEVENT( HDU_MCU_REG_REQ,			        EV_MCUHDU_BGN );
OSPEVENT( MCU_HDU_REG_ACK,			        EV_MCUHDU_BGN + 1 );
OSPEVENT( MCU_HDU_REG_NACK,			        EV_MCUHDU_BGN + 2 );

//通道开始播放
OSPEVENT( MCU_HDU_START_PLAY_REQ,		    EV_MCUHDU_BGN + 3 );
OSPEVENT( HDU_MCU_START_PLAY_ACK,		    EV_MCUHDU_BGN + 4 );
OSPEVENT( HDU_MCU_START_PLAY_NACK,		    EV_MCUHDU_BGN + 5 );

//通道停止播放
OSPEVENT( MCU_HDU_STOP_PLAY_REQ,		    EV_MCUHDU_BGN + 6 );
OSPEVENT( HDU_MCU_STOP_PLAY_ACK,		    EV_MCUHDU_BGN + 7 );
OSPEVENT( HDU_MCU_STOP_PLAY_NACK,		    EV_MCUHDU_BGN + 8 );

//请求外设状态
OSPEVENT( MCU_HDU_GETSTATUS_REQ,		    EV_MCUHDU_BGN + 9 );
OSPEVENT( HDU_MCU_GETSTATUS_ACK,		    EV_MCUHDU_BGN + 10 );
OSPEVENT( HDU_MCU_GETSTATUS_NACK,		    EV_MCUHDU_BGN + 11 );
OSPEVENT( HDU_MCU_STATUS_NOTIF,			    EV_MCUHDU_BGN + 12 );

//--------请求关键帧-------
OSPEVENT( HDU_MCU_NEEDIFRAME_CMD,		    EV_MCUHDU_BGN + 13 );
//--------调节音量---------
//消息体：FIXME。涉及chnlid,vol
OSPEVENT( MCU_HDU_SETCHNNLVOL_CMD,			EV_MCUHDU_BGN + 14 );
//某通道音量通知消息
OSPEVENT( HDU_MCU_CHNNLVOL_NOTIF,			EV_MCUHDU_BGN + 15 );
//通道状态通知
OSPEVENT( HDU_MCU_CHNNLSTATUS_NOTIF,		EV_MCUHDU_BGN + 16 ); 

//改变输入输出制式及显示比例通知
OSPEVENT( MCU_HDU_CHANGEMODEPORT_NOTIF,		EV_MCUHDU_BGN + 17 ); 

// 设置音量大小命令
OSPEVENT( MCU_HDU_CHANGEVOLUME_CMD,		    EV_MCUHDU_BGN + 19 ); 

// 变更模式
OSPEVENT( MCU_HDU_CHANGEMODE_CMD,		    EV_MCUHDU_BGN + 20 ); 

//通道空闲显示策略变更
OSPEVENT( MCU_HDU_CHANGEPLAYPOLICY_NOTIF,   EV_MCUHDU_BGN + 21 ); 


//HDU通道模式切换(多画面或单通道)
OSPEVENT( MCU_HDU_CHGHDUVMPMODE_REQ,		EV_MCUHDU_BGN + 22 ); 
OSPEVENT( HDU_MCU_CHGHDUVMPMODE_ACK,		EV_MCUHDU_BGN + 23 ); 
OSPEVENT( HDU_MCU_CHGHDUVMPMODE_NACK,		EV_MCUHDU_BGN + 24 ); 

//刷新音频载荷
OSPEVENT( MCU_HDU_FRESHAUDIO_PAYLOAD_CMD, 	EV_MCUHDU_BGN + 25 );  


/*----------MCU和HDU业务消息定义结束 (26441-26500) ------------*/

/*--------  MCU和DCS会话业务消息号定义 (26501-26600) -----------*/

//MCU请求DCS创建会议，MCU->DCSSSN，消息体为 TDataConfInfo
OSPEVENT( MCU_DCSSSN_CREATECONF_REQ,		EV_MCUDCS_BGN  );
//DCSSSN 创建会议同意应答，DCSSSN->MCU，消息体为 NULL
OSPEVENT( DCSSSN_MCU_CREATECONF_ACK,		EV_MCUDCS_BGN + 1 );
//DCSSSN 创建会议拒绝应答，DCSSSN->MCU，消息体为 u8(byErrorId)
OSPEVENT( DCSSSN_MCU_CREATECONF_NACK,		EV_MCUDCS_BGN + 2 );
//DCSSSN 增量发送会议通知,DCSSSN->MCU,消息体为 TDataConfInfo
OSPEVENT( DCSSSN_MCU_CONFCREATED_NOTIF,		EV_MCUDCS_BGN +	3 );

//MCU请求DCS结束会议，MCU->DCSSSN，消息体为 NULL
OSPEVENT( MCU_DCSSSN_RELEASECONF_REQ,		EV_MCUDCS_BGN +	4 );
//DCSSSN 结束会议同意应答，DCSSSN->MCU，消息体为 NULL
OSPEVENT( DCSSSN_MCU_RELEASECONF_ACK,		EV_MCUDCS_BGN + 5 );
//DCSSSN 结束会议拒绝应答，DCSSSN->MCU，消息体为 u8(byErrorId)
OSPEVENT( DCSSSN_MCU_RELEASECONF_NACK,		EV_MCUDCS_BGN + 6 );
//DCSSSN 增量发送会议结束通知,DCSSSN->MCU,消息体为 NULL
OSPEVENT( DCSSSN_MCU_CONFRELEASED_NOTIF,	EV_MCUDCS_BGN +	7 );


//MCU请求DCS增加终端，MCU->DCSSSN，消息体为 TDataMtInfo
OSPEVENT( MCU_DCSSSN_ADDMT_REQ,				EV_MCUDCS_BGN +	10 );
//DCSSSN 增加终端同意应答，DCSSSN->MCU，消息体为 u8(byMtId)
OSPEVENT( DCSSSN_MCU_ADDMT_ACK,				EV_MCUDCS_BGN + 11 );
//DCSSSN 增加终端拒绝应答，DCSSSN->MCU，消息体为 u8(byMtId) + u8(byErrorId)
OSPEVENT( DCSSSN_MCU_ADDMT_NACK,			EV_MCUDCS_BGN + 12 );
//DCSSSN 终端加入通知，DCSSSN->MCU，消息体为 TDataMtInfo
OSPEVENT( DCSSSN_MCU_MTJOINED_NOTIF,		EV_MCUDCS_BGN + 13 );


//MCU请求DCS删除终端，MCU->DCSSSN，消息体为 u8(byMtId)
OSPEVENT( MCU_DCSSSN_DELMT_REQ,				EV_MCUDCS_BGN +	14 );
//DCSSSN 删除终端同意应答，DCSSSN->MCU，消息体为 u8(byMtId)
OSPEVENT( DCSSSN_MCU_DELMT_ACK,				EV_MCUDCS_BGN + 15 );
//DCSSSN 删除终端拒绝应答，DCSSSN->MCU，消息体为 u8(byMtId) + u8(byErrorId)
OSPEVENT( DCSSSN_MCU_DELMT_NACK,			EV_MCUDCS_BGN + 16 );
//DCSSSN 终端离会(主动, 或是被删除)通知，DCSSSN->MCU，消息体为 u8(byMtId)
OSPEVENT( DCSSSN_MCU_MTLEFT_NOTIF,			EV_MCUDCS_BGN + 17 );

/*------    MCU和DCS会话业务消息号定义结束 (26501-26600)   ------*/


/*----------    MCU和BAS业务消息号定义 (26601-26700)   ----------*/

//码率适配器向MCU发出注册请求，BAS->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( BAS_MCU_REG_REQ,					EV_MCUBAS_BGN );
//MCU接受应答，MCU->BAS，消息体为消息体为TPeriEqpRegAck + TPrsTimeSpan + u16(MTU size)
OSPEVENT( MCU_BAS_REG_ACK,					EV_MCUBAS_BGN + 1 );
//MCU拒绝应答，MCU->BAS，无消息体
OSPEVENT( MCU_BAS_REG_NACK,					EV_MCUBAS_BGN + 2 );
//适配器状态通知，BAS->MCU，消息体为TPeriEqpStatus
OSPEVENT( BAS_MCU_BASSTATUS_NOTIF,			EV_MCUBAS_BGN + 3 );

//MCU启动适配请求，MCU->BAS，消息体为TAdaptParam+TMediaEncrypt(video)+TDoublePayload(video)+
//                       TMediaEncrypt(audio)+TDoublePayload(Audio)结构+TCapSupportEx,
//     业务消息的通道索引号指明通道 1到 MAXNUM_RTPHDR_CHNNL 为RTP头重组通道，其他为适配通道
//apu2 音频适配请求时，消息体为TAudAdaptParam*byOutNum+TMediaEncrypt+TDoublePayload
OSPEVENT( MCU_BAS_STARTADAPT_REQ,			EV_MCUBAS_BGN + 5 );
//启动适配应答消息，BAS->MCU，消息体: TEqp
OSPEVENT( BAS_MCU_STARTADAPT_ACK,			EV_MCUBAS_BGN + 6 );
//启动适配应答消息，BAS->MCU，消息体: TEqp
OSPEVENT( BAS_MCU_STARTADAPT_NACK,			EV_MCUBAS_BGN + 7 );

//MCU停止适配命令，MCU->BAS，无消息体，业务消息的通道索引号指明通道
OSPEVENT( MCU_BAS_STOPADAPT_REQ,			EV_MCUBAS_BGN + 10 );
//停止适配应答消息，BAS->MCU，无消息体
OSPEVENT( BAS_MCU_STOPADAPT_ACK,			EV_MCUBAS_BGN + 11 );
//停止适配应答消息，BAS->MCU，无消息体
OSPEVENT( BAS_MCU_STOPADAPT_NACK,			EV_MCUBAS_BGN + 12 );

//MCU改变适配参数请求，MCU->BAS，消息体为TAdaptParam结构,业务消息的通道索引号指明通道
//TAdaptParam 中的音视频类型无效
OSPEVENT( MCU_BAS_SETADAPTPARAM_CMD,		EV_MCUBAS_BGN + 15 );
OSPEVENT( MCU_BAS_CHANGEAUDPARAM_REQ,		EV_MCUBAS_BGN + 16 );
OSPEVENT( BAS_MCU_CHANGEAUDPARAM_ACK,		EV_MCUBAS_BGN + 17 );
OSPEVENT( BAS_MCU_CHANGEAUDPARAM_NACK,		EV_MCUBAS_BGN + 18 );
//码流适配器通道状态通知，BAS-> MCU ，消息体BYTE(外设ID) + TBasChnStatus,业务消息的通道索引号指明通道
OSPEVENT( BAS_MCU_CHNNLSTATUS_NOTIF,		EV_MCUBAS_BGN + 20 );

OSPEVENT( MCU_BAS_FASTUPDATEPIC_CMD,		EV_MCUBAS_BGN + 25 );
OSPEVENT( BAS_MCU_NEEDIFRAME_CMD,			EV_MCUBAS_BGN + 26 );

// 高清BAS消息, zgc, 2008-08-02
//MCU启动高清适配请求，MCU->HDBAS
//消息体：THDAdaptParam+THDAdaptParam+TMediaEncrypt(video)+TDoublePayload(video)
//        业务消息的通道索引号指明通道, 对于双流通道，两个TAdaptParam完全一致
OSPEVENT( MCU_HDBAS_STARTADAPT_REQ,         EV_MCUBAS_BGN + 30 );
//启动适配应答消息，HDBAS->MCU，消息体: TEqp, 业务消息的通道索引号指明通道
OSPEVENT( HDBAS_MCU_STARTADAPT_ACK,         EV_MCUBAS_BGN + 31 );
//启动适配应答消息，HDBAS->MCU，消息体: TEqp, 业务消息的通道索引号指明通道
OSPEVENT( HDBAS_MCU_STARTADAPT_NACK,        EV_MCUBAS_BGN + 32 );
//适配器状态通知，HDBAS->MCU，消息体: TPeriEqpStatus
OSPEVENT( HDBAS_MCU_BASSTATUS_NOTIF,		EV_MCUBAS_BGN + 33 );

//MCU停止适配命令，MCU->HDBAS，无消息体，业务消息的通道索引号指明通道
OSPEVENT( MCU_HDBAS_STOPADAPT_REQ,          EV_MCUBAS_BGN + 35 );
//停止适配应答消息，HDBAS->MCU，无消息体
OSPEVENT( HDBAS_MCU_STOPADAPT_ACK,          EV_MCUBAS_BGN + 36 );
//停止适配应答消息，HDBAS->MCU，无消息体
OSPEVENT( HDBAS_MCU_STOPADAPT_NACK,         EV_MCUBAS_BGN + 37 );

//消息体：THDAdaptParam+THDAdaptParam+TMediaEncrypt(video)+TDoublePayload(video)
//        业务消息的通道索引号指明通道, 对于双流通道，两个TAdaptParam完全一致
OSPEVENT( MCU_HDBAS_SETADAPTPARAM_CMD,		EV_MCUBAS_BGN + 40 );
//码流适配器通道状态通知，HDBAS-> MCU ，消息体: BYTE(通道类型) + THDBasVidChnStatus/THDBasDVidChnStatus,业务消息的通道索引号指明通道
OSPEVENT( HDBAS_MCU_CHNNLSTATUS_NOTIF,		EV_MCUBAS_BGN + 45 );

//音频适配器通道状态通知 AUDBAS->MCU，消息：u8(外设类型：APU2_BAS_CHN) + TAudBasChnStatus
OSPEVENT( AUDBAS_MCU_CHNNLSTATUS_NOTIF,		EV_MCUBAS_BGN + 46 );

//HDBas启动或参数修改
//消息体：THDAdaptParam+THDAdaptParam+TMediaEncrypt(video)+TDoublePayload(video)
//        业务消息的通道索引号指明通道, 对于双流通道，两个TAdaptParam完全一致
OSPEVENT( MCU_HDBAS_CHANGEPARAM_REQ,		EV_MCUBAS_BGN + 50 );
OSPEVENT( MCU_HDBAS_CHANGEPARAM_ACK,		EV_MCUBAS_BGN + 51 );
OSPEVENT( MCU_HDBAS_CHANGEPARAM_NACK,		EV_MCUBAS_BGN + 52 );

/*---------- MCU和BAS业务消息定义结束 (26601-26700) ----------*/


/*----------    以下为画面复合器消息(26701-26800)   ----------*/

//VMP向MCU注册请求，VMP->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( VMP_MCU_REGISTER_REQ,				EV_MCUVMP_BGN );
//MCU给VMP注册确认,MCU->VMP，消息体为TPeriEqpRegAck + TPrsTimeSpan + u16(MTU size)　
OSPEVENT( MCU_VMP_REGISTER_ACK,				EV_MCUVMP_BGN + 1 );
//MCU给VMP注册拒绝,MCU->VMP，无消息体
OSPEVENT( MCU_VMP_REGISTER_NACK,			EV_MCUVMP_BGN + 2 );
//VMP给MCU的状态上报,VMP->MCU，消息体:TPeriEqpStatus
OSPEVENT( VMP_MCU_VMPSTATUS_NOTIF,			EV_MCUVMP_BGN + 3 );

//MCU设置画面合成风格, 消息体:TVmpStyleCfgInfo
OSPEVENT( MCU_VMP_SETSTYLE_CMD,				EV_MCUVMP_BGN + 4 );
//MCU命令VMP停止MAP, 消息体: 无, zgc, 2007/04/24
OSPEVENT( MCU_VMP_STOPMAP_CMD,				EV_MCUVMP_BGN + 5 );
//MCU命令外设修改发送IP地址, 消息体：u32
OSPEVENT( MCU_EQP_MODSENDADDR_CMD,          EV_MCUVMP_BGN + 6 );

//MCU请求VMP开始工作,MCU->VMP，消息体为CKDVVMPParam+CKDVVMPParam+u8(是否prs)+TVmpStyleCfgInfo+TCapSupportEx
OSPEVENT( MCU_VMP_STARTVIDMIX_REQ,			EV_MCUVMP_BGN + 11 );
//VMP给MCU开始工作确认,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_STARTVIDMIX_ACK,			EV_MCUVMP_BGN + 12 );
//VMP给MCU开始工作拒绝,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_STARTVIDMIX_NACK,			EV_MCUVMP_BGN + 13 );
//VMP给MCU的开始结果，VMP->MCU,消息体空
OSPEVENT( VMP_MCU_STARTVIDMIX_NOTIF,		EV_MCUVMP_BGN + 14 );

//MCU请求VMP停止工作,MCU->VMP，无消息体
OSPEVENT( MCU_VMP_STOPVIDMIX_REQ,			EV_MCUVMP_BGN + 21 );
//VMP给MCU停止工作确认,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_STOPVIDMIX_ACK,			EV_MCUVMP_BGN + 22 );
//VMP给MCU停止工作拒绝,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_STOPVIDMIX_NACK,			EV_MCUVMP_BGN + 23 );
//VMP给MCU的停止结果，VMP->MCU,消息体空
OSPEVENT( VMP_MCU_STOPVIDMIX_NOTIF,			EV_MCUVMP_BGN + 24 );

//MCU请求VMP改变复合参数,MCU->VMP，消息体为CKDVVMPParam+CKDVVMPParam+u8(是否prs)+TVmpStyleCfgInfo
OSPEVENT( MCU_VMP_CHANGEVIDMIXPARAM_CMD,	EV_MCUVMP_BGN + 31 );
//VMP给MCU改变复合参数确认,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_CHANGEVIDMIXPARAM_ACK,	EV_MCUVMP_BGN + 32 );
//VMP给MCU改变复合参数拒绝,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_CHANGEVIDMIXPARAM_NACK,	EV_MCUVMP_BGN + 33 );
//VMP给MCU的状态改变结果，VMP->MCU,消息体CKDVVMPParam结构
OSPEVENT( VMP_MCU_CHANGESTATUS_NOTIF,		EV_MCUVMP_BGN + 34 );

OSPEVENT( MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ,EV_MCUVMP_BGN + 35 );

OSPEVENT( MCU_VMP_FASTUPDATEPIC_CMD,		EV_MCUVMP_BGN + 36 );

//MCU对合成通道LOG的操作命令, 消息体：u8( 0 - 取消LOG， 1 - 增加LOG )
OSPEVENT( MCU_VMP_CHNNLLOGOPR_CMD,          EV_MCUVMP_BGN + 37 );
//MCU 给VMP某通道设置台标命令， 消息体：[s8 * aliasname + byChlIdx] * 通道数
OSPEVENT( MCU_VMP_CHANGEMEMALIAS_CMD,		EV_MCUVMP_BGN + 38 );

//MCU请求VMP回送复合参数,MCU->VMP，无消息体
OSPEVENT( MCU_VMP_GETVIDMIXPARAM_REQ,		EV_MCUVMP_BGN + 41 );
//VMP给MCU回送复合参数确认,VMP->MCU，消息体为CKDVVMPParam结构
OSPEVENT( VMP_MCU_GETVIDMIXPARAM_ACK,		EV_MCUVMP_BGN + 42 );
//VMP给MCU回送复合参数拒绝,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_GETVIDMIXPARAM_NACK,		EV_MCUVMP_BGN + 43 );
//启动VMP初始化
OSPEVENT( EV_VMP_INIT,	  	  				EV_MCUVMP_BGN + 50 );
//显示所有的统计和状态
OSPEVENT( EV_VMP_DISPLAYALL,	  			EV_MCUVMP_BGN + 60 );
//显示VMP内部的合成参数
OSPEVENT( EV_VMP_SHOWPARAM,					EV_MCUVMP_BGN + 61 );
//MCU请求VMP回送复合参数,MCU->VMP，消息体u16的码率，CServMsg.SetChnIndex()[1 or 2]
OSPEVENT( MCU_VMP_SETCHANNELBITRATE_REQ,	EV_MCUVMP_BGN + 65 );
//VMP给MCU回送复合参数确认,VMP->MCU，消息体为CKDVVMPParam结构
OSPEVENT( VMP_MCU_SETCHANNELBITRATE_ACK,	EV_MCUVMP_BGN + 66 );
//VMP给MCU回送复合参数拒绝,VMP->MCU，无消息体
OSPEVENT( VMP_MCU_SETCHANNELBITRATE_NACK,	EV_MCUVMP_BGN + 67 );
//请求I帧,CServMsg.GetChnIndex为请求的通道号
OSPEVENT( VMP_MCU_NEEDIFRAME_CMD,			EV_MCUVMP_BGN + 70 );
//备板输出是否启用命令，VMP内部使用，消息体：u8: 0不启用，1启用
OSPEVENT( EV_VMP_BACKBOARD_OUT_CMD,         EV_MCUVMP_BGN + 71 );
//设置VMP帧率
OSPEVENT( EV_VMP_SETFRAMERATE_CMD,			EV_MCUVMP_BGN + 72 );
//MCU通知VMP增加或删除通道(伴随起停相应通道的接收)命令，消息体：u8: channelidx, u8 byAdd
OSPEVENT( MCU_VMP_ADDREMOVECHNNL_CMD,		EV_MCUVMP_BGN + 73 );
//MCU通知VMP停或开某一输出通道命令，消息体：u8 chnnelIdx,u8 byActive
OSPEVENT( MCU_VMP_STARTSTOPCHNNL_CMD,		EV_MCUVMP_BGN + 74 );

/*----------    画面复合器消息定义结束(26701-26800)  ----------*/


/*----------     以下为复合电视墙消息(26701-26800)   ----------*/

//VMP向MCU注册请求，VMP->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT(VMPTW_MCU_REGISTER_REQ,			EV_MCUVMPTW_BGN);
//MCU给VMP注册确认,MCU->VMP，无消息体
OSPEVENT(MCU_VMPTW_REGISTER_ACK,            EV_MCUVMPTW_BGN + 1 );
//MCU给VMP注册拒绝,MCU->VMP，无消息体
OSPEVENT(MCU_VMPTW_REGISTER_NACK,           EV_MCUVMPTW_BGN + 2 );
//VMP给MCU的状态上报,VMP->MCU，消息体:TPeriEqpStatus
OSPEVENT(VMPTW_MCU_VMPTWSTATUS_NOTIF,       EV_MCUVMPTW_BGN + 3 );

//MCU请求VMP开始工作,MCU->VMP，消息体为CKDVVMPParam结构+u8(needprs)+TCapSupportEx
OSPEVENT(MCU_VMPTW_STARTVIDMIX_REQ,         EV_MCUVMPTW_BGN + 11 );
//VMP给MCU开始工作确认,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_STARTVIDMIX_ACK,         EV_MCUVMPTW_BGN + 12 );
//VMP给MCU开始工作拒绝,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_STARTVIDMIX_NACK,        EV_MCUVMPTW_BGN + 13 );
//VMP给MCU的开始结果，VMP->MCU,消息体空
OSPEVENT(VMPTW_MCU_STARTVIDMIX_NOTIF,       EV_MCUVMPTW_BGN + 14 );

//MCU请求VMP停止工作,MCU->VMP，无消息体
OSPEVENT(MCU_VMPTW_STOPVIDMIX_REQ,          EV_MCUVMPTW_BGN + 21 );
//VMP给MCU停止工作确认,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_STOPVIDMIX_ACK,          EV_MCUVMPTW_BGN + 22 );
//VMP给MCU停止工作拒绝,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_STOPVIDMIX_NACK,         EV_MCUVMPTW_BGN + 23 );
//VMP给MCU的停止结果，VMP->MCU,消息体空
OSPEVENT(VMPTW_MCU_STOPVIDMIX_NOTIF,        EV_MCUVMPTW_BGN + 24 );

//MCU请求VMP改变复合参数,MCU->VMP，消息体为CKDVVMPParam结构
OSPEVENT(MCU_VMPTW_CHANGEVIDMIXPARAM_REQ,   EV_MCUVMPTW_BGN + 31 );
//VMP给MCU改变复合参数确认,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_CHANGEVIDMIXPARAM_ACK,   EV_MCUVMPTW_BGN + 32 );
//VMP给MCU改变复合参数拒绝,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_CHANGEVIDMIXPARAM_NACK,  EV_MCUVMPTW_BGN + 33 );
//VMP给MCU的状态改变结果，VMP->MCU,消息体CKDVVMPParam结构
OSPEVENT(VMPTW_MCU_CHANGESTATUS_NOTIF,      EV_MCUVMPTW_BGN + 34 );

OSPEVENT(MCU_VMPTW_UPDATAVMPENCRYPTPARAM_REQ, EV_MCUVMPTW_BGN + 35 );

OSPEVENT(MCU_VMPTW_FASTUPDATEPIC_CMD,       EV_MCUVMPTW_BGN + 36 );

//MCU请求VMP回送复合参数,MCU->VMP，无消息体
OSPEVENT(MCU_VMPTW_GETVIDMIXPARAM_REQ,      EV_MCUVMPTW_BGN + 41 );
//VMP给MCU回送复合参数确认,VMP->MCU，消息体为CKDVVMPParam结构
OSPEVENT(VMPTW_MCU_GETVIDMIXPARAM_ACK,      EV_MCUVMPTW_BGN + 42 );
//VMP给MCU回送复合参数拒绝,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_GETVIDMIXPARAM_NACK,     EV_MCUVMPTW_BGN + 43 );
//启动VMP初始化
OSPEVENT(EV_VMPTW_INIT,                     EV_MCUVMPTW_BGN + 50 );
//显示所有的统计和状态
OSPEVENT(EV_VMPTW_DISPLAYALL,               EV_MCUVMPTW_BGN + 60 );
//MCU请求VMP回送复合参数,MCU->VMP，消息体u16的码率，CServMsg.SetChnIndex()[1 or 2]
OSPEVENT(MCU_VMPTW_SETCHANNELBITRATE_REQ,   EV_MCUVMPTW_BGN + 65 );
//VMP给MCU回送复合参数确认,VMP->MCU，消息体为CKDVVMPParam结构
OSPEVENT(VMPTW_MCU_SETCHANNELBITRATE_ACK,   EV_MCUVMPTW_BGN + 66 );
//VMP给MCU回送复合参数拒绝,VMP->MCU，无消息体
OSPEVENT(VMPTW_MCU_SETCHANNELBITRATE_NACK,  EV_MCUVMPTW_BGN + 67 );
//请求I帧,CServMsg.GetChnIndex为请求的通道号
OSPEVENT(VMPTW_MCU_NEEDIFRAME_CMD,          EV_MCUVMPTW_BGN + 70 );

/*----------     复合电视墙消息定义结束(26701-26800)   ----------*/


/*----------     以下为智能丢包恢复消息(26801-26900)   ----------*/

//PRS向MCU注册请求，PRS->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( PRS_MCU_REGISTER_REQ,				EV_MCUPRS_BGN );
//MCU给PRS注册确认,MCU->PRS，消息体TRegAck
OSPEVENT( MCU_PRS_REGISTER_ACK,				EV_MCUPRS_BGN + 1 );
//MCU给PRS注册拒绝,MCU->PRS，无消息体
OSPEVENT( MCU_PRS_REGISTER_NACK,			EV_MCUPRS_BGN + 2 );
//VMP给MCU的状态上报,PRS->MCU，消息体:TPeriEqpStatus
OSPEVENT( PRS_MCU_PRSSTATUS_NOTIF,			EV_MCUPRS_BGN + 3 );

//MCU请求PRS开始保存信息源,MCU->PRS，消息体TPrsParam，业务消息的通道索引号指明通道
OSPEVENT( MCU_PRS_SETSRC_REQ,				EV_MCUPRS_BGN + 5 );
//PRS给MCU保存信息源确认,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_SETSRC_ACK,				EV_MCUPRS_BGN + 6 );
//PRS给MCU保存信息源拒绝,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_SETSRC_NACK,				EV_MCUPRS_BGN + 7 );
//PRS给MCU的保存信息源结果，PRS->MCU,消息体TPeriEqpStatus
OSPEVENT( PRS_MCU_SETSRC_NOTIF,				EV_MCUPRS_BGN + 8 );

//MCU请求PRS添加一个重传通道,MCU->PRS，消息体TPrsParam，业务消息的通道索引号指明通道
OSPEVENT( MCU_PRS_ADDRESENDCH_REQ,			EV_MCUPRS_BGN + 10 );
//PRS给MCU停止工作确认,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_ADDRESENDCH_ACK,			EV_MCUPRS_BGN + 11 );
//PRS给MCU停止工作拒绝,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_ADDRESENDCH_NACK,			EV_MCUPRS_BGN + 12 );
//PRS给MCU的停止结果，PRS->MCU,消息体TPeriEqpStatus
OSPEVENT( PRS_MCU_ADDRESENDCH_NOTIF,		EV_MCUPRS_BGN + 13 );

//MCU请求PRS移出一个重传通道,MCU->PRS，消息体TPrsParam，业务消息的通道索引号指明通道
OSPEVENT( MCU_PRS_REMOVERESENDCH_REQ,		EV_MCUPRS_BGN + 15 );
//PRS给MCU改变复合参数确认,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_REMOVERESENDCH_ACK,		EV_MCUPRS_BGN + 16 );
//PRS给MCU改变复合参数拒绝,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_REMOVERESENDCH_NACK,		EV_MCUPRS_BGN + 17 );
//PRS给MCU的状态改变结果，PRS->MCU,消息体TPeriEqpStatus
OSPEVENT( PRS_MCU_REMOVERESENDCH_NOTIF,		EV_MCUPRS_BGN + 18 );

//MCU请求PRS移出所有的重传通道,MCU->PRS，无消息体，业务消息的通道索引号指明通道
OSPEVENT( MCU_PRS_REMOVEALL_REQ,			EV_MCUPRS_BGN + 20 );
//PRS给MCU改变复合参数确认,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_REMOVEALL_ACK,			EV_MCUPRS_BGN + 21 );
//PRS给MCU改变复合参数拒绝,PRS->MCU，无消息体
OSPEVENT( PRS_MCU_REMOVEALL_NACK,			EV_MCUPRS_BGN + 22 );
//PRS给MCU的状态改变结果，PRS->MCU,消息体TPeriEqpStatus
OSPEVENT( PRS_MCU_REMOVEALL_NOTIF,			EV_MCUPRS_BGN + 23 );

//启动PRS初始化
OSPEVENT( EV_PRS_INIT,	  	  				EV_MCUPRS_BGN + 25 );
//显示通道状态
OSPEVENT( EV_PRS_STATUS,	  	  			EV_MCUPRS_BGN + 30 );

/*----------   智能丢包恢复消息定义结束(26801-26900)  --------*/


/*----------     以下为文件编码器消息(26901-27000)    --------*/

//File Encoder向MCU注册请求，File Encoder->MCU，消息体为TPeriEqpRegReq结构
/*OSPEVENT( FILEENC_MCU_REGISTER_REQ,		EV_MCUFILEENC_BGN );
//MCU给File Encoder注册确认,MCU->File Encoder，消息体TPeriEqpRegAck
OSPEVENT( MCU_FILEENC_REGISTER_ACK,			EV_MCUFILEENC_BGN + 1 );
//MCU给File Encoder注册拒绝,MCU->File Encoder，无消息体
OSPEVENT( MCU_FILEENC_REGISTER_NACK,		EV_MCUFILEENC_BGN + 2 );


//文件编码器开始编码, MCU->File Encoder(Daemon), 消息体为 视频码流类型(1B)+音频码流类型(1B)+码率(2B 网络序)
OSPEVENT( MCU_FILEENC_STARTENC_REQ,			EV_MCUFILEENC_BGN + 4 );
//文件编码器开始编码应答, File Encoder -> MCU, 消息体为 使用的文件编码器实例号(1B)
OSPEVENT( MCU_FILEENC_STARTENC_ACK,			EV_MCUFILEENC_BGN + 5 );
//文件编码器开始编码否定应答, File Encoder -> MCU, 无消息体
OSPEVENT( MCU_FILEENC_STARTENC_NACK,		EV_MCUFILEENC_BGN + 6 );

//文件编码器停止编码, MCU->File Encoder(Daemon), 消息体为 文件编码器实例号(1B)
OSPEVENT( MCU_FILEENC_STOPENC_REQ,			EV_MCUFILEENC_BGN + 8 );
//文件编码器停止编码应答, File Encoder -> MCU, 无消息体
OSPEVENT( MCU_FILEENC_STOPENC_ACK,			EV_MCUFILEENC_BGN + 9 );
//文件编码器停止编码否定应答, File Encoder -> MCU, 无消息体
OSPEVENT( MCU_FILEENC_STOPENC_NACK,			EV_MCUFILEENC_BGN + 10 );

//File Encoder给MCU的状态上报,File Encoder->MCU，消息体:TPeriEqpStatus
OSPEVENT( FILEENC_MCU_FILEENCSTATUS_NOTIF,	EV_MCUFILEENC_BGN + 12 );

//内部消息
OSPEVENT( EV_FILEENC_SHOW,					EV_MCUFILEENC_BGN + 16 );
OSPEVENT( EV_FILEENC_INI,					EV_MCUFILEENC_BGN + 20 );
OSPEVENT( EV_FILEENC_CONNECT,				EV_MCUFILEENC_BGN + 24 );
OSPEVENT( EV_FILEENC_REGISTER,				EV_MCUFILEENC_BGN + 28 );
OSPEVENT( EV_FILEENC_REGTIMEOUT,			EV_MCUFILEENC_BGN + 32 );*/

/*------------     文件编码器消息定义结束(26901-27000)    ---------*/


/*-- 所有外设共用消息, 属于保留的前100个, 即(26001-26100)  --*/

//MCU给外设环路维护请求，消息体:u32序列号。(不需要CServMsg封装）
OSPEVENT( MCU_EQP_ROUNDTRIP_REQ,			EV_MCUEQP_BGN + 35 );
//消息体u32序列号。
OSPEVENT( EQP_MCU_ROUNDTRIP_ACK,			EV_MCUEQP_BGN + 40 );

// 设置qos，消息体：TMcuQosCfgInfo
OSPEVENT( MCU_EQP_SETQOS_CMD,				EV_MCUEQP_BGN + 30 );
// 外设取Mcu倒换是否成功
OSPEVENT( EQP_MCU_GETMSSTATUS_REQ,          EV_MCUEQP_BGN + 31 );
// Mcu 给外设倒换状态, 消息体：TMcuMsStatus
OSPEVENT( MCU_EQP_GETMSSTATUS_ACK,          EV_MCUEQP_BGN + 32 );
// Mcu 给外设倒换状态
OSPEVENT( MCU_EQP_GETMSSTATUS_NACK,         EV_MCUEQP_BGN + 33 );
// Mcu 通知外设建立交换成功
OSPEVENT( MCU_EQP_SWITCHSTART_NOTIF,        EV_MCUEQP_BGN + 34 );

// Mcu 通知外设重连 MPCB 板, zgc, 2007/04/30
OSPEVENT( MCU_EQP_RECONNECTB_CMD,			EV_MCUEQP_BGN + 36 );
/*---------- 所有外设共用消息定义结束(26001-26100)  ----------*/

/**********************************************************
 以下为MCU和外设之间业务接口的补充 (39601-40000)
**********************************************************/

/*----------    以下为MpuBas消息(39601-39650)   ----------*/

//MPUBAS向MCU发出注册请求，MPUBAS->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( MPUBAS_MCU_REG_REQ,			    EV_MCUMPUBAS_BGN );
//MCU接受应答，MCU->MPUBAS，消息体为消息体为TPeriEqpRegAck + TPrsTimeSpan + u16(MTU size)
OSPEVENT( MCU_MPUBAS_REG_ACK,				EV_MCUMPUBAS_BGN + 1 );
//MCU拒绝应答，MCU->MPUBAS，无消息体
OSPEVENT( MCU_MPUBAS_REG_NACK,				EV_MCUMPUBAS_BGN + 2 );

//适配器状态通知，MPUBAS->MCU，消息体为TPeriEqpStatus
OSPEVENT( MPUBAS_MCU_MPUBASSTATUS_NOTIF,	EV_MCUMPUBAS_BGN + 3 );
//码流适配器通道状态通知，MPUBAS->MCU ，消息体: THDBasVidChnStatus,业务消息的通道索引号指明通道
OSPEVENT( MPUBAS_MCU_CHNNLSTATUS_NOTIF,		EV_MCUMPUBAS_BGN + 4 );

//消息体：THDAdaptParam+THDAdaptParam+TMediaEncrypt(video)+TDoublePayload(video)，业务消息的通道索引号指明通道
OSPEVENT( MCU_MPUBAS_STARTADAPT_REQ,		EV_MCUMPUBAS_BGN + 5 );
OSPEVENT( MCU_MPUBAS_STARTADAPT_ACK,		EV_MCUMPUBAS_BGN + 6 );
OSPEVENT( MCU_MPUBAS_STARTADAPT_NACK,		EV_MCUMPUBAS_BGN + 7 );

//MCU停止适配命令，MCU->MPUBAS，无消息体，业务消息的通道索引号指明通道
OSPEVENT( MCU_MPUBAS_STOPADAPT_REQ,         EV_MCUMPUBAS_BGN + 8 );
//停止适配应答消息，MPUBAS->MCU，无消息体
OSPEVENT( MPUBAS_MCU_STOPADAPT_ACK,         EV_MCUMPUBAS_BGN + 9 );
//停止适配应答消息，MPUBAS->MCU，无消息体
OSPEVENT( MPUBAS_MCU_STOPADAPT_NACK,        EV_MCUMPUBAS_BGN + 10 );

//消息体：THDAdaptParam+THDAdaptParam+TMediaEncrypt(video)+TDoublePayload(video)，业务消息的通道索引号指明通道
OSPEVENT( MCU_MPUBAS_SETADAPTPARAM_CMD,		EV_MCUMPUBAS_BGN + 11 );

OSPEVENT( MCU_MPUBAS_FASTUPDATEPIC_CMD,		EV_MCUMPUBAS_BGN + 12 );
OSPEVENT( MPUBAS_MCU_NEEDIFRAME_CMD,		EV_MCUMPUBAS_BGN + 13 );

/*----------    MpuBas消息结束(39601-39650)   ----------*/

/*----------    以下为MpuEBap消息(39651-39700)   ----------*/
/*----------    MpuEBap消息结束(39651-39700)   ----------*/

/*----------    以下为MpuEVpu消息(39701-39800)   ----------*/
/*----------    MpuEVpu消息结束(39701-39800)   ----------*/

/*----------    以下为MpuSVmp消息(39801-39900)   ----------*/
/*----------    MpuSVmp消息结束(39801-39900)   ----------*/

/*----------    以下为MpuDVmp消息(39901-40000)   ----------*/
//VMP向MCU注册请求，VMP->MCU，消息体为TPeriEqpRegReq结构
OSPEVENT( MPUDVMP_MCU_REGISTER_REQ,				EV_MCUMPUDVMP_BGN );
//MCU给VMP注册确认,MCU->VMP，消息体为TPeriEqpRegAck + TPrsTimeSpan + u16(MTU size)　
OSPEVENT( MCU_MPUDVMP_REGISTER_ACK,				EV_MCUMPUDVMP_BGN + 1 );
//MCU给VMP注册拒绝,MCU->VMP，无消息体
OSPEVENT( MCU_MPUDVMP_REGISTER_NACK,			EV_MCUMPUDVMP_BGN + 2 );
//VMP给MCU的状态上报,VMP->MCU，消息体:TPeriEqpStatus
OSPEVENT( MPUDVMP_MCU_VMPSTATUS_NOTIF,			EV_MCUMPUDVMP_BGN + 3 );

//MCU设置画面合成风格, 消息体:TVmpStyleCfgInfo
OSPEVENT( MCU_MPUDVMP_SETSTYLE_CMD,				EV_MCUMPUDVMP_BGN + 4 );
//MCU命令VMP停止MAP, 消息体: 无, zgc, 2007/04/24
OSPEVENT( MCU_MPUDVMP_STOPMAP_CMD,				EV_MCUMPUDVMP_BGN + 5 );

//MCU请求VMP开始工作,MCU->VMP，消息体为CKDVVMPParam+CKDVVMPParam+u8(是否prs)+TVmpStyleCfgInfo+TCapSupportEx
OSPEVENT( MCU_MPUDVMP_STARTVIDMIX_REQ,			EV_MCUMPUDVMP_BGN + 11 );
//VMP给MCU开始工作确认,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_STARTVIDMIX_ACK,			EV_MCUMPUDVMP_BGN + 12 );
//VMP给MCU开始工作拒绝,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_STARTVIDMIX_NACK,			EV_MCUMPUDVMP_BGN + 13 );
//VMP给MCU的开始结果，VMP->MCU,消息体空
OSPEVENT( MPUDVMP_MCU_STARTVIDMIX_NOTIF,		EV_MCUMPUDVMP_BGN + 14 );

//MCU请求VMP停止工作,MCU->VMP，无消息体
OSPEVENT( MCU_MPUDVMP_STOPVIDMIX_REQ,			EV_MCUMPUDVMP_BGN + 21 );
//VMP给MCU停止工作确认,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_STOPVIDMIX_ACK,			EV_MCUMPUDVMP_BGN + 22 );
//VMP给MCU停止工作拒绝,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_STOPVIDMIX_NACK,			EV_MCUMPUDVMP_BGN + 23 );
//VMP给MCU的停止结果，VMP->MCU,消息体空
OSPEVENT( MPUDVMP_MCU_STOPVIDMIX_NOTIF,			EV_MCUMPUDVMP_BGN + 24 );

//MCU请求VMP改变复合参数,MCU->VMP，消息体为CKDVVMPParam+CKDVVMPParam+u8(是否prs)+TVmpStyleCfgInfo
OSPEVENT( MCU_MPUDVMP_CHANGEVIDMIXPARAM_REQ,	EV_MCUMPUDVMP_BGN + 31 );
//VMP给MCU改变复合参数确认,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_CHANGEVIDMIXPARAM_ACK,	EV_MCUMPUDVMP_BGN + 32 );
//VMP给MCU改变复合参数拒绝,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_CHANGEVIDMIXPARAM_NACK,	EV_MCUMPUDVMP_BGN + 33 );
//VMP给MCU的状态改变结果，VMP->MCU,消息体CKDVVMPParam结构
OSPEVENT( MPUDVMP_MCU_CHANGESTATUS_NOTIF,		EV_MCUMPUDVMP_BGN + 34 );

OSPEVENT( MCU_MPUDVMP_UPDATAVMPENCRYPTPARAM_REQ,EV_MCUMPUDVMP_BGN + 35 );

OSPEVENT( MCU_MPUDVMP_FASTUPDATEPIC_CMD,		EV_MCUMPUDVMP_BGN + 36 );

//MCU请求VMP回送复合参数,MCU->VMP，无消息体
OSPEVENT( MCU_MPUDVMP_GETVIDMIXPARAM_REQ,		EV_MCUMPUDVMP_BGN + 41 );
//VMP给MCU回送复合参数确认,VMP->MCU，消息体为CKDVVMPParam结构
OSPEVENT( MPUDVMP_MCU_GETVIDMIXPARAM_ACK,		EV_MCUMPUDVMP_BGN + 42 );
//VMP给MCU回送复合参数拒绝,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_GETVIDMIXPARAM_NACK,		EV_MCUMPUDVMP_BGN + 43 );
//启动VMP初始化
OSPEVENT( EV_MPUDVMP_INIT,	  	  				EV_MCUMPUDVMP_BGN + 50 );
//显示所有的统计和状态
OSPEVENT( EV_MPUDVMP_DISPLAYALL,	  			EV_MCUMPUDVMP_BGN + 60 );
//显示VMP内部的合成参数
OSPEVENT( EV_MPUDVMP_SHOWPARAM,					EV_MCUMPUDVMP_BGN + 61 );
//MCU请求VMP回送复合参数,MCU->VMP，消息体u16的码率，CServMsg.SetChnIndex()[1 or 2]
OSPEVENT( MCU_MPUDVMP_SETCHANNELBITRATE_REQ,	EV_MCUMPUDVMP_BGN + 65 );
//VMP给MCU回送复合参数确认,VMP->MCU，消息体为CKDVVMPParam结构
OSPEVENT( MPUDVMP_MCU_SETCHANNELBITRATE_ACK,	EV_MCUMPUDVMP_BGN + 66 );
//VMP给MCU回送复合参数拒绝,VMP->MCU，无消息体
OSPEVENT( MPUDVMP_MCU_SETCHANNELBITRATE_NACK,	EV_MCUMPUDVMP_BGN + 67 );
//请求I帧,CServMsg.GetChnIndex为请求的通道号
OSPEVENT( MPUDVMP_MCU_NEEDIFRAME_CMD,			EV_MCUMPUDVMP_BGN + 70 );
//备板输出是否启用命令，VMP内部使用，消息体：u8: 0不启用，1启用
OSPEVENT( EV_MPUDVMP_BACKBOARD_OUT_CMD,         EV_MCUMPUDVMP_BGN + 71 );
/*----------    MpuDVmp消息结束(39901-40000)   ----------*/

#endif /* _EV_MCUEQP_H_ */
