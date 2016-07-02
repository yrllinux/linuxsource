/*****************************************************************************
  模块名      : apu2lib
  文件名      : apu2eqp.h
  相关文件    : 
  文件实现功能: apu2外设对象拓展
  作者        : 周嘉麟
  版本        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2EQP_H_
#define _APU2EQP_H_

#include "eqpbase.h"
#include "apu2chnnl.h"
#include "evapu2.h"
#include "eqpautotest.h"

#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)     // 连接超时值3s

	/************************************************************************/
	/*							CApu2Eqp类定义								*/
	/************************************************************************/
class CApu2Eqp:public CEqpBase
{
public:
	CApu2Eqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum);
	~CApu2Eqp();

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
	BOOL32  OnMixPrint(u16 wMsgID);

	/************************业务接口***********************/
	//外设初始化
	BOOL32  Init();
	//外设销毁
	BOOL32  Destroy();
	//获取实例化通道
	CApu2Chnnl* GetChnnl(const u16 wChnIdx);

private:
	BOOL32 m_bInited;
};

/************************************************************************/
/*							CTestEqp类定义								*/
/************************************************************************/
class CTestEqp:public CEqpBase
{
public:
	CTestEqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum);
	~CTestEqp();
	
public:
	/***********************消息入口*************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);
	
    /***********************消息接口*************************/
	//注册Mcu成功处理
	BOOL32  OnRegisterServerAck(CMessage *const pcMsg);
	//注册Mcu失败处理
	BOOL32  OnRegisterServerNack(CMessage *const pcMsg);
	//打印
	BOOL32  OnMixPrint(u16 wMsgID);
	//生产测试注册服务器定时处理
	virtual BOOL32 OnRegisterServerTimer();
	/************************业务接口***********************/
	//外设初始化
	BOOL32  Init();
	//外设销毁
	BOOL32  Destroy();
	//获取实例化通道
	CApu2Chnnl* GetChnnl(const u16 wChnIdx);
	
private:
	BOOL32 m_bInited;
};
#endif