/*****************************************************************************
模块名      : 数据会议系统各模块
文件名      : dccommon.h
相关文件    : 无
文件实现功能: 数据会议系统各模块共用的常量定义
作者        : 赖齐
版本        : 4.0
-----------------------------------------------------------------------------
修改记录:
日期(D/M/Y)     版本     修改人      修改内容
16/08/2005      0.1      赖齐        从V3.6移植
******************************************************************************/

#ifndef _DCCCOMMON_H
#define _DCCCOMMON_H

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif

#include "osp.h"
#include "kdvdef.h"
#include "eventid.h"
#include "errorid.h"

//日志级别定义
#define     LOGLVL_EXCEPTION		 0	 //异常
#define     LOGLVL_IMPORTANT	     1	 //重要日志
#define     LOGLVL_DEBUG1		     254 //一级调试信息
#define     LOGLVL_DEBUG2	         255 //二级调试信息

//数据会议服务器181
#define AID_DCSERVER        (AID_DCS_BGN)       // DCServer App ID
#define AID_DCS_CALLMODULE  (AID_DCS_BGN + 1)   // DCServer中CallModule使用的App ID
#define AID_INT_TEST_DCS    (AID_DCS_BGN + 2)   // DCServer集成测试App ID
#define AID_UNIT_TEST_DCS   (AID_DCS_BGN + 3)   // DCServer单元测试App ID
#define AID_DCCLIB_SIM      (AID_DCS_BGN + 4)   // DCCLib桩模块App ID
#define AID_MT_SIM          (AID_DCS_BGN + 5)   // 终端桩模块App ID
#define AID_DCSGUARD        (AID_DCS_BGN + 6)   // DCSGuard App ID

//数据会议控制台201
#define AID_DCCONSOLE       (AID_DCC_BGN)       //DCConsole App ID
#define AID_INTE_TEST_DCC   (AID_DCC_BGN + 1)   //DCConsole集成测试 App ID
#define AID_UNIT_TEST_DCC   AID_INTE_TEST_DCC   //DCConsole单元测试 App ID

const u16 MAX_E164NUM_LENGTH        = 16;   //E164号码长度

const u16 DCC_TESTAPP_LISTENPORT    = 3000; //20默认端口
const u16 DCS_DEFAULT_PORT          = 1503; //T120默认端口

const u8  DCS_INS_NUM	            = 32;                       //APP最大实例
const u8  DCS_CONF_MAX_NUM	        = 32;                       //会议最大数
const u8  MAX_HEIGHT_IN_DATACONF    = 16;                       //数据会议最大级联高度
const u8  MAX_CONNECTION_PER_MT     = 32;                       //数据会议中每个节点最大连接数（直接上级节点＋直接下级节点）
extern u32 MAX_MT_PER_DATACONF/*       = 128*/;	                    //数据会议所有终端支持数
const u8  MAX_MT_IN_DATACONF        = MAX_CONNECTION_PER_MT;	//数据会议最大显示终端支持数

const u8 MAX_MTENTRY_NUM            = 128;  //地址簿最大终端数
const u8 MAX_CONFENTRY_NUM          = 64;   //地址簿最大模板数
const u8 MAX_DCUSER_NUM             = 64;   //地址簿最大用户数

const char * const CONF_NAME_NUM = "120";   //默认会议名 数字段

const u8 MT_IS_ONLINE				= 1;    //终端在线
const u8 MT_NOT_ONLINE				= 0;    //终端不在线

const u8 DCUSER_IS_ONLINE			= 1;    //用户在线
const u8 DCUSER_NOT_ONLINE			= 0;    //用户不在线

const u8 DCUSER_ADMINISTRATOR		= 2;    //管理员
const u8 DCUSER_OPERATOR			= 1;    //操作员
const u8 DCUSER_READONLY			= 0;    //只读用户

const u8 CONF_ENABLE_APPSHARING   = 0x01;   //会议允许应用共享功能
const u8 CONF_ENABLE_WHITEBOARD   = 0x02;   //会议允许白板功能
const u8 CONF_ENABLE_FILETRANSFER = 0x04;   //会议允许文件传输功能
const u8 CONF_ENABLE_CHAT         = 0x08;   //会议允许聊天功能

const u8 CONF_ENABLE_ACCEPTMTCALL = 0x01;   //会议允许终端主动呼入
const u8 CONF_ENABLE_AUTOINVITEMT = 0x02;   //自动重邀断线终端
const u8 CONF_ENABLE_LOCKED       = 0x04;   //会议被锁定
const u8 CONF_ENABLE_TMMANUAL     = 0x08;   //会议手动终止
const u8 CONF_SUPPORT_CHAIRMAN    = 0x10;   //会议支持主席
const u8 CONF_NEED_PASSWORD       = 0x20;   //会议需要密码

const u8 MAX_USER_LENGTH			= 16;	//用户名最大长度
const u8 MAX_PASSWORD_LENGTH		= 16;	//用户密码最大长度
const u8 MAX_ALIAS_LENGTH			= 32;	//别名最大长度
const u8 MAX_EMAIL_LENGTH			= 32;	//Email地址最大长度(unicode格式长度)

const u8 MAX_DC_CONFNAME_LENGTH		= 32;	//数据会议会议名最大长度
const u8 MAX_DC_PASSWORD_LENGTH		= 16;	//数据会议会议密码最大长度
const u8 MAX_NODENAME_LENGTH		= 32;   //终端名最大长度
const u8 MAX_NODEPHONE_LENGTH		= 16;   //电话号码最大长度(unicode格式长度)
const u8 MAX_NODELOCATION_LENGTH	= 32;   //终端位置最大长度(unicode格式长度)
const u8 MAX_MT_INFO_LENGTH     	= 64;	//终端备注最大长度
const u8 MAX_CONFNAME_NUM_LENGTH	= 16;   //会议名数字段最大长度
const u8 MAX_CONFNAME_TXT_LENGTH	= 64;   //会议名文本段最大长度


//DCSGuard与DCServer之间的消息
//轮询请求
OSPEVENT( DCSGUARD_DCS_POLL_REQ,                EV_DCSDCSGUARD_BGN );	
//轮询应答
OSPEVENT( DCS_DCSGUARD_POLL_ACK,                EV_DCSDCSGUARD_BGN + 1 );


//DCConsole与DCServer之间的消息

//DCConsole请求与DCServer建链，DCC->DCC->DCS，消息体为TDCUser
OSPEVENT( DCC_DCS_CONNECT_REQ,                  EV_DCSDCC_BGN );	
//DCS与DCC建链成功，DCS->DCC，消息体为TDCUser
OSPEVENT( DCS_DCC_CONNECT_ACK,                  EV_DCSDCC_BGN + 1 );
//DCS拒绝建链，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_CONNECT_NACK,                 EV_DCSDCC_BGN + 2 );

//DCConsole请求与DCServer断链，DCC->DCC，消息体为u32
OSPEVENT( DCC_DCS_DISCONNECT_REQ,               EV_DCSDCC_BGN + 6);	
//DCS与DCC断链成功，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DISCONNECT_ACK,               EV_DCSDCC_BGN + 7 );
//DCS拒绝断链，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DISCONNECT_NACK,              EV_DCSDCC_BGN + 8 );

//DCConsole请求DCServer发送地址薄信息，DCC->DCS，无消息体
OSPEVENT( DCC_DCS_GET_ADDRBOOK_REQ,             EV_DCSDCC_BGN +	12 );
//DCS发送地址薄完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_ADDRBOOK_ACK,             EV_DCSDCC_BGN + 13 );
//DCS拒绝发送地址薄，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_ADDRBOOK_NACK,            EV_DCSDCC_BGN + 14 );

//DCConsole请求DCServer增加终端，DCC->DCS，消息体为TDCBaseMt
OSPEVENT( DCC_DCS_ADD_MTENTRY_REQ,				EV_DCSDCC_BGN +	18 );
//DCS发送增加终端完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_ADD_MTENTRY_ACK,			    EV_DCSDCC_BGN + 19 );
//DCS拒绝增加终端，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_ADD_MTENTRY_NACK,			    EV_DCSDCC_BGN + 20 );
//DCS增量发送终端增加通知,DCS->DCC,消息体为TDCBaseMt数组
OSPEVENT( DCS_DCC_ADD_MTENTRY_NOTIF,            EV_DCSDCC_BGN +	21 );

//DCConsole请求DCServer删除终端，DCC->DCS，消息体为TDCBaseMt
OSPEVENT( DCC_DCS_DEL_MTENTRY_REQ,				EV_DCSDCC_BGN +	25 );
//DCS发送删除终端完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DEL_MTENTRY_ACK,				EV_DCSDCC_BGN + 26 );
//DCS拒绝删除终端，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DEL_MTENTRY_NACK,			    EV_DCSDCC_BGN + 27 );
//DCS增量发送终端删除通知,DCS->DCC,消息体为TDCBaseMt
OSPEVENT( DCS_DCC_DEL_MTENTRY_NOTIF,            EV_DCSDCC_BGN +	28 );

//DCConsole请求DCServer修改终端，DCC->DCS，消息体为TDCBaseMt
OSPEVENT( DCC_DCS_MODIFY_MTENTRY_REQ,			EV_DCSDCC_BGN +	32 );
//DCS发送修改终端完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_MODIFY_MTENTRY_ACK,			EV_DCSDCC_BGN + 33 );
//DCS拒绝修改终端，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_MODIFY_MTENTRY_NACK,		    EV_DCSDCC_BGN + 34 );
//DCS增量发送终端修改通知,DCS->DCC,消息体为TDCBaseMt
OSPEVENT( DCS_DCC_MODIFY_MTENTRY_NOTIF,         EV_DCSDCC_BGN +	35 );

//DCConsole请求DCServer增加会议模板，DCC->DCS，消息体为TDataConfTemplate
OSPEVENT( DCC_DCS_ADD_CONFENTRY_REQ,			EV_DCSDCC_BGN +	39 );
//DCS发送增加会议模板完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_ADD_CONFENTRY_ACK,			EV_DCSDCC_BGN + 40 );
//DCS拒绝增加会议模板，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_ADD_CONFENTRY_NACK,			EV_DCSDCC_BGN + 41 );
//DCS增量发送会议模板增加通知,DCS->DCC,消息体为TDataConfTemplate
OSPEVENT( DCS_DCC_ADD_CONFENTRY_NOTIF,          EV_DCSDCC_BGN +	42 );

//DCConsole请求DCServer删除会议模板，DCC->DCS，消息体为CConfId
OSPEVENT( DCC_DCS_DEL_CONFENTRY_REQ,			EV_DCSDCC_BGN +	46 );
//DCS发送删除会议模板完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DEL_CONFENTRY_ACK,			EV_DCSDCC_BGN + 47 );
//DCS拒绝删除会议模板，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DEL_CONFENTRY_NACK,			EV_DCSDCC_BGN + 48 );
//DCS增量发送会议模板删除通知,DCS->DCC,消息体为CConfId
OSPEVENT( DCS_DCC_DEL_CONFENTRY_NOTIF,          EV_DCSDCC_BGN +	49 );

//DCConsole请求DCServer修改会议模板，DCC->DCS，消息体为TDataConfTemplate
OSPEVENT( DCC_DCS_MODIFY_CONFENTRY_REQ,		    EV_DCSDCC_BGN +	53 );
//DCS发送修改会议模板完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_MODIFY_CONFENTRY_ACK,		    EV_DCSDCC_BGN + 54 );
//DCS拒绝修改会议模板，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_MODIFY_CONFENTRY_NACK,		EV_DCSDCC_BGN + 55 );
//DCS增量发送会议模板修改通知,DCS->DCC,消息体为TDataConfTemplate
OSPEVENT( DCS_DCC_MODIFY_CONFENTRY_NOTIF,       EV_DCSDCC_BGN +	56 );

//DCConsole请求DCServer发送会议列表，DCC->DCS，无消息体
OSPEVENT( DCC_DCS_GET_CONFLIST_REQ,             EV_DCSDCC_BGN +	60 );
//DCS发送会议列表完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_CONFLIST_ACK,             EV_DCSDCC_BGN + 61 );
//DCS拒绝发送会议列表，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_CONFLIST_NACK,            EV_DCSDCC_BGN + 62 );

//DCConsole请求DCServer创建会议，DCC->DCS，消息体为TDataConfFullInfo
OSPEVENT( DCC_DCS_CREATE_CONF_REQ,				EV_DCSDCC_BGN +	66 );
//DCS发送创建会议完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_CREATE_CONF_ACK,				EV_DCSDCC_BGN + 67 );
//DCS拒绝创建会议，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_CREATE_CONF_NACK,			    EV_DCSDCC_BGN + 68 );
//DCS增量发送会议状态通知,DCS->DCC,消息体为TDataConfFullInfo
OSPEVENT( DCS_DCC_CONF_NOTIF,                   EV_DCSDCC_BGN +	69 );

//DCConsole请求DCServer结束会议，DCC->DCS，消息体为CConfId
OSPEVENT( DCC_DCS_TERM_CONF_REQ,			    EV_DCSDCC_BGN +	73 );
//DCS发送结束会议完成，DCS->DCC，无消息体	
OSPEVENT( DCS_DCC_TERM_CONF_ACK,			    EV_DCSDCC_BGN + 74 );
//DCS拒绝结束会议，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_TERM_CONF_NACK,			EV_DCSDCC_BGN + 75 );
//DCS增量发送会议结束通知,DCS->DCC,消息体为CConfId
OSPEVENT( DCS_DCC_TERM_CONF_NOTIF,           EV_DCSDCC_BGN +	76 );

//DCConsole请求DCServer指定主席，DCC->DCS，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCC_DCS_SPEC_CHAIRMAN_REQ,			EV_DCSDCC_BGN +	80 );
//DCS发送指定主席完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_SPEC_CHAIRMAN_ACK,			EV_DCSDCC_BGN + 81 );
//DCS拒绝指定主席，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_SPEC_CHAIRMAN_NACK,			EV_DCSDCC_BGN + 82 );
//DCS指定主席通知，DCS->DCC，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCS_DCC_SPEC_CHAIRMAN_NOTIF,			EV_DCSDCC_BGN + 83 );

//DCConsole请求DCServer取消主席，DCC->DCS，消息头会议号有效，无消息体
OSPEVENT( DCC_DCS_CANCEL_CHAIRMAN_REQ,		    EV_DCSDCC_BGN +	87 );
//DCS发送取消主席完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_CANCEL_CHAIRMAN_ACK,		    EV_DCSDCC_BGN + 88 );
//DCS拒绝取消主席，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_CANCEL_CHAIRMAN_NACK,		    EV_DCSDCC_BGN + 89 );
//DCS取消主席通知，DCS->DCC，消息头会议号有效，无消息体
OSPEVENT( DCS_DCC_CANCEL_CHAIRMAN_NOTIF,		EV_DCSDCC_BGN + 90 );

//DCConsole请求DCServer邀请终端，DCC->DCS，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCC_DCS_INVITE_MT_REQ,					EV_DCSDCC_BGN +	94 );
//DCS发送邀请终端完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_INVITE_MT_ACK,					EV_DCSDCC_BGN + 95 );
//DCS拒绝邀请终端，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_INVITE_MT_NACK,					EV_DCSDCC_BGN + 96 );
//DCS终端加入通知，DCS->DCC，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCS_DCC_INVITE_MT_NOTIF,					EV_DCSDCC_BGN + 97 );

//DCConsole请求DCServer退出终端，DCC->DCS，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCC_DCS_EJECT_MT_REQ,					EV_DCSDCC_BGN +	101 );
//DCS发送退出终端完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_EJECT_MT_ACK,					EV_DCSDCC_BGN + 102 );
//DCS拒绝退出终端，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_EJECT_MT_NACK,					EV_DCSDCC_BGN + 103 );
//DCS终端退出通知，DCS->DCC，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCS_DCC_EJECT_MT_NOTIF,					EV_DCSDCC_BGN + 104 );

//DCS终端连线、退出通知，DCS->DCC，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCS_DCC_MTSTATUS_NOTIF,				EV_DCSDCC_BGN + 108 );

//DCConsole请求DCServer用户列表，DCC->DCS，消息体为空
OSPEVENT( DCC_DCS_GET_USERLIST_REQ,			    EV_DCSDCC_BGN + 112 );
//DCS添加用户完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_USERLIST_ACK,			    EV_DCSDCC_BGN + 113 );
//DCS拒绝添加用户，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_USERLIST_NACK,			EV_DCSDCC_BGN +	114 );

//DCConsole请求DCServer添加用户，DCC->DCS，消息体为TDCUser
OSPEVENT( DCC_DCS_ADD_DCC_USER_REQ,			    EV_DCSDCC_BGN + 118 );
//DCS添加用户完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_ADD_DCC_USER_ACK,			    EV_DCSDCC_BGN + 119 );
//DCS拒绝添加用户，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_ADD_DCC_USER_NACK,			EV_DCSDCC_BGN +	120 );
//DCS添加用户通知，DCS->DCC，消息体为TDCUser数组
OSPEVENT( DCS_DCC_ADD_DCC_USER_NOTIF,			EV_DCSDCC_BGN +	121 );

//DCConsole请求DCServer删除用户，DCC->DCS，消息体为TDCUser
OSPEVENT( DCC_DCS_DEL_DCC_USER_REQ,			    EV_DCSDCC_BGN + 125 );
//DCS删除用户完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DEL_DCC_USER_ACK,			    EV_DCSDCC_BGN + 126 );
//DCS拒绝删除用户，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_DEL_DCC_USER_NACK,			EV_DCSDCC_BGN + 127 );
//DCS删除用户通知，DCS->DCC，为消息体TDCUser
OSPEVENT( DCS_DCC_DEL_DCC_USER_NOTIF,			EV_DCSDCC_BGN + 128 );

//DCConsole请求DCServer修改用户，DCC->DCS，消息体为TDCUser
OSPEVENT( DCC_DCS_MODIFY_DCC_USER_REQ,			EV_DCSDCC_BGN + 132 );
//DCS修改用户完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_MODIFY_DCC_USER_ACK,			EV_DCSDCC_BGN + 133 );
//DCS拒绝修改用户，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_MODIFY_DCC_USER_NACK,			EV_DCSDCC_BGN +	134 );
//DCS修改用户通知，DCS->DCC，消息体为TDCUser
OSPEVENT( DCS_DCC_MODIFY_DCC_USER_NOTIF,		EV_DCSDCC_BGN +	135 );

//DCConsole请求DCServer删除终端，DCC->DCS，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCC_DCS_REMOVE_MT_REQ,					EV_DCSDCC_BGN +	139 );
//DCS发送删除终端完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_REMOVE_MT_ACK,					EV_DCSDCC_BGN + 140 );
//DCS拒绝删除终端，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_REMOVE_MT_NACK,					EV_DCSDCC_BGN + 141 );
//DCS终端删除通知，DCS->DCC，消息头会议号有效，消息体为TDCMt
OSPEVENT( DCS_DCC_REMOVE_MT_NOTIF,					EV_DCSDCC_BGN + 142 );

//DCConsole请求DCServer传送指定DCServer上的会议列表，DCC->DCS，消息体为TDCSConfInfo
OSPEVENT( DCC_DCS_GET_REMOTE_CONFLIST_REQ,			EV_DCSDCC_BGN +	146 );
//DCS发送指定DCServer上的会议列表完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_REMOTE_CONFLIST_ACK,			EV_DCSDCC_BGN + 147 );
//DCS拒绝发送指定DCServer上的会议列表，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_GET_REMOTE_CONFLIST_NACK,			EV_DCSDCC_BGN + 148 );
//DCS发送指定DCServer上的会议列表通知，DCS->DCC，消息体为TDCSConfInfo
OSPEVENT( DCS_DCC_GET_REMOTE_CONFLIST_NOTIF,		EV_DCSDCC_BGN + 149 );

//DCConsole请求DCServer加入指定DCServer上的指定会议，DCC->DCS，消息体为TDCSConfInfo
OSPEVENT( DCC_DCS_JOIN_REMOTE_CONF_REQ,				EV_DCSDCC_BGN +	153 );
//DCS发送加入指定DCServer上的指定会议完成，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_JOIN_REMOTE_CONF_ACK,				EV_DCSDCC_BGN + 154 );
//DCS拒绝加入指定DCServer上的指定会议，DCS->DCC，无消息体
OSPEVENT( DCS_DCC_JOIN_REMOTE_CONF_NACK,			EV_DCSDCC_BGN + 155 );
//DCS加入指定DCServer上的指定会议通知，DCS->DCC，消息头会议号有效，消息体为TDataConfFullInfo
OSPEVENT( DCS_DCC_JOIN_REMOTE_CONF_NOTIF,			EV_DCSDCC_BGN + 156 );

//DCConsole请求与DCServerc重启，无消息体
OSPEVENT( DCC_DCS_RESTART_SERVER_REQ,           EV_DCSDCC_BGN + 157);	
//DCServerc重启成功，无消息体
OSPEVENT( DCS_DCC_RESTART_SERVER_ACK,           EV_DCSDCC_BGN + 158);	
//DCServerc拒绝重启，无消息体
OSPEVENT( DCS_DCC_RESTART_SERVER_NACK,          EV_DCSDCC_BGN + 159);

//对端能力通知   消息体TParamPeerCap
OSPEVENT( GK_DCS_PEER_CAP_NOTIFY,				    EV_DCSDCC_BGN +	160 );
//对端RTCP/RTP地址通知  消息体TParamPeerAddr
OSPEVENT( GK_DCS_PEER_DATAADDR_NOTIF,				EV_DCSDCC_BGN + 161 );
//创建新呼叫   消息体TCallHandle
OSPEVENT( GK_DCS_NEWCALL_CREATE_NOTIF,				EV_DCSDCC_BGN + 162 );
//新呼叫到来   消息体TParamNewCall
OSPEVENT( GK_DCS_NEWCALL_COMMING_NOTIF,				EV_DCSDCC_BGN + 163 );
//呼叫连接通知   消息体TCallHandle
OSPEVENT( GK_DCS_CALL_CONNECT_NOTIF,				EV_DCSDCC_BGN + 164 );
//呼叫断开通知   消息体TCallHandle
OSPEVENT( GK_DCS_CALL_DISCONNECT_NOTIF,				EV_DCSDCC_BGN + 165 );
//新的信道建立   消息体TCallHandle
OSPEVENT( GK_DCS_NEWCHAN_CREATE_NOTIF,				EV_DCSDCC_BGN + 166 );
//协议栈初始化结果
OSPEVENT( GK_DCS_INIT_STACK_RESULT,                 EV_DCSDCC_BGN + 167 );
//向GK注册结果
OSPEVENT( GK_DCS_REGISTER_RESULT,                   EV_DCSDCC_BGN + 168 );
//重新项GK注册请求
OSPEVENT( GK_DCS_REGISTER_REQUEST,                  EV_DCSDCC_BGN + 169 );


//重新项GK注册请求
OSPEVENT( DCS_WATCHDOG_CONNECT_REQ,                  EV_DCSDCC_BGN + 170 );


//////////////////////////////////////////////////////////////////////////
//Error ID
//成功
const u16 ERROR_DCS_SUCCESS                  = ERR_DCS_BGN + 0;
//未知错误
const u16 ERROR_DCS_UNKNOWN					 = ERR_DCS_BGN + 1;
//请求操作对象正在处理中
const u16 ERROR_DCS_BUSY                     = ERR_DCS_BGN + 2;
//用户部分
//用户不存在
const u16 ERROR_DCS_DCUSER_NOT_EXIST         = ERR_DCS_BGN + 10;
//用户密码错误
const u16 ERROR_DCS_DCUSER_PASSWORD_ERROR    = ERR_DCS_BGN + 11;
//用户无权进行此项操作
const u16 ERROR_DCS_DCUSER_ERROR_LEVEL       = ERR_DCS_BGN + 12;
//用户已存在，无法添加
const u16 ERROR_DCS_DCUSER_EXISTED           = ERR_DCS_BGN + 13;
//用户在线
const u16 ERROR_DCS_DCUSER_ONLINE            = ERR_DCS_BGN + 14;
//用户数已满
const u16 ERROR_DCS_DCUSER_FULL              = ERR_DCS_BGN + 15;
//不能删除自己的帐户
const u16 ERROR_DCS_NO_DEL_SELF              = ERR_DCS_BGN + 16;

//终端信息部分
//MtEntry已存在
const u16 ERROR_DCS_MTENTRY_EXISTED          = ERR_DCS_BGN + 20;
//MtEntry不存在
const u16 ERROR_DCS_MTENTRY_NOT_EXIST        = ERR_DCS_BGN + 21;
//MtEntry已满
const u16 ERROR_DCS_MTENTRY_FULL             = ERR_DCS_BGN + 22;

//会议模板部分
//ConfEntry存在
const u16 ERROR_DCS_CONFENTRY_EXISTED        = ERR_DCS_BGN + 40;
//ConfEntry不存在
const u16 ERROR_DCS_CONFENTRY_NOT_EXIST      = ERR_DCS_BGN + 41;
//ConfEntry已满
const u16 ERROR_DCS_CONFENTRY_FULL           = ERR_DCS_BGN + 42;

//会议部分
//会议不存在
const u16 ERROR_DCS_CONF_NOT_EXIST			    = ERR_DCS_BGN + 60;
//终端不存在
const u16 ERROR_DCS_MT_NOT_EXIST				= ERR_DCS_BGN + 61;
//终端不在线
const u16 ERROR_DCS_MT_NOT_ONLINE			    = ERR_DCS_BGN + 62;
//会议不支持主席
const u16 ERROR_DCS_CONF_NOT_SUPPORT_CHAIRMAN   = ERR_DCS_BGN + 63;
//会议终端已满
const u16 ERROR_DCS_CONF_MT_FULL                = ERR_DCS_BGN + 64;
//终端在线
const u16 ERROR_DCS_MT_ONLINE				    = ERR_DCS_BGN + 65;
//会议没有主席
const u16 ERROR_DCS_CONF_NO_CHAIRMAN			= ERR_DCS_BGN + 66;
//会议已存在
const u16 ERROR_DCS_CONF_EXISTED			    = ERR_DCS_BGN + 67;
//会议数已满
const u16 ERROR_DCS_CONF_FULL			        = ERR_DCS_BGN + 68;
//拒绝终端加入会议的请求
const u16 ERROR_DCS_USER_REJECT                 = ERR_DCS_BGN + 69;
//拒绝发送会议列表
const u16 ERROR_DCS_REJECT_CONFLIST             = ERR_DCS_BGN + 70;
//拒绝DCS加入远端会议
const u16 ERROR_DCS_REJECT_CONF                 = ERR_DCS_BGN + 71;
//会议密码错误
const u16 ERROR_DCS_WRONG_PASSWD                = ERR_DCS_BGN + 72;
//终端信息不满足呼叫条件
const u16 ERROR_INFO_LACK                       = ERR_DCS_BGN + 73;
//服务器没有注册到GK
const u16 ERROR_DCS_NO_REGISTER_GK              = ERR_DCS_BGN + 74;
//远端会议被锁定，无法加入
const u16 ERROR_DCS_REMOTECONF_LOCK             = ERR_DCS_BGN + 75;

//数据库部分
const u16 ERROR_DCS_DB_ERROR                    = ERR_DCS_BGN + 80;

#endif // _DCCCOMMON_H
