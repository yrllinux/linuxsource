/*****************************************************************************
模块名      : prs
文件名      : mcuprsguard.h
相关文件    : mcuprsguard.cpp
文件实现功能: prs守卫应用类头文件
作者        : 周广程
版本        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/04/07  4.0         周广程        创建
******************************************************************************/

#ifndef __PRSGUARD_H
#define __PRSGUARD_H

#include "osp.h"
#include "evmcueqp.h"
#include "mcustruct.h"
#include "mcuconst.h"
#include "mcuprs.h"


#define PRSGUARD_MAINTASKSCAN_TIMEOUT			(u16)(10*1000)		// prs guard定时探察prs主线程的时间间隔
#define MAXNUM_MAINTASKSCAN_TIMEOUT				(u8)3				// 未收到正确响应的最大次数

class CMcuPrsGuard : public CInstance
{
	enum //实例状态
	{
		STATE_IDLE   = 0,
		STATE_NORMAL = 1,
	};
public:
	CMcuPrsGuard(){};
	~CMcuPrsGuard(){};

private:
	//-------------消息响应----------------
	void InstanceEntry( CMessage *const pcMsg );    //消息入口
	void ProcGuardPowerOn( CMessage *const pcMsg );	//启动消息响应函数
	void ProcMainTaskScanTimeOut( CMessage *const pcMsg );	//探测主线程定时器超时处理函数
	void ProcMainTaskScanAck( CMessage *const pcMsg );	//探察回应消息处理函数
};


class CMcuPrsGuardData
{
public:
	CMcuPrsGuardData();
	virtual ~CMcuPrsGuardData(){};

public:
	u8 GetNoScanAckTimes(void);
	void AddNoScanAckTimes(void);
	void ResetNoScanAckTimes(void);

	BOOL32 IsRecvScanAck(void);
	void  SetIsRecvScanAck(BOOL32 bIsRecv);

private:
	u8 m_byNoScanAckTimes;
	u8 m_byRecvScanAck;
};

typedef zTemplate< CMcuPrsGuard, 1, CMcuPrsGuardData > CMcuPrsGuardApp;
extern CMcuPrsGuardApp g_cMcuPrsGuardApp;

#endif

