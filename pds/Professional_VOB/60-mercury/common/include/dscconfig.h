/*****************************************************************************
   模块名      : Board Agent
   文件名      : dscagent.h
   相关文件    : 
   文件实现功能: 相关配置函数定义
   作者        : 李 博
   版本        : V3.6  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/04/25  3.6         李 博         创建
******************************************************************************/
#ifndef _DSCCONFIG_H_
#define _DSCCONFIG_H_

#include "mcuagtstruct.h"
#include "eqpcfg.h"
#include "kdvsys.h"
#include "boardconfigbasic.h"

#define FILENAME_MODULECFG      (s8*)"/usr/bin/autorun.inf"

// mtadpdebug.ini		
#define FILE_MTADPDEBUG_INI		(s8*)"mtadpdebug.ini"
#define SECTION_calldata		(s8*)"calldata"
#define KEY_rasport				(s8*)"rasport"
#define KEY_q931port			(s8*)"q931port"

// modcfg.ini
#define FILE_MODCFG_INI			(s8*)"modcfg.ini"
#define SETCION_Modules			(s8*)"Modules"
#define KEY_mtadp				(s8*)"mtadp"
#define DEFVALUE_mtadp			(u8)1
#define KEY_mp					(s8*)"mp"
#define DEFVALUE_mp				(u8)1
#define KEY_prs					(s8*)"prs"
#define DEFVALUE_prs			(u8)0

#define SECTION_mpcfg			(s8*)"mpcfg"
#define KEY_waitingIFrame		(s8*)"waitingIFrame"
#define DEFVALUE_waitingIFrame	(u8)0

// pxysrvCfg.ini
#define FILENAME_PXYSRVCFG		(s8*)"/usr/etc/pxy/conf/pxysrvCfg.ini"

#define SECTION_StartInfo		(s8*)"StarInfo"
#define KEY_StartPxy			(s8*)"StartPxy"
#define DEFVALUE_StartPxy		(u8)1
#define KEY_StartMp				(s8*)"StartMp"
#define DEFVALUE_StartMp		(u8)1

#define SETCION_LocalInfo		(s8*)"LocalInfo"
#define KEY_IpAddr				(s8*)"IpAddr"
#define KEY_RasPort				(s8*)"RasPort"
#define DEFVALUE_RasPort		(u16)1819
#define KEY_H245Port			(s8*)"H245Port"
#define DEFVALUE_H245Port		(u16)1820
#define KEY_OSPPORT				(s8*)"OSPPORT"
#define DEFVALUE_OSPPORT		(u16)2776
#define KEY_RTPPORT				(s8*)"RTPPORT"
#define DEFVALUE_RTPPORT		(u16)2776
#define KEY_RTCPPORT			(s8*)"RTCPPORT"
#define DEFVALUE_RTCPPORT		(u16)2777
#define KEY_MEDIABASEPORT		(s8*)"MEDIABASEPORT"
#define DEFVALUE_MEDIABASEPORT	(u16)48000

#define SECTION_GKInfo			(s8*)"GKInfo"
#define KEY_GKIpAddr			(s8*)"IpAddr"
#define KEY_GKRasPort			(s8*)"RasPort"
#define DEFVALUE_GKRasPort		(u16)1719

#define SECTION_defaultGW		(s8*)"DefaultGW"

// DCSConfig.ini
#define FILENAME_DCSCONFIG		(s8*)"/usr/etc/t120/DCSConfig.ini"

#define SECTION_DCServer		(s8*)"DCServer"
#define KEY_DCSIP				(s8*)"DCSIP"
#define KEY_DCSMCUIP			(s8*)"DCSMCUIP"
#define KEY_DCSGKIP				(s8*)"DCSGKIP"

#define KEY_DCST120Port			(s8*)"DCST120Port"
#define DEFVALUE_DCST120Port	(u16)1503
#define KEY_DCSDccPort			(s8*)"DCSDccPort"
#define DEFVALUE_DCSDccPort		(u16)61000
#define KEY_DCSRASPort			(s8*)"DCSRASPort"
#define DEFVALUE_DCSRASPort		(u16)1721
#define	KEY_DCSH323Port			(s8*)"DCSH323Port"
#define DEFVALUE_DCSH323Port	(u16)1722 
#define	KEY_DCSTelnetPort		(s8*)"DCSTelnetPort"
#define DEFVALUE_DCSTelnetPort	(u16)1110              
#define KEY_DCSMCUPort			(s8*)"DCSMCUPort"
#define DEFVALUE_DCSMCUPort		(u16)60000
#define KEY_DCSBACKUPMCUIP		(s8*)"DCSBACKUPMCUIP"
#define DEFVALUE_DCSBACKUPMCUIP	(u16)0
#define KEY_DCSBACKUPMCUPort	(s8*)"DCSBACKUPMCUPort"
#define DEFVALUE_DCSBACKUPMCUPort (u16)60000
#define KEY_DCSGKPort			(s8*)"DCSGKPort"
#define DEFVALUE_DCSGKPort		(u16)1719
#define KEY_DCSGKRegNum			(s8*)"DCSGKRegNum"
#define DEFVALUE_DCSGKRegNum	(u16)120
#define KEY_DCSAlias			(s8*)"DCSAlias"
#define DEFVALUE_DCSAlias		(s8*)"DCServer"
#define KEY_DCSEmail			(s8*)"DCSEmail"
#define DEFVALUE_DCSEmail		(s8*)"laiqi@kedacom.com"
#define KEY_DCSLocation			(s8*)"DCSLocation"
#define DEFVALUE_DCSLocation	(s8*)"Shanghai"
#define KEY_DCSPhoneNum			(s8*)"DCSPhoneNum"
#define DEFVALUE_DCSPhoneNum	(s8*)"021-64273989"
////////////////////////////////////////////////////////

#define NETTYPE_INTERNALNET		(u8)0
#define NETTYPE_EXTERNALNET		(u8)1
#define NETTYPE_MIXINGNET		(u8)2

#define RASPORT_GK				(u32)1709
#define Q931PORT_GK				(u32)1710
#define RASPORT_NOGK			(u32)1719
#define Q931PORT_NOGK			(u32)1720

class CDscConfig
{
public:
	CDscConfig();
	virtual ~CDscConfig();
    friend class CBoardAgent;
	/*====================================================================
	功能：获取单板的槽位号
	参数：无
	返回值：ID值，0表示失败
	====================================================================*/
	u8	GetBoardId();

	/*====================================================================
	功能：获取要连接的MCU的IP地址(网络序)
	参数：无
	返回值：MCU的IP地址(网络序)，0表示失败
	====================================================================*/
	u32 GetConnectMcuIpAddr();

	/*====================================================================
	功能：获取连接的MCU的端口号
	参数：无
	返回值：MCU的端口号，0表示失败
	====================================================================*/
	u16  GetConnectMcuPort();
	
	/*====================================================================
	功能：得到单板IP地址
	参数：无
	返回值：单板IP地址(网络序)
	====================================================================*/
	u32 GetBrdIpAddr( );

	/*====================================================================
	功能：是否运行Prs
	参数：无
	返回值：运行返回TRUE，反之FALSE
	====================================================================*/
	BOOL IsRunPrs();

	/*====================================================================
	功能：获取prs配置信息
	参数：无
	返回值：MCU的端口号，0表示失败
	====================================================================*/
	BOOL GetPrsCfg(TPrsCfg * ptCfg);

	/*====================================================================
	功能：设置单板IP地址
	参数：无
	返回值：TRUE/FALSE
	====================================================================*/
	BOOL SetBrdIpAddr(u32 dwIp);

	/*====================================================================
	功能：设置PRS的配置信息
	参数：无
	返回值：TRUE/FALSE
	====================================================================*/
	BOOL SetPrsConfig( TEqpPrsEntry * ptPrsCfg);

	/*====================================================================
	功能：设置运行的模块信息，如果和本地配置文件不一致，则更新本地文件
	参数：
	返回值：是否发生了写入
	====================================================================*/
    BOOL32 SetDscRunningModule(const TDSCModuleInfo &tInfo, BOOL32 bCompareModule = TRUE);
	
	/*====================================================================
	功能：从文件读配置信息
	参数：无
	返回值：TRUE/FALSE
	====================================================================*/
	BOOL ReadConnectMcuInfo();

	/*====================================================================
	功能：写入配置信息文件
	参数：无
	返回值：TRUE/FALSE 是否发生写入
	====================================================================*/
    BOOL32 WriteConnectMcuInfo();
    
	/*====================================================================
	功能：获取本地配置文件保存的运行的模块信息
	参数：
	返回值：无
	====================================================================*/
    TDSCModuleInfo GetLocalModuleInfo()
    {
        return m_tLocalModuleCfg;
    }

	/*====================================================================
	功能：获取MCU配置的DSC模块信息
	参数：
	返回值：无
	====================================================================*/
    TDSCModuleInfo GetRunningModuleInfo()
    {
        return m_tRunningModuleCfg;
    }

	/*=============================================================================
	函 数 名： SetLastRunningModuleInfo
	功    能： 
	算法实现： 
	全局变量： 
	参    数： const TDSCModuleInfo &tLastDscModuleInfo
	返 回 值： void 
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/6/28   4.0			周广程                  创建
	=============================================================================*/
	void SetLastRunningModuleInfo( const TDSCModuleInfo &tLastDscModuleInfo )
	{
		memcpy( &m_tLastRunningModuleCfg, &tLastDscModuleInfo, sizeof(TDSCModuleInfo) );
	}

	/*=============================================================================
	函 数 名： GetLastRunningModuleInfo
	功    能： 
	算法实现： 
	全局变量： 
	参    数： 
	返 回 值： TDSCModuleInfo 
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/6/28   4.0			周广程                  创建
	=============================================================================*/
	TDSCModuleInfo GetLastRunningModuleInfo()
	{
		return m_tLastRunningModuleCfg;
	}

    u16  GetMpcBPort(void);
    u16  SetMpcBPort(u16 wPort);

    u32  GetMpcBIp(void);
    u16  SetMpcBIp(u32 dwIp);
    
    // /usr/bin/autorun.cfg 文件读写
    void ReadModuleInfoFromBinCfg(TDSCModuleInfo &tInfo);
    void SaveModuleInfo2BinCfg(const TDSCModuleInfo &tInfo);

	// /usr/etc/pxy/conf/pxysrvCfg.ini 文件
	BOOL32 ReadDefaultGWFromCfg( TMINIMCUNetParamAll &tNetParamAll);
	BOOL32 SaveDefaultGWToCfg( const TMINIMCUNetParamAll &tNetParamAll);
	BOOL32 DelDefaultGWInCfg( void );
	BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum );
	BOOL32 SaveGKInfoToCfg( u32 dwGKIp, u16 wGKPort );
	s32	   MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate );
	s32	   MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate );
	s32	   DelSection( FILE *stream, const s8* lpszSectionName );
	BOOL32 TrimCfgFile( const s8* lpszCfgFilePath );
	BOOL32 CreatPxySrvCfgFile();
	BOOL32 CreatMtadpDebugFile();
	BOOL32 CreatBrdCfgFile();
	BOOL32 CreatModCfgFile();
	BOOL32 CreatDcsCfgFile();
	BOOL32 CreatDir(const s8* lpszDirPath);

	// 把DSC info中的CallAddr设置到DSC板上, zgc, 2007-07-19
	BOOL32 SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo );

	u32 GetGKIpAddr() { return ntohl(m_dwGKIpAddr); }
	void SetGKIpAddr(u32 dwGKIpAddr ) { m_dwGKIpAddr = htonl(dwGKIpAddr); }

	// 修改RAS PORT, zgc, 2007-08-02
	BOOL32 ModifyMtadpPort( u32 dwRasPort, u32 dwQ931Port );

	// 修改Dcs, zgc, 2007-08-02
	BOOL32 ModifyDcsConfig( u32 dwDcsIp, u32 dwMcuIp = 0, u32 dwGKIp = 0 );

	void PreTreatDscNIP(void);		// 检查DSC是否配置了IP, zgc, 2007-07-30

    /*
	u8	GetNetType(void) const { return m_byNetType; }
	void SetNetType(u8 byNetType) { m_byNetType = byNetType; }
    */

public:
    u32     m_dwDstMcuNode;
    u32     m_dwDstMcuNodeB;

// 下面得参数可以由单板代理读配置文件得到
    u32	    m_dwMpcIpAddr; 
	u16 	m_wMpcPort;
	u32	    m_dwMpcIpAddrB;
	u16 	m_wMpcPortB;

    u32     m_dwLocalInnerIp;    //本端用于连接Mpc的内部IP地址
    
    u16  m_wDiscHeartBeatTime;
    u8   m_byDiscHeartBeatNum;

// 下面参数从/usr/bin/module.cfg 中得到，只关心非MP8000B及DSC代理能控制的模块
    TDSCModuleInfo m_tLocalModuleCfg;    

private:
	BOOL    m_bIsRunPrs;      //是否运行PRS
	TPrsCfg m_prsCfg;
    u8      m_byBrdId;
	u32     m_dwBrdIpAddr;    //MPC上配置的本DSC板的外部主IP地址
    u8      m_byChoice;
	u32		m_dwGKIpAddr;		// 本端代理服务器的GK地址, zgc, 2007-07-21
	u8		m_byNetType;		// 网络类型, zgc, 2007-07-27
    TDSCModuleInfo m_tRunningModuleCfg;   // 其他需要启动的模块信息，从MCU获取

	TDSCModuleInfo m_tLastRunningModuleCfg;		// 修改前的dscmoduleinfo
};

#endif /* _DSCCONFIG_H_ */
