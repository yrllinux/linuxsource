/*****************************************************************************
  模块名      : apu2lib
  文件名      : apu2inst.h
  相关文件    : 
  文件实现功能: 消息转发
  作者        : 周嘉麟
  版本        : 1.0  
-----------------------------------------------------------------------------*/
#ifndef _APU2INST_H_
#define _APU2INST_H_

#include "osp.h" 
#include "eqpcfg.h"        
#include "mcustruct.h"     
#include "mcuver.h"
#include "evapu2.h"
#include "boardagent.h"
#include "eqpbase.h"
#include "apu2cfg.h"
#include "apu2eqp.h"

/************************************************************************/
/*							CApu2Inst类定义								*/
/************************************************************************/
class CApu2Inst:public CEqpMsgSender, public CInstance
{
public:
	CApu2Inst();
	virtual~CApu2Inst();
public:
	/***********************Daemon实例处理**************************/
	void   DaemonInstanceEntry(CMessage * const pcMsg, CApp* pcApp );
	void   DaemonProcInit(CMessage * const pcMsg);
	void   DaemonProcPrint(CMessage * const pcMsg);
#ifdef _8KI_//主要为代替板代理从MCU获取混音器配置
	//8KI上电处理
	void   DaemonProcPowerOn();
	//建联超时
	void   DaemonProcConnetTimerOut();
	//注册超时
	void   DaemonProcRegTimerOut();
	//注册成功
	void   DaemonProcRegAck(CMessage * const pcMsg);
	//注册失败
	void   DaemonProcRegNack();
	//断链处理
	void   DaemonProcOspDisconnect();
#endif
	/***********************普通实例处理****************************/
    void   InstanceEntry( CMessage * const pcMsg );	
	//外设初始化
	void   ProcInit(CMessage * const pcMsg);
	//生产测试初始化
	void   ProcTestInit(CMessage * const pcMsg);

	/**************************协议接口*****************************/
	//消息发送
	void   PostEqpMsg(const u32 dwMcuNode, const u32 dwDstId, CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE);	
	//设置定时
	int    SetTimer(u32 dwTimerId, long nMilliSeconds, u32 dwPara = 0 );	
	//取消定时
	int	   KillTimer(u32 dwTimerId);	
	//断链注册
	int    DisConRegister( u32 dwMcuNode );
	//校验Tcp结点有效性
	BOOL32 IsValidTcpNode(const u32 dwTcpNode);
	//断开一个Tcp结点连接
	BOOL32 DisconnectTcpNode(const u32 dwTcpNode);
	//创建TCP连接
	int    ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort);
	//获取实例号
	u16    GetInsID();

	/***********************消息转换处理****************************/
	//mcu与外设通信消息转换成EqpBase消息
	void   TransMcuMsgToEqpBaseMsg(CMessage * const pcMsg);
	//EqpBase消息转换成mcu与外设通信消息
	void   TransEqpBaseMsgToMcuMsg(CServMsg& cMsg);

private:
	CEqpBase* m_pcEqp;
#ifdef _8KI_
	BOOL32    m_bIsGetMixerCfg;//标志是否从MCU获取到混音器配置
	u32       m_dwMcuNode;     //标记与MCU获取配置节点号，获取配置后释放断开该节点
#endif
};

typedef zTemplate< CApu2Inst, MAXNUM_APU2_MIXER> CApu2MixApp;
extern CApu2MixApp g_cApu2MixApp;

#endif


