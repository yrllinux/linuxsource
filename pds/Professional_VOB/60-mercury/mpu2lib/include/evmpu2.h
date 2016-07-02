/*****************************************************************************
   模块名      : mpu2lib
   文件名      : evmpu2.h
   相关文件    : 
   文件实现功能: mpu2lib内部消息定义
   作者        : 倪志俊
   版本        : V4.7  Copyright(C) 2012-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2012/07/26  4.7         倪志俊       创建
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
//第二个BAS外设注册消息
OSPEVENT( EV_SECOND_BAS_REGISTER,       EV_BAS_BGN + 19 );

OSPEVENT( EV_SECOND_BAS_REGISTERB,         EV_BAS_BGN + 20 );	

OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //内部定时器
OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //关键帧定时器
OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 26 );            //连接定时器
OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 27 );            //连接定时器
OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 28 );            //注册定时器
OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 29 );            //注册定时器
OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 30 );            //取主备倒换状态
OSPEVENT( TIMER_TEST_LED,           EV_VMP_BGN + 31 );            //生产测试的点灯测试timer


OSPEVENT( EV_VMP_SHOWDEBUG,            EV_BAS_BGN + 19 );			//取VMPdebug文件信息,19是了和BAS的消息进行区别，如以后BAS消息增加，这个值就得超过

/*MPU自动检测消息（57741－57800）*/ 
#define EV_C_BASE             57741

#define EV_C_CONNECT_TIMER                 EV_C_BASE + 1    // 建链
#define EV_C_REGISTER_TIMER				   EV_C_BASE + 2
#define EV_C_INIT                          EV_C_BASE + 3    // 初始化

				   

#define EV_C_CHANGEAUTOTEST_CMD            EV_C_BASE + 6    // 改变自动测试标志

#define EV_C_BASCHL_INIT					EV_C_BASE + 10	// bas单通道初始化 

#endif// _EVENT_MPUBAS_H_

//END OF FILE

