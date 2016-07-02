/*****************************************************************************
   模块名      : Board Agent
   文件名      : driagent.h
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
#ifndef DRIAGENT_H
#define DRIAGENT_H

#include "dri_config.h"
#include "boardagentbasic.h"


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

	//取面板灯状态消息处理
	void ProcLedStatusReq( CMessage* const pcMsg );
	//e1环回命令处理
	void ProcE1LoopBackCmd( CMessage* const pcMsg );

	//灯状态改变
	void ProcBoardLedStatus( CMessage* const pcMsg );
	//链路状态改变
	void ProcBoardLinkStatus( CMessage* const pcMsg );
	//MPC修改单板配置, zgc, 2007-09-19
	void ProcBoardConfigModify( CMessage* const pcMsg );
	
	//单板E1告警状态通知
	void ProcBoardE1AlarmNotif(CMessage* const pcMsg);
    //处理E1线路检测超时
    void ProcBoardCheckE1TimerOut( void );

private:
//#ifndef WIN32
	//TBrdDRILedStateDesc m_tLedState;
	TBrdLedState m_tBrdLedState;

    u16 m_wCheckE1Time;            //Dri板检测E1线路的时间间隔(毫秒)
    u8  m_wPortE1Num[8];               //各端口配置E1端口数
    u16 m_wLastE1BandWidth[8];     //保存上次检测E1带宽结果

//#endif

	u8 m_byE1Alarm[8];	// 记录8路E1的Alarm信息 [10/26/2011 chendaiwei]
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CDriConfig, 0> CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

API BOOL InitDriAgent();
API u32 BrdGetDstMcuNode(void);
API u32 BrdGetDstMcuNodeB(void);
API void noupdatesoft(void);
API void updatesoft(void);
API void pdrimsg(void);
API void npdrimsg(void);
void DriLog(char * fmt, ...);
API u8  GetBoardSlot();

#endif  /* DRIAGENT_H */
