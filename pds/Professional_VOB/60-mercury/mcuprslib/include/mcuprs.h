/*****************************************************************************
模块名      : prs
文件名      : mcuprs.h
相关文件    : mcuprs.cpp
文件实现功能: prs模块统一头文件
作者        : 周广程
版本        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/04/07  4.0         周广程        创建
******************************************************************************/
#ifndef _MCUPRS_H_
#define _MCUPRS_H_

#include "eventid.h"

#define FILE_PRSLOG				( LPCSTR )"prs.log"

OSPEVENT( EV_MCUPRS_TIMER,               EV_PRS_BGN );                   // 内部定时器
OSPEVENT( EV_CONNECT_TIMER,              EV_PRS_BGN + 1 );               // connect to MCU.A
OSPEVENT( EV_CONNECT_TIMERB,             EV_PRS_BGN + 2 );               // connect to MCU.B
OSPEVENT( EV_REGISTER_TIMER,             EV_PRS_BGN + 3 );               // register to MCU.A
OSPEVENT( EV_REGISTER_TIMERB,            EV_PRS_BGN + 4 );               // register to MCU.B
OSPEVENT( TIME_GET_MSSTATUS,             EV_PRS_BGN + 5 );               // 取主备倒换状态

OSPEVENT( EV_PRSGUARD_MAINTASKSCAN_TIMER,	 EV_PRS_BGN + 10 );			 // PRS GUARD探测主线程定时器
OSPEVENT( EV_MAINTASKSCAN_REQ,			 EV_PRS_BGN + 11 );				 // 探测请求
OSPEVENT( EV_MAINTASKSCAN_ACK,			 EV_PRS_BGN + 12 );				 // 探测回应

#endif

