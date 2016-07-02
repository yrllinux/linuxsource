/*****************************************************************************
   模块名      : mcu_new
   文件名      : McsSsn.h
   相关文件    : McsSsn.cpp
   文件实现功能: MCU会议控制台会话应用类头文件
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
******************************************************************************/

#ifndef _MCSSSN_H_
#define _MCSSSN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "usermanage.h"
#include "addrbook.h"

class CMcsSsnInst : public CInstance  
{

public:
	u32   m_dwMcsNode;				  //连接的会议控制台节点号
	u32   m_dwMcsIp;                  //mcs的IP地址
	u32	  m_dwMcsIId;                 //Mcs的IId
	s8    m_achUser[MAXLEN_PWD];      //本会控用户名

protected:
	enum 
	{ 
		STATE_IDLE,
		STATE_NORMAL,
		STATE_INVALID
	};

	CConfId cCurConfId;
	
	BOOL32 SendMsgToMcs( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0, BOOL32 bMsFilter = TRUE );
    void AddrEntryOut2In(CAddrEntry *pcAddrIn, TMcuAddrEntry *ptAddrOut, u8 byNum=1);
    void AddrGroupOut2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroup *ptGroupOut, u8 byNum=1);
	void AddrEntryV1Out2In(CAddrEntry *pcAddrIn, TMcuAddrEntryV1 *ptAddrOut, u8 byNum=1);
	void AddrGroupV1Out2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroupV1 *ptGroupOut, u8 byNum=1);

    void AddrEntryIn2Out(TMcuAddrEntry *ptAddrIn, TADDRENTRY *ptAddrOut);
    void AddrGroupIn2Out(TMcuAddrGroup *ptGroupIn, TADDRMULTISETENTRY *ptGroupOut);

    //N+1用户组和用户处理函数
    void ProcNPlusGrpInfo( const CMessage * pcMsg );
    void ProcNPlusUsrInfo( const CMessage * pcMsg );

public:
	//这里处理集成测试的用户部分
	void DaemonProcIntTestUserInfo( CMessage *pcMsg );
	//正常的业务处理	
	void ProcReoOverFlow( const CMessage * pcMsg );
	void ProcMcsMcuDaemonConfMsg( const CMessage * pcMsg );
	void ProcMcsMcuMsg( const CMessage * pcMsg );

    // 正常MCU回应,根据消息类型区分是否要根据用户组进行过滤
	void ProcMcuMcsMsg( const CMessage * pcMsg, BOOL32 bCheckGrp );
	void ProcMcsMcuStopPlayReq( const CMessage * pcMsg );
	void ProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg );
	void ProcMcsMcuGetAllMtStatusReq( const CMessage * pcMsg );
	void ProcMcsDisconnect( const CMessage * pcMsg );
	void ProcMcsMcuConnectReq( const CMessage * pcMsg );
	void ProcMcsInquiryMcuTime( const CMessage * pcMsg );
    void ProcMcsMcuChgSysTime( const CMessage * pcMsg );
	void ProcUserManagerMsg( const CMessage * pcMsg );
    void ProcAddrBookMsg( const CMessage * pcMsg );
    void ProcMcuCfgMsg( const CMessage *const pcMsg);    
    void ProcMcuCfgRsp( const CMessage * pcMsg);
    void ProcMcuCfgFileMsg( const CMessage * pcMsg);    // 处理配置文件上下载
    void ProcMcsMcuGetMsStatusReq( const CMessage * pcMsg );
	void ProcMcuUnitTestMsg( const CMessage * pcMsg );
    void DaemonProcNPlusUsrGrpNotif( const CMessage * pcMsg );
    
    void ProcMcsNPlusReq( const CMessage * pcMsg );
    void ProcNPlusMcsRsp( const CMessage * pcMsg );

    void DaemonProcPowerOn( void );
	void DaemonProcAddrInitialized( const CMessage * pcMsg );	
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
	void InstanceEntry( CMessage * const pcMsg );

    //会控远程重启
    void McsMcuRebootCmd( const CMessage * const pcMsg );

    // 删除用户
    void ProcMcuDelUser( CMessage * const pcMsg );
	void InstanceDump( u32 param = 0 );
	//抓包相关
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	void ProcMcsMcuStartNetCapCMD(const CMessage* pcMsg);//mcs开始抓包
	
	void ProcMcsMcuStoptNetCapCmd(const CMessage* pcMsg);//mcs停止抓包通知
	
	void ProcMcsMcuGetNetCapStatusReq(const CMessage* pcMsg);//mcs获取当前抓包状态请求

	void NotifyAllMcsCurNetCapStatus();

	void ProcMcsMcuOpenDebugModeCmd( const CMessage* pcMsg );

	void ProcMcsMcuStopDebugModeCmd( const CMessage* pcMsg );

	void NotifyAllMcsCurDebugMode();

	void ProcMcsMcuGetDebugModeReq(const CMessage* pcMsg);
#endif

	CMcsSsnInst();
	virtual ~CMcsSsnInst();
};

class CMcsSsn
{
public:
    // 各个会控实例用户所属的组Id。下标=实例号-1 
    static u8 m_abyUsrGrp[MAXNUM_MCU_MC + MAXNUM_MCU_VC];

public:
    // 设置McSsn的用户组
    static void SetUserGroup( u16 byInsId, u8 byGrpId ) ;
    // 获取McSsn的用户组
    static u8   GetUserGroup( u16 byInsId );    
    // 检验登陆
    static BOOL32 CheckLoginValid( CUsrManage& cUsrManageObj, CLoginRequest *pcUsr, u8 &byUserRole );


public:
	static BOOL32 SendMsgToMcsSsn( u16 wInstId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	static void   BroadcastToAllMcsSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

    // 广播消息到特定的用户组。
    // 业务调用这个接口发送的消息，一般都是无法在McSsn会话中区分用户组别的消息（消息头不带组信息），
    // 或者是每个用户组特异的消息
    // 普通会议消息只需要 BroadcastToAllMcsSsn，再由McSsn根据用户组信息过滤即可
    void Broadcast2SpecGrpMcsSsn( u8 byGrdIp, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	void SetCurrentDebugMode( emDebugMode emMode );
	emDebugMode GetCurrentDebugMode( void );

	void SetCurrentDebugModeInsId( u8 byInsId );
	u8 GetCurrentDebugModeInsId( void );

	CMcsSsn();
	virtual ~CMcsSsn();
private:
	u8 m_byCurrendDebugMode;
	u8 m_byCurrendDebugInsId;
};

typedef zTemplate< CMcsSsnInst, MAXNUM_MCU_MC + MAXNUM_MCU_VC, CMcsSsn > CMcsSsnApp;

extern CMcsSsnApp	g_cMcsSsnApp;	//会议控制台会话应用实例
extern CUsrManage   g_cUsrManage;
extern CAddrBook    *g_pcAddrBook;

#endif //_MCSSSN_H_
