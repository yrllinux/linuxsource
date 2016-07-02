/*****************************************************************************
   模块名      : Board Agent
   文件名      : criagent.h
   相关文件    : 
   文件实现功能: 相关配置函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#ifndef CRICONFIG_H
#define CRICONFIG_H

#include "boardconfigbasic.h"

// add to brdcfg.ini [2012/05/05 liaokang]
// 支持通过配置文件配置监听端口
#define SECTION_BoardMasterSystem	    (s8*)"BoardMasterSystem"
#define KEY_BrdMasterSysListenPort	    (s8*)"BrdMasterSysListenPort"
#define DEFVALUE_BrdMasterSysListenPort BRDMASTERSYS_LISTEN_PORT   // 主系统（IS2.2 8548）默认监听端口
// 支持 系统根据配置文件配置IP
#define SECTION_IpConfig			    (s8*)"IpConfig"
#define KEY_IsSupportIpConfig           (s8*)"IsSupportIpConfig"
#define DEFVALUE_IsSupportIpConfig		(u8)0           // 默认 不支持
#define KEY_EthChoice				    (s8*)"EthChoice"
#define DEFVALUE_EthChoice			    (u8)1           // 默认 后网口
#define KEY_BrdMasterSysIp	            (s8*)"BrdMasterSysIp"
#define KEY_BrdMasterSysInnerIp	        (s8*)"BrdMasterSysInnerIp"
#define KEY_BrdMasterSysIPMask	        (s8*)"BrdMasterSysIPMask"
#define KEY_BrdMasterSysDefGateway	    (s8*)"BrdMasterSysDefGateway"
// brdcfg.ini end

class CCriConfig : public CBBoardConfig
{
public:
	CCriConfig();
	~CCriConfig();
    friend class CBoardAgent;
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
	BOOL GetPrsCfg( TPrsCfg* ptCfg );

	/*====================================================================
	功能：设置PRS的配置信息
	参数：无
	返回值：TRUE/FALSE
	====================================================================*/
	BOOL SetPrsConfig( TEqpPrsEntry *ptPrsCfg );

	BOOL32 IsPrsCfgEqual( const TPrsCfg &tPrsCfg);

	void GetBoardSequenceNum( u8 *pBuf, s8* achSep ); //获取单板序列号

    // 依配置文件初始化本地配置信息
    BOOL32 InitLocalCfgInfoByCfgFile(void);     // [2012/05/05 liaokang] 支持 系统根据配置文件配置IP
    // 设置单板主系统的监听端口
    BOOL32 SetBrdMasterSysListenPort( u16 wListenPort );
    // 获取单板主系统的监听端口
    u16    GetBrdMasterSysListenPort(void);

private:
	BOOL    bIsRunPrs;      //是否运行PRS
	TPrsCfg m_prsCfg;
    u16     m_wBrdMasterSysListenPort;  // 主系统（IS2.2 8548） Listen Port
};

#endif /* CRICONFIG_H */
