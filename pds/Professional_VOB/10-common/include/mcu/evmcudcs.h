/*****************************************************************************
   模块名      : MCU的T120集成
   文件名      : evmcudcs.h
   相关文件    : 
   文件实现功能: MCU和DCS接口消息定义
                 *_REQ: 需应答请求
				 *_ACK, *_NACK: 请求之答复
				 *_CMD: 不需应答命令
				 *_NOTIF: 状态通知
   作者        : 张宝卿
   版本        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/12/09  4.0         张宝卿      创建
******************************************************************************/
#ifndef _EV_MCUDCS_H_
#define _EV_MCUDCS_H_

#include "osp.h"
#include "eventid.h"

//注：为保证程序正确运行，所有成功和失败应答比原消息号分别加1，2

/**********************************************************
 以下为MCU(会话)和DCS业务接口 (38301-38500)
**********************************************************/

// ---MCU_DCS CONNECT (38301-38310)---
//MCU请求与DCServer建链，MCU->DCS
OSPEVENT( MCU_DCS_CONNECT_REQ,			EV_DCSMCU_BGN );	
//DCS与MCU建链成功，DCS->MCU
OSPEVENT( DCS_MCU_CONNECT_ACK,			EV_DCSMCU_BGN + 1 );
//DCS拒绝建链，DCS->MCU，无消息体
OSPEVENT( DCS_MCU_CONNECT_NACK,			EV_DCSMCU_BGN + 2 );

//MCU请求与DCServer断链，MCU->DCC
OSPEVENT( MCU_DCS_DISCONNECT_REQ,		EV_DCSMCU_BGN + 4 );
//DCS与MCU断链成功，DCS->MCU，无消息体
OSPEVENT( DCS_MCU_DISCONNECT_ACK,		EV_DCSMCU_BGN + 5 );
//DCS拒绝断链，DCS->MCU，无消息体
OSPEVENT( DCS_MCU_DISCONNECT_NACK,		EV_DCSMCU_BGN + 6);

// ---REGISTER (38311-38320)---
//DCS向MCU注册请求, DCS->MCU, 消息体为CDcsMcuRegReqPDU
OSPEVENT( DCS_MCU_REG_REQ,				EV_DCSMCU_BGN + 10 );
//MCU对DCS注册接受应答, MCU->DCS, 消息体为CMcuDcsRegAckPDU
OSPEVENT( MCU_DCS_REG_ACK,				EV_DCSMCU_BGN + 11 );
//MCU对DCS注册拒绝应答, MCU->DCS, 消息体为CMcuDcsRegNackPDU
OSPEVENT( MCU_DCS_REG_NACK,				EV_DCSMCU_BGN + 12 );

// ---MCU_DCS CONF (38321-38340)---
//MCU请求DCServer创建会议，MCU->DCS，消息体为CMcuDcsCreateConfReqPDU
OSPEVENT( MCU_DCS_CREATECONF_REQ,		EV_DCSMCU_BGN +	20 );
//DCS发送创建会议同意应答，DCS->MCU，消息体为CDcsMcuCreateConfAckPDU
OSPEVENT( DCS_MCU_CREATECONF_ACK,		EV_DCSMCU_BGN + 21 );
//DCS拒绝创建会议，DCS->MCU，消息体为CDcsMcuCreateConfNackPDU
OSPEVENT( DCS_MCU_CREATECONF_NACK,		EV_DCSMCU_BGN + 22 );
//DCS增量发送会议通知,DCS->MCU,消息体为CDcsMcuCreateConfNotifyPDU
OSPEVENT( DCS_MCU_CONFCREATED_NOTIF,	EV_DCSMCU_BGN +	23 );

//MCU请求DCServer结束会议，MCU->DCS，消息体为CMcuDcsReleaseConfReqPDU
OSPEVENT( MCU_DCS_RELEASECONF_REQ,		EV_DCSMCU_BGN +	24 );
//DCS发送结束会议同意应答，DCS->MCU，消息体为CDcsMcuReleaseConfAckPDU
OSPEVENT( DCS_MCU_RELEASECONF_ACK,		EV_DCSMCU_BGN + 25 );
//DCS拒绝结束会议，DCS->MCU，消息体为CDcsMcuReleaseConfNackPDU
OSPEVENT( DCS_MCU_RELEASECONF_NACK,		EV_DCSMCU_BGN + 26 );
//DCS增量发送会议结束通知,DCS->MCU,消息体为CDcsMcuReleaseConfNotifyPDU
OSPEVENT( DCS_MCU_CONFRELEASED_NOTIF,	EV_DCSMCU_BGN +	27 );

// ---MCU_DCS MT (38341-38350)---
//MCU请求DCServer增加终端，MCU->DCS，消息体为CMcuDcsAddMtReqPDU
OSPEVENT( MCU_DCS_ADDMT_REQ,			EV_DCSMCU_BGN +	40 );
//DCS发送增加终端同意应答，DCS->MCU，消息体为CDcsMcuAddMtAckPDU
OSPEVENT( DCS_MCU_ADDMT_ACK,			EV_DCSMCU_BGN + 41 );
//DCS拒绝邀请终端，DCS->MCU，消息体为CDcsMcuAddMtNackPDU
OSPEVENT( DCS_MCU_ADDMT_NACK,			EV_DCSMCU_BGN + 42 );
//DCS终端加入通知，DCS->MCU，消息体为CDcsMcuMtMtOnlineNotifyPDU
OSPEVENT( DCS_MCU_MTJOINED_NOTIF,		EV_DCSMCU_BGN + 43 );


//MCU请求DCServer删除终端，MCU->DCS，消息体为CMcuDcsDelMtReqPDU
OSPEVENT( MCU_DCS_DELMT_REQ,			EV_DCSMCU_BGN +	44 );
//DCS发送删除终端同意应答，DCS->MCU，消息体为CDcsMcuDelMtAckPDU
OSPEVENT( DCS_MCU_DELMT_ACK,			EV_DCSMCU_BGN + 45 );
//DCS拒绝删除终端，DCS->MCU，消息体为CDcsMcuDelMtNackPDU
OSPEVENT( DCS_MCU_DELMT_NACK,			EV_DCSMCU_BGN + 46 );
//DCS终端离会(主动, 或是被删除)通知，DCS->MCU，消息体为CDcsMcuMtMtOfflineNotifyPDU
OSPEVENT( DCS_MCU_MTLEFT_NOTIF,			EV_DCSMCU_BGN + 47 );


// ---MCU_DCS OTHER (38351-38360)---
//MCU请求DCS开启数据应用服务
OSPEVENT( MCU_DCS_STARTT120_REQ,		EV_DCSMCU_BGN + 50 );
//DCS同意开启数据应用服务应答
OSPEVENT( DCS_MCU_STARTT120_ACK,		EV_DCSMCU_BGN + 51 );
//DCS拒绝开启数据应用服务应答
OSPEVENT( DCS_MCU_STARTT120_NACK,		EV_DCSMCU_BGN + 52 );
//DCS开启数据应用服务通知
OSPEVENT( DCS_MCU_STARTT120_NOTIF,		EV_DCSMCU_BGN + 53 );

//DCS请求MCU的主备状态: CDcsMcuGetmsstatusReqPdu
OSPEVENT( DCS_MCU_GETMSSTATUS_REQ,		EV_DCSMCU_BGN + 54 );
//MCU应答主备状态: CMcuDcsGetmsstatusAckPdu
OSPEVENT( MCU_DCS_GETMSSTATUS_ACK,		EV_DCSMCU_BGN + 55 );
//MCU拒绝应答主备状态: CMcuDcsGetmsstatusNackPdu
OSPEVENT( MCU_DCS_GETMSSTATUS_NACK,		EV_DCSMCU_BGN + 56 );


// ---Reserved (38371-38490) total 130---

// ---TEST (38491-38500)---
//MCU请求DCServer邀请终端，MCU->DCS，消息体为CMcuDcsAddMtReqPDU
OSPEVENT( TEST_MCU_DCS_DELMT_REQ,		EV_DCSMCU_BGN + 90 );
//MCU请求DCServer删除终端，MCU->DCS，消息体为CMcuDcsDelMtReqPDU
OSPEVENT( TEST_MCU_DCS_ADDMT_REQ,		EV_DCSMCU_BGN + 91 );


#endif /* _EV_MCUDCS_H_ */
