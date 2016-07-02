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
public:
    TBrdLedState m_tLedState;

public:
    void  InstanceEntry( CMessage* const pMsg );
	//配置应答消息
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//配置否定应答消息
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
    //LED灯获取
    void ProcLedStatusReq(CMessage* const pcMsg);

	void ProcBoardLedStatus( CMessage* const pcMsg );

	void ProcBoardConfigModify( CMessage* const pcMsg );
};

class CBoardConfig : public CBBoardConfig
{
public:
    BOOL32 ReadConfig();

    u8     GetEqpId() const
    {
        return m_byEqpId;
    }
    void   SetEqpId( u8 byEqpId ) 
    {
        m_byEqpId = byEqpId;
    }
    u8     GetEqpIdB() const
    {
        return m_byEqpIdB;
    }
    void   SetEqpIdB( u8 byEqpId ) 
    {
        m_byEqpIdB = byEqpId;
    }
    u8     GetWorkMode() const
    {
        return m_byWorkMode;
    }
    void   SetWorkMode( u8 byWorkMode ) 
    {
        m_byWorkMode = byWorkMode;
    }
    void   SetEqpRcvStartPort( u16 wEqpRcvStartPort) 
    { 
        m_wEqpRcvStartPort = htons(wEqpRcvStartPort); 
    }
	u16    GetEqpRcvStartPort( void ) const 
    { 
        return ntohs(m_wEqpRcvStartPort); 
    }
    void   SetEqpRcvStartPort2( u16 wEqpRcvStartPort) 
    { 
        m_wEqpRcvStartPort2 = htons(wEqpRcvStartPort); 
    }
	u16    GetEqpRcvStartPort2( void ) const 
    { 
        return ntohs(m_wEqpRcvStartPort2); 
    }

	BOOL  GetIsTest(void){ return m_bTest; }
    void  SetIsTest( BOOL bTest ){ m_bTest = bTest; }
private:
    u8     m_byEqpId; 
    u8     m_byEqpIdB;
    u8     m_byWorkMode;
    u16    m_wEqpRcvStartPort;
    u16    m_wEqpRcvStartPort2;
	BOOL32 m_bTest;
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CBoardConfig > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	

API BOOL InitBrdAgent();
void BrdMpuAPIEnableInLinux(void);

#endif  /* BAPAGENT_H */
