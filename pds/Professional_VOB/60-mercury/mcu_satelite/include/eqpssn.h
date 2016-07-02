/*****************************************************************************
   模块名      : mcu_new
   文件名      : eqpssn.h
   相关文件    : eqpssn.cpp
   文件实现功能: 外设会话应用类头文件
   作者        : 薛新文
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/08/19  0.9         薛新文      创建
   2002/08/20  0.9         LI Yi       添加代码
******************************************************************************/

#ifndef _MCU_EQPSSN_H
#define _MCU_EQPSSN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEqpSsnInst : public CInstance  
{
	enum 
	{ 
		STATE_IDLE,
		STATE_NORMAL
	};

public:
	u32		m_dwEqpNode;	//对应外设节点号
	u32		m_dwEqpIId;		//外设的IId
	u32     m_dwEqpIpAddr;  //外设IP地址
	u8		m_byEqpId;		//外设ID
	u8		m_byEqpType;	//外设类型
	char    m_achAlias[MAXLEN_EQP_ALIAS];     //外设别名

protected:
	BOOL32 SendMsgToPeriEqp( u16 wEvent, u8 * const pbyMsg, u16 wLen );

public:
	void ProcEqpDisconnect( const CMessage * pcMsg );
	void ProcEqpMcuRegMsg( const CMessage * pcMsg );
	void ProcEqpToMcuMsg( const CMessage * pcMsg );
	void ProcMcuToEqpMsg( const CMessage * pcMsg );
	void ProcEqpToMcuDaemonConfMsg( const CMessage * pcMsg );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void InstanceDump( u32 param = 0 );
	void ProcEqpGetMsStatusReq(CMessage* const pcMsg); // 处理外设取主备倒换状态
	void ProcMcuDisconnectEqp(CMessage* const pcMsg);	

    //qos msg
    void ProcMcuEqpSetQos(CServMsg &cMsg);
	
	CEqpSsnInst();
	virtual ~CEqpSsnInst();
};


class CEqpConfig
{
public:

public:
	BOOL32 SendMsgToPeriEqpSsn( u8 byPeriEqpId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void   BroadcastToAllPeriEqpSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	CEqpConfig();
	virtual ~CEqpConfig();
};

typedef zTemplate< CEqpSsnInst, MAXNUM_MCU_PERIEQP, CEqpConfig, sizeof( u8 ) > CEqpSsnApp;

extern CEqpSsnApp	g_cEqpSsnApp;	//外设会话应用实例


#endif

// END OF FILE
