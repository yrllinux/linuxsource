/*****************************************************************************
   模块名      : KDVMCU
   文件名      : brdguard.h
   相关文件    : 
   文件实现功能: 单板守卫基类定义
   作者        : 李屹
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人          修改内容
   2002/01/25  0.9      李屹            创建
   2002/07/29  1.0      李屹            新方案调整
   2003/07/15  1.1      jianghy         方案调整
   2003/11/11  3.0      jianghy         3.0实现
   2004/11/10  3.6      libo            新接口修改
   2004/11/29  3.6      libo            移植到Linux
   2005/08/17  4.0      liuhuiyun       更新
******************************************************************************/
#ifndef BRDGUARD_H
#define BRDGUARD_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"

#ifdef _VXWORKS_
#include "brddrvlib.h"
#endif

#ifdef WIN32
#include "winbrdwrapper.h"
#endif

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

// 定时扫描单板状态定时器时长
#define SCAN_STATE_TIME_OUT			5000

class CCfgAgent;

class CBoardGuard : public CInstance  
{
public:
	CBoardGuard();
	~CBoardGuard();
	
protected:
	void InstanceEntry( CMessage * const pcMsg );   // 消息入口函数，必须重载
	void ProcGuardPowerOn( void );   // GUARD模块启动消息
	void ProcGuardStateScan( void ); // 单板状态定时扫描
    void ProcReboot(CMessage* const pMsg);          // 重启

protected:
	u8 m_byPowerStatus;
	u8 m_byPowerFanStatus;
	u8 m_byBoxFanStatus;
	u8 m_byMPCCpuStatus;
	u8 m_byMPC2TempStatus;
	u8 m_byPowerTempStatus;
	u8 m_byMPCMemoryStatus;
	u8 m_byNetSyncStatus;
	u32 m_dwSDHStatus;
	u8  m_byOldRemoteMpcState; // 对端Mpc板上一次状态
//#ifndef WIN32   // vx, linux
    TBrdLedState m_tBrdLedState;
//#endif
    BOOL32 m_b8KASlamOn;     //8ka当前的告警灯是否已经点上了
#ifdef _8KI_
	u32	m_dwCpuFan;			//cpu风扇转速
	u8 m_byCpuTemp;			//cpu温度
#endif
};

typedef zTemplate< CBoardGuard, 1 > CBoardGuardApp;
extern CBoardGuardApp	g_cBrdGuardApp;	

#endif /*End BRDGUARD_H*/
