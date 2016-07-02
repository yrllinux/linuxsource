/*****************************************************************************
   模块名      : 终端适配会话
   文件名      : mtadpssn.h
   相关文件    : mtadpssn.cpp
   文件实现功能: 终端适配会话定义
   作者        : 贾清亮
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/08  3.0         贾清亮        创建
******************************************************************************/
#ifndef __MTADPSSN_H_
#define __MTADPSSN_H_

#include "osp.h"

class CMtAdpSsnInst : public CInstance  
{
	enum 
	{ 
		STATE_IDLE = 0,  //空闲
		STATE_NORMAL,    //正常
	};
	
protected:
	//发消息给MtAdp
	void SendMsgToMtAdp( u16 wEvent, u8 * const pbyMsg, u16 wLen );
    //发消息给323MtAdp
    void SendMsgToH323MtAdp( CServMsg &cMsg );
	//发消息给Mcu
	void SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen );

public:
	//普通实例入口
	void InstanceEntry( CMessage * const pcMsg );
	//处理MtAdp注册消息
	void ProcMtAdpRegisterReq( CMessage * const pcMsg );
	//处理Mp断链
	void ProcMtAdpDisconnect( CMessage * const pcMsg );	
	//处理终端到MCU的消息
	void ProcMtAdpMcuMsg( CMessage * const pcMsg );
	//处理MCU到终端的消息
	void ProcMcuMtAdpMsg( CMessage * const pcMsg );    
    
	//处理MCU级联消息
	void ProcMcuCasCadeMsg( CMessage * const pcMsg );
    
    void ProcMtAdpGetMsStatusReq( CMessage * const pcMsg );
	
	//处理终端到MCU的GK消息
	void ProcMtAdpMcuGKMsg( CMessage * const pcMsg );
	//处理MCU到终端的GK消息
	void ProcMcuMtAdpGKMsg( CMessage * const pcMsg );

	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );

public:
	CMtAdpSsnInst();
	virtual ~CMtAdpSsnInst();

private:
	u32 m_dwMtAdpNode;       //对应MtAdp节点号
	u32 m_dwMtAdpAppIId;     //注册该MtAdp板的MtAdp实例APP号
	u8  m_byMtAdpId;         //MtAdp号
	u8  m_byMtAdpAttachMode; //附属方式
	u8  m_byProtocolType;    //适配的协议类型//H323、H320、SIP
	u8  m_byMaxMtNum;        //适配的支持最大终端数
};


class CMtAdpConfig
{
public:
	BOOL32 SendMsgToMtAdpSsn( u8 byMpId, u16 wEvent, CServMsg & cServMsg );
	BOOL32 SendMsgToMtAdpSsn( const TMt & tDstMt, u16 wEvent, CServMsg & cServMsg );
	BOOL32 BroadcastToAllMtAdpSsn( u16 wEvent, const CServMsg & cServMsg );	

public:
	CMtAdpConfig();
	virtual ~CMtAdpConfig();
};

typedef zTemplate< CMtAdpSsnInst, MAXNUM_DRI, CMtAdpConfig, sizeof( u8 ) > CMtAdpSsnApp;

extern  CMtAdpSsnApp g_cMtAdpSsnApp;

#endif
