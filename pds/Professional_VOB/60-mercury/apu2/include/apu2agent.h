/*****************************************************************************
   模块名      : apu2boardagent
   文件名      : apu2agent.h
   相关文件    : 
   文件实现功能: 单板代理
   作者        : 周嘉麟
   版本        :  1.0
   日期		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/
#ifndef BAPAGENT_H
#define BAPAGENT_H

#include "boardagentbasic.h"
#include "brdwrapperdef.h"
#include "apu2cfg.h"

	/************************************************************************/
	/*						CBoardAgent单板代理类定义		                */
	/************************************************************************/
class CBoardAgent : public CBBoardAgent
{
public:
	CBoardAgent(){}
	~CBoardAgent(){}

public:
    void InstanceEntry( CMessage* const pMsg );

	void ProcBoardGetConfigAck( CMessage* const pcMsg );

	void ProcBoardGetConfigNAck( CMessage* const pcMsg );

    void ProcLedStatusReq(CMessage* const pcMsg);

	void ProcBoardLedStatus( CMessage* const pcMsg );

	void ProcBoardConfigModify( CMessage* const pcMsg );

public:
    TBrdLedState m_tLedState;
};


	/************************************************************************/
	/*						CBoardConfig单板配置类定义		                */
	/************************************************************************/
class CBoardConfig : public CBBoardConfig
{
public:
	CBoardConfig(){}
	~CBoardConfig(){}
public:

    BOOL32		ReadConfig();

    BOOL32		SetEqpEMixerEntry(u8 byMixIdx, TEqpMixerEntry tEqpMixerEntry);
	
	BOOL32		SetEqpBasEntry(u8 byBasIdx, TEqpMpuBasEntry tEqpMpuBasEntry);

	void		SetApu2MixerNum(u8 byMixerNum);

	void		SetApu2BasNum(u8 byBasNum);

	TApu2EqpCfg GetApu2Cfg();

	void		SetProductTest(BOOL32 bIsProduct);

	BOOL32		IsProductTest();
 
	BOOL32		IsSimuApu();
	
private:
	BOOL32		 m_bIsSimuApu;   //是否模拟apu
	u16          m_wSimApuVer;   //模拟apu版本号
    TApu2EqpCfg  m_tApu2Cfg;     //Apu2配置
	u8			 m_byQualityLvl; //音质等级
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	

API BOOL InitBrdAgent();
#endif