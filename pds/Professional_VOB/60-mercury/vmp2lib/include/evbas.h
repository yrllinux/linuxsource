/*****************************************************************************
   模块名      : 码流适配器
   文件名      : evbas.h
   创建时间    : 2003年 6月 25日
   实现功能    : 内部消息定义
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
 
******************************************************************************/
#ifndef _EVENT_BAS_H_
#define _EVENT_BAS_H_
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

#endif// _EVENT_BAS_H_
