/*****************************************************************************
   模块名      : Board Agent
   文件名      : bapagent.h
   相关文件    : 
   文件实现功能: 单板代理
   作者        : guzh
   版本        : V4.5  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#ifndef BAPAGENT_H
#define BAPAGENT_H

#include "boardagentbasic.h"

class CBoardAgent : public CBBoardAgent
{
    void ProcBoardGetConfigAck( CMessage* const )
    {
    }
    void ProcBoardGetConfigNAck( CMessage* const )
    {
    }
};

class CBoardConfig : public CBBoardConfig
{
public:
    BOOL32 ReadConfig();

    u8     GetEqpId() const
    {
        return m_byEqpId;
    }
private:
    u8     m_byEqpId;
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

API BOOL InitBrdAgent();

#endif  /* BAPAGENT_H */
