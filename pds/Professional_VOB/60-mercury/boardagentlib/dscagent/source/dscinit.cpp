/*****************************************************************************
   模块名      : Board Agent
   文件名      : BoardInit.cpp
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#include "osp.h"
#include "dscagent.h"
#include "dscguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "mcuver.h"

#ifdef _LINUX_
#include<unistd.h>
#endif
/*
*该信号量向MPC取配置信息时使用
*/
SEMHANDLE      g_semDSC;
extern BOOL32  g_bPrintDscLog;

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
    05/11/10    4.0         john          change
====================================================================*/
void CreateftpDir( void )
{
#ifdef WIN32
	CreateDirectory( ( LPCTSTR )DIR_FTP, NULL );
#else
#ifdef _VXWORKS_
	mkdir( ( s8* )DIR_FTP );
#else
#ifdef _LINUX_
    mkdir( (s8*)DIR_FTP, 0777);
    chmod( (s8*)DIR_FTP, 0777 );
#endif /* _LINUX_ */
#endif /* _VXWORKS_ */
#endif /* WIN32 */
}

/*====================================================================
    函数名      ：SetCriAgentDebug
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
API void SetCriAgentDebug()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 255, 255);

	OspSetLogLevel( AID_MCU_BRDGUARD, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);
	return;
}

/*====================================================================
    函数名      ：InitCriAgent
    功能        ：初始化MMP单板代理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API BOOL InitDscAgent()
{
#ifndef _VXWORKS_
	if( ERROR == BrdInit() )
	{
		printf("[InitAgent] call BrdInit fail.\n");
		return FALSE;
	}
#endif   

#ifdef OSPEVENT
    #undef OSPEVENT
#endif
    
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
    
#ifdef _EV_AGTSVC_H_
    #undef _EV_AGTSVC_H_
    #include "evagtsvc.h"
#else
    #include "evagtsvc.h"
#endif

    DscAgentAPIEnableInLinux();

	static u16 wFlag;
	if( wFlag != 0 )
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}

	wFlag++;
	if( !OspSemBCreate(&g_semDSC) )
	{
		printf("[Dsc] Create SemBCreate fail\n");
        return FALSE;
	}

	OspSemTake(g_semDSC);

	// 读配置
	if( !g_cBrdAgentApp.ReadConnectMcuInfo() )
	{
		printf("[Dsc] Fail to read configre file\n");
        return FALSE;
	}        

	// 创建ftp目录
	CreateftpDir( );

	// 先检查NIP是否设置了IP, zgc, 2007-07-31
	g_cBrdAgentApp.PreTreatDscNIP();

	// 创建单板代理应用
	g_cBrdAgentApp.CreateApp( "DscAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	// 创建单板守卫应用
	g_cBrdGuardApp.CreateApp( "DscGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	// 单板代理开始工作
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_AGENT_POWERON );
	// 单板守卫模块开始工作
	OspPost( MAKEIID(AID_MCU_BRDGUARD, 1), BOARD_GUARD_POWERON );
/*
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	OspSetLogLevel( AID_MCU_BRDGUARD, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);
*/
    //SetCriAgentDebug();

	//wait here
	OspSemTake(g_semDSC);  // 等待，直到收到配置信息
#ifdef _LINUX_
	printf("\nStart DSC Agent(pid: %d) OK!\n", getpid() );
#endif
	
	return TRUE;
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
#else
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)&dwIp );
	strncpy( achPeerIpAddr, pchPeerIpAddr, sizeof(achPeerIpAddr) );
	achPeerIpAddr[ sizeof(achPeerIpAddr) - 1 ] = '\0';	
#endif

	return (char*)achPeerIpAddr;
}

/*====================================================================
    函数名      ：DscAgentSelfTest
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
API void DscAgentSelfTest()
{
	OspPrintf(TRUE, FALSE, "Board Id    = %d \n", g_cBrdAgentApp.GetBoardId() ); 
	OspPrintf(TRUE, FALSE, "Mcu IpAddr  = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetConnectMcuIpAddr())) ); 
	OspPrintf(TRUE, FALSE, "Mcu Port	= %d \n", g_cBrdAgentApp.GetConnectMcuPort() ); 
	OspPrintf(TRUE, FALSE, "Brd IpAddr	= %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetBrdIpAddr())) ); 

#ifndef WIN32
    // DSC 没有位置
    /*
	//单板位置, zgc, 07/02/02
	TBrdPosition tBrdPos;    
	BrdQueryPosition( &tBrdPos );
	OspPrintf( TRUE, FALSE, "Brd Layer: %d, Brd Slot: %d\n", tBrdPos.byBrdLayer, tBrdPos.byBrdSlot );
    */
#endif

    TDSCModuleInfo tInfo;
    tInfo = g_cBrdAgentApp.GetLocalModuleInfo();
    OspPrintf(TRUE, FALSE, "[ReadModuleInfoFromBinCfg] Local Config: \n");
    tInfo.Print();

    tInfo = g_cBrdAgentApp.GetRunningModuleInfo();
    OspPrintf(TRUE, FALSE, "[ReadModuleInfoFromBinCfg] Running Config: \n");
    tInfo.Print();

	// prs 配置信息
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

/*====================================================================
    函数名      ：GetBoardSlot
    功能        ：获取板的槽号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API u8 GetBoardSlot()
{
	return g_cBrdAgentApp.GetBoardId();
}

/*====================================================================
    函数名      ：criagtver
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
API void dscagtver(void)
{
	OspPrintf( TRUE, FALSE, "DscAgent: %s  compile time: %s    %s\n",  
		VER_DSCAGENT, __DATE__, __TIME__ );
}

API void pdscmsg(void)
{
    g_bPrintDscLog = TRUE;
}

API void npdscmsg(void)
{
    g_bPrintDscLog = FALSE;
}

API void dschelp(void)
{
	OspPrintf( TRUE, FALSE, "dscagtver:				dscagt version command\n");
	OspPrintf( TRUE, FALSE, "pdscmsg:				pdscmsg command\n");
	OspPrintf( TRUE, FALSE, "npdscmsg:				npdscmsg clear command\n");
	OspPrintf( TRUE, FALSE, "DscAgentSelfTest:		self test debug command\n");
}

void DscAgentAPIEnableInLinux()
{
#ifdef _LINUX_    
    OspRegCommand("dscagtver", (void*)dscagtver, "dscagt version command");    
    OspRegCommand("pdscmsg",   (void*)pdscmsg,   "pdscmsg command");
    OspRegCommand("npdscmsg",  (void*)npdscmsg,  "npdscmsg clear command");    
    OspRegCommand("DscAgentSelfTest", (void*)DscAgentSelfTest, "self test debug command");
	OspRegCommand("dschelp",  (void*)dschelp,  "dsc help command");   
#endif
    
    return;
}

#ifdef WIN32
#ifndef _LIB
void main()
{
    OspInit(TRUE);
    InitDscAgent();
    while (true)
    {
        Sleep(INFINITE);
    }
}
#endif
#endif

