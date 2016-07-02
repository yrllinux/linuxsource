/*****************************************************************************
  模块名      : apu2lib
  文件名      : apu2basinst.h
  相关文件    : 
  文件实现功能: 消息转发
  作者        : 国大卫
  版本        : 2.0  
-----------------------------------------------------------------------------*/
#ifndef _APU2BASINST_H_
#define _APU2BASINST_H_

#include "osp.h" 
#include "eqpcfg.h"        
#include "mcustruct.h"     
#include "mcuver.h"
#include "evapu2.h"
#include "boardagent.h"
#include "eqpbase.h"
#include "apu2cfg.h"
#include "apu2baseqp.h"

/************************************************************************/
/*							CApu2BasInst类定义							*/
/************************************************************************/
class CApu2BasInst:public CEqpMsgSender, public CInstance
{
public:
	CApu2BasInst();
	virtual~CApu2BasInst();
public:
	void DaemonInstanceEntry(CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcInit(CMessage * const pcMsg);
	void DaemonProcPrint(CMessage * const pcMsg);

    void InstanceEntry( CMessage * const pcMsg );	
	void ProcInit(CMessage * const pcMsg);

	void PostEqpMsg(const u32 dwMcuNode, const u32 dwDstId, CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE);	
	int SetTimer(u32 dwTimerId, long nMilliSeconds, u32 dwPara = 0 );	
	int	KillTimer(u32 dwTimerId);	
	int DisConRegister( u32 dwMcuNode );
	BOOL32 IsValidTcpNode(const u32 dwTcpNode);
	BOOL32 DisconnectTcpNode(const u32 dwTcpNode);
	int ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort);
	u16 GetInsID();

	void TransMcuMsgToEqpBaseMsg(CMessage * const pcMsg);
	void TransEqpBaseMsgToMcuMsg(CServMsg& cMsg);

private:
	CEqpBase* m_pcEqp;
};

typedef zTemplate< CApu2BasInst, MAXNUM_APU2_BAS> CApu2BasApp;
extern CApu2BasApp g_cApu2BasApp;

#endif


