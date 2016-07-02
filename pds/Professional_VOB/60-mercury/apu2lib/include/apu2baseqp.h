/*****************************************************************************
  模块名      : apu2lib
  文件名      : apu2baseqp.h
  相关文件    : 
  文件实现功能: apu2外设对象拓展
  作者        : 国大卫
  版本        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2BASEQP_H_
#define _APU2BASEQP_H_

#include "eqpbase.h"
#include "apu2baschnnl.h"
#include "evapu2.h"
#include "eqpautotest.h"

#define BAS_CONNETC_TIMEOUT             (u16)(3 * 1000)     // 连接超时值3s

	/************************************************************************/
	/*							CApu2BasEqp类定义								*/
	/************************************************************************/
class CApu2BasEqp:public CEqpBase
{
public:
	CApu2BasEqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum);
	~CApu2BasEqp();

public:
	/***********************消息入口*************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);

    /***********************消息接口*************************/
	//注册Mcu成功处理
	BOOL32  OnRegisterMcuAck(CMessage *const pcMsg);
	//注册Mcu失败处理
	BOOL32  OnRegisterMcuNack(CMessage *const pcMsg);
	//修改发送地址
	BOOL32	OnModifyMcuRcvIp(CMessage* const pcMsg);
	//打印
	BOOL32	OnBasPrint(u16 wMsgID);

	/************************业务接口***********************/
	//外设初始化
	BOOL32  Init();
	//外设销毁
	BOOL32  Destroy();
	//获取实例化通道
	CApu2BasChnnl* GetChnnl(const u16 wChnIdx);
	//发送通道状态
	void	SendChnlNotify(const u8 byState, const u16 wChIdx, BOOL32 bIsMainBrd);
	//发送设备状态
	void	SendEqpStusNotify(void);

private:
	BOOL32 m_bInited;
};
#endif