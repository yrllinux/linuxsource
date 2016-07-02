/*****************************************************************************
   模块名      : 录像机
   文件名      : EvRec.h
   相关文件    : 
   文件实现功能: 录像机接口消息定义
				 *_REQ: 需应答请求
				 *_ACK, *_NACK: 请求之答复
				 *_CMD: 不需应答命令
				 *_NOTIF: 状态通知
   作者        : 张明义
   版本        : V1.0  Copyright(C) 2001-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/04  3.0         张明义      对正在录放像文件的保护
******************************************************************************/
#ifndef EVREC_H
#define EVREC_H

#include "eventid.h"

//-------------     录像机内部消息     --------------

//录像机初始化消息，
OSPEVENT( EV_REC_INIT,				EV_REC_BGN  + 1 );
//录像机各通道初始化消息
OSPEVENT( EV_REC_CHNINIT,			EV_REC_BGN  + 2 );
//录像机定时建链消息
OSPEVENT( EV_REC_CONNECTTIMER,		EV_REC_BGN  + 3 );
//定时注册消息
OSPEVENT( EV_REC_REGTIMER,			EV_REC_BGN  + 4 );
//录像机设备通知
OSPEVENT( EV_REC_DEVNOTIF,			EV_REC_BGN  + 5 );
//等待注册应答消息
OSPEVENT( EV_REC_REGWAIT,			EV_REC_BGN  + 6 );
//强行停止个通道设备
OSPEVENT( EV_REC_STOPDEVICE,		EV_REC_BGN  + 7 );
//录像通道进程通知
OSPEVENT( EV_REC_RECPROGTIME,		EV_REC_BGN  + 8 );
//显示录像及状态信息
OSPEVENT( EV_REC_STATUSSHOW,		EV_REC_BGN  + 9 );
//录像机退出
OSPEVENT( EV_REC_QUIT,				EV_REC_BGN + 10 );
//录像机定时状态上报消息
OSPEVENT( EV_REC_STATUSNOTYTIME,	EV_REC_BGN + 11 );
//维护录像机播放列表
//OSPEVENT( EV_REC_PLAYLIST_MAINTAIN,	EV_REC_BGN + 12 );
//磁盘空间告警
//OSPEVENT( EV_REC_DISKNOTIFY,		EV_REC_BGN + 13 );
//录像机上电
OSPEVENT( EV_REC_POWERON,			EV_REC_BGN + 14 );
//对话框取配置信息
OSPEVENT(EV_DLG_REC_GETCONF,		EV_REC_BGN + 15 );
//向Mcu取主备倒换状态
OSPEVENT(EV_REC_GETMSSTATUS_TIMER,	EV_REC_BGN + 16 );
//录像机重启
OSPEVENT( EV_REC_RESTART,			EV_REC_BGN + 21 );
//录像机定时建链消息
OSPEVENT( EV_REC_CONNECTTIMERB,		EV_REC_BGN + 22 );
//定时注册消息
OSPEVENT( EV_REC_REGTIMERB,			EV_REC_BGN + 23 );
//通知对应的录像通道，改变当前录像文件名
OSPEVENT( EV_REC_REFRESH_FILENAME,  EV_REC_BGN + 24 );
//开始放象等待交换建立通知定时器
OSPEVENT( EV_REC_SWITCHSTART_WAITTIMER, EV_REC_BGN + 25 );
//向MCU请求关键帧
OSPEVENT( EV_REC_MCU_NEEDIFRAME_TIMER, EV_REC_BGN + 26 );
//创建发布点定时器
OSPEVENT( EV_REC_PUBLISHPOINT_TIMER, EV_REC_BGN + 27 );

//--------------     测试使用消息     -------------------

//获得应答消息
OSPEVENT(REC_TEST_GET_RECACKEVENT,	EV_TEST_REC_BGN + 1 );
//获取录像机外设状态
OSPEVENT( REC_TEST_GETEQPSTATUS,	EV_TEST_REC_BGN + 3 );

// 界面消息
#define WM_REC_RESTART              (WM_USER+0x101)     // 重新启动
#define WM_SHOWTASK				    (WM_USER+0x102)     // 任务栏消息
#define WM_OPENDATABASE			    (WM_USER+0x103)		// 打开数据库
#define OPEN_DATABASE_TIMER         (WM_USER+0x104)	    // 打开数据库定时器
#define REFRSH_CHNL_TIMER           (WM_USER+0x105)	    // 刷新界面
#define WM_QUITREC					(WM_USER+0x106)		// 退出REC
#endif /*!EVREC_H*/
