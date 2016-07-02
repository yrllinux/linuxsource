/*****************************************************************************
   模块名      : WatchDog 库宏、常数定义
   文件名      : bindwatchdog.h
   相关文件    : 
   文件实现功能: 启动WatchDog，向服务器注册自身状态。

   作者        : 顾振华
   版本        : V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/05/26  4.0         顾振华      创建
******************************************************************************/

#ifndef _WATCHDOGCONSTDEF_H_
#define _WATCHDOGCONSTDEF_H_

#include "dsccomm.h"

// 定时器周期
#define TIMER_REG_POWERON   500
#define TIMER_REG_DISC      5000

// 定时器事件

// 连接服务器
OSPEVENT(EV_CONNECTWDSERV_TIMER,       (EV_DSCCONSOLE_BGN +90) );   

// 向服务器注册
OSPEVENT(EV_REGWDSERV_TIMER,           (EV_DSCCONSOLE_BGN +91) );   


#endif
