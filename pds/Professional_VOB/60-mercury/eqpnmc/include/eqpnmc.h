/*****************************************************************************
    模块名      : EqpNmc
    文件名      : eqpnmc.h
    相关文件    : 
    文件实现功能: 外设网管客户端
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef EQPNMC_H_
#define EQPNMC_H_
#include "eqpagtcommon.h"

// kdvlog config file
#if defined( WIN32 ) || defined(_WIN32)
#define EQPNMC_KDVLOG_FILE	    ( LPCSTR )"./conf/kdvlog_eqpnmc.ini"
#else
#define EQPNMC_KDVLOG_FILE	    ( LPCSTR )"/usr/etc/config/conf/kdvlog_eqpnmc.ini"
#endif

// EQP
#define EQP_TELNET_PORT         2500

// EQP所有应用OSP初始授权定义
#define EQP_TEL_USRNAME         ( LPCSTR )"admin"
#define EQP_TEL_PWD             ( LPCSTR )"admin"

#define AUTORUN_FLAG            ( LPCSTR )" -autorun"

#define WM_TRAY_MESSAGE         (WM_USER + 1)

API BOOL32 IsAutoRun(void);
API BOOL32 AutoRun(void);
API void CancelAutoRun(void);
API void quit(void);

#endif  // _EQPNMC_H_