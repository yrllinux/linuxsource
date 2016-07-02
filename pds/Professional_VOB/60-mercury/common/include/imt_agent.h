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
   2004/12/05  3.5         李 博        新接口的修改
******************************************************************************/
#ifndef IMTAGENT_H
#define IMTAGENT_H

#include "osp.h"
#include "agtcomm.h"
#include "mcuagtstruct.h"
#include "boardagentbasic.h"
#include "imt_config.h"


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

	//以下是和MANAGER的消息处理
	//配置应答消息
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//配置否定应答消息
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
	
	//取面板灯状态消息处理
	void ProcLedStatusReq( CMessage* const pcMsg );

	//以下是状态消息处理
	//内存状态
	//模块信息处理
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//灯状态改变
	void ProcBoardLedStatus( CMessage* const pcMsg );
	//风扇状态改变
	void ProcImtFanStatus( CMessage* const pcMsg );

	//MPC修改单板配置, zgc, 2007-09-17
	void ProcBoardConfigModify( CMessage* const pcMsg );

protected:
    
    void MsgGetConfAck(CMessage* const pcMsg);
    
//#ifndef WIN32
#ifdef IMT
	TBrdIMTLedStateDesc m_tLedState;
#endif
//#endif
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CImtConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

void ImtLog(char * fmt, ...);
API BOOL InitImtAgent();
API void pimtmsg(void);
API void npimtmsg(void);

#endif  /* IMTAGENT_H */
