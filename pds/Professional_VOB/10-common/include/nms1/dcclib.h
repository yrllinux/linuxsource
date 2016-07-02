/*****************************************************************************
   模块名      : DCCLib
   文件名      : DCCLib.h
   相关文件    : DCCLib.h
                 DCCLib.cpp
   文件实现功能: DCCLib提供的接口函数
   作者		   : 罗健锋
   版本        : V4.0
   -----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   15/08/05    1.0      罗健锋      创建
******************************************************************************/

#if !defined(AFX_DCCLIB_H__7526EA9E_31B0_4DE8_9162_F44147157E9E__INCLUDED_)
#define AFX_DCCLIB_H__7526EA9E_31B0_4DE8_9162_F44147157E9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dcstrc.h"
#include "kdvsys.h"

//配置文件key
#define SECTION_USER_TABLE      _T("DCC_UserTable")
#define KEY_TABLE_AMOUNT        _T("TableAmount")
#define KEY_LOGON_LOG           _T("Logon")
#define KEY_LATEST_USER         _T("LatestUser")

/////////////////////////////////////////////////////////////////////////////////////////
//Window消息接口
//高位(WPARAM)参数均为DCServer IP地址
#define		DCC_WM_USER_MSG_BEG					WM_USER + 1
//DCClib通知UI界面模块已得到服务器列表配置信息(WPARAM)NULL+(LPARAM)(vector<TDCServerInfo>*)
#define		DCC_DISP_SERVERINFO_NOTIF			DCC_WM_USER_MSG_BEG

//DCClib通知UI界面模块与指定服务器建链成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_CONNECT_ACK				DCC_WM_USER_MSG_BEG + 4
//DCClib通知UI界面模块与指定服务器建链失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_CONNECT_NACK				DCC_WM_USER_MSG_BEG + 5
//DCClib通知UI界面模块与指定服务器建链超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_CONNECT_TIMEOUT			DCC_WM_USER_MSG_BEG + 6

//DCClib通知UI界面模块与指定服务器断链成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DISCONNECT_ACK				DCC_WM_USER_MSG_BEG + 10
//DCClib通知UI界面模块与指定服务器断链失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_DISCONNECT_NACK			DCC_WM_USER_MSG_BEG + 11
//DCClib通知UI界面模块与指定服务器断链超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DISCONNECT_TIMEOUT			DCC_WM_USER_MSG_BEG + 12

//DCClib通知UI界面模块刷新地址薄成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_ADDRBOOK_ACK			DCC_WM_USER_MSG_BEG + 16
//DCClib通知UI界面模块刷新地址薄失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_GET_ADDRBOOK_NACK			DCC_WM_USER_MSG_BEG + 17
//DCClib通知UI界面模块刷新地址薄超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_ADDRBOOK_TIMEOUT		DCC_WM_USER_MSG_BEG + 18

//DCClib通知UI界面模块增加地址薄终端条目成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_ADD_MTENTRY_ACK			DCC_WM_USER_MSG_BEG + 22
//DCClib通知UI界面模块增加地址薄终端条目失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_ADD_MTENTRY_NACK			DCC_WM_USER_MSG_BEG + 23
//DCClib通知UI界面模块增加地址薄终端条目超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_ADD_MTENTRY_TIMEOUT		DCC_WM_USER_MSG_BEG + 24
//DCClib通知UI界面模块地址薄条目增加(WPARAM)DWORD+(LPARAM)( const vector<TDCBaseMt> * )
#define		DCC_DISP_ADD_MTENTRY_NOTIF			DCC_WM_USER_MSG_BEG + 25

//DCClib通知UI界面模块删除地址薄终端条目成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DEL_MTENTRY_ACK			DCC_WM_USER_MSG_BEG + 29
//DCClib通知UI界面模块删除地址薄终端条目失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_DEL_MTENTRY_NACK			DCC_WM_USER_MSG_BEG + 30
//DCClib通知UI界面模块删除地址薄终端条目超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DEL_MTENTRY_TIMEOUT		DCC_WM_USER_MSG_BEG + 31
//DCClib通知UI界面模块地址薄条目减少(WPARAM)DWORD+(LPARAM)(const TDCBaseMt*)
#define		DCC_DISP_DEL_MTENTRY_NOTIF			DCC_WM_USER_MSG_BEG + 32

//DCClib通知UI界面模块修改地址薄终端条目成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_MODIFY_MTENTRY_ACK			DCC_WM_USER_MSG_BEG + 36
//DCClib通知UI界面模块修改地址薄终端条目失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_MODIFY_MTENTRY_NACK		DCC_WM_USER_MSG_BEG + 37
//DCClib通知UI界面模块修改地址薄终端条目超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_MODIFY_MTENTRY_TIMEOUT		DCC_WM_USER_MSG_BEG + 38
//DCClib通知UI界面模块地址薄条目修改(WPARAM)DWORD+(LPARAM)(const TDCBaseMt*)
#define		DCC_DISP_MODIFY_MTENTRY_NOTIF		DCC_WM_USER_MSG_BEG + 39

//DCClib通知UI界面模块增加地址薄会议组成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_ADD_CONFENTRY_ACK			DCC_WM_USER_MSG_BEG + 43
//DCClib通知UI界面模块增加地址薄会议组失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_ADD_CONFENTRY_NACK			DCC_WM_USER_MSG_BEG + 44
//DCClib通知UI界面模块增加地址薄会议组超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_ADD_CONFENTRY_TIMEOUT		DCC_WM_USER_MSG_BEG + 45
//DCClib通知UI界面模块地址薄会议组增加(WPARAM)DWORD+(LPARAM)(const TDataConfTemplate*)
#define		DCC_DISP_ADD_CONFENTRY_NOTIF		DCC_WM_USER_MSG_BEG + 46

//DCClib通知UI界面模块删除地址薄会议组成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DEL_CONFENTRY_ACK			DCC_WM_USER_MSG_BEG + 50
//DCClib通知UI界面模块删除地址薄会议组失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_DEL_CONFENTRY_NACK			DCC_WM_USER_MSG_BEG + 51
//DCClib通知UI界面模块删除地址薄会议组超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DEL_CONFENTRY_TIMEOUT		DCC_WM_USER_MSG_BEG + 52
//DCClib通知UI界面模块地址薄会议组减少(WPARAM)DWORD+(LPARAM)(const CConfId*)
#define		DCC_DISP_DEL_CONFENTRY_NOTIF		DCC_WM_USER_MSG_BEG + 53

//DCClib通知UI界面模块修改地址薄会议组成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_MODIFY_CONFENTRY_ACK		DCC_WM_USER_MSG_BEG + 57
//DCClib通知UI界面模块修改地址薄会议组失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_MODIFY_CONFENTRY_NACK		DCC_WM_USER_MSG_BEG + 58
//DCClib通知UI界面模块修改地址薄会议组超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_MODIFY_CONFENTRY_TIMEOUT	DCC_WM_USER_MSG_BEG + 59
//DCClib通知UI界面模块地址薄会议组修改(WPARAM)DWORD+(LPARAM)(const TDataConfTemplate*)
#define		DCC_DISP_MODIFY_CONFENTRY_NOTIF		DCC_WM_USER_MSG_BEG + 60

//DCClib通知UI界面模块刷新会议列表成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_CONFLIST_ACK			DCC_WM_USER_MSG_BEG + 64
//DCClib通知UI界面模块刷新会议列表失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_GET_CONFLIST_NACK			DCC_WM_USER_MSG_BEG + 65
//DCClib通知UI界面模块刷新会议列表超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_CONFLIST_TIMEOUT		DCC_WM_USER_MSG_BEG + 66

//DCClib通知UI界面模块创建会议成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_CREATE_CONF_ACK			DCC_WM_USER_MSG_BEG + 70
//DCClib通知UI界面模块创建会议失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_CREATE_CONF_NACK			DCC_WM_USER_MSG_BEG + 71
//DCClib通知UI界面模块创建会议超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_CREATE_CONF_TIMEOUT		DCC_WM_USER_MSG_BEG + 72
//DCClib通知UI界面模块会议状态改变(WPARAM)DWORD+(LPARAM)(const TDataConfFullInfo*)
#define		DCC_DISP_CONF_NOTIF					DCC_WM_USER_MSG_BEG + 73

//DCClib通知UI界面模块结束会议成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_TERM_CONF_ACK			DCC_WM_USER_MSG_BEG + 77
//DCClib通知UI界面模块结束会议失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_TERM_CONF_NACK			DCC_WM_USER_MSG_BEG + 78
//DCClib通知UI界面模块结束会议超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_TERM_CONF_TIMEOUT		DCC_WM_USER_MSG_BEG + 79
//DCClib通知UI界面模块会议结束(WPARAM)DWORD+(LPARAM)(const CConfId*)
#define		DCC_DISP_TERM_CONF_NOTIF			DCC_WM_USER_MSG_BEG + 80

//DCClib通知UI界面模块指定主席成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_SPEC_CHAIRMAN_ACK			DCC_WM_USER_MSG_BEG + 84
//DCClib通知UI界面模块指定主席失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_SPEC_CHAIRMAN_NACK			DCC_WM_USER_MSG_BEG + 85
//DCClib通知UI界面模块指定主席超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_SPEC_CHAIRMAN_TIMEOUT		DCC_WM_USER_MSG_BEG + 86
//DCClib通知UI界面模块主席变化(WPARAM)DWORD+(LPARAM)( const CConfId* + const TDCMt* )
#define		DCC_DISP_SPEC_CHAIRMAN_NOTIF		DCC_WM_USER_MSG_BEG + 87

//DCClib通知UI界面模块取消主席成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_CANCEL_CHAIRMAN_ACK		DCC_WM_USER_MSG_BEG + 91
//DCClib通知UI界面模块取消主席失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_CANCEL_CHAIRMAN_NACK		DCC_WM_USER_MSG_BEG + 92
//DCClib通知UI界面模块取消主席超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_CANCEL_CHAIRMAN_TIMEOUT	DCC_WM_USER_MSG_BEG + 93
//DCClib通知UI界面模块取消主席通知(WPARAM)DWORD+(LPARAM)( const CConfId* )
#define		DCC_DISP_CANCEL_CHAIRMAN_NOTIF		DCC_WM_USER_MSG_BEG + 94

//DCClib通知UI界面模块邀请终端成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_INVITE_MT_ACK				DCC_WM_USER_MSG_BEG + 98
//DCClib通知UI界面模块邀请终端失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_INVITE_MT_NACK				DCC_WM_USER_MSG_BEG + 99
//DCClib通知UI界面模块邀请终端超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_INVITE_MT_TIMEOUT			DCC_WM_USER_MSG_BEG + 100
//DCClib通知UI界面模块终端加入会议(WPARAM)DWORD+(LPARAM)( const CConfId* + const TDCMt* )
#define		DCC_DISP_INVITE_MT_NOTIF			DCC_WM_USER_MSG_BEG + 101

//DCClib通知UI界面模块退出终端成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_EJECT_MT_ACK				DCC_WM_USER_MSG_BEG + 105
//DCClib通知UI界面模块退出终端失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_EJECT_MT_NACK				DCC_WM_USER_MSG_BEG + 106
//DCClib通知UI界面模块退出终端超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_EJECT_MT_TIMEOUT			DCC_WM_USER_MSG_BEG + 107
//DCClib通知UI界面模块终端退出会议(WPARAM)DWORD+(LPARAM)( const CConfId* + const TDCMt* )
#define		DCC_DISP_EJECT_MT_NOTIF				DCC_WM_USER_MSG_BEG + 108

//DCClib通知UI界面模块终端状态变化(WPARAM)DWORD+(LPARAM)( const CConfId* + const TDCMt* )
#define		DCC_DISP_MTSTATUS_NOTIF				DCC_WM_USER_MSG_BEG + 112

//DCClib通知UI界面模块刷新用户列表成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_USERLIST_ACK			DCC_WM_USER_MSG_BEG + 116
//DCClib通知UI界面模块刷新用户列表失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_GET_USERLIST_NACK			DCC_WM_USER_MSG_BEG + 117
//DCClib通知UI界面模块刷新用户列表超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_USERLIST_TIMEOUT		DCC_WM_USER_MSG_BEG + 118

//DCClib通知UI界面模块增加用户成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_ADD_USER_ACK				DCC_WM_USER_MSG_BEG + 122
//DCClib通知UI界面模块增加用户失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_ADD_USER_NACK				DCC_WM_USER_MSG_BEG + 123
//DCClib通知UI界面模块增加用户超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_ADD_USER_TIMEOUT			DCC_WM_USER_MSG_BEG + 124
//DCClib通知UI界面模块增加用户(WPARAM)DWORD+(LPARAM)( const vector<TDCUser>* )
#define		DCC_DISP_ADD_USER_NOTIF				DCC_WM_USER_MSG_BEG + 125

//DCClib通知UI界面模块删除用户成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DEL_USER_ACK				DCC_WM_USER_MSG_BEG + 129
//DCClib通知UI界面模块删除用户失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_DEL_USER_NACK				DCC_WM_USER_MSG_BEG + 130
//DCClib通知UI界面模块删除用户超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DEL_USER_TIMEOUT			DCC_WM_USER_MSG_BEG + 131
//DCClib通知UI界面模块删除用户(WPARAM)DWORD+(LPARAM)( const TDCUser* )
#define		DCC_DISP_DEL_USER_NOTIF				DCC_WM_USER_MSG_BEG + 132

//DCClib通知UI界面模块修改用户成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_MODIFY_USER_ACK			DCC_WM_USER_MSG_BEG + 136
//DCClib通知UI界面模块修改用户失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_MODIFY_USER_NACK			DCC_WM_USER_MSG_BEG + 137
//DCClib通知UI界面模块修改用户超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_MODIFY_USER_TIMEOUT		DCC_WM_USER_MSG_BEG + 138
//DCClib通知UI界面模块修改用户(WPARAM)DWORD+(LPARAM)( const TDCUser* )
#define		DCC_DISP_MODIFY_USER_NOTIF			DCC_WM_USER_MSG_BEG + 139

//DCClib通知UI界面模块与服务器链路断(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DISCONNECT					DCC_WM_USER_MSG_BEG + 143
//DCClib通知UI界面模块服务器同时连接控制台已达上限(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_DCS_OVERFLOW				DCC_WM_USER_MSG_BEG + 144
//DCClib通知UI界面模块控制台同时连接服务器已达上限(WPARAM)NULL+(LPARAM)NULL
#define		DCC_DISP_DCC_OVERFLOW				DCC_WM_USER_MSG_BEG + 145

//DCClib通知UI界面模块删除终端成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_REMOVE_MT_ACK				DCC_WM_USER_MSG_BEG + 149
//DCClib通知UI界面模块删除终端失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_REMOVE_MT_NACK				DCC_WM_USER_MSG_BEG + 150
//DCClib通知UI界面模块删除终端超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_REMOVE_MT_TIMEOUT			DCC_WM_USER_MSG_BEG + 151
//DCClib通知UI界面模块会议删除终端(WPARAM)DWORD+(LPARAM)( const CConfId* + const TDCMt* )
#define		DCC_DISP_REMOVE_MT_NOTIF			DCC_WM_USER_MSG_BEG + 152

//DCClib通知UI界面模块请求远端DCServer会议列表成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_REMOTE_CONFLIST_ACK	DCC_WM_USER_MSG_BEG + 156
//DCClib通知UI界面模块请求远端DCServer会议列表失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_GET_REMOTE_CONFLIST_NACK	DCC_WM_USER_MSG_BEG + 157
//DCClib通知UI界面模块请求远端DCServer会议列表超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_GET_REMOTE_CONFLIST_TIMEOUT	DCC_WM_USER_MSG_BEG + 158
//DCClib通知UI界面模块远端DCServer会议通知(WPARAM)DWORD+(LPARAM)( const TDCSConfInfo* )
#define		DCC_DISP_GET_REMOTE_CONFLIST_NOTIF  	DCC_WM_USER_MSG_BEG + 159

//DCClib通知UI界面模块请求加入远端DCServer会议成功(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_JOIN_REMOTE_CONF_ACK   	DCC_WM_USER_MSG_BEG + 163
//DCClib通知UI界面模块请求加入远端DCServer会议失败(WPARAM)DWORD+(LPARAM)WORD
#define		DCC_DISP_JOIN_REMOTE_CONF_NACK	    DCC_WM_USER_MSG_BEG + 164
//DCClib通知UI界面模块请求加入远端DCServer会议超时(WPARAM)DWORD+(LPARAM)NULL
#define		DCC_DISP_JOIN_REMOTE_CONF_TIMEOUT	DCC_WM_USER_MSG_BEG + 165
//DCClib通知UI界面模块加入远端DCServer会议通知(WPARAM)DWORD+(LPARAM)( const TDataConfFullInfo* )
#define		DCC_DISP_JOIN_REMOTE_CONF_NOTIF	    DCC_WM_USER_MSG_BEG + 166

//DCClib通知UI界面模块请求远端DCServer重启成功  (WPARAM)NULL+(LPARAM)NULL
#define		DCC_DISP_RESTART_SERVER_ACK   	    DCC_WM_USER_MSG_BEG + 170
//DCClib通知UI界面模块请求远端DCServer重启失败  (WPARAM)NULL+(LPARAM)NULL
#define		DCC_DISP_RESTART_SERVER_NACK	    DCC_WM_USER_MSG_BEG + 171
//DCClib通知UI界面模块请求远端DCServer重启超时  (WPARAM)NULL+(LPARAM)NULL
#define		DCC_DISP_RESTART_SERVER_TIMEOUT  	DCC_WM_USER_MSG_BEG + 172



//DCCLib内部自定义WINDOWS消息结尾
#define		DCC_WM_USER_MSG_END					DCC_WM_USER_MSG_BEG + 200

//日志文件名(相对路径)
#define DCC_LOG										(LPSTR)"dcc.txt"

//最大同时可控制服务器
#define MAXIMUM_CONTROL_DCSERVER					(int)10
//接口函数返回
#define DCC_RETURN_TRUE								(WORD)0 //操作成功
//消息发送或接口条件不满足时的错误号
#define OSP_ALWAYS_INIT								ERR_DCC_BGN	//已经初始化
#define NO_WINDOW_IS_SUCH_HWND						ERR_DCC_BGN + 1	//窗口句柄不代表一个有效窗口
#define OSP_NOT_INIT								ERR_DCC_BGN + 2 //没有初始化
#define FAIL_TO_CONNECT_DCS							ERR_DCC_BGN + 3 //主动与DCS建链失败
#define FAIL_TO_SEND_MSG_TO_DCS						ERR_DCC_BGN + 4 //链路忙，发送消息给DCS失败
#define OPERATE_MUST_IN_IDLE_STATE					ERR_DCC_BGN + 5 //只有未成功与DCS建链才能执行该操作
#define OPERATE_MUST_IN_NORMAL_STATE				ERR_DCC_BGN + 6 //只有当成功与DCS建链才能执行该操作
#define OPERATE_MUST_WAIT							ERR_DCC_BGN + 7 //DCS忙，正在处理前一个请求
#define CONFIGURE_FILE_INVALID						ERR_DCC_BGN + 8 //配置文件格式错误
#define FAIL_TO_SEARSH_DCSERVER						ERR_DCC_BGN + 9 //查询服务器失败

//DCC_ASSERT在日志文件中先打印出错语句信息
#define DCC_ASSERT(exp)    \
{\
    if ( !( exp ) ) {\
	::OspLog( 0, "ASSERT FAILED:\nFile:%s-Line:%d-%s\n", __FILE__, __LINE__, #exp );\
    }\
}

/*====================================================================
功能        ：  DCCLib初始化
输入参数说明：  dwDCCIpAddr－－Ip
                wDCCLibListenPort－－监听端口
                wDCCLibTelnetPort－－Telnet端口
                hwndBackup－－UI界面模块接收上行消息的窗口句柄
返回值说明  ：  无
====================================================================*/
WORD DCCLibInit( DWORD dwDCCIpAddr , WORD wDCCLibListenPort , WORD wDCCLibTelnetPort , HWND hwndBackup );

/*====================================================================
功能        ：  卸载功能库
输入参数说明：  
返回值说明  ：  无
====================================================================*/
WORD DCCLibExit( );

/*====================================================================
功能        ：  请求与服务器建链
输入参数说明：  dwDCSIpAddr－－DCS Ip
                wDCSListenPort－－DCS监听端口
                tUser－－用户信息
返回值说明  ：  无
====================================================================*/
WORD ConnectToServer( DWORD dwDCSIpAddr , WORD wDCSListenPort , const TDCUser& tUser );

/*====================================================================
功能        ：  断开与服务器的连接
输入参数说明：  dwServerpAddr－－服务器Ip
返回值说明  ：  无
====================================================================*/
WORD DisConnectFromServer( DWORD dwServerIp );

/*====================================================================
功能        ：  重启DCServer服务器
输入参数说明：  dwServerpAddr－－服务器Ip
返回值说明  ：  无
====================================================================*/
WORD ReStartDcServer( DWORD dwServerIp );

/*====================================================================
功能        ：  获取用户列表
输入参数说明：  dwServerIp －－服务器Ip
返回值说明  ：  无
====================================================================*/
WORD	GetUserList( DWORD dwServerIp );

/*====================================================================
功能        ：  增加用户
输入参数说明：  dwServerIp －－服务器Ip
                tUser－－用户信息
返回值说明  ：  无
====================================================================*/
WORD AddUser( DWORD dwServerIp , const TDCUser& tUser );

/*====================================================================
功能        ：  删除用户
输入参数说明：  dwServerIp －－服务器Ip
                tUser－－用户信息
返回值说明  ：  无
====================================================================*/
WORD DelUser( DWORD dwServerIp , const TDCUser& tUser );

/*====================================================================
功能        ：  修改用户
输入参数说明：  dwServerIp －－服务器Ip
                tUser－－用户信息
返回值说明  ：  无
====================================================================*/
WORD ModifyUser( DWORD dwServerIp , const TDCUser& tUser );

/*====================================================================
功能        ：  刷新地址薄信息
输入参数说明：  dwServerIp －－服务器Ip
返回值说明  ：  无
====================================================================*/
WORD GetAddrBook( DWORD dwServerIp );

/*====================================================================
功能        ：  增加终端条目
输入参数说明：  dwServerIp －－服务器Ip
                tMt－－终端信息
返回值说明  ：  无
====================================================================*/
WORD AddMtEntry( DWORD dwServerIp , const TDCBaseMt& tMt );

/*====================================================================
功能        ：  删除终端条目
输入参数说明：  dwServerIp －－服务器Ip
                tMt－－终端信息
返回值说明  ：  无
====================================================================*/
WORD DelMtEntry( DWORD dwServerIp , const TDCBaseMt& tMt );

/*====================================================================
功能        ：  修改终端条目
输入参数说明：  dwServerIp －－服务器Ip
                tMt－－终端信息
返回值说明  ：  无
====================================================================*/
WORD ModifyMtEntry( DWORD dwServerIp , const TDCBaseMt& tMt );

/*====================================================================
功能        ：  增加会议模板
输入参数说明：  dwServerIp －－服务器Ip
                tTemplate－－模板信息
返回值说明  ：  无
====================================================================*/
WORD AddConfEntry( DWORD dwServerIp , const TDataConfTemplate& tTemplate );

/*====================================================================
功能        ：  删除会议模板
输入参数说明：  dwServerIp －－服务器Ip
                cConfId－－模板Id
返回值说明  ：  无
====================================================================*/
WORD DelConfEntry( DWORD dwServerIp , const CConfId& cConfId );

/*====================================================================
功能        ：  修改会议模板
输入参数说明：  dwServerIp －－服务器Ip
                tTemplate－－模板信息
返回值说明  ：  无
====================================================================*/
WORD ModifyConfEntry( DWORD dwServerIp , const TDataConfTemplate& tTemplate );

/*====================================================================
功能        ：  获取会议列表
输入参数说明：  dwServerIp －－服务器Ip
返回值说明  ：  无
====================================================================*/
WORD GetConfList( DWORD dwServerIp );

/*====================================================================
功能        ：  创建会议
输入参数说明：  dwServerIp －－服务器Ip
                tInfo－－会议信息
返回值说明  ：  无
====================================================================*/
WORD CreateConf( DWORD dwServerIp , const TDataConfFullInfo& tInfo );

/*====================================================================
功能        ：  结束会议
输入参数说明：  dwServerIp －－服务器Ip
                cConfId－－会议号
返回值说明  ：  无
====================================================================*/
WORD TermConf( DWORD dwServerIp , const CConfId& cConfId );

/*====================================================================
功能        ：  指定主席
输入参数说明：  dwServerIp －－服务器Ip
                cConfId－－会议号
                tDcMt－－终端信息            
返回值说明  ：  无
====================================================================*/
WORD SpecChairman( DWORD dwServerIp , const CConfId& cConfId , const TDCMt& tDcMt );

/*====================================================================
功能        ：  取消主席
输入参数说明：  dwServerIp －－服务器Ip
                cConfId－－会议号
返回值说明  ：  无
====================================================================*/
WORD CancelChairman( DWORD dwServerIp , const CConfId& cConfId );

/*====================================================================
功能        ：  邀请终端
输入参数说明：  dwServerIp －－服务器Ip
                cConfId－－会议号
                tDcMt－－终端信息
返回值说明  ：  无
====================================================================*/
WORD InviteMt( DWORD dwServerIp , const CConfId& cConfId , const TDCMt& tDcMt );

/*====================================================================
功能        ：  挂断终端
输入参数说明：  dwServerIp －－服务器Ip
                cConfId－－会议号
                tDcMt－－终端信息
返回值说明  ：  无
====================================================================*/
WORD EjectMt( DWORD dwServerIp , const CConfId& cConfId , const TDCMt& tDcMt );

/*====================================================================
功能        ：  删除终端
输入参数说明：  dwServerIp －－服务器Ip
                cConfId－－会议号
                tDcMt－－终端信息
返回值说明  ：  无
====================================================================*/
WORD RemoveMt( DWORD dwServerIp , const CConfId& cConfId , const TDCMt& tDcMt );

/*====================================================================
功能        ：  获取远端会议列表
输入参数说明：  dwServerIp －－服务器Ip
                tDCSConfInfo－－远端服务器信息
返回值说明  ：  无
====================================================================*/
WORD GetRemoteConfList( DWORD dwServerIp , const TDCSConfInfo& tDCSConfInfo );

/*====================================================================
功能        ：  加入远端会议
输入参数说明：  dwServerIp －－服务器Ip
                tDCSConfInfo－－远端会议信息
返回值说明  ：  无
====================================================================*/
WORD ReqJoinRemoteConf( DWORD dwServerIp , const TDCSConfInfo& tDCSConfInfo );

//////////////////////////////////////////////////////////////////////////////////
/*====================================================================
功能        ：  获取错误码描述
输入参数说明：  wErrorCode －－错误码
返回值说明  ：  无
====================================================================*/
CString	DCCGetErrorDetail( WORD wErrorCode );

/*====================================================================
功能        ：  重设TRC到屏幕的级别
输入参数说明：  无
返回值说明  ：  无
====================================================================*/
API void DCCLibSetDebugTrcScreen();

/*====================================================================
功能        ：  置空TRC到屏幕的级别
输入参数说明：  无
返回值说明  ：  无
====================================================================*/
API void DCCLibSetReleaseTrcScreen();

/*====================================================================
功能        ：  重设LOG到屏幕的级别
输入参数说明：  无
返回值说明  ：  无
====================================================================*/
API void DCCLibSetDebugLogScreen();

/*====================================================================
功能        ：  置空LOG到屏幕的级别
输入参数说明：  无
返回值说明  ：  无
====================================================================*/
API void DCCLibSetReleaseLogScreen();

/*====================================================================
功能        ：  DCCCLib版本号
输入参数说明：  无
返回值说明  ：  无
====================================================================*/
API void DCCLibVersion();

/*====================================================================
功能        ：  帮助文件
输入参数说明：  无
返回值说明  ：  无
====================================================================*/
API	void dcclibhelp();

#endif // !defined(AFX_DCCLIB_H__7526EA9E_31B0_4DE8_9162_F44147157E9E__INCLUDED_)
