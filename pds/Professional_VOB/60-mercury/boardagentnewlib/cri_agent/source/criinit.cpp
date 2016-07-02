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
#include "boardguardbasic.h"
#include "cri_agent.h"
//#include "evagtsvc.h"
#include "mcuver.h"
/*lint -save -e765*/

/*
*该信号量向MPC取配置信息时使用
*/
SEMHANDLE      g_semCRI;

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

/*lint -save -e765*/
/*====================================================================
    函数名      ：CriAgentSelfTest
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
API void CriAgentSelfTest()
{
	 
	OspPrintf(TRUE, FALSE, "McuA IpAddr = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetMpcIpA())) ); 
	OspPrintf(TRUE, FALSE, "McuA Port	= %d \n", g_cBrdAgentApp.GetMpcPortA() ); 
	OspPrintf(TRUE, FALSE, "McuB IpAddr = %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetMpcIpB())) ); 
	OspPrintf(TRUE, FALSE, "McuB Port	= %d \n", g_cBrdAgentApp.GetMpcPortB() ); 

	OspPrintf(TRUE, FALSE, "Board Id    = %d \n", g_cBrdAgentApp.GetBoardId() );
	OspPrintf(TRUE, FALSE, "Brd IpAddr	= %s \n", IptoStr(ntohl(g_cBrdAgentApp.GetBrdIpAddr())) ); 

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
API void criagtver(void)
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
	OspPrintf( TRUE, FALSE, "CriAgent: %s  compile time: %s    %s\n",  
		VER_CRIAGENT, __DATE__, __TIME__ );
	OspPrintf( TRUE, FALSE, "BasicBoardAgent: %s  compile time: %s	  %s\n",
		VER_BASICBOARDAGENT, __DATE__, __TIME__ );
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
	mkdir( (s8*)DIR_FTP, 700 );
#endif
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
	OspPrintf( TRUE, FALSE, "criagtver:			criagt version command\n");
	OspPrintf( TRUE, FALSE, "SetCriAgentDebug:	set criagt debug command\n");
	OspPrintf( TRUE, FALSE, "CriAgentSelfTest:	self test debug command\n");
	OspPrintf( TRUE, FALSE, "pcrimsg:			print crimsg command\n");
	OspPrintf( TRUE, FALSE, "npcrimsg:			not print pcrimsg command\n");
}

/*====================================================================
    函数名      ：CriAgentAPIEnableInLinux
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
void CriAgentAPIEnableInLinux()
{
#ifdef _LINUX_ 
    OspRegCommand("criagtver", (void*)criagtver, "criagt version command");    
    OspRegCommand("SetCriAgentDebug",   (void*)SetCriAgentDebug,   "set criagt debug command");  
    OspRegCommand("CriAgentSelfTest", (void*)CriAgentSelfTest, "self test debug command");
	OspRegCommand("pcrimsg",   (void*)pcrimsg,   "print crimsg command");
	OspRegCommand("npcrimsg",   (void*)npcrimsg,   "not print pcrimsg command");
	OspRegCommand("brdagthelp",	(void*)brdagthelp, "brd agent command help");
#endif
    
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
API BOOL InitCriAgent()
{

	CriAgentAPIEnableInLinux();

	static u16 wFlag = 0;
	if( wFlag != 0 )
	{
		printf("BrdAgent: Already Inited!\n" );
		return TRUE;
	}

	wFlag++;
	if( !OspSemBCreate(&g_semCRI) )
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

	OspSemTake(g_semCRI);

    // [2012/05/05 liaokang] 支持 系统根据配置文件配置IP
    g_cBrdAgentApp.InitLocalCfgInfoByCfgFile();

    // 从配置文件中读出侦听端口，失败采用默认值
    // 创建连接用的监听端口 [2012/05/05 liaokang] 支持IS2.2内部通信
    OspCreateTcpNode( 0, g_cBrdAgentApp.GetBrdMasterSysListenPort() ); 

	//读配置
	if( !g_cBrdAgentApp.ReadConnectMcuInfo() )
	{
		printf("Read brd cfg failed!\n");
		return FALSE;
	}
	//创建ftp目录
	CreateftpDir( );

    // 添加单板代理消息字符串
    AddBrdAgtEventStr();

	//创建单板代理应用
	g_cBrdAgentApp.CreateApp( "CriAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//创建单板守卫应用
	g_cBrdGuardApp.CreateApp( "CriGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//单板代理开始工作
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	//单板守卫模块开始工作
	
	// 单板守卫保存单板类型信息，方便根据不同单板类型有不同告警 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	OspSetLogLevel( AID_MCU_BRDGUARD, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

    //SetCriAgentDebug();

	//wait here
	OspSemTake(g_semCRI);
	
	return TRUE;
}
/*lint -restore*/

