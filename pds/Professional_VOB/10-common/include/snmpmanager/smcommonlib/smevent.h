/*****************************************************************************
   模块名      : KDVNMS
   文件名      : smevent.h
   相关文件    : smevent.cpp
   文件实现功能: 服务器的Event定义
   作者		   : 薛新文
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2010/01/01  5.0      陆昆朋      创建
******************************************************************************/
#ifndef SM_SMEVENT_H
#define SM_SMEVENT_H

#include "osp.h"
#include "eventid.h"

#define EV_SYS_BGN EV_SVR_BGN

#define EV_SM_BGN EV_SVR_BGN + 100

#define EV_CM_BGN EV_SVR_BGN + 200

#define EV_FM_BGN EV_SVR_BGN + 300

#define EV_CB_BGN EV_SVR_BGN + 400

#define EV_NP_BGN EV_SVR_BGN + 500

//System消息定义
OSPEVENT( EV_SYS_SM_CMD,						EV_SYS_BGN + 0 );
OSPEVENT( EV_SYS_CM_CMD,						EV_SYS_BGN + 1 );
OSPEVENT( EV_SYS_FM_CMD,						EV_SYS_BGN + 2 );
OSPEVENT( EV_SYS_SM_START_CMD,					EV_SYS_BGN + 3 );

////////////////////////////////////////////////////////////////////
//SessionManager 消息定义
//所有主动获取的消息都转发给CM
//而对于设备主动上报的信息都走FM，另外数据库操作相关也走FM

//自身消息
OSPEVENT( EV_SM_CONNECT,						EV_SM_BGN + 0 );
OSPEVENT( EV_SM_REGISTER,						EV_SM_BGN + 1 );
//snmpmanaer向nms建链及注册Timer消息
OSPEVENT( EV_SM_CONNECT_REGISTER,				EV_SM_BGN + 2 );

//SM主动与CM通信消息
OSPEVENT( EV_SM_CM_CMD,							EV_SM_BGN + 10 );
OSPEVENT( EV_SM_CM_START_CMD,					EV_SM_CM_CMD + 1 );
OSPEVENT( EV_SM_CM_MERGEDEVICE_NTY,				EV_SM_CM_CMD + 2 );
//设备信息添加删除及修改消息
OSPEVENT( EV_SM_CM_DEVICE_OPT_CMD,				EV_SM_CM_CMD + 3 );
//需要感知REQ及ACK还有MSG TYPE的消息定义
OSPEVENT( EV_SM_CM_DEVICE_CFG_CMD,				EV_SM_CM_CMD + 4 );
OSPEVENT( EV_SM_CM_DEVICE_PFM_CMD,				EV_SM_CM_CMD + 5 );
OSPEVENT( EV_SM_CM_DEVICE_RESET_CMD,			EV_SM_CM_CMD + 6 );
OSPEVENT( EV_SM_CM_DEVICE_SELFUPDATE_CMD,		EV_SM_CM_CMD + 7 );
OSPEVENT( EV_SM_CM_DEVICE_BATCH_CFG_CMD,		EV_SM_CM_CMD + 8 );

//SM主动与FM通信消息
OSPEVENT( EV_SM_FM_CMD,							EV_SM_BGN + 50 );
OSPEVENT( EV_SM_FM_PFMSTATISTIC_CMD,			EV_SM_FM_CMD + 1 );
//SessionManager 消息定义 End
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//CM消息定义
//自身消息
OSPEVENT( EV_CM_START_POLL_CMD,					EV_CM_BGN + 0 );
OSPEVENT( EV_CM_TIMER_FOR_POLL_CMD,				EV_CM_BGN + 1 );
OSPEVENT( EV_CM_TIMER_FOR_DEVICEINFO_CMD,		EV_CM_BGN + 2 );
OSPEVENT( EV_CM_TIMER_FOR_QUERY_UPDATE_CMD,		EV_CM_BGN + 3 );

//CM主动与SM通信消息
OSPEVENT( EV_CM_SM_CMD,							EV_CM_BGN + 10 );
OSPEVENT( EV_CM_SM_DEVICE_ONLINE_CMD,			EV_CM_SM_CMD + 1 );
OSPEVENT( EV_CM_SM_DEVICE_OFFLINE_CMD,			EV_CM_SM_CMD + 2 );
OSPEVENT( EV_CM_SM_CHANGEIP_NOTIFY,				EV_CM_SM_CMD + 3 );
OSPEVENT( EV_CM_SM_OPER_FAIL,					EV_CM_SM_CMD + 4 );

//CM主动与FM通信消息
OSPEVENT( EV_CM_FM_CMD,							EV_CM_BGN + 1 );
//测试告警
OSPEVENT( EV_CM_FM_UNIT_ALARM_NOTIFY,			EV_CM_FM_CMD + 1 );
//CM消息定义 End
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//FM消息定义
//FM发给自己的消息

//FM主动与SM通信消息
OSPEVENT( EV_FM_SM_CMD,							EV_FM_BGN + 10 );
OSPEVENT( EV_FM_SM_ALARM_NOTIFY,				EV_FM_SM_CMD + 1 );

//FM主动与CM通信消息
OSPEVENT( EV_FM_CM_CMD,							EV_FM_BGN + 40 );
//FM消息定义 End
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//CB消息定义

//CB主动与CM通信消息
OSPEVENT( EV_CB_CM_CMD,							EV_CB_BGN + 0 );
OSPEVENT( EV_CB_CM_BOARDSTATUS_NTF,				EV_CB_CM_CMD + 1 );
OSPEVENT( EV_CB_CM_SNMPREVFAIL_NTF,				EV_CB_CM_CMD + 2 );
OSPEVENT( EV_CB_CM_GETPOLLINFO_NTF,				EV_CB_CM_CMD + 3 );
OSPEVENT( EV_CB_CM_GETDEVICEINFO_NTF,			EV_CB_CM_CMD + 4 );
OSPEVENT( EV_CB_CM_DEVICECFG_NTF,				EV_CB_CM_CMD + 5 );
OSPEVENT( EV_CB_CM_GETDEVICEPFM_NTF,			EV_CB_CM_CMD + 6 );
OSPEVENT( EV_CB_CM_ADDBOARD_NTF,				EV_CB_CM_CMD + 7 );
OSPEVENT( EV_CB_CM_GETBOARD_NTF,				EV_CB_CM_CMD + 8 );
OSPEVENT( EV_CB_CM_GETBOARDCOMPLETED_NTF,		EV_CB_CM_CMD + 9 );
OSPEVENT( EV_CB_CM_CHGIP_NTF,					EV_CB_CM_CMD + 10 );
OSPEVENT( EV_CB_CM_DEVICELINK_NTF,				EV_CB_CM_CMD + 11 );
OSPEVENT( EV_CB_CM_DEVOPER_NTF,				    EV_CB_CM_CMD + 12 );
OSPEVENT( EV_CB_CM_QUERYUPDATE_NTF,				EV_CB_CM_CMD + 13 );

//CB主动与FM通信消息
OSPEVENT( EV_CB_FM_CMD,							EV_CB_BGN + 40 );
OSPEVENT( EV_CB_FM_ALARM_NOTIFY,				EV_CB_FM_CMD + 1 );
OSPEVENT( EV_CB_FM_PERFORMANCE_NOTIFY,			EV_CB_FM_CMD + 2 );

//CB主动与SM通信消息 (所有Nack消息，当cb获取失败后，直接发给sm)
OSPEVENT( EV_CB_SM_CMD,							EV_CB_BGN + 80 );
OSPEVENT( EV_CB_SM_OPER_FAIL,					EV_CB_SM_CMD + 1 );

//CB消息定义 End
////////////////////////////////////////////////////////////////////


//NP插件消息定义

//此消息暂时不发送
//表示所有NP发送给SM的消息总称
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK/SUBEV_TYPE_NOTIFY/SUBEV_TYPE_FINISH
OSPEVENT( EV_NP_SM_CMD,							EV_NP_BGN + 0 );

//此消息为NP向SM发送的配置相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP发送的MsgType为TNM_MSG_GET(获取)/TNM_MSG_SET(设置)
//SM回复消息体为CBoardCfgInfo/CMtCfgInfo对应的GetBuffer
//NP处理的时候直接调用CCfgInfoBase/CMtCfgInfo(const s8* pszBuffer, u32 dwSize)即可
OSPEVENT( EV_NP_SM_DEVICE_CFG,					EV_NP_BGN + 1 );

//此消息为NP向SM发送的获取当前性能相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP发送的MsgType为TNM_MSG_GET(获取)
//SM回复消息体为CDevPfmInfoBase对应的GetBuffer
//NP处理的时候直接调用CDevPfmInfoBase(const s8* pszBuffer, u32 dwSize)即可
OSPEVENT( EV_NP_SM_DEVICE_PFM,					EV_NP_BGN + 2 );


//此消息为NP向SM发送的配置统计相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK(回复)/SUBEV_TYPE_NACK(拒绝)/SUBEV_TYPE_NOTIFY(通知)/SUBEV_TYPE_FINISH()
//NP发送的MsgType为TNM_MSG_GET(获取)/TNM_MSG_NTF(通知)
//由于考虑到量比较大，SM会分批发送
//SM回复消息体如下
//SUBEV_TYPE_ACK消息体：
//SUBEV_TYPE_NOTIFY消息体：CDevPfmInfoNotify对应的GetBuffer(可能发多次，每次最多1000)
//SUBEV_TYPE_FINISH消息体：u32 num(性能的总数)
OSPEVENT( EV_NP_SM_DEVICE_PFMSTATISTIC,			EV_NP_BGN + 3 );

//此消息为NP向SM发送的机架图相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP发送的MsgType为TNM_MSG_GET(获取)
//SM回复消息体为CMockMachineInfo对应的GetBuffer
//NP处理的时候直接调用CMockMachineInfo(const s8* pszBuffer, u32 dwSize)即可
OSPEVENT( EV_NP_SM_DEVICE_MOCKMACHIN,			EV_NP_BGN + 4 );

//此消息为NP向SM发送的批量配置相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP发送的MsgType为TNM_MSG_GET(获取)/TNM_MSG_SET(设置)
//SM回复消息体为CBoardCfgInfo/CMtCfgInfo对应的GetBuffer
//NP处理的时候直接调用CCfgInfoBase/CMtCfgInfo(const s8* pszBuffer, u32 dwSize)即可
OSPEVENT( EV_NP_SM_DEVICE_BATCH_CFG,			EV_NP_BGN + 5 );


//系统中的定时器定义
//For Session mamage
#define SM_TIMER_FOR_OPER				EV_SVR_BGN + 120

//系统定时器定时到发出的消息定义
//For Session mamage
#define EV_SM_TIMER_FOR_OPER			EV_SVR_BGN + 120


#endif
