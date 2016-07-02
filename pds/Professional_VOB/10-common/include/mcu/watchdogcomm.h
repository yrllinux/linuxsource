/*****************************************************************************
模块名      : Watchdog
文件名      : Watchdog.h
创建时间    : 2009年 02月 23日
实现功能    : 
作者        : 张洪彬
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/02/23  1.0         张洪彬        创建
******************************************************************************/
#ifndef _WATCHDOGCOMM_H_
#define _WATCHDOGCOMM_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "eventid.h"

#ifdef WIN32
#define WM_MY_MESSAGE      (WM_USER+100)
#define WM_MY_MESSAGE_QUIT (WM_USER+101)
#define WM_MY_ICONNOTIF    (WM_USER+102) 
#endif

//////////////////////////////// MDSC/HDSC APP ID ///////////////////////////////
#define  WD_SER_APP_ID             (AID_DSC_BGN+1)
#define  WD_CLT_APP_ID             (AID_DSC_BGN+2) 


//////////////////////////////// 宏定义 ///////////////////////////////
//服务器参数 
#define  WD_SER_IP                 inet_addr( "127.0.0.1" )
#define  WD_SER_TCP_PORT           (u16) 3500
#define  WD_SER_TELNET_PORT        (u16) 3501

//////////////////////////////// 消息定义 ///////////////////////////////

/*EV_CS_MODULECON_REQ
  请求连接
  消息内容: (TModuleParameter ，1)
*/
OSPEVENT(WD_CLI_SER_MODULECON_REQ,            (EV_WD8000E_BGN +1  ));
OSPEVENT(WD_SER_CLI_MODULECON_ACK,            (EV_WD8000E_BGN +2  ));
OSPEVENT(EV_SER_CLI_MODULECON_NACK,           (EV_WD8000E_BGN +3  ));

/*EV_CS_GETOTERSTATE_REQ
  mcu请求其他连接了那些模块
  目前仅限于mcu使用
  内容：
*/
OSPEVENT(WD_CLI_SER_GETOTERSTATE_REQ,         (EV_WD8000E_BGN +4  ));
OSPEVENT(WD_SER_CLI_GETOTERSTATE_ACK,         (EV_WD8000E_BGN +5  ));
OSPEVENT(WD_SER_CLI_GETOTERSTATE_NOTFY,       (EV_WD8000E_BGN +6  ));

/*EV_CS_SETMODULESTATE_CMD
  设置状态
  内容：(TModuleParameter ，1)
*/	
OSPEVENT(WD_CLI_SER_REPORTCLISTATE_REQ,       (EV_WD8000E_BGN +7  ));
OSPEVENT(WD_SER_CLI_REPORTCLISTATE_ACK,       (EV_WD8000E_BGN +8  ));
OSPEVENT(WD_SER_CLI_REPORTCLISTATE_NACK,      (EV_WD8000E_BGN +9  ));


OSPEVENT(WD_SER_CLI_HB_REQ,                   (EV_WD8000E_BGN +10 ));
OSPEVENT(WD_CLI_SER_HB_ACK,                   (EV_WD8000E_BGN +11 ));

OSPEVENT(EV_SER_HB_TIMER,                     (EV_WD8000E_BGN +12 ));
OSPEVENT(EV_SER_RESTART_TIMER,                (EV_WD8000E_BGN +13 ));

OSPEVENT(WD_CLI_GUARD_QUIT,                   (EV_WD8000E_BGN +14 ));
OSPEVENT(WD_CLI_SERV_APPBLOCK,                (EV_WD8000E_BGN +15 ));

//mcu要求guard重启硬件
OSPEVENT(WD_MCUCLI_GUARD_REBOOT_REQ,          (EV_WD8000E_BGN +16 ));
OSPEVENT(WD_MCUCLI_GAURD_GETKEY_REQ,          (EV_WD8000E_BGN +17 ));
OSPEVENT(WD_GAURD_MCUCLI_GETKEY_ACK,          (EV_WD8000E_BGN +18 ));
OSPEVENT(WD_MCUCLI_GUARD_UPDATE_REQ,          (EV_WD8000E_BGN +19 ));
OSPEVENT(WD_MCUCLI_GUARD_SHUTDOWN_REQ,        (EV_WD8000E_BGN +20 ));
OSPEVENT(WD_MCUCLI_GUARD_SOFTRESTART_REQ,     (EV_WD8000E_BGN +21 )); 

//客户端要求guard添加路由命令，消息体：u32(dstIp,网络序)+u32(mask,网络序)+u32(gatewayIp,网络序)+u8(metric)
OSPEVENT(WD_CLI_SER_ADDROUTE_CMD,             (EV_WD8000E_BGN +22 ));
//客户端要求guard删除路由命令，消息体：u32(dstIp,网络序)+u32(mask,网络序)+u32(gatewayIp,网络序)
OSPEVENT(WD_CLI_SER_DELROUTE_CMD,             (EV_WD8000E_BGN +23 ));

//认证请求
OSPEVENT(WD_GLOBALAUTH_CMD,				      (EV_WD8000E_BGN +90 )); 
OSPEVENT(WD_GLOBALAUTH_NOTIF,			      (EV_WD8000E_BGN +91 )); 

//认证定时器
OSPEVENT(WD_GLOBALAUTH_TIMER,			      (EV_WD8000E_BGN +96 )); 

//链接失败原因
enum emConnFailReason
{
	emUnknowModule = 0,             //未知的模块类型
	emHadConnected  = 1             //已处于链接状态

};

#endif