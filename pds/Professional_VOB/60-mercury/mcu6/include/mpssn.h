/*****************************************************************************
   模块名      : MP会话模块
   文件名      : mpssn.h
   相关文件    : 
   文件实现功能: MP会话模块实例定义
   作者        : 胡昌威
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/11  0.1         胡昌威        创建
******************************************************************************/
#ifndef __MPSSN_H_
#define __MPSSN_H_

class CMpSsnInst : public CInstance  
{
	enum 
	{ 
		STATE_IDLE,      //空闲
		STATE_NORMAL,    //正常
	};
	
protected:
	//发消息给Mp
	void SendMsgToMp( u16 wEvent, u8 * const pbyMsg, u16 wLen );
	//发消息给Mcu
	void SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen );

public:
	//普通实例入口
	void InstanceEntry( CMessage * const pcMsg );
	//处理Mp注册消息
	void ProcMpRegisterReq( CMessage * const pcMsg);
	//处理Mp断链
	void ProcMpDisconnect( CMessage * const pcMsg);

	// MP 取主备倒换状态
	void ProcMpGetMsStatusReq(CMessage* const pcMsg);
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );

	// [12/17/2009 xliang] add new functions
	BOOL32	IsMpReged( const u32 &dwMpIp );			//暂不用
    u8		GetIdleInst( void );					
	
    void	SetMpIp( u32 dwMpIp ) { m_dwMpIp = htonl(dwMpIp); }
    u32		GetMpIp( void ) { return ntohl(m_dwMpIp); }

public:
	CMpSsnInst();
	virtual ~CMpSsnInst();

private:
	u32 m_dwMpNode;       //对应Mp节点号
	u32 m_dwMpIId;        //Mp的App的IId
	u8  m_byMpId;         //Mp号
	u8  m_byMpAttachMode; //附属方式
	u32 m_dwMpIp;         //对应MP的IP : 网络序	// [12/17/2009 xliang] 
};


class CMpConfig
{
public:
	void SendMsgToMpSsn( u8 byMpId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void BroadcastToAllMpSsn(u16 wEvent, u8 * const pbyMsg, u16 wLen);

public:
	CMpConfig();
	virtual ~CMpConfig();
};

typedef zTemplate< CMpSsnInst, MAXNUM_DRI+1, CMpConfig, sizeof( u8 ) > CMpSsnApp;

extern  CMpSsnApp g_cMpSsnApp;

#endif
