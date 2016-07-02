/*****************************************************************************
模块名      : 高清解码单元
文件名      : hdu2_eventid.h
相关文件    : 
文件实现功能: hdu2实例头文件
作者        : 田志勇
版本        : 4.7  Copyright(C) 2011-2013 KDV, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7         田志勇      创建
******************************************************************************/
#ifndef _EVENET_HDU2_H_
#define _EVENET_HDU2_H_

#include "osp.h"
#include "eventid.h"
/**********************************************************
*            HDU2内部业务消息（30501-30600）              *
***********************************************************/
//通道管理组创建
OSPEVENT( EV_HDUCHNMGRGRP_CREATE,		   EV_HDU_BGN + 1 );
//连接MCU消息
OSPEVENT( EV_HDU_CONNECT_TIMER,			   EV_HDU_BGN + 2 );
//连接MCUB消息
OSPEVENT( EV_HDU_CONNECTB_TIMER,		   EV_HDU_BGN + 3 );
//向MCU注册消息
OSPEVENT( EV_HDU_REGISTER_TIMER,		   EV_HDU_BGN + 4 );
//向MCUB注册消息
OSPEVENT( EV_HDU_REGISTERB_TIMER,		   EV_HDU_BGN + 5 );
//初始化通道配置
OSPEVENT( EV_HDU_INITCHNCFG,	           EV_HDU_BGN + 6);
//HDU向MCU请求关键帧
OSPEVENT( EV_HDU_NEEDIFRAME_TIMER,	       EV_HDU_BGN + 7 );
//向Mcu取主备倒换状态向Mcu取主备倒换状态
OSPEVENT( EV_HDU_GETMSSTATUS_TIMER,	       EV_HDU_BGN + 8 );
//通知通道开启通道组解码及码流接收(仅生产测试用)
OSPEVENT( EV_C_STARTSWITCH_CMD,		       EV_HDU_BGN + 9 );
//通知通道停止通道组解码及码流接收(仅生产测试用)
OSPEVENT( EV_C_STOPSWITCH_CMD,		       EV_HDU_BGN + 10 );
//显示HDU外设相关信息
OSPEVENT( EV_HDU_STATUSSHOW,		       EV_HDU_BGN + 11 );
//显示HDU当前保存的模式
OSPEVENT( EVENT_HDU_SHOWMODWE,		       EV_HDU_BGN + 12 );
//清除HDU当前模式
OSPEVENT( EVENT_HDU_CLEARMODWE,		       EV_HDU_BGN + 13 );

#endif

