 /*****************************************************************************
   模块名      : 新方案业务
   文件名      : evagtsvc.h
   相关文件    : 
   文件实现功能: 新方案业务和代理接口消息定义
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/29  0.9         李屹        创建
******************************************************************************/
#ifndef _EV_AGTSVC_H_
#define _EV_AGTSVC_H_

#include "osp.h"
#include "eventid.h"

/**********************************************************
 以下为代理和业务接口消息（20001-22000）
**********************************************************/

//单板状态改变，MCU->AGENT，消息体为结构CDevStatus
OSPEVENT( MCU_AGT_BOARD_STATUSCHANGE,		EV_AGTSVC_BGN );
//DRI、DSI单板E1状态改变，MCU->AGENT，消息体为结构CDevStatus
OSPEVENT( MCU_AGT_LINK_STATUSCHANGE,		EV_AGTSVC_BGN + 1 );
//MTE1状态改变，MT->AGENT，消息体为结构CDevStatus
OSPEVENT( MT_AGT_E1_STATUSCHANGE,			EV_AGTSVC_BGN + 2 );    //已经不用，可以删除
//同步源状态改变，MCU->AGENT，消息体为结构CSyncSourceAlarm
OSPEVENT( MCU_AGT_SYNCSOURCE_STATUSCHANGE,  EV_AGTSVC_BGN + 3 );
//当前同步源发生切换，MCU->AGENT，消息体为结构CCurrentMasterSync
OSPEVENT( MCU_AGT_SYNCSOURCE_SWITCH,		EV_AGTSVC_BGN + 4 );    //已经不用，可以删除
//内存状态改变，SVC->AGENT，消息体为单字节，0正常，1异常
OSPEVENT( SVC_AGT_MEMORY_STATUS,			EV_AGTSVC_BGN + 100 );
//文件系统状态改变，SVC->AGENT，消息体为单字节，0正常，1异常
OSPEVENT( SVC_AGT_FILESYSTEM_STATUS,		EV_AGTSVC_BGN + 101 );
//任务状态改变，SVC->AGENT，消息体为结构CTaskStatus
OSPEVENT( SVC_AGT_TASK_STATUS,		        EV_AGTSVC_BGN + 102 );
//MAP状态改变，SVC->AGENT，消息体为结构CMapStatus
OSPEVENT( SVC_AGT_MAP_STATUS,		        EV_AGTSVC_BGN + 103 );  //已经不用，可以删除
//CCI状态改变，SVC->AGENT，消息体为结构CCCIStatus
OSPEVENT( SVC_AGT_CCI_STATUS,		        EV_AGTSVC_BGN + 104 );  //已经不用，可以删除
//以太网状态，SVC->AGENT，消息体为结构CEthernetCardStatus
OSPEVENT( SVC_AGT_ETHERNETCARD_STATUS,		EV_AGTSVC_BGN + 105 );  //已经不用，可以删除
//媒体流状态，SVC->AGENT，消息体为结构CMediaStreamStatus
OSPEVENT( SVC_AGT_MEDIASTREAM_STATUS,		EV_AGTSVC_BGN + 106 );  //已经不用，可以删除
//设备连接状态改变，SVC->AGENT，消息体为结构CEqpConnStatus
OSPEVENT( SVC_AGT_EQPCONNECT_STATUS,		EV_AGTSVC_BGN + 107 );  //已经不用，可以删除
//视频矩阵的状态, SVC->AGENT, 消息体为单字节，0正常，1异常
OSPEVENT( SVC_AGT_VIDEOSOURCE_STATUS,		EV_AGTSVC_BGN + 108 );  //已经不用，可以删除
//风扇状态改变，SVC->AGENT，消息体为结构CFanStatus
OSPEVENT( SVC_AGT_FAN_STATUS,		        EV_AGTSVC_BGN + 109 );  //已经不用，可以删除
//V35状态改变，SVC->AGENT, 消息体为单字节 0正常，bit0载波检测失败
//bit1清除发送失败, bit2请求发送失败,bit3数据终端未就绪,bit4数据未准备好
OSPEVENT( SVC_AGT_V35_STATUS,		        EV_AGTSVC_BGN + 110 );  //已经不用，可以删除
//SDH状态改变，Guard->AGENT，消息体为结构TSDHStatus
OSPEVENT( SVC_AGT_SDH_STATUS,		        EV_AGTSVC_BGN + 111 );

//电源状态改变，Guard->AGENT，消息体为结构TPowerStatus
OSPEVENT( SVC_AGT_POWER_STATUS,		        EV_AGTSVC_BGN + 112 );
//模块状态改变，Guard->AGENT，消息体为结构TModuleStatus
OSPEVENT( SVC_AGT_MODULE_STATUS,		    EV_AGTSVC_BGN + 113 );
//电源风扇状态改变，Guard->AGENT，消息体为结构TPowerFanStatus
OSPEVENT( SVC_AGT_POWER_FAN_STATUS,		    EV_AGTSVC_BGN + 114 );

//会议开始通知消息, 消息体为TConfNotify
OSPEVENT( SVC_AGT_CONFERENCE_START,		    EV_AGTSVC_BGN + 115 );
//会议结束通知消息, 消息体为TConfNotify
OSPEVENT( SVC_AGT_CONFERENCE_STOP,		    EV_AGTSVC_BGN + 116 );

//增加终端通知消息, 消息体为TMtNotify 
OSPEVENT( SVC_AGT_ADD_MT,		            EV_AGTSVC_BGN + 117 );
//删除终端通知消息, 消息体为TMtNotify
OSPEVENT( SVC_AGT_DEL_MT,		            EV_AGTSVC_BGN + 118 );
OSPEVENT( SVC_AGT_LED_STATUS,		        EV_AGTSVC_BGN + 119 );

// MCU命令代理和单板断链, zgc, 2007-08-28
// 消息体: u8 byBrdLayer, u8 byBrdSlot
OSPEVENT( SVC_AGT_DISCONNECTBRD_CMD,		EV_AGTSVC_BGN + 120 );

// miaoqingsong  NMS5.0 add
// 机箱风扇状态告警上报，Guard->AGENT，消息体为结构TBoxFanStatus
OSPEVENT( SVC_AGT_BOX_FAN_STATUS,           EV_AGTSVC_BGN + 121 );      
//CPU状态改变，SVC->AGENT，消息体为结构TMPCCpuStatus
OSPEVENT( SVC_AGT_CPU_STATUS,               EV_AGTSVC_BGN + 122 );    
//内存状态改变，SVC->AGENT，消息体为结构TMPCMemoryStatus
OSPEVENT( SVC_AGT_MPCMEMORY_STATUS,         EV_AGTSVC_BGN + 123 );
//电源板温度状态改变，SVR->AGENT,消息体为结构TPowerBrdTempStatus
OSPEVENT( SVC_AGT_POWERTEMP_STATUS,         EV_AGTSVC_BGN + 124 );
//MPC2板温度状态改变，SVC->AGENT,消息体为结构TMPC2TempStatus
OSPEVENT( SVC_AGT_MPC2TEMP_STATUS,          EV_AGTSVC_BGN + 125 );
//CPU风扇状态告警上报 Guard->AGENT
OSPEVENT( SVC_AGT_CPU_FAN_STATUS,          EV_AGTSVC_BGN + 126 );
//CPU温度状态告警上报 Guard->AGENT
OSPEVENT( SVC_AGT_CPUTEMP_STATUS,          EV_AGTSVC_BGN + 127 );
// NMS5.0 end     

// MCU请求代理设置/修改DSC info, zgc, 2007-07-16
OSPEVENT( SVC_AGT_SETDSCINFO_REQ,			EV_AGTSVC_BGN + 130 );
// 代理的设置/修改DSC info应答, zgc, 2007-07-16
OSPEVENT( AGT_SVC_SETDSCINFO_ACK,			EV_AGTSVC_BGN + 131 );
// 代理的设置/修改DSC info否定应答, zgc, 2007-07-16
OSPEVENT( AGT_SVC_SETDSCINFO_NACK,			EV_AGTSVC_BGN + 132 );

// MCU通知代理GK info, zgc, 2007-07-21
OSPEVENT( SVC_AGT_DSCGKINFO_UPDATE_CMD,			EV_AGTSVC_BGN + 134 );
// 代理通知MCU DSC板已经注册成功, zgc, 2007-07-21
OSPEVENT( AGT_SVC_DSCREGSUCCEED_NOTIF,		EV_AGTSVC_BGN + 135 );
// MCU通知代理更新LoginInfo, zgc, 2007-10-12
OSPEVENT( SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD,	EV_AGTSVC_BGN + 136 );

// 单板状态通知,(u8)0:正常，1:断链, 2:未知
OSPEVENT( AGT_SVC_BOARDSTATUS_NOTIFY,       EV_AGTSVC_BGN + 150 );
// 备用MPC状态通知,由MS应用发给MCU代理,(u8)1:正常，2:断链
OSPEVENT( SVC_AGT_STANDBYMPCSTATUS_NOTIFY,  EV_AGTSVC_BGN + 151 );


//Mcu 通知代理性能改变通知
OSPEVENT( MCU_AGT_PFM_INFOCHANGE,			EV_AGTSVC_BGN + 152 );

//冷启动完毕，SVC->AGENT
OSPEVENT( SVC_AGT_COLD_RESTART,				EV_AGTSVC_BGN + 200 );
//关机，SVC->AGENT
OSPEVENT( SVC_AGT_POWEROFF,					EV_AGTSVC_BGN + 201 );
//配置出错
OSPEVENT( SVC_AGT_CONFIG_ERROR,				EV_AGTSVC_BGN + 202 );

//mcu通知代理更新单板版本(包括自己)
//消息体：u8(单板索引) + u8(源文件名个数) + u8(文件名长度) + s8[](文件名)  //注：(文件名包含绝对路径)
OSPEVENT( SVC_AGT_UPDATEBRDVERSION_CMD,     EV_AGTSVC_BGN + 205 );
//有新的DSC板注册请求(8000B), zgc, 2007-03-05
//消息体：u8(单板类型) + u32(DSC板IP,网络序) + u32(DSC板连接的本地MCU的IP, 网络序) + u32(本地IP MASK, 网络序) 
OSPEVENT( AGT_SVC_NEWDSCREGREQ_NOTIFY,		EV_AGTSVC_BGN + 206);
//版本更新结果通知，消息体：u8(单板索引) + u8(1-成功，0-失败)
OSPEVENT( AGT_SVC_UPDATEBRDVERSION_NOTIF,   EV_AGTSVC_BGN + 208 );

//命令DSC单板更新版本, zgc, 2007-08-20
//消息体：u8(单板索引) + u8(会控会话索引) + u8(源文件名个数) + u8(文件名长度) + s8[](文件名)  //注：(文件名包含绝对路径)
OSPEVENT( SVC_AGT_STARTUPDATEDSCVERSION_REQ, EV_AGTSVC_BGN + 209 );
//消息体：同上
OSPEVENT( AGT_SVC_STARTUPDATEDSCVERSION_ACK, EV_AGTSVC_BGN + 210 );
//消息体：同上
OSPEVENT( AGT_SVC_STARTUPDATEDSCVERSION_NACK,EV_AGTSVC_BGN + 211 );

//消息体: u8(单板索引) + u8(会控会话索引) + TDscUpdateReqHead + [filecontent]……
OSPEVENT( SVC_AGT_DSCUPDATEFILE_REQ,		EV_AGTSVC_BGN + 213 );
//消息体: u8(单板索引) + u8(会控会话索引) + TDscUpdateRsp
OSPEVENT( AGT_SVC_DSCUPDATEFILE_ACK,		EV_AGTSVC_BGN + 214 );
//消息体: u8(单板索引) + u8(会控会话索引) + TDscUpdateRsp
OSPEVENT( AGT_SVC_DSCUPDATEFILE_NACK,	EV_AGTSVC_BGN + 215 );

//MCU通知代理会控注册成功DSC板建立路由, zq, 2008-07-14
//消息体：u8(单板层号) + u8(单板槽号) + u32(IP个数) + u32(会控IP1 网络序) + u32(会控IP2 网络序) + …………………………
OSPEVENT( SVC_AGT_DSCCREATEROUTE_CMD,		EV_AGTSVC_BGN + 216 );

//MCU通知代理会控断连DSC板删除路由, zq, 2008-07-14
//消息体：u8(单板层号) + u8(单板槽号) + u32(会控IP 网络序)
OSPEVENT( SVC_AGT_DSCDELROUTE_CMD,		EV_AGTSVC_BGN + 218 );

//主mcu通知代理备mcu的LED灯显的notify
OSPEVENT( SVC_AGT_LEDSTATUS_NOTIFY,		EV_AGTSVC_BGN + 219 );

//单板配置通知
//消息体: u8(单板层号) + u8(单板槽号) + u8(外设个数)+[u8(外设类型)+外设配置信息结构+……]+u8(E1_CLK_OUTPUT_ENABLE)+u8(网同步时钟参考选择)+u32(单板IP)+u8(使用看门狗标志)
OSPEVENT( SVC_AGT_BOARDCFGMODIFY_NOTIF,		EV_AGTSVC_BGN + 217 );
//E1线路带宽通知
OSPEVENT( AGT_SVC_E1BANDWIDTH_NOTIF,		EV_AGTSVC_BGN + 218 );

//代理发的需重启动消息，无消息体
OSPEVENT( AGT_SVC_REBOOT,					EV_AGTSVC_BGN + 300 );
//代理发的需待机消息，无消息体
OSPEVENT( AGT_SVC_POWEROFF,					EV_AGTSVC_BGN + 301 );
//代理发的需开机消息，无消息提
OSPEVENT( AGT_SVC_POWERON,					EV_AGTSVC_BGN + 302 );

//代理发的，无消息体
OSPEVENT( AGT_SVC_PFMINFO_REQ,				EV_AGTSVC_BGN + 303 );


/*以下是MCU AGENT单板管理的消息*/

//单板的注册消息，消息体为TBrdPosition+u32(单板IP)+u8(网口号)+u8(OsType)
OSPEVENT( BOARD_MPC_REG,					EV_AGTSVC_BGN + 401 );
//单板的注册应答
OSPEVENT( MPC_BOARD_REG_ACK,				EV_AGTSVC_BGN + 402 );
//单板的注册否定应答
OSPEVENT( MPC_BOARD_REG_NACK,				EV_AGTSVC_BGN + 403 );


//单板取配置的消息
OSPEVENT( BOARD_MPC_GET_CONFIG,				EV_AGTSVC_BGN + 404 );
//单板取配置应答消息
//消息体为 u8(外设个数)+[u8(外设类型)+外设配置信息结构+……]+u8(E1_CLK_OUTPUT_ENABLE)+u8(网同步时钟参考选择)+u32(单板IP)+u8(使用看门狗标志)
OSPEVENT( MPC_BOARD_GET_CONFIG_ACK,			EV_AGTSVC_BGN + 405 );
//单板取配置否定应答消息
OSPEVENT( MPC_BOARD_GET_CONFIG_NACK,		EV_AGTSVC_BGN + 406 );

//单板告警同步消息
OSPEVENT( MPC_BOARD_ALARM_SYNC_REQ,			EV_AGTSVC_BGN + 407 );
//单板告警同步消息的应答
//消息体为 sizeof(TBrdPosition)  + sizeof(u16)[ 告警个数] + 告警个数 * sizeof(TBoardAlarmMsgInfo)
OSPEVENT( BOARD_MPC_ALARM_SYNC_ACK,			EV_AGTSVC_BGN + 408 );
//单板给MPC的告警通知
//消息体为 sizeof(TBrdPosition)  + sizeof(u16)[ 告警个数] + 告警个数 * sizeof(TBoardAlarmMsgInfo)
OSPEVENT( BOARD_MPC_ALARM_NOTIFY,			EV_AGTSVC_BGN + 409 );


//MPC让单板进行误码测试的消息
OSPEVENT( MPC_BOARD_BIT_ERROR_TEST_CMD,		EV_AGTSVC_BGN + 410 );
//MPC让单板进行时间同步的消息
OSPEVENT( MPC_BOARD_TIME_SYNC_CMD,			EV_AGTSVC_BGN + 411 );
//MPC让单板进行自测的消息
OSPEVENT( MPC_BOARD_SELF_TEST_CMD,			EV_AGTSVC_BGN + 412 );
//MPC让单板进行重启动的消息
OSPEVENT( MPC_BOARD_RESET_CMD,			    EV_AGTSVC_BGN + 413 );


//MPC获取单板的统计信息的消息  (对应不同的单板，统计信息结构可能不一致)
OSPEVENT( MPC_BOARD_GET_STATISTICS_REQ,		EV_AGTSVC_BGN + 414 );
//单板给MPC的统计信息应答 
OSPEVENT( BOARD_MPC_GET_STATISTICS_ACK,		EV_AGTSVC_BGN + 415 );

//MPC获取单板模块信息的消息
OSPEVENT( MPC_BOARD_GET_MODULE_REQ,			EV_AGTSVC_BGN + 417 );
//单板给MPC的模块信息应答
OSPEVENT( BOARD_MPC_GET_MODULE_ACK,			EV_AGTSVC_BGN + 418 );
//单板E1带宽指示
OSPEVENT( BOARD_MPC_E1BANDWIDTH_NOTIF,		EV_AGTSVC_BGN + 419 );


//MPC获取单板版本信息的消息
OSPEVENT( MPC_BOARD_GET_VERSION_REQ,		EV_AGTSVC_BGN + 420 );
//单板给MPC的版本信息应答
OSPEVENT( BOARD_MPC_GET_VERSION_ACK,		EV_AGTSVC_BGN + 421 );


//单板代理启动消息
OSPEVENT( BOARD_AGENT_POWERON,			    EV_AGTSVC_BGN + 423 );
//单板GUARD模块启动消息
OSPEVENT( BOARD_GUARD_POWERON,              EV_AGTSVC_BGN + 424 );

//MPC让单板进行软件更新的消息
//消息体: u8(单板索引) + u8(源文件名个数) + u8(文件名长度) + s8[](文件名)  //注：(linux文件名包含绝对路径)
OSPEVENT( MPC_BOARD_UPDATE_SOFTWARE_CMD,	EV_AGTSVC_BGN + 425 );

//版本更新结果通知，消息体：u8(单板索引) + u8(文件数) + u8[](是否成功<对会控>) 
// + u8(文件长度) + s8[](文件名) + u8 + s8[] + ...
OSPEVENT( BOARD_MPC_UPDATE_NOTIFY,			EV_AGTSVC_BGN + 426 );

//MPC发送给单板的E1环回命令消息
OSPEVENT( MPC_BOARD_E1_LOOP_CMD,			EV_AGTSVC_BGN + 428 );

/************************************************************************/
/*  以下是单板守卫(GUARD)定时扫描产生的单板状态改变消息                 */
/************************************************************************/
//单板E1状态改变消息
OSPEVENT( BOARD_LINK_STATUS,			    EV_AGTSVC_BGN + 429 );
//单板模块状态改变消息
OSPEVENT( BOARD_MODULE_STATUS,              EV_AGTSVC_BGN + 430 );
//单板面板灯状态消息
OSPEVENT( BOARD_LED_STATUS,                 EV_AGTSVC_BGN + 431 );
//单板风扇状态消息
OSPEVENT( MCU_BRD_FAN_STATUS,               EV_AGTSVC_BGN + 450 );

//MPC发送给单板的取单板的面板灯状态消息
OSPEVENT( MPC_BOARD_LED_STATUS_REQ,         EV_AGTSVC_BGN + 432 );
//单板给MPC的取单板的面板灯状态应答消息
OSPEVENT( BOARD_MPC_LED_STATUS_ACK,         EV_AGTSVC_BGN + 433 );
//单板面板灯状态消息
OSPEVENT( BOARD_MPC_LED_STATUS_NOTIFY,      EV_AGTSVC_BGN + 434 );

//Board register information to report to mcu
OSPEVENT( AGENT_MCU_REGISTER_NOTIFY,        EV_AGTSVC_BGN + 435 );
//显示所有注册单板
OSPEVENT( EV_AGENT_SHOWREGEDBOARD,          EV_AGTSVC_BGN + 436 );

//MPC通知配置信息修改
//消息体: TDSCModuleInfo
OSPEVENT( MPC_BOARD_SETDSCINFO_REQ,         EV_AGTSVC_BGN + 437 );
OSPEVENT( BOARD_MPC_SETDSCINFO_ACK,         EV_AGTSVC_BGN + 438 );
OSPEVENT( BOARD_MPC_SETDSCINFO_NACK,        EV_AGTSVC_BGN + 439 );

//单板温度异常状态通知(单板guard->单板agent)
OSPEVENT(BOARD_TEMPERATURE_STATUS_NOTIF,			EV_AGTSVC_BGN + 440);
//单板温度异常状态通知（0:正常 1:异常）
OSPEVENT(SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY,        EV_AGTSVC_BGN + 441);
//单板CPU占用率状态通知(0:正常 1:占用过高 单板guard->单板agent)
OSPEVENT(BOARD_CPU_STATUS_NOTIF,			EV_AGTSVC_BGN + 442); 
//单板CPU占用率状态通知(0:正常 1:占用过高)
OSPEVENT(SVC_AGT_BRD_CPU_STATUS_NOTIF,			EV_AGTSVC_BGN + 443); 

//MPC命令更新DSCGKINFO, 消息体：u32(网络序), zgc, 2007-07-21
OSPEVENT( MPC_BOARD_DSCGKINFO_UPDATE_CMD,	EV_AGTSVC_BGN + 444 );
//MPC命令和单板断链, zgc, 2007-08-28
OSPEVENT( MPC_BOARD_DISCONNECT_CMD,			EV_AGTSVC_BGN + 445 );
//单板配置修改统治, zgc, 2007-09-24
OSPEVENT( MCU_BOARD_CONFIGMODIFY_NOTIF,	EV_AGTSVC_BGN + 451 );
//MPC命令DSC更新LoginInfo, zgc, 2007-10-12
OSPEVENT( MCU_BOARD_DSCTELNETLOGININFO_UPDATE_CMD, EV_AGTSVC_BGN + 452 );

//MPC通知DSC板会控注册成功需要建立路由, zq, 2008-07-14
//消息体：u8(单板层号) + u8(单板槽号) + u32(IP个数) + u32(会控IP1 网络序) + u32(会控IP2 网络序) + …………………………
OSPEVENT( MPC_BOARD_DSCCREATEROUTE_CMD,	EV_AGTSVC_BGN + 460 );

//MPC通知DSC板会控断连需要删除路由, zq, 2008-07-14
//消息体：u8(单板层号) + u8(单板槽号) + u32(msc IP地址)
OSPEVENT( MPC_BOARD_DSCDELROUTE_CMD,	EV_AGTSVC_BGN + 462 );

//单板通知MPC板软件版本号 [12/13/2011 chendaiwei]
//OSPEVENT( BOARD_MPC_SOFTWARE_VERSION_NOTIFY, EV_AGTSVC_BGN + 463 );
//单板通知MPC板软件版本号 [12/13/2011 chendaiwei]
//OSPEVENT(SVC_AGT_BRD_SOFTWARE_VERSION_NOTIFY, EV_AGTSVC_BGN + 464);

// 初始化数据区
OSPEVENT( EV_MSGCENTER_POWER_ON,            EV_AGTSVC_BGN + 500 );
// 关机通知
OSPEVENT( EV_MSGCENTER_POWER_OFF,           EV_AGTSVC_BGN + 501 );
// 冷启动
OSPEVENT( EV_AGENT_COLD_RESTART,            EV_AGTSVC_BGN + 502 );
// 重启备份mcu
OSPEVENT( EV_AGENT_MCU_MCU_RESTART,         EV_AGTSVC_BGN + 503 );
//升级重启备份mcu
OSPEVENT( EV_AGENT_MCU_MCU_UPDATE,          EV_AGTSVC_BGN + 504 );
// Led 告警
OSPEVENT( EV_BOARD_LED_ALARM,               EV_AGTSVC_BGN + 505 );
// 删除单板告警
OSPEVENT( EV_DELETEBOARD_ALARM,             EV_AGTSVC_BGN + 506 );
// 初始化Snmp信息
OSPEVENT( EV_TOMSGCENTER_INITAL_SNMPINFO,   EV_AGTSVC_BGN + 510 );
// 单板代理连接Mcu代理
OSPEVENT( EV_BOARD_MPC_CONNECT,             EV_AGTSVC_BGN + 511 );
// 单板代理向Mcu代理注册
OSPEVENT( EV_BOARD_MPC_REGISTER,            EV_AGTSVC_BGN + 512 );
// MPC 板升级完成通知
OSPEVENT( EV_MSGCENTER_MPC_UPDATED_NTF,     EV_AGTSVC_BGN + 513 );
// 普通单板升级完成通知
OSPEVENT( EV_MSGCENTER_BRD_UPDATED_NTF,     EV_AGTSVC_BGN + 514 );

// 网管ftp推式升级单板
OSPEVENT( EV_MSGCENTER_NMS_UPDATEDMPC_CMD,  EV_AGTSVC_BGN + 515 );
// 网管ftp拉式升级单板
//OSPEVENT( EV_SELFCENTER_NMS_UPDATEDMPC_CMD,  EV_AGTSVC_BGN + 531 );

// mpc请求DSC更新版本, zgc, 2007-08-20
//消息体:  u8(单板索引) + u8(会控会话索引) + u8(源文件名个数) + u8(文件名长度) + s8[](文件名)  //注：(linux文件名包含绝对路径)
OSPEVENT( MPC_DSC_STARTUPDATE_SOFTWARE_REQ,	EV_AGTSVC_BGN + 516 );
//消息体:  同上
OSPEVENT( DSC_MPC_STARTUPDATE_SOFTWARE_ACK,	EV_AGTSVC_BGN + 517 );
//消息体:  同上
OSPEVENT( DSC_MPC_STARTUPDATE_SOFTWARE_NACK,EV_AGTSVC_BGN + 518 );

//消息体: u8(单板索引) + u8(会控会话索引) + TDscUpdateReqHead + [filecontent]……
OSPEVENT( MPC_DSC_UPDATEFILE_REQ,			EV_AGTSVC_BGN + 519 );
//消息体: u8(单板索引) + u8(会控会话索引) + TDscUpdateRsp
OSPEVENT( DSC_MPC_UPDATEFILE_ACK,			EV_AGTSVC_BGN + 520 );
//消息体: u8(单板索引) + u8(会控会话索引) + TDscUpdateRsp
OSPEVENT( DSC_MPC_UPDATEFILE_NACK,			EV_AGTSVC_BGN + 521 );
//消息体: 
OSPEVENT( SVC_AGT_MCULED_ALARM,		        EV_AGTSVC_BGN + 522 );
// 升级定时器
OSPEVENT( DSC_UPDATE_SOFTWARE_WAITTIMER,	EV_AGTSVC_BGN + 530 );

// 网管ftp拉式升级单板
OSPEVENT( EV_SM_UPDATEDMPC_CMD,				EV_AGTSVC_BGN + 531 );

// 定时检测网线是否断掉
OSPEVENT( EV_ETH_STATDETECT_TIME,			EV_AGTSVC_BGN + 533 );

// 通知mcucfg网口发生切变
OSPEVENT( AGT_MCU_CFG_ETHCHANGE_NOTIF,		EV_AGTSVC_BGN + 534 );

OSPEVENT( BOARD_MPC_CFG_TEST,				EV_AGTSVC_BGN + 1000 );
OSPEVENT( BOARD_MPC_GET_ALARM,				EV_AGTSVC_BGN + 1001 );
OSPEVENT( BOARD_MPC_QUERY_ALARM,			EV_AGTSVC_BGN + 1002 );
OSPEVENT( BOARD_MPC_MANAGERCMD_TEST,        EV_AGTSVC_BGN + 1003 );
OSPEVENT( TEST_BOARDMANAGERCMD_DUMPINST,    EV_AGTSVC_BGN + 1004 );

//  xsl [6/9/2006] 单板代理用定时器id
OSPEVENT( BRDAGENT_CONNECT_MANAGERA_TIMER,  EV_AGTSVC_BGN + 1200 );
OSPEVENT( BRDAGENT_CONNECT_MANAGERB_TIMER,  EV_AGTSVC_BGN + 1201 );
OSPEVENT( BRDAGENT_REGISTERA_TIMER,         EV_AGTSVC_BGN + 1202 );
OSPEVENT( BRDAGENT_REGISTERB_TIMER,         EV_AGTSVC_BGN + 1203 );
OSPEVENT( BRDAGENT_GET_CONFIG_TIMER,        EV_AGTSVC_BGN + 1204 );
OSPEVENT( BRDAGENT_SCAN_STATE_TIMER,        EV_AGTSVC_BGN + 1205 );

OSPEVENT( BRDAGENT_CONNECT_TEST_TIMER,      EV_AGTSVC_BGN + 1207 );
OSPEVENT( BRDAGENT_CONNECT_TEST_OVER_TIMER,		EV_AGTSVC_BGN + 1208 ); // zgc, 2007-03-15

// [11/25/2010 xliang] new ID
OSPEVENT( BRDAGENT_CONNECT_MANAGER_TIMER,   EV_AGTSVC_BGN + 1210);
OSPEVENT( BRDAGENT_REGISTER_TIMER,			EV_AGTSVC_BGN + 1211);
OSPEVENT( BRDAGENT_E1CHECK_TIMER,           EV_AGTSVC_BGN + 1212);      //E1线路检测定时器

//mcu代理用定时器
OSPEVENT( MCUAGENT_SCAN_STATE_TIMER,        EV_AGTSVC_BGN + 1220 );
OSPEVENT( MCUAGENT_PRINT_CONFIG_ERROR_TIMER,EV_AGTSVC_BGN + 1221 );
OSPEVENT( MCUAGENT_UPDATE_MCUPFMINFO_TIMER, EV_AGTSVC_BGN + 1222 );     // 代理更新pfmInfo
OSPEVENT( MCUAGENT_OPENORCLOSE_MPCLED_TIMER,EV_AGTSVC_BGN + 1223 );     // MPC板NMS灯状态 mqs 20101118 add

// add for IS2.2（ 8548 sys(MASTER) and 8313 sys(SLAVE) ）[05/04/2012 liaokang]
/*  以下是IS2.2两套系统新增消息：建链、注册、定时扫描及状态获取  */
// 建链
OSPEVENT( BRDSYS_SLAVE_MASTER_CONNECT_TIMER,EV_AGTSVC_BGN + 1224 );     // 8313
// 注册
OSPEVENT( BRDSYS_SLAVE_MASTER_REG_REQ,      EV_AGTSVC_BGN + 1225 );     // 8313 -> 8548 BoardAgent
OSPEVENT( BRDSYS_MASTER_SLAVE_REG_ACK,      EV_AGTSVC_BGN + 1226 );     // 8548 BoardAgent -> 8313
OSPEVENT( BRDSYS_MASTER_SLAVE_REG_NACK,     EV_AGTSVC_BGN + 1227 );     // 8548 BoardAgent -> 8313
OSPEVENT( BRDSYS_SLAVE_MASTER_REG_TIMER,    EV_AGTSVC_BGN + 1228 );     // 8313
// 定时扫描
OSPEVENT( BRDSYS_SLAVE_SCAN_STATE_TIMER,    EV_AGTSVC_BGN + 1229 );     // 8313
// LED状态
OSPEVENT( BRDSYS_SLAVE_MASTER_LED_STATUS_NOTIFY,EV_AGTSVC_BGN + 1230 ); // 8313 -> 8548 BoardAgent
// 网口状态	
OSPEVENT( BRDSYS_SLAVE_MASTER_ETHPORT_STATUS_NOTIFY,EV_AGTSVC_BGN + 1231 );// 8313 -> 8548 BoardAgent
/*  以下是McuAgent、BoardAgent新增消息：获取网口状态  */
OSPEVENT( BOARD_ETHPORT_STATUS,             EV_AGTSVC_BGN + 1232 );     // BoardGuard -> BoardAgent
OSPEVENT( MPC_BOARD_ETHPORT_STATUS_REQ,     EV_AGTSVC_BGN + 1233 );     // McuAgent brdmanager -> BoardAgent
OSPEVENT( BOARD_MPC_ETHPORT_STATUS_ACK,     EV_AGTSVC_BGN + 1234 );     // BoardAgent -> McuAgent brdmanager
OSPEVENT( BOARD_MPC_ETHPORT_STATUS_NOTIFY,  EV_AGTSVC_BGN + 1235 );     // BoardAgent -> McuAgent brdmanager
OSPEVENT( EV_BOARD_ETHPORT_ALARM,           EV_AGTSVC_BGN + 1236 );     // McuAgent brdmanager -> McuAgent agentmsgcenter
OSPEVENT( EV_CHANGE_ETH_NOTIF,				EV_AGTSVC_BGN + 1237 );     // 8000H-M: eth has changed [pengguofeng 6/6/2013]

#endif /* _EV_AGTSVC_H_ */
