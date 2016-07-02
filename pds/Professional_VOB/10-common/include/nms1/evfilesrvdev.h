/*=======================================================================
模块名      : 文件升级服务器
文件名      : evfilesrvdev.h
相关文件    : 无
文件实现功能: 服务器和终端交互的消息定义
作者        : 王昊
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-------------------------------------------------------------------------
修改记录:
日      期  版本    修改人  修改内容
2006/12/28  1.0     王昊    创建
=======================================================================*/

#ifndef _EVFILESRVDEV_H
#define _EVFILESRVDEV_H

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif//WIN32

#include "osp.h"
#include "eventid.h"

// #define EV_FILESRVDEV_BEGIN                         (u16)39100

/*------------------------------------------------------------------------------
  文件升级服务器和设备(MCU/MT/单板)业务消息(39101-39200)
------------------------------------------------------------------------------*/

//1.1	建立连接

//1.1.1	注册
    //设备通知Server已连接，请求准入，消息体：IDevLoginInfo
    OSPEVENT( DEV_FILESRV_LOGIN_REQ,          EV_FILESRVDEV_BGN + 1 );
    //Server准入应答，消息体：u32(Server服务号，网络序，该设备的后续消息请填写)
    OSPEVENT( FILESRV_DEV_LOGIN_ACK,          EV_FILESRVDEV_BGN + 2 );
    //Server拒绝应答，消息体：无；    错误码可以从CErrorString::MtGetErrorStr获取描述
    OSPEVENT( FILESRV_DEV_LOGIN_NACK,         EV_FILESRVDEV_BGN + 3 );

    //Server从文件服务器上取版本失败通知，消息体：无；错误码可以从MtGetErrorStr获取描述
    OSPEVENT( FILESRV_DEV_GETFILEERROR_NOTIF, EV_FILESRVDEV_BGN + 4 );
    //Server准备好新版本，允许设备开始获取文件通知，消息体：IDevVerInfo
    OSPEVENT( FILESRV_DEV_FILEPREPARE_NOTIF,  EV_FILESRVDEV_BGN + 5 );

//1.1.2 获取文件准备状态
    //设备请求Server，版本是否准备好，消息体：无
    //(FILESRV_DEV_LOGIN_ACK之后，前面两条NOTIF之前可以不断查询是否准备好)
//    OSPEVENT( DEV_FILESRV_FILEPREPARE_REQ,    EV_FILESRVDEV_BGN + 10 );
    //Server应答版本准备状态，消息体：u8(准备成功：1；尚未成功：0)
//    OSPEVENT( FILESRV_DEV_FILEPREPARE_ACK,    EV_FILESRVDEV_BGN + 11 );
    //Server拒绝版本准备状态应答，消息体：无；错误码可以从CErrorString::GetFileSrvErrorStr获取描述
//    OSPEVENT( FILESRV_DEV_FILEPREPARE_NACK,   EV_FILESRVDEV_BGN + 12 );

//1.1.3 反注册
    //设备请求Server断开连接，消息体：无
    OSPEVENT( DEV_FILESRV_LOGOUT_REQ,         EV_FILESRVDEV_BGN + 20 );
    //Server同意断开应答，消息体：无
    OSPEVENT( FILESRV_DEV_LOGOUT_ACK,         EV_FILESRVDEV_BGN + 21 );
    //Server拒绝断开应答，消息体：无；错误码可以从CErrorString::MtGetErrorStr获取描述
    OSPEVENT( FILESRV_DEV_LOGOUT_NACK,        EV_FILESRVDEV_BGN + 22 );

//1.2 文件传输

//1.2.1 指定文件
    //设备请求Server传输指定文件，消息体：s8[MAX_FILESRV_FILENAME_LEN](含'\0'文件名)
    OSPEVENT( DEV_FILESRV_LOADFILE_REQ,       EV_FILESRVDEV_BGN + 30 );
    //Server文件传输准备完毕应答，消息体：u16(指定文件总包数，网络序)
    OSPEVENT( FILESRV_DEV_LOADFILE_ACK,       EV_FILESRVDEV_BGN + 31 );
    //Server拒绝文件传输准备应答，消息体：无；错误码可以从CErrorString::MtGetErrorStr获取描述
    OSPEVENT( FILESRV_DEV_LOADFILE_NACK,      EV_FILESRVDEV_BGN + 32 );

//1.2.2 传输文件
    //设备请求Server传输文件块，消息体：u16(文件SliceNo，网络序)
    OSPEVENT( DEV_FILESRV_GETFILE_REQ,        EV_FILESRVDEV_BGN + 35 );
    //Server传输文件，消息体：u16(当前传输的包SliceNo，网络序) + u16(当前包长，网络序) + u8[](部分文件)
    OSPEVENT( FILESRV_DEV_GETFILE_ACK,        EV_FILESRVDEV_BGN + 36 );
    //Server拒绝传输文件，消息体：无；错误码可以从CErrorString::MtGetErrorStr获取描述
    OSPEVENT( FILESRV_DEV_GETFILE_NACK,       EV_FILESRVDEV_BGN + 37 );


#endif//_EVFILESRVDEV_H
