/*****************************************************************************
   模块名      : KDVSNMPMANAGER
   文件名      : kdvnpif.h
   相关文件    : 
   文件实现功能: 插件的消息及消息体定义,供插件引用
   作者		   : 陆昆朋
   版本        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2009/12/31  5.0      陆昆朋      创建
******************************************************************************/
#ifndef KDV_NPIF_H
#define KDV_NPIF_H

//消息体定义头文件
#include "smevent.h"

//SM与NP交互的消息处理类
#include "smmsg.h"

//SM与NP交互的常用函数封装
#include "smTool.h"

//配置相关消息体管理头文件
#include "smcfginfo.h"

//性能相关消息体管理头文件
#include "smperformanceinfo.h"

//机架图相关消息体管理头文件
#include "smmockmachineinfo.h"


//以下为各消息及消息体详解

//此消息为NP向SM发送的配置相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP发送的MsgType为TNM_MSG_GET(获取)/TNM_MSG_SET(设置)
//SM回复消息体为CBoardCfgInfo/CMtCfgInfo对应的GetBuffer
//NP处理的时候直接调用CCfgInfoBase/CMtCfgInfo(const s8* pszBuffer, u32 dwSize)即可
//-----OSPEVENT( EV_NP_SM_DEVICE_CFG,					EV_NP_BGN + 1 );


//此消息为NP向SM发送的获取当前性能相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP发送的MsgType为TNM_MSG_GET(获取)
//SM回复消息体为CDevPfmInfoBase对应的GetBuffer
//NP处理的时候直接调用CDevPfmInfoBase(const s8* pszBuffer, u32 dwSize)即可
//-----OSPEVENT( EV_NP_SM_DEVICE_PFM,					EV_NP_BGN + 2 );


//此消息为NP向SM发送的配置统计相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK(回复)/SUBEV_TYPE_NACK(拒绝)/SUBEV_TYPE_NOTIFY(通知)/SUBEV_TYPE_FINISH()
//NP发送的MsgType为TNM_MSG_GET(获取)/TNM_MSG_NTF(通知)
//由于考虑到量比较大，SM会分批发送
//SM回复消息体如下
//SUBEV_TYPE_ACK消息体：
//SUBEV_TYPE_NOTIFY消息体：CDevPfmInfoNotify对应的GetBuffer(可能发多次，每次最多1000)
//SUBEV_TYPE_FINISH消息体：u32 num(性能的总数)
//-----OSPEVENT( EV_NP_SM_DEVICE_PFMSTATISTIC,			EV_NP_BGN + 3 );


//此消息为NP向SM发送的机架图相关消息
//NP发送请求为SUBEV_TYPE_REQ
//SM回复请求为SUBEV_TYPE_ACK/SUBEV_TYPE_NACK
//NP发送的MsgType为TNM_MSG_GET(获取)
//SM回复消息体为CMockMachineInfo对应的GetBuffer
//NP处理的时候直接调用CMockMachineInfo(const s8* pszBuffer, u32 dwSize)即可
//-----OSPEVENT( EV_NP_SM_DEVICE_MOCKMACHIN,			EV_NP_BGN + 4 );

#endif
