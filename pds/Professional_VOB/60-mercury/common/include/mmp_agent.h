/*****************************************************************************
   模块名      : Board Agent
   文件名      : mmpagent.h
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#ifndef MMPAGENT_H
#define MMPAGENT_H

#include "osp.h"
#include "mmp_config.h"
#include "boardagentbasic.h"


#define  MAX_TIME_SPAN_REBOOT	    3		// 最大重启时间间隔(分钟)

class CBoardAgent:public CBBoardAgent
{

public:
	CBoardAgent();
	~CBoardAgent();

protected:
	//实例消息处理入口函数，必须override
	void InstanceEntry(CMessage* const pMsg);

	//退出时调用
	void InstanceExit();

	//配置应答消息
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//配置否定应答消息
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );

	//MPC修改单板配置, zgc, 2007-09-17
	void ProcBoardConfigModify( CMessage* const pcMsg );
	
	//取面板灯状态消息处理
	void ProcLedStatusReq( CMessage* const pcMsg );
	//模块信息处理
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//灯状态改变
	void ProcBoardLedStatus( CMessage* const pcMsg );

	// 比较两个时间是否超过指定间隔
	BOOL32 Compare2TimeIsValid(struct tm* ptTimeA, struct tm* ptTimeB); 

private:
	time_t  m_tLastDisTimeA; // Mpc A 上次断链的时间
	time_t  m_tLastDisTimeB; // Mpc B 上次断链的时间
    
    u8  m_byConnectNum;
#ifdef MMP
	TBrdMMPLedStateDesc m_tLedState;
#endif
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CMmpConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

API BOOL InitMmpAgent();
void MmpLog(char * fmt, ...);
API void pmmpmsg(void);
API void npmmpmsg(void);

#endif  /* MMPAGENT_H */
