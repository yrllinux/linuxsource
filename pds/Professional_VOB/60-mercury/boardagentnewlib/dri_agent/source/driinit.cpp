 /*****************************************************************************
   模块名      : Board Agent
   文件名      : driinit.cpp
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
#include "boardguardbasic.h"
#include "dri_agent.h"
//#include "evagtsvc.h"
#include "mcuver.h"
/*lint -save -e765*/
/*
*该信号量向MPC取配置信息时使用
*/
SEMHANDLE      g_semDRI;

/*====================================================================
    函数名      ：SetDriAgentDebug
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
// API void SetDriAgentDebug()
// {
// 	OspSetLogLevel(AID_MCU_BRDAGENT, 255, 255);
// 	return;
// }

API void SetDriAgentRelease()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);
	return;
}

API void SetDriAgentDebug1()
{
	OspSetLogLevel(AID_MCU_BRDAGENT, 255, 255);
	OspSetTrcFlag(AID_MCU_BRDGUARD, 255, 255);

	OspSetLogLevel(AID_MCU_BRDAGENT, 255, 255);
	OspSetTrcFlag(AID_MCU_BRDGUARD, 255, 255);
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
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)(void*)&dwIp );
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
API void DriAgentSelfTest()
{
	OspPrintf(TRUE, FALSE, "Board Id    = %d \n", g_cBrdAgentApp.GetBoardId() ); 
	OspPrintf(TRUE, FALSE, "Brd Ip	    = %s \n", IptoStr( g_cBrdAgentApp.GetBrdIpAddr() ) );
	OspPrintf(TRUE, FALSE, "McuA IpAddr = %s \n", IptoStr(g_cBrdAgentApp.GetMpcIpA()) ); 
	OspPrintf(TRUE, FALSE, "Mcu Port	= %d \n", g_cBrdAgentApp.GetMpcPortA() ); 
	OspPrintf(TRUE, FALSE, "McuB IpAddr = %s \n", IptoStr(g_cBrdAgentApp.GetMpcIpB()) );
	OspPrintf(TRUE, FALSE, "McuB Port   = %d \n", g_cBrdAgentApp.GetMpcPortB() );

	//单板位置, zgc, 07/02/02
	TBrdPosition tBrdPos;
	BrdQueryPosition( &tBrdPos );  //此处不用转为TBrdPos,因为只是打印，jlb_081120
	OspPrintf( TRUE, FALSE, "Brd Layer: %d, Brd Slot: %d\n", tBrdPos.byBrdLayer, tBrdPos.byBrdSlot );

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

/*====================================================================
    函数名      ：driagtver
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
API void driagtver(void)
{
	s8 gs_VersionBuf[128] = {0};
	
    strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    s8 achFullDate[24] = {0};
    
    s8 achDate[32] = {0};
    sprintf(achDate, "%s", __DATE__);
    StrUpper(achDate);
    
    sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
    
    if ( 0 == strcmp( achMon, "JAN") )		 
        byMonth = 1;
    else if ( 0 == strcmp( achMon, "FEB") )
        byMonth = 2;
    else if ( 0 == strcmp( achMon, "MAR") )
        byMonth = 3;
    else if ( 0 == strcmp( achMon, "APR") )		 
        byMonth = 4;
    else if ( 0 == strcmp( achMon, "MAY") )
        byMonth = 5;
    else if ( 0 == strcmp( achMon, "JUN") )
        byMonth = 6;
    else if ( 0 == strcmp( achMon, "JUL") )
        byMonth = 7;
    else if ( 0 == strcmp( achMon, "AUG") )
        byMonth = 8;
    else if ( 0 == strcmp( achMon, "SEP") )		 
        byMonth = 9;
    else if ( 0 == strcmp( achMon, "OCT") )
        byMonth = 10;
    else if ( 0 == strcmp( achMon, "NOV") )
        byMonth = 11;
    else if ( 0 == strcmp( achMon, "DEC") )
        byMonth = 12;
    else
        byMonth = 0;
    
    if ( byMonth != 0 )
    {
        sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    else
    {
        // for debug information
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
	OspPrintf( TRUE, FALSE, "RECORDER Version: %s\n", gs_VersionBuf);
	OspPrintf( TRUE, FALSE, "DriAgent: %s  compile time: %s    %s\n",  
		VER_DRIAGENT, __DATE__, __TIME__ );
	OspPrintf( TRUE, FALSE, "BasicBoardAgent: %s  compile time: %s    %s\n",  
		VER_BASICBOARDAGENT, __DATE__, __TIME__ );
}

/*=============================================================================
函 数 名： brdagthelp
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
API void brdagthelp()
{
	OspPrintf( TRUE, FALSE, "driagtver:				cdriagt version command\n");
	OspPrintf( TRUE, FALSE, "SetDriAgentDebug1:		set driagt debug command\n");
	OspPrintf( TRUE, FALSE, "DriAgentSelfTest:		self test debug command\n");
	OspPrintf( TRUE, FALSE, "pdrimsg:				print dri msg\n");
	OspPrintf( TRUE, FALSE, "npdrimsg:				stop printing dri msg\n");
	OspPrintf( TRUE, FALSE, "SetDriAgentRelease:	release driagt debug command\n");
}

/*====================================================================
    函数名      ：DriAgentAPIEnableInLinux
    功能        ：向Linux注册API函数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/02/02    4.0         周广程         创建
====================================================================*/
/*lint -save -e611*/
void DriAgentAPIEnableInLinux()
{
#ifdef _LINUX_ 
    OspRegCommand("driagtver", (void*)driagtver, "driagt version command");    
    OspRegCommand("SetDriAgentDebug1",   (void*)SetDriAgentDebug1,   "set driagt debug command");
	OspRegCommand("SetDriAgentRelease",   (void*)SetDriAgentRelease,   "release driagt debug command");  
    OspRegCommand("DriAgentSelfTest", (void*)DriAgentSelfTest, "self test debug command");
	OspRegCommand("pdrimsg",   (void*)pdrimsg,   "print dri msg");
	OspRegCommand("npdrimsg",   (void*)pdrimsg,   "stop printing dri msg");
	OspRegCommand("brdagthelp",	(void*)brdagthelp, "brd agent command help");
#endif
    
    return;
}
/*lint -restore*/

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
    函数名      ：InitDriAgent
    功能        ：初始化DRI单板代理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API BOOL InitDriAgent()
{  
	DriAgentAPIEnableInLinux();

	static u16 wFlag = 0;
	if (wFlag != 0)
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}

	wFlag++;
	if (!OspSemBCreate(&g_semDRI))
	{
		return FALSE;
	}

#ifndef _VXWORKS_
	if( ERROR == BrdInit() )
	{
		printf("[InitAgent] call BrdInit fail.\n");
		return FALSE;
	}
#endif	

	OspSemTake(g_semDRI);

	if (!g_cBrdAgentApp.ReadConnectMcuInfo())// 读配置
	{
        printf("[DriAgent]ReadConnectMcuInfo error!\n");
		return FALSE;
	}

	//创建ftp目录
	CreateftpDir();

	//创建单板代理应用
	g_cBrdAgentApp.CreateApp("DriAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT);
	//创建单板守卫应用
	g_cBrdGuardApp.CreateApp("DriGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//单板代理开始工作
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );

	// 单板守卫保存单板类型信息，方便根据不同单板类型有不同告警 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));

	OspSetLogLevel(AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag(AID_MCU_BRDGUARD, 0, 0);

	OspSetLogLevel(AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag(AID_MCU_BRDGUARD, 0, 0);

	//wait here
	OspSemTake(g_semDRI);

	return TRUE;
}

