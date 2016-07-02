    /*****************************************************************************
   模块名      : Board Agent
   文件名      : dsiinit.cpp
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/12/05  3.5         李 博        新接口的修改
******************************************************************************/
#include "osp.h"
#include "dsiagent.h"
#include "dsiguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "mcuver.h"
/*
*该信号量向MPC取配置信息时使用
*/
SEMHANDLE      g_semDSI;

extern s32 g_nLogLevel;

/*=============================================================================
  函 数 名： setdsilog
  功    能： 打印Dsi板信息
  算法实现： 
  全局变量： 
  参    数： s32 nLvl：0：不打印，1：打印一般信息，2：打印重要信息
  返 回 值： API void 
=============================================================================*/
API void setdsilog(s32 nLvl)
{
	if( nLvl > 2 ) 
	{
		nLvl = 2;
	}
    g_nLogLevel = nLvl;
    return;
}

/*====================================================================
    函数名      ：SetDsiAgentDebug
    功能        ：设置MMPAGENT打印为DEBUG级别
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void SetDsiAgentDebug()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);

	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);
	return;
}

/*====================================================================
    函数名      ：IptoStr
    功能        ：将IP地址(主机序)转换成字符串
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：IP字符串
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
static char* IptoStr( u32 dwIpAddr )
{
	static char achPeerIpAddr[20];
	char* pchPeerIpAddr;
	u32 dwIp = htonl( dwIpAddr );

#ifdef _VXWORKS_   /*将IP地址转换成字符串*/ 
	inet_ntoa_b(*(struct in_addr*)&dwIp, achPeerIpAddr);
#else // WIN32 and linux
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)&dwIp );
	strncpy( achPeerIpAddr, pchPeerIpAddr, sizeof(achPeerIpAddr) );
	achPeerIpAddr[ sizeof(achPeerIpAddr) - 1 ] = '\0';	
#endif

	return (char*)achPeerIpAddr;
}

/*====================================================================
    函数名      ：DriAgentSelfTest
    功能        ：打印当前的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void DsiAgentSelfTest()
{
	OspPrintf(TRUE, FALSE, "Board Id    = %d \n", g_cBrdAgentApp.GetBoardId() ); 
	OspPrintf(TRUE, FALSE, "Mcu IpAddr  = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetConnectMcuIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "Mcu Port	= %d \n", g_cBrdAgentApp.GetConnectMcuPort() ); 
	OspPrintf(TRUE, FALSE, "Brd Port	= %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetBrdIpAddr())) ); 

#ifndef WIN32
	//单板位置, zgc, 07/02/02
	TBrdPosition tBrdPos;
	BrdQueryPosition( &tBrdPos );
	OspPrintf( TRUE, FALSE, "Brd Layer: %d, Brd Slot: %d\n", tBrdPos.byBrdLayer, tBrdPos.byBrdSlot );
#endif

	//prs 配置信息
	if( g_cBrdAgentApp.IsRunPrs() )
	{
		TPrsCfg tPrsCfg;
		g_cBrdAgentApp.GetPrsCfg(&tPrsCfg);
		OspPrintf(TRUE, FALSE, "\n\nPrs Id          = %d \n", tPrsCfg.byEqpId ); 
		OspPrintf(TRUE, FALSE, "Prs Type            = %d \n", tPrsCfg.byEqpType ); 
		OspPrintf(TRUE, FALSE, "Prs Alias           = %s \n", tPrsCfg.achAlias ); 
		OspPrintf(TRUE, FALSE, "Prs IpAddr          = %s \n", IptoStr(ntohl(tPrsCfg.dwLocalIP)) ); 
		OspPrintf(TRUE, FALSE, "Prs Recv Start Port = %d \n", tPrsCfg.wRcvStartPort ); 

		OspPrintf(TRUE, FALSE, "Prs First Time Span  = %d \n", tPrsCfg.m_wFirstTimeSpan ); 
		OspPrintf(TRUE, FALSE, "Prs Second Time Span = %d \n", tPrsCfg.m_wSecondTimeSpan ); 
		OspPrintf(TRUE, FALSE, "Prs Third Time Span  = %d \n", tPrsCfg.m_wThirdTimeSpan ); 
		OspPrintf(TRUE, FALSE, "Prs Reject Time Span = %d \n", tPrsCfg.m_wRejectTimeSpan ); 

	
	}

	return;
}


API u8 GetBoardSlot()
{
	return g_cBrdAgentApp.GetBoardId();
}

/*====================================================================
    函数名      ：dsiagtver
    功能        ：打印当前模块的版本信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void dsiagtver(void)
{
	OspPrintf( TRUE, FALSE, "DsiAgent: %s  compile time: %s    %s\n",  
		VER_DSIAGENT, __DATE__, __TIME__ );
}

/*=============================================================================
函 数 名： dsihelp
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/5/17   4.0			周广程                  创建
=============================================================================*/
API void dsihelp()
{
	OspPrintf( TRUE, FALSE, "dsiagtver:				dsiagt version command\n");
	OspPrintf( TRUE, FALSE, "SetDsiAgentDebug:		set dsiagt debug command\n");
	OspPrintf( TRUE, FALSE, "DsiAgentSelfTest:		self test debug command\n");
}

/*====================================================================
    函数名      ：DsiAgentAPIEnableInLinux
    功能        ：向Linux注册API函数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/02/05    4.0         周广程         创建
====================================================================*/
void DsiAgentAPIEnableInLinux()
{
#ifdef _LINUX_    
    OspRegCommand("dsiagtver", (void*)dsiagtver, "dsiagt version command");    
    OspRegCommand("DsiAgentSelfTest", (void*)DsiAgentSelfTest, "self test debug command");
	OspRegCommand("SetDsiAgentDebug", (void*)SetDsiAgentDebug, "set dsiagt debug command");
	OspRegCommand("setdsilog", (void*)setdsilog, "set dsi log level: 0-always print, 1-common info, 2-important info");
	OspRegCommand("dsihelp",	(void*)dsihelp, "dsi command help");
#endif
    
    return;
}

/*====================================================================
    函数名      ：CreateftpDir
    功能        ：创建FTP目录
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
void CreateftpDir( void )
{
#ifdef WIN32
	CreateDirectory( ( LPCTSTR )DIR_FTP, NULL );
#elif defined( _VXWORKS_ )
	mkdir( ( s8* )DIR_FTP );
#else // linux
	mkdir( (s8*)DIR_FTP, 0777 );
    chmod( (s8*)DIR_FTP, 0777 );
#endif
}

/*====================================================================
    函数名      ：InitDsiAgent
    功能        ：初始化DSI单板代理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API BOOL InitDsiAgent()
{

	DsiAgentAPIEnableInLinux();

	static u16 wFlag;
	if( wFlag != 0 )
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}

	wFlag++;
	if( !OspSemBCreate(&g_semDSI) )
	{
		return FALSE;
	}

#ifdef _LINUX_
	if( ERROR == BrdInit() )
	{
		printf("[InitAgent] call BrdInit fail.\n");
		return FALSE;
	}
#endif

	OspSemTake(g_semDSI);

	//读配置
	if( !g_cBrdAgentApp.ReadConnectMcuInfo() )
	{
		return FALSE;
	}
	//创建ftp目录
	CreateftpDir( );

	//根据配置信息恢复E1链路的RELAY MODE
#ifndef WIN32
	g_cBrdAgentApp.ResumeE1RelayMode();
#endif

	//创建单板代理应用
	g_cBrdAgentApp.CreateApp( "DsiAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//创建单板守卫应用
	g_cBrdGuardApp.CreateApp( "DsiGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//单板代理开始工作
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), BOARD_AGENT_POWERON );
	//单板守卫模块开始工作
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON );

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	//wait here
	OspSemTake(g_semDSI);

	return TRUE;
}

