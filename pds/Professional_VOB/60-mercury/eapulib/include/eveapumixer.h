/*****************************************************************************
   模块名      : 混音器
   文件名      : evmixer.h
   创建时间    : 2010年 01月 09日
   实现功能    : 
   作者        : 
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/09    3.0                     创建
******************************************************************************/

#ifndef _EVENT_MCU_MIXER_H_
#define _EVENT_MCU_MIXER_H_
#include "evmcueqp.h"
#include "eventid.h"

//--------------------混音器内部消息-------------------

//混音器初始化消息,消息为TMixerCfg
OSPEVENT(EV_MIXER_INIT,                 EV_MIXER_BGN + 1 ); 
//混音组初始化消息,无消息体
OSPEVENT(EV_MIXER_GRPINIT,              EV_MIXER_BGN + 2 );
//定时建链消息,无消息体
OSPEVENT(EV_MIXER_CONNECT,              EV_MIXER_BGN + 3 );
//向MCU注册消息,无消息体
OSPEVENT(EV_MIXER_REGISTER,             EV_MIXER_BGN + 4 );
//向MCU注册消息等待应答超时消息,无消息体
OSPEVENT(EV_MIXER_REGTIMEOUT,           EV_MIXER_BGN + 5 );
//混音活动成员变化通知消息
OSPEVENT(EV_MIXER_ACTIVECHANGE_NOTIF,   EV_MIXER_BGN + 6 );
//混音器退出消息
OSPEVENT(EV_MIXER_EXIT,                 EV_MIXER_BGN + 7 );
//显示混音器状态底层函数消息
OSPEVENT(EV_MIXER_DEVICESTATUSSHOW,     EV_MIXER_BGN + 8 );
//显示混音组状态
OSPEVENT(EV_MIXER_SHOWGROUP,            EV_MIXER_BGN + 9 );
//显示混音成员
OSPEVENT(EV_MIXER_SHOWMEMBER,           EV_MIXER_BGN + 10 );
//连接Mcu.B
OSPEVENT(EV_MIXER_CONNECTB,             EV_MIXER_BGN + 11 );
//向Mcu.B注册
OSPEVENT(EV_MIXER_REGISTERB,            EV_MIXER_BGN + 12 );
//向Mcu取主备倒换状态
OSPEVENT(EV_MIXER_MCU_GETMSSTATUS,      EV_MIXER_BGN + 13 );
//显示混音器当前终端和混音通道映射表
OSPEVENT(EV_MIXER_SHOWMT2CHNNL,         EV_MIXER_BGN + 14 );


//--------------------混音器测试消息-------------------

OSPEVENT(EV_MIXER_TEST_GETMSG,          EV_TEST_MIXER_BGN + 1 );
//获取外设状态
OSPEVENT(EV_MIXER_TEST_GETEQPSTATUS,    EV_TEST_MIXER_BGN + 2 );
//强迫MCU与混音器断链
OSPEVENT(EV_MIXER_TEST_DISCONNECTMIXER, EV_TEST_MIXER_BGN + 3 );

#endif//!_EVENT_MCU_MIXER_H_
