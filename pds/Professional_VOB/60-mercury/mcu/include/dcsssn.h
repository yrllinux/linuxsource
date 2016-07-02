/*****************************************************************************
   模块名      : mcu
   文件名      : dcsssn.h
   相关文件    : dcsssn.cpp
   文件实现功能: MCU的T120集成应用头文件
   作者        : 张宝卿
   版本        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/11/28  4.0         张宝卿        创建
******************************************************************************/
#ifndef _MCUDCS_H_
#define _MCUDCS_H_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcudcspdu.h"

class CDcsSsnInst : public CInstance
{
	enum
	{
		STATE_T120_IDLE,
		STATE_T120_NORMAL
	};

public:
	u32		m_dwDcsNode;			//DCS的OSP连接结点号
	u32		m_dwDcsIId;				//通讯DCS实例号
	u32		m_dwMcuIId;				//通讯MCU实例号
	u32		m_dwDcsIp;				//DCS的地址
	u16		m_wDcsPort;				//DCS的端口
	
public:
	void	DaemonInstanceEntry( CMessage *const pcMsg, CApp * pcApp );	
	void	DaemonProcDcsMcuRegReq( CMessage *const pcMsg, CApp *pcApp );
    void    DaemonProcAppTaskRequest( const CMessage * pcMsg );
    
	void	InstanceEntry( CMessage *const pcMsg );
	void	ProcDcsMcuMsg( CMessage *const pcMsg );
	void	ProcMcuDcsMsg( CMessage *const pcMsg );
	void	ProcDcsMcuRegisterReq( CMessage *const pcMsg ); 
	void	ProcMcuDcsCreateConfReq( CMessage *const pcMsg );
	void	ProcDcsMcuCreateConfRsp( CMessage *const pcMsg );
	void	ProcMcuDcsReleaseConfReq( CMessage *const pcMsg );
	void	ProcDcsMcuReleaseConfRsp( CMessage *const pcMsg );
	void	ProcMcuDcsAddMtReq( CMessage *const pcMsg );
	void	ProcMcuDcsDelMtReq(	CMessage *const pcMsg );
	void	ProcDcsMcuAddMtRsp( CMessage *const pcMsg );
	void	ProcDcsMcuDelMtRsp( CMessage *const pcMsg );
	void	ProcDcsMcuMtOnlineNotify ( CMessage *const pcMsg );
	void	ProcDcsMcuMtOfflineNotify( CMessage *const pcMsg );    
	void	ProcDcsDisconnect( CMessage *const pcMsg );
    void    ProcMSStateExchageNotify( CMessage *const pcMsg );
    void    ProcDcsMcuGetMsStatusReq( CMessage *const pcMsg );
	void	ClearInst();
	
    void    InstanceDump( u32 dwParam = 0 );
protected:
	BOOL32	SendMsgToDcs( u16 wEvent, u8 *const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32	SendMsgToInst( s32 nInst,  u16 wEvent, u8 *const pbyMsg = NULL, u16 wLen = 0 );
	
	void	McuDcsRegNack( CMcuDcsRegNackPdu cRegNackPdu, CNetDataCoder cRegCoder, 
		                   u8 achRegNackData[sizeof(CMcuDcsRegNackPdu)], u16 wErrorType, u16 wEvent );
	void	McuDcsRegAck(  CMcuDcsRegAckPdu cRegAckPdu, CNetDataCoder cRegCoder, 
						   u8 achRegAckData[sizeof(CMcuDcsRegAckPdu)], u16 wEvent );
public:
	CDcsSsnInst();
	virtual ~CDcsSsnInst();
};

class CDcsConfig
{
public:
	u32 m_adwDcsIp[MAXNUM_MCU_DCS];		//注册上来的DCS地址数组
	
public:
	BOOL32 SendMsgToMcuConf( u8 byConfIdx, u16 wEvent, u8 *const pbyMsg  = NULL, u16 wLen  = 0 );
	BOOL32 SendMsgToMcuConf( CServMsg &cServMsg );
	BOOL32 SendMsgToMcuDaemon( CServMsg &cServMsg );
    void BroadcastToAllDcsSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

public:	
	CDcsConfig();
	virtual ~CDcsConfig();
};

inline BOOL32 MSGBODY_LEN_GE(CServMsg &cServMsg, u16 wLen);

typedef zTemplate < CDcsSsnInst, MAXNUM_MCU_DCS, CDcsConfig > CDcsSsnApp;
extern CDcsSsnApp g_cDcsSsnApp;

#endif    // _MCUDCSSSN_H_
