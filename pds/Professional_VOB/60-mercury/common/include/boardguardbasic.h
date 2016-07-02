/*****************************************************************************
   模块名      : Board Agent Basic
   文件名      : boardguardbasic.h
   相关文件    : 
   文件实现功能: 单板守卫基类定义
   作者        : 周广程
   版本        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/08/20  4.0         周广程       创建
******************************************************************************/
#ifndef BOARDGUARDBASIC_H
#define BOARDGUARDBASIC_H

#include "osp.h"
#include "mcuagtstruct.h"
#include "boardagentbasic.h"

class CBBoardGuard : public CInstance  
{
public:
	CBBoardGuard();
	~CBBoardGuard();
	
protected:
	//消息入口函数，必须重载
	void InstanceEntry( CMessage * const pcMsg );
	//GUARD模块启动消息
	void ProcGuardPowerOn(CMessage* const pcMsg);
	//单板状态定时扫描
	void ProcGuardStateScan(CMessage* const pcMsg);

private:
	TBrdAlarmState   m_tBrdAlarmState;
	TBrdLedState	 m_tBrdLedState;
#ifdef _LINUX12_  // 目前仅brdwrapper.h中有TBrdEthInfo
    /* 对于外设单板来说,IS2.2 8313获取内部通信网口（后网口）信息，因此普通单板仅需获取前网口信息 */
    TBrdEthInfo      m_tBrdFrontEthPortState; // 前网口状态
#endif
    u8 m_byBrdTempStatus;			//单板温度是否过高，过高为1，正常为0 [10/25/2011 chendaiwei]
	u8 m_byBrdCpuStatus;			//单板CPU占用情况，过高为1，正常为0 [10/25/2011 chendaiwei]
	u8 m_byBrdId;					//单板类型(BRD_TYPE_HDU等) [12/1/2011 chendaiwei]
	
};

typedef zTemplate< CBBoardGuard, 1 > CBoardGuardApp;
extern CBoardGuardApp	g_cBrdGuardApp;	//单板的守卫应用实例

#endif    /* CRIGUARD_H */
