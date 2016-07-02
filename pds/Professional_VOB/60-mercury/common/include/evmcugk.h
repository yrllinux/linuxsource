/*****************************************************************************
   模块名      : 新方案业务
   文件名      : evras.h
   相关文件    : 
   文件实现功能: 新方案业务RAS消息定义
   作者        : zhangsh
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/10/15  1.0         zhangsh        创建
******************************************************************************/
#ifndef _EV_MCUGK_H_
#define _EV_MCUGK_H_

#include "osp.h"
#include "eventid.h"

/**********************************************************
 以下为业务RAS内部消息
**********************************************************/
//协议栈返回请求消息,消息体在BUF中
OSPEVENT( STACK_GKM_REQ,			  EV_RAS_BGN );
//协议栈返回确认消息,消息体在BUF中
OSPEVENT( STACK_GKM_CFM,			  EV_RAS_BGN + 1 );
//协议栈返回拒绝消息,消息体在BUF中
OSPEVENT( STACK_GKM_REJ,			  EV_RAS_BGN + 2 );
//协议栈返回超时消息,消息体在BUF中
OSPEVENT( STACK_GKM_EXP,			  EV_RAS_BGN + 3 );
//外部定时器的消息
OSPEVENT( MTADP_GKM_TIMER,			  EV_RAS_BGN + 4 );
/*-----------以下为外部消息接口-----------------------------*/
//注册MCU请求，MCU->GKM,消息体为空
OSPEVENT( MCU_GKM_REGISTERMCU_REQ,		  EV_RAS_BGN + 10 );
//注册会议请求，MCU->GKM,消息体为CRegisterRequest
OSPEVENT( MCU_GKM_REGISTERCOF_REQ,		  EV_RAS_BGN + 20 );
//注册MCU确认,GKM->MCU,消息体为空
OSPEVENT( GKM_MCU_REGISTER_ACK,		  	  EV_RAS_BGN + 11 );
//注册MCU拒绝,GKM->MCU,消息体为错误代码
OSPEVENT( GKM_MCU_REGISTER_NACK,		  EV_RAS_BGN + 12 );

//初始带宽和地址解析请求，MCU->GKM,消息体为CAdmissinRequest
OSPEVENT( MCU_GKM_ADMISSION_REQ,		  EV_RAS_BGN + 30 );
//初始带宽和地址解析确认,GKM->MCU,消息体为TTransportAddr
OSPEVENT( GKM_MCU_ADMISSION_ACK,		  EV_RAS_BGN + 31 );
//初始带宽和地址解析拒绝,GKM->MCU,消息体为错误代码
OSPEVENT( GKM_MCU_ADMISSION_NACK,		  EV_RAS_BGN + 32 );

//带宽改变请求，MCU->GKM,消息体为CBandWidthRequest
OSPEVENT( MCU_GKM_BANDWIDTH_REQ,		  EV_RAS_BGN + 40 );
//带宽改变确认,GKM->MCU,消息体为无
OSPEVENT( GKM_MCU_BANDWIDTH_ACK,		  EV_RAS_BGN + 41 );
//带宽改变拒绝,GKM->MCU,消息体为错误代码
OSPEVENT( GKM_MCU_BANDWIDTH_NACK,		  EV_RAS_BGN + 42 );

//带宽释放请求，MCU->GKM,消息体为CDisengageRequest
OSPEVENT( MCU_GKM_DISENGAGE_REQ,		  EV_RAS_BGN + 50 );
//带宽释放确认,GKM->MCU,消息体为无
OSPEVENT( GKM_MCU_DISENGAGE_ACK,		  EV_RAS_BGN + 51 );
//带宽释放拒绝,GKM->MCU,消息体为错误代码
OSPEVENT( GKM_MCU_DISENGAGE_NACK,		  EV_RAS_BGN + 52 );

//注销会议请求，MCU->GKM,消息体为CUnregisterRequest
OSPEVENT( MCU_GKM_UNREGISTERCOF_REQ,	  	  EV_RAS_BGN + 60 );
//注销MCU请求，MCU->GKM,消息体为无
OSPEVENT( MCU_GKM_UNREGISTERMCU_REQ,		  EV_RAS_BGN + 70 );
//注销会议确认,GKM->MCU,消息体为无
OSPEVENT( GKM_MCU_UNREGISTER_ACK,	  	  EV_RAS_BGN + 61 );
//注销会议拒绝,GKM->MCU,消息体为错误代码
OSPEVENT( GKM_MCU_UNREGISTER_NACK,		  EV_RAS_BGN + 62 );

/*------------------其它消息-----------------------------------*/
//超时信息,GKM->MCU,消息体为错误代码
OSPEVENT( GKM_MCU_TIMEOUT,		  	  EV_RAS_BGN + 80 );
//网守注销MCU或者会议,GKM->MCU,消息体为空
OSPEVENT( GKM_MCU_UNREGISTER_NOTIFY,	  	  EV_RAS_BGN + 81 );
//网守释放带宽通知,GKM->MCU,消息体为CDisengageRequest
OSPEVENT( GKM_MCU_DISENGAGE_NOTIFY,		  EV_RAS_BGN + 82 );

#endif /* _EV_RAS_H_ */
