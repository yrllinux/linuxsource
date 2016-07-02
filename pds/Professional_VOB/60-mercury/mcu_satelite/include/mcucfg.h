  /*****************************************************************************
   模块名      : mcu配置界面化
   文件名      : mcucfg.h
   相关文件    : mcucfg.cpp
   文件实现功能: MCU配置界面化头文件
   作者        : 许世林
   版本        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/23  4.0         许世林        创建
******************************************************************************/

#ifndef __MCUCFG_H
#define __MCUCFG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "osp.h"
#include "mcustruct.h"
#include "agentcommon.h"

//封装部分mcu agent结构
struct TAgentMcuGenCfg
{
    u8              m_byVmpStyleNum;
    TVmpAttachCfg   m_atVmpStyle[MAX_VMPSTYLE_NUM];
    TLocalInfo      m_tLocal;
    TNetWorkInfo    m_tNet;
    TQosInfo        m_tQos;
    TNetSyncInfo    m_tNetSyn;
    u32             m_dwDcsIp;

    u8              m_byTrapListNum;                          
    TTrapInfo       m_atTrapList[MAXNUM_TRAP_LIST];
    TDSCModuleInfo  m_tDscInfo;
	TLoginInfo		m_tLoginInfo;
};

class CMcuCfgInst : public CInstance  
{
	enum 
	{ 
		STATE_IDLE,
		STATE_CFGING
	};

public:
    void  InstanceEntry( CMessage * const pcMsg );

    CMcuCfgInst();
    virtual ~CMcuCfgInst();

    void  DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
    void  DaemonProcAppTaskRequest( const CMessage * pcMsg );

public:
    void  ProcCpuPercentage(const CMessage * pcMsg);
    void  ProcMcuEqpCfg(const CMessage * pcMsg);
    void  ProcBrdCfg(const CMessage * pcMsg);
    void  ProcMcuGeneralCfg(const CMessage * pcMsg);
    void  ProcRebootBrd(const CMessage * pcMsg);
    void  ProcMcsGetVmpScheme(const CMessage* pcMsg);
    void  ProcMcuSynMcsCfgNtf(const CMessage* pcMsg);
	void  ProcUpdateBrdVersion( const CMessage * const pcMsg );
    void  ProcMcsGetHduScheme(const CMessage* pcMsg);                  //4.6版本 新加 jlb
    void  ProcMcsChangeHduScheme(const CMessage* pcMsg);

	void  ProcStartUpdateDscVersionRsp( const CMessage* pcMsg );
	void  ProcDscUpdateFileRsp( const CMessage* pcMsg );

	void  ProcDscRegSucceedNotif( const CMessage* pcMsg ); // 有DSC板注册成功, zgc, 2007-08-24

	void  ProcSetDscInfoRsp( const CMessage * const pcMsg );
	void  ProcDscCfgWaitingChangeOverTimerOut( const CMessage * const pcMsg ); 

	void  ProcUpdateTelnetLoginInfo(const CMessage * const pcMsg);

	void  ProcMcsDisconnectNotif(const CMessage * const pcMsg);	// 通知会控断链，清处配置保护，zgc, 2007-11-09

	void  ProcVCSGeneralCfg(const CMessage * pcMsg);
	
private:
    u8      GetSlotType(u8 byBrdType);
    BOOL32  CheckBrdIndex(TBrdCfgInfo * ptBrdCfg, u8 byBrdNum, u8 byBrdIndex);
    BOOL32  IsValidRRQMtadpIp(TMcuNetCfgInfo &tNetCfgInfo);

    u8      BrdTypeOut2In(u8 byType);
    u8      BrdTypeIn2Out(u8 byType);
    u8      BrdStatusOut2In(u8 byStatus);
    u8      BrdStatusIn2Out(u8 byStatus);
    BOOL32  BrdCfgOut2In(TBoardInfo *ptIn, TBrdCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  BrdCfgIn2Out(TBrdCfgInfo *ptIn, TBoardInfo *ptOut, u8 byNum = 1);
    BOOL32  MixerCfgOut2In(TEqpMixerInfo *ptIn, TEqpMixerCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  MixerCfgIn2Out(TEqpMixerCfgInfo *ptIn, TEqpMixerInfo *ptOut, u8 byNum = 1);
    BOOL32  RecCfgOut2In(TEqpRecInfo *ptIn, TEqpRecCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  RecCfgIn2Out(TEqpRecCfgInfo *ptIn, TEqpRecInfo *ptOut, u8 byNum = 1);
    BOOL32  TWCfgOut2In(TEqpTVWallInfo *ptIn, TEqpTvWallCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  TWCfgIn2Out(TEqpTvWallCfgInfo *ptIn, TEqpTVWallInfo *ptOut, u8 byNum = 1);
    BOOL32  BasCfgOut2In(TEqpBasInfo *ptIn, TEqpBasCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  BasCfgIn2Out(TEqpBasCfgInfo *ptIn, TEqpBasInfo *ptOut, u8 byNum = 1);
    //zw[08/07/2008]
    BOOL32  BasHDCfgOut2In(TEqpBasHDInfo *ptIn, TEqpBasHDCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  BasHDCfgIn2Out(TEqpBasHDCfgInfo *ptIn, TEqpBasHDInfo *ptOut, u8 byNum = 1);

	//4.6新加外设 jlb
	BOOL32  HduCfgOut2In(TEqpHduInfo *ptIn, TEqpHduCfgInfo *ptOut, u8 byNum = 1);
	BOOL32  HduCfgIn2Out(TEqpHduCfgInfo *ptIn, TEqpHduInfo *ptOut, u8 byNum = 1);
	BOOL32  SvmpCfgOut2In(TEqpSvmpInfo *ptIn, TEqpSvmpCfgInfo *ptOut, u8 byNum = 1);
	BOOL32  SvmpCfgIn2Out(TEqpSvmpCfgInfo *ptIn, TEqpSvmpInfo *ptOut, u8 byNum = 1);
	BOOL32  DvmpCfgOut2In(TEqpDvmpBasicInfo *ptIn, TEqpDvmpCfgBasicInfo *ptOut, u8 byNum = 1);
	BOOL32  DvmpCfgIn2Out(TEqpDvmpCfgBasicInfo *ptIn, TEqpDvmpBasicInfo *ptOut, u8 byNum = 1);
	BOOL32  MpuBasCfgOut2In(TEqpMpuBasInfo *ptIn, TEqpMpuBasCfgInfo *ptOut, u8 byNum = 1);
	BOOL32  MpuBasCfgIn2Out(TEqpMpuBasCfgInfo *ptIn, TEqpMpuBasInfo *ptOut, u8 byNum = 1);
	BOOL32  EbapCfgOut2In(TEqpEbapInfo *ptIn, TEqpEbapCfgInfo *ptOut, u8 byNum = 1);
	BOOL32  EbapCfgIn2Out(TEqpEbapCfgInfo *ptIn, TEqpEbapInfo *ptOut, u8 byNum = 1);
	BOOL32  EvpuCfgOut2In(TEqpEvpuInfo *ptIn, TEqpEvpuCfgInfo *ptOut, u8 byNum = 1);
	BOOL32  EvpuCfgIn2Out(TEqpEvpuCfgInfo *ptIn, TEqpEvpuInfo *ptOut, u8 byNum = 1);

    BOOL32  VmpCfgOut2In(TEqpVMPInfo *ptIn, TEqpVmpCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  VmpCfgIn2Out(TEqpVmpCfgInfo *ptIn, TEqpVMPInfo *ptOut, u8 byNum = 1);
    BOOL32  PrsCfgOut2In(TEqpPrsInfo *ptIn, TPrsCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  PrsCfgIn2Out(TPrsCfgInfo *ptIn, TEqpPrsInfo *ptOut, u8 byNum = 1);
    BOOL32  MtwCfgOut2In(TEqpMpwInfo *ptIn, TEqpMTvwallCfgInfo *ptOut, u8 byNum = 1);
    BOOL32  MtwCfgIn2Out(TEqpMTvwallCfgInfo *ptIn, TEqpMpwInfo *ptOut, u8 byNum = 1);
	BOOL32  DscInfoOut2In( TDSCModuleInfo &tIn, T8000BDscMod &tOut );
	BOOL32  DscInfoIn2Out( T8000BDscMod &tIn, TDSCModuleInfo &tOut );
    BOOL32  GeneralCfgOut2In(TAgentMcuGenCfg &tIn, TMcuGeneralCfg &tOut);
    BOOL32  GeneralCfgIn2Out(TMcuGeneralCfg &tIn, TAgentMcuGenCfg &tOut);

    BOOL32  NotifyMcsCpuPercentage(void);
    BOOL32  NotifyMcsEqpCfg(void);
    BOOL32  NotifyMcsBrdCfg(void);
    BOOL32  NotifyMcsGeneralCfg(void);
    BOOL32	NotifyMcsHduSchemeCfg( CServMsg &cMsg );                //4.6 jlb 

    void    NotifyMcsBoardStatus(const CMessage * pcMsg);
    void    SendMsg2Mcsssn(CServMsg &cServMsg, u16 wEvent, BOOL32 bAllMcsSsn = FALSE);
	BOOL32  IsIpMaskValid(u32 dwIpMask);

private:
	BOOL32  BrdCfgProcIntegration(CServMsg &cServMsg);
	BOOL32  IsValidNetParamAll(TMINIMCUNetParamAll tNetParamAll, u32 dwIpAddr=0, u32 dwIpMask=0);
#ifdef _MINIMCU_
	BOOL32  CheckNewDscInfo(TDSCModuleInfo tDscInfo);
#endif
	
private:
	u8  m_abyServMsgHead[SERV_MSGHEAD_LEN];			// 记录消息头, zgc, 2007-07-03
};

class CMcuCfgData
{
public:
	CMcuCfgData(){};
    virtual ~CMcuCfgData(){};

public:
	
private:

};

typedef zTemplate< CMcuCfgInst, 1, CMcuCfgData > CMcuCfgApp;

extern CMcuCfgApp	g_cMcuCfgApp;	//MCU配置界面化应用实例

#endif // __MCUCFG_H
