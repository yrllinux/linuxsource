#ifndef _VCSSSN_H_
#define _VCSSSN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcsssn.h"


class CVcsSsnInst : public CMcsSsnInst  
{
public:
	CVcsSsnInst( void );
	virtual ~CVcsSsnInst( void );

public:
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );	
	void VCSDaemonProcPowerOn( void );

	// 用户管理模块处理函数
	void ProcVCSUserManagerMsg( const CMessage * pcMsg );
	// 处理VCS的会议模板消息
	void ProcVCSConfTemMsg( CMessage * const pcMsg );
	// 处理VCS直接分发给VC DAEMON的消息(含创会请求，组呼配置信息更新请求)
	void ProcVCSMcuVCDaemonMsg( const CMessage * pcMsg );
	// 处理VCS的调度请求
	void ProcVCSMcuVCMsg( const CMessage * pcMsg );
	// 处理VCS开始调度席操作请求
	void ProcVCSConfOprMsg( const CMessage * pcMsg );
	void ProcVCSSoftNameMsg( const CMessage * pcMsg );
	// 处理VCS关于MCU无需处理信息的设置及读取
	void ProcVCSMcuUnProcCfgMsg( const CMessage * pcMsg );
	
	// 所有VCS命令应答消息统一转发函数
	void ProcMcuVcsMsg( const CMessage * pcMsg );

	// 实现将管理模块获取到的用户信息与对应的任务信息打包
	void PackUserTaskInfo(u8 byUserNumInPack, u8 *pbyExUserBuf, u8 *pbyVCSUserBuf);

	// 调度员退出调度席处理
	void QuitCurVCConf( BOOL32 bNormalQuit );
	void StopToMcByMcIndex(u8 byMcIndex);

	// 在Daemon中处理延迟结会的timer
	void DaemonProcWaitReleaseTimeOut( const CMessage * const pcMsg );

};

class CVcsSsn 
{
public:
	CVcsSsn(void);
	virtual ~CVcsSsn(void);
	
public:
	// 检查指定的会议是否仍处于被控制状态
    BOOL ChkVCConfStatus( CConfId cConfId );
	// VCS释放指定会议
	void VCSRlsConf( CConfId cConfId );
	// 设置指定实例操作的会议ID号
	void SetCurConfID( u16 byInsId, CConfId& cConfID );
	// 获取指定实例操作的会议ID号
	CConfId GetCurConfID(u16 byInsId);
	// 发送消息给所用同组的VCS,VCS MCS组号可能出现相同,所以分两个函数区分处理，
    void Broadcast2SpecGrpVcsSsn( u8 byGrdIp, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	// 获取需要同步的VCSSSn信息
	BOOL32 GetVCSSSsnData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
    BOOL32 SetVCSSSsnData( u8 *pbyBuf, u32 dwBufLen );

	// 设置需要等待延迟结束的会议
	BOOL32 SetWaitReleaseConf( const CConfId &cConfId, u8 &byTimerIdx );
	// 判断某会议是否在等待延迟结束
	BOOL32 IsConfWaitRelease( const CConfId &cConfId );
	// 通过会议Id得到某个等待延迟结会的会议占用的TimerIdx
	BOOL32 GetTimerIdxByConfId( const CConfId &cConfId, u8 &byTimerIdx );
	// 通过TimerIdx得到某个等待延迟结会的会议Id
	BOOL32 GetConfIdByTimerIdx( const u8 byTimerIdx, CConfId &cConfId);
	// 将某会议从等待延迟结束中清除(通过会议Id 或 定时器索引来清)
	void CleanConfInWaitRelease( const CConfId &cConfId );
	

protected:
	// 指定VCS实例操作的会议ID号 下标=实例号-1 
	CConfId m_acCurConfID[MAXNUM_MCU_MC + MAXNUM_MCU_VC];
	// vcs异常断链后等待结束的会议
	CConfId m_acWaitReleaseConfId[MAXNUM_MCU_VC];
};

typedef zTemplate< CVcsSsnInst, MAXNUM_MCU_MC + MAXNUM_MCU_VC, CVcsSsn> CVcsSsnApp;

extern CVcsSsnApp g_cVcsSsnApp;
extern CUsrManage g_cVCSUsrManage;

#endif /*_VCSSSN_H_*/



