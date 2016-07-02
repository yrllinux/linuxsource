/*****************************************************************************
模块名      : Board Agent
文件名      : hduagent.h
相关文件    : 
文件实现功能: 单板代理
作者        : 江乐斌
版本        : V4.6  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
******************************************************************************/
#ifndef _HDUAGENT_H
#define _HDUAGENT_H

#include "boardagentbasic.h"
#define MAXLEN_IPADDR_BRD_HDU   64 

class CHduBoardAgent : public CBBoardAgent
{
   void InstanceEntry(CMessage* const pMsg);
   void  ProcBoardGetConfigAck( CMessage* const pcMsg );  //取配置信息应答消息
   void  ProcBoardGetConfigNAck( CMessage* const pcMsg );    //取配置信息否定应答
};

class CHduBoardConfig : public CBBoardConfig
{
public:
	CHduBoardConfig()
	{
		m_byEqpId = 0;
		m_byHduModel = 0;
		m_byStartMode = 0;
		memset(atHduModePort, 0x0, sizeof(atHduModePort));
	}
    BOOL32 ReadConfig(void);
    void  GetOutPortType(u8 byNum, THduModePort &tModePort) 
	{
		tModePort.SetOutModeType( atHduModePort[byNum].GetOutModeType() );
		tModePort.SetOutPortType( atHduModePort[byNum].GetOutPortType() );
		tModePort.SetZoomRate( atHduModePort[byNum].GetZoomRate() );
	}
	void SetOutPortType( u8 byNum, THduModePort &tModePort )
	{
        atHduModePort[byNum].SetOutModeType( tModePort.GetOutModeType() );
		atHduModePort[byNum].SetOutPortType( tModePort.GetOutPortType() );
		atHduModePort[byNum].SetZoomRate( tModePort.GetZoomRate() );
	}

    u8     GetEqpId(void) const
    {
        return m_byEqpId;
    }
	
	void  SetEqpId( u8 byEqpId )
	{
		m_byEqpId = byEqpId;
	}
    u8     GetHduMode(void) const
    {
        return m_byHduModel;
    }
	
	void  SetHduMode( u8 byHduMode )
	{
		m_byHduModel = byHduMode;
	}
    
	BOOL  GetIsTest(void){ return m_bIsTest; }
    void  SetIsTest( BOOL bIsTest ){ m_bIsTest = bIsTest; }

	void SetStartMode(u8 byStartMode){ m_byStartMode = byStartMode;}
	u8   GetStartMode(){ return m_byStartMode;}

private:
	u8   m_byHduModel;
    u8   m_byEqpId;    
	BOOL m_bIsTest;
    THduModePort atHduModePort[MAXNUM_HDU_CHANNEL];
	u8   m_byStartMode;  //启动模式(由mpc告知)
	u8   m_byReserved;

};

typedef zTemplate< CHduBoardAgent, BOARD_AGENT_MAX_INSTANCE, CHduBoardConfig, 0 > CHduBrdAgentApp;

extern CHduBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

API BOOL InitBrdAgent(void);
u32 BrdGetDstMcuNode(void);
u32 BrdGetDstMcuNodeB(void);
u8 BrdGetHduModel(void);

#endif  /* _HDUAGENT_H */

