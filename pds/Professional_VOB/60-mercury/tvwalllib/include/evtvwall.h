/*****************************************************************************
   模块名      : 多画面显示
   文件名      : evtvwall.h
   相关文件    : 
   文件实现功能: 多画面显示内部消息
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
  -----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/20     			   zhangsh		MCC
******************************************************************************/
#ifndef _EVENET_TVWALL_H_
#define _EVENET_TVWALL_H_

#include "osp.h"
#include "eventid.h"

/**********************************************************
 电视墙内部业务消息（30401-30500）
**********************************************************/

//上电消息
OSPEVENT( EV_TVWALL_INIT,				EV_TVWALL_BGN + 1 );
//连接MCU消息
OSPEVENT( EV_TVWALL_CONNECT,			EV_TVWALL_BGN + 2 );
//向MCU注册消息
OSPEVENT( EV_TVWALL_REGISTER,			EV_TVWALL_BGN + 3 );
//注册定时消息
OSPEVENT( EV_TVWALL_REGTIMEOUT,			EV_TVWALL_BGN + 4 );
//显示电视墙配置
OSPEVENT( EV_TVWALL_CONFIGSHOW,			EV_TVWALL_BGN +	5 );
//显示电视墙状态
OSPEVENT( EV_TVWALL_STATUSSHOW,			EV_TVWALL_BGN +	6 );
//连接MCU消息
OSPEVENT( EV_TVWALL_CONNECTB,			EV_TVWALL_BGN + 7 );
//向MCU注册消息
OSPEVENT( EV_TVWALL_REGISTERB,			EV_TVWALL_BGN + 8 );
//向Mcu取主备倒换状态
OSPEVENT( EV_TVWALL_GETMSSTATUS_TIMER,	EV_TVWALL_BGN + 9 );

//...............................................................

//...............................................................

//集成测试消息
OSPEVENT( EV_TVWALL_INTEG_GETSTATE_CMD  ,	EV_TVWALL_BGN + 80 );

//获取指定通道来源
OSPEVENT( EV_TVWALL_INTEG_GETCHNSRC_CMD ,	EV_TVWALL_BGN + 81 );

#endif // _EVENET_TVWALL_H_
