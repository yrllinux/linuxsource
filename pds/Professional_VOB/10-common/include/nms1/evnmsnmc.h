/*=============================================================================
模   块   名: 网管服务器
文   件   名: evnmsnmc.h
相 关  文 件: 无
文件实现功能: NMS-NMC消息定义
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/09/07  4.0     王昊        创建
=============================================================================*/

#ifndef _EVNMSNMC_20050907_H_
#define _EVNMSNMC_20050907_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "eventid.h"

//服务器和客户端交互消息

//操作命令请求
OSPEVENT( EV_CLT_OPER_REQ,                      EV_CLTSVR_BGN + 0 );
//操作命令结果
OSPEVENT( EV_CLT_OPER_RET,                      EV_CLTSVR_BGN + 1 );

//新客户端接入请求
OSPEVENT( EV_CLT_NEWSESSION_REQ,                EV_CLTSVR_BGN + 2 );
//接入结果返回
OSPEVENT( EV_CLT_NEWSESSION_RET,                EV_CLTSVR_BGN + 3 );

//客户端退出请求
OSPEVENT( EV_CLT_QUIT_REQ,                      EV_CLTSVR_BGN + 4 );
//退出结果返回
OSPEVENT( EV_CLT_QUIT_RET,                      EV_CLTSVR_BGN + 5 );

//告警广播消息
OSPEVENT( EV_BC_FM_ALARM,                       EV_CLTSVR_BGN + 6 );

//配置改变刷新广播消息
OSPEVENT( EV_BC_CM_REFRESH,                     EV_CLTSVR_BGN + 7 );

//服务器和前台链路改变广播消息
OSPEVENT( EV_BC_CM_LINKCHANGE,                  EV_CLTSVR_BGN + 8 );

//单板状态广播消息
OSPEVENT( EV_BC_CM_BOARD_STATUS,                EV_CLTSVR_BGN + 9 );

//服务器退出
OSPEVENT( EV_SRV_QUIT,                          EV_CLTSVR_BGN + 10 );

//停止FTP
OSPEVENT( EV_CLT_STOP_FTP,                      EV_CLTSVR_BGN + 11 );

//Ftp进展
OSPEVENT( EV_CLT_FTP_PROC,                      EV_CLTSVR_BGN + 12 );

//机架构件的状态发生改变事件, 包括新增单板、删除单板等
OSPEVENT( EV_CLT_FRAMEWORK_STATUS,              EV_CLTSVR_BGN + 13 );

//板卡状态改变
OSPEVENT( EV_CLT_BOARD_STATUS,                  EV_CLTSVR_BGN + 14 );

//告警过滤更新, 客户端发往服务器, 不需要返回
OSPEVENT( EV_CLT_ALARM_FILTER,                  EV_CLTSVR_BGN + 15 );

    //设备的变更事件
    //一般来说，网管客户端在界面上新添加了一个设备或者删除了一个设备的时候，
    //要给网管服务器发送事件EV_CLT_COMMAND_DEVICE，网管服务器在接收到该
    //事件以后，给所有的网管客户端发送EV_CLT_BROADCAST_DEVICE的通知
    //对于具体的变更内容，看CM_EQUIP_ADD和CM_EQUIP_DEL
//网管客户端发送给网管服务器的设备修改通知，包括新增设备和删除设备等
OSPEVENT( EV_CLT_COMMAND_DEVICE,                EV_CLTSVR_BGN + 16 );
//从网管服务器发送给网管客户端的设备变更通知
OSPEVENT( EV_CLT_BROADCAST_DEVICE,              EV_CLTSVR_BGN + 17 );

    //用户的变更事件
    //一般来说，网管客户端在界面上新添加了一个用户或者删除了一个用户的时候，
    //要给网管服务器发送事件EV_CLT_COMMAND_USER，网管服务器在接收到该事件
    //以后，给所有的网管客户端发送EV_CLT_BROADCAST_USER的通知
    //对于具体的变更内容，看SM_USER_ADD、SM_USER_DEL等
//网管客户端发送给网管服务器的用户变更通知，包括用户的增加、删除等
OSPEVENT( EV_CLT_COMMAND_USER,                  EV_CLTSVR_BGN + 18 );
//从网管服务器发送给网管客户端的用户变更通知
OSPEVENT( EV_CLT_BROADCAST_USER,                EV_CLTSVR_BGN + 19 );

    //组的变更事件
    //一般来说，网管客户端在界面上新添加了一个组或者删除了一个组的时候，
    //要给网管服务器发送事件EV_CLT_COMMAND_GROUP，网管服务器在接收到该事件
    //以后，给所有的网管客户端发送EV_CLT_BROADCAST_GROUP的通知
    //对于具体的变更内容，看SM_GROUP_ADD、SM_GROUP_DEL等
//网管客户端发送给网管服务器的用户变更通知，包括用户的增加、删除等
OSPEVENT( EV_CLT_COMMAND_GROUP,                 EV_CLTSVR_BGN + 20 );
//从网管服务器发送给网管客户端的用户变更通知
OSPEVENT( EV_CLT_BROADCAST_GROUP,               EV_CLTSVR_BGN + 21 );

//客户端得到服务器配置信息命令
OSPEVENT( EV_CLT_GET_SVR_CFG_REQ,               EV_CLTSVR_BGN + 22 );
//服务器向客户端返回自己的配置信息
OSPEVENT( EV_CLT_GET_SVR_CFG_RET,               EV_CLTSVR_BGN + 23 );

//文件更新失败
OSPEVENT( EV_NMS_NMC_UPDATE_FAIL,               EV_CLTSVR_BGN + 25 );


//////////////////////////////////////////////////////////////////////////
//事件大类,在CNmsMsg的事件高位
#define UNKNOWN_EVENT       (u8)~0		// 未知的命令类型
#define EVENT_CTRL_BEGIN    (u8)1
#define COMMON_CM_CTRL      (u8)EVENT_CTRL_BEGIN + 0        // 1、CM普通控制
#define FTP_CTRL            (u8)EVENT_CTRL_BEGIN + 1        // 2、Ftp控制
#define COMMON_FM_CTRL      (u8)EVENT_CTRL_BEGIN + 2		// 3、FM普通控制
#define REMOTE_CTRL         (u8)EVENT_CTRL_BEGIN + 3        // 4、远程控制
#define COMMON_SM_CTRL      (u8)EVENT_CTRL_BEGIN + 4        // 5、SM普通控制
#define EVENT_CTRL_END      (u8)127

//事件子类,在CNmsMsg的事件低位
//配置控制操作命令类型
#define CM_SYNC_CONFIG          (u8)1   //配置同步
#define CM_SYNC_TIME            (u8)2   //时间同步
#define CM_QUERY_SINGLE_BOARD   (u8)3   //查询单块板卡信息
#define CM_QUERY_VERSION        (u8)4  //查询单板版本号 

//Ftp控制操作命令类型
#define FTP_PUT_FILE        (u8)1       // 上传文件
#define FTP_GET_FILE        (u8)2       // 下载文件
#define FTP_FILE_RENAME     (u8)3       // 上传所有文件后, 通知终端修改文件名

//故障控制操作命令类型
#define FM_SYNC_ALARM       (u8)1       // 告警同步

//安全控制操作命令类型
#define SM_PASSWORD_CHANGE  (u8)1       // 修改密码

//远程控制操作命令类型
#define RPC_RESTART         (u8)1   //重启
#define RPC_STOP            (u8)2   //停止
#define RPC_SELFTEST        (u8)3   //自检
#define RPC_BITERRTEST      (u8)4   //误码测试

//单板
#define BOARD_REMOTE_RESTART    (u8)10  //单板复位
#define BOARD_SYNC_TIME         (u8)11  //单板时间同步
#define BOARD_BIRERROR_TEST     (u8)12  //单板误码测试
#define BOARD_SEL_TEST          (u8)13  //单板自测
#define BOARD_E1LINK_CFG        (u8)14  //E1链路设置

//////////////////////////////////////////////////////////////////////////

// 设备广播定义设备的修改
#define CM_DEVICE_ADD       (u8)1   // 新增一个设备
#define CM_DEVICE_DEL       (u8)2   // 删除一个设备
#define CM_DEVICE_MODIFY    (u8)3   // 修改设备

// 用户广播定义用户的变更
#define SM_USER_ADD         (u8)1   // 新增一个用户
#define SM_USER_DEL         (u8)2   // 删除一个用户
#define SM_USER_MODIFY      (u8)3   // 修改用户资料
#define SM_USER_LINK        (u8)4   // 用户和组建立关联
#define SM_USER_UNLINK      (u8)5   // 撤销关联

// 组广播定义组的变更
#define SM_GROUP_ADD        (u8)1   // 新增一个组
#define SM_GROUP_DEL        (u8)2   // 删除一个组
#define SM_GROUP_MODIFY     (u8)3   // 修改组资料
#define SM_GROUP_ASSIGN     (u8)4   // 组添加成员用户
#define SM_GROUP_UNASSIGN   (u8)5   // 删除成员用户
#define SM_GROUP_PRIV_ADD   (u8)6   // 用户添加权限
#define SM_GROUP_PRIV_DEL   (u8)7   // 用户删除权限

#endif  //  _EVNMSNMC_20050907_H_