 /*****************************************************************************
   模块名      : mpuhardware (vmp)auto test client 
   文件名      : mpuvmphwtest_evid.h
   相关文件    : mpuvmphwtest.cpp
   文件实现功能: MPU画面合成自动化测试
   作者        : 
   版本        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2009/05/27  1.0         薛亮        创建 
******************************************************************************/
#ifndef MPUHARDWARETEST_EV_H
#define MPUHARDWARETEST_EV_H

/*MPU自动检测消息（57741－57800）*/ 
#define EV_C_BASE             57741

#define EV_C_CONNECT_TIMER                 EV_C_BASE + 1    // 建链
#define EV_C_REGISTER_TIMER				   EV_C_BASE + 2
#define EV_C_INIT                          EV_C_BASE + 3    // 初始化

				   

#define EV_C_CHANGEAUTOTEST_CMD            EV_C_BASE + 6    // 改变自动测试标志

#define EV_C_BASCHL_INIT					EV_C_BASE + 10	// bas单通道初始化 

#endif
