/*=============================================================================
模   块   名: 网管服务器查询库定义
文   件   名: nmsinttestcom.h
相 关  文 件: 无
文件实现功能: 消息定义/结构定义
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/07/26  4.0     王昊        创建
=============================================================================*/

#ifndef _NMSINTTESTCOM_20050726_H_
#define _NMSINTTESTCOM_20050726_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kdvtype.h"
#include "eventid.h"

#define EV_SVR_TEST_REQ         (u8)(EV_TEST_SVR_BGN + 1)   //查询同步操作返回结果
#define EV_SVR_TEST_QRY         (u8)(EV_TEST_SVR_BGN + 2)

#define SVR_QUERY_EQP_STAMP         (u8)1   //检查设备的告警时间戳 u32
#define SVR_QUERY_EQP_LINK          (u8)2   //检查设备的链路状态 u8
#define	SVR_QUERY_CONFIG            (u8)3   //服务器配置查询 TSvrConfig
#define SVR_QUERY_TIME              (u8)4   //检查系统当前时间是否和指定时间相近 time_t
#define SVR_QUERY_EQP_ISEXIST       (u8)5   //检查设备是否存在 BOOL32
#define SVR_QUERY_CMD_RET           (u8)6   //查询上次操作的结果 u32
#define SVR_QUERY_ALLUSERNAME       (u8)7   //查询所有用户名 用户名数组
#define SVR_QUERY_EQP_VER           (u8)8   //查询设备版本号 版本号字符串

#define SVR_START_CM_LOOP_CMD       (u8)20  //开始配置轮询
#define SVR_STOP_CM_LOOP_CMD        (u8)21  //停止配置轮询
#define SVR_START_FM_LOOP_CMD       (u8)22  //开始告警轮询
#define SVR_STOP_FM_LOOP_CMD        (u8)23  //停止告警轮询

#endif  //  _NMSINTTESTCOM_20050726_H_