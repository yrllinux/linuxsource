/*****************************************************************************
模块名      : hduhardwate auto test client
文件名      : hduhwautotest_evid.h
相关文件    : hduhwautotest.cpp
文件实现功能: HDU硬件自动化测试
作者        : 
版本        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/04/21  1.0         江乐斌        创建 
******************************************************************************/
#ifndef HDUHWAUTOTEST_EV_H
#define HDUHWAUTOTEST_EV_H

/*hdu自动检测消息（57641－57700）*/ 
#define EV_C_BASE             57641

#define EV_C_STARTSWITCH_CMD               EV_C_BASE + 1    // 开始hdu交换
#define EV_C_STOPSWITCH_CMD                EV_C_BASE + 2    // 停止hdu交换

#define EV_C_CONNECT_TIMER                 EV_C_BASE + 3    // 建链             
#define EV_C_INIT                          EV_C_BASE + 4    // 初始化

#define EV_C_INITCHL                       EV_C_BASE + 5    // 初始化通道
#define EV_C_CHANGEAUTOTEST_CMD            EV_C_BASE + 6    // 改变自动测试标志

#endif
