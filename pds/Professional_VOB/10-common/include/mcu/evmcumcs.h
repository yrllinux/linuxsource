/*****************************************************************************
   模块名      : mcu
   文件名      : evmcumcs.h
   相关文件    : 
   文件实现功能: MCU与会议控制台接口消息定义
   作者        : 胡昌威
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/10/27  3.0         胡昌威      创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/
#ifndef _EVMCUMCS_H_
#define _EVMCUMCS_H_

#include "osp.h"
#include "eventid.h"

/*------------------------------------------------------------------------------
 MCU与会议控制台接口消息（22001-23000）
------------------------------------------------------------------------------*/

//3.1	建立连接

//3.1.1	建链
    //控制台通知MCU已连接，请求准入，消息体: TMcsRegInfo
    OSPEVENT( MCS_MCU_CONNECT_REQ,		    EV_MCSVC_BGN + 1 );	
    //MCU准入应答，消息体: TMcu + TMtAlias(MCU的别名) + dwAnotherMpcIP(另一块MpcIP,网络序)
    //                    + byMaster(1-当前Mpc主用,0-当前Mpc备用) 
    //                    + u8 byGrpId(用户所属的组Id) 
    //                    + u8 OS 类型(OS_TYPE_VXWORKS等)
	//					 如果是8000BMCU (TMcu.GetMcuType() == MCU_TYPE_KDV8000B
	//					  + TMcu8kbPfmFilter CMcuPfmLmt::m_tFilter(8000b性能限制结构体,请自行保存并设置)
    OSPEVENT( MCU_MCS_CONNECT_ACK,		    EV_MCSVC_BGN + 2 );
    //MCU拒绝应答，消息体无
    OSPEVENT( MCU_MCS_CONNECT_NACK,		    EV_MCSVC_BGN + 3 );

//3.1.2	时间同步
    //会议控制台查询mcu本地时间，无消息体
    OSPEVENT( MCS_MCU_CURRTIME_REQ,		    EV_MCSVC_BGN + 5 );
    //mcu成功应答，消息体为TKdvTime
    OSPEVENT( MCU_MCS_CURRTIME_ACK,		    EV_MCSVC_BGN + 6 );
    //mcu拒绝应答，无消息体
    OSPEVENT( MCU_MCS_CURRTIME_NACK,        EV_MCSVC_BGN + 7 );

//3.1.3	查询MCU状态
    //查询MCU状态命令，消息体: 无
    OSPEVENT( MCS_MCU_GETMCUSTATUS_CMD,     EV_MCSVC_BGN + 9 );
	//查询MCU状态请求，消息体: 无
    OSPEVENT( MCS_MCU_GETMCUSTATUS_REQ,	    EV_MCSVC_BGN + 10 );
    //返回的成功应答，消息体: TMcuStatus
    OSPEVENT( MCU_MCS_GETMCUSTATUS_ACK,     EV_MCSVC_BGN + 11 );
    //返回的失败应答，消息体: 无
    OSPEVENT( MCU_MCS_GETMCUSTATUS_NACK,    EV_MCSVC_BGN + 12 );
    //MCU状态通知，消息体为结构TMcuStatus
    OSPEVENT( MCU_MCS_MCUSTATUS_NOTIF,	    EV_MCSVC_BGN + 13 );
    
    //查询mcu主备状态，消息体：无
    OSPEVENT( MCS_MCU_GETMSSTATUS_REQ,      EV_MCSVC_BGN + 14 );
    //消息体：TMcuMsStatus
    OSPEVENT( MCU_MCS_GETMSSTATUS_ACK,      EV_MCSVC_BGN + 15 );

	//消息体：byMtId+tSrcMt+byMtId+tSrcMt...
	OSPEVENT(MCU_MCS_MTSECSEL_NOTIFY,      EV_MCSVC_BGN + 16 );
	//消息体：TMt
	OSPEVENT(MCU_MCS_SECSPEAKER_NOTIFY,      EV_MCSVC_BGN + 17 );
    
//3.1.4 上传文件(mcu.z, mcuos and mcu.ios)
    //MCS上传文件完成后更名请求, 消息体: u8(byLayer) + u8(bySlot) + u8(bySlotType) + u16(wLen) + s8[wLen]
    // ~~ as seperate between file name in s8[wLen]
    // demo: 0 0 1 21 'mcu.zudp~~mcuosudp~~mcu.iosudp\0'	
//    OSPEVENT( MCS_MCU_UPLOADFILENAME_REQ,   EV_MCSVC_BGN + 17 );
    //MCU返回的成功应答, 消息体：无
//    OSPEVENT( MCU_MCS_UPLOADFILENAME_ACK,   EV_MCSVC_BGN + 18 );
    //MCU返回的失败应答, 消息体：无
//    OSPEVENT( MCU_MCS_UPLOADFILENAME_NACK,  EV_MCSVC_BGN + 19 );

//3.2	会议管理
//3.2.1	请求会议列表	
    //会议控制台查询MCU上的会议列表，无消息体 
    OSPEVENT( MCS_MCU_LISTALLCONF_REQ,		EV_MCSVC_BGN + 20 );
    //应答MCS_MCU_LISTALLCONF_REQ，无消息体，发完MCU_MCS_CONF_NOTIF后 
    OSPEVENT( MCU_MCS_LISTALLCONF_ACK,		EV_MCSVC_BGN + 21 );
    //MCU上没有会议,无消息体
    OSPEVENT( MCU_MCS_LISTALLCONF_NACK,     EV_MCSVC_BGN + 22 );
    //会议完整信息通知,消息体: TConfInfo
	//若是预约会议,则有可能有以下两字段:
	//                  + TTvWallModule(可选字段，由会议属性中的是否有电视墙模板决定) 
	//                  + TVmpModule(可选字段，由会议属性中的是否有画面合成墙模板决定) 
    OSPEVENT( MCU_MCS_CONFINFO_NOTIF,       EV_MCSVC_BGN + 23 );

    //会议简单信息通知，消息体：TBasicConfInfo
    OSPEVENT( MCU_MCS_SIMCONFINFO_NOTIF,    EV_MCSVC_BGN + 24 );

//3.2.2	创建会议 	
    //会议控制台在MCU上创建一个会议  
	//消息体: TConfInfo + 2 byte(网络序,TMtAlias数组打包长度,即下面的n)
	//                  + n byte(TMtAlias数组打包,每包为 1byte(类型)+1byte(长度)+xbyte(字符串)+2byte(呼叫码率) 
	//                            第1字节为TMtAlias类型,第2字节为长度x,下面的x字节为TMtAlias内容)
	//                  + TTvWallModule(可选字段，由会议属性中的是否有电视墙模板决定) 
	//                  + TVmpModule(可选字段，由会议属性中的是否有画面合成墙模板决定)                  
    OSPEVENT( MCS_MCU_CREATECONF_REQ,		EV_MCSVC_BGN + 25 );
    //会议控制台在MCU上创建一个会议成功应答, 消息体: TConfInfo,
    //注意:成功后可能同时有提示性错误码
    OSPEVENT( MCU_MCS_CREATECONF_ACK,		EV_MCSVC_BGN + 26 );
    //会议控制台在MCU上创建一个会议失败, 无消息体
    OSPEVENT( MCU_MCS_CREATECONF_NACK,      EV_MCSVC_BGN + 27 );
	
    //会议控制台在MCU上按会议模板创建一个即时会议, 无消息体                
    OSPEVENT( MCS_MCU_CREATECONF_BYTEMPLATE_REQ,		EV_MCSVC_BGN + 28 );
    //会议模板成功地转变成即时会议, 无消息体 
    OSPEVENT( MCU_MCS_CREATECONF_BYTEMPLATE_ACK,		EV_MCSVC_BGN + 29 );
    //会议模板转变成即时会议失败, 无消息体
    OSPEVENT( MCU_MCS_CREATECONF_BYTEMPLATE_NACK,       EV_MCSVC_BGN + 30 );

    //会议控制台在MCU上按会议模板创建一个预约会议, 消息体: TKdvTime   (开始时间)             
    OSPEVENT( MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ,		EV_MCSVC_BGN + 31 );
    //会议模板成功地转变成预约会议, 无消息体 
    OSPEVENT( MCU_MCS_CREATESCHCONF_BYTEMPLATE_ACK,		EV_MCSVC_BGN + 32 );
    //会议模板转变成预约会议失败, 无消息体
    OSPEVENT( MCU_MCS_CREATESCHCONF_BYTEMPLATE_NACK,    EV_MCSVC_BGN + 33 );

    //会议在GK上注册成功通知, 消息体:u8 (1:成功，0:失败)
    OSPEVENT( MCU_MCS_CONFREGGKSTATUS_NOTIF,            EV_MCSVC_BGN + 34 );
    
//3.2.3	结束会议 
    //会议控制台请求MCU结束一个会议, 无消息体
    OSPEVENT( MCS_MCU_RELEASECONF_REQ,		    EV_MCSVC_BGN + 35 );
    //MCU成功结束会议应答, 无消息体
    OSPEVENT( MCU_MCS_RELEASECONF_ACK,		    EV_MCSVC_BGN + 36 );
    //MCU拒绝结束会议, 无消息体
    OSPEVENT( MCU_MCS_RELEASECONF_NACK,         EV_MCSVC_BGN + 37 );
    //MCU结束会议通知, 消息体: u8 (takemode: 见宏定义) CONF_TAKEMODE_SCHEDULED, CONF_TAKEMODE_ONGOING，预约会议或即时会议
    OSPEVENT( MCU_MCS_RELEASECONF_NOTIF,        EV_MCSVC_BGN + 38 );

    //有上级MCU呼叫通知，消息体：u32(对端IP，网络序)
    OSPEVENT( MCU_MCS_MMCUCALLIN_NOTIF,         EV_MCSVC_BGN + 39 );
	
//3.2.4	保护会议
//3.2.4.1	锁定会议
    //会议控制台请求MCU改变会议保护方式, 消息体:1byte(会议保护方式) 
    OSPEVENT( MCS_MCU_CHANGECONFLOCKMODE_REQ,   EV_MCSVC_BGN + 40 );
    //MCU成功改变会议保护方式会议应答, 消息体:1byte(会议保护方式)
    OSPEVENT( MCU_MCS_CHANGECONFLOCKMODE_ACK,   EV_MCSVC_BGN + 41 );
    //MCU拒绝改变会议保护方式会议, 无消息体
    OSPEVENT( MCU_MCS_CHANGECONFLOCKMODE_NACK,  EV_MCSVC_BGN + 42 );
    //消息体：1byte(会议保护方式，会议号在CServMsg中)
    OSPEVENT( MCU_MCS_CONFLOCKMODE_NOTIF,       EV_MCSVC_BGN + 43 );

//3.2.4.8	会议独享 
	//得到会议独享消息
	OSPEVENT( MCS_MCU_GETLOCKINFO_REQ,      EV_MCSVC_BGN + 45 );
	//会议独享消息应答
	//消息体: 4byte(独享会控的IP,网络序)+32byte(独享会控的用户名)
	OSPEVENT( MCU_MCS_GETLOCKINFO_ACK,      EV_MCSVC_BGN + 46 );
	//得到会议独享拒绝
	OSPEVENT( MCU_MCS_GETLOCKINFO_NACK,     EV_MCSVC_BGN + 47 );
    //MCU给会议控制台的当前独享会议的控制台的信息,
	//消息体: 4byte(独享会控的IP,网络序)+32byte(独享会控的用户名)
    OSPEVENT( MCU_MCS_LOCKUSERINFO_NOTIFY,  EV_MCSVC_BGN + 48 );

//3.2.4.2	输入密码
    //MCU要求会议控制台输入会议密码, 无消息体 
    OSPEVENT( MCU_MCS_ENTERCONFPWD_REQ,		EV_MCSVC_BGN + 55 );
    //会议控制台回应MCU的密码请求, 消息体: 32byte(会议密码)
    OSPEVENT( MCS_MCU_ENTERCONFPWD_ACK,		EV_MCSVC_BGN + 56 );
    //会议控制台拒绝MCU的密码请求, 无消息体
    OSPEVENT( MCS_MCU_ENTERCONFPWD_NACK,    EV_MCSVC_BGN + 57 );
    //MCU提示会议控制台密码错误, 无消息体
    OSPEVENT( MCU_MCS_WRONGCONFPWD_NOTIF,   EV_MCSVC_BGN + 58 );

//3.2.4.3	更改会议密码 
    //会议控制台请求MCU更改会议密码, 消息体:32byte字符串(会议密码) 
    OSPEVENT( MCS_MCU_CHANGECONFPWD_REQ,	EV_MCSVC_BGN + 61 );
    //MCU成功更改会议密码应答, 消息体:32byte字符串(会议密码
    OSPEVENT( MCU_MCS_CHANGECONFPWD_ACK,	EV_MCSVC_BGN + 62 );
    //MCU拒绝更改会议密码, 无消息体
    OSPEVENT( MCU_MCS_CHANGECONFPWD_NACK,   EV_MCSVC_BGN + 63 );
    //消息体:32byte字符串(会议密码)
    OSPEVENT( MCU_MCS_CHANGECONFPWD_NOTIF,  EV_MCSVC_BGN + 64 );

//3.2.5	保存会议 
    //会议控制台请求MCU保存会议, 消息体: 1byte(1:保存为缺省会议,0:不存为缺省会议) 
    OSPEVENT( MCS_MCU_SAVECONF_REQ,		    EV_MCSVC_BGN + 65 );
    //MCU成功保存会议应答, 消息体:1byte(1:保存为会议模板,0:不存为会议模板) 
    OSPEVENT( MCU_MCS_SAVECONF_ACK,		    EV_MCSVC_BGN + 66 );
    //MCU拒绝保存会议, 无消息体
    OSPEVENT( MCU_MCS_SAVECONF_NACK,        EV_MCSVC_BGN + 67 );
	

//3.2.6	修改预约会议 
    //会议控制台在MCU上创建一个会议，消息体同 MCS_MCU_CREATECONF_REQ
	OSPEVENT( MCS_MCU_MODIFYCONF_REQ,		EV_MCSVC_BGN + 69 );
    //MCU成功修改预约会议, 无消息体
    OSPEVENT( MCU_MCS_MODIFYCONF_ACK,		EV_MCSVC_BGN + 70 );
    //MCU拒绝修改预约会议, 无消息体
    OSPEVENT( MCU_MCS_MODIFYCONF_NACK,      EV_MCSVC_BGN + 71 ); 
    
    //会控创建模板，消息体同 MCS_MCU_CREATECONF_REQ
    OSPEVENT( MCS_MCU_CREATETEMPLATE_REQ,   EV_MCSVC_BGN + 73 );
    //消息体：无
    OSPEVENT( MCU_MCS_CREATETEMPLATE_ACK,   EV_MCSVC_BGN + 74 );
    //消息体：无
    OSPEVENT( MCU_MCS_CREATETEMPLATE_NACK,  EV_MCSVC_BGN + 75 );

    //会控修改模板，消息体同 MCS_MCU_CREATECONF_REQ
    OSPEVENT( MCS_MCU_MODIFYTEMPLATE_REQ,   EV_MCSVC_BGN + 77 );
    //消息体：无
    OSPEVENT( MCU_MCS_MODIFYTEMPLATE_ACK,   EV_MCSVC_BGN + 78 );
    //消息体：无
    OSPEVENT( MCU_MCS_MODIFYTEMPLATE_NACK,  EV_MCSVC_BGN + 79 );

    //会控删除模板，消息体：无
    OSPEVENT( MCS_MCU_DELTEMPLATE_REQ,      EV_MCSVC_BGN + 81 );
    //消息体：无
    OSPEVENT( MCU_MCS_DELTEMPLATE_ACK,      EV_MCSVC_BGN + 82 );
    //消息体：无
    OSPEVENT( MCU_MCS_DELTEMPLATE_NACK,     EV_MCSVC_BGN + 83 );
    //消息体：无
    OSPEVENT( MCU_MCS_DELTEMPLATE_NOTIF,    EV_MCSVC_BGN + 84 );

	//把会议另存为模板, 消息体: 无	---预留消息接口, 满足以后的需求, zgc, 2007/04/20
	OSPEVENT( MCS_MCU_SAVECONFTOTEMPLATE_REQ,	EV_MCSVC_BGN + 85 );
	//消息体: 无
	OSPEVENT( MCU_MCS_SAVECONFTOTEMPLATE_ACK,	EV_MCSVC_BGN + 86 );
	//消息体: 无
	OSPEVENT( MCU_MCS_SAVECONFTOTEMPLATE_NACK,	EV_MCSVC_BGN + 87 );

    //模板或预约会议信息通知，消息体同 MCS_MCU_CREATECONF_REQ    
    OSPEVENT( MCU_MCS_TEMSCHCONFINFO_NOTIF, EV_MCSVC_BGN + 88 );

    //单独列出，MCS接收外设信息通知    
    OSPEVENT( MCU_MCS_MCUPERIEQPINFO, EV_MCSVC_BGN + 89 );

//3.2.7	延长会议 
	//会议控制台请求MCU延长会议, 消息体: 1 u16 (网络序, 延长时间, 单位: 分钟)
	OSPEVENT( MCS_MCU_DELAYCONF_REQ,        EV_MCSVC_BGN + 90 );
	//MCU成功延长会议应答, 消息体: 1 u16 (网络序，延长时间，单位：分钟) 
	OSPEVENT( MCU_MCS_DELAYCONF_ACK,        EV_MCSVC_BGN + 91 );
	//MCU拒绝延长会议应答
	OSPEVENT( MCU_MCS_DELAYCONF_NACK,       EV_MCSVC_BGN + 92 );
	//MCU延长会议通知, 消息体: 1 u16 (网络序，延长时间，单位：分钟)
	OSPEVENT( MCU_MCS_DELAYCONF_NOTIF,      EV_MCSVC_BGN + 93 );	
	//MCU通知会控会议结束剩余时间, 消息体: 1 u16 (网络序, 会议结束剩余时间, 单位: 分钟)
	OSPEVENT( MCU_MCS_CONFTIMELEFT_NOTIF,   EV_MCSVC_BGN + 94 );

//3.3	会议操作
//3.3.1	指定主席 
	//会议控制台指定一台终端为主席, 消息体：  TMt(做主席的终端)
    OSPEVENT( MCS_MCU_SPECCHAIRMAN_REQ,		EV_MCSVC_BGN + 95 );
	//MCU成功指定会议主席应答, 消息体：  无
	OSPEVENT( MCU_MCS_SPECCHAIRMAN_ACK,		EV_MCSVC_BGN + 96 );
	//MCU拒绝指定会议主席应答, 消息体：  无
	OSPEVENT( MCU_MCS_SPECCHAIRMAN_NACK,	EV_MCSVC_BGN + 97 );
	//指定主席通知, 消息体： TMt(做主席的终端) 
	OSPEVENT( MCU_MCS_SPECCHAIRMAN_NOTIF,   EV_MCSVC_BGN + 98 );
	//终端申请主席通知, 消息体： TMt(申请的终端) 
	OSPEVENT( MCU_MCS_MTAPPLYCHAIRMAN_NOTIF,   EV_MCSVC_BGN + 99 );

//3.3.2	取消主席 
	//会议控制台取消当前会议主席，消息体：无
    OSPEVENT( MCS_MCU_CANCELCHAIRMAN_REQ,   EV_MCSVC_BGN + 101 );
	//MCU成功取消会议主席应答,   消息体：  无
    OSPEVENT( MCU_MCS_CANCELCHAIRMAN_ACK,   EV_MCSVC_BGN + 102 );
	//会议中无主席
	OSPEVENT( MCU_MCS_CANCELCHAIRMAN_NACK,  EV_MCSVC_BGN + 103 );
	//取消主席通知
	OSPEVENT( MCU_MCS_CANCELCHAIRMAN_NOTIF, EV_MCSVC_BGN + 104 );

	//会议控制台设置会议的主席方式 
	//消息体: 1byte(0-无主席，有则取消，申请与指定不会成功,1-可以有主席)
	OSPEVENT( MCS_MCU_SETCHAIRMODE_CMD,     EV_MCSVC_BGN + 105 );
    //消息体：1byte(0-无主席，1-可以有主席)
    OSPEVENT( MCU_MCS_CONFCHAIRMODE_NOTIF,  EV_MCSVC_BGN + 106 );    

//3.3.3	指定发言 
    //会议控制台指定一台终端发言, 消息体：  TMt(发言终端)
    OSPEVENT( MCS_MCU_SPECSPEAKER_REQ,      EV_MCSVC_BGN + 111 );
    //MCU成功指定发言者, 消息体：无
    //消息头可能包含错误码，错误码不为0时需要提示该错误信息
    OSPEVENT( MCU_MCS_SPECSPEAKER_ACK,      EV_MCSVC_BGN + 112 );
    //指定之前的发言者被取消, 消息体：无  消息头包含错误码
    OSPEVENT( MCU_MCS_SPECSPEAKER_NACK,     EV_MCSVC_BGN + 113 );
    //指定发言通知, 消息体：  TMt(发言终端)
    OSPEVENT( MCU_MCS_SPECSPEAKER_NOTIF,    EV_MCSVC_BGN + 114 );

   //强制广播发言人
   //消息体: 1byte(0-取消强制广播发言人,1-开始强制广播发言人)
   OSPEVENT( MCS_MCU_MTSEESPEAKER_CMD,      EV_MCSVC_BGN + 115 );

   //MCU通知会控终端申请发言, 消息体：  TMt(申请终端)
   OSPEVENT( MCU_MCS_MTAPPLYSPEAKER_NOTIF,  EV_MCSVC_BGN + 116 );
   //[5/4/2011 zhushengze]VCS控制发言人发双流
   OSPEVENT(MCS_MCU_CHANGEMTSECVIDSEND_CMD, EV_MCSVC_BGN + 117);
   
   //[2/23/2012 zhushengze]界面、终端消息透传
   //消息体:u16(目的终端数)+多个TMt+界面、终端协商消息体
   OSPEVENT(MCS_MCU_TRANSPARENTMSG_NOTIFY,  EV_MCSVC_BGN + 118);
   //消息体:TMt + 终端与界面协商消息类型和消息体
   OSPEVENT(MCU_MCS_TRANSPARENTMSG_NOTIFY,  EV_MCSVC_BGN + 119 );
	

//3.3.4	取消发言
	//会议控制台取消会议Speaker, 消息体：  无
	OSPEVENT( MCS_MCU_CANCELSPEAKER_REQ,    EV_MCSVC_BGN + 121 );
   //MCU成功取消会议主席应答 ,消息体：  无
    OSPEVENT( MCU_MCS_CANCELSPEAKER_ACK,    EV_MCSVC_BGN + 122 );
   //取消发言人失败，消息体：  无
	OSPEVENT( MCU_MCS_CANCELSPEAKER_NACK,   EV_MCSVC_BGN + 123 );
	//取消发言人通知, 消息体：  无
	OSPEVENT( MCU_MCS_CANCELSPEAKER_NOTIF,  EV_MCSVC_BGN + 124 );

//3.3.5 指定回传
    //会议控制台指定本会场向上级回传的终端, 消息体：  TMt
    OSPEVENT( MCS_MCU_SPECOUTVIEW_REQ,      EV_MCSVC_BGN + 125 );
    //MCU成功指定本会场向上级回传的终端 ,消息体：  TMt
    OSPEVENT( MCU_MCS_SPECOUTVIEW_ACK,      EV_MCSVC_BGN + 126 );
    //指定本会场向上级回传的终端失败，消息体：  TMt
    OSPEVENT( MCU_MCS_SPECOUTVIEW_NACK,     EV_MCSVC_BGN + 127 );


//3.3.14 切换会议是否为抢答模式
//消息体：u8 byMode
OSPEVENT( MCS_MCU_CHGSPEAKMODE_REQ,      EV_MCSVC_BGN + 128 );
//应答：消息体 NULL
OSPEVENT( MCU_MCS_CHGSPEAKMODE_ACK,      EV_MCSVC_BGN + 129 );
//拒绝应答：消息体 NULL
OSPEVENT( MCU_MCS_CHGSPEAKMODE_NACK,     EV_MCSVC_BGN + 130 );

//3.3.15 发言/抢答列表的获取
//消息体：NULL
OSPEVENT( MCS_MCU_GETAPPLYSPEAKLIST_REQ, EV_MCSVC_BGN + 132 );
//消息体：u8 + TMt[u8]
OSPEVENT( MCU_MCS_GETAPPLYSPEAKLIST_ACK, EV_MCSVC_BGN + 133 );
//消息体：NULL
OSPEVENT( MCU_MCS_GETAPPLYSPEAKLIST_NACK, EV_MCSVC_BGN + 134 );

//MCU通知会控终端申请发言的列表，消息体：u8 + TMt[u8]
OSPEVENT( MCU_MCS_MTAPPLYSPEAKERLIST_NOTIF,  EV_MCSVC_BGN + 135 );

//3.3.6	增加终端 
	//会议控制台请求MCU增加终端, 消息体：  TMcu+TAddMtInfo 数组(要增加的终端列表)
	OSPEVENT( MCS_MCU_ADDMT_REQ,            EV_MCSVC_BGN + 141 );
	//MCU同意会议控制台增加终端, 消息体：  无
	OSPEVENT( MCU_MCS_ADDMT_ACK,            EV_MCSVC_BGN + 142 );
	//MCU拒绝会议控制台增加终端, 消息体：  无
	OSPEVENT( MCU_MCS_ADDMT_NACK,           EV_MCSVC_BGN + 143 );
	//调试版，会议控制台请求MCU按照指定的能力集增加终端
	//消息体：TMcu+TAddMtInfoEx(增加终端的数组)
	OSPEVENT( MCS_MCU_ADDMTEX_REQ,          EV_MCSVC_BGN + 144 );

	//会控请求获取所有接入功能单板(CRI,HDI,CRI2,IS2.2,MPC）MAC地址信息 消息体：无
	OSPEVENT( MCS_MCU_GETCRIMAC_REQ,            EV_MCSVC_BGN + 145 );
	//MCU同意会议控制台获取所有接入功能单板（CRI，CRI2，HDI,IS2.2，MPC）MAC地址信息, 消息体：  无
	OSPEVENT( MCU_MCS_GETCRIMAC_ACK,            EV_MCSVC_BGN + 146 );
	//MCU拒绝会议控制台获取所有接入功能单板（CRI，CRI2，HDI,IS2.2，MPC）MAC地址信息, 消息体：  无
	OSPEVENT( MCU_MCS_GETCRIMAC_NACK,           EV_MCSVC_BGN + 147 );
	//MCU通知McS所有接入功能单板的MAC地址信息 
	//消息体 u8单板总数目 + (板1：u8单板索引 +u16接入总数 + u16高清接入数+ u8[6]Mac地址 + 板2：...） 
	OSPEVENT( MCU_MCS_GETCRIMAC_NOTIF,          EV_MCSVC_BGN + 148 );


//3.3.7	删除终端
	//会议控制台驱逐终端离会，消息体：  TMt(被删除终端)
	OSPEVENT( MCS_MCU_DELMT_REQ,            EV_MCSVC_BGN + 151 );
	//终端被成功驱逐，消息体：  无 
	OSPEVENT( MCU_MCS_DELMT_ACK,            EV_MCSVC_BGN + 152 );
	//消息描述：拒绝驱逐终端，消息体：  无
	OSPEVENT( MCU_MCS_DELMT_NACK,           EV_MCSVC_BGN + 153 );

//3.3.8	会控选看
    // zw [06/27/2008] 将MCU_MCS_STARTSWITCHMC_ACK和MCU_MCS_STARTPLAY_NOTIF的消息体后面添加TAudAACCap(Audio)
	//会控选看终端, 消息体：  TSwitchInfo
	OSPEVENT( MCS_MCU_STARTSWITCHMC_REQ,    EV_MCSVC_BGN + 161 );
	//MCU应答会议控制台选看终端, 
	//消息体：  TSwitchInfo+TMediaEncrypt(video)+TDoublePayload(video)+TMediaEncrypt(Audio)+TDoublePayload(Audio)+TAudAACCap(Audio)
	OSPEVENT( MCU_MCS_STARTSWITCHMC_ACK,    EV_MCSVC_BGN + 162 );
	//MCU拒绝会议控制台选看终端, 消息体：  无
	OSPEVENT( MCU_MCS_STARTSWITCHMC_NACK,   EV_MCSVC_BGN + 163 );
	//MCU给会控的开始选看通知, 
	//消息体：  TSwitchInfo+TMediaEncrypt(video)+TDoublePayload(video)+TMediaEncrypt(Audio)+TDoublePayload(Audio)+TAudAACCap(Audio)
	//          + TTransportAddr(video rtcp addr) + TTransportAddr(audio rtcp addr) 	
	OSPEVENT( MCU_MCS_STARTPLAY_NOTIF,      EV_MCSVC_BGN + 164 );
	

//3.3.9	会控取消选看
	//会控取消选看终端, 消息体：  TSwitchInfo
	OSPEVENT( MCS_MCU_STOPSWITCHMC_REQ,     EV_MCSVC_BGN + 171 );
	//MCU应答会议控制台选看终端, 消息体：  TSwitchInfo
	OSPEVENT( MCU_MCS_STOPSWITCHMC_ACK,     EV_MCSVC_BGN + 172 );
	//MCU拒绝会议控制台选看终端, 消息体：  无
	OSPEVENT( MCU_MCS_STOPSWITCHMC_NACK,    EV_MCSVC_BGN + 173 );
	//MCU给会控的停止选看通知, 消息体：  TSwitchInfo
	OSPEVENT( MCU_MCS_STOPPLAY_NOTIF,       EV_MCSVC_BGN + 174 );

//3.3.10	发送短消息
	//会议控制台命令MCU向终端发送短消息, 
	//消息体：2 byte(终端数,网络序,值为N,0为广播到所有终端) + N个TMt +TROLLMSG
   OSPEVENT( MCS_MCU_SENDRUNMSG_CMD,        EV_MCSVC_BGN + 181 );
	//MCU给会议控制台的发送短消息通知, 消息体：  TRunMsgInfo 
   OSPEVENT( MCU_MCS_SENDRUNMSG_NOTIF,      EV_MCSVC_BGN + 182 );

//3.3.11	请求终端列表
   //会议控制台发给MCU的得到终端列表请求, 消息体：  TMcu (MCU号为0表示所有MCU)
   OSPEVENT( MCS_MCU_GETMTLIST_REQ,         EV_MCSVC_BGN + 191 );
   //MCU回应会议控制台的得到终端列表请求, 消息体：  TMcu
   OSPEVENT( MCU_MCS_GETMTLIST_ACK,         EV_MCSVC_BGN + 192 );
   //MCU拒绝给出会议列表， 消息体：  无
   OSPEVENT( MCU_MCS_GETMTLIST_NACK,        EV_MCSVC_BGN + 193 );
   //MCU发给会议控制台的终端列表通知, 消息体：  TMcu+TMtExt数组
   OSPEVENT( MCU_MCS_MTLIST_NOTIF,          EV_MCSVC_BGN + 194 );
   //MCU发给会议控制台的终端类型改变通知, 消息体：  TMt
   OSPEVENT( MCU_MCS_MTTYPECHANGE_NOTIF,    EV_MCSVC_BGN + 195 );
   //MCU发给会议控制台的终端所有信息通知, 消息体：  TConfAllMtInfo
   OSPEVENT( MCU_MCS_CONFALLMTINFO_NOTIF,   EV_MCSVC_BGN + 197 );
   // 下级会议扩展信息通知，目前里面的内容是Tmt(下级mcu信息) + u8(是否支持多回传标识，0不支持)
   OSPEVENT( MCU_MCS_CONFEXTINFO_NOTIF,     EV_MCSVC_BGN + 198 );
/* mcu给界面报的终端列表，
   // 目前的消息结构：u8 byMtNum + u16 wMcuIdx + u8 byHasHeadInfo +
   //                 u32 dwHeadLen + u8 byMemNum + byMemNum * u16 MemLen + byMtNum* TMtExt_U 
   // [pengguofeng 7/1/2013]
*/
   OSPEVENT( MCU_MCS_GETMTLIST_NOTIF,          EV_MCSVC_BGN + 199 );

//3.3.12	查询会议信息
   //会议控制台向MCU查询会议信息, 消息体：  无 
   OSPEVENT( MCS_MCU_GETCONFINFO_REQ,       EV_MCSVC_BGN + 201 );
   //MCU回应会议控制台的会议信息请求, 消息体：  TConfInfo
   OSPEVENT( MCU_MCS_GETCONFINFO_ACK,       EV_MCSVC_BGN + 202 );
   //MCU拒绝会议控制台的会议信息请求, 消息体：  无
   OSPEVENT( MCU_MCS_GETCONFINFO_NACK,      EV_MCSVC_BGN + 203 );
   
//3.3.13 查询MAU使用状态信息
   //查询请求: NULL
   OSPEVENT( MCS_MCU_GETMAUSTATUS_REQ,      EV_MCSVC_BGN + 204 );
   //应答: TMcuMauStatus
   OSPEVENT( MCU_MCS_GETMAUSTATUS_ACK,      EV_MCSVC_BGN + 205 );
   //拒绝应答: NULL
   OSPEVENT( MCU_MCS_GETMAUSTATUS_NACK,     EV_MCSVC_BGN + 206 );
   //状态主动通知: TMcuHdBasStatus
   OSPEVENT( MCU_MCS_MAUSTATUS_NOTIFY,      EV_MCSVC_BGN + 207 );
   
   // EV_MCSVC_BGN+208,209,210 已经在下面用掉了
//3.4	会议控制
//3.4.1	轮询操作
//3.4.1.1	轮询命令
   //会议控制台命令该会议开始轮询广播, 消息体：  TPollInfo + TMtPollParam数组
   OSPEVENT( MCS_MCU_STARTPOLL_CMD,         EV_MCSVC_BGN + 211 ); 
   //会议控制台命令该会议停止轮询广播, 消息体：  无
   OSPEVENT( MCS_MCU_STOPPOLL_CMD,          EV_MCSVC_BGN + 212 );
   //会议控制台命令该会议暂停轮询广播, 消息体：  无
   OSPEVENT( MCS_MCU_PAUSEPOLL_CMD,         EV_MCSVC_BGN + 213 );
   //会议控制台命令该会议继续轮询广播, 消息体：  无
   OSPEVENT( MCS_MCU_RESUMEPOLL_CMD,        EV_MCSVC_BGN + 214 );

   //会议控制台命令该会议更新轮询列表, 消息体：TPollInfo + TMtPollParam数组。只用于轮询过程中(轮询中，暂停)的列表修改，未开始轮询，请不要使用该消息
   OSPEVENT( MCS_MCU_CHANGEPOLLPARAM_CMD,   EV_MCSVC_BGN + 219 );
   //MCU所有的会控刷新轮询列表, 消息体：：TPollInfo + TMtPollParam数组
   OSPEVENT( MCU_MCS_POLLPARAMCHANGE_NTF,   EV_MCSVC_BGN + 220 );
   //会议控制台指定轮询位置, 消息体：TMt
   OSPEVENT( MCS_MCU_SPECPOLLPOS_REQ,		EV_MCSVC_BGN + 208 );
   //MCU应答会议控制台指定轮询位置, 消息体：无
   OSPEVENT( MCU_MCS_SPECPOLLPOS_ACK,		EV_MCSVC_BGN + 209 );
   //MCU拒绝会议控制台指定轮询位置, 消息体：无
   OSPEVENT( MCU_MCS_SPECPOLLPOS_NACK,		EV_MCSVC_BGN + 210 );
   


//3.4.1.2	查询轮询参数
   //会议控制台向MCU查询会议轮询参数, 消息体：  无 
   OSPEVENT( MCS_MCU_GETPOLLPARAM_REQ,      EV_MCSVC_BGN + 215 );
   //MCU应答会议控制台查询会议轮询参数, 消息体：  TPollInfo + TMtPollParam数组
   OSPEVENT( MCU_MCS_GETPOLLPARAM_ACK,      EV_MCSVC_BGN + 216 ); 
   //MCU拒绝会议控制台查询会议轮询参数, 消息体：  无
   OSPEVENT( MCU_MCS_GETPOLLPARAM_NACK,     EV_MCSVC_BGN + 217 );
   //MCU通知会议控制台当前的轮询状态, 消息体：  TPollInfo
   OSPEVENT( MCU_MCS_POLLSTATE_NOTIF,       EV_MCSVC_BGN + 218 );

   // EV_MCSVC_BGN+219 和 EV_MCSVC_BGN+220 已经在上面用掉了.

//电视墙轮询
   //会议控制台命令该会议开始电视墙轮询, 消息体：  TTvWallPollInfo + TMtPollParam数组
   OSPEVENT( MCS_MCU_STARTTWPOLL_CMD,       EV_MCSVC_BGN + 221 ); 
   //会议控制台命令该会议停止电视墙轮询, 消息体：  无
   OSPEVENT( MCS_MCU_STOPTWPOLL_CMD,        EV_MCSVC_BGN + 222 );
   //会议控制台命令该会议暂停电视墙轮询, 消息体：  无
   OSPEVENT( MCS_MCU_PAUSETWPOLL_CMD,       EV_MCSVC_BGN + 223 );
   //会议控制台命令该会议继续电视墙轮询, 消息体：  无
   OSPEVENT( MCS_MCU_RESUMETWPOLL_CMD,      EV_MCSVC_BGN + 224 );
   
   //会议控制台向MCU查询会议轮询参数, 消息体：  无 
   OSPEVENT( MCS_MCU_GETTWPOLLPARAM_REQ,    EV_MCSVC_BGN + 225 );
   //MCU应答会议控制台查询会议轮询参数, 消息体：  TTvWallPollInfo + TMtPollParam数组
   OSPEVENT( MCU_MCS_GETTWPOLLPARAM_ACK,    EV_MCSVC_BGN + 226 ); 
   //MCU拒绝会议控制台查询会议轮询参数, 消息体：  无
   OSPEVENT( MCU_MCS_GETTWPOLLPARAM_NACK,   EV_MCSVC_BGN + 227 );
   //MCU通知会议控制台当前的轮询状态, 消息体：  TTvWallPollInfo
   OSPEVENT( MCU_MCS_TWPOLLSTATE_NOTIF,     EV_MCSVC_BGN + 228 );
   
//3.4.2	语音激励控制
//3.4.2.1	启用语音激励控制发言
   //会议控制台请求MCU开始语音激励控制发言, 消息体：  无
   OSPEVENT( MCS_MCU_STARTVAC_REQ ,         EV_MCSVC_BGN + 231 );
   //MCU同意会议控制台开始语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MCS_STARTVAC_ACK,          EV_MCSVC_BGN + 232 );
   //MCU拒绝会议控制台开始语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MCS_STARTVAC_NACK,         EV_MCSVC_BGN + 233 );
   //MCU给会议控制台开始语音激励控制发言的通知, 消息体：  无
   OSPEVENT( MCU_MCS_STARTVAC_NOTIF,        EV_MCSVC_BGN + 234 );

//3.4.2.2	取消语音激励控制发言
   //会议控制台请求MCU停止语音激励控制发言, 消息体：  无
   OSPEVENT( MCS_MCU_STOPVAC_REQ ,          EV_MCSVC_BGN + 241 );
   //MCU同意会议控制台停止语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MCS_STOPVAC_ACK,           EV_MCSVC_BGN + 242 );
   //MCU拒绝会议控制台停止语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MCS_STOPVAC_NACK,          EV_MCSVC_BGN + 243 );
   //MCU给会议控制台停止语音激励控制发言的通知, 消息体：  无
   OSPEVENT( MCU_MCS_STOPVAC_NOTIF,         EV_MCSVC_BGN + 244 );

//3.4.2.3  改变语音激励切换时间
   //会议控制台请求MCU改变语音激励切换时间, 消息体： 1byte(单位:秒)
   OSPEVENT( MCS_MCU_CHANGEVACHOLDTIME_REQ,     EV_MCSVC_BGN + 245 );
   //MCU同意会议控制台改变语音激励切换时间的请求, 消息体： 1byte(单位:秒)
   OSPEVENT( MCU_MCS_CHANGEVACHOLDTIME_ACK,     EV_MCSVC_BGN + 246 );
   //MCU拒绝会议控制台改变语音激励切换时间的请求, 消息体： 1byte(单位:秒)
   OSPEVENT( MCU_MCS_CHANGEVACHOLDTIME_NACK,    EV_MCSVC_BGN + 247 );
   //MCU给会议控制台改变语音激励切换时间的通知, 消息体：  1byte(单位:秒)
   OSPEVENT( MCU_MCS_CHANGEVACHOLDTIME_NOTIF,   EV_MCSVC_BGN + 248 );

  
//3.4.3	会议讨论/混音
//3.4.3.1	开始智能混音
    //开始智能混音请求, 消息体：  NULL
	OSPEVENT( MCS_MCU_STARTDISCUSS_REQ,		EV_MCSVC_BGN + 250 );
	//MCU同意开始智能混音请求, 消息体：  
	OSPEVENT( MCU_MCS_STARTDISCUSS_ACK,		EV_MCSVC_BGN + 251 );
	//MCU拒绝开始智能混音请求, 消息体：  
	OSPEVENT( MCU_MCS_STARTDISCUSS_NACK,	EV_MCSVC_BGN + 252 );

    //zbq[11/01/2007] 混音优化
	//会议控制台请求MCU结束会议讨论, 消息体：  TMcu+1byte(同时参加讨论(混音)的成员个数)+TMt数组(指定成员混音时才有)   - 用于跨级开始讨论操作
	//OSPEVENT( MCS_MCU_STARTDISCUSS_CMD,     EV_MCSVC_BGN + 253 );

	//MCU开始会议讨论通知, 消息体：  TMcu+1byte(同时参加讨论(混音)的成员个数)
	//OSPEVENT( MCU_MCS_STARTDISCUSS_NOTIF,	EV_MCSVC_BGN + 254 );

//3.4.3.2	结束智能混音
	//会议控制台请求MCU结束智能混音请求, 消息体：无
	//tianzhiyong  2010/03/23 开启会议讨论时增加手动自动方式 增加字段： 1：开启方式 2：如果为手动，则加上选择的EQPID号
	OSPEVENT( MCS_MCU_STOPDISCUSS_REQ,      EV_MCSVC_BGN + 255 );
	//MCU同意会议控制台结束智能混音的请求, 消息体： 
	OSPEVENT( MCU_MCS_STOPDISCUSS_ACK,      EV_MCSVC_BGN + 256 );
	//MCU拒绝会议控制台结束智能混音的请求, 消息体：  
	OSPEVENT( MCU_MCS_STOPDISCUSS_NACK,     EV_MCSVC_BGN + 257 );
	
    //zbq[11/01/2007] 混音优化
    //会议控制台请求MCU结束会议讨论, 消息体：  TMcu   - 用于跨级停止讨论操作
	//OSPEVENT( MCS_MCU_STOPDISCUSS_CMD,      EV_MCSVC_BGN + 258 );

	//MCU给会议控制台结束会议讨论的通知, 消息体：  TMcu
	//OSPEVENT( MCU_MCS_STOPDISCUSS_NOTIF,	EV_MCSVC_BGN + 259 );

//3.4.3.3  定制混音成员控制(第一次Add即为开启定制混音，最后一次Remove即为停止定制混音)
    //增加混音成员, 消息体：TMt数组
    //20100708_tzy 消息体内容调整 
    //未开启混音器时 消息体：u8(开启方式) + u8(选择的EQPID) + u8(混音成员个数) + TMt数组
    //混音器已开启只是添加成员时 消息体：u8(混音成员个数) + TMt数组
    OSPEVENT( MCS_MCU_ADDMIXMEMBER_CMD,	    EV_MCSVC_BGN + 265 );	
    
    //移除混音成员, 消息体：TMt数组
    OSPEVENT( MCS_MCU_REMOVEMIXMEMBER_CMD,	EV_MCSVC_BGN + 268 );
    
    //替换混音成员，消息体：TOldMt + TNewMt
    OSPEVENT( MCS_MCU_REPLACEMIXMEMBER_CMD,	EV_MCSVC_BGN + 273 );

//3.4.3.4	查询混音参数
    
    //查询混音参数请求, 消息体： 无
    OSPEVENT( MCS_MCU_GETMIXPARAM_REQ,	        EV_MCSVC_BGN + 260 );
    //MCU同意查询混音参数, 消息体：u8 枚举 mcuNoMix 等
    OSPEVENT( MCU_MCS_GETMIXPARAM_ACK,	        EV_MCSVC_BGN + 261 );
    //MCU拒绝查询混音参数, 消息体：  
    OSPEVENT( MCU_MCS_GETMIXPARAM_NACK,	        EV_MCSVC_BGN + 262 );
    //MCU给会议控制台的混音参数通知, 每次混音参数变化时发送。消息体：  u8 枚举 mcuNoMix 等
    OSPEVENT( MCU_MCS_MIXPARAM_NOTIF,	        EV_MCSVC_BGN + 263 );

    //zbq[11/01/2007] 混音优化：本组消息的废止，可能导致会控的修改最大。所有依
    //赖于本组消息的参数都要修改，基本想法是：关于会议状态的参数，取ConfStatus，终端状态的参数，取MtStatus。
    /*
	//查询讨论参数请求, 消息体：  TMcu 
	OSPEVENT( MCS_MCU_GETDISCUSSPARAM_REQ,	EV_MCSVC_BGN + 260 );
	//MCU同意查询讨论参数, 消息体：  TMcu+TDiscussParam
	OSPEVENT( MCU_MCS_GETDISCUSSPARAM_ACK,	EV_MCSVC_BGN + 261 );
	//MCU拒绝查询讨论参数, 消息体：  TMcu
	OSPEVENT( MCU_MCS_GETDISCUSSPARAM_NACK,	EV_MCSVC_BGN + 262 );
	//MCU给会议控制台的讨论参数通知, 消息体：  TMcu+TDiscussParam
	OSPEVENT( MCU_MCS_DISCUSSPARAM_NOTIF,	EV_MCSVC_BGN + 263 );
    */

	//终端申请加入混音通知, 消息体：TMt(申请终端)
    OSPEVENT( MCU_MCS_MTAPPLYMIX_NOTIF,     EV_MCSVC_BGN + 270 );

    //混音延时设置, 消息体: u8 (单位：秒)
    OSPEVENT( MCS_MCU_CHANGEMIXDELAY_REQ,   EV_MCSVC_BGN + 276 );
    //消息体：无
    OSPEVENT( MCU_MCS_CHANGEMIXDELAY_ACK,   EV_MCSVC_BGN + 277 );
    //消息体：无
    OSPEVENT( MCU_MCS_CHANGEMIXDELAY_NACK,  EV_MCSVC_BGN + 278 );
    //消息体: u8 (单位：秒)
    OSPEVENT( MCU_MCS_CHANGEMIXDELAY_NOTIF, EV_MCSVC_BGN + 279 );

//3.4.4	视频复合控制
//3.4.4.1	开始视频复合
	//开始视频复合请求, 消息体：  TVMPParam
	OSPEVENT( MCS_MCU_STARTVMP_REQ,         EV_MCSVC_BGN + 281 );
	//MCU同意视频复合请求, 消息体： 无
	OSPEVENT( MCU_MCS_STARTVMP_ACK,         EV_MCSVC_BGN + 282 );
	//MCU不同意视频复合请求, 消息体： 无
	OSPEVENT( MCU_MCS_STARTVMP_NACK,        EV_MCSVC_BGN + 283 );
	//视频复合成功开始通知, 消息体：  TVMPParam
	OSPEVENT( MCU_MCS_STARTVMP_NOTIF,       EV_MCSVC_BGN + 284 );
	

//3.4.4.2 停止视频复合	
	//结束视频复合请求, 消息体：  
	OSPEVENT( MCS_MCU_STOPVMP_REQ,		    EV_MCSVC_BGN + 291 );
	//MCU同意视频结束复合请求, 消息体：  无
	OSPEVENT( MCU_MCS_STOPVMP_ACK,		    EV_MCSVC_BGN + 292 );
	//MCU不同意结束视频复合请求, 消息体：  无
	OSPEVENT( MCU_MCS_STOPVMP_NACK,		    EV_MCSVC_BGN + 293 );
	//视频复合成功结束通知, 消息体：  无
	OSPEVENT( MCU_MCS_STOPVMP_NOTIF,        EV_MCSVC_BGN + 294 );
	
//3.4.4.3	改变视频复合参数
	//会议控制台请求MCU改变视频复合参数, 消息体：  TVMPParam
	OSPEVENT( MCS_MCU_CHANGEVMPPARAM_REQ,	EV_MCSVC_BGN + 301 );
	//MCU同意会议控制台的改变视频复合参数请求, 消息体： TVMPParam
	OSPEVENT( MCU_MCS_CHANGEVMPPARAM_ACK,	EV_MCSVC_BGN + 302 );
	//MCU拒绝会议控制台的改变视频复合参数请求, 消息体：  无
	OSPEVENT( MCU_MCS_CHANGEVMPPARAM_NACK,	EV_MCSVC_BGN + 303 );
    //视频复合参数改变通知,消息体： TVMPParam
    OSPEVENT( MCU_MCS_CHANGEVMPPARAM_NOTIF, EV_MCSVC_BGN + 304 );

//3.4.4.4	查询视频复合参数	
	//查询视频复合音成员请求, 消息体：  无 
	OSPEVENT( MCS_MCU_GETVMPPARAM_REQ,	    EV_MCSVC_BGN + 311 );
	//MCU同意查询视频复合成员, 消息体：  TVMPParam
	OSPEVENT( MCU_MCS_GETVMPPARAM_ACK,	    EV_MCSVC_BGN + 312 );
	//MCU不同意查询视频复合成员, 消息体：  无
	OSPEVENT( MCU_MCS_GETVMPPARAM_NACK,	    EV_MCSVC_BGN + 313 );
	//MCU给会议控制台的视频复合参数通知, 消息体：  TVMPParam
	OSPEVENT( MCU_MCS_VMPPARAM_NOTIF,	    EV_MCSVC_BGN + 314 );

//3.4.4.5	复合视频流广播控制
	//会控请求MCU广播视频复合码流, 消息体无  
	OSPEVENT( MCS_MCU_STARTVMPBRDST_REQ,	EV_MCSVC_BGN + 320 );
	//MCU同意广播视频复合码流, 消息体无
	OSPEVENT( MCU_MCS_STARTVMPBRDST_ACK,	EV_MCSVC_BGN + 321 );
	//MCU拒绝广播视频复合码流, 消息体无
	OSPEVENT( MCU_MCS_STARTVMPBRDST_NACK,	EV_MCSVC_BGN + 322 );

	//会控请求MCU停止广播视频复合码流, 消息体无  
	OSPEVENT( MCS_MCU_STOPVMPBRDST_REQ,		EV_MCSVC_BGN + 325 );
	//MCU同意停止广播视频复合码流, 消息体无
	OSPEVENT( MCU_MCS_STOPVMPBRDST_ACK,		EV_MCSVC_BGN + 326 );
	//MCU拒绝停止广播视频复合码流, 消息体无
	OSPEVENT( MCU_MCS_STOPVMPBRDST_NACK,	EV_MCSVC_BGN + 327 );
//-----------------------------------------------------------------------


//3.4.4	复合电视墙
//3.4.4.1	开始复合电视墙
    //开始复合电视墙请求, 消息体：  TVMPParam
    OSPEVENT( MCS_MCU_STARTVMPTW_REQ,       EV_MCSVC_BGN + 330 );
    //MCU同意复合电视墙请求, 消息体： 无
    OSPEVENT( MCU_MCS_STARTVMPTW_ACK,       EV_MCSVC_BGN + 331 );
    //MCU不同意复合电视墙请求, 消息体： 无
    OSPEVENT( MCU_MCS_STARTVMPTW_NACK,      EV_MCSVC_BGN + 332 );
    //复合电视墙成功开始通知, 消息体：  TVMPParam
    OSPEVENT( MCU_MCS_STARTVMPTW_NOTIF,     EV_MCSVC_BGN + 333 );

//3.4.4.2 停止复合电视墙	
    //结束复合电视墙请求, 消息体：  
    OSPEVENT( MCS_MCU_STOPVMPTW_REQ,        EV_MCSVC_BGN + 335 );
    //MCU同意结束复合电视墙请求, 消息体：  无
    OSPEVENT( MCU_MCS_STOPVMPTW_ACK,        EV_MCSVC_BGN + 336 );
    //MCU不同意结束复合电视墙请求, 消息体：  无
    OSPEVENT( MCU_MCS_STOPVMPTW_NACK,       EV_MCSVC_BGN + 337 );
    //复合电视墙成功结束通知, 消息体：  无
    OSPEVENT( MCU_MCS_STOPVMPTW_NOTIF,      EV_MCSVC_BGN + 338 );

//3.4.4.3	改变复合电视墙参数
    //会议控制台请求MCU改变复合电视墙参数, 消息体：  TVMPParam
    OSPEVENT( MCS_MCU_CHANGEVMPTWPARAM_REQ, EV_MCSVC_BGN + 340 );
    //MCU同意会议控制台的改变复合电视墙参数请求, 消息体： TVMPParam
    OSPEVENT( MCU_MCS_CHANGEVMPTWPARAM_ACK, EV_MCSVC_BGN + 341 );
    //MCU拒绝会议控制台的改变复合电视墙参数请求, 消息体：  无
    OSPEVENT( MCU_MCS_CHANGEVMPTWPARAM_NACK,EV_MCSVC_BGN + 342 );

    //MCU给会议控制台的视频复合参数通知, 消息体：  TVMPParam
	OSPEVENT( MCU_MCS_VMPTWPARAM_NOTIF,     EV_MCSVC_BGN + 343 );
//-----------------------------------------------------------------------


//3.4.5	查询会议状态
	//会议控制台向MCU查询会议状态, 消息体：  无
	OSPEVENT( MCS_MCU_GETCONFSTATUS_REQ,	EV_MCSVC_BGN + 351 );
	//MCU给出查询会议状态应答, 消息体： TConfStatus 
	OSPEVENT( MCU_MCS_GETCONFSTATUS_ACK,	EV_MCSVC_BGN + 352 );
	//MCU拒绝给出查询会议状态应答, 消息体： 无
	OSPEVENT( MCU_MCS_GETCONFSTATUS_NACK,	EV_MCSVC_BGN + 353 );
	//MCU给会议控制台的会议状态通知, 消息体： TConfStatus
	OSPEVENT( MCU_MCS_CONFSTATUS_NOTIF,	    EV_MCSVC_BGN + 354 );

	
	//会议控制台发给MCU的刷新其他MCU的命令, 消息体：  TMcu
	OSPEVENT( MCS_MCU_REFRESHMCU_CMD,       EV_MCSVC_BGN + 355 );
	
	//会议中级联的Mcu媒体源请求, 消息体：  TMcu
	OSPEVENT( MCS_MCU_MCUMEDIASRC_REQ,      EV_MCSVC_BGN + 356 );
	//会议中级联的Mcu媒体源成功应答, 消息体：  TMcu
	OSPEVENT( MCU_MCS_MCUMEDIASRC_ACK,      EV_MCSVC_BGN + 357 );
	//会议中级联的Mcu媒体源失败应答, 消息体：  TMcu
	OSPEVENT( MCU_MCS_MCUMEDIASRC_NACK,     EV_MCSVC_BGN + 358 );

	//会议中级联的Mcu媒体源通知,消息体：  TMcu+TMt
	OSPEVENT( MCU_MCS_MCUMEDIASRC_NOTIF,    EV_MCSVC_BGN + 359 );

    //会议模式通知,消息体：TConfMode
    OSPEVENT( MCU_MCS_CONFMODE_NOTIF,       EV_MCSVC_BGN + 360 );

//3.4.6 点名操作
    //开始点名请求, 消息体：点名模式 u8 (ROLLCALL_MODE_NONE 等)
    OSPEVENT( MCS_MCU_STARTROLLCALL_REQ,	EV_MCSVC_BGN + 285 );
    //MCU同意开始点名请求 消息体：  点名模式 u8
    OSPEVENT( MCU_MCS_STARTROLLCALL_ACK,	EV_MCSVC_BGN + 286 );
    //MCU拒绝开始点名请求 消息体：  
    OSPEVENT( MCU_MCS_STARTROLLCALL_NACK,	EV_MCSVC_BGN + 287 );
    //MCU给会议控制台的开始点名通知, 消息体：  点名模式 u8
    OSPEVENT( MCU_MCS_STARTROLLCALL_NOTIF,	EV_MCSVC_BGN + 288 );
    
    //停止点名请求, 消息体：
    OSPEVENT( MCS_MCU_STOPROLLCALL_REQ,		EV_MCSVC_BGN + 295 );
    //MCU同意停止点名请求 消息体：  
    OSPEVENT( MCU_MCS_STOPROLLCALL_ACK,		EV_MCSVC_BGN + 296 );
    //MCU拒绝停止点名请求 消息体：  
    OSPEVENT( MCU_MCS_STOPROLLCALL_NACK,	EV_MCSVC_BGN + 297 );
    //MCU给会议控制台的停止点名通知, 消息体：  
    OSPEVENT( MCU_MCS_STOPROLLCALL_NOTIF,	EV_MCSVC_BGN + 298 );
    
    //点名变更请求, 消息体：TMt＋TMt
    OSPEVENT( MCS_MCU_CHANGEROLLCALL_REQ,	EV_MCSVC_BGN + 305 );
    //MCU同意点名变更请求 消息体：  TMt＋TMt
    OSPEVENT( MCU_MCS_CHANGEROLLCALL_ACK,	EV_MCSVC_BGN + 306 );
    //MCU拒绝点名变更请求 消息体：  
    OSPEVENT( MCU_MCS_CHANGEROLLCALL_NACK,	EV_MCSVC_BGN + 307 );
    //MCU给会议控制台的点名变更通知, 消息体：  TMt＋TMt
    OSPEVENT( MCU_MCS_CHANGEROLLCALL_NOTIF, EV_MCSVC_BGN + 308 );
    


//3.5	终端操作
//3.5.1	呼叫终端
	//会议控制台请求MCU呼叫终端, 消息体：  TMt(被呼叫终端)
	OSPEVENT( MCS_MCU_CALLMT_REQ,			EV_MCSVC_BGN + 365 );
	//MCU同意呼叫终端应答, 消息体：  TMt(入会终端)
	OSPEVENT( MCU_MCS_CALLMT_ACK,			EV_MCSVC_BGN + 366 );
	//MCU拒绝呼叫终端, 消息体：  无
	OSPEVENT( MCU_MCS_CALLMT_NACK,			EV_MCSVC_BGN + 367 );
    //终端在线状态变化通知, 消息体：  TMt+1byte(状态:0-离线 1-上线)+1byte(离线原因: 0(MTLEFT_REASON_EXCEPT)-异常 1-正常挂断 2-线路断 3-类型不匹配)
    OSPEVENT( MCU_MCS_MTONLINECHANGE_NOTIF,	EV_MCSVC_BGN + 368 );
    //终端呼叫不成功通知, 消息体：  TMt+ (若原因为ERR_MCU_VCS_MTBUSY, 消息体+ s8[MAXLEN_CONFNAME](当前所在会议的名称))  原因见错误码
    OSPEVENT( MCU_MCS_CALLMTFAILURE_NOTIF,	EV_MCSVC_BGN + 369 );


//3.5.2	设置呼叫终端方式
	//会议控制台设置MCU呼叫终端方式, 消息体：  TMt(被呼叫终端)+1byte(呼叫方式)
	OSPEVENT( MCS_MCU_SETCALLMTMODE_REQ,	EV_MCSVC_BGN + 371 );
	//成功设置MCU呼叫终端方式  消息体：  TMt(被呼叫终端)+1byte(呼叫方式)
	OSPEVENT( MCU_MCS_SETCALLMTMODE_ACK,	EV_MCSVC_BGN + 372 );
	//拒绝设置MCU呼叫终端方式, 消息体：  无
	OSPEVENT( MCU_MCS_SETCALLMTMODE_NACK,	EV_MCSVC_BGN + 373 );
	//MCU呼叫终端方式通知  消息体：  TMtExt
	OSPEVENT( MCU_MCS_CALLMTMODE_NOTIF,		EV_MCSVC_BGN + 374 );
	

//3.5.3	挂断终端
	//会议控制台挂断终端请求, 消息体：  TMt (被挂断终端)
	OSPEVENT( MCS_MCU_DROPMT_REQ,			EV_MCSVC_BGN + 381 );
	//终端被成功挂断   消息体：  无
	OSPEVENT( MCU_MCS_DROPMT_ACK,			EV_MCSVC_BGN + 382 );
	//MCU拒绝挂断该终端, 消息体：  无
	OSPEVENT( MCU_MCS_DROPMT_NACK,			EV_MCSVC_BGN + 383 );


//3.5.4	强制终端选看
	//会控强制目的终端选看源终端, 消息体：  TSwitchInfo
	OSPEVENT( MCS_MCU_STARTSWITCHMT_REQ,	EV_MCSVC_BGN + 391 );
	//MCU应答会控强制目的终端选看源终端, 消息体：  TSwitchInfo
	OSPEVENT( MCU_MCS_STARTSWITCHMT_ACK,	EV_MCSVC_BGN + 392 );
	//拒绝会控强制目的终端选看源终端, 消息体：  空
	OSPEVENT( MCU_MCS_STARTSWITCHMT_NACK,	EV_MCSVC_BGN + 393 );



//3.5.5	取消终端选看  
	//会控取消目的终端选看源终端, 消息体：  TSwitchInfo
	OSPEVENT( MCS_MCU_STOPSWITCHMT_REQ,		EV_MCSVC_BGN + 401 );
	//MCU应答会控取消目的终端选看源终端, 消息体：  TSwitchInfo
	OSPEVENT( MCU_MCS_STOPSWITCHMT_ACK,		EV_MCSVC_BGN + 402 );
	//MCU拒绝会控取消目的终端选看源终端, 消息体：  空
	OSPEVENT( MCU_MCS_STOPSWITCHMT_NACK,	EV_MCSVC_BGN + 403 );


//3.5.6	查询终端状态
	//会控向MCU查询终端状态, 消息体：  TMt(要查询的终端)
	OSPEVENT( MCS_MCU_GETMTSTATUS_REQ,		EV_MCSVC_BGN + 411 );
	//MCU应答会议控制台查询终端状态, 消息体：  TMtStatus
	OSPEVENT( MCU_MCS_GETMTSTATUS_ACK,		EV_MCSVC_BGN + 412 );
	//MCU拒绝会议控制台的查询终端状态, 消息体：  空
	OSPEVENT( MCU_MCS_GETMTSTATUS_NACK,		EV_MCSVC_BGN + 413 );
	//MCU给会议控制台的查询终端状态通知, 消息体：  TMtStatus数组 
	OSPEVENT( MCU_MCS_MTSTATUS_NOTIF,		EV_MCSVC_BGN + 414 );
	
	//会控向MCU查询所有终端状态, 消息体：  TMCU
	OSPEVENT( MCS_MCU_GETALLMTSTATUS_REQ,	EV_MCSVC_BGN + 395 );
	//MCU应答会议控制台查询所有终端状态, 消息体：  TMCU + TMtStatus数组 
	OSPEVENT( MCU_MCS_GETALLMTSTATUS_ACK,	EV_MCSVC_BGN + 396 );
	//MCU拒绝会议控制台的查询所有终端状态, 消息体：  TMCU
	OSPEVENT( MCU_MCS_GETALLMTSTATUS_NACK,	EV_MCSVC_BGN + 397 );
	//MCU给会议控制台的所有终端状态通知, 消息体：  TMCU + TMtStatus数组 
	OSPEVENT( MCU_MCS_ALLMTSTATUS_NOTIF,	EV_MCSVC_BGN + 398 );

//3.5.7 查询终端别名
	//会控向MCU查询终端别名, 消息体：  TMt(要查询的终端)
	OSPEVENT( MCS_MCU_GETMTALIAS_REQ,		EV_MCSVC_BGN + 415 );
	//MCU应答会议控制台查询终端别名, 消息体：  TMt+TMtAlias数组
	OSPEVENT( MCU_MCS_GETMTALIAS_ACK,		EV_MCSVC_BGN + 416 );
	//MCU没有终端的别名信息, 消息体：  TMt(要查询的终端)
	OSPEVENT( MCU_MCS_GETMTALIAS_NACK,		EV_MCSVC_BGN + 417 );
	//MCU给会议控制台的终端别名通知, 消息体：  TMt+TMtAlias数组
	OSPEVENT( MCU_MCS_MTALIAS_NOTIF,		EV_MCSVC_BGN + 418 );

	//会控向MCU查询所有终端别名, 消息体：  TMCU
	OSPEVENT( MCS_MCU_GETALLMTALIAS_REQ,	EV_MCSVC_BGN + 405 );
	//MCU应答会议控制台查询所有终端别名, 消息体：  TMCU
	OSPEVENT( MCU_MCS_GETALLMTALIAS_ACK,	EV_MCSVC_BGN + 406 );
	//MCU拒绝会议控制台的查询所有终端别名, 消息体：  TMCU
	OSPEVENT( MCU_MCS_GETALLMTALIAS_NACK,	EV_MCSVC_BGN + 407 );
	//MCU给会议控制台的查询所有终端别名通知, 
    //消息体： 某Mcu的McuId(u8) + 终端个数n(u8) + n*每个终端别名打包缓存(MtId(u8)+ 别名个数m(u8) + m*(别名类型(u8)+别名长度(u8)+别名(不带\0)) )
	OSPEVENT( MCU_MCS_ALLMTALIAS_NOTIF,		EV_MCSVC_BGN + 408 );

//3.5.8	调节终端码率
	//会议控制台命令MCU调节终端码率, 消息体：  TMt + u16 (会议码率(单位:Kbps,1K=1024)网络序)
	OSPEVENT( MCS_MCU_SETMTBITRATE_CMD,     EV_MCSVC_BGN + 421 );
    
    //会议控制台查询终端码率，消息体：TMt (若TMt为空默认为请求所有终端码率)
    OSPEVENT( MCS_MCU_GETMTBITRATE_REQ,     EV_MCSVC_BGN + 423 );
    //消息体：无
    OSPEVENT( MCU_MCS_GETMTBITRATE_ACK,     EV_MCSVC_BGN + 424 );
    //消息体：无
    OSPEVENT( MCU_MCS_GETMTBITRATE_NACK,    EV_MCSVC_BGN + 425 ); 
    //单个终端码率通知, 消息体：TMtBitrate
    OSPEVENT( MCU_MCS_MTBITRATE_NOTIF,      EV_MCSVC_BGN + 426 );

//3.5.9	设置终端视频源
	//会控要求MCU设置终端视频源, 消息体：  TMt(要设置的终端)+1byte(视频源索引0:S端子 1-16:C端子)
	OSPEVENT( MCS_MCU_SETMTVIDSRC_CMD,		EV_MCSVC_BGN + 431 );


//3.5.10 终端信息获取
    //会控请求终端信息，消息体： TMt(被请求的终端,  tMt.IsNull()表示请求所有)
    OSPEVENT( MCS_MCU_GETMTEXTINFO_REQ,     EV_MCSVC_BGN + 440 );
    //消息体：TMtExt2
    OSPEVENT( MCU_MCS_GETMTEXTINFO_ACK,     EV_MCSVC_BGN + 441 );
    //消息体：TMtExt2
    OSPEVENT( MCU_MCS_GETMTEXTINFO_NACK,    EV_MCSVC_BGN + 442 );


//3.5.11 终端视频源别名
    //会控请求获取终端视频源别名信息，消息体：u8 byMtNum（请求的终端数量 0：请求所以终端），TMt tMt（终端信息）
	OSPEVENT( MCS_MCU_GETMTVIDEOALIAS_REQ,  EV_MCSVC_BGN + 443 );
	//接受回应，消息体：请求消息体原样返回
	OSPEVENT( MCU_MCS_GETMTVIDEOALIAS_ACK,  EV_MCSVC_BGN + 444 );
	//拒接，消息体：请求消息体原样返回
	OSPEVENT( MCU_MCS_GETMTVIDEOALIAS_NACK, EV_MCSVC_BGN + 445 );
	//MCU通知mcs终端视频源别名信息，消息体：u8 byMtNum（终端个数）+ TMt tMt（终端信息）+ u8 byVidNum（视频源个数）+
    // u8 byAliasLen + s8 *pchAlias + u8 byAliasLen + s8 *pchAlias ...+ TMt tMt（终端信息) + u8 byVidNum（视频源个数）...
	OSPEVENT( MCU_MCS_MTVIDEOALIAS_NOTIF,   EV_MCSVC_BGN + 446 );

//3.5.12	设置终端静音
	//会控要求MCU设置终端静音, 消息体：  TMt +1byte(1- MUTE 0-NOMUTE)+1byte(1-DECODER 2-CAPTURE)
	OSPEVENT( MCS_MCU_MTAUDMUTE_REQ,		EV_MCSVC_BGN + 451 );
	//MCU应答会议控制台设置终端静音, 消息体：TMt +1byte(1- MUTE 0-NOMUTE)+1byte(1-DECODER 2-CAPTURE) 
	OSPEVENT( MCU_MCS_MTAUDMUTE_ACK,		EV_MCSVC_BGN + 452 );
	//MCU拒绝会议控制台设置终端静音, 消息体：  TMt
	OSPEVENT( MCU_MCS_MTAUDMUTE_NACK,		EV_MCSVC_BGN + 453 );
	//MCU通知会议控制台设置终端静音, 消息体：  TMt +1byte(1- MUTE 0-NOMUTE)+1byte(1-DECODER 2-CAPTURE)
	OSPEVENT( MCU_MCS_MTAUDMUTE_NOTIF,		EV_MCSVC_BGN + 454 );
	

//3.5.13	终端摄像头调节
	//会议控制台控制终端摄像机镜头运动命令, 消息体：  TMt +1byte(参见摄像头控制参数定义)
	OSPEVENT( MCS_MCU_MTCAMERA_CTRL_CMD,	    EV_MCSVC_BGN + 461 );
	//会议控制台控制终端摄像机镜头停止运动命令, 消息体：  TMt +1byte(参见摄像头控制参数定义)
	OSPEVENT( MCS_MCU_MTCAMERA_CTRL_STOP,	    EV_MCSVC_BGN + 462 );
	//会议控制台命令终端摄像机遥控使能，消息体：  TMt +1byte(1-ENABLE 0-DISABLE) 
	OSPEVENT( MCS_MCU_MTCAMERA_RCENABLE_CMD,	EV_MCSVC_BGN + 463 );
	//会议控制台命令终端摄像机将当前位置信息存入指定位置，消息体：  TMt +1byte(1～4：预设位置号) 
	OSPEVENT( MCS_MCU_MTCAMERA_SAVETOPOS_CMD,	EV_MCSVC_BGN + 464 );
	//会议控制台命令终端摄像机调整到指定位置，消息体：  TMt +1byte(1～4：预设位置号) 
	OSPEVENT( MCS_MCU_MTCAMERA_MOVETOPOS_CMD,	EV_MCSVC_BGN + 465 );

	
//3.5.14有关视频内置矩阵操作    
    
    //所有矩阵方案请求  MC->ter, 消息体：TMt
    OSPEVENT( MCS_MCU_MATRIX_GETALLSCHEMES_CMD,     EV_MCSVC_BGN + 470 );
    //所有矩阵方案信息通知，消息体为：TMt +u8( 方案数 ) + ITInnerMatrixScheme[]
    OSPEVENT( MCU_MCS_MATRIX_ALLSCHEMES_NOTIF,      EV_MCSVC_BGN + 473 );
    
    //单个矩阵方案请求  MC->ter, 消息体：TMt +u8 ( 0 为默认方案 )       方案索引从( 0 - 6 )
    OSPEVENT( MCS_MCU_MATRIX_GETONESCHEME_CMD,      EV_MCSVC_BGN + 474 );
    //消息体：TMt +ITInnerMatrixScheme
    OSPEVENT( MCU_MCS_MATRIX_ONESCHEME_NOTIF,       EV_MCSVC_BGN + 477 );
    
    //保存矩阵方案命令,消息体:TMt +ITInnerMatrixScheme
    OSPEVENT( MCS_MCU_MATRIX_SAVESCHEME_CMD,		EV_MCSVC_BGN + 478 );
    //消息体：TMt +ITInnerMatrixScheme
    OSPEVENT( MCU_MCS_MATRIX_SAVESCHEME_NOTIF,      EV_MCSVC_BGN + 481 );
    
    //指定当前方案,消息体：TMt +u8  方案索引
    OSPEVENT( MCS_MCU_MATRIX_SETCURSCHEME_CMD,      EV_MCSVC_BGN + 482 );
    //消息体：TMt +u8  方案索引
    OSPEVENT( MCU_MCS_MATRIX_SETCURSCHEME_NOTIF,    EV_MCSVC_BGN + 485 );
    
    //获取当前方案,消息体：TMt 
    OSPEVENT( MCS_MCU_MATRIX_GETCURSCHEME_CMD,      EV_MCSVC_BGN + 486 );
    //消息体：TMt +u8  方案索引
    OSPEVENT( MCU_MCS_MATRIX_CURSCHEME_NOTIF,       EV_MCSVC_BGN + 489 );
    
// 3.5.15 终端音量调节

    //会议控制台控制终端音量，消息体：TMt + u8(音量类型 VOLUME_TYPE_IN ...) + u8(调整后的音量)
    //PCMT的音量范围是0-255；普通Keda终端的音量范围是0-32
    OSPEVENT( MCS_MCU_SETMTVOLUME_CMD,              EV_MCSVC_BGN + 490 );
    

//DSC版本更新
	//mcs通知8000B/C更新DSC版本, zgc, 20070820
	//消息体：u8(单板索引) + u8(文件名个数) + u8(文件一名长度) + s8[](文件一名) + u8(文件二名长度) + s8[](文件二名) + …… 注：(文件名包含绝对路径)
	OSPEVENT( MCS_MCU_STARTUPDATEDSCVERSION_REQ,	EV_MCSVC_BGN + 492 );
	//MCU同意进行升级
	//消息体：u8(单板索引) + u8(文件名个数) + u8(文件一名长度) + s8[](文件一名) + u8(文件二名长度) + s8[](文件二名) + …… 注：(文件名包含绝对路径)
	OSPEVENT( MCU_MCS_STARTUPDATEDSCVERSION_ACK,	EV_MCSVC_BGN + 493 );
	//MCU不同意进行升级
	//消息体：u8(单板索引) + u8(文件名个数) + u8(文件一名长度) + s8[](文件一名) + u8(文件二名长度) + s8[](文件二名) + …… 注：(文件名包含绝对路径)
	OSPEVENT( MCU_MCS_STARTUPDATEDSCVERSION_NACK,	EV_MCSVC_BGN + 494 );
	//MCS向MCU发送升级文件数据包请求
	//消息体：u8(单板索引) + TDscUpdateReqHead + [filecontent]……
	OSPEVENT( MCS_MCU_DSCUPDATEFILE_REQ,			EV_MCSVC_BGN + 495 );
	//MCU向MCS发出的接收成功回应
	//消息体：u8(单板索引) + TDscUpdateRsp（填写下一帧序号）
	OSPEVENT( MCU_MCS_DSCUPDATEFILE_ACK,		EV_MCSVC_BGN + 496 );
	//MCU向MCS发出的接收失败回应
	//消息体：u8(单板索引) + TDscUpdateRsp（填写该帧序号，等同于通知会控重发）
	OSPEVENT( MCU_MCS_DSCUPDATEFILE_NACK,		EV_MCSVC_BGN + 497 );

//3.6	外设操作
//3.6.1	录放像机操作
//3.6.1.1	录像操作
//3.6.1.1.1	请求录像
    //会控向MCU请求录像
    //消息体区分录像机与vrs新录播：
	// 录像机外设：TMt+TEqp+TRecStartPara+录像名(以0结尾，固定大小128字节)+TSimCapSet+TVideoStreamCap+TAudioTypeDesc
	// vrs新录播：TMt+TEqp+TRecStartPara+录像名(以0结尾，固定大小129字节)+TSimCapSet+TVideoStreamCap+TAudioTypeDesc+tMtalias
    //	TMt(标识是否会议录像，会议录像mtid为0，否则为终端录像)
    //	TEqp(标识录像设备,如果为空表示新录播设备，最后带上新录播设备别名，否则为老录播设备)
	//	TRecStartPara(录像基本参数)
	//	录像名(以0结尾，固定大小128字节)
	//	TSimCapSet主流适配录像参数
	//	TVideoStreamCap(双流适配录像参数)
	//	TAudioTypeDesc(音频适配录像参数)
    //	tMtalias(新录播alias)
    OSPEVENT( MCS_MCU_STARTREC_REQ,		    EV_MCSVC_BGN + 501 );
    //MCU同意会控放像请求,消息体TMt + TEqp
    OSPEVENT( MCU_MCS_STARTREC_ACK,		    EV_MCSVC_BGN + 502 );
    //MCU拒绝会控放像请求,消息体TMt + TEqp
    OSPEVENT( MCU_MCS_STARTREC_NACK,	    EV_MCSVC_BGN + 503 );
    
    //MCS查询录相进度，消息体：TMt（若为空则为会议）
    OSPEVENT( MCS_MCU_GETRECPROG_CMD,       EV_MCSVC_BGN + 504 );
    //MCU通知会控录像进度, 消息体：  TMt + TEqp+TRecProg
    OSPEVENT( MCU_MCS_RECPROG_NOTIF,        EV_MCSVC_BGN + 505 );

//3.6.1.1.2	暂停录像请求
	//会控向MCU暂停录像 TMt
	OSPEVENT( MCS_MCU_PAUSEREC_REQ,		    EV_MCSVC_BGN + 506 );
	//MCU同意会控暂停放像请求,消息体TMt
	OSPEVENT( MCU_MCS_PAUSEREC_ACK,		    EV_MCSVC_BGN + 507 );
	//MCU拒绝会控暂停放像请求,消息体TMt
	OSPEVENT( MCU_MCS_PAUSEREC_NACK,	    EV_MCSVC_BGN + 508 );

//3.6.1.1.3	恢复录像
	//会控向MCU恢复录像 TMt
	OSPEVENT( MCS_MCU_RESUMEREC_REQ,		EV_MCSVC_BGN + 511 );
	//MCU同意会控恢复放像请求,消息体TMt
	OSPEVENT( MCU_MCS_RESUMEREC_ACK,		EV_MCSVC_BGN + 512 );
	//MCU拒绝会控恢复放像请求,消息体TMt
	OSPEVENT( MCU_MCS_RESUMEREC_NACK,		EV_MCSVC_BGN + 513 );


//3.6.1.1.4	停止录像
	//会控向MCU停止录像TMt
	OSPEVENT( MCS_MCU_STOPREC_REQ,		    EV_MCSVC_BGN + 515 );
	//MCU同意会控停止放像请求,消息体TMt
	OSPEVENT( MCU_MCS_STOPREC_ACK,		    EV_MCSVC_BGN + 516 );
	//MCU拒绝会控停止放像请求,消息体TMt
	OSPEVENT( MCU_MCS_STOPREC_NACK,		    EV_MCSVC_BGN + 517 );

//3.6.1.1.5	改变录像模式
	//改变录像模式请求, 消息体TMt ＋ u8( REC_MODE_NORMAL, REC_MODE_SKIPFRAME )
	OSPEVENT( MCS_MCU_CHANGERECMODE_REQ,	EV_MCSVC_BGN + 520 );
	//同意，MCU->MCS，消息体为 TMt 
	OSPEVENT( MCU_MCS_CHANGERECMODE_ACK,	EV_MCSVC_BGN + 521 );
	//反对，MCU->MCS，消息体为 TMt 
	OSPEVENT( MCU_MCS_CHANGERECMODE_NACK,	EV_MCSVC_BGN + 522 );   

//3.6.1.2	放像操作
//3.6.1.2.1	开始播放
	//会控开始放像请求， 消息体：
	//		TEqp(标识录像设备) + u8(是否双流放像) + 录像名(0结尾字符串,固定大小129字节)
	//		+ tMtalias(新录播alias)+ 播放文件的文件ID(u32)
	OSPEVENT( MCS_MCU_STARTPLAY_REQ,        EV_MCSVC_BGN + 530 );
	//MCU开始放像应答, 消息体：  TEqp + [TPlayFileAttrib + TPlayFileMediaInfo + TTransportAddr]后面信息mcs不使用，新录播只回复TEqp
	OSPEVENT( MCU_MCS_STARTPLAY_ACK,        EV_MCSVC_BGN + 531 );
	//MCU拒绝开始放像, 消息体：  TEqp
	OSPEVENT( MCU_MCS_STARTPLAY_NACK,       EV_MCSVC_BGN + 532 );
	
    //MCS查询放相进度, 消息体:无
    OSPEVENT( MCS_MCU_GETPLAYPROG_CMD,      EV_MCSVC_BGN + 533 );
    //放像进度通知,消息体：  TEqp+TRecProg
	OSPEVENT( MCU_MCS_PLAYPROG_NOTIF,       EV_MCSVC_BGN + 534 );
	

//3.6.1.2.2	暂停播放
	//会控暂停放像请求, 消息体：  TEqp
	OSPEVENT( MCS_MCU_PAUSEPLAY_REQ,        EV_MCSVC_BGN + 535 );
	//MCU暂停放像应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_PAUSEPLAY_ACK,        EV_MCSVC_BGN + 536 );
	//MCU拒绝暂停放像, 消息体：  TEqp
	OSPEVENT( MCU_MCS_PAUSEPLAY_NACK,       EV_MCSVC_BGN + 537 );

//3.6.1.2.3	恢复放像
	//会控恢复放像请求, 消息体：  TEqp
	OSPEVENT( MCS_MCU_RESUMEPLAY_REQ,       EV_MCSVC_BGN + 540 );
	//MCU恢复放像应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_RESUMEPLAY_ACK,       EV_MCSVC_BGN + 541 );
	//MCU拒绝恢复放像, 消息体：  TEqp
	OSPEVENT( MCU_MCS_RESUMEPLAY_NACK,      EV_MCSVC_BGN + 542 );

//3.6.1.2.4	停止放像
	//会控停止放像请求, 消息体：  TEqp
	OSPEVENT( MCS_MCU_STOPPLAY_REQ,         EV_MCSVC_BGN + 545 );
	//MCU停止放像应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_STOPPLAY_ACK,         EV_MCSVC_BGN + 546 );
	//MCU停止恢复放像, 消息体：  TEqp
	OSPEVENT( MCU_MCS_STOPPLAY_NACK,        EV_MCSVC_BGN + 547 );


//3.6.1.2.5	请求快进放像
	//会控快进放像请求, 消息体：  TEqp＋快进倍数(u8)
	OSPEVENT( MCS_MCU_FFPLAY_REQ,           EV_MCSVC_BGN + 550 );
	//MCU快进放像应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_FFPLAY_ACK,           EV_MCSVC_BGN + 551 );
	//MCU快进放像, 消息体：  TEqp
	OSPEVENT( MCU_MCS_FFPLAY_NACK,          EV_MCSVC_BGN + 552 );

//3.6.1.2.6	请求快退放像
//会控快退放像请求, 消息体：  TEqp＋快进倍数(u8)
	OSPEVENT( MCS_MCU_FBPLAY_REQ,           EV_MCSVC_BGN + 555 );
	//MCU快进放像应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_FBPLAY_ACK,           EV_MCSVC_BGN + 556 );
	//MCU快进放像, 消息体：  TEqp
	OSPEVENT( MCU_MCS_FBPLAY_NACK,          EV_MCSVC_BGN + 557 );


//3.6.1.2.7	放像进度调整
    //会控调整放像进度请求, 消息体：  TEqp＋TRecProg
	OSPEVENT( MCS_MCU_SEEK_REQ,             EV_MCSVC_BGN + 560 );
	//MCU同意调整放像进度应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_SEEK_ACK,             EV_MCSVC_BGN + 561 );
	//MCU拒绝调整放像进度应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_SEEK_NACK,            EV_MCSVC_BGN + 562 );
	//消息描述：会议录放像异常通知, 消息体：  TEqp + u8(byType录像/放像) + TMt(tSrc) 
	OSPEVENT( MCU_MCS_RECORDEREXCPT_NOTIF,  EV_MCSVC_BGN + 563 );
	
	//消息描述：查询录像机状态, 消息体：u8(是否全部) + u8(byType录像/放像) + TMt(tSrc)
	OSPEVENT( MCS_MCU_GETRECSTATUS_CMD,		EV_MCSVC_BGN + 564 );
	//消息描述：新录播录像状态上报, 消息体：(tmtalias(录播服务器) + TRecChnnlStatus)*个数
	OSPEVENT( MCU_MCS_RECORDCHNSTATUS_NOTIF,  EV_MCSVC_BGN + 565 );
   
//3.6.1.3	查询及记录文件操作
//3.6.1.3.1	查询录像机状态
	//查询录像机状态, 消息体：  TEqp
	OSPEVENT( MCS_MCU_GETRECSTATUS_REQ,     EV_MCSVC_BGN + 570 );
	//MCU查询录像机状态应答, 消息体：  TPeriEqpStatus.
	OSPEVENT( MCU_MCS_GETRECSTATUS_ACK,     EV_MCSVC_BGN + 571 );
	//MCU查询录像机状态拒绝, 消息体：  TEqp
	OSPEVENT( MCU_MCS_GETRECSTATUS_NACK,    EV_MCSVC_BGN + 572 );

//3.6.1.3.2	记录列表请求
    //会控查询记录请求, 消息体：  TEqp + TMtAlias（新录播）+ 起始条目（u32）+ 获取条目数（u32） + 文件组Id（u32）
	// 文件组Id=0, 起始条目=0, 获取条目数=0: 表示获取所有分组信息
	// 文件组Id=1, 起始条目=0, 获取条目数=0: 表示系统所有文件
	// 文件组Id=x, 起始条目=0, 获取条目数=0: 表示获取该组下全部文件
	OSPEVENT( MCS_MCU_LISTALLRECORD_REQ,    EV_MCSVC_BGN + 575 );
	//查询记录应答, 消息体：  NULL
	OSPEVENT( MCU_MCS_LISTALLRECORD_ACK,    EV_MCSVC_BGN + 576 );
	//查询记录拒绝, 消息体：  NULL
	OSPEVENT( MCU_MCS_LISTALLRECORD_NACK,   EV_MCSVC_BGN + 577 );
    //录像机列表通知消息, 消息体为TEqp+TRecFileListNotify
    // 新录播文件列表上报，TEqp + TMtAlias（新录播）+ 个数N(u32)+ N*文件信息[文件类型(u8)+ID(u32)+名字(s8*129)+number(u32)]
    //                    文件类型：0=普通视频文件，1=文件组
    //                    如果文件类型=0，文件信息=文件类型+文件ID+文件名字+文件所属组ID
    //                    如果文件类型=1，文件信息=文件类型+文件组ID+组名+组内文件个数
    OSPEVENT( MCU_MCS_LISTALLRECORD_NOTIF,	EV_MCSVC_BGN + 578 );
    // 新录播节目组列表上报，TEqp + TMtAlias（新录播）+ 总的组个数n（u32）+（组名（49*s8）+ 组Id（u32）+ 本组文件个数（u32））* n 
    OSPEVENT( MCU_MCS_FILEGROUPLIST_NOTIF,	EV_MCSVC_BGN + 579 );


//3.6.1.3.3	删除记录文件请求
	//会控请求删除文件, 消息体：  TEqp+记录名。
	OSPEVENT( MCS_MCU_DELETERECORD_REQ,     EV_MCSVC_BGN + 580 );
	//会控请求删除文件应答, 消息体：  TEqp
	OSPEVENT( MCU_MCS_DELETERECORD_ACK,     EV_MCSVC_BGN + 581 );
	//会控请求删除文件拒绝, 消息体：  TEqp
	OSPEVENT( MCU_MCS_DELETERECORD_NACK,    EV_MCSVC_BGN + 582 );

//3.6.1.3.4 会控请求MCU更改记录文件(新旧名都要带路径)
	//更改记录文件请求, 消息体为TEqp + u8(旧记录名长度) + 旧记录名 + u8(新记录名长度) + 新记录名 
    //新旧名称都要带路径名，且路径名不应该修改
	OSPEVENT( MCS_MCU_RENAMERECORD_REQ,     EV_MCSVC_BGN + 583 );
	//同意，消息体为TEqp
	OSPEVENT( MCU_MCS_RENAMERECORD_ACK,     EV_MCSVC_BGN + 584 );
	//反对，消息体为TEqp
	OSPEVENT( MCU_MCS_RENAMERECORD_NACK,    EV_MCSVC_BGN + 585 );
	//会控关闭文件列表界面，消息体：TEqp + TMtAlias（新录播）
	OSPEVENT( MCS_MCU_CLOSELISTRECORD_CMD,  EV_MCSVC_BGN + 586 );

//3.6.1.4 文件发布操作
//3.6.1.4.1 会控请求MCU发布文件
	//发布录像请求 消息体为TEqp + u8(PUBLISH_LEVEL) + 录像名
	OSPEVENT( MCS_MCU_PUBLISHREC_REQ,		EV_MCSVC_BGN + 590 );
	//同意，消息体为TEqp + u8(PUBLISH_LEVEL) + 录像名
	OSPEVENT( MCU_MCS_PUBLISHREC_ACK,		EV_MCSVC_BGN + 591 );
	//反对，消息体为TEqp + u8(PUBLISH_LEVEL) + 录像名
	OSPEVENT( MCU_MCS_PUBLISHREC_NACK,		EV_MCSVC_BGN + 592 );
	
//3.6.1.4.2 会控请求MCU取消发布文件
	//发布录像请求 消息体为TEqp + u8(PUBLISH_LEVEL) + 录像名
	OSPEVENT( MCS_MCU_CANCELPUBLISHREC_REQ,	EV_MCSVC_BGN + 595 );
	//同意，消息体为TEqp + u8(PUBLISH_LEVEL) + 录像名
	OSPEVENT( MCU_MCS_CANCELPUBLISHREC_ACK,	EV_MCSVC_BGN + 596 );
	//反对，消息体为TEqp + u8(PUBLISH_LEVEL) + 录像名
	OSPEVENT( MCU_MCS_CANCELPUBLISHREC_NACK,EV_MCSVC_BGN + 597 );
	
//VMP单通道轮询
	//会议控制台命令该会议开始Vmp单通道轮询, 消息体：  TVMPPollParam
	OSPEVENT( MCS_MCU_STARTVMPPOLL_CMD,         EV_MCSVC_BGN + 600 ); 
	//会议控制台命令该会议停止Vmp单通道轮询, 消息体：  无
	OSPEVENT( MCS_MCU_STOPVMPPOLL_CMD,          EV_MCSVC_BGN + 601 );
	//会议控制台命令该会议暂停Vmp单通道轮询, 消息体：  无
	OSPEVENT( MCS_MCU_PAUSEVMPPOLL_CMD,         EV_MCSVC_BGN + 602 );
	//会议控制台命令该会议继续Vmp单通道轮询, 消息体：  无
	OSPEVENT( MCS_MCU_RESUMEVMPPOLL_CMD,        EV_MCSVC_BGN + 603 );
	
	//会议控制台向MCU查询Vmp单通道轮询参数, 消息体：  无 
	OSPEVENT( MCS_MCU_GETVMPPOLLPARAM_REQ,      EV_MCSVC_BGN + 604 );
	//MCU应答会议控制台查询Vmp单通道轮询参数, 消息体：  TVMPPollParam
	OSPEVENT( MCU_MCS_GETVMPPOLLPARAM_ACK,      EV_MCSVC_BGN + 605 ); 
	//MCU拒绝会议控制台查询Vmp单通道轮询参数, 消息体：  无
	OSPEVENT( MCU_MCS_GETVMPPOLLPARAM_NACK,     EV_MCSVC_BGN + 606 );
	//MCU通知会议控制台当前的Vmp单通道轮询状态, 消息体：  TPollInfo
	OSPEVENT( MCU_MCS_VMPPOLLSTATE_NOTIF,       EV_MCSVC_BGN + 607 );

//3.6.2	混音器操作
	//查询混音器请求, 消息体：  TEqp
	OSPEVENT( MCS_MCU_GETMIXERSTATUS_REQ,   EV_MCSVC_BGN + 641 );
	//查询混音器请求应答, 消息体：  TPeriEqpStatus
	OSPEVENT( MCU_MCS_GETMIXERSTATUS_ACK,   EV_MCSVC_BGN + 642 );
	//查询混音器请求拒绝, 消息体：  TEqp
	OSPEVENT( MCU_MCS_GETMIXERSTATUS_NACK,  EV_MCSVC_BGN + 643 );

//3.6.3	电视墙操作
	//会议控制台请求将指定Mt的图像交换到指定电视墙的指定索引的通道上,消息体 TTWSwitchInfo
	OSPEVENT( MCS_MCU_START_SWITCH_TW_REQ,  EV_MCSVC_BGN + 651 );
	//MCU同意开始交换图像, 消息体 无
	OSPEVENT( MCU_MCS_START_SWITCH_TW_ACK,  EV_MCSVC_BGN + 652 );
	//MCU拒绝开始交换图像, 消息体 无
	OSPEVENT( MCU_MCS_START_SWITCH_TW_NACK, EV_MCSVC_BGN + 653 );

	//会议控制台请求停止将指定Mt的图像交换到指定电视墙的指定索引的通道上,消息体 TTWSwitchInfo
	OSPEVENT( MCS_MCU_STOP_SWITCH_TW_REQ,   EV_MCSVC_BGN + 661 );
	//MCU同意停止交换图像, 消息体 无
	OSPEVENT( MCU_MCS_STOP_SWITCH_TW_ACK,   EV_MCSVC_BGN + 662 );
	//MCU拒绝停止交换图像, 消息体 无
	OSPEVENT( MCU_MCS_STOP_SWITCH_TW_NACK,  EV_MCSVC_BGN + 663 );

	//会议控制台请求查询MCU外设状态, 消息体: TEqp
	OSPEVENT( MCS_MCU_GETMCUPERIEQPSTATUS_REQ,  EV_MCSVC_BGN + 666 );
	//MCU应答会议控制台的查询外设状态请求, 消息体：TPeriEqpStatus  
	OSPEVENT( MCU_MCS_GETMCUPERIEQPSTATUS_ACK,  EV_MCSVC_BGN + 667 );
	//MCU拒会议控制台的查询外设状态请求, 消息体：  
	OSPEVENT( MCU_MCS_GETMCUPERIEQPSTATUS_NACK, EV_MCSVC_BGN + 668 );	

//3.6.4 数据会议服务器操作
    //查询DCS请求: TEqp
    OSPEVENT( MCS_MCU_GETPERIDCSSTATUS_REQ,     EV_MCSVC_BGN + 670 );
    //MCU应答会议控制台查询请求: TPeriDcsStatus
    OSPEVENT( MCU_MCS_GETPERIDCSSTATUS_ACK,     EV_MCSVC_BGN + 671 );
    //MCU拒绝会议控制台查询请求: TEqp
    OSPEVENT( MCU_MCS_GETPERIDCSSTATUS_NACK,    EV_MCSVC_BGN + 672 );
    //通知会控DCS状态改变：TPeriDcsStatus
    OSPEVENT( MCU_MCS_MCUPERIDCSSTATUS_NOTIF,   EV_MCSVC_BGN + 673 );

//3.6.5	外设状态通知
	//通知会控外设状态改变,消息体:TPeriEqpStatus
	OSPEVENT( MCU_MCS_MCUPERIEQPSTATUS_NOTIF,   EV_MCSVC_BGN + 678 );

	//zgc [2007/01/12] 通知会控MP转发能力超限，消息体：u32 MP 的IP地址(网络序)，同时 CServMsg头中 srcdriid 包括了MpId
	OSPEVENT( MCU_MCS_MPFLUXOVERRUN_NOTIFY,		EV_MCSVC_BGN + 679);

//3.8	MCU用户管理
//3.8.1	添加用户
	//会议控制台请求MCU添加用户, 消息体：CExUsrInfo
	OSPEVENT( MCS_MCU_ADDUSER_REQ,          EV_MCSVC_BGN + 681 );
	//MCU接受会议控制台的添加用户请求, 消息体：用户名(32byte, 以下同) 
	OSPEVENT( MCU_MCS_ADDUSER_ACK,          EV_MCSVC_BGN + 682 );
	//MCU拒绝会议控制台的添加用户请求, 消息体：用户名 
	OSPEVENT( MCU_MCS_ADDUSER_NACK,         EV_MCSVC_BGN + 683 );
    //MCU发给会议控制台的添加用户通知, 消息体：CExUsrInfo
    OSPEVENT( MCU_MCS_ADDUSER_NOTIF,        EV_MCSVC_BGN + 684 );

//3.8.2	删除用户
	//会议控制台请求MCU删除用户, 消息体：CExUsrInfo
	OSPEVENT( MCS_MCU_DELUSER_REQ,          EV_MCSVC_BGN + 685 );
	//MCU接受会议控制台的删除用户请求, 消息体：用户名
	OSPEVENT( MCU_MCS_DELUSER_ACK,          EV_MCSVC_BGN + 686 );
	//MCU拒绝会议控制台的删除用户请求, 消息体：用户名
	OSPEVENT( MCU_MCS_DELUSER_NACK,         EV_MCSVC_BGN + 687 );
    //MCU发给会议控制台的删除用户通知, 消息体：用户名
    OSPEVENT( MCU_MCS_DELUSER_NOTIF,        EV_MCSVC_BGN + 688 );

//3.8.3	获取用户列表
	//会议控制台请求MCU得到用户列表, 消息体：无
	OSPEVENT( MCS_MCU_GETUSERLIST_REQ,      EV_MCSVC_BGN + 691 );
	//MCU接受会议控制台的得到用户列表请求, 消息体无
	OSPEVENT( MCU_MCS_GETUSERLIST_ACK,      EV_MCSVC_BGN + 692 );
	//MCU拒绝会议控制台的得到用户列表请求, 消息体：无
	OSPEVENT( MCU_MCS_GETUSERLIST_NACK,     EV_MCSVC_BGN + 693 );
	//MCU发给MCS的用户列表通知，消息体为CLongMessage+CExUsrInfo数组
	OSPEVENT( MCU_MCS_USERLIST_NOTIF,       EV_MCSVC_BGN + 694 );

//3.8.4	修改用户信息
	//会议控制台请求MCU修改用户信息, 消息体：CExUsrInfo
	OSPEVENT( MCS_MCU_CHANGEUSER_REQ,       EV_MCSVC_BGN + 695 );
	//MCU接受会议控制台的修改用户信息请求, 消息体：用户名
	OSPEVENT( MCU_MCS_CHANGEUSER_ACK,       EV_MCSVC_BGN + 696 );
	//MCU拒绝会议控制台的修改用户信息请求, 消息体：用户名
	OSPEVENT( MCU_MCS_CHANGEUSER_NACK,      EV_MCSVC_BGN + 697 );
    //MCU发给会议控制台的修改用户信息通知, 消息体：CExUsrInfo
    OSPEVENT( MCU_MCS_CHANGEUSER_NOTIF,     EV_MCSVC_BGN + 698 );
        

//级联MCU控制
    //锁定下级MCU请求 消息体 TMCU+u8 (1-锁定，0-解锁) 
	OSPEVENT( MCS_MCU_LOCKSMCU_REQ,         EV_MCSVC_BGN + 710 );
	//锁定下级MCU请求成功应答 消息体 TMCU+u8 (1-锁定，0-解锁) 
	OSPEVENT( MCU_MCS_LOCKSMCU_ACK,         EV_MCSVC_BGN + 711 );
	//锁定下级MCU请求失败应答 消息体 TMCU+u8 (1-锁定，0-解锁) 
	OSPEVENT( MCU_MCS_LOCKSMCU_NACK,        EV_MCSVC_BGN + 712 );

	//请求下级MCU的锁定状态 消息体 TMCU 
	OSPEVENT( MCS_MCU_GETMCULOCKSTATUS_REQ, EV_MCSVC_BGN + 715 );
	//请求下级MCU的锁定状态成功应答 消息体 TMCU + u8 (1-锁定，0-解锁) 
	OSPEVENT( MCU_MCS_GETMCULOCKSTATUS_ACK, EV_MCSVC_BGN + 716 );
	//请求下级MCU的锁定状态失败应答 消息体 TMCU
	OSPEVENT( MCU_MCS_GETMCULOCKSTATUS_NACK,EV_MCSVC_BGN + 717 );
	//下级MCU的锁定状态通知 消息体 TMCU + u8 (1-锁定，0-解锁) 
	OSPEVENT( MCU_MCS_MCULOCKSTATUS_NOTIF,  EV_MCSVC_BGN + 718 );

//mcu配置界面化
    //4.6 新加 jlb
	//取Hdu预案
	OSPEVENT( MCS_MCU_GETHDUSCHEMEINFO_REQ , EV_MCSVC_BGN + 719);
	// 消息体：TVmpStyleCfgInfo[MAX_VMPSTYLE_NUM]
    OSPEVENT( MCU_MCS_GETHDUSCHEMEINFO_ACK,     EV_MCSVC_BGN + 720 );
    // 消息体：无，原因见错误码
    OSPEVENT( MCU_MCS_GETHDUSCHEMEINFO_NACK,    EV_MCSVC_BGN + 721 );

    //MCS请求mcu 的cpu占有率 消息体：无
    OSPEVENT( MCS_MCU_CPUPERCENTAGE_REQ,    EV_MCSVC_BGN + 722 );
    //消息体：无
    OSPEVENT( MCU_MCS_CPUPERCENTAGE_ACK,    EV_MCSVC_BGN + 723 );
    //消息体：无
    OSPEVENT( MCU_MCS_CPUPERCENTAGE_NACK,   EV_MCSVC_BGN + 724 );
    //MCS请求mcu 的cpu占有率 消息体：无
    OSPEVENT( MCS_MCU_CPUPERCENTAGE_CMD,    EV_MCSVC_BGN + 725 );
    //消息体：u8  (0-100)%
    OSPEVENT(MCU_MCS_CPUPERCENTAGE_NOTIF,   EV_MCSVC_BGN + 726 );

    //MCS请求修改mcu设备配置信息 消息体：TMcuEqpCfg
    OSPEVENT( MCS_MCU_CHANGEMCUEQPCFG_REQ,  EV_MCSVC_BGN + 728 );
    //消息体：无
    OSPEVENT( MCU_MCS_CHANGEMCUEQPCFG_ACK,  EV_MCSVC_BGN + 729 );
    //消息体：无
    OSPEVENT( MCU_MCS_CHANGEMCUEQPCFG_NACK, EV_MCSVC_BGN + 730 );
    //MCS请求mcu设备配置信息 消息体：无
    OSPEVENT( MCS_MCU_GETMCUEQPCFG_REQ,     EV_MCSVC_BGN + 731 );
    //消息体：无
    OSPEVENT( MCU_MCS_GETMCUEQPCFG_ACK,     EV_MCSVC_BGN + 732 );
    //消息体：无
    OSPEVENT( MCU_MCS_GETMCUEQPCFG_NACK,    EV_MCSVC_BGN + 733 );
    //消息体：TMcuEqpCfg
    OSPEVENT( MCU_MCS_MCUEQPCFG_NOTIF,      EV_MCSVC_BGN + 734 );
   
	//mcs修改单板配置信息 
    //消息体:u8(单板个数)+TBrdCfgInfo数组+u8(混音器个数)+TEqpMixerCfgInfo数组
	//	+u8(录像机个数)+TEqpRecCfgInfo数组+u8(电视墙个数)+TEqpTvWallCfgInfo数组
	//	+u8(bas个数)+TEqpBasCfgInfo数组+u8(vmp个数)+TEqpVmpCfgInfo数组
	//	+u8(prs个数)+TPrsCfgInfo数组+u8(复合电视墙个数)+TEqpMTvwallCfgInfo数组
	//	+u8(HDbas个数)+TEqpBasHDCfgInfo数组
	//	+svmp个数+TEqpSvmpCfgInfo数组+mpubas个数+TEqpMpuBasCfgInfo数组
	//	+Ebap个数+TEqpEbapCfgInfo数组+Evpu个数+TEqpEvpuCfgInfo数组
	//	+hdu个数+TEqpHduCfgInfo数组+T8000BDscMod
	//	+ u8(新录播配置个数)+TEqpNewRecCfgInfo数组
    OSPEVENT( MCS_MCU_CHANGEBRDCFG_REQ,     EV_MCSVC_BGN + 737 );
    //消息体 :无
    OSPEVENT( MCU_MCS_CHANGEBRDCFG_ACK,     EV_MCSVC_BGN + 738 );
    //消息体 :无 原因见错误码
    OSPEVENT( MCU_MCS_CHANGEBRDCFG_NACK,    EV_MCSVC_BGN + 739 );
	//mcs 请求单板配置信息 消息体:无
	OSPEVENT( MCS_MCU_GETBRDCFG_REQ,        EV_MCSVC_BGN + 740 );
	//消息体 :无
	OSPEVENT( MCU_MCS_GETBRDCFG_ACK,        EV_MCSVC_BGN + 741 );
	//消息体 :无 原因见错误码
	OSPEVENT( MCU_MCS_GETBRDCFG_NACK,       EV_MCSVC_BGN + 742 );	
	//单板配置信息通知 消息体: 同MCS_MCU_CHANGEBRDCFG_REQ
	OSPEVENT( MCU_MCS_BRDCFG_NOTIF,         EV_MCSVC_BGN + 743 );
	//未配置的DSC板注册通知 消息体：无
	OSPEVENT( MCU_MCS_UNCFGDSCREG_NOTIF,	EV_MCSVC_BGN + 744 );
    	
	//mcs修改mcu配置信息基本设置 消息体：TMcuGeneralCfg + u8 byInEffectNow(8000H,8000A等略有不同) 
	//+ u8是否启用密码注册（1启用，0不启用）+64bytes密码（即上述1的修改）
	//+ u8是否启用DMZ（1启用，0不启用）+u32 dmzIp（网络序） + u8 dmzethindx
	//+ u8(多运营商网口个数-1) + 多运营商信息TMultiEthManuNetAccess*个数
    OSPEVENT( MCS_MCU_CHANGEMCUGENERALCFG_REQ,  EV_MCSVC_BGN + 746 );
    //消息体：无
    OSPEVENT( MCU_MCS_CHANGEMCUGENERALCFG_ACK,  EV_MCSVC_BGN + 747 );
    //消息体：无 原因见错误码
    OSPEVENT( MCU_MCS_CHANGEMCUGENERALCFG_NACK, EV_MCSVC_BGN + 748 );

    // mcs 请求mcu基本配置信息 消息体：无
    OSPEVENT( MCS_MCU_GETMCUGENERALCFG_REQ,     EV_MCSVC_BGN + 749 );
    //消息体：无
    OSPEVENT( MCU_MCS_GETMCUGENERALCFG_ACK,     EV_MCSVC_BGN + 750 );
    //消息体：无 原因见错误码
    OSPEVENT( MCU_MCS_GETMCUGENERALCFG_NACK,    EV_MCSVC_BGN + 751 );
    //消息体：同MCS_MCU_CHANGEMCUGENERALCFG_REQ
    OSPEVENT( MCU_MCS_MCUGENERALCFG_NOTIF,      EV_MCSVC_BGN + 752 );
    // 取VmpStyle方案数组
    OSPEVENT( MCS_MCU_GETVMPSCHEMEINFO_REQ,     EV_MCSVC_BGN + 753 );
    // 消息体：TVmpStyleCfgInfo[MAX_VMPSTYLE_NUM]
    OSPEVENT( MCU_MCS_GETVMPSCHEMEINFO_ACK,     EV_MCSVC_BGN + 754 );
    // 消息体：无，原因见错误码
    OSPEVENT( MCU_MCS_GETVMPSCHEMEINFO_NACK,    EV_MCSVC_BGN + 755 );
    // 消息体：TBoardStatusNotify
    OSPEVENT(  MCU_MCS_BOARDSTATUS_NOTIFY,      EV_MCSVC_BGN +756 );

	// 有新的DSC板发起注册, 消息体: u8(DSC类型)+u32(DSC 外部ip(网络序))+u32(DSC 内部ip (网络序))+u32(DSC连接的本地MCU的内部IP(网络序)) + u32( 内部IP MASK, 网络序) , zgc, 2007-03-05
	OSPEVENT( MCU_MCS_NEWDSCREGREQ_NOTIFY,		EV_MCSVC_BGN + 757);

    //会控远程重启MCU,消息体：u32 (要重启的mpc板ip(网络序)，若为0主备mpc板均重启)
    OSPEVENT( MCS_MCU_REBOOT_CMD,           EV_MCSVC_BGN + 760 );
    //会控修改mcu系统时间, 消息体: TKdvTime
    OSPEVENT( MCS_MCU_CHANGESYSTIME_REQ,    EV_MCSVC_BGN + 765 );
    //会控修改mcu系统时间成功，消息体：TKdvTime(当前mcu系统时间)
    OSPEVENT( MCU_MCS_CHANGESYSTIME_ACK,    EV_MCSVC_BGN + 766 );
    //会控修改mcu系统时间成功，消息体：无
    OSPEVENT( MCU_MCS_CHANGESYSTIME_NACK,   EV_MCSVC_BGN + 767 );

    //会控重启某单板,消息体：u8 (单板索引)
    OSPEVENT( MCS_MCU_REBOOTBRD_REQ,        EV_MCSVC_BGN + 770 );
    //消息体：u8 (单板索引)
    OSPEVENT( MCU_MCS_REBOOTBRD_ACK,        EV_MCSVC_BGN + 771 );
    //消息体：u8 (单板索引)
    OSPEVENT( MCU_MCS_REBOOTBRD_NACK,       EV_MCSVC_BGN + 772 );

    //会控请求下载MCU配置文件，消息体：无
    OSPEVENT( MCU_MCS_DOWNLOADCFG_REQ,      EV_MCSVC_BGN + 775 );
    //消息体：u32 (内容长度,网络序) + s8[] 实际文件(长度由前面指定)
    OSPEVENT( MCU_MCS_DOWNLOADCFG_ACK,      EV_MCSVC_BGN + 776 );
    //消息体：无
    OSPEVENT( MCU_MCS_DOWNLOADCFG_NACK,     EV_MCSVC_BGN + 777 );

    //会控请求上传配置文件，消息体：u32 (内容长度,网络序) + s8[] 实际文件(长度由前面指定)
    OSPEVENT( MCU_MCS_UPLOADCFG_REQ,        EV_MCSVC_BGN + 778 );
    //上传配置文件成功。消息体：无
    OSPEVENT( MCU_MCS_UPLOADCFG_ACK,        EV_MCSVC_BGN + 779 );
    //上传配置文件失败。消息体：无
    OSPEVENT( MCU_MCS_UPLOADCFG_NACK ,      EV_MCSVC_BGN + 780 );

    //mcs通知mcu更新单板版本
    //消息体：u8(单板索引) + u8(源文件名个数) + u8(文件名长度) + s8[](文件名)  //注：(文件名包含绝对路径)
    OSPEVENT( MCS_MCU_UPDATEBRDVERSION_CMD,         EV_MCSVC_BGN + 782 );    
    //版本更新结果通知，消息体：u8(单板索引) + u8(升级文件数byNum)  + u8[byNum](各文件是否成功)
    OSPEVENT( MCU_MCS_UPDATEBRDVERSION_NOTIF,       EV_MCSVC_BGN + 785 );

	//mcs请求mcu更新telnet登陆信息, zgc, 2007-10-19
	//消息体: TLoginInfo
	OSPEVENT( MCS_MCU_UPDATETELNETLOGININFO_REQ,	EV_MCSVC_BGN + 786 );
	OSPEVENT( MCU_MCS_UPDATETELNETLOGININFO_ACK,	EV_MCSVC_BGN + 787 );
	OSPEVENT( MCU_MCS_UPDATETELNETLOGININFO_NACK,	EV_MCSVC_BGN + 788 );
	
    
//外置矩阵
    //获取终端外置矩阵类型，消息体：TMt 
    OSPEVENT( MCS_MCU_EXMATRIX_GETINFO_CMD,         EV_MCSVC_BGN + 790 );
    //终端外置矩阵类型指示, 消息体：TMt ＋u8 （矩阵输入端口数 0表示没有矩阵）
    OSPEVENT( MCU_MCS_EXMATRIXINFO_NOTIFY,          EV_MCSVC_BGN + 793 );
    
    //设置外置矩阵连接端口号,消息体：TMt ＋u8  (1-64 )
    OSPEVENT( MCS_MCU_EXMATRIX_SETPORT_CMD,         EV_MCSVC_BGN + 794 );
    
    //请求获取外置矩阵连接端口,消息体：TMt 
    OSPEVENT( MCS_MCU_EXMATRIX_GETPORT_REQ,         EV_MCSVC_BGN + 798 );
    //消息体：无
    OSPEVENT( MCU_MCS_EXMATRIX_GETPORT_ACK,         EV_MCSVC_BGN + 799 );
    //消息体：无
    OSPEVENT( MCU_MCS_EXMATRIX_GETPORT_NACK,        EV_MCSVC_BGN + 800 );
    //消息体：TMt ＋u8  (1-64 )
    OSPEVENT( MCU_MCS_EXMATRIX_GETPORT_NOTIF,       EV_MCSVC_BGN + 801 );
    
    //设置外置矩阵连接端口名,消息体：TMt ＋u8  (1-64 )+s8[MAXLEN_MATRIXPORTNAME] 0结尾的字符串 
    OSPEVENT( MCS_MCU_EXMATRIX_SETPORTNAME_CMD,     EV_MCSVC_BGN + 802 );
    //外置矩阵端口名指示, 消息体：TMt ＋u8  (1-64 )+s8[MAXLEN_MATRIXPORTNAME] 0结尾的字符串 
    OSPEVENT( MCU_MCS_EXMATRIX_PORTNAME_NOTIF,      EV_MCSVC_BGN + 805 );
    
    //请求获取外置矩阵的所有端口名,消息体：无
    OSPEVENT( MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD,  EV_MCSVC_BGN + 806 );
    //外置矩阵的所有端口名指示，消息体：TMt ＋s8[][MAXLEN_MATRIXPORTNAME] 索引0 表示输出端口名 其余为输入端口，（字符串数组）
    OSPEVENT( MCU_MCS_EXMATRIX_ALLPORTNAME_NOTIF,   EV_MCSVC_BGN + 809 );

//扩展视频源
    //获取终端视频源信息,消息体：TMt
    OSPEVENT( MCS_MCU_GETVIDEOSOURCEINFO_CMD,       EV_MCSVC_BGN + 810 );
    //终端全部视频源指示，消息体：TMt＋tagITVideoSourceInfo[]
    OSPEVENT( MCU_MCS_ALLVIDEOSOURCEINFO_NOTIF,     EV_MCSVC_BGN + 813 );

    //设置终端视频源信息，消息体：TMt＋tagITVideoSourceInfo
    OSPEVENT( MCS_MCU_SETVIDEOSOURCEINFO_CMD,       EV_MCSVC_BGN + 814 );
    //终端视频源信息指示,消息体：TMt＋tagITVideoSourceInfo
    OSPEVENT( MCU_MCS_VIDEOSOURCEINFO_NOTIF,        EV_MCSVC_BGN + 817 );
    
    //发给终端的选择扩展视频源命令, 消息体：TMt + u8
    OSPEVENT( MCS_MCU_SELECTEXVIDEOSRC_CMD,         EV_MCSVC_BGN + 820 );

// mcu地址簿管理
	//消息体：u8(TMcuAddrEntryV1成员个数) + u16[](每个结构体成员的字节数，数组长度为消息体第一个字段) + TMcuAddrEntryV1
	OSPEVENT( MCU_MCS_ADDRBOOK_GETENTRYLISTEXT_NOTIF,	EV_MCSVC_BGN + 838);
	//消息体：u8(TMcuAddrGroupV1成员个数) + u16[](每个结构体成员的字节数，数组长度为消息体第一个字段) + TMcuAddrGroupV1
	OSPEVENT( MCU_MCS_ADDRBOOK_GETGROUPLISTEXT_NOTIF,	EV_MCSVC_BGN + 839);

    //获取所有地址簿列表，消息体：无
    OSPEVENT( MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ,    EV_MCSVC_BGN + 840 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_GETENTRYLIST_ACK,    EV_MCSVC_BGN + 841 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_GETENTRYLIST_NACK,   EV_MCSVC_BGN + 842 );
    //消息体：TMcuAddrEntry数组 （若条目较多分多个消息通知，CServMsg消息头中可以获取消息总包数，当前包索引。以下会议组同）
    OSPEVENT( MCU_MCS_ADDRBOOK_GETENTRYLIST_NOTIF,  EV_MCSVC_BGN + 843 );
    
    //获取所有会议组列表，消息体：无
    OSPEVENT( MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ,    EV_MCSVC_BGN + 844 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_GETGROUPLIST_ACK,    EV_MCSVC_BGN + 845 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_GETGROUPLIST_NACK,   EV_MCSVC_BGN + 846 );
    //消息体：TMcuAddrGroup数组
    OSPEVENT( MCU_MCS_ADDRBOOK_GETGROUPLIST_NOTIF,  EV_MCSVC_BGN + 847 );     

    //增加地址簿条目，消息体：TMcuAddrEntry
    OSPEVENT( MCS_MCU_ADDRBOOK_ADDENTRY_REQ,        EV_MCSVC_BGN + 848 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDENTRY_ACK,        EV_MCSVC_BGN + 849 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDENTRY_NACK,       EV_MCSVC_BGN + 850 );
    //消息体：TMcuAddrEntry
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDENTRY_NOTIF,      EV_MCSVC_BGN + 851 );

    //删除地址簿条目，消息体：u32 （地址簿索引, 网络序）
    OSPEVENT( MCS_MCU_ADDRBOOK_DELENTRY_REQ,        EV_MCSVC_BGN + 852 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_DELENTRY_ACK,        EV_MCSVC_BGN + 853 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_DELENTRY_NACK,       EV_MCSVC_BGN + 854 );
    //消息体：u32
    OSPEVENT( MCU_MCS_ADDRBOOK_DELENTRY_NOTIF,      EV_MCSVC_BGN + 855 );

    //修改地址簿条目，消息体：TMcuAddrEntry
    OSPEVENT( MCS_MCU_ADDRBOOK_MODIFYENTRY_REQ,     EV_MCSVC_BGN + 856 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_MODIFYENTRY_ACK,     EV_MCSVC_BGN + 857 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_MODIFYENTRY_NACK,    EV_MCSVC_BGN + 858 );
    //消息体：TMcuAddrEntry
    OSPEVENT( MCU_MCS_ADDRBOOK_MODIFYENTRY_NOTIF,   EV_MCSVC_BGN + 859 );

    //增加会议组条目，消息体：TMcuAddrGroup
    OSPEVENT( MCS_MCU_ADDRBOOK_ADDGROUP_REQ,        EV_MCSVC_BGN + 860 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDGROUP_ACK,        EV_MCSVC_BGN + 861 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDGROUP_NACK,       EV_MCSVC_BGN + 862 );
    //消息体：TMcuAddrGroup
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDGROUP_NOTIF,      EV_MCSVC_BGN + 863 );

    //删除会议组条目，消息体：u32（会议组索引, 网络序）
    OSPEVENT( MCS_MCU_ADDRBOOK_DELGROUP_REQ,        EV_MCSVC_BGN + 864 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_DELGROUP_ACK,        EV_MCSVC_BGN + 865 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_DELGROUP_NACK,       EV_MCSVC_BGN + 866 );
    //消息体：u32
    OSPEVENT( MCU_MCS_ADDRBOOK_DELGROUP_NOTIF,      EV_MCSVC_BGN + 867 );

    //修改会议组条目，消息体：TMcuAddrGroup
    OSPEVENT( MCS_MCU_ADDRBOOK_MODIFYGROUP_REQ,     EV_MCSVC_BGN + 868 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_MODIFYGROUP_ACK,     EV_MCSVC_BGN + 869 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_MODIFYGROUP_NACK,    EV_MCSVC_BGN + 870 );
    //消息体：TMcuAddrGroup
    OSPEVENT( MCU_MCS_ADDRBOOK_MODIFYGROUP_NOTIF,   EV_MCSVC_BGN + 871 );

    //增加地址簿条目到会议组，消息体：TMcuAddrGroup
    OSPEVENT( MCS_MCU_ADDRBOOK_ADDENTRYTOGROUP_REQ,     EV_MCSVC_BGN + 872 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_ACK,     EV_MCSVC_BGN + 873 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NACK,    EV_MCSVC_BGN + 874 );
    //消息体：TMcuAddrGroup
    OSPEVENT( MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NOTIF,   EV_MCSVC_BGN + 875 );

    //从会议组中删除地址簿条目，消息体：TMcuAddrGroup
    OSPEVENT( MCS_MCU_ADDRBOOK_DELENTRYFROMGROUP_REQ,   EV_MCSVC_BGN + 876 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_DELENTRYFROMGROUP_ACK,   EV_MCSVC_BGN + 877 );
    //消息体：无
    OSPEVENT( MCU_MCS_ADDRBOOK_DELENTRYFROMGROUP_NACK,  EV_MCSVC_BGN + 878 );
    //消息体：TMcuAddrGroup
    OSPEVENT( MCU_MCS_ADDRBOOK_DELENTRYFROMGROUP_NOTIF, EV_MCSVC_BGN + 879 );

//3.8.5 用户组管理，顾振华@2006.06.21
	//会议控制台请求MCU添加用户组, 消息体：TUsrGrpInfo
	OSPEVENT( MCS_MCU_ADDUSERGRP_REQ,                   EV_MCSVC_BGN + 880 );
	//MCU接受会议控制台的添加用户组请求, 消息体：TUsrGrpInfo
	OSPEVENT( MCU_MCS_ADDUSERGRP_ACK,                   EV_MCSVC_BGN + 881 );
	//MCU拒绝会议控制台的添加用户组请求, 消息体：用户组名 
	OSPEVENT( MCU_MCS_ADDUSERGRP_NACK,                  EV_MCSVC_BGN + 882 );
    //MCU发给会议控制台的用户组通知, 消息体：TUsrGrpInfo
    OSPEVENT( MCU_MCS_USERGRP_NOTIF,                    EV_MCSVC_BGN + 883 );

    //会议控制台请求MCU修改用户组, 消息体：TUsrGrpInfo
	OSPEVENT( MCS_MCU_CHANGEUSERGRP_REQ,                EV_MCSVC_BGN + 884 );
	//MCU接受会议控制台的修改用户组请求, 消息体：TUsrGrpInfo，同时发送 MCU_MCS_USERGRP_NOTIF
	OSPEVENT( MCU_MCS_CHANGEUSERGRP_ACK,                EV_MCSVC_BGN + 885 );
	//MCU拒绝会议控制台的修改用户组请求, 消息体：u8 byGrpId
	OSPEVENT( MCU_MCS_CHANGEUSERGRP_NACK,               EV_MCSVC_BGN + 886 );

    //会议控制台请求MCU删除用户组, 消息体：u8 byGrpId
	OSPEVENT( MCS_MCU_DELUSERGRP_REQ,                   EV_MCSVC_BGN + 888 );
	//MCU接受会议控制台的修改用户组请求, 消息体：u8 byGrpId
	OSPEVENT( MCU_MCS_DELUSERGRP_ACK,                   EV_MCSVC_BGN + 889 );
	//MCU拒绝会议控制台的添加用户组请求, 消息体：u8 byGrpId
	OSPEVENT( MCU_MCS_DELUSERGRP_NACK,                  EV_MCSVC_BGN + 890 );
    //MCU发给会议控制台的删除用户组通知, 消息体：u8 byGrpId
    OSPEVENT( MCU_MCS_DELUSERGRP_NOTIF,                 EV_MCSVC_BGN + 891 );
    
    //会议控制台请求MCU用户组信息
	OSPEVENT( MCS_MCU_GETUSERGRP_REQ,                   EV_MCSVC_BGN + 892 );
	//MCU接受会议控制台的用户组信息请求, 消息体：u8 byNum + TUsrGrpInfo[]数组
	OSPEVENT( MCU_MCS_GETUSERGRP_ACK,                   EV_MCSVC_BGN + 893 );
	//MCU拒绝会议控制台的用户组信息请求, 消息体：无
	OSPEVENT( MCU_MCS_GETUSERGRP_NACK,                  EV_MCSVC_BGN + 894 );

//操作失败提示用户消息，消息体：CServMsg（目前只填错误码信息）
    OSPEVENT( MCU_MCS_ALARMINFO_NOTIF,                  EV_MCSVC_BGN + 900 );
    
//N+1备份消息
    //请求手动将备份服务器的数据回滚到N端，当仅当备份服务器处于接替工作状态，且未配置自动回滚时有效。仅admin有权限。消息体：无
    OSPEVENT( MCS_MCU_NPLUSROLLBACK_REQ,                EV_MCSVC_BGN + 910 );
    OSPEVENT( MCU_MCS_NPLUSROLLBACK_ACK,                EV_MCSVC_BGN + 911 ); 
    OSPEVENT( MCU_MCS_NPLUSROLLBACK_NACK,               EV_MCSVC_BGN + 912 );

    //手动将正在接替工作的备份服务器重置为空闲状态。仅admin有权限，建议用户确认。消息体：无
    OSPEVENT( MCS_MCU_NPLUSRESET_CMD,                   EV_MCSVC_BGN + 914 );

	//切换HDU某通道风格命令  消息体：u8 (要切换的新风格)
	OSPEVENT( MCS_MCU_CHGHDUVMPMODE_CMD,                EV_MCSVC_BGN + 915 );
	OSPEVENT( MCU_MCS_CHGHDUVMPMODE_NOTIF,				EV_MCSVC_BGN + 916 );

	// xliang [11/28/2008] mpu-vmp相关操作
	//VMP参数中VIP身份的MT超过VMP能力，消息体：TVmpHdChnnlMemInfo
	OSPEVENT( MCU_MCS_VMPOVERHDCHNNLLMT_NTF,				EV_MCSVC_BGN + 919) ;
	//VMP优先级抢占请求，消息体：TMt + TChnnlMemberInfo[]
	OSPEVENT( MCU_MCS_VMPPRISEIZE_REQ,						EV_MCSVC_BGN + 920 );
	//vmp优先级抢占应答，消息体：u8 byMtId+TSeizeChoice[]
	OSPEVENT( MCS_MCU_VMPPRISEIZE_ACK,						EV_MCSVC_BGN + 921 );
	//vmp优先级抢占失败应答，消息体：u8 byMtId
	OSPEVENT( MCS_MCU_VMPPRISEIZE_NACK,						EV_MCSVC_BGN + 922 );
	//开始VMP批量轮询, 消息体：
	OSPEVENT( MCS_MCU_START_VMPBATCHPOLL_REQ,				EV_MCSVC_BGN + 923 );
	OSPEVENT( MCU_MCS_START_VMPBATCHPOLL_ACK,				EV_MCSVC_BGN + 924 );
	OSPEVENT( MCU_MCS_START_VMPBATCHPOLL_NACK,				EV_MCSVC_BGN + 925 );
	//暂停VMP批量轮询, 消息体:无
	OSPEVENT( MCS_MCU_PAUSE_VMPBATCHPOLL_CMD,				EV_MCSVC_BGN + 926 );
	//恢复VMP批量轮询, 消息体:无
	OSPEVENT( MCS_MCU_RESUME_VMPBATCHPOLL_CMD,				EV_MCSVC_BGN + 927 );
	//停止VMP批量轮询, 消息体：无
	OSPEVENT( MCS_MCU_STOP_VMPBATCHPOLL_CMD,				EV_MCSVC_BGN + 928 );
	//主席MT选看VMP，消息体：TMt
	OSPEVENT( MCS_MCU_STARTSWITCHVMPMT_REQ,					EV_MCSVC_BGN + 929 );
	OSPEVENT( MCU_MCS_STARTSWITCHVMPMT_ACK,				    EV_MCSVC_BGN + 930 );
	OSPEVENT( MCU_MCS_STARTSWITCHVMPMT_NACK,                EV_MCSVC_BGN + 931 );

	
	//停止HDU图像交换，消息体：THduSwitchInfo
	OSPEVENT( MCS_MCU_STOP_SWITCH_HDU_REQ,					EV_MCSVC_BGN + 933 );
	OSPEVENT( MCU_MCS_STOP_SWITCH_HDU_ACK,					EV_MCSVC_BGN + 934 );
	OSPEVENT( MCU_MCS_STOP_SWITCH_HDU_NACK,				    EV_MCSVC_BGN + 935 );
	
	//接上面配置界面化消息
        //hdu预案通知
    OSPEVENT( MCU_MCS_HDUSCHEMEINFO_NOTIF,                   EV_MCSVC_BGN + 936 );

	//Hdu预案
    OSPEVENT( MCS_MCU_CHANGEHDUSCHEMEINFO_REQ,              EV_MCSVC_BGN + 937 );
    OSPEVENT( MCU_MCS_CHANGEHDUSCHEMEINFO_ACK,              EV_MCSVC_BGN + 938 );
    OSPEVENT( MCU_MCS_CHANGEHDUSCHEMEINFO_NACK,             EV_MCSVC_BGN + 939 );

    //hdu通道选看vmp
	OSPEVENT( MCS_MCU_STARTSWITCH_HDUVMP_REQ,               EV_MCSVC_BGN + 940 );
	OSPEVENT( MCS_MCU_STARTSWITCH_HDUVMP_ACK,               EV_MCSVC_BGN + 941 );
	OSPEVENT( MCS_MCU_STARTSWITCH_HDUVMP_NACK,              EV_MCSVC_BGN + 942 );

	//hdu通道停止选看vmp
	OSPEVENT( MCS_MCU_STOPSWITCH_HDUVMP_REQ,                EV_MCSVC_BGN + 943 );
	OSPEVENT( MCS_MCU_STOPSWITCH_HDUVMP_ACK,                EV_MCSVC_BGN + 944 );
	OSPEVENT( MCS_MCU_STOPSWITCH_HDUVMP_NACK,               EV_MCSVC_BGN + 945 );

	//hdu轮询
	//会议控制台命令该会议开始HDU轮询, 消息体：  THduPollInfo + TMtPollParam数组
	OSPEVENT( MCS_MCU_STARTHDUPOLL_CMD,                     EV_MCSVC_BGN + 950 ); 
	//会议控制台命令该会议停止HDU轮询, 消息体：  无
	OSPEVENT( MCS_MCU_STOPHDUPOLL_CMD,                      EV_MCSVC_BGN + 951 );
	//会议控制台命令该会议暂停HDU轮询, 消息体：  无
	OSPEVENT( MCS_MCU_PAUSEHDUPOLL_CMD,                     EV_MCSVC_BGN + 952 );
	//会议控制台命令该会议继续HDU轮询, 消息体：  无
	OSPEVENT( MCS_MCU_RESUMEHDUPOLL_CMD,                    EV_MCSVC_BGN + 953 );
	
	//会议控制台向MCU查询HDU轮询参数, 消息体：  无 
	OSPEVENT( MCS_MCU_GETHDUPOLLPARAM_REQ,                  EV_MCSVC_BGN + 954 );
	//MCU应答会议控制台查询HDU轮询参数, 消息体：  THduPollInfo + TMtPollParam数组
	OSPEVENT( MCU_MCS_GETHDUPOLLPARAM_ACK,                  EV_MCSVC_BGN + 955 ); 
	//MCU拒绝会议控制台查询HDU轮询参数, 消息体：  无
	OSPEVENT( MCU_MCS_GETHDUPOLLPARAM_NACK,                 EV_MCSVC_BGN + 956 );
	//MCU通知会议控制台当前的轮询状态, 消息体：  THduPollInfo
    OSPEVENT( MCU_MCS_HDUPOLLSTATE_NOTIF,                   EV_MCSVC_BGN + 957 );

	// xliang [12/3/2008] hdu相关操作
	//开始HDU图像交换，消息体：THduSwitchInfo
	OSPEVENT( MCS_MCU_START_SWITCH_HDU_REQ,					EV_MCSVC_BGN + 958 );
	OSPEVENT( MCU_MCS_START_SWITCH_HDU_ACK,					EV_MCSVC_BGN + 959 );
	OSPEVENT( MCU_MCS_START_SWITCH_HDU_NACK,				EV_MCSVC_BGN + 960 );

	// 设置音量大小 消息体 ：THduVolumeInfo
	OSPEVENT( MCS_MCU_CHANGEHDUVOLUME_REQ,		            EV_MCSVC_BGN + 961 );
	OSPEVENT( MCU_MCS_CHANGEHDUVOLUME_ACK,		            EV_MCSVC_BGN + 962 );
	OSPEVENT( MCU_MCS_CHANGEHDUVOLUME_NACK,		            EV_MCSVC_BGN + 963 );

	// 开始批量轮询 消息体 ：THduSchemePollInfo
	OSPEVENT( MCS_MCU_STARTHDUBATCHPOLL_REQ,		        EV_MCSVC_BGN + 964 );
	OSPEVENT( MCU_MCS_STARTHDUBATCHPOLL_ACK,		        EV_MCSVC_BGN + 965 );
	OSPEVENT( MCU_MCS_STARTHDUBATCHPOLL_NACK,		        EV_MCSVC_BGN + 966 );

	// 停止批量轮询 消息体 ：无
	OSPEVENT( MCS_MCU_STOPHDUBATCHPOLL_REQ,		            EV_MCSVC_BGN + 967 );
	OSPEVENT( MCU_MCS_STOPHDUBATCHPOLL_ACK,		            EV_MCSVC_BGN + 968 );
	OSPEVENT( MCU_MCS_STOPHDUBATCHPOLL_NACK,		        EV_MCSVC_BGN + 969 );

	// 暂停批量轮询 消息体 ：无
	OSPEVENT( MCS_MCU_PAUSEHDUBATCHPOLL_REQ,		        EV_MCSVC_BGN + 970 );
	OSPEVENT( MCU_MCS_PAUSEHDUBATCHPOLL_ACK,		        EV_MCSVC_BGN + 971 );
	OSPEVENT( MCU_MCS_PAUSEHDUBATCHPOLL_NACK,		        EV_MCSVC_BGN + 972 );

	// 恢复批量轮询 消息体 ：无
	OSPEVENT( MCS_MCU_RESUMEHDUBATCHPOLL_REQ,		        EV_MCSVC_BGN + 973 );
	OSPEVENT( MCU_MCS_RESUMEHDUBATCHPOLL_ACK,		        EV_MCSVC_BGN + 974 );
	OSPEVENT( MCU_MCS_RESUMEHDUBATCHPOLL_NACK,		        EV_MCSVC_BGN + 975 );

    // 批量轮询通知 消息体：ThduSchemePollInfo
	OSPEVENT( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF,		    EV_MCSVC_BGN + 976 );


	//取消回传通道中回传终端和交换 消息体 ：无
	OSPEVENT( MCS_MCU_CANCELCASCADESPYMT_REQ,		        EV_MCSVC_BGN + 977 );
	OSPEVENT( MCU_MCS_CANCELCASCADESPYMT_ACK,		        EV_MCSVC_BGN + 978 );
	OSPEVENT( MCU_MCS_CANCELCASCADESPYMT_NACK,		        EV_MCSVC_BGN + 979 );

	//界面向MCU请求关键帧 消息体：TMt + mode
	OSPEVENT( MCS_MCU_GETIFRAME_CMD,						EV_MCSVC_BGN + 980 );
	
	//界面通知MCU将log输出到文件
	OSPEVENT( MCS_MCU_FLUSHLOG_CMD,							EV_MCSVC_BGN + 983 );

	//MCU告知MCS额外的外设状态信息，针对新版本界面HDU扩容使用
	//消息结构:	u8 nSize + TEqp*nSize + u8*nSize + u32*nSize 
	OSPEVENT( MCU_MCS_MCUEXTSTATUS_NOTIFY,					EV_MCSVC_BGN + 981 );
	
	//mcs请求某个会议的电视墙批量轮询信息
	OSPEVENT( MCS_MCU_GETBATCHPOLLSTATUS_CMD,				EV_MCSVC_BGN + 985 );
	//mcs开启抓包
	OSPEVENT( MCS_MCU_STARTNETCAP_CMD,						EV_MCSVC_BGN + 986 );
	
	//mcu告知mcs当前抓包状态
	OSPEVENT( MCU_MCS_NETCAPSTATUS_NOTIFY,					EV_MCSVC_BGN + 987 );

	//mcs通知停止抓包
	OSPEVENT( MCS_MCU_STOPNETCAP_CMD,						EV_MCSVC_BGN + 988 );

	//mcs请求当前抓包状态
	OSPEVENT( MCS_MCU_GETNETCAPSTATUS_REQ,					EV_MCSVC_BGN + 989 );
	OSPEVENT( MCU_MCS_GETNETCAPSTATUS_ACK,					EV_MCSVC_BGN + 990 );
	OSPEVENT( MCU_MCS_GETNETCAPSTATUS_NACK,					EV_MCSVC_BGN + 991 );

	//mcs通知停止抓包
	OSPEVENT( MCS_MCU_STOPDEBUGMODE_CMD,					EV_MCSVC_BGN + 992 );

	//mcs请求开启调试模式 
	OSPEVENT( MCS_MCU_OPENDEBUGMODE_CMD,					EV_MCSVC_BGN + 993 );

	//mcu告知mcs当前调试模式状态 消息体u8 (emDebugModeDebug或emDebugModeNull)
	OSPEVENT( MCU_MCS_OPENDEBUGMOED_NOTIFY,					EV_MCSVC_BGN + 994 );

	//mcs请求当前抓包状态
	OSPEVENT( MCS_MCU_GETDEBUGMODE_REQ,						EV_MCSVC_BGN + 995 );
	OSPEVENT( MCU_MCS_GETDEBUGMODE_ACK,						EV_MCSVC_BGN + 996 );
	OSPEVENT( MCU_MCS_GETDEBUGMODE_NACK,					EV_MCSVC_BGN + 997 );

	//会议控制台命令该会议更改HDU轮询参数, 消息体：  TPollInfo + TMtPollParam数组
	OSPEVENT( MCS_MCU_CHANGEHDUPOLLPARAM_CMD,        		EV_MCSVC_BGN + 998 );

	
#endif /*_EVMCUMCS_H_*/
