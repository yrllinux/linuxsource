/*****************************************************************************
   模块名      : 
   文件名      : evmodem.h
   相关文件    : 
   文件实现功能: 卫星MODEM消息
   作者        : 
   版本        : V0.9  Copyright(C) 2001-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2004/10/25  0.9         zhangsh        创建
******************************************************************************/
#ifndef _EV_MODEM_H_
#define _EV_MODEM_H_

#include "osp.h"
#include "eventid.h"

//向MODEM模块注册TMt + TTransportAddr + type(u8)
OSPEVENT( MODEM_MCU_REG_REQ,				EV_MODEM_BGN );
//注册确认
OSPEVENT( MODEM_MCU_REG_ACK,				EV_MODEM_BGN + 1 );
//注册拒绝
OSPEVENT( MODEM_MCU_REG_NACK,				EV_MODEM_BGN + 2 );

//MODEM进入接收状态
OSPEVENT( MCU_MODEM_RECEIVE,				EV_MODEM_BGN + 10 );
//确认
OSPEVENT( MODEM_MCU_REV_ACK,				EV_MODEM_BGN + 11 );
//拒绝
OSPEVENT( MODEM_MCU_REV_NACK,				EV_MODEM_BGN + 12 );
//通知
OSPEVENT( MODEM_MCU_REV_NOTIF,				EV_MODEM_BGN + 13 );

//MODEM进入发送状态
OSPEVENT( MCU_MODEM_SEND,				    EV_MODEM_BGN + 20 );
//确认
OSPEVENT( MODEM_MCU_SEND_ACK,				EV_MODEM_BGN + 21 );
//拒绝
OSPEVENT( MODEM_MCU_SEND_NACK,				EV_MODEM_BGN + 22 );
//通知
OSPEVENT( MODEM_MCU_SEND_NOTIF,				EV_MODEM_BGN + 23 );

//MODEM进入停止发送状态
OSPEVENT( MCU_MODEM_STOPSEND,				EV_MODEM_BGN + 30 );
//确认
OSPEVENT( MODEM_MCU_STOPSEND_ACK,			EV_MODEM_BGN + 31 );
//拒绝
OSPEVENT( MODEM_MCU_STOPSEND_NACK,			EV_MODEM_BGN + 32 );
//通知
OSPEVENT( MODEM_MCU_STOPSEND_NOTIF,			EV_MODEM_BGN + 33 );

//MODEM主从时钟设定
OSPEVENT( MCU_MODEM_SETSRC,				    EV_MODEM_BGN + 40 );
//确认
OSPEVENT( MODEM_MCU_SETSRC_ACK,				EV_MODEM_BGN + 41 );
//拒绝
OSPEVENT( MODEM_MCU_SETSRC_NACK,			EV_MODEM_BGN + 42 );
OSPEVENT( MODEM_MCU_SETSRCCLK_NOTIF,		EV_MODEM_BGN + 43 );
OSPEVENT( MODEM_MCU_SETREVCLK_NOTIF,		EV_MODEM_BGN + 44 );

//MODEM释放
OSPEVENT( MCU_MODEM_RELEASE,				EV_MODEM_BGN + 50 );
//确认
OSPEVENT( MODEM_MCU_RELEASE_ACK,			EV_MODEM_BGN + 51 );
//拒绝
OSPEVENT( MODEM_MCU_RELEASE_NACK,			EV_MODEM_BGN + 52 );
OSPEVENT( MODEM_MCU_RELEASE_NOTIF,			EV_MODEM_BGN + 53 );

OSPEVENT( MODEM_MCU_ERRSTAT_NOTIF,			EV_MODEM_BGN + 60 );
OSPEVENT( MODEM_MCU_RIGHTSTAT_NOTIF,		EV_MODEM_BGN + 70 );
//MODEM释放
OSPEVENT( MCU_MODEM_SETBITRATE,				EV_MODEM_BGN + 80 );
//确认
OSPEVENT( MODEM_MCU_SETBITRATE_ACK,			EV_MODEM_BGN + 81 );
//拒绝
OSPEVENT( MODEM_MCU_SETBITRATE_NACK,		EV_MODEM_BGN + 82 );
OSPEVENT( MODEM_MCU_SETBITRATE_NOTIF,		EV_MODEM_BGN + 83 );

#endif /* _EV_MCU_H_ */
