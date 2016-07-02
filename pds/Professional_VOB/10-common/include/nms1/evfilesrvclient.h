/*=======================================================================
模块名      : 文件升级服务器
文件名      : evfilesrvclient.h
相关文件    : 无
文件实现功能: 服务器和Client交互的消息定义
作者        : 王昊
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-------------------------------------------------------------------------
修改记录:
日      期  版本    修改人  修改内容
2006/12/29  1.0     王昊    创建
=======================================================================*/

#ifndef _EVFILESRVCLIENT_H
#define _EVFILESRVCLIENT_H

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif//WIN32

#include "osp.h"
#include "eventid.h"

/*------------------------------------------------------------------------------
  文件升级服务器和客户端业务消息(39201-39400)
------------------------------------------------------------------------------*/

//1.1	建立连接

//1.1.1	注册
    //客户端通知Server已连接，请求准入，消息体：IFCSrvLoginInfo
    OSPEVENT( CLIENT_FILESRV_LOGIN_REQ,         EV_FILESRVCLIENT_BGN + 1 );
    //Server准入应答，消息体：u32(Server服务号，该Client的后续消息请填写)
    OSPEVENT( FILESRV_CLIENT_LOGIN_ACK,         EV_FILESRVCLIENT_BGN + 2 );
    //Server拒绝应答，消息体：无；    错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_LOGIN_NACK,        EV_FILESRVCLIENT_BGN + 3 );

//1.1.2 注销
    //客户端请求Server断开连接，消息体：无
    OSPEVENT( CLIENT_FILESRV_LOGOUT_REQ,        EV_FILESRVCLIENT_BGN + 4 );
    //Server允许断开应答，消息体：无
    OSPEVENT( FILESRV_CLIENT_LOGOUT_ACK,        EV_FILESRVCLIENT_BGN + 5 );
    //Server拒绝断开应答，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_LOGOUT_NACK,       EV_FILESRVCLIENT_BGN + 6 );

//1.1.3 重启
    //客户端命令Server重启，消息体：u8(1有终端登录则不重启/0不理会是否有终端登录)
    OSPEVENT( CLIENT_FILESRV_RESET_REQ,         EV_FILESRVCLIENT_BGN + 7 );
    //Server允许重启应答，消息体：无
    OSPEVENT( FILESRV_CLIENT_RESET_ACK,         EV_FILESRVCLIENT_BGN + 8 );
    //Server拒绝重起应答，消息体：无；
    //错误码可以从CErrorString::ClientGetErrorStr获取描述
    //客户端上传版本中则无条件拒绝
    OSPEVENT( FILESRV_CLIENT_RESET_NACK,         EV_FILESRVCLIENT_BGN + 9 );

//1.2 用户

//1.2.1 获取用户列表
    //客户端请求用户列表，消息体：无
    OSPEVENT( CLIENT_FILESRV_GETUSERLIST_REQ,   EV_FILESRVCLIENT_BGN + 10 );
    //Server回应客户端用户列表请求(操作员只能看到自己)，
    //消息体：无
    OSPEVENT( FILESRV_CLIENT_GETUSERLIST_ACK,   EV_FILESRVCLIENT_BGN + 11 );
    //Server拒绝客户端用户列表请求应答，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_GETUSERLIST_NACK,  EV_FILESRVCLIENT_BGN + 12 );

    //Server通知客户端用户列表(操作员只能看到自己)，
    //消息体：u32(用户个数，网络序，最大MAX_FILESRV_USER_NUM) + CFCSrvUserInfo * n
    OSPEVENT( FILESRV_CLIENT_USERLIST_NOTIF,    EV_FILESRVCLIENT_BGN + 13 );

//1.2.2 添加用户
    //客户端请求添加用户，消息体：CUserFullInfo
    OSPEVENT( CLIENT_FILESRV_ADDUSER_REQ,       EV_FILESRVCLIENT_BGN + 15 );
    //Server同意添加用户应答(后续会发送FILESRV_CLIENT_USERLIST_NOTIF)，消息体：无
    OSPEVENT( FILESRV_CLIENT_ADDUSER_ACK,       EV_FILESRVCLIENT_BGN + 16 );
    //Server拒绝添加用户应答，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_ADDUSER_NACK,      EV_FILESRVCLIENT_BGN + 17 );

//1.2.3 修改用户
    //客户端请求修改用户，消息体：CFCSrvUserInfo
    OSPEVENT( CLIENT_FILESRV_EDITUSER_REQ,      EV_FILESRVCLIENT_BGN + 20 );
    //Server同意修改用户应答(后续会发送FILESRV_CLIENT_USERLIST_NOTIF)，消息体：无
    OSPEVENT( FILESRV_CLIENT_EDITUSER_ACK,      EV_FILESRVCLIENT_BGN + 21 );
    //Server拒绝修改用户应答，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_EDITUSER_NACK,     EV_FILESRVCLIENT_BGN + 22 );

//1.2.4 删除用户
    //客户端请求修改用户，消息体：u32(用户编号，网络序)
    OSPEVENT( CLIENT_FILESRV_DELUSER_REQ,       EV_FILESRVCLIENT_BGN + 25 );
    //Server同意删除用户应答(后续会发送FILESRV_CLIENT_USERLIST_NOTIF)，消息体：无
    OSPEVENT( FILESRV_CLIENT_DELUSER_ACK,       EV_FILESRVCLIENT_BGN + 26 );
    //Server拒绝删除用户应答，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_DELUSER_NACK,      EV_FILESRVCLIENT_BGN + 27 );


//1.3 配置管理

//1.3.1 查询所有推荐版本
    //客户端查询当前所有推荐版本请求，消息体：无
    OSPEVENT( CLIENT_FILESRV_GETALLRECOMMEND_REQ,   EV_FILESRVCLIENT_BGN + 40 );
    //Server回应所有推荐版本查询请求(后续发送FILESRV_CLIENT_ALLRECOMMEND_NOTIF)，
    //消息体：无
    OSPEVENT( FILESRV_CLIENT_GETALLRECOMMEND_ACK,   EV_FILESRVCLIENT_BGN + 41 );
    //Server拒绝所有推荐版本查询请求，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_GETALLRECOMMEND_NACK,  EV_FILESRVCLIENT_BGN + 42 );

    //Server通知所有推荐版本信息，消息体：
    //u32(版本总个数，网络序) + u32(已发送个数，网络序)
    //+ u32(当前包版本个数，网络序) + IFCSrvDevVerFullInfo * n
    OSPEVENT( FILESRV_CLIENT_ALLRECOMMEND_NOTIF,    EV_FILESRVCLIENT_BGN + 43 );

//1.3.2 查询某一设备历史版本详细信息(包含推荐版本)
    //客户端查询某一设备历史版本详细信息请求(包含推荐版本)，
    //消息体：IFCSrvDev
    OSPEVENT( CLIENT_FILESRV_GETDEVVERINFO_REQ,     EV_FILESRVCLIENT_BGN + 45 );
    //Server回应某一设备历史版本详细信息查询请求(后续发送FILESRV_CLIENT_DEVVERINFO_NOTIF)，
    //消息体：IFCSrvDev
    OSPEVENT( FILESRV_CLIENT_GETDEVVERINFO_ACK,     EV_FILESRVCLIENT_BGN + 46 );
    //Server拒绝某一设备历史版本详细信息查询请求，消息体：无；
    //错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_GETDEVVERINFO_NACK,    EV_FILESRVCLIENT_BGN + 47 );

    //Server通知某一设备历史版本详细信息(包含推荐版本)，消息体：
    //u32(版本总个数，网络序) + u32(已发送个数，网络序)
    //+ u32(当前包版本个数，网络序) + IFCSrvDevVerFullInfo * n
    OSPEVENT( FILESRV_CLIENT_DEVVERINFO_NOTIF,      EV_FILESRVCLIENT_BGN + 48 );

//1.3.3 将某一版本指定为推荐版本
    //客户端指定某一版本为该设备的推荐版本请求，消息体：u32(版本编号，网络序)
    OSPEVENT( CLIENT_FILESRV_SETRECOMMEND_REQ,  EV_FILESRVCLIENT_BGN + 50 );
    //Server回应指定某一版本为该设备的推荐版本请求，消息体：u32(版本编号，网络序)
    OSPEVENT( FILESRV_CLIENT_SETRECOMMEND_ACK,  EV_FILESRVCLIENT_BGN + 51 );
    //Server拒绝指定某一版本为该设备的推荐版本请求，消息体：无；
    //错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_SETRECOMMEND_NACK, EV_FILESRVCLIENT_BGN + 52 );

    //Server通知某一设备推荐版本变更，消息体：
    //u32(原有推荐版本设备编号，网络序，如果为-1则说明原来没有推荐版本)
    //+ IFCSrvDevVerFullInfo(新的推荐版本)
    //如果有错误码，则说明失败；m_wTotalPkt为1000表示该消息传递文件处理进度通知, 
    //此时m_wCurPkt表示进度，除以10得到百分数
    OSPEVENT( FILESRV_CLIENT_SETRECOMMEND_NOTIF,    EV_FILESRVCLIENT_BGN + 53 );

//1.3.4 删除某一版本
    //客户端删除某一版本请求，消息体：
    //u32(版本编号，网络序)
    OSPEVENT( CLIENT_FILESRV_DELDEVVER_REQ,     EV_FILESRVCLIENT_BGN + 55 );
    //Server回应客户端删除某一版本请求，消息体：
    //u32(版本编号，网络序)
    OSPEVENT( FILESRV_CLIENT_DELDEVVER_ACK,     EV_FILESRVCLIENT_BGN + 56 );
    //Server拒绝客户端删除某一版本请求，消息体：无；
    //错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_DELDEVVER_NACK,    EV_FILESRVCLIENT_BGN + 57 );

    //Server通知版本删除，消息体：u32(被删除的版本编号，网络序)
    //如果有错误码，则说明删除失败
    OSPEVENT( FILESRV_CLIENT_DELDEVVER_NOTIF,   EV_FILESRVCLIENT_BGN + 58 );

//1.3.5 修改某一版本
    //客户端修改某一版本请求，消息体：
    //u32(设备编号，网络序) + IFCHardwareVer + s8[MAX_FILESRV_BUG_REPORT_LEN]
    OSPEVENT( CLIENT_FILESRV_EDITVER_REQ,       EV_FILESRVCLIENT_BGN + 60 );
    //Server修改某一版本回应，消息体：
    //u32(设备编号，网络序) + IFCHardwareVer + s8[MAX_FILESRV_BUG_REPORT_LEN]
    OSPEVENT( FILESRV_CLIENT_EDITVER_ACK,       EV_FILESRVCLIENT_BGN + 61 );
    //Server拒绝修改某一版本，消息体：无；
    //错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_EDITVER_NACK,      EV_FILESRVCLIENT_BGN + 62 );

    //Server修改某一版本通知：消息体：
    //u32(设备编号，网络序) + IFCHardwareVer + s8[MAX_FILESRV_BUG_REPORT_LEN]
    OSPEVENT( FILESRV_CLIENT_EDITVER_NOTIF,     EV_FILESRVCLIENT_BGN + 63 );

//1.3.6 获取服务器配置
    //客户端获取服务器配置请求，消息体：无
    OSPEVENT( CLIENT_FILESRV_GETCFG_REQ,        EV_FILESRVCLIENT_BGN + 65 );
    //服务器回应客户端获取配置，消息体：ISusConfig
    OSPEVENT( FILESRV_CLIENT_GETCFG_ACK,        EV_FILESRVCLIENT_BGN + 66 );
    //服务器拒绝客户端获取配置，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_GETCFG_NACK,       EV_FILESRVCLIENT_BGN + 67 );

    //服务器配置修改通知，消息体：ISusConfig
    OSPEVENT( FILESRV_CLIENT_CFG_NOTIF,         EV_FILESRVCLIENT_BGN + 68 );

//1.3.7 修改服务器配置
    //客户端修改服务器配置请求，消息体：ISusConfig
    OSPEVENT( CLIENT_FILESRV_EDITCFG_REQ,       EV_FILESRVCLIENT_BGN + 70 );
    //服务器回应客户端修改服务器配置(后续发送FILESRV_CLIENT_CFG_NOTIF)，消息体：ISusConfig
    OSPEVENT( FILESRV_CLIENT_EDITCFG_ACK,       EV_FILESRVCLIENT_BGN + 71 );
    //服务器拒绝客户端修改配置，消息体：无，错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_EDITCFG_NACK,      EV_FILESRVCLIENT_BGN + 72 );

//1.4 日志查询

//1.4.1 查询日志
    //客户端日志查询请求，消息体：ILogReqInfo
    OSPEVENT( CLIENT_FILESRV_GETLOG_REQ,        EV_FILESRVCLIENT_BGN + 75 );
    //Server回应日志查询请求(后续发送FILESRV_CLIENT_LOG_NOTIF)，
    //消息体：u32(符合要求的日志总个数，网络序)
    OSPEVENT( FILESRV_CLIENT_GETLOG_ACK,        EV_FILESRVCLIENT_BGN + 76 );
    //Server拒绝日志查询请求，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_GETLOG_NACK,       EV_FILESRVCLIENT_BGN + 77 );

    //Server通知日志条目(仅发往REQ的Client)，消息体：
    //u32(总个数，网络序) + u32(已发送个数，网络序) + u32(当前包个数，网络序)
    //+ ILogRetInfo * n
    OSPEVENT( FILESRV_CLIENT_LOG_NOTIF,         EV_FILESRVCLIENT_BGN + 78 );

//1.4.2 删除指定日志信息
    //客户端删除指定日志条目，消息体：u32(删除总个数，网络序) + u32(每条日志的ID，网络序) * n
    OSPEVENT( CLIENT_FILESRV_DELLOG_REQ,        EV_FILESRVCLIENT_BGN + 80 );
    //服务器同意删除指定日志条目(后续发送FILESRV_CLIENT_DELLOG_NOTIF)，消息体：无
    OSPEVENT( FILESRV_CLIENT_DELLOG_ACK,        EV_FILESRVCLIENT_BGN + 81 );
    //Server拒绝删除指定日志条目，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_DELLOG_NACK,       EV_FILESRVCLIENT_BGN + 82 );

    //Server删除日志条目通知，消息体：u32(删除总个数，网络序) + u32(每条日志的ID，网络序) * n
    OSPEVENT( FILESRV_CLIENT_DELLOG_NOTIF,      EV_FILESRVCLIENT_BGN + 83 );

//1.4.3 清空日志信息
    //客户端请求清空所有日志信息，消息体：无
    OSPEVENT( CLIENT_FILESRV_CLEARLOG_REQ,      EV_FILESRVCLIENT_BGN + 85 );
    //服务器同意清空所有日志信息(后续发送FILESRV_CLIENT_CLEARLOG_NOTIF)，消息体：无
    OSPEVENT( FILESRV_CLIENT_CLEARLOG_ACK,      EV_FILESRVCLIENT_BGN + 86 );
    //Server拒绝清空所有日志信息，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_CLEARLOG_NACK,     EV_FILESRVCLIENT_BGN + 87 );

    //Server清空所有日志信息通知，消息体：无
    OSPEVENT( FILESRV_CLIENT_CLEARLOG_NOTIF,    EV_FILESRVCLIENT_BGN + 88 );


//1.5 文件上传

//1.5.1 准备上传
    //客户端告知上传文件请求，消息体：IFCSrvDevVerInfo
    OSPEVENT( CLIENT_FILESRV_UPDATEPREPARE_REQ, EV_FILESRVCLIENT_BGN + 100 );
    //Server同意上传应答，消息体：IFCSrvDevVerInfo
    OSPEVENT( FILESRV_CLIENT_UPDATEPREPARE_ACK, EV_FILESRVCLIENT_BGN + 101 );
    //Server拒绝上传应答，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_UPDATEPREPARE_NACK,EV_FILESRVCLIENT_BGN + 102 );

//1.5.2 告知文件名
    //客户端上传文件准备，消息体：s8[MAX_FILESRV_FILENAME_LEN](准备本次上传的文件名)
    OSPEVENT( CLIENT_FILESRV_SENDFILENAME_REQ,  EV_FILESRVCLIENT_BGN + 105 );
    //Server同意上传指定文件，消息体：u16(每一包的最大长度，网络序)
    OSPEVENT( FILESRV_CLIENT_SENDFILENAME_ACK,  EV_FILESRVCLIENT_BGN + 106 );
    //Server拒绝上传指定文件，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_SENDFILENAME_NACK, EV_FILESRVCLIENT_BGN + 107 );

//1.5.3 上传文件
    //客户端上传文件，消息体：u16(当前包长，网络序) + u8[](部分文件)，需要填写m_wTotalPkt和m_wCurPkt
    OSPEVENT( CLIENT_FILESRV_UPLOADFILE_REQ,    EV_FILESRVCLIENT_BGN + 110 );
    //Server接收指定文件，消息体：无，需要填写m_wCurPkt
    OSPEVENT( FILESRV_CLIENT_UPLOADFILE_ACK,    EV_FILESRVCLIENT_BGN + 111 );
    //Server拒绝接收指定文件，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_UPLOADFILE_NACK,   EV_FILESRVCLIENT_BGN + 112 );

//1.5.4 文件上传通知
    //Server接收指定文件完毕通知(仅提示REQ的客户端)，消息体：s8[MAX_FILESRV_FILENAME_LEN](文件名)
    OSPEVENT( FILESRV_CLIENT_ONEFILEEND_NOTIF,  EV_FILESRVCLIENT_BGN + 115 );
    //Server接收所有文件完毕通知(仅提示REQ的客户端)，消息体：无
    OSPEVENT( FILESRV_CLIENT_ALLFILEEND_NOTIF,  EV_FILESRVCLIENT_BGN + 116 );

//1.5.5 文件处理进度通知
    //Server通知文件处理进度(客户端上传完毕后，服务器处理进度，按照所有文件计算)，
    //消息体：u16(进度，精确到%前小数点后一位，网络序)
//    OSPEVENT( FILESRV_CLIENT_FILEPROGRESS_NOTIF, EV_FILESRVCLIENT_BGN + 120 );
    //Server通知文件处理结果(进度100%后发送)，消息体：
    //u8(文件个数)
    //+ [ s8[MAX_FILESRV_FILENAME_LEN](文件名) + u32(0为成功，否则为错误码，网络序) ]
    //  * n
    OSPEVENT( FILESRV_CLIENT_FILERESULT_NOTIF,   EV_FILESRVCLIENT_BGN + 121 );

//1.5.6 停止文件传输
    //Client通知Server停止文件传输，消息体：无
    OSPEVENT( CLIENT_FILESRV_STOPUPDATE_REQ,     EV_FILESRVCLIENT_BGN + 130 );
    //Server同意停止文件传输应答(后续不会发送任何文件处理的NOTIF)，消息体：无
    OSPEVENT( FILESRV_CLIENT_STOPUPDATE_ACK,     EV_FILESRVCLIENT_BGN + 131 );
    //Server拒绝停止文件传输应答，消息体：无；错误码可以从CErrorString::ClientGetErrorStr获取描述
    OSPEVENT( FILESRV_CLIENT_STOPUPDATE_NACK,    EV_FILESRVCLIENT_BGN + 132 );

#endif//_EVFILESRVCLIENT_H
