/*****************************************************************************
模块名      : Hdu2Board Agent
文件名      : hdu2agent.h
相关文件    : 
文件实现功能: HDU2单板代理
作者        : 田志勇
版本        : V4.7  Copyright(C) 2011 KDV, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/11/28    4.7         田志勇		  创建
******************************************************************************/
#ifndef _HDU2AGENT_H
#define _HDU2AGENT_H
#include "boardagentbasic.h"
#include "boardguardbasic.h"
/*=====================================================================
函数  : hdu2agtLog
功能  : HDU2代理打印接口
输入  : const u8 byLevel 打印级别
		const u16 wModule 打印模块号
		const s8* pszFmt, ...可变参数表
输出  : 无
返回  : 无
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
inline void hdu2agtLog( const u8 byLevel, const u16 wModule, const s8* pszFmt, ...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;		      
    va_start( argptr, pszFmt );
	vsnprintf(achBuf , 1024, pszFmt, argptr);

    va_end(argptr); 
	LogPrint( byLevel, wModule, achBuf );
    return;
}
class CHdu2BoardAgent : public CBBoardAgent
{
private:
   void  InstanceEntry(CMessage* const pMsg);
   void  ProcBoardGetConfigAck( CMessage* const pcMsg );     //取配置信息应答消息
   void  ProcBoardGetConfigNAck( CMessage* const pcMsg );    //取配置信息否定应答
};
class CHdu2BoardConfig : public CBBoardConfig
{
private:
	BOOL32 m_bIsTest;
    u8     m_byEqpId;    
    THduModePort m_atHdu2ChnCfg[MAXNUM_HDU_CHANNEL];
	u8     m_byStartMode;  //启动模式(由mpc告知)
	//用于记录从配置文件中读取到的各个通道的输出接口类型
	u8     m_byOutPortTypeInCfg[MAXNUM_HDU_CHANNEL];
public:
	CHdu2BoardConfig()
	{
		m_bIsTest = FALSE;
		m_byEqpId = 0;
		m_byStartMode = 0;
		memset(m_atHdu2ChnCfg, 0x0, sizeof(m_atHdu2ChnCfg));
		memset(m_byOutPortTypeInCfg, 0x0, sizeof(m_byOutPortTypeInCfg));
	}
    BOOL32 ReadConfig(void);
    void  GetChnCfg(u8 byNum, THduModePort &tModePort) 
	{
		tModePort.SetOutModeType( m_atHdu2ChnCfg[byNum].GetOutModeType() );
		tModePort.SetOutPortType( m_atHdu2ChnCfg[byNum].GetOutPortType() );
		tModePort.SetZoomRate( m_atHdu2ChnCfg[byNum].GetZoomRate() );
		tModePort.SetScalingMode( m_atHdu2ChnCfg[byNum].GetScalingMode() );
	}
	void SetChnCfg( u8 byNum, THduModePort &tModePort )
	{
        m_atHdu2ChnCfg[byNum].SetOutModeType( tModePort.GetOutModeType() );
		m_atHdu2ChnCfg[byNum].SetOutPortType( tModePort.GetOutPortType() );
		m_atHdu2ChnCfg[byNum].SetZoomRate( tModePort.GetZoomRate() );
		m_atHdu2ChnCfg[byNum].SetScalingMode( tModePort.GetScalingMode() );
	}
	
    u8 GetEqpId(void) const
    {
        return m_byEqpId;
    }
	
	void  SetEqpId( u8 byEqpId )
	{
		m_byEqpId = byEqpId;
	}
    
	BOOL  GetIsTest(void){ return m_bIsTest; }
    void  SetIsTest( BOOL bIsTest ){ m_bIsTest = bIsTest; }

	void SetStartMode(u8 byStartMode){ m_byStartMode = byStartMode;}
	u8   GetStartMode(){ return m_byStartMode;}
	void SetOutPortTypeInCfg(u8 byChnIdx,u8 byOutPortTypeInCfg)
	{
		if (byChnIdx >= MAXNUM_HDU_CHANNEL)
		{
			return;
		}
		m_byOutPortTypeInCfg[byChnIdx] = byOutPortTypeInCfg;
	}
	u8   GetOutPortTypeInCfg(u8 byChnIdx)
	{
		if (byChnIdx >= MAXNUM_HDU_CHANNEL)
		{
			return 0;
		}
		return m_byOutPortTypeInCfg[byChnIdx];
	}
};
typedef zTemplate< CHdu2BoardAgent, BOARD_AGENT_MAX_INSTANCE, CHdu2BoardConfig, 0 > CHdu2BrdAgentApp;
extern CHdu2BrdAgentApp	g_cBrdAgentApp;	//单板代理应用APP
API BOOL32 InitBrdAgent(void);
u32 BrdGetDstMcuNode(void);
u32 BrdGetDstMcuNodeB(void);
#endif  /* _HDU2AGENT_H */

