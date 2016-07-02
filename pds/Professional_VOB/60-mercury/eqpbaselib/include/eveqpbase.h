/*****************************************************************************
   模块名      : eqpbase
   文件名      : eveqpbase.h
   相关文件    : 
   文件实现功能: 外设基类消息定义
   作者        : 周嘉麟
   版本        :  1.0
   日期		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/

#ifndef _EQP_BASE_EVENT_H
#define _EQP_BASE_EVENT_H

#include "osp.h"
#include "eventid.h"

/********************************说明************************************/
/* 1. Osp保留消息0x400
   2. 业务消息起始 10000
   3. eqpbase从9900开始*/
#define EV_EQPBASE_BGN        (u16)9900
/************************************************************************/
/*				    	外设与mcu之间消息定义                           */
/************************************************************************/
//外设向mcu请求注册
OSPEVENT( EQP_MCU_REG_REQ,					EV_EQPBASE_BGN + 1 );
//外设注册成功时mcu的回应
OSPEVENT( MCU_EQP_REG_ACK,					EV_EQPBASE_BGN + 2 );
//外设注册失败时, mcu的回应
OSPEVENT( MCU_EQP_REG_NACK,					EV_EQPBASE_BGN + 3 );

//向mcu请求主备状态
OSPEVENT( EQP_MCU_GETMSSTAT_REQ,			EV_EQPBASE_BGN + 4 );
//主备状态获取成功
OSPEVENT( MCU_EQP_GETMSSTAT_ACK,			EV_EQPBASE_BGN + 5 );
//请求主备状态失败
OSPEVENT( MCU_EQP_GETMSSTAT_NACK,			EV_EQPBASE_BGN + 6 );
// 设置qos，消息体：TMcuQosCfgInfo
OSPEVENT( MCU_EQP_QOS_CMD,					EV_EQPBASE_BGN + 7 );
//修改发送地址
OSPEVENT(MCU_EQP_MODIFYADDR_CMD,			EV_EQPBASE_BGN + 8 );

/************************************************************************/
/*                      外设内部的通用消息定义				            */
/************************************************************************/
//连接MCU消息
OSPEVENT( EV_EQP_CONNECT_TIMER,				EV_EQPBASE_BGN + 51 );
//连接MCUB消息
OSPEVENT( EV_EQP_CONNECTB_TIMER,			EV_EQPBASE_BGN + 52 );
//外设定时注册Mcu
OSPEVENT( EV_EQP_REGISTER_TIMER,			EV_EQPBASE_BGN + 53 );
//外设定时注册McuB
OSPEVENT( EV_EQP_REGISTERB_TIMER,			EV_EQPBASE_BGN + 54 );
//外设定时获取主备状态
OSPEVENT( EV_EQP_GETMSSTATUS_TIMER,			EV_EQPBASE_BGN + 55 );


/************************************************************************/
/*				     	外设与生产测试服务器消息定义                    */
/************************************************************************/
//外设定时注册生产测试服务器
OSPEVENT( EQP_SERVER_REGISTER_REQ,		    EV_EQPBASE_BGN + 70 );
OSPEVENT( SERVER_EQP_REGISTER_ACK,		    EV_EQPBASE_BGN + 71 );
OSPEVENT( SERVER_EQP_REGISTER_NACK,		    EV_EQPBASE_BGN + 72 );

/************************************************************************/
/*					       外设生产测试内部消息                         */
/************************************************************************/
//生产测试外设定时连接
OSPEVENT( EV_EQPSERVER_CONNECT_TIMER,		EV_EQPBASE_BGN + 80 );
//生产测试外设定时注册
OSPEVENT( EV_EQPSERVER_REGISTER_TIMER,		EV_EQPBASE_BGN + 81 );

#endif