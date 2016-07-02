/*****************************************************************************
模块名      : Board Agent
文件名      : isslavesys.h
相关文件    : isslavesys.cpp
文件实现功能: IS从系统（IS2.2 8313）业务
作者        : liaokang
版本        : V4R7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/05/05  1.0         liaokang      创建
******************************************************************************/
#ifndef _ISSLAVESYS_H_
#define _ISSLAVESYS_H_

#include "osp.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "kdvsys.h"
#include "kdvlog.h"
#include "kdvtype.h"
#include "mcuver.h"

#ifdef _LINUX_
#ifdef _LINUX12_
    #include "brdwrapper.h"
    #include "brdwrapperdef.h"
    #include "nipwrapper.h"
    #include "nipwrapperdef.h"
#else
    #include "boardwrapper.h"
#endif
#endif

#ifdef WIN32
    #include "winbrdwrapper.h"
#endif

// log打印等级定义，对应kdvlog
#define	LOG_OFF				            (u8)0	// 关闭打印
#define	LOG_ERROR				        (u8)1	// 程序运行错误(逻辑或业务出错),必须输出
#define	LOG_WARNING			            (u8)2	// 告警信息, 可能正确, 也可能错误
#define	LOG_KEYSTATUS			        (u8)3	// 程序运行到一个关键状态时的信息输出
#define	LOG_DETAIL				        (u8)4	// 普通信息, 最好不要写进log文件中

// brdcfg.ini begin
#define SECTION_BoardMasterSystem	    (s8*)"BoardMasterSystem"
#define KEY_BrdMasterSysInnerIp	        (s8*)"BrdMasterSysInnerIp"
#define KEY_BrdMasterSysListenPort	    (s8*)"BrdMasterSysListenPort"
#define DEFVALUE_BrdMasterSysListenPort BRDMASTERSYS_LISTEN_PORT   // 主系统（IS2.2 8548）默认监听端口

#define SECTION_IpConfig			    (s8*)"IpConfig"
#define KEY_IsSupportIpConfig           (s8*)"IsSupportIpConfig"
#define DEFVALUE_IsSupportIpConfig		(u8)0           // 默认 不支持
#define KEY_EthChoice				    (s8*)"EthChoice"
#define DEFVALUE_EthChoice			    (u8)1           // 默认 后网口
#define KEY_BrdSlaveSysIp	            (s8*)"BrdSlaveSysIp"
#define KEY_BrdSlaveSysInnerIp	        (s8*)"BrdSlaveSysInnerIp"
#define KEY_BrdSlaveSysIPMask	        (s8*)"BrdSlaveSysIPMask"
#define KEY_BrdSlaveSysDefGateway	    (s8*)"BrdSlaveSysDefGateway"
// brdcfg.ini end

// default param
// time out
#define CONNECT_BRDMASTERSYS_TIMEOUT    (u32)2000     //连接间隔 2000ms
#define REGISTER_BRDMASTERSYS_TIMEOUT   (u32)2000     //注册间隔 2000ms
#define SCAN_BRDSLAVESYS_STATE_TIMEOUT  (u32)5000     //扫描间隔 5000ms
// heart beat check param
#define HEART_BEAT_TIME                 (u8)10        // osp断链检测参数 Time
#define HEART_BEAT_NUM                  (u8)3         // osp断链检测参数 Num

// kdvlog config file
#if defined( WIN32 ) || defined(_WIN32)
    #define BRDSLAVESYS_KDVLOG_FILE	    (s8*)"./conf/kdvlog_isslavesys.ini"
#else
    #define BRDSLAVESYS_KDVLOG_FILE	    (s8*)"/usr/etc/config/conf/kdvlog_isslavesys.ini"
#endif

// IS从系统
class CBrdSlaveSysInst:public CInstance
{
protected:
    enum 
    {
        STATE_IDLE,			//空闲
        STATE_INIT,			//初始化
        STATE_NORMAL,		//工作
	};

public:
	CBrdSlaveSysInst();
	~CBrdSlaveSysInst();

protected:
	// 实例消息处理入口函数，必须override
	void InstanceEntry(CMessage* const pcMsg);
    // 清理实例
    void ClearInst();

    // 启动
	void ProcBrdSlaveSysPowerOn( CMessage* const pcMsg ) ;
    // 连接IS主系统（IS2.2 8548）
    void ProcConnectBrdMasterSysTimeOut();
    // 注册IS主系统（IS2.2 8548）响应
    void ProcRegToBrdMasterSysRsp( CMessage* const pcMsg );
    // 注册IS主系统（IS2.2 8548）TimeOut
    void ProcRegToBrdMasterSysTimeOut( );
    
    // 定时扫描
    void ProcBrdSlaveSysStateScan(void);

    // 升级
    void ProcBrdSlaveSysUpdateSoftwareCmd( CMessage* const pcMsg );
    // 重启动
    void ProcBrdSlaveSysResetCmd( CMessage* const pcMsg );
    // 断链处理
    void ProcDisconnect( void );

    // 发送消息至IS主系统（IS2.2 8548）
    void PostMsgToBrdMasterSys( u16 wEvent, void * pMsg = NULL, u16 wLen = 0 );

private:
    TBrdLedState  m_tBrdSlaveSysLedState;    // 从系统（IS2.2 8313）led状态   
    SWEthInfo     m_tBrdSlaveSysEthPortState;// 从系统（IS2.2 8313）网口状态
    u32           m_dwBrdMasterSysNode;      // 主系统（IS2.2 8548）node id
};

class CBrdSlaveSysCfg
{
public:
    CBrdSlaveSysCfg();
    virtual ~CBrdSlaveSysCfg();
public:
    // 依配置文件初始化本地配置信息
    BOOL32  ReadConnectConfigInfo();
    // 初始化本地配置信息
    BOOL32  InitLocalCfgInfoByCfgFile();
    // 获取主系统（IS2.2 8548） secondary ip
    u32     GetBrdMasterSysInnerIP();
    // 获取主系统（IS2.2 8548） Listen Port
    u16     GetBrdMasterSysListenPort();
private:
    u32           m_dwBrdMasterSysInnerIP;    // 内部通信 ip（second ip）
    u16           m_wBrdMasterSysListenPort;  // 主系统（IS2.2 8548） Listen Port
};

typedef zTemplate< CBrdSlaveSysInst, 1, CBrdSlaveSysCfg > CBrdSlaveSysApp;
extern CBrdSlaveSysApp	g_cBrdSlaveSysApp;

void IsSlaveSysLog( u8 byPrintLvl, s8* pszFmt, ... );
void staticlog( LPCSTR lpszFmt, ...);
API void isslavesysver(void);     // 版本打印
API void isslavesyshelp( void );   // 帮助信息
API void isslavesysinit(void);    // IS22Mpc8313初始化
API void quit(void);              // quit
void AddEventStr();            // 添加消息字符串

#endif /* _ISSLAVESYS_H_ */
/* end of file isslavesys.h */