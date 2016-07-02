/*****************************************************************************
   模块名      : mpulib
   文件名      : evmpu.h
   相关文件    : 
   文件实现功能: mpulib内部消息定义
   作者        : 周文
   版本        : V4.6  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      注释
******************************************************************************/

#ifndef _EVENT_MPUBAS_H_
#define _EVENT_MPUBAS_H_
#include "eventid.h"

//码流适配器初始化消息
OSPEVENT( EV_BAS_INI,               EV_BAS_BGN + 1 );
//码流适配器退出消息，
OSPEVENT( EV_BAS_EXIT,              EV_BAS_BGN + 2 );
//定时建链消息
OSPEVENT( EV_BAS_CONNECT,           EV_BAS_BGN + 3 );
//定时注册消息
OSPEVENT( EV_BAS_REGISTER,          EV_BAS_BGN + 4 );
//等待注册应答
OSPEVENT( EV_BAS_REGTIMEOUT,        EV_BAS_BGN + 5 );
//退出适配
OSPEVENT( EV_BAS_QUIT,              EV_BAS_BGN + 6 );
//显示适配器信息
OSPEVENT( EV_BAS_SHOW,              EV_BAS_BGN + 7 );
//强制关闭个通道
OSPEVENT( EV_BAS_STOPCHNADP,        EV_BAS_BGN + 8 );
//显示Bas状态
OSPEVENT( EV_BAS_SHOWINFO,          EV_BAS_BGN + 10 );
//连接MCU.B
OSPEVENT( EV_BAS_CONNECTB,          EV_BAS_BGN + 11 );
//向MCU.B注册
OSPEVENT( EV_BAS_REGISTERB,         EV_BAS_BGN + 12 );
//向MCU取主备倒换状态
OSPEVENT( EV_BAS_GETMSSTATUS,       EV_BAS_BGN + 13 );
//请求关键帧
OSPEVENT( EV_BAS_NEEDIFRAME_TIMER,  EV_BAS_BGN + 14 );
//请求发送通道状态
OSPEVENT( EV_BAS_SENDCHNSTATUS,     EV_BAS_BGN + 15 );

//本地BAS状态切换定时器
OSPEVENT( EV_BAS_STATUSCHK_TIMER,	EV_BAS_BGN + 16 );
//修改BAS发送地址
OSPEVENT( EV_BAS_MODSENDADDR,       EV_BAS_BGN + 17 );

OSPEVENT( EV_BAS_SHOWDEBUG,         EV_BAS_BGN + 18 );				//取BASdebug文件信息

OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //内部定时器
OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //关键帧定时器
OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 2 );            //连接定时器
OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 3 );            //连接定时器
OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 4 );            //注册定时器
OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 5 );            //注册定时器
OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 6 );            //取主备倒换状态
OSPEVENT( EV_VMP_SHOWDEBUG,            EV_BAS_BGN + 19 );			//取VMPdebug文件信息,19是了和BAS的消息进行区别，如以后BAS消息增加，这个值就得超过

#endif// _EVENT_MPUBAS_H_

//END OF FILE

