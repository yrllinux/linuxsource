/*****************************************************************************
   模块名      : Board Agent
   文件名      : criagent.h
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
#ifndef CRIAGENT_H
#define CRIAGENT_H

#include "cri_config.h"
#include "boardagentbasic.h"

class CBoardAgent:public CBBoardAgent
{

public:
	CBoardAgent();
	~CBoardAgent();

protected:
	//实例消息处理入口函数，必须override
	void InstanceEntry(CMessage* const pMsg);
	void InstanceExit();

	//配置应答消息
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//配置否定应答消息
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
	//取面板灯状态消息处理
	void ProcLedStatusReq( CMessage* const pcMsg );     // change[2012/05/04 liaokang]
	//灯状态改变 
	void ProcBoardLedStatus( CMessage* const pcMsg );   // change[2012/05/04 liaokang]
    // 取网口状态消息处理
	void ProcEthPortStatusReq( CMessage* const pcMsg );
    // 网口状态改变（主系统）
	void ProcBoardEthPortStatus( CMessage* const pcMsg );   // [2012/05/04 liaokang]
	//模块状态改变
	void ProcBoardModuleStatus( CMessage* const pcMsg );
	//MPC修改单板配置, zgc, 2007-09-17
	void ProcBoardConfigModify( CMessage* const pcMsg );
    
    //  支持取支持从系统（IS2.2 8313）Led、网口状态[2012/05/04 liaokang]
    // 从系统（IS2.2 8313） 注册请求
    void ProcBrdSlaveSysRegReq( CMessage* const pcMsg );
    // 从系统（IS2.2 8313） led状态
    void ProcBrdSlaveSysLedStatus( CMessage* const pcMsg );
    // 从系统（IS2.2 8313） 网口状态
    void ProcBrdSlaveSysEthPortStatus( CMessage* const pcMsg );
    // 断链处理
    void ProcDisconnect( CMessage* const pcMsg );
    // 发消息给从系统（IS2.2 8313）
    void PostMsgToBrdSlaveSys( u16 wEvent, void * pMsg = NULL, u16 wLen = 0 );

	//处理IS2.2重启[6/8/2013 chendaiwei]
	void ProcBoardIS22ResetCmd( CMessage* const pcMsg );

//#ifndef WIN32
//	TBrdCRILedStateDesc m_tLedState;
//#endif

// #if (defined _LINUX_ && defined _LINUX12_)
// 	TBrdCRI2LedDesc m_tLedState;
// #else
// 	TBrdCRILedStateDesc m_tLedState;
// #endif

//  支持IS2.2 [2012/05/04 liaokang]
 private:    
    TBrdLedState	 m_tBrdMasterSysLedState;   // 主系统（CRI/CRI2/IS2.2 8548）led状态
#ifdef _LINUX12_   // 目前仅brdwrapper.h中有TBrdEthInfo SWEthInfo
    /* 对于外设单板来说,IS2.2 8313获取内部通信网口（后网口）信息，因此普通单板仅需获取前网口信息 */
    TBrdEthInfo      m_tBrdMasterSysFrontEthPortState; // 主系统（CRI/CRI2/IS2.2 8548）前网口状态
    SWEthInfo        m_tBrdSlaveSysEthPortState;// 从系统（IS2.2 8313）网口状态 
#endif //end _LINUX12_
    TBrdLedState	 m_tBrdSlaveSysLedState;    // 从系统（IS2.2 8313）led状态
    u32			     m_dwBrdSlaveSysNode;		// 从系统（IS2.2 8313）的结点号
	u32			     m_dwBrdSlaveSysIId;		// 从系统（IS2.2 8313）的IID
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CCriConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

API BOOL InitCriAgent();
API void pcrimsg(void);
API void npcrimsg( void );
API void CriLog(char * fmt, ...);
API u32 BrdGetDstMcuNode( void );
API u32 BrdGetDstMcuNodeB( void );
API u8 GetBoardSlot();

#endif  /* CRIAGENT_H */
