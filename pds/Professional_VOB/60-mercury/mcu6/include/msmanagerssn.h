/*****************************************************************************
   模块名      : MCU主控热备份的通讯及实现会话模块
   文件名      : msmanagerssn.h
   相关文件    : msmanagerssn.cpp
   文件实现功能: MCU主控热备份的通讯及实现会话模块定义
   作者        : 万春雷
   版本        : V4.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/18  4.0         万春雷        创建
******************************************************************************/

#ifndef __MSMANAGERSSN_H_
#define __MSMANAGERSSN_H_

#include "osp.h"
#include "agentinterface.h"
#include "kdvlog.h"


//...............................................................................................

enum emMCUMSType
{ 
	MCU_MSTYPE_INVALID = 0,   //无效板
	MCU_MSTYPE_SLAVE,         //备位板    mc0	
	MCU_MSTYPE_MASTER,        //主位板    mc1
};

enum emMCUMSState
{ 
	MCU_MSSTATE_OFFLINE = 0,  //空闲
	MCU_MSSTATE_STANDBY,      //备用状态
	MCU_MSSTATE_ACTIVE,       //主用状态
};

#define CONNECT_MASTERMCU_TIMEOUT   (u32)2000     //连接间隔 2″
#define REGISTER_MASTERMCU_TIMEOUT  (u32)2000     //注册间隔 2″

#define MS_CEHCK_MSSTATE_TIMEOUT    (u32)200      //vxworks下扫描硬件主备用状态的检测间隔 0.2″

#define MS_CHECK_MSSTATE_TIMES      (u8)3         //vxworks下扫描硬件主备用状态的出现状态变更次数,3次以上进行切换

#define MS_INIT_LOCK_SPAN_TIMEOUT   (u32)60000    //主备模块初始化锁定超时间隔 60″

#define MS_SYN_LOCK_SPAN_TIMEOUT    (u32)60000    //主备数据倒换时 主备模块与业务模块的竞争锁定超时间隔 60″

#define MS_WAITFOTRSP_TIMEOUT       (u32)5000     //等待对端请求应答超时间隔 5″

#define MS_SYN_NOW_SPAN_TIMEOUT     (u32)100      //主备倒换主备协商后即刻开始同步时间间隔 0.1″
#define MS_SYN_MIN_SPAN_TIMEOUT     (u32)10000    //主备倒换对端需进行完全同步，恢复性定时同步时间间隔 10″

#define MS_DEBUG_CRITICAL           (u8)LOG_LVL_ERROR
#define MS_DEBUG_ERROR              (u8)LOG_LVL_ERROR
#define MS_DEBUG_WARNING            (u8)LOG_LVL_WARNING
#define MS_DEBUG_INFO               (u8)LOG_LVL_KEYSTATUS
#define MS_DEBUG_VERBOSE            (u8)LOG_LVL_DETAIL

//...............................................................................................

//主备数据倒换间的通讯协议的数据结构
#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#else
#endif


#define MS_MAX_PACKET_LEN    (u32)(20*1024)     //主备倒换时整帧切包后的单包最大的消息体长度，超过此长度则进行切分处理 20 Kbytes
#define MS_MAX_FRAME_LEN     (u32)(3600*1024)   //主备倒换时整帧切包前的最大消息体长度 3600 Kbytes
#define MS_MAX_PACKET_NUM    (u8)(MS_MAX_FRAME_LEN/MS_MAX_PACKET_LEN)


#define MS_MAX_STRING_LEN    255


//主控模块启动参数
struct TMSStartParam
{
	u8     m_byMcuType;
	u16    m_wMcuListenPort;
	BOOL32 m_bMSDetermineType; //vxworks下硬件平台支持主备用功能，则直接由os获取主备用结果，默认不支持
};


//外设实际连接状态
struct TMSVcPeriEqpState
{
	u8 m_abyOnline[MAXNUM_MCU_PERIEQP]; //外设号减1为数组下标, 1-online
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MC实际连接状态
struct TMSVcMCState
{
	u8 m_abyOnline[MAXNUM_MCU_MC + MAXNUM_MCU_VC]; //会控实例号减1为数组下标, 1-online
	TMcsRegInfo m_tMcsRegInfo[MAXNUM_MCU_MC + MAXNUM_MCU_VC];
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MP实际连接状态
struct TMSVcMpState
{
	u8 m_abyOnline[MAXNUM_DRI]; //设备号减1为数组下标, 1-online
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MTADP实际连接状态
struct TMSVcMtAdpState
{
	u8 m_abyOnline[MAXNUM_DRI]; //设备号减1为数组下标, 1-online
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

struct TMSVcDcsState
{
    u8 m_abyOnline[MAXNUM_MCU_DCS];
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//同步前的本端外部模块实际连接状态
typedef struct tagMSSynEnvState
{
	TMSVcPeriEqpState m_tEqpState;
	TMSVcMCState      m_tMCState;
	TMSVcMpState      m_tMpState;
	TMSVcMtAdpState   m_tMtAdpState;
    // guzh [9/14/2006] DCS
    TMSVcDcsState     m_tDcsState;
}TMSSynEnvState
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//同步请求时的消息体：本端环境状态，协商是否处于同一稳态下
typedef struct tagMSSynInfoReq
{
	u8 m_byInited;                //同步前的本端主备模块初始化状态 1-Inited
	TMSSynEnvState m_tEnvState;   //同步前的本端外部模块状态
}TMSSynInfoReq
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//同步请求回应时的消息体：
typedef struct tagMSSynInfoRsp
{
	u8 m_byInited;                //同步前的本端主备模块初始化状态 1-Inited
}TMSSynInfoRsp
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//待同步的数据类型 - 对应 TMSReqHead::m_byMsgID
enum emgMSSynDataType
{
	emSynDataStart = 0,           // 数据同步起始点
	//emConfigDataStart,          // 配置文件数据同步点
	emSysTime,
	emSysCritData,                // guzh [6/12/2007] 系统重要数据
	emCfgFile,
	emDebugFile,
    emAddrbookFile_Utf8,
	emAddrbookFile,
	emConfinfoFile,
	emUnProcConfinfoFile,         // fxh VCS分组信息
	emLoguserFile,
	emVCSLoguserFile,             // fxh VCS用户信息
	emVCSUserTaskFile,            // fxh VCS用户任务分配信息
    emUserExFile,                 // 扩展用户组信息
	//emConfigDataEnd,

	//emVcDataStart,              //业务数据同步点
	emVcDeamonPeriEqpData,        //业务公用区内存数据同步点
	emVcDeamonMCData,
	emVcDeamonMpData,
	emVcDeamonMtadpData,          //
	emVcDeamonTemplateData,
	emVcDeamonOtherData,
	emAllVcInstState,
	emOneVcInstConfMtTableData,   //业务指定实例区内存数据同步点
	emOneVcInstConfSwitchTableData,
	emOneVcInstConfOtherMcuTableData,
	emOneVcInstOtherData,
	//emVcDataEnd
	emSynDataEnd                  // 数据同步结束点
};

//response消息的返回值字段
enum emMSReturnValue
{
    emMSReturnValue_Ok = 0,   //request消息已得到正确的处理
	emMSReturnValue_Error,    //一般性错误，错误原因不明
	emMSReturnValue_Invalid   //request无效,不会被处理
};

typedef struct tagMSFrmHead
{
	u32 m_dwFrmLen;   //整帧大小（主机序）
	u16 m_wFrmSN;     //数据帧流水号（主机序）
	u8  m_byFrmType;  //待同步的数据类型 - 对应 TMSReqHead::m_byMsgID
	u8 *m_pbyFrmBuf;  //整帧缓冲指针，发送缓冲将在前面预留sizeof(TMSReqHead)
}TMSFrmHead
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//request: 同步数据请求时的切包后的单包消息头：
typedef struct tagMSSynDataReqHead
{
	u32 m_dwFrmLen;   //数据帧大小（网络序）
	u16 m_wFrmSN;     //数据帧流水号（网络序）
	u8  m_byMsgID;    //自定义消息/数据类型 ID
	u8  m_byPackIdx;  //单包在整帧的索引 (以0开始)
	u8  m_byMark;     //单包是否为结束包
}TMSSynDataReqHead
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//消息头
//response: 同步数据组帧完整后的回应时的消息体：
typedef struct tagMSSynDataRsp
{
	u32 m_dwFrmLen;   //数据帧大小（网络序）
	u16 m_wFrmSN;     //数据帧流水号（网络序）
	u8  m_byMsgID;    //自定义消息/数据类型 ID
	u8  m_byRetVal;   //request消息处理结果
}TMSSynDataRsp
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//需同步有效MC消息体：
struct TMSVcMCStateHead
{
	u8 m_abyValid[MAXNUM_MCU_MC + MAXNUM_MCU_VC]; //会控实例号减1为数组下标, 0,1 1-Valid
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//需同步有效EQP消息体：
struct TMSVcPeriEqpStateHead
{
	u8 m_abyValid[MAXNUM_MCU_PERIEQP]; //外设号减1为数组下标, 0,1 1-Valid 包括离线外设
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

struct TMSVcConfState
{
	u8  m_abyValid[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];      //0,1 1-Valid	
	u32 m_awConfDataLen[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1]; //会议数据长度，网络序
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcTemplateState
{
	u8 m_abyValid[MAXNUM_MCU_TEMPLATE]; //0,1 1-Valid
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcInstState
{
	u8 m_abyState[MAXNUM_MCU_CONF]; //STATE_IDLE STATE_SCHEDULED STATE_ONGOING
	u8 m_abyConfIdx[MAXNUM_MCU_CONF];//会议的m_byConfIdx
	CConfId m_acConfId[MAXNUM_MCU_CONF];//会议的confid
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcInstDataHead
{
	u8 m_byInstID;       //会议实例ID [1, MAXNUM_MCU_CONF]
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSVcOtherMcuState
{
	u8 m_abyOnline[MAXNUM_CONFSUB_MCU]; //1-online
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMSCurSynState
{
	u8 m_byInstID;                //0－表示文件或公用数据区数据；[1, MAXNUM_MCU_CONF]－表示某会议实例区数据
	emgMSSynDataType m_emSynType; //待同步的数据类型
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

//...............................................................................................

//ms-mcu mutex
typedef struct tagMCULock
{
	tagMCULock()
    {
        m_bRet = FALSE;
		m_hMutex = NULL;
    }
	
    BOOL32 SetLock( SEMHANDLE hMutex )
	{		
		if( NULL == hMutex )
		{
			m_bRet = FALSE;
			return m_bRet;
		}	
        m_bRet = TRUE;
		m_hMutex = hMutex;
		m_bRet = OspSemTakeByTime(m_hMutex, 3000);

		return m_bRet;
	}
	
	~tagMCULock()
	{
        if( TRUE == m_bRet )
        {
            if( NULL != m_hMutex )
			{
				OspSemGive(m_hMutex);
			}
        }
	}

protected:
	BOOL32 m_bRet;
	SEMHANDLE m_hMutex;
	
}TMCULock;

//...............................................................................................


class CMSManagerSsnInst : public CInstance  
{
	enum 
	{ 
		MS_STATE_IDLE = 0,    //空闲
		MS_STATE_INIT,        //初始化
		MS_STATE_NORMAL,      //工作
	};
		
public:
	CMSManagerSsnInst();
	virtual ~CMSManagerSsnInst();
		
protected:
	//普通实例入口
	void InstanceEntry( CMessage * const pcMsg );
	
	void ProcPowerOnCmd( void );
	void ClearInst( void );
	
	//向另一块MCU发送消息（消息体不切分直接投递）
	BOOL32 PostMsgToOtherMcuDaemon(u16 wEvent, u8 *const pbyMsg, u16 wLen);
	BOOL32 PostMsgToOtherMcuInst(u16 wEvent, u8 *const pbyMsg, u16 wLen);

	//MCU之间 断链通知
	void ProcMcuMcuDisconnectNtf( void );	

	//备位板MCU向主位板MCU 的注册请求
	void ProcMcuMcuRegisterReq( CMessage * const pcMsg );
	void ProcMcuMcuRegisterRsp( CMessage * const pcMsg );
	
	//MCU之间 对端MCU的主备位类型及当前的主备使用状态通知，进行主备用协商
	void ProcMcuMcuMSDetermine( CMessage * const pcMsg );
	//MCU之间 主备用协商结果通知
	void ProcMcuMcuMSResult( CMessage * const pcMsg );

	//处理主备用板MCU之间 的数据倒换定时器
	void ProcMcuMcuSynTimeOut( void );

	//处理主备用板MCU之间 等待对端的请求应答超时定时器
	void ProcMcuMcuWaitforRspTimeOut( CMessage * const pcMsg );

	//处理主备用板MCU之间 开始数据倒换 交互
	void ProcMcuMcuStartSynReq( CMessage * const pcMsg );
	void ProcMcuMcuStartSynRsp( CMessage * const pcMsg );
	//处理主用板MCU向备用板MCU 结束数据倒换 通知
	void ProcMcuMcuEndSynNtf( void );

	//处理主备用板MCU之间 实际切包组包的数据倒换 交互
	void ProcMcuMcuSynningDataReq( CMessage * const pcMsg );
	void ProcMcuMcuSynningDataRsp( CMessage * const pcMsg );
    
    void ProcMcuMcuProbeReq( void );
    void ProcMcuMcuProbeAck( void );

    void PrintMSVerbose( s8*  pszFmt, ... );
	void PrintMSInfo( s8*  pszFmt, ... );
	void PrintMSWarning( s8*  pszFmt, ... );
	void PrintMSException( s8*  pszFmt, ... );
	void PrintMSCritical( s8*  pszFmt, ... );
	void MSManagerPrint( u8 byLevel, s8 *pszPrintStr );
	
protected:
	void ProcMcuMcuConnectTimeOut( void );
	void ProcMcuMcuRegisterTimeOut( void );
	void ProcMcuMcuCheckMSStateTimeOut( void );

	//备位板MCU连接主位板mcu请求
	BOOL32 ConnectMasterMcu( void );

protected:
	//向另一块MCU-Inst发送消息（消息体切分投递）
	BOOL32 SendCustomMsgToOtherMcu(TMSFrmHead *ptSndFrmHead);

	//接收另一块MCU-Inst的消息（消息体完整重组）
	TMSFrmHead *RecvCustomMsgFromOtherMcu( CMessage * const pcMsg );
	
	//处理数据倒换时的 一条组包完整的自定义主备同步消息
	BOOL32 DealOneCustomSynMsg( TMSFrmHead *ptMSFrmHead );
	
protected:
	//MCU之间 数据倒换
	BOOL32 SendSynData( BOOL32 bStart = FALSE );
	BOOL32 SendSynData( TMSCurSynState tMSCurSynState );
	
	BOOL32 SendSynDataOfSysTime( void );
	BOOL32 SendSynDataOfSysCritData( void );
	BOOL32 SendSynDataOfCfgFile( void );
	BOOL32 SendSynDataOfDebugFile( void );
    BOOL32 SendSynDataOfAddrbookFileUtf8( void );
	BOOL32 SendSynDataOfAddrbookFile( void );
	BOOL32 SendSynDataOfConfinfoFile( void );
	BOOL32 SendSynDataOfLoguserFile( void ); //
    BOOL32 SendSynDataOfUserExFile( void ); // 扩展用户组信息
	BOOL32 SendSynDataOfUnProcConfinfoFile( void );
    BOOL32 SendSynDataOfVCSLoguserFile( void );
    BOOL32 SendSynDataOfVCSUserTaskFile( void );
	BOOL32 SendSynDataOfVcDeamonPeriEqpData( void );
	BOOL32 SendSynDataOfVcDeamonMCData( void );
	BOOL32 SendSynDataOfVcDeamonMpData( void );
	BOOL32 SendSynDataOfVcDeamonMtadpData( void );
	BOOL32 SendSynDataOfVcDeamonTemplateData( void );
	BOOL32 SendSynDataOfVcDeamonOtherData( void );
	BOOL32 SendSynDataOfAllVcInstState( void );//
	BOOL32 SendSynDataOfOneVcInstConfMtTableData( u8 byInstID );
	BOOL32 SendSynDataOfOneVcInstConfSwitchTableData( u8 byInstID );
	BOOL32 SendSynDataOfOneVcInstConfOtherMcTableData( u8 byInstID );
	BOOL32 SendSynDataOfOneVcInstOtherData( u8 byInstID );
		
	BOOL32 SaveSynDataOfSysTime( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfSysCritData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfCfgFile( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfDebugFile( u8 *pbyBuf, u32 dwBufLen );
    BOOL32 SaveSynDataOfAddrbookFileUtf8( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfAddrbookFile( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfConfinfoFile( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfLoguserFile( u8 *pbyBuf, u32 dwBufLen ); //
    BOOL32 SaveSynDataOfUserExFile( u8 *pbyBuf, u32 dwBufLen ); // 扩展用户组信息
	BOOL32 SaveSynDataOfVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonMCData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonMpData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonMtadpData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonTemplateData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfVcDeamonOtherData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfAllVcInstState( u8 *pbyBuf, u32 dwBufLen );//
	BOOL32 SaveSynDataOfOneVcInstConfMtTableData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfOneVcInstConfSwitchTableData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfOneVcInstConfOtherMcTableData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfOneVcInstOtherData( u8 *pbyBuf, u32 dwBufLen );
	BOOL32 SaveSynDataOfUnProcConfinfoFile( u8 *pbyBuf, u32 dwBufLen );
    BOOL32 SaveSynDataOfVCSLoguserFile( u8 *pbyBuf, u32 dwBufLen );
    BOOL32 SaveSynDataOfVCSUserTaskFile( u8 *pbyBuf, u32 dwBufLen );
	
private:	
	u32    m_dwMcuNode;         //另一块Mcu对应节点号
	u32    m_dwMcuAppIId;       //注册该Mcu板的Mcu实例APP号

	u32    m_dwRcvFrmLen;       //正在接收的数据帧已接收长度
	TMSFrmHead m_tSndFrmHead;   //待发送的数据缓冲区
	TMSFrmHead m_tRcvFrmHead;   //正在接收的数据缓冲区

	TMSCurSynState m_tMSCurSynState; //当前发送出去的同步数据的状态 SnedReq 后等待相应Ack
	                                 //若请求超时检测机制：尝试重传2次，仍失败等待下次同步
		
	BOOL32 m_bRemoteInited;          //对端是否已经完成初始化
	u8     m_byCheckTimes;           //vxworks下扫描硬件主备用状态的出现状态变更次数,3次以上进行切换
};


//..............................................................
//主备数据倒换间的通讯协议的数据处理
class CMSManagerConfig
{
	/*lint -save -sem(CMSManagerConfig::FreeMSConfig,cleanup)*/
public:
	CMSManagerConfig();
	virtual ~CMSManagerConfig();

public:
	BOOL32 InitMSConfig(TMSStartParam tMSStartParam);
	void   FreeMSConfig(void);

    void   SetRemoteMpcConnected(BOOL32 bConnect);
    BOOL32 IsRemoteMpcConnected() const;

	BOOL32 InitLocalCfgInfo(u8 byLocalMcuType);

    void   SetMpcOusLed(BOOL32 bOpen);

	u32    GetLocalMcuIP(void);
	void   SetLocalMcuIP(u32 dwLocalMcuIP);
	u32    GetAnotherMcuIP(void);
	void   SetAnotherMcuIP(u32 dwAnotherMcuIP);
	u16    GetMcuConnectPort(void);
	void   SetMcuConnectPort(u16 wConnectPort);
	u8     GetLocalMcuType(void);
	void   SetLocalMcuType(u8 byLocalMcuType);
	BOOL32 IsDoubleLink(void);
	void   SetDoubleLink(BOOL32 bDoubleLink);	
	emMCUMSType  GetLocalMSType(void);
	void   SetLocalMSType(emMCUMSType emLocalMSType);
	emMCUMSState GetCurMSState(void);
	void   SetCurMSState(emMCUMSState emCurMSState);
    TMSSynState  GetCurMSSynState(void);
    void   SetCurMSSynState(TMSSynState &tMSSynState);

	BOOL32 DetermineMSlave(emMCUMSType emLocalMSType, emMCUMSState &emLocalMSState, 
		                   emMCUMSType emRemoteMSType, emMCUMSState &emRemoteMSState);

	BOOL32 IsMSConfigInited(void);

	void   SetMSSynOKFlag(BOOL32 bSynOK);
	BOOL32 IsMSSynOK(void);

    void   SetMsSwitchOK(BOOL32 bSwitchOK);
    BOOL32 IsMsSwitchOK(void);

	BOOL32 JudgeSndMsgPass(void);
	BOOL32 JudgeRcvMsgPass(void);

	BOOL32 EnterMSInitLock(void);
	void   LeaveMSInitLock(void);
	
	void   EnterMSSynLock( u16 wAppId );
	void   LeaveMSSynLock( u16 wAppId );

	u8     GetDebugLevel(void);
	void   SetDebugLevel(u8 byDebugLevel);

	BOOL32 GetMSDetermineType(void);
	void   SetMSDetermineType(BOOL32 bMSDetermineType);

    void   IncDitheringTimes(void);
    u32    GetDitheringTimes(void);

	void   RebootMCU(void);

	// guzh [6/12/2007] 增加系统会话校验值
    void   SetMSSsrc(u32 dwSSrc);   // 内部调用，外部请勿使用
    u32    GetMSSsrc(void) const; 

	//记录主板会控连接状态, zgc, 2008-03-25
	void   SetMSVcMCState( const TMSVcMCState &tMSVcMCState )
	{
		memcpy( &m_tMSVcMCState, &tMSVcMCState, sizeof(TMSVcMCState) );
	}
	void   GetMSVcMCState( TMSVcMCState &tMSVcMCState ) const
	{
		memcpy( &tMSVcMCState, &m_tMSVcMCState, sizeof(TMSVcMCState) );
	}

public:
	BOOL32 SetLocalMcuTime(time_t tTime);
	BOOL32 IsActiveBoard(void);

private:
	BOOL32 GetSlotThroughName(const s8* pszName, u8* pbySlot);
	BOOL32 GetTypeThroughName(const s8* pszName, u8* pbyType);
	
private:
	u32     m_dwLocalMcuIP;         //本机mcu的ip 网络序
	u32     m_dwRemoteMcuIP;        //另一块mcu的ip 网络序
	u16     m_wMcuListenPort;       //另一块mcu的port 主机序
	
	u8      m_byLocalMcuType;       //当前MCU类型 -- 8000A、8000B、WIN32
	BOOL32  m_bDoubleLink;          //Mcu Linking mode ( TRUE-DoubleLink, FALSE-SingleLink )
	BOOL32  m_bInited;              //是否已经完成初始化
    BOOL32  m_bRemoteConnected;     //对端是否已经连接(RegistAck)
	
	SEMHANDLE m_hSemInitLock;       //主备模块初始化同步锁定的互斥信号量
	
	//主备数据倒换时 主备模块与业务模块的竞争锁定
	SEMHANDLE m_hSemSynLock;        //主备模块与业务模块的竞争 锁定模块的互斥信号量
	u16       m_wLockedAppId;       //当前锁定的App应用ID

	//以下变量读写需同步保护
	emMCUMSType  m_emLocalMSType;   //本机mcu的主备位类型 -- 主位板、备位板、无效板
	emMCUMSState m_emCurMSState;    //当前MCU主备用状态 -- 主用、备用、空闲
                                    //主用板完成相应的对外业务，备用板只接收消息而不对外响应
	BOOL32       m_bSynOK;          //是否已完成同步操作 FALSE-备用板未完成同步则丢弃接受到的消息，防止误判

    BOOL32       m_bMsSwitchOK;     //主备切换是否成功（若主备切换时数据还没来得及同步完，则主备切换失败）

	//SEMHANDLE m_hSemDataLock;     //主备模块App与业务模块数据竞争同步锁定的互斥信号量
	//BOOL32    m_bDataLocked;      //是否已经进入数据竞争同步锁定

	u8          m_byDebugLevel;     //调试打印等级

	BOOL32      m_bMSDetermineType; //TRUE－采用定时器获取硬件主备用状态；FALSE－采用Osp消息协商主备用状态
    
    u32         m_dwDitheringTimes; //统计主备切换时硬件抖动次数
    
    TMSSynState m_tMSSynState;      //主备失败状态记录

	u32     m_dwSysSSrc;            //本次启动系统的SSRC值，提供给各个外设会话
	TMSVcMCState m_tMSVcMCState;	//记录主板会控连接状态, zgc, 2008-03-25
};


typedef zTemplate< CMSManagerSsnInst, 1, CMSManagerConfig, sizeof( u8 ) > CMSManagerSsnApp;

/*=============================================================================
    函 数 名： MSGBODY_LEN_EQ
    功    能： check if body len of CServMsg is equal to len
    算法实现： 
    全局变量： 
    参    数： CServMsg &cServMsg
               u16 wLen
    返 回 值： inline BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/08/18  4.0			万春雷                  创建
=============================================================================*/
inline BOOL32 MSGBODY_LEN_EQ( CServMsg &cServMsg, u16 wLen )
{
	if(cServMsg.GetMsgBodyLen() != wLen)
	{
		const s8* pszMsgStr = OspEventDesc(cServMsg.GetEventId());
		return FALSE;
	}
	
	return TRUE;
}

/*=============================================================================
    函 数 名： MSGBODY_LEN_GE
    功    能： check if the body len of CServMsg >= len
    算法实现： 
    全局变量： 
    参    数： CServMsg &cServMsg
               u16 wLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/08/18  4.0			万春雷                  创建
=============================================================================*/
inline BOOL32 MSGBODY_LEN_GE(CServMsg &cServMsg, u16 wLen)
{
	if(cServMsg.GetMsgBodyLen() < wLen)
	{
		const s8* pszMsgStr = OspEventDesc(cServMsg.GetEventId());
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    函数名      : CompareStringElements
    功能        : 比较字符串
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/05/09              liaokang      创建
====================================================================*/
inline BOOL32 CompareStringElements(const s8* pchStr1, const s8* pchStr2)
{    
    if( NULL == pchStr1 || NULL == pchStr2 )
    {
        return FALSE;
    }

    if( strlen(pchStr1) != strlen(pchStr2) ||
        0 != strncmp( pchStr1, pchStr2, strlen(pchStr1) ) )
    {
        return FALSE;
    }
    return TRUE;
}

API void msdebug( s32 nDbgLvl );
API void setmschecktime(u32 dwTime);
API void showmsstate();

#endif //__MSMANAGERSSN_H_

//END OF FILE
