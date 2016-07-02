/*****************************************************************************
   模块名      : MpwLib多画面复合电视墙
   文件名      : evmpw.h
   相关文件    : 
   文件实现功能: 定义mpw的内不事件号
   作者        : john
   版本        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/09/25  1.0         john         创建
******************************************************************************/
#ifndef _VIDEO_MPWEVENT_H_
#define _VIDEO_MPWEVENT_H_

#include "osp.h"

#define DOUBLE_VIDEO                (u8)2       // 双路发送
#define FIRST_REGACK                (u8)1       // 第一次收到注册Ack
#define FIRST_CHANNEL               (u8)1       // 一路视频
#define SECOND_CHANNEL              (u8)2       // 两路视频

#define MPW_CONNECT_TIMEOUT         (u16)3*1000
#define MPW_REGISTER_TIMEOUT        (u16)5*1000
#define CHECK_IFRAME_INTERVAL       (u16)1000    // 请求关键帧时间间隔

//取主备倒换状态
OSPEVENT( EV_MPW_GETMSSTATUS_TIMER,  EV_VMPTW_BGN + 1 );
//连接MCU.A
OSPEVENT( EV_MPW_CONNECT_TIMER,      EV_VMPTW_BGN + 2 );  
//连接MCU.B
OSPEVENT( EV_MPW_CONNECT_TIMERB,     EV_VMPTW_BGN + 3 ); 
//注册MCU.A
OSPEVENT( EV_MPW_REGISTER_TIMER,     EV_VMPTW_BGN + 4 ); 
//注册MCU.B
OSPEVENT( EV_MPW_REGISTER_TIMERB,    EV_VMPTW_BGN + 5 ); 
//请求关键帧定时器
OSPEVENT( EV_MPW_NEEDIFRAME_TIMER,   EV_VMPTW_BGN + 6 ); 
#endif
