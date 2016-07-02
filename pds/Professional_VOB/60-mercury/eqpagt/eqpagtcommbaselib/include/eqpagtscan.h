/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtscan.h
    相关文件    : 
    文件实现功能: 扫描设备trap信息
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef _EQPAGT_SCAN_H_
#define _EQPAGT_SCAN_H_
#include "eqpagtutility.h"

// EqpAgtScan线程相关消息
// 必须有括号（涉及到运算，但是EV_AGT_BGN本身没有括号且存在优先级的问题）
#define  EQPAGT_SCAN_POWERON        (EV_AGT_BGN)        // 启动
#define  EQPAGT_TRAPINFO            (EV_AGT_BGN + 1)    // Trap信息（用于调试）
#define  EQPAGT_SCAN_TIMER          (EV_AGT_BGN + 2)    // 常规定时器

// 定时扫描设备状态定时器时长
#define  SCAN_STATE_TIME_OUT		5000

class CEqpAgtScan : public CInstance  
{
public:
	CEqpAgtScan();
	~CEqpAgtScan();
	
protected:
	void InstanceEntry( CMessage * const pcMsg );   // 消息入口函数，必须重载
	void ProcScanPowerOn( void );                   // 模块启动消息
    void ProcScanTimer( void );                     // 常规定时器
    void ProcTrapInfoScan( CMessage* const pcMsg ); // 各业务模块Trap信息定时扫描
    void ScanAndSendTrap( TEqpAgtTrapFunc pfTrapFunc ); // Trap信息扫描并发送
private:
    u32  m_dwMaxTimerIdx;                           // 当前最大定时器序号
};

typedef zTemplate< CEqpAgtScan, 1 > CEqpAgtScanApp;
extern CEqpAgtScanApp	g_cEqpAgtScanApp;	

#endif /*End _EQPAGT_SCAN_H_ */
