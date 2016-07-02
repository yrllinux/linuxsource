/*****************************************************************************
   模块名      : 新方案业务
   文件名      : evmcu.h
   相关文件    : 
   文件实现功能: 新方案业务MCU内部消息定义
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/24  0.9         李屹        创建
******************************************************************************/
#ifndef _EV_MCU_H_
#define _EV_MCU_H_

#include "osp.h"
#include "eventid.h"

/**********************************************************
 以下为业务MCU内部消息（28001-29000）
**********************************************************/

///////////////////////////////////////////////////////////
//同一MCU内部消息

//MCU自身发起创建会议请求，消息体为结构TConfFullInfo，表示对应会议信息
OSPEVENT( MCU_CREATECONF_FROMFILE,			EV_MCU_BGN + 1 );
//发给MCU内部会议实例通知某下级MT登记成功，消息体为结构TMt
//OSPEVENT( MCU_MTCONNECTED_NOTIF,			EV_MCU_BGN + 2 );
//发给MCU内部会议实例通知某下级MT断链，消息体为结构TMt
//OSPEVENT( MCU_MTDISCONNECTED_NOTIF,		EV_MCU_BGN + 3 );

//MCU 从N+1备份创建会议
OSPEVENT( MCU_CREATECONF_NPLUS,             EV_MCU_BGN + 3 );

//MCU 通知会话 断链 相应实例, zgc, 2007/04/29
OSPEVENT( MCU_DISCONNECT_CMD,		EV_MCU_BGN + 4 );
//MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30
OSPEVENT( MCU_CONNECTMPCB_CMD,		EV_MCU_BGN + 5 );

//发给MCU内部会议实例通知外设建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_EQPCONNECTED_NOTIF,			EV_MCU_BGN + 11 );
//发给MCU内部会议实例通知外设断链，消息体为TEqp
OSPEVENT( MCU_EQPDISCONNECTED_NOTIF,		EV_MCU_BGN + 12 );
//发给MCU内部会议实例通知DCS建链成功, 消息体为 CDcsMcuRegReqPdu
OSPEVENT( MCU_DCSCONNCETED_NOTIF,			EV_MCU_BGN + 13 );
//发给MCU内部会议实例通知DCS断链, 消息体为 CDcsMcuRegReqPdu
OSPEVENT( MCU_DCSDISCONNECTED_NOTIF,		EV_MCU_BGN + 14 );

//发给MCU内部会议实例通知会控建链成功，消息体为单字节会控实例号+CVcCtrlRegReq
OSPEVENT( MCU_MCSCONNECTED_NOTIF,			EV_MCU_BGN + 21 );
//发给MCU内部会议实例通知会控断链，消息体为单字节会控实例号
OSPEVENT( MCU_MCSDISCONNECTED_NOTIF,		EV_MCU_BGN + 22 );

//发给MCU内部 MCU重新注册GK 通知，消息体：NULL
OSPEVENT( MCU_MCUREREGISTERGK_NOITF,        EV_MCU_BGN + 23 );

//申请频率
OSPEVENT( MCU_NMS_SENDNMSMSG_CMD,           EV_MCU_BGN + 30 );

//发给MCU内部会议实例通知BAS建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_BASCONNECTED_NOTIF,			EV_MCU_BGN + 31 );
//发给MCU内部会议实例通知BAS断链，消息体为TEqp
OSPEVENT( MCU_BASDISCONNECTED_NOTIF,		EV_MCU_BGN + 32 );

//发给MCU内部会议实例通知PRS建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_PRSCONNECTED_NOTIF,			EV_MCU_BGN + 35 );
//发给MCU内部会议实例通知PRS断链，消息体为TEqp
OSPEVENT( MCU_PRSDISCONNECTED_NOTIF,		EV_MCU_BGN + 36 );

//发给MCU内部会议实例通知REC断链建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_RECCONNECTED_NOTIF,           EV_MCU_BGN + 40 );
//发给MCU内部会议实例通知REC断链，消息体为TEqp
OSPEVENT( MCU_RECDISCONNECTED_NOTIF,		EV_MCU_BGN + 41 );

//发给MCU内部会议实例通知Mixer建链成功，消息体为TEqp
OSPEVENT( MCU_MIXERCONNECTED_NOTIF,		    EV_MCU_BGN + 42 );
//发给MCU内部会议实例通知Mixer断链，消息体为TEqp
OSPEVENT( MCU_MIXERDISCONNECTED_NOTIF,		EV_MCU_BGN + 43 );

//发给MCU内部会议实例通知Vmp建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_VMPCONNECTED_NOTIF,		    EV_MCU_BGN + 44 );
//发给MCU内部会议实例通知Vmp断链，消息体为TEqp
OSPEVENT( MCU_VMPDISCONNECTED_NOTIF,		EV_MCU_BGN + 45 );

//发给MCU内部会议实例通知VmpTw建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_VMPTWCONNECTED_NOTIF,		    EV_MCU_BGN + 46 );
//发给MCU内部会议实例通知VmpTw断链，消息体为TEqp
OSPEVENT( MCU_VMPTWDISCONNECTED_NOTIF,		EV_MCU_BGN + 47 );

//tvwall 2
//发给MCU内部会议实例通知TvWall建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_TVWALLCONNECTED_NOTIF,		EV_MCU_BGN + 48 );
//发给MCU内部会议实例通知Tvwall断链，消息体为TEqp
OSPEVENT( MCU_TVWALLDISCONNECTED_NOTIF,		EV_MCU_BGN + 49 );

// Mcu发给自己的从文件恢复会议,消息体为TConfInfo
OSPEVENT( MCU_WAITEQP_CREATE_CONF_NOTIFY,   EV_MCU_BGN + 50 );

//MCU的GUARD模块向其它模块发的测试请求消息
OSPEVENT( MCU_APPTASKTEST_REQ,				EV_MCU_BGN + 51 );
//MCU的其它模块向GUARD模块发的测试回应消息
OSPEVENT( MCU_APPTASKTEST_ACK,				EV_MCU_BGN + 52 );
//MCU停止GUARD内部定时器
OSPEVENT( MCU_STOPGUARDTIMER_CMD,           EV_MCU_BGN + 53 );
//MCU开启GUARD内部定时器
OSPEVENT( MCU_STARTGUARDTIMER_CMD,          EV_MCU_BGN + 54 );

//MCU初始化地址簿完成通告
OSPEVENT( MCU_ADDRINITIALIZED_NOTIFY,       EV_MCU_BGN + 55 );

//MP会话通知MCU断链
OSPEVENT( MCU_MTADP_DISCONNECTED_NOTIFY,    EV_MCU_BGN + 56 );

//VCS将会议模板变为即时会议
OSPEVENT( MCU_SCHEDULE_VCSCONF_START,       EV_MCU_BGN + 59 );
//预约会议变为即时会议
OSPEVENT( MCU_SCHEDULE_CONF_START,			EV_MCU_BGN + 60 );

OSPEVENT( MCU_ADDRBOOK_GETENTRYLIST_NOTIF,  EV_MCU_BGN + 61 );
OSPEVENT( MCU_ADDRBOOK_GETGROUPLIST_NOTIF,  EV_MCU_BGN + 62 );

//Mcu Guard timer id
OSPEVENT( MCUGD_GETLICENSE_DATA,            EV_MCU_BGN + 63 );	        // license加密数据 
OSPEVENT( MCUGD_SCHEDULED_CHECK_TIMER,      EV_MCU_BGN + 65 );	        // 定时检查
//OSPEVENT( MCUGD_PASSIVE_CHECK_TIMER,      EV_MCU_BGN + 66 );			// 被动检查
OSPEVENT( MCUGD_FEEDDOG_TIMER,              EV_MCU_BGN + 66 );	        // 定时喂狗 

//McuVc timer id
OSPEVENT( MCUVC_APPLYFREQUENCE_CHECK_TIMER,     EV_MCU_BGN + 69 );          //卫星会议，mcu向网管服务器申请频率检查
OSPEVENT( MCUVC_SENDFLOWCONTROL_TIMER,          EV_MCU_BGN + 70 );          //mcu发给双流发起终端flowcontrol时的定时器
//OSPEVENT( MCUVC_VMP_WAITVMPRSP_TIMER,			EV_MCU_BGN + 71 );          //MCU等待VMP应答定时,扩充为16个
OSPEVENT( MCUVC_MIX_WAITMIXERRSP_TIMER,	        EV_MCU_BGN + 72 );          //MCU等待MIXER应答定时     
OSPEVENT( MCUVC_SCHEDULED_CHECK_TIMER,          EV_MCU_BGN + 73 );	        //预约会议定时检查
OSPEVENT( MCUVC_ONGOING_CHECK_TIMER,            EV_MCU_BGN + 74 );			//进行中会议定时检查
OSPEVENT( MCUVC_INVITE_UNJOINEDMT_TIMER,        EV_MCU_BGN + 75 );			//定时邀请未与会终端
OSPEVENT( MCUVC_POLLING_CHANGE_TIMER,           EV_MCU_BGN + 76 );			//轮询改变时钟
//OSPEVENT( MCUVC_TWPOLLING_CHANGE_TIMER,         EV_MCU_BGN + 77 );			//电视墙轮询改变时钟  
OSPEVENT( MCUVC_VMPPOLLING_CHANGE_TIMER,        EV_MCU_BGN + 77 );			//轮询改变时钟
OSPEVENT( MCUVC_WAIT_MPREG_TIMER,               EV_MCU_BGN + 78 );			//等待Mp注册时钟     
OSPEVENT( MCUVC_REFRESH_MCS_TIMER,              EV_MCU_BGN + 79 );			//刷新会控时钟          
OSPEVENT( MCUVC_MCUSRC_CHECK_TIMER,             EV_MCU_BGN + 80 );			//mcu源通知           
OSPEVENT( MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER,   EV_MCU_BGN + 81 );            
OSPEVENT( MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER,  EV_MCU_BGN + 82 );
OSPEVENT( MCUVC_CHANGE_VMPPARAM_TIMER,          EV_MCU_BGN + 83 );
OSPEVENT( MCUVC_RECREATE_DATACONF_TIMER,        EV_MCU_BGN + 84 );			//重新创建数据会议
OSPEVENT( MCU_SMCUOPENDVIDEOCHNNL_TIMER,		EV_MCU_BGN + 85 );			//下级mcu等待打开双流通道定时器
OSPEVENT( MCUVC_WAIT_CASCADE_CHANNEL_TIMER,     EV_MCU_BGN + 86 );          //等待打开下级MCU的级联通道
OSPEVENT( MCUVC_WAIT_MPOVERLOAR_TIMER,          EV_MCU_BGN + 87 );          //等待 发会控的MP超限消息的 定时器
OSPEVENT( MCUVC_AUTOSWITCH_TIMER,               EV_MCU_BGN + 88 );          //Rad级联轮询KedaMcu本地终端SetOut定时器
//OSPEVENT( MCUVC_HDUPOLLING_CHANGE_TIMER,        EV_MCU_BGN + 89 );          //hdu轮询改变时钟

// xliang [12/12/2008] vmp hd通道抢占中，要抢占的终端进行抢占定时
OSPEVENT( MCUVC_MTSEIZEVMP_TIMER,				EV_MCU_BGN + 90 );
// xliang [12/22/2008] VMP 批量轮询定时
OSPEVENT( MCUVC_VMPBATCHPOLL_TIMER,				EV_MCU_BGN + 91 );

// hdu批量轮询定时
OSPEVENT( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER,     EV_MCU_BGN + 92 );          //hdu批量轮询改变时钟

OSPEVENT( MCUVC_VCMTOVERTIMER_TIMER,            EV_MCU_BGN + 93 );          //调度终端超时定时器
OSPEVENT( MCUVC_VCS_CHAIRPOLL_TIMER,            EV_MCU_BGN + 94 );          //VCS主席论询定时器

//MCU等待适配器应答定时, 因为bas有5种类型，要预留5个空间
OSPEVENT( MCUVC_WAITBASRRSP_TIMER,			    EV_MCU_BGN + 95 );
//MCU给终端发FASTUPDATE的定时器组 留199的空间
OSPEVENT( MCUVC_FASTUPDATE_TIMER_ARRAY,	        EV_MCU_BGN + 100 );


//MCU内部MP交换能力满 通知消息
OSPEVENT( MCUVC_MPOVERLOAD_NOTIF,               EV_MCU_BGN + 298 );
//MCU给终端发双流FASTUPDATE的定时器
OSPEVENT( MCUVC_SECVIDEO_FASTUPDATE_TIMER,  	EV_MCU_BGN + 299 );

//MCU会议开始自动录像定时器，
OSPEVENT( MCUVC_CONFSTARTREC_TIMER,             EV_MCU_BGN + 300 ); 
//MCU开始放像定时器, zgc, 2008-03-27
OSPEVENT( MCUVC_RECPLAY_WAITMPACK_TIMER,		EV_MCU_BGN + 301 );
//MCU每天凌晨0点，设置该定时器，用它来刷新该天的自动录像定时器, pengjie, 20091225
OSPEVENT( MCUVC_UPDATA_AUTOREC_TIMER,		EV_MCU_BGN + 302 );

//双流放像，mcu定时发令牌环的定时器 [pengjie 2010/2/26]
OSPEVENT( MCUVC_NOTIFYMCUH239TOKEN_TIMER, 	EV_MCU_BGN + 303 );

// [pengjie 2010/9/29] 检查会议是否有广播源
OSPEVENT( MCUVC_CHECK_CONFBRDSRC_TIMER,		EV_MCU_BGN + 304 );

//MCU等待HD主视频适配器应答定时, 因为会议hd-bas主视频适配最多有4路，要预留3个空间（从0开始）
OSPEVENT( MCUVC_WAITHDVIDBASRSP_TIMER,			EV_MCU_BGN + 305 );
//MCU等待HD双流适配器应答定时, 因为会议hd-bas双流适配最多有1路
OSPEVENT( MCUVC_WAITHDDVIDBASRSP_TIMER,			EV_MCU_BGN + 310 );

//卫星会议主控的部分定时
OSPEVENT( TIMER_SCHEDULED_CHECK,                EV_MCU_BGN + 311 );
OSPEVENT( TIMER_DELAYCONF,                      EV_MCU_BGN + 312 );
OSPEVENT( TIMER_RESCHED,                        EV_MCU_BGN + 313 );
OSPEVENT( TIMER_ADDMT,                          EV_MCU_BGN + 314 );
OSPEVENT( TIMER_DELMT,                          EV_MCU_BGN + 315 );
OSPEVENT( TIMER_INVITE_UNJOINEDMT,              EV_MCU_BGN + 316 );
OSPEVENT( TIMER_SCHEDCONF,                      EV_MCU_BGN + 317 );

//卫星会议，定时刷新; 卫星会议终端链路的KeepAlive基于本消息处理
OSPEVENT( MCUVC_CONFINFO_MULTICAST_TIMER,		EV_MCU_BGN + 318 );

//发给MCU内部会议实例通知Hdu建链成功，消息体为TPeriEqpRegReq
OSPEVENT( MCU_HDUCONNECTED_NOTIF,		        EV_MCU_BGN + 350 );
//发给MCU内部会议实例通知Hdu断链，消息体为TEqp
OSPEVENT( MCU_HDUDISCONNECTED_NOTIF,		    EV_MCU_BGN + 351 );
//mcucfg等待DSC修改配置结束超时定时器, zgc, 2007-06-23
OSPEVENT( MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER,	EV_MCU_BGN + 352 );
//会控会话通知mcucfg断链消息, zgc, 2007-11-09
//消息体：无
OSPEVENT( MCSSSN_MCUCFG_MCSDISSCONNNECT_NOTIFY,		EV_MCU_BGN + 353 );

// [11/14/2011 liuxu]缓冲发送AllMtInfoToAllMcs的timer
OSPEVENT( MCUVC_SENDALLMT_ALLMCS_TIMER,	        EV_MCU_BGN + 354 );

// [11/14/2011 liuxu]缓冲发送AllMtInfoToAllMcs的timer
OSPEVENT( MCUVC_SEND_SMCUMTSTATUS_TIMER,	    EV_MCU_BGN + 355 );


//MCU给外设发FASTUPDATE的定时器组 留48个空间 _(+360 ～+408)
//不考虑HDU、Tvwall、Recorder、Mixer、Prs，只考虑bas/hdbas，vmp/hdvmp，vmptw，暂不区分其通道ID
OSPEVENT( MCUVC_FASTUPDATE_TIMER_ARRAY4EQP,     EV_MCU_BGN + 360 );


/*
*	Special For Satellate MCU
*/
OSPEVENT( MCU_MULTICAST_MSG,				    EV_MCU_BGN + 410 );

OSPEVENT( MCU_MULTICAST_CONFLIST,				EV_MCU_BGN + 411 );

/////////////////////////////////////////////////////////////////////////
//mcu内部消息
//电视墙多通道轮询定时器(包括所有dec5, hdu)预留80个空间(412 ~ 491)
OSPEVENT(MCUVC_TWPOLLING_CHANGE_TIMER ,		    EV_MCU_BGN + 412 );

//////////////////////////////////////////////////////////////////////////
//  mcuhdu内部消息
//
//  通知hdu开始交换码流
//  消息体  THduSwitchInfo
OSPEVENT( MCUVC_STARTSWITCHHDU_NOTIFY,		  EV_MCU_BGN + 492 );
 
///////////////////////////////////////////////////////////
// mcuvcs内部消息
// 上级MCU释放对下级调度会议控制的权限      消息体:无(消息头中含有指定会议的CONFID)
OSPEVENT( MCUVC_VCSSN_MMCURLSCTRL_CMD,	 EV_MCU_BGN + 493 );
// 上级MCU开始接管对会议的调度权限          消息体:无
OSPEVENT( MCUVC_MMCUGETCTRL_CMD,             EV_MCU_BGN + 494 );

///////////////////////////////////////////////////////////
//主备位板MCU之间的主备倒换的通讯消息

//主备倒换监守模块启动消息
OSPEVENT( MCU_MSEXCHANGER_POWERON_CMD,	EV_MCU_BGN + 500 );

//备位板MCU向主位板MCU 的连接定时器
OSPEVENT( MCU_MCU_CONNECT_TIMER,		EV_MCU_BGN + 501 );
//备位板MCU向主位板MCU 的注册定时器
OSPEVENT( MCU_MCU_REGISTER_TIMER,		EV_MCU_BGN + 502 );

//vxworks下检测主备用状态的定时器
OSPEVENT( MCU_MCU_CEHCK_MSSTATE_TIMER,	EV_MCU_BGN + 503 );

//主用板MCU向备用板MCU 的数据倒换定时器
OSPEVENT( MCU_MCU_SYN_TIMER,		    EV_MCU_BGN + 504 );

//MCU之间 等待对端的请求应答 的超时定时器
OSPEVENT( MCU_MCU_WAITFORRSP_TIMER,	    EV_MCU_BGN + 505 );

//主MCU向备mcu发重启命令
OSPEVENT( MCU_MCU_REBOOT_CMD,	        EV_MCU_BGN + 506 );
//主MCU向备mcu发升级重启命令
OSPEVENT( MCU_MCU_UPDATE_CMD,	        EV_MCU_BGN + 509 );

//mcuagt向备mcu发送重启备份mcu的命令
OSPEVENT( MCUAGT_MCU_REBOOT_CMD,	    EV_MCU_BGN + 507 );

//MCU之间 ROUNDTRIP链路检测 的超时定时器
OSPEVENT( MCU_MCU_ROUNDTRIP_TIMER,	    EV_MCU_BGN + 508 );

//备位板MCU向主位板MCU 的注册请求 消息体: u32 dwLocalIp
OSPEVENT( MCU_MCU_REGISTER_REQ,			EV_MCU_BGN + 511 );
//备位板MCU向主位板MCU 的注册请求接受
OSPEVENT( MCU_MCU_REGISTER_ACK,			EV_MCU_BGN + 512 );
//备位板MCU向主位板MCU 的注册请求拒绝
OSPEVENT( MCU_MCU_REGISTER_NACK,		EV_MCU_BGN + 513 );

//MCU之间 通知 对方 本地MCU的主备位类型及当前的主备使用状态，进行主备用协商 消息体: u8 LocalMSType + u8 CurMSState
OSPEVENT( MCU_MCU_MS_DETERMINE,			EV_MCU_BGN + 516 );
//MCU之间 通知 对方 本地MCU的协商的主备使用结果 消息体: u8 CurMSState
OSPEVENT( MCU_MCU_MS_RESULT,			EV_MCU_BGN + 517 );

//主用板MCU向备用板MCU 请求 开始数据倒换 消息体: TMSSynInfoReq
OSPEVENT( MCU_MCU_START_SYN_REQ,		EV_MCU_BGN + 521 );
//备用板MCU向主用板MCU 接受 开始数据倒换 消息体: TMSSynInfoRsp
OSPEVENT( MCU_MCU_START_SYN_ACK,		EV_MCU_BGN + 522 );
//备位板MCU向主位板MCU 配置信息同步请求拒绝 消息体: TMSSynState
OSPEVENT( MCU_MCU_START_SYN_NACK,		EV_MCU_BGN + 523 );
//主用板MCU向备用板MCU 结束数据倒换 通知
OSPEVENT( MCU_MCU_END_SYN_NTF,			EV_MCU_BGN + 524 );

//主位板MCU向备位板MCU 实际切包组包的数据倒换 同步 请求 消息体: TMSSynDataReqHead
OSPEVENT( MCU_MCU_SYNNING_DATA_REQ,		EV_MCU_BGN + 526 );
//备位板MCU向主位板MCU 实际切包组包的数据倒换 同步 回应 消息体: TMSSynDataRsp
OSPEVENT( MCU_MCU_SYNNING_DATA_RSP,		EV_MCU_BGN + 527 );

//备用板升级为主用板时 通知业务和各个会话模块主备用状态变更，消息体: u8是否切换成功
OSPEVENT( MCU_MSSTATE_EXCHANGE_NTF,		EV_MCU_BGN + 531 );

//主备探测消息
OSPEVENT( MCU_MCU_PROBE_REQ,            EV_MCU_BGN + 532 );
OSPEVENT( MCU_MCU_PROBE_ACK,            EV_MCU_BGN + 533 );  

//备mcu向主mcu发LED灯显的notify。消息体：TBrdLedState
OSPEVENT( MCU_MCU_LEDSTATUS_NOTIFY,	    EV_MCU_BGN + 534 );

OSPEVENT( EV_MCU_OPENSECVID_TIMER,		EV_MCU_BGN + 535 );//目前只针对polycom

//会议支持多vmp，vmp相关Timer需扩为16份，预留32个(608-639)
//包含MCUVC_VMP_WAITVMPRSP_TIMER与MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER
OSPEVENT( MCUVC_VMP_WAITVMPRSP_TIMER,        EV_MCU_BGN + 608 );
OSPEVENT( MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER,  EV_MCU_BGN + 624 );

//N+1模式 MCU内部消息
OSPEVENT( NPLUS_VC_DATAUPDATE_NOTIF,        EV_MCU_BGN + 640 );
//mcu业务发给主mcu备份管理模块消息
OSPEVENT( VC_NPLUS_MSG_NOTIF,               EV_MCU_BGN + 641 );
//通知N+1备份服务器重置，停止备份服务，但不断开连接的客户端
OSPEVENT( VC_NPLUS_RESET_NOTIF,             EV_MCU_BGN + 642 );

//N+1模式 mcu间消息
//连接定时器
OSPEVENT( MCU_NPLUS_CONNECT_TIMER,          EV_MCU_BGN + 650 );

//注册定时器
OSPEVENT( MCU_NPLUS_REG_TIMER,              EV_MCU_BGN + 651 );

//链路检测超时定时器
OSPEVENT( MCU_NPLUS_RTD_TIMER,              EV_MCU_BGN + 652 );

//注册消息，消息体：TCMcuRegInfo
OSPEVENT( MCU_NPLUS_REG_REQ,                EV_MCU_BGN + 700 );
OSPEVENT( MCU_NPLUS_REG_ACK,                EV_MCU_BGN + 701 );
OSPEVENT( MCU_NPLUS_REG_NACK,               EV_MCU_BGN + 702 );

//链路检测消息，消息体：无
OSPEVENT( MCU_NPLUS_RTD_REQ,                EV_MCU_BGN + 703 );
OSPEVENT( MCU_NPLUS_RTD_RSP,                EV_MCU_BGN + 704 );

//mcu会议信息通知消息（创建/结束会议时通知），消息体：u8(NPLUS_CONF_START, NPLUS_CONF_RELEASE)+TConfInfo
OSPEVENT( MCU_NPLUS_CONFINFOUPDATE_REQ,     EV_MCU_BGN + 705 );
OSPEVENT( MCU_NPLUS_CONFINFOUPDATE_ACK,     EV_MCU_BGN + 706 );
OSPEVENT( MCU_NPLUS_CONFINFOUPDATE_NACK,    EV_MCU_BGN + 707 );

//mcu用户组信息通知消息 （注册成功和用户组信息变化时或会议回滚时通知），消息体：CUsrGrpsInfo
OSPEVENT( MCU_NPLUS_USRGRPUPDATE_REQ,         EV_MCU_BGN + 708 );
OSPEVENT( MCU_NPLUS_USRGRPUPDATE_ACK,         EV_MCU_BGN + 709 );
OSPEVENT( MCU_NPLUS_USRGRPUPDATE_NACK,        EV_MCU_BGN + 710 );

//与会终端信息通知（与会终端有变化时通知），消息体：TMtInfo[]
OSPEVENT( MCU_NPLUS_CONFMTUPDATE_REQ,       EV_MCU_BGN + 711 );
OSPEVENT( MCU_NPLUS_CONFMTUPDATE_ACK,       EV_MCU_BGN + 712 );
OSPEVENT( MCU_NPLUS_CONFMTUPDATE_NACK,      EV_MCU_BGN + 713 );

//主席终端通知，消息体：TMtAlias
OSPEVENT( MCU_NPLUS_CHAIRUPDATE_REQ,        EV_MCU_BGN + 714 );
OSPEVENT( MCU_NPLUS_CHAIRUPDATE_ACK,        EV_MCU_BGN + 715 );
OSPEVENT( MCU_NPLUS_CHAIRUPDATE_NACK,       EV_MCU_BGN + 716 );

//发言终端通知，消息体：TMtAlias
OSPEVENT( MCU_NPLUS_SPEAKERUPDATE_REQ,      EV_MCU_BGN + 717 );
OSPEVENT( MCU_NPLUS_SPEAKERUPDATE_ACK,      EV_MCU_BGN + 718 );
OSPEVENT( MCU_NPLUS_SPEAKERUPDATE_NACK,     EV_MCU_BGN + 719 );

//VMP，消息体：TNPlusVmpInfo+TVMPParam
OSPEVENT( MCU_NPLUS_VMPUPDATE_REQ,          EV_MCU_BGN + 720 );
OSPEVENT( MCU_NPLUS_VMPUPDATE_ACK,          EV_MCU_BGN + 721 );
OSPEVENT( MCU_NPLUS_VMPUPDATE_NACK,         EV_MCU_BGN + 722 );

//ConfData通知消息（注册成功时或会议回滚时通知），消息体：TNPlusConfData + N字节扩展数据+MAXNUM_SUB_MCU*TSmcuCallInfo
OSPEVENT( MCU_NPLUS_CONFDATAUPDATE_REQ,       EV_MCU_BGN + 723 );
OSPEVENT( MCU_NPLUS_CONFDATAUPDATE_ACK,       EV_MCU_BGN + 724 );
OSPEVENT( MCU_NPLUS_CONFDATAUPDATE_NACK,      EV_MCU_BGN + 725 );

//用户信息通知消息（注册成功时或会议回滚时通知），消息体：CExUsrInfo[]，需要考虑切包
OSPEVENT( MCU_NPLUS_USRINFOUPDATE_REQ,        EV_MCU_BGN + 726 );
OSPEVENT( MCU_NPLUS_USRINFOUPDATE_ACK,        EV_MCU_BGN + 727 );
OSPEVENT( MCU_NPLUS_USRINFOUPDATE_NACK,       EV_MCU_BGN + 728 );

//会议信息回滚
OSPEVENT( MCU_NPLUS_CONFROLLBACK_REQ,        EV_MCU_BGN + 729 );
OSPEVENT( MCU_NPLUS_CONFROLLBACK_ACK,        EV_MCU_BGN + 730 );
OSPEVENT( MCU_NPLUS_CONFROLLBACK_NACK,       EV_MCU_BGN + 731 );

//用户信息回滚
OSPEVENT( MCU_NPLUS_USRROLLBACK_REQ,        EV_MCU_BGN + 732 );
OSPEVENT( MCU_NPLUS_USRROLLBACK_ACK,        EV_MCU_BGN + 733 );
OSPEVENT( MCU_NPLUS_USRROLLBACK_NACK,       EV_MCU_BGN + 734 );

//用户组信息回滚
OSPEVENT( MCU_NPLUS_GRPROLLBACK_REQ,        EV_MCU_BGN + 735 );
OSPEVENT( MCU_NPLUS_GRPROLLBACK_ACK,        EV_MCU_BGN + 736 );
OSPEVENT( MCU_NPLUS_GRPROLLBACK_NACK,       EV_MCU_BGN + 737 );

//GK实体注册通知消息，消息体：TRASInfo
OSPEVENT( MCU_NPLUS_RASINFOUPDATE_REQ,      EV_MCU_BGN + 738 );
OSPEVENT( MCU_NPLUS_RASINFOUPDATE_ACK,      EV_MCU_BGN + 739 );
OSPEVENT( MCU_NPLUS_RASINFOUPDATE_NACK,     EV_MCU_BGN + 740 );

//AutoMix，消息体：BOOL32
OSPEVENT( MCU_NPLUS_AUTOMIXUPDATE_REQ,          EV_MCU_BGN + 741 );
OSPEVENT( MCU_NPLUS_AUTOMIXUPDATE_ACK,          EV_MCU_BGN + 742 );
OSPEVENT( MCU_NPLUS_AUTOMIXUPDATE_NACK,         EV_MCU_BGN + 743 );

//同步下级MCU呼叫信息 消息体：TSmcuCallInfo[]数组
OSPEVENT( MCU_NPLUS_SMCUINFOUPDATE_REQ,         EV_MCU_BGN + 744 );
OSPEVENT( MCU_NPLUS_SMCUINFOUPDATE_ACK,			EV_MCU_BGN + 745 );
OSPEVENT( MCU_NPLUS_SMCUINFOUPDATE_NACK,		EV_MCU_BGN + 746 );

//外设基本能力通知，消息体：TNPlusEqpCap
OSPEVENT( MCU_NPLUS_EQPCAP_NOTIF,           EV_MCU_BGN + 750 );

//MCU接收VCS发送的分组信息定时器 留MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE(128个 +800 -- +927)
OSPEVENT( MCUVC_PACKINFO_TIMER,             EV_MCU_BGN + 800);

// [1/7/2011 xliang] 短消息分批发送定时
OSPEVENT( MCUVC_SMSPACK_TIMER,				EV_MCU_BGN + 950);

// [pengjie 2011/5/18]消息951~953在其它流上已经被占用，考虑到功能合并等问题，这里将其预留

// [pengjie 2011/5/19] 有vcsssn普通instan发给daemon的开启定时命令，消息体为 u8 byTimeIdx 定时器Id
OSPEVENT( MCUVC_STARTRELEASECONFTIMER_CMD,			EV_MCU_BGN + 954);
// [pengjie 2011/5/18] vcs断链延迟结会定时器，需要占用16个timer( 955 ~ 971 )
OSPEVENT( MCUVC_RELEASECONF_TIMER,			EV_MCU_BGN + 955);

// END (EV_MCU_BGN + 971)
//终端请求H239token等待开双流通道定时器
OSPEVENT( MCUVC_WAITOPENDSCHAN_TIMER,           EV_MCU_BGN + 972 );
//调整画面合成成员如果改变的所有的成员都为下级终端（级联多回传），扩到16个
//OSPEVENT( MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER,			    EV_MCU_BGN + 973 );
//主备环境，主板重启后需要恢复会议后，需要给备板同步数据
OSPEVENT( MCUVC_RECOVERCONF_SYN_TIMER,           EV_MCU_BGN + 974 );
//多回传，点名与被点名人模式下，点下级终端的时候需要等待下级终端会ack才认为可以点下个终端
OSPEVENT( MCUVC_WAIT_LASTVMPROLLCALLFINISH_TIMER,           EV_MCU_BGN + 975 );
//主备备份会议数据的时候，给instance设置（ongoing或schedule）状态后，起等待会议数据timer
OSPEVENT( MCUVC_WAITMSCONFDATA_TIMER,           EV_MCU_BGN + 976 );
///////////////////////////////////////////////////////////

#endif /* _EV_MCU_H_ */
