/*****************************************************************************
   模块名      : 用户管理
   文件名      : umev.h
   相关文件    : 
   文件实现功能: 用户管理消息定义
   作者        : zhangsh
   版本        : V0.9  Copyright(C) 2001-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/25  1.0         zhangsh        创建
******************************************************************************/
#ifndef _UMEV_H_
#define _UMEV_H_

#include "osp.h"
#include "eventid.h"
/**********************************************************
 以下为登录消息
************************************CLogin**********************/
//客户端请求登录消息,消息体为CLoginRequest
OSPEVENT(MCS_UM_LOGIN_REQ,	EV_UM_BGN);
//客户端请求登录确认应答,消息体为CUserFullInfo
OSPEVENT(UM_MCS_LOGIN_ACK,	EV_UM_BGN+1);
//客户端请求登录拒绝应答,消息体为错误号[u32]
OSPEVENT(UM_MCS_LOGIN_NACK,	EV_UM_BGN+2);
/**********************************************************
 以下为添加用户消息
**********************************************************/
//客户端添加用户请求,消息体为CUserFullInfo
OSPEVENT(MCS_UM_ADDUSER_REQ,	EV_UM_BGN+3);
//客户端添加用户确认，消息体为CUserFullInfo
OSPEVENT(UM_MCS_ADDUSER_ACK,	EV_UM_BGN+4);
//客户端添加用户拒绝，消息体为CUserFullInfo
OSPEVENT(UM_MCS_ADDUSER_NACK,	EV_UM_BGN+5);
/**********************************************************
 以下为删除用户消息
**********************************************************/
//客户端删除用户请求,消息体为CUserFullInfo
OSPEVENT(MCS_UM_DELUSER_REQ,	EV_UM_BGN+6);
//客户端删除用户确认，消息体为CUserFullInfo
OSPEVENT(UM_MCS_DELUSER_ACK,	EV_UM_BGN+7);
//客户端删除用户拒绝，消息体为CUserFullInfo
OSPEVENT(UM_MCS_DELUSER_NACK,	EV_UM_BGN+8);
/**********************************************************
 以下为更改用户信息消息
**********************************************************/
//客户端更改用户信息请求,消息体为新用户信息CUserFullInfo
OSPEVENT(MCS_UM_CHANGEPWD_REQ,	EV_UM_BGN+9);
//客户端更改用户信息应答,消息体为CUserFullInfo
OSPEVENT(UM_MCS_CHANGEPWD_ACK,	EV_UM_BGN+10);
//客户端更改用户信息拒绝,消息体为CUserFullInfo
OSPEVENT(UM_MCS_CHANGEPWD_NACK,	EV_UM_BGN+11);
/**********************************************************
 以下为得到所有用户消息
**********************************************************/
////客户端得到所有用户信息请求，消息体空
OSPEVENT(MCS_UM_GETALLUSER_REQ,	EV_UM_BGN+12);
////客户端得到所有用户信息确认，消息体空
OSPEVENT(UM_MCS_GETALLUSER_ACK,	EV_UM_BGN+13);
////客户端得到所有用户信息拒绝，消息体错误号
OSPEVENT(UM_MCS_GETALLUSER_NACK,EV_UM_BGN+14);
//客户端得到所有用户信息确认，消息体为CLongMessage+15*CUserFullInfo
OSPEVENT(UM_MCS_GETALLUSER_NOTIF,EV_UM_BGN+15);
////客户端删除所有用户请求
OSPEVENT(MCS_UM_DELUSERALL_REQ,EV_UM_BGN+16);
////客户端删除所有用户确认
OSPEVENT(MCS_UM_DELUSERALL_ACK,EV_UM_BGN+17);
////客户端删除所有用户拒绝
OSPEVENT(MCS_UM_DELUSERALL_NACK,EV_UM_BGN+18);
//用户管理初始化命令
OSPEVENT(MCS_UM_INIT_CMD,EV_UM_BGN+19);

#endif
