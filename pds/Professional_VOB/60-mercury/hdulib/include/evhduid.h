/*****************************************************************************
模块名      : 高清解码卡
文件名      : evhduid.h
相关文件    : 
文件实现功能: HDU内部消息
作者        : 江乐斌
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/11/21              江乐斌	     创建		
******************************************************************************/
#ifndef _EVENET_HDU_H_
#define _EVENET_HDU_H_

#include "osp.h"
#include "eventid.h"


/**********************************************************
*            HDU内部业务消息（30501-30600）              *
***********************************************************/
//通道解码器创建
OSPEVENT( EV_HDU_INIT,		        	   EV_HDU_BGN + 1 );
//停止HDU
OSPEVENT( EV_HDU_QUIT,		        	   EV_HDU_BGN + 2 );
//连接MCU消息
OSPEVENT( EV_HDU_CONNECT,			       EV_HDU_BGN + 3 );
//连接MCUB消息
OSPEVENT( EV_HDU_CONNECTB,			       EV_HDU_BGN + 4 );
//向MCU注册消息
OSPEVENT( EV_HDU_REGISTER,			       EV_HDU_BGN + 5 );
//向MCUB注册消息
OSPEVENT( EV_HDU_REGISTERB,			       EV_HDU_BGN + 6 );
//显示HDU配置
OSPEVENT( EV_HDU_CONFIGSHOW,			   EV_HDU_BGN + 8 );
//显示HDU状态
OSPEVENT( EV_HDU_STATUSSHOW,		       EV_HDU_BGN + 9 );
//显示通道状态
OSPEVENT( EV_HDU_SENDCHNSTATUS,		       EV_HDU_BGN + 10 );
//向Mcu取主备倒换状态
OSPEVENT( EV_HDU_GETMSSTATUS_TIMER,	       EV_HDU_BGN + 11 );
//HDU向MCU请求关键帧
OSPEVENT( EV_HDU_NEEDIFRAME_TIMER,	       EV_HDU_BGN + 12 );
//设置HDU输入输出制式
OSPEVENT( EV_HDU_SETMODE,	               EV_HDU_BGN + 13 );
//初始化通道信息
OSPEVENT( EV_HDU_INITCHNLINFO,	           EV_HDU_BGN + 14 );




#endif

