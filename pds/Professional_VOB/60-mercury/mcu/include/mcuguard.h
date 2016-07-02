  /*****************************************************************************
   模块名      : mcu_new
   文件名      : mcuguard.h
   相关文件    : mcuguard.cpp
   文件实现功能: MCU守卫应用类头文件
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
   2003/03/28  1.1         杨皞昀     修改
******************************************************************************/

// mcuguard.h: interface for the CMcuGuardInst class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MCUGUARD_H
#define __MCUGUARD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//定时核查间隔时间
#define TIMESPACE_GUARD_ONGOING_CHECK (u32)60000 //单位(毫秒)
//定时喂狗
#define TIMESPACE_FEEDDOG             (u32)10000  //单位(毫秒)

//定时任务检测最大失败次数
#define TIMESPACE_MAX_FAILED_TIMES    (u8)3  

//定时内存检测最小剩余内存尺寸
#define MEMSPACE_MIN_SIZE             (u32)(3*1024*1024)

//读License错误信息
#define READ_LICENSE_SUCCESS					"Read License success!"
#define READ_LICENSE_FILE_ERROR					"Read License file error!"
#define READ_LICENSE_VERSION_ERROR				"Read License version error!"
#define READ_LICENSE_TYPE_ERROR					"Read License type error!"
#define READ_LICENSE_KEY_ERROR					"Read License key error!"
#define READ_LICENSE_MCUDEVICEID_ERROR			"Read MCU device id error!"
#define READ_LICENSE_MCUTYPE_ERROR				"Read MCU type error!"
#define READ_LICENSE_MCUACCESSNUM_ERROR			"Read MCU access num error!"
#define READ_LICENSE_VCSACCESSNUM_ERROR			"Read VCS access num error!"
#define READ_LICENSE_MCUEXPIREDATA_ERROR		"Read MCU expire data error!"
#define READ_LICENSE_ILLEGALEBOARD_ERROR        "Illegal Running Board!"
//[2011/01/28 zhushz]PCmt error add
#define READ_LICENSE_PCMTACCESSNUM_ERROR        "Read PcMt access num error!"
#define READ_LICENSE_UNDEFINED_ERROR			"Undefined error!"

class CMcuGuardInst : public CInstance  
{
	enum
	{
		SHIFT_MCSSN_CHECK = 0,
		SHIFT_PERIEQPSSN_CHECK,
		SHIFT_MCUVC_CHECK,
		SHIFT_MCUCONFIG_CHECK,
		SHIFT_MPSSN_CHECK,
		SHIFT_MTADPSSN_CHECK,
        SHIFT_DCSSSN_CHECK,
        SHIFT_END_CHECK
	};
	//核查门限值
	enum
	{
		STATUS_NORMAL = 65,
		STATUS_BUSY = 75,
		STATUS_CONGESTION = 85,
		STATUS_ALLFULL = 95
	};
public:
	CMcuGuardInst();
	virtual ~CMcuGuardInst();

protected:
	void InstanceEntry( CMessage * const pcMsg );

	void ProcReoPowerOn(void);					//上电处理函数
	void ProcMemCheck(void);					//内存核查函数
	void ProcFileSystemCheck(void);				//文件系统核查函数
	void ProcTaskCheck(void);					//APP TASK 核查函数
	void ProcCpuStatusCheck(void);				//CPU 状态核查函数
	void ProcGuardTest(void);					//GUARD模块测试函数
	void ProcPowerOff(void);					//下电消息处理函数
	void ProcTaskTestAck( const CMessage * pcMsg );	//任务核查确认消息处理函数
	void ProcSyncSrcStateCheck(void);
	void ProcTimerFeedDog( void );	
	void ProcGetLicenseDataFromFile( void );
private:
	void ProcEachTaskCheck( u8 byShift );
	u8   GuardGetBrdNetSyncLock();
	void PingEveryTask();        //向每一个应用发送测试包
	void GetBoardSequenceNum( u8 *pBuf, s8* achSep );
    
	void GenEncodeData( u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect = TRUE );

	// zgc [2007/01/23] License版本更新
	BOOL32 GetOldLicenseDataFromFile( s8* pchPath );	//读取旧版本License
	BOOL32 GetCurrentLicenseDataFromFile( s8* pchPath );	//读取当前版本License

private:
	u32  m_dwGuardTestSeriNo;	 //核查消息序号
	u8   m_byCurSyncSourceState;
	u8   m_bySyncSourceMode;     //时钟源方式
	u8   m_byTaskState;          //所有应用的状态，每一位代表一个应用
        	                     //偏移量为SHIFT_**_CHECK
	u8   m_abyTaskBusyTimes[SHIFT_END_CHECK];
	
	BOOL32 m_bRebootMcu;         //是否需要重启MCU
	s8     achAppName[32];
};

// zgc [2007/01/26]
class CMcuGuardData
{
public:
	CMcuGuardData();
	virtual ~CMcuGuardData();

public:
	u8 GetLicenseErrorCode();	// 读Licnese错误码
	void SetLicenseErrorCode( u8 byErrorCode );	// 写Licnese错误码

    void SetLicenseSN(const s8* szSN);
    const s8* GetLicenseSN(void);

private:
	u8	m_byLicenseErrorCode;	// License错误码
    s8  m_szLicenseSN[64];      // 序列号

};

// [12/4/2011 liuxu] 用于纠正看门狗线程因不能及时收到被检测线程的回复而把该线程挂断的问题
class CGuardCheck
{
public:
	CGuardCheck() { Destroy(); }
	~CGuardCheck(){ Destroy(); }
	
public:
	BOOL32	Init( const u8 byMaxUnRsp );
	void	Destroy();
	
	// 被检测线程设置自己的标记位
	void	ClientSet();
	
	// Guard检测
	BOOL32	GuardCheck();
	
	// Guard判断Client是否无响应
	BOOL32	IsClientUnRsp() const { return m_byUnRspTimes >= m_byMaxUnRspTimes; }
	
	// Guard重设RspTimes
	BOOL32	ResetUnRspTimes( ) { m_byUnRspTimes = 0; }
	
private:
	enum EGCFlag
	{
		E_GC_FLAG_CHECK = 0,
			E_GC_FLAG_BUSY
	};
	
private:
	SEMHANDLE		m_hSem;					// 同步信号量
	u8				m_byFlag;				// 标记
	u8				m_byUnRspTimes;			// 被检测线程无响应次数
	u8				m_byMaxUnRspTimes;		// 最大无响应次数
};

API void mcuguarddisable(void);
API void mcuguardenable(void);
API void showlicense(void);

typedef zTemplate< CMcuGuardInst, 1, CMcuGuardData > CMcuGuardApp;

extern CMcuGuardApp	g_cMcuGuardApp;	//MCU守卫应用实例

#endif //__MCUGUARD_H
