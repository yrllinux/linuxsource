/*****************************************************************************
   模块名      : mcu
   文件名      : mcu.cpp
   相关文件    : 
   文件实现功能: MCU业务普通函数定义
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/23  0.9         LI Yi       创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/


#include "evmcu.h"
#include "mcuvc.h"
#include "mcuver.h"
#include "mcsssn.h"
#include "mcuguard.h"
#include "mtadpssn.h"
#include "mpssn.h"
#include "mcucfg.h"
#include "msmanagerssn.h"
#include "addrbook.h"
#include "usermanage.h"
#include "mcuutility.h"
#include "dcsssn.h"
#include "eqpssn.h"
#include "dataswitch.h"
#include "mpmanager.h"
#include "nplusmanager.h"
#include "vcsssn.h"
#include "udpnms.h"
#include "udpdispatch.h"

#if defined(_VXWORKS_)

    #include <inetLib.h>
    #include "brdDrvLib.h"

#elif defined(_LINUX_)

    #include "boardwrapper.h"
    #include "nipdebug.h"
    
    #ifdef _MINIMCU_
        #define HARD_CODEC
        #include "codeccommon.h"
    #endif

#endif



/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <errno.h>
#include <string.h>

Elf32_Sym *get_symtab(Elf32_Sym *symtab, int tabsize, int addr);
Elf32_Shdr * read_elf_section(FILE *file, Elf32_Ehdr *ehdr);
void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,   int *size);
void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size);
int  read_elf_head(FILE *file, Elf32_Ehdr *ehdr);
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr);
int printf_symbl_name(int pc);

Elf32_Sym *sym;
char *strtab;
int symsize, strsize;

#define SECTION_HEADER_INDEX(I)				\
  ((I) < SHN_LORESERVE					\
   ? (I)						\
   : ((I) <= SHN_HIRESERVE				\
      ? 0						\
      : (I) - (SHN_HIRESERVE + 1 - SHN_LORESERVE)))


LPCSTR g_szAppName;

// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_
/////////////////////////////////////////////////////异常捕获处理

#ifdef WIN32

HWND g_hwndMain = NULL;

#ifdef _DEBUG
BOOL32 g_bOpenTelnetWin32 = TRUE;
#else
BOOL32 g_bOpenTelnetWin32 = FALSE;
#endif

#endif 


extern CModemSsnApp	g_cModemSsnApp;	//下级MCU会话应用实例
extern CMtSsnApp	g_cMtSsnApp;	//下级MCU会话应用实例

extern TMcuTopo	    g_atMcuTopo[MAXNUM_TOPO_MCU];
extern u8		    g_wMcuTopoNum;
extern TMtTopo		g_atMtTopo[MAXNUM_TOPO_MT];	//保存全网中MT拓扑信息
extern u16		    g_wMtTopoNum;		        //保存全网中MT数目

extern u8              g_byMcuModemNum;		    //Mcu Modem的个数,为0则系统中无需控制modem
extern TMcuModemTopo   g_atMcuModemTopo[MAXNUM_MCU_MODEM];    //Mcu Modem信息


void McuAPIRegInLinux(void);
    
/*====================================================================
    函数名      ：SetPrint
    功能        ：设置各应用TRACE和日志打印级别
    算法实现    ：（可选项）
    引用全局变量：无
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/01/30    1.0         LI Yi        创建
====================================================================*/
API void setPrint( u8 byApp = ( u8 )-1, u8 byLogScreen = 255, u8 byLogFile = 255, 
						 u8 byTrcScreen = 255, u8 byTrcFile = 255 )
{
	if( byApp == ( u8 )-1 )
	{
		for ( u16 wLoop = 0; wLoop < 254; wLoop++ )
		{
			OspSetLogLevel( ( u8 )wLoop, byLogFile, byLogScreen );
			OspSetTrcFlag( ( u8 )wLoop, byTrcFile, byTrcScreen );
		}
	}
	else
	{
		OspSetLogLevel( byApp, byLogFile, byLogScreen );
		OspSetTrcFlag( byApp, byTrcFile, byTrcScreen );
	}
}


API void setDebugTrcScreen( void )
{
	OspSetScrnTrcFlag( INVALID_APP, 0 );
	OspSetScrnTrcFlag( AID_MCU_AGENT, 0 );
	OspSetScrnTrcFlag( AID_MCU_VC, TRCSTATE|TRCEVENT|TRCTIMER );
	OspSetScrnTrcFlag( AID_MCU_GUARD, 0 );
	OspSetScrnTrcFlag( AID_MCU_MCSSN, 0 );
	OspSetScrnTrcFlag( AID_MCU_PERIEQPSSN, 0 );
	return;
}

API void setReleaseTrcScreen( void )
{
	OspSetScrnTrcFlag( INVALID_APP, 0 );
	OspSetScrnTrcFlag( AID_MCU_AGENT, 0 );
	OspSetScrnTrcFlag( AID_MCU_VC, 0 );
	OspSetScrnTrcFlag( AID_MCU_GUARD, 0 );
	OspSetScrnTrcFlag( AID_MCU_MCSSN, 0 );
	OspSetScrnTrcFlag( AID_MCU_PERIEQPSSN, 0 );
	return;
}

API void setDebugTrcFile( void )
{
	OspSetFileTrcFlag( INVALID_APP, 0 );
	OspSetFileTrcFlag( AID_MCU_AGENT, 0 );
	OspSetFileTrcFlag( AID_MCU_VC, TRCSTATE|TRCEVENT|TRCTIMER );
	OspSetFileTrcFlag( AID_MCU_GUARD, 0 );
	OspSetFileTrcFlag( AID_MCU_MCSSN, 0 );
	OspSetFileTrcFlag( AID_MCU_PERIEQPSSN, 0 );	
	return;
}

API void setReleaseTrcFile( void )
{
	OspSetFileTrcFlag( INVALID_APP, 0 );
	OspSetFileTrcFlag( AID_MCU_AGENT, 0 );
	OspSetFileTrcFlag( AID_MCU_VC, 0 );
	OspSetFileTrcFlag( AID_MCU_GUARD, 0 );
	OspSetFileTrcFlag( AID_MCU_MCSSN, 0 );
	OspSetFileTrcFlag( AID_MCU_PERIEQPSSN, 0 );
	return;
}

API void setDebugLogScreen( void )
{
	OspSetScrnLogLevel( INVALID_APP, LOGLVL_DEBUG2 );
	OspSetScrnLogLevel( AID_MCU_AGENT, LOGLVL_DEBUG2 );
	OspSetScrnLogLevel( AID_MCU_VC, LOGLVL_DEBUG2 );
	OspSetScrnLogLevel( AID_MCU_GUARD, LOGLVL_DEBUG2 );
	OspSetScrnLogLevel( AID_MCU_MCSSN, LOGLVL_DEBUG2 );
	OspSetScrnLogLevel( AID_MCU_PERIEQPSSN, LOGLVL_DEBUG2 );
	OspSetScrnLogLevel( 46, LOGLVL_DEBUG2 );	
	return;
}

API void setReleaseLogScreen( void )
{
	OspSetScrnLogLevel( INVALID_APP, LOGLVL_IMPORTANT );
	OspSetScrnLogLevel( AID_MCU_AGENT, LOGLVL_IMPORTANT );
	OspSetScrnLogLevel( AID_MCU_VC, LOGLVL_IMPORTANT );
	OspSetScrnLogLevel( AID_MCU_GUARD, LOGLVL_IMPORTANT );
	OspSetScrnLogLevel( AID_MCU_MCSSN, LOGLVL_IMPORTANT );
	OspSetScrnLogLevel( AID_MCU_PERIEQPSSN, LOGLVL_IMPORTANT );
	return;
}

API void setDebugLogFile(void)
{
	OspSetFileLogLevel( INVALID_APP, LOGLVL_DEBUG2 );
	OspSetFileLogLevel( AID_MCU_AGENT, LOGLVL_DEBUG2 );
	OspSetFileLogLevel( AID_MCU_VC, LOGLVL_DEBUG2 );
	OspSetFileLogLevel( AID_MCU_GUARD, LOGLVL_DEBUG2 );
	OspSetFileLogLevel( AID_MCU_MCSSN, LOGLVL_DEBUG2 );
	OspSetFileLogLevel( AID_MCU_PERIEQPSSN, LOGLVL_DEBUG2 );	
	return;
}

API void setReleaseLogFile( void )
{
	OspSetFileLogLevel( INVALID_APP, LOGLVL_EXCEPTION );
	OspSetFileLogLevel( AID_MCU_AGENT, LOGLVL_EXCEPTION );
	OspSetFileLogLevel( AID_MCU_VC, LOGLVL_EXCEPTION );
	OspSetFileLogLevel( AID_MCU_GUARD, LOGLVL_EXCEPTION );
	OspSetFileLogLevel( AID_MCU_MCSSN, LOGLVL_EXCEPTION );
	OspSetFileLogLevel( AID_MCU_PERIEQPSSN, LOGLVL_EXCEPTION );	
	return;
}

API void setReleaseTrcAll( void )
{
	OspSetTrcFlag( INVALID_APP, 0, 0 );
	OspSetTrcFlag( AID_MCU_AGENT, 0, 0 );
	OspSetTrcFlag( AID_MCU_VC, 0, 0 );
	OspSetTrcFlag( AID_MCU_GUARD, 0, 0 );
	OspSetTrcFlag( AID_MCU_MCSSN, 0, 0 );
	OspSetTrcFlag( AID_MCU_PERIEQPSSN, 0, 0 );
	
	return;
}

API void setReleaseLogAll( void )
{
	OspSetLogLevel( INVALID_APP, LOGLVL_EXCEPTION, LOGLVL_IMPORTANT );
	OspSetLogLevel( AID_MCU_AGENT, LOGLVL_EXCEPTION, LOGLVL_IMPORTANT );
	OspSetLogLevel( AID_MCU_VC, LOGLVL_EXCEPTION, LOGLVL_IMPORTANT );
	OspSetLogLevel( AID_MCU_GUARD, LOGLVL_EXCEPTION, LOGLVL_IMPORTANT );
	OspSetLogLevel( AID_MCU_MCSSN, LOGLVL_EXCEPTION, LOGLVL_IMPORTANT );
	OspSetLogLevel( AID_MCU_PERIEQPSSN, LOGLVL_EXCEPTION, LOGLVL_IMPORTANT );
	
	return;
}

API void userinit()
{
	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("MCU: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("MCU: StartCaptureException Succeeded.\n");
    }
#endif

#ifdef _VXWORKS_
	if(BrdDrvLibInit()==ERROR)
	{
		printf("\nBrdDrvLibInit() return ERROR,Quit now!\n");
        return;
	}
#endif
	
#ifdef _LINUX_
    if (BrdInit() == ERROR)
	{
		printf("\nBrdInit() return ERROR,Quit now!\n");
        return;
	}
#endif
    
    BOOL32 bRet = CreateConfStoreFile();
    if ( !bRet )
    {
        return;
    }

	//尝试从调试配置文件中读出本mcu侦听连接的端口，尝试失败采用默认值
	g_cMcuVcApp.GetBaseInfoFromDebugFile();
    g_cMcuVcApp.GetSatInfoFromDebugFile();
	u16 wMcuTelnetPort = g_cMcuVcApp.GetMcuTelnetPort();
	u16 wMcuListenPort = g_cMcuVcApp.GetMcuListenPort();

	//OSP initialization
    #ifdef WIN32
        OspInit( g_bOpenTelnetWin32, 0 );
        OspRegistModule("kdv323.dll");
    #else
        OspInit( TRUE, wMcuTelnetPort );
    #endif	

    //Osp telnet 初始授权 [11/28/2006-zbq]
#if !( defined(WIN32) || defined(_X86_) )
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

#ifdef _LINUX_
    #ifdef _MINIMCU_
        CodecWrapperPreInit();
    #endif
#endif

    McuAPIRegInLinux();

    OspPrintf( TRUE, FALSE, "MCU Version: %s\n", GetMcuVersion() );

	AddEventStr();

//    s8 szLogFile[KDV_MAX_PATH];
//    sprintf( szLogFile, "%s/mcu.log", DIR_DATA );
//	  OspOpenLogFile( szLogFile, 1000, 5 );

    u8 byMcuType = GetMcuPdtType();
	
	//创建连接用的监听端口
	OspCreateTcpNode( 0, wMcuListenPort);
	
	//初始化主备板倒换业务设置，并设置主备用状态
	TMSStartParam tMSStartParam;
	tMSStartParam.m_byMcuType = byMcuType;
	tMSStartParam.m_wMcuListenPort = wMcuListenPort;
	tMSStartParam.m_bMSDetermineType = g_cMcuVcApp.GetMSDetermineType();
	BOOL32 bMpcEnable = g_cMSSsnApp.InitMSConfig(tMSStartParam);

	//MCU代理，设置为主备用状态
	McuAgentInit(byMcuType, bMpcEnable);       //agent reads config file
	
	// 检查当前MCU主备用状态，如果为OFFLINE，则重启MCU, zgc, 2007-03-20
	if( MCU_MSSTATE_OFFLINE == g_cMSSsnApp.GetCurMSState() )
	{
        printf("\nERROR: The Mcu state is OFFLINE! \n");
//		OspDelay( 30000 ); // 防止不断重启, 给调试留出一定时间, zgc, 2007-03-22        
//		g_cMSSsnApp.RebootMCU();
	}
	
	
	//获取指定MCU Debug文件关于指定终端的协议适配板资源及码流转发板资源的设置
	g_cMcuVcApp.GetMtCallInterfaceInfoFromDebugFile();
	
	//初始化设备列表
	g_cMcuVcApp.InitPeriEqpList();
    
    //初始化DCS列表
    g_cMcuVcApp.InitPeriDcsList();
    
    //N+1备份 与 热备份互斥
    if (!g_cMSSsnApp.IsDoubleLink())
    {
        //初始化N+1备份状态
        BOOL32 bNPlusMode = g_cNPlusApp.InitNPlusState();
        if (bNPlusMode)
        {
            g_cNPlusApp.CreateApp("McuNPlus", AID_MCU_NPLUSMANAGER, APPPRI_MCUNPLUS, 600, 512<<10);
            OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), OSP_POWERON);
        }
    }    

	//MCU业务
#ifdef _X86_    
	g_cMcuVcApp.CreateApp("McuVc", AID_MCU_VC, APPPRI_MCUVC, 4000, 512<<10);
#else
    g_cMcuVcApp.CreateApp("McuVc", AID_MCU_VC, APPPRI_MCUVC, 4000, 2<<20);
#endif
#ifndef _X86_
	//MCU守卫
	g_cMcuGuardApp.CreateApp("McuGuard", AID_MCU_GUARD, APPPRI_MCUGUARD, 100);
    g_cMcuGuardApp.SetPriority(APPPRI_MCUGUARD);
#endif
	//MCU会议控制台会话
	g_cMcsSsnApp.CreateApp( "McSsn", AID_MCU_MCSSN, APPPRI_MCUSSN, 2000, 500<<10 );
	//MCU外设会话
	g_cEqpSsnApp.CreateApp( "EqpSsn", AID_MCU_PERIEQPSSN, APPPRI_MCUSSN, 2000, 500<<10 );
	//终端适配会话
	g_cMtAdpSsnApp.CreateApp("MtAdpSsn", AID_MCU_MTADPSSN, APPPRI_MCUSSN, 4000, 500<<10);
	//MCU的MP会话
	g_cMpSsnApp.CreateApp("MpSsn", AID_MCU_MPSSN, APPPRI_MCUSSN, 2000 );
    //mcu 配置界面化
	g_cMcuCfgApp.CreateApp("McuConfig", AID_MCU_CONFIG, APPPRI_MCUCONFIG, 100, 500<<10);
	//mcu T120集成
	g_cDcsSsnApp.CreateApp( "DcsSsn", AID_MCU_DCSSSN, APPPRI_MCUSSN, 200, 500<<10 );
	//VCS会话
	g_cVcsSsnApp.CreateApp( "VcSsn", AID_MCU_VCSSN, APPPRI_MCUSSN, 2000, 500<<10 );
    
//#ifdef _SATELITE_
    
    //卫星MT会话
    g_cMtSsnApp.CreateApp( "MtSsn", AID_MCU_MTSSN, APPPRI_MTADP, 1000);
    
    //卫星modem会话
    g_cModemSsnApp.CreateApp( "ModemSsn", AID_MCU_MODEMSSN, APPPRI_MTADP, 1000);

    //接收终端UDP
    g_cUdpDispatchApp.CreateApp( "UdpDispatchSsn", AID_UDPDISPATCH, 90, 50 );

    //Modem控制
	g_cUdpNmsApp.CreateApp( "UDPNMS", AID_UDPNMS, 80, 2 );
    
    MdcomInit();

    s8 achProfileName[32];
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_TOPOCFG);
    g_wMcuTopoNum = ReadMcuTopoTable( achProfileName, g_atMcuTopo, MAXNUM_TOPO_MCU );
    g_wMtTopoNum = ReadMtTopoTable( achProfileName, g_atMtTopo, MAXNUM_TOPO_MT );
    //读入MODEM信息
    g_byMcuModemNum = ReadMcuModemTable( achProfileName, g_atMcuModemTopo, 16 );

    printf("[userinit] McuTopoNum.%d MtTopoNum.%d, ModemTopoNum.%d!\n",
            g_wMcuTopoNum, g_wMtTopoNum, g_byMcuModemNum );

//#endif

	//set log and trace limit
	setReleaseTrcAll();
	setReleaseLogAll();
	
    // guzh [1/16/2007] 通过debug文件设置
	if ( g_cMcuAgent.GetIsUseMpcTranData() )
	{
		//存在启用主备倒换服务下将不启动内嵌local_mp_service
		if (FALSE == g_cMSSsnApp.IsDoubleLink())
		{
			StartLocalMp();
/*
            // 在Vx8000的MPC上, 启动Mp大概需要5M内存，数量-3
            #if defined(_VXWORKS_) && !defined(_MINIMCU_)
                g_cMcuVcApp.m_byMaxOngoConfSupported -= 3;    
            #endif
*/
		}
		else
		{
			OspPrintf( TRUE, FALSE, "Local_Mp_Service dont startup under doublelinking mode\n");
		}
	}

	if (g_cMcuAgent.GetIsUseMpcStack())
	{
		//存在启用主备倒换服务下将不启动内嵌local_mtadp_service
		if (FALSE == g_cMSSsnApp.IsDoubleLink())
		{
			StartLocalMtadp();
/*
            // 在Vx或者Linux 8000的MPC上
            // 启动协议大概需要23M内存，数量-12
            #if defined(_VXWORKS_) && !defined(_MINIMCU_)
            g_cMcuVcApp.m_byMaxOngoConfSupported -= 12;
            #endif           
*/
		}
		else
		{
			OspPrintf( TRUE, FALSE, "Local_Mtadp_Service dont startup under doublelinking mode\n");
		}
	}
    
	//不定时打印状态
	OspStatusMsgOutSet(FALSE);

	char achMcuAlias[32];
	g_cMcuAgent.GetMcuAlias(achMcuAlias, sizeof(achMcuAlias));
	OspPrintf(TRUE, FALSE, "MCU: %s DoubleLink.%d Init Success!\n", achMcuAlias, g_cMSSsnApp.IsDoubleLink());

	// 初始化
	OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), OSP_POWERON);
    OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), OSP_POWERON);
	OspPost(MAKEIID(AID_MCU_VCSSN, CInstance::DAEMON), OSP_POWERON);
	OspPost(MAKEIID(AID_MCU_GUARD), OSP_POWERON);
    
    OspPost(MAKEIID(AID_MCU_MTSSN, CInstance::DAEMON), OSP_POWERON);
    OspPost(MAKEIID(AID_UDPDISPATCH, 1), OSP_POWERON);
    OspPost(MAKEIID(AID_UDPNMS, 1), OSP_POWERON);


    // win32/redhat only for test
#if defined(WIN32) || defined(_X86_) 
    g_cMcuVcApp.SetLicenseNum(MAXNUM_MCU_MT);
	g_cMcuVcApp.SetVCSAccessNum(MAXNUM_MCU_VC);
#else
	// license
	::OspPost( MAKEIID( AID_MCU_GUARD, 1), MCUGD_GETLICENSE_DATA, NULL, 0 );	
#endif
	
	//启动外设
	//存在启用主备倒换服务下将不启动内嵌local_eqp_service
	if (FALSE == g_cMSSsnApp.IsDoubleLink())
	{
		StartLocalEqp();
	}
	else
	{
		OspPrintf( TRUE, FALSE, "Local_Eqp_Service dont startup under doublelinking mode\n");
	}

	return;
}

API void userquit()
{
#ifdef WIN32
    McuAgentQuit(FALSE);
    OspDelay(100);
    OspQuit();
#else
    if (!g_cMSSsnApp.IsDoubleLink())
    {
        // 不是双链
        OspQuit();
    }
#endif
}

API void Reboot()
{
	userquit();
#ifndef WIN32
    BrdHwReset();
#else
    PostMessage( g_hwndMain, WM_CLOSE, NULL, NULL );
#endif
    
}

/*====================================================================
    函数名      ：mcuShowAllConf
    功能        ：打印MCU版本信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/18    1.0         LI Yi         创建
====================================================================*/
API void mcuver( void )
{
    u8 byMcuType = GetMcuPdtType();
    s8 szMcuType[16] = {0};
    switch (byMcuType)
    {
    case MCU_TYPE_KDV8000:
        strcpy(szMcuType, "8000");
        break;
    case MCU_TYPE_KDV8000B:
        strcpy(szMcuType, "8000B");
        break;
	case MCU_TYPE_KDV8000B_HD:
		strcpy(szMcuType, "8000B-HD");
		break;
    case MCU_TYPE_KDV8000C:
        strcpy(szMcuType, "8000C");
        break;
    default:
        strcpy(szMcuType, "WIN32");
        break;
    }
    OspPrintf( TRUE, FALSE, "MCU(%s) Version: %s\n", 
               szMcuType, GetMcuVersion()  );
    
	OspPrintf( TRUE, FALSE, "Module Ver: %s  Compile Time: %s, %s\n", 
               VER_MCU, __TIME__, __DATE__ );
	ospver();
	mcuagtver();
	dsver();
    OspPrintf(TRUE, FALSE, "\n");
	mpver();
	mtadpver();
	//prsver();
	addrbookver();
	umver();
}

/*====================================================================
    函数名      ：showprint
    功能        ：显示分类打印命令 detail
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/25    3.0         胡昌威         创建
====================================================================*/
API void showprint( )
{
	OspPrintf( TRUE, FALSE, "MCU消息分类打印命令列表:\n" ); 
	OspPrintf( TRUE, FALSE, "1.  pmtmsg:       打印与终端的自定义消息.\n" );
	OspPrintf( TRUE, FALSE, "1.1 npmtmsg:      关闭打印与终端的自定义消息.\n" );
	OspPrintf( TRUE, FALSE, "2.  pmtadpmsg:    打印发向MtAdp的自定义消息.\n" );
	OspPrintf( TRUE, FALSE, "2.1 npmtadpmsg:   关闭打印发向MtAdp的自定义消息.\n" );
	OspPrintf( TRUE, FALSE, "3.  pmpmgrmsg:    打印与MP的消息.\n" );
	OspPrintf( TRUE, FALSE, "3.1 npmpmgrmsg:   关闭打印与MP的消息.\n" );
	OspPrintf( TRUE, FALSE, "4.  peqpmsg:      打印与外设的消息.\n" );
	OspPrintf( TRUE, FALSE, "4.1 npeqpmsg:     关闭打印与外设的消息.\n" );
	OspPrintf( TRUE, FALSE, "5.  pmcsmsg:      打印与会控的消息.\n" );
	OspPrintf( TRUE, FALSE, "5.1 npmcsmsg:     关闭打印与会控的消息.\n" );
	OspPrintf( TRUE, FALSE, "6.  pcallmsg:     打印终端呼叫信息.\n" );
	OspPrintf( TRUE, FALSE, "6.1 npcallmsg:    关闭打印终端呼叫信息.\n" );
	OspPrintf( TRUE, FALSE, "7.  pmmcumsg:     打印级联业务信息.\n" );
	OspPrintf( TRUE, FALSE, "7.1 npmmcumsg:    关闭打印级联业务信息.\n" );
	OspPrintf( TRUE, FALSE, "8.  pgdmsg:       打印GUARD信息.\n" );
	OspPrintf( TRUE, FALSE, "8.1 npgdmsg:      关闭打印GUARD信息.\n" );	
	OspPrintf( TRUE, FALSE, "9.  pcfgmsg:      打印配置界面化信息.\n" );
	OspPrintf( TRUE, FALSE, "9.1 npcfgmsg:     关闭打印配置界面化信息.\n" );
	OspPrintf( TRUE, FALSE, "10.  pdcsmsg:     打印DCS与MCU的DCS会话的消息.\n" );
	OspPrintf( TRUE, FALSE, "10.1 npdcsmsg:    关闭打印DCS与MCU的DCS会话的消息.\n" );	
	OspPrintf( TRUE, FALSE, "11.  pmcudcsmsg:  打印MCU的DCS会话与MCU业务的信息.\n" );
	OspPrintf( TRUE, FALSE, "11.1 npmcudcsmsg: 关闭打印MCU的DCS会话与MCU业务的信息.\n" );
    OspPrintf( TRUE, FALSE, "12.  ppfmmsg:     打印8000B性能限制消息.\n");
    OspPrintf( TRUE, FALSE, "12.1 nppfmmsg:    关闭打印8000B性能限制消息.\n");
    OspPrintf( TRUE, FALSE, "13.  pvcsmsg:     打印与VCS的消息.\n");
    OspPrintf( TRUE, FALSE, "13.1 npvcsmsg:    关闭打印VCS的消息.\n");
    OspPrintf( TRUE, FALSE, "14.  psatmsg:     打印卫星会议的消息.\n");
    OspPrintf( TRUE, FALSE, "14.1 npsatmsg:    关闭打印卫星会议的消息.\n");
    OspPrintf( TRUE, FALSE, "15.  pmdmmsg:     打印卫星会议modem会话的消息.\n");
    OspPrintf( TRUE, FALSE, "15.1 npmdmmsg:    关闭打印卫星会议modem会话的消息.\n");
    OspPrintf( TRUE, FALSE, "16.  pmdmpxymsg:  打印卫星会议modem代理的消息.\n");
    OspPrintf( TRUE, FALSE, "16.1 npmdmpxymsg: 关闭打印卫星会议modem代理的消息.\n");
	OspPrintf( TRUE, FALSE, "16.1 setmdmpxylog:设置打印卫星会议modem代理的消息级别(0-Err/default, 1-warning, 2-info, 3-detail, 4-verbose).\n");

	return;
}

/*====================================================================
    函数名      ：showconfprint
    功能        ：显示会议打印命令 detail
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威         创建
====================================================================*/
API void showconfprint()
{
	OspPrintf( TRUE, FALSE, "会议信息打印命令列表:\n" ); 
	OspPrintf( TRUE, FALSE, "1. showconfall:    打印会议所有信息.\n" );
	OspPrintf( TRUE, FALSE, "2. showconfinfo:   打印会议信息.\n" );
	OspPrintf( TRUE, FALSE, "3. showconfswitch: 打印会议交换信息.\n" );
	OspPrintf( TRUE, FALSE, "4. showmtmonitor:  打印终端监控信息.\n" );
	OspPrintf( TRUE, FALSE, "5. showconfmt:     打印会议终端信息.\n" );
	OspPrintf( TRUE, FALSE, "6. showconfeqp:    打印会议外设信息.\n" );
	OspPrintf( TRUE, FALSE, "7. showmtstat:     打印终端状态.\n" );
	OspPrintf( TRUE, FALSE, "8. showtemplate:   打印会议模板信息.\n" );
	OspPrintf( TRUE, FALSE, "9. showconfmap:    打印会议ID映射表信息.\n" );
	OspPrintf( TRUE, FALSE, "10. showbas:		打印会议的适配信息.\n" );
	OspPrintf( TRUE, FALSE, "11. showbasgrp:	打印会议需要适配的群组.\n");
    OspPrintf( TRUE, FALSE, "12. showbasall:    打印MCU所有的HD-BAS信息");
	OspPrintf( TRUE, FALSE, "13. showmodem:		打印卫星会议modem当前的状态信息.\n");
	OspPrintf( TRUE, FALSE, "14. showconffreq:	打印卫星会议当前的频率和带宽状态信息.\n");
	

	return;
}


/*====================================================================
    函数名      ：showconfall
    功能        ：打印MCU下会议所有信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威         创建
====================================================================*/
API void showconfall( void )
{	
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCONFALL ) );
}

/*====================================================================
    函数名      ：showconfinfo
    功能        ：打印MCU下会议信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威         创建
====================================================================*/
API void showconfinfo( void )
{	
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCONFINFO ) );
}

/*====================================================================
    函数名      ：showconfswitch
    功能        ：打印会议交换信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威         创建
====================================================================*/
API void showconfswitch( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCONFSWITCH ) );
}

/*====================================================================
    函数名      svam
    功能        ：打印会议交换信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/03/27    4.6         薛亮          创建
====================================================================*/
API void svam( void)
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPVMPADAPT ) ); 
}

API void dumpmodemssn( void )
{
    OspInstDump( AID_MCU_MODEMSSN, CInstance::EACH, 0 ); 
}

API void dumpmtssn( void )
{
    OspInstDump( AID_MCU_MTSSN, CInstance::EACH, 0 ); 
}

/*====================================================================
    函数名      ：showconfmt
    功能        ：打印会议交换终端信息
    算法实现    ：默认参数打印所有会议的所有终端的基本信息，不含能力级；
                  byConfIdx!=0，byMtId=0打印某会议的终端基本信息，不含能力级；
                  byConfIdx!=0, byMtId!=0打印具体终端的基本信息和能力级.
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威          创建
    07/12/21    4.0         张宝卿          增加区分打印和能力级打印
====================================================================*/
API void showconfmt( u8 byConfIdx = 0, u8 byMtIdx = 0 )
{
    if ( 0 != byConfIdx )
    {
        if ( byConfIdx > MAX_CONFIDX )
        {
            OspPrintf( TRUE, FALSE, "Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            OspPrintf( TRUE, FALSE, "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
            return;
        }
        else
        {
            u16 wInsId = pVcInst->GetInsID();
            u16 wParam = (u16)byMtIdx;
            OspInstDump( AID_MCU_VC, wInsId, MAKEDWORD( wParam, DUMPCONFMT ) );
        }
    }
    else
    {
        OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCONFMT ) );        
    }
}

API void changemtres( u8 byConfIdx = 0, u8 byMtIdx = 0, u8 byRes = 3 )
{
    if ( 0 != byConfIdx )
    {
        if ( byConfIdx > MAX_CONFIDX )
        {
            OspPrintf( TRUE, FALSE, "Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            OspPrintf( TRUE, FALSE, "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
            return;
        }
        else
        {
            u16 wInsId = pVcInst->GetInsID();
            u16 wParam = MAKEWORD(byMtIdx, byRes);
            OspInstDump( AID_MCU_VC, wInsId, MAKEDWORD( wParam, DUMPCHGMTRES ) );
        }
    }
    else
    {
        OspPrintf( TRUE, FALSE, "confIdx.%d is illegal, try again ...\n" );
    }
}

/*====================================================================
    函数名      ：rlscf
    功能        ：结束会议
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/18    4.5         张宝卿          创建
====================================================================*/
API void rlscf(u8 byConfIdx)
{
    if ( byConfIdx > MAX_CONFIDX || byConfIdx == 0)
    {
        OspPrintf( TRUE, FALSE, "Unexpected confIdx.%d, try again ...\n", byConfIdx );
        return;
    }
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if ( NULL == pVcInst )
    {
        OspPrintf( TRUE, FALSE, "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
        return;
    }
    else
    {
        u16 wInsId = pVcInst->GetInsID();
        u16 wParam = (u16)byConfIdx;
        OspInstDump( AID_MCU_VC, wInsId, MAKEDWORD( wParam, DUMPRLSCONF ) );
    }
    return;
}

API void rlsbas(u8 byConfIdx)
{
    if ( byConfIdx > MAX_CONFIDX || byConfIdx == 0)
    {
        OspPrintf( TRUE, FALSE, "Unexpected confIdx.%d, try again ...\n", byConfIdx );
        return;
    }
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if ( NULL == pVcInst )
    {
        OspPrintf( TRUE, FALSE, "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
        return;
    }
    else
    {
        u16 wInsId = pVcInst->GetInsID();
        u16 wParam = (u16)byConfIdx;
        OspInstDump( AID_MCU_VC, wInsId, MAKEDWORD( wParam, DUMPRLSBAS ) );
    }
    return;
}

/*====================================================================
    函数名      ：showbas
    功能        ：打印当前的适配分配
    算法实现    ：默认参数打印所有会议的所有适配的分配信息；
                  byConfIdx!=0，byBasId!=0打印某会议的某适配的基本信息；
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/09/03    4.0         张宝卿          创建
====================================================================*/
API void showbas( u8 byConfIdx, u8 byBasId )
{
	if ( 0 != byConfIdx )
    {
        if ( byConfIdx > MAX_CONFIDX )
        {
            OspPrintf( TRUE, FALSE, "Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            OspPrintf( TRUE, FALSE, "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
            return;
        }
        else
        {
            u16 wInsId = pVcInst->GetInsID();
            u16 wParam = (u16)byBasId;
            OspInstDump( AID_MCU_VC, wInsId, MAKEDWORD( wParam, DUMPBASINFO ) );
        }
    }
    else
    {
        OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPBASINFO ) );        
    }

    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId ++)
    {
        TPeriEqpStatus tStatus;
        
        if (!g_cMcuVcApp.IsPeriEqpValid(byEqpId))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
        {
            continue;   
        }
        
        g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
        
        TPeriEqpStatus * ptStatus = &tStatus;
        
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }

        u8 byChnIdx = 0;
        u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();

        if (byEqpType == TYPE_MAU_NORMAL || byEqpType == TYPE_MAU_H263PLUS)
        {
            if ( byEqpType== TYPE_MAU_NORMAL)
            {
                OspPrintf(TRUE, FALSE, "BAS.%d(mau-normal) chan status:\n", byEqpId);
            }
            if (byEqpType == TYPE_MAU_H263PLUS)
            {
                OspPrintf(TRUE, FALSE, "BAS.%d(mau-h263p) chan status:\n", byEqpId);
            }
            for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {
                OspPrintf(TRUE, FALSE, "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->GetStatus());
            }
            
            for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_DVCHN; byChnIdx++)
            {
                OspPrintf(TRUE, FALSE, "\tds <chnIdx.%d, Res.%d, Status.%d>\n",
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx)->GetChnIdx(),
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx)->GetReservedType(),
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx)->GetStatus());
            }
        }
        else if (byEqpType == TYPE_MPU)
        {
            OspPrintf(TRUE, FALSE, "BAS.%d(mpu) chan status:\n", byEqpId);
            for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_VCHN; byChnIdx++)
            {
                OspPrintf(TRUE, FALSE, "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
                    ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
                    ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
                    ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetStatus());
            }
        }
    }
}

API void showbasall()
{
    OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( 0, DUMPBASINFO ) );
}

API void showmodem( void )
{
	g_cMcuVcApp.DisplayModemInfo();
}

API void showconffreq( void )
{
	g_cMcuVcApp.ShowConfFreqInfo();
} 

API void showsatinfo( void )
{
	g_cMcuVcApp.ShowSatInfo();
}

/*====================================================================
    函数名      ：showmtmonitor
    功能        ：打印终端监控信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威         创建
====================================================================*/
API void showmtmonitor( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPMTMONITOR ) );
}

/*====================================================================
    函数名      ：showconfeqp
    功能        ：打印会议外设信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威         创建
====================================================================*/
API void showconfeqp( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCONFEQP ) );
}

/*====================================================================
    函数名      ：showmtstat
    功能        ：打印终端状态
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威         创建
====================================================================*/
API void showmtstat( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPMTSTAT ) );
}

/*=============================================================================
    函 数 名： showtemplate
    功    能： 打印会议模板信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： API void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/1/10   4.0			万春雷                  创建
=============================================================================*/
API void showtemplate( void )
{
    OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD(0, DUMPTEMPLATE) );
}

/*=============================================================================
    函 数 名： showconfmap
    功    能： 打印会议ID映射表信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： API void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/1/10   4.0			万春雷                  创建
=============================================================================*/
API void showconfmap(void)
{
    OspInstDump(AID_MCU_VC, CInstance::DAEMON, MAKEDWORD(0, DUMPCONFMAP));
}

/*====================================================================
    函数名      ：showmc
    功能        ：打印MCU连接所有会议控制台信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/18    1.0         LI Yi         创建
====================================================================*/
API void showmc( void )
{
	OspPrintf( TRUE, FALSE, "Data Information In McSsn:\n" );
	OspPrintf( TRUE, FALSE, "INSTID NODE \n" );
	OspPrintf( TRUE, FALSE, "------ ---- \n" );
	
	OspInstDump( AID_MCU_MCSSN, CInstance::EACH, 0 );
	OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( 0, DUMPMCINFO ) );
}

/*====================================================================
    函数名      ：showmcsrc
    功能        ：打印mcs接收/发送媒体源
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/12    1.0         JQL         创建
====================================================================*/
API void showmcsrc( void )
{
	OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( DUMPSRC, DUMPMCINFO) );
}

/*=============================================================================
函 数 名： shownplus
功    能： 打印n＋1备份信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
API void shownplus()
{
    TLocalInfo tLocalInfo;
    g_cMcuAgent.GetLocalInfo( &tLocalInfo );
    s8 achState[32] = "IDLE";
    if (MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState())
    {
        strcpy(achState, "N-Mode-IDLE");
        OspPrintf(TRUE, FALSE, "LocalNPlusState: %s, Configed backup MCU.0x%x\n", 
                                         achState, tLocalInfo.GetNPlusMcuIp() );
    }
    else if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        strcpy(achState, "N-Mode-Connected");
        OspPrintf(TRUE, FALSE, "LocalNPlusState: %s, Configed backup MCU.0x%x\n", 
                                         achState, tLocalInfo.GetNPlusMcuIp() );
    }
    else if (MCU_NPLUS_SLAVE_IDLE == g_cNPlusApp.GetLocalNPlusState())
    {
        strcpy(achState, "1-Mode-IDLE");
        OspPrintf(TRUE, FALSE, "LocalNPlusState: %s RtdTime:%d RtdNum:%d\n", 
                  achState, g_cNPlusApp.GetRtdTime(), g_cNPlusApp.GetRtdNum());
    }
    else if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        strcpy(achState, "1-Mode-Switched");
        OspPrintf(TRUE, FALSE, "LocalNPlusState: %s RtdTime:%d RtdNum:%d, switched ip<0x%x>\n", 
            achState, g_cNPlusApp.GetRtdTime(), g_cNPlusApp.GetRtdNum(), g_cNPlusApp.GetMcuSwitchedIp() );
    }
    else
    {
        strcpy(achState, "NONE N+1");
    }
    OspInstDump( AID_MCU_NPLUSMANAGER, CInstance::EACH, 0 );
}

/*====================================================================
    函数名      ：showDri
    功能        ：显示DRI信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u8 byDriId DRI编号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
====================================================================*/
API void showdri( u8 byDriId )
{
	g_cMcuVcApp.ShowDri( byDriId );
}

/*=============================================================================
    函 数 名： showalluser
    功    能： 
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： API void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/1/10   4.0			万春雷                  创建
=============================================================================*/
API void showalluser(void)
{
	g_cUsrManage.CmdLine( 0 );
    g_cMcuVcApp.PrtUserGroup();
}

/*====================================================================
    函数名      ：showeqp
    功能        ：打印MCU连接所有外设信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/21    1.0         LI Yi         创建
====================================================================*/
API void showeqp( void )
{
	OspPrintf( TRUE, FALSE, "\nData Information In EqpSsn and DcsSsn:\n" );

	OspPrintf( TRUE, FALSE, "INSTID EQPID NODE TYPE   ALIAS\n" );
	OspPrintf( TRUE, FALSE, "------ ----- ---- ----   -------\n" );
	OspInstDump( AID_MCU_PERIEQPSSN, CInstance::EACH, 0 );
    OspInstDump( AID_MCU_DCSSSN,     CInstance::EACH, 0 );
	OspPrintf( TRUE, FALSE, "________________________________\n" );
    
	OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( 0, DUMPEQPINFO) );
}

API void eqp( u8 byEqpId )
{
	OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( byEqpId, DUMPEQPSTATUS) );
}

API void killGuardTimer( void )
{
	OspPost( MAKEIID( AID_MCU_GUARD, 1 ), MCU_STOPGUARDTIMER_CMD );
}

API void setGuardTimer( void )
{
	OspPost( MAKEIID( AID_MCU_GUARD, 1 ), MCU_STARTGUARDTIMER_CMD );
}

API void showaddrbook( void )
{
    if(NULL != g_pcAddrBook)
    {
        g_pcAddrBook->PrintAddrBook();
    }    
}

API void setsmcucasport( u16 wCasPort )
{
    g_cMcuVcApp.SetSMcuCasPort(wCasPort);
}

API void showdebug( )
{
    g_cMcuVcApp.ShowDebugInfo();
}

API void showgk( void )
{
    if( g_cMcuVcApp.GetConfRegState(0) )
    {
        OspPrintf( TRUE, FALSE, "Mcu has registered to GK!\n" );
    }
    else
    {
        OspPrintf( TRUE, FALSE, "Mcu not register to GK!\n" );
    }
}

/*====================================================================
    函数名      : mcutau
    功能        ：mcu telnet 授权
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/11/28    4.0         张宝卿        创建
====================================================================*/
API void mcutau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

/*====================================================================
    函数名      ：mpmanagerhelp
    功能        ：屏幕打印Mp帮助
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/
API void mpmanagerhelp()
{
	OspPrintf(TRUE,FALSE,"\nshowmp()-----show mp info\n");
	OspPrintf(TRUE,FALSE,"ssw(%d)--------show switch info\n");
	OspPrintf(TRUE,FALSE,"showbridge()---show switch bridge info\n");

	return;
}

// guzh  [12/13/2006] NPLUS测试
API void nplusreset()
{
    OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), VC_NPLUS_RESET_NOTIF);
}

/*====================================================================
    函数名      ：mcuhelp
    功能        ：print the MCU's help commands 
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/28    1.0         廖伟江        创建
    2006/1/10   4.0			万春雷        修改
====================================================================*/
API void mcuhelp( void )
{
	OspPrintf( TRUE, FALSE, "MCU Ver: %s   Compile Time: %s, %s\n", VER_MCU, __TIME__, __DATE__ );
    // OspPrintf( TRUE, FALSE, "Max Ongoing Conf Supported : %d\n", g_cMcuVcApp.m_byMaxOngoConfSupported );
    // telnet 授权
    OspPrintf( TRUE, FALSE, "mcutau(UsrName,Pwd): mcu telnet author.\n" );

	OspPrintf( TRUE, FALSE, "\nFollowing are all debug commands in MCU module:\n" );
	OspPrintf( TRUE, FALSE, "mcuver:             print version of current MCU.\n" );
	OspPrintf( TRUE, FALSE, "Reboot:             Reboot current MCU.\n\n" );
    OspPrintf( TRUE, FALSE, "showdebug:          print mcu debug and performance info.\n" );
	
	OspPrintf( TRUE, FALSE, "showconfprint:      show conf print command.\n" );
	OspPrintf( TRUE, FALSE, "showprint:          show mcu print command.\n" );
	OspPrintf( TRUE, FALSE, "mpmanagerhelp:      show mpmanager print command.\n\n" );

	//master-slave
	OspPrintf( TRUE, FALSE, "msdebug(nDbgLvl):   set the debug level of master-slave.\n" );
	OspPrintf( TRUE, FALSE, "showmsstate:        show master-slave state.\n" );
    OspPrintf( TRUE, FALSE, "setmschecktime:     set master-slave check time.\n\n");

    //n+1
    OspPrintf( TRUE, FALSE, "shownplus:          show nplus state.\n\n");
    OspPrintf( TRUE, FALSE, "nplusreset:         Reset Nplus Backup server to idle. All conf will lose.\n\n");
    
    // mcu guard
    OspPrintf( TRUE, FALSE, "mcuguarddisable:    disable mcu guard reboot.\n" );
    OspPrintf( TRUE, FALSE, "mcuguardenable:     enable mcu guard reboot.\n" );
	OspPrintf( TRUE, FALSE, "showlicense:        show license.\n" );
	
	//mcs
	OspPrintf( TRUE, FALSE, "showmc:             print information of all mcs.\n" );
	OspPrintf( TRUE, FALSE, "showmcsrc:          print information of all mcs rcv_or_snd_channel.\n" );
	OspPrintf( TRUE, FALSE, "showalluser:        show all user mcs can use.\n\n" );

	//cri/dri reg info
	OspPrintf( TRUE, FALSE, "showdri:            print information of all cri/dri.\n" );
	OspPrintf( TRUE, FALSE, "showmtadp:          print information of all mtadp module.\n" );
	OspPrintf( TRUE, FALSE, "showmp:             print information of all mp module.\n\n" );

	//eqp reg info
	OspPrintf( TRUE, FALSE, "showeqp:            print information of all eqp module.\n" );
	OspPrintf( TRUE, FALSE, "eqp(EqpId):         show Eqp status with param EqpId.\n\n" );
	
	//mcu->gk info
	OspPrintf( TRUE, FALSE, "showgk:             print register GK info.\n\n" );

    //addr
    OspPrintf( TRUE, FALSE, "showaddrbook:       print address book info.\n\n" );

	//依赖模块
	OspPrintf( TRUE, FALSE, "umhelp:             print help of userlist book.\n" );
	OspPrintf( TRUE, FALSE, "addrhelp:           print help of address book.\n" );
	OspPrintf( TRUE, FALSE, "mcuagenthelp:       print help of mcuagent module.\n" );
	OspPrintf( TRUE, FALSE, "osphelp:            print help of osp module.\n\n" );
	
	//可选模块
	OspPrintf( TRUE, FALSE, "dshelp:             print help of dataswitch module.\n" );
	OspPrintf( TRUE, FALSE, "mphelp:             print help of mp module.\n" );
	OspPrintf( TRUE, FALSE, "mtadphelp:          print help of mtadp module.\n" );
	OspPrintf( TRUE, FALSE, "vmphelp:            print help of vmp module.\n" );
	OspPrintf( TRUE, FALSE, "mixhelp:            print help of mixer module.\n" );
	OspPrintf( TRUE, FALSE, "prshelp:            print help of prs module.\n" );
	OspPrintf( TRUE, FALSE, "netbufhelp:         print help of netbuf module.\n" );
	OspPrintf( TRUE, FALSE, "kdvmedianethelp:    print help of medianet module.\n\n" );

    //级联测试
    //OspPrintf( TRUE, FALSE, "showsmcucasport:    print SMCU cascade listen port(for debug only).\n");

	OspPrintf( TRUE, FALSE, "sconftotemp(conf name string):	save conf as template.\n\n");
    
	return;
}

#ifdef _LINUX_
API void querynipstate(void)
{
    s32 nRet = BrdQueryNipState();
    OspPrintf(TRUE, FALSE, "BrdQueryNipState ret.%d\n", nRet);
}
#endif

void McuAPIRegInLinux(void)
{
#ifdef _LINUX_    
    OspRegCommand("mcuver",             (void*)mcuver,          "Mcu version");
    OspRegCommand("mcuhelp",            (void*)mcuhelp,         "Mcu help");
    OspRegCommand("mcutau",             (void*)mcutau,          "Mcu telnet author");
    OspRegCommand("showconfprint",      (void*)showconfprint,   "show conf print");
    OspRegCommand("showprint",          (void*)showprint,       "show mcu message print control");
    OspRegCommand("mpmanagerhelp",      (void*)mpmanagerhelp,   "show mpmanager print");
    OspRegCommand("msdebug",            (void*)msdebug,         "set the debug level of master-slave");
    OspRegCommand("showmsstate",        (void*)showmsstate,     "show master-slave state");
    OspRegCommand("setmschecktime",     (void*)setmschecktime,  "set master-slave check time");
    OspRegCommand("showdebug",          (void*)showdebug,       "show mcu debug info.\n");
    
    OspRegCommand("Reboot",             (void*)Reboot,          "Reboot Mcu manually");
    OspRegCommand("mcuguarddisable",    (void*)mcuguarddisable, "disable mcu guard reboot");
    OspRegCommand("mcuguardenable",     (void*)mcuguardenable,  "enable mcu guard reboot");
	OspRegCommand("showlicense",		(void*)showlicense,     "show license");
    OspRegCommand("querynipstate",      (void*)querynipstate,   "query nip state");
    OspRegCommand("nplusreset",         (void*)nplusreset,      "Reset Nplus Backup server to idle.");    

    OspRegCommand("showmc",             (void*)showmc,          "print information of all mcs");
    OspRegCommand("showmcsrc",          (void*)showmcsrc,       "print information of all mcs rcv_or_snd_channel");
    OspRegCommand("showalluser",        (void*)showalluser,     "show all user mcs can use");
    OspRegCommand("showdri",            (void*)showdri,         "print information of all cri/dri");
    OspRegCommand("showmtadp",          (void*)showmtadp,       "print information of all mtadp module");
    OspRegCommand("showmp",             (void*)showmp,          "print information of all mp module");
    OspRegCommand("showbridge",         (void*)showbridge,      "print switch bridge info");
    OspRegCommand("showeqp",            (void*)showeqp,         "print information of all eqp module");
    OspRegCommand("eqp",                (void*)eqp,             "show Eqp status with param EqpId");
    OspRegCommand("showgk",             (void*)showgk,          "print register GK info");
    OspRegCommand("showaddrbook",       (void*)showaddrbook,    "print address book info");
    OspRegCommand("setsmcucasport",     (void*)setsmcucasport,  "set SMcu Cascade Listen port");
	OspRegCommand("ssw",                (void*)ssw,             "Show VC Switch");
    OspRegCommand("shownplus",          (void*)shownplus,       "Show NPlus State");
    
	OspRegCommand("svam",				(void*)svam,			"Show VMP adapter channel info");
    OspRegCommand("showconfall",        (void*)showconfall,     "show conf all info");
    OspRegCommand("showconfinfo",       (void*)showconfinfo,    "show conf info");
    OspRegCommand("showconfswitch",     (void*)showconfswitch,  "show conf switch");    
    OspRegCommand("showmtmonitor",      (void*)showmtmonitor,   "show mt monitor");    
    OspRegCommand("showconfmt",         (void*)showconfmt,      "show conf mt");    
    OspRegCommand("showconfeqp",        (void*)showconfeqp,     "show conf eqp");    
    OspRegCommand("showmtstat",         (void*)showmtstat,      "show mt stat");    
    OspRegCommand("showtemplate",       (void*)showtemplate,    "show template");    
    OspRegCommand("showconfmap",        (void*)showconfmap,     "show conf map");
	OspRegCommand("showbas",			(void*)showbas,         "show bas info");
    OspRegCommand("showbasall",			(void*)showbasall,      "show bas info");
	OspRegCommand("showmodem",			(void*)showmodem,		"show satelite conf modem info");
	OspRegCommand("showconffreq",		(void*)showconffreq,	"show satelite conf frequence info");
	OspRegCommand("showsatinfo",		(void*)showsatinfo,		"show mcu satelite config info");
	

    OspRegCommand("rlscf",              (void*)rlscf,           "release conference");
    OspRegCommand("rlsbas",             (void*)rlsbas,          "release bas");
    OspRegCommand("changemtres",        (void*)changemtres,     "change mt video format");

    OspRegCommand("pdcsmsg",            (void*)pdcsmsg,     "print dcsmsg");
    OspRegCommand("npdcsmsg",           (void*)npdcsmsg,    "close print dcsmsg");
    OspRegCommand("pmcsmsg",            (void*)pmcsmsg,     "print mcsmsg");
    OspRegCommand("npmcsmsg",           (void*)npmcsmsg,    "close print mcsmsg");
    OspRegCommand("pvcsmsg",            (void*)pvcsmsg, 	"print vcsmsg");
    OspRegCommand("npvcsmsg",           (void*)npvcsmsg, 	"close print vcsmsg");
    OspRegCommand("peqpmsg",            (void*)peqpmsg,     "print eqpmsg");
    OspRegCommand("npeqpmsg",           (void*)npeqpmsg,    "close print eqpmsg");
    OspRegCommand("pmmcumsg",           (void*)pmmcumsg,    "print mmcumsg");
    OspRegCommand("npmmcumsg",          (void*)npmmcumsg,   "close print mmcumsg");
    OspRegCommand("pmt2msg",            (void*)pmt2msg,     "print mt2msg");
    OspRegCommand("npmt2msg",           (void*)npmt2msg,    "close print mt2msg");
    OspRegCommand("pcallmsg",           (void*)pcallmsg,    "print callmsg");
    OspRegCommand("npcallmsg",          (void*)npcallmsg,   "close print callmsg");
    OspRegCommand("pmpmgrmsg",          (void*)pmpmgrmsg,   "print mpmgrmsg");
    OspRegCommand("npmpmgrmsg",         (void*)npmpmgrmsg,  "close print mpmgrmsg");
    OspRegCommand("pcfgmsg",            (void*)pcfgmsg,     "print cfgmsg");
    OspRegCommand("npcfgmsg",           (void*)npcfgmsg,    "close print cfgmsg");
    OspRegCommand("pmtmsg",             (void*)pmtmsg,      "print mtmsg");
    OspRegCommand("npmtmsg",            (void*)npmtmsg,     "close print mtmsg");
    OspRegCommand("pgdmsg",             (void*)pgdmsg,      "print gdmsg");
    OspRegCommand("npgdmsg",            (void*)npgdmsg,     "close print gdmsg");    
    OspRegCommand("pnplusmsg",          (void*)pnplusmsg,   "print nplus msg");
    OspRegCommand("npnplusmsg",         (void*)npnplusmsg,  "close print nplus msg");
    OspRegCommand("ppfmmsg",            (void*)ppfmmsg,     "print 8000b performance msg");
    OspRegCommand("nppfmmsg",           (void*)nppfmmsg,    "close print 8000b performance msg");
    OspRegCommand("psatmsg",            (void*)psatmsg,      "print satelite msg");
    OspRegCommand("npsatmsg",           (void*)npsatmsg,     "close print satelite msg");
    
    OspRegCommand("dumpmtssn",          (void*)dumpmtssn,    "dumpmtssn");
    OspRegCommand("dumpmodemssn",       (void*)dumpmodemssn, "dumpmodemssn");

	OspRegCommand("sconftotemp",		(void*)sconftotemp, "save conf to be a template");

    OspRegCommand("addrhelp",           (void*)addrhelp,    "Addrbook help");
    OspRegCommand("addrbookver",        (void*)addrbookver, "Addrbook version");
    OspRegCommand("prtaddrbook",        (void*)prtaddrbook, "Addrbook Print");
    OspRegCommand("addrlogon",          (void*)addrlogon,   "Addrbook Debug Enable");
    OspRegCommand("addrlogoff",         (void*)addrlogoff,  "Addrbook Debug Disable");
    
	OspRegCommand("pmdmmsg",			(void*)pmdmmsg,		"Print modem ctrl message");
	OspRegCommand("npmdmmsg",			(void*)npmdmmsg,		"Print modem ctrl message");
#endif
}

#ifdef _LINUX_

BOOL32 g_bQuit = FALSE;
void OspDemoQuitFunc( int nSignalNum )
{
	g_bQuit = TRUE;
}

int main( int argc, char** argv )
{
    if (argc > 0)
    {
        g_szAppName = argv[0];
    }
    else
    {
        g_szAppName = APPNAME;
    }
    printf("[AppEntry]Starting: %s\n", g_szAppName);
    
    userinit();

    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
    g_bQuit = FALSE;

    while(!g_bQuit)
    {               
        OspDelay(1000);
    }

    printf("mcu quit normally!\n");

    return 0;
}	
#endif //_LINUX_

#if defined( WIN32 )
	

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#define CLASS_MCU          "Windows Mcu 4.0 - Kedacom (TEST USE ONLY)"
#define MCU_RUNNING        "System Running "
#define TIMER_UPWND         (WM_USER + 0xA0)

const s8                    achRunningState[] = "-\\|/\0";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    
    LPCSTR lpszCmdLine = GetCommandLine();

    // 是否带了参数0
    if ( lpszCmdLine[strlen(lpszCmdLine) -1] == '0')
    {
        g_bOpenTelnetWin32 = FALSE;
    }
    
	WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = CLASS_MCU;
	wc.lpfnWndProc = WndProc;

    if (!hPrevInstance)
    {
	    if ( !RegisterClassEx(&wc) )
        {
		    MessageBox (NULL, TEXT ("Register window error!"), CLASS_MCU, MB_ICONERROR) ;
		    return -1;
        }
    }

    s8 szWindowTitle[KDV_MAX_PATH] = {0};
    sprintf(szWindowTitle, "Windows Mcu 4.0 - %s", lpszCmdLine);
	g_hwndMain = CreateWindowEx(WS_EX_WINDOWEDGE,
					CLASS_MCU,
					szWindowTitle,
					WS_OVERLAPPEDWINDOW,
					0, 0,
					640, 480,
					0, 0, hInstance, 0);

	if ( !g_hwndMain )
    {
		MessageBox (NULL, "Create window error!", CLASS_MCU, MB_ICONERROR) ;
		return -1;
    }

    #ifdef _DEBUG
        // debug 不显示    
        ShowWindow(g_hwndMain, SW_HIDE);
    #else
        ShowWindow(g_hwndMain, nCmdShow);
    #endif // _DEBUG
    
	UpdateWindow(g_hwndMain);

    // 启动 Console
    if (g_bOpenTelnetWin32)
    {
        AllocConsole();
        freopen("CONOUT$","w+t",stdout);          
        freopen("CONIN$","r+t",stdin);  
    }
    
	userinit();
   
    SetTimer(g_hwndMain,  TIMER_UPWND, 2000, NULL);

	MSG msg;
	while ( GetMessage(&msg, 0, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// MCU 窗口处理函数
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam) 
{ 
    static BYTE byState = 0;
    switch(message) 
    {
        case WM_DESTROY:
        {
            // 退出系统
            userquit();
            if (g_bOpenTelnetWin32)
            {
                FreeConsole();
            }            
            PostQuitMessage(0);
            break;
        }
        case WM_TIMER:
        {            
            // 刷新界面定时器
            byState++;
            if (byState >= 4)
            {
                byState = 0;
            }
            // 通知刷新
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            break;
        }   
        case WM_PAINT:
        {
            HDC dc = ::GetDC(hwnd);
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(dc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

            SetTextColor(dc, RGB(0, 0, 0) );
            SetBkMode(dc, TRANSPARENT);
            TextOut(dc, 100, 100, MCU_RUNNING, strlen(MCU_RUNNING) );
            TextOut(dc, 220, 100, achRunningState + byState, 1);
            TextOut(dc, 100, 140, "使用telnet localhost 2500可以打开调试窗口.\0", 42);
            break;
        }
        default:
            break;
    } 

    return DefWindowProc(hwnd,message,wParam,lParam); 
}

#endif



/////////////////////////////////////////////////////Linux 下异常捕获处理

#ifdef _LINUX_

void writeexclog(const s8 *pachBuf, s32 nBufLen)
{
    if (NULL == pachBuf || nBufLen <= 0)
    {
        printf("invalid param in writeexclog. pachBuf.%d nBufLen.%d\n",
            (int)pachBuf, nBufLen);
        return;
    }		
    
    FILE *hLogFile = fopen(DIR_EXCLOG, "r+b");
    if (NULL == hLogFile)
    {
        printf("exc.log not exist and create it\n");
        hLogFile = fopen(DIR_EXCLOG, "w+b");
        if (NULL == hLogFile)
        {
            printf("create exc.log failed for %s\n", strerror(errno));     
            return;
        }
    }
    
    fseek(hLogFile, 0, SEEK_END);
    s32 nLen = fwrite(pachBuf, nBufLen, 1, hLogFile);
    if (0 == nLen)
    {
        printf("write to exc.log failed\n");
        return;
    }
    
    fclose(hLogFile);
    return;
}

#ifdef _X86_

/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */
int parse_x86_context(void* tmp)
{
	struct ucontext* ct = (struct ucontext*)tmp;	
	int* regs = (int*)ct->uc_mcontext.gregs;
	int  epc = regs[14];
	int *presp = (int *)regs[6];

	printf("\nexception instruction address: 0x%x\n", epc);
	
	printf("\n===================CPU registers==============================\n");

	printf("cpu register value:\n");
	printf("GS:0x%x\t FS:0x%x\t ES:0x%x\t DS:0x%x\n", 
	       regs[0], regs[1], regs[2], regs[3]);

	printf("CS:EIP:   0x%x:0x%x\nSS:UESP:  0x%x:0x%x\n", 
	       regs[15],regs[14], regs[18], regs[17]);

	printf("EDI:0x%x\t ESI:0x%x\t EBP:0x%x\n",regs[4], regs[5], regs[6]);
	printf("ESP:0x%x\t EBX:0x%x\t EDX:0x%x\n",regs[7], regs[8], regs[9]);
	printf("ECX:0x%x\t\t EAX:0x%x\n",regs[10], regs[11]);
	printf("TRAPNO:0x%x\t ERR:0x%x\t EFL:0x%x\n",regs[12], regs[13], regs[16]);

	printf("\n=================== call trace ==============================\n");


	/* ebp will record frame-pointer */
#if 0	
	*(stack-1) = epc;/* put the exception addr in the stack */
	for(i = -1;; i++) 
		if (printf_symbl_name(*(stack+i)) == -1)
			break;
#else
	printf_symbl_name(epc);/*fault addr*/
	while(1)
	{
		int ret = printf_symbl_name(*(presp + 1));
		if (ret == -1 || !ret)
		{
			if (!ret)
				printf("programs's user stack error !!\n");
			break;
		}
		presp = (int *)(*(presp));/* pre function's base sp */		
	}
	
#endif			
	free(sym);
	free(strtab);
	return 0;
}


#else   // Power PC 


/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */
int parse_ppc_context(void* tmp)
{
	struct ucontext* ct = (struct ucontext*)tmp;
	int* regs = (int*)ct->uc_mcontext.regs;
	int  epc = regs[32];
	int* stack = (int*)regs[1];
	int  link = regs[36];/* link register */
	int* presp = (int *)(*stack);

	printf("\nexception instruction address: 0x%x\n", epc);
	printf("\n=================== call trace ==============================\n");

    // xsl [8/16/2006] write to log
    char achInfo[255];
    time_t tiCurTime = ::time(NULL);             
    int nLen = sprintf(achInfo, "\nSytem time %s\n", ctime(&tiCurTime));
    writeexclog(achInfo, nLen);
    nLen = sprintf(achInfo, "Exception instruction address: 0x%x\n", epc);
    writeexclog(achInfo, nLen);
    nLen = sprintf(achInfo, "\n=========== call trace ============\n");
    writeexclog(achInfo, nLen);
	
	/* ebp will record frame-pointer */

	int func_size ;
	func_size = printf_symbl_name(epc);/*fault addr*/
	if (func_size > 0)/* link register and faule addr may be in the same function so do follows */
	{
		if ((epc - link < 0) || (epc - link >= func_size))
			printf_symbl_name(link);
	}
	
	while(1)
	{
		int ret = printf_symbl_name(*(presp + 1));
		if (ret == -1 || !ret)
		{
			if (!ret)
				printf("programs's user stack error !!\n");
			break;
		}
		presp = (int *)(*(presp));/* pre function's base sp */		
	}
	
	free(sym);
	free(strtab);
	return 0;
}
#endif


void parse_context(void* uc)
{
#ifdef _X86_
	parse_x86_context(uc);
#else   // Power PC
	parse_ppc_context(uc);
#endif
}


int printf_symbl_name(int pc)
{
	Elf32_Sym *syml = get_func_symb_by_addr(sym,symsize, pc);
	if (syml) {
		if (syml->st_name >= strsize)
		{
			printf("st_name if big then strtable size\r\n");
			return -1;
		}
		//return (strtab+syml->st_name);
		printf("pc:%x %s\r\n", pc, (strtab+syml->st_name));

        // xsl [8/16/2006] write to log
        char achInfo[255];
        int nLen = sprintf(achInfo, "pc:%x %s\r\n", pc, (strtab+syml->st_name));
        writeexclog(achInfo, nLen);
		
		if (!strcmp((strtab+syml->st_name), "main"))		
			return -1;
			
		return syml->st_value;	
	}	
	return 0;
}
void my_sigaction(int signo, siginfo_t* info, void* ct)
{
    s8 szReason[255] = {0};

	if(signo == SIGSEGV) 
    {
		sprintf(szReason, "\nCatch segment fault! PID = %d\n", getpid());        
	}

	if(info->si_code == SI_KERNEL)
    {
        sprintf(szReason, "Signal is sent by Kernel\n");
    }
		
    printf(szReason);
    writeexclog(szReason, strlen(szReason));    

	if(info->si_code == SEGV_MAPERR)
    {
		sprintf(szReason, "Caused by:\tAccess a fault address: %p\n", info->si_addr);
    }
	else if(info->si_code == SEGV_ACCERR)
    {
		sprintf(szReason, "Caused by:\tAccess a no permission address: %p\n", info->si_addr);
    }
	else
    {
		sprintf(szReason, "Caused by:\tUnknow reason\n");
    }

    printf(szReason);
    writeexclog(szReason, strlen(szReason));
    
	if (ct)
		parse_context(ct);
	else
		printf("process context is NULL\n");	

    sprintf(szReason, "------End of Crash Dump----------\n\n");
    printf(szReason);
    writeexclog(szReason, strlen(szReason));

	/* sigment fault is critical fault, we should exit right now */
	exit(1);
}

void install_sigaction()
{
	struct sigaction sigact;

	sigemptyset (&sigact.sa_mask);
	//memset(&sigact, 0, sizeof (struct sigaction));
	sigact.sa_flags = SA_ONESHOT | SA_SIGINFO;
	sigact.sa_sigaction = my_sigaction;
	sigaction(SIGSEGV , &sigact, NULL);	
}



/* get the function'symbol that the "addr" is in the function or the same of the function */
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr)
{	
	for (;tabsize > 0; tabsize -= sizeof(Elf32_Sym), symtab ++)
	{		
#if 1
		if (ELF32_ST_TYPE(symtab->st_info) != STT_FUNC || symtab->st_shndx == 0)/* we only find the function symbol and not a und  */	
			continue;
		if (!symtab->st_value)
			continue;
		if (addr >= symtab->st_value && addr < (symtab->st_size+symtab->st_value))
			return symtab;	
#else
		if (ELF32_ST_TYPE(symtab->st_info) == STT_FUNC)/* we only find the function symbol */	
			printf("name %s addr %x size %x\r\n", (strtab+symtab->st_name), symtab->st_value, symtab->st_size);
#endif			
	}
	return NULL;
}	


Elf32_Sym *get_symbol(Elf32_Sym *symtab, int tabsize, int addr)
{
	for (;tabsize > 0; tabsize -= sizeof(Elf32_Sym), symtab ++)
		if (symtab->st_value == addr)
			return symtab;

	return NULL;		
}
Elf32_Shdr * read_elf_section(FILE *file, Elf32_Ehdr *ehdr)
{
	Elf32_Shdr *pshdr;	
	int s_size = ehdr->e_shentsize * ehdr->e_shnum;
	
	pshdr = (Elf32_Shdr *)malloc(s_size);
	if (pshdr == NULL) 
	{
		printf("malloc shdr error\r\n");
		return NULL;
	}	

	if (fseek (file, ehdr->e_shoff, SEEK_SET))
	{
		printf("fseek error\r\n");
		return NULL;
	}	
	
	if (fread(pshdr, s_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		return NULL;
	}

	return pshdr;
}


void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr, int *size)
{
	int sec_num;
	int tb_size;
	void *sym;

	
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num ++, shdr ++)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			break;
	}	
	
	if (sec_num == ehdr->e_shnum)
	{
		printf("No symbol table\n");
		return NULL;
	}
	*size = tb_size = shdr->sh_size;
	
	sym = (void *)malloc(tb_size);
	if (sym ==NULL)
	{	
		printf("malloc error in func %s\r\n", __func__);
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(sym, tb_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		return NULL;
	}	
	return sym;
}

void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size)
{
	int sec_num;
	int tb_size;
	void *sym;
	Elf32_Shdr *lshdr = shdr;
	
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num ++, shdr ++)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			break;
	}	

	shdr  = lshdr + SECTION_HEADER_INDEX(shdr->sh_link);
	
	*size = tb_size = shdr->sh_size;
	
	sym = (void *)malloc(tb_size);
	if (sym ==NULL)
	{	
		printf("malloc error in func %s\r\n", __func__);
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(sym, tb_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		return NULL;
	}	

	return sym;
}

int  read_elf_head(FILE *file, Elf32_Ehdr *ehdr)
{
	if (fread(ehdr, sizeof(Elf32_Ehdr), 1, file) != 1)
		return -1;

	return 0;
}


/*=============================================================================
    函 数 名： StartCaptureException
    功    能： 设置进程异常捕获回调
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： s32 0-成功，非0-错误码
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/7/11   1.0			万春雷                  创建
=============================================================================*/
s32 StartCaptureException()
{
#ifndef _X86_
    return init_nipdebug(0, NULL, DIR_EXCLOG);
#else
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdr;
	FILE *file;

	install_sigaction();
	
	file = fopen(g_szAppName, "rb");
	if (file == NULL)
	{
		printf("Mcu: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
		return errno;
	}

	/* read the elf file's head */
	if (read_elf_head(file, &ehdr) == -1)
		return -1;
	
	/* read all sections of the elf file */
	shdr = read_elf_section(file, &ehdr);
	if (shdr == NULL)
		return -3;

	/* read static symbol table through symbol section*/
	sym = (Elf32_Sym *)read_symtable(file, shdr, &ehdr,  &symsize);
	if (sym == NULL)
		return -4;
	
	/* read string table through string section */
	strtab = (char *)read_strtable(file, shdr, &ehdr, &strsize);
	if (strtab == NULL)
		return -5;

	free(shdr);

    fclose(file);

	return 0;
#endif
}


API void showmcutopo()
{
    OspPrintf(TRUE, FALSE, "\tSatMcuTopoInfo as follows:\n");
    for(u8 byIdx = 0; byIdx < MAXNUM_TOPO_MCU; byIdx ++)
    {

    }
}

API void showmttopo()
{

}

API void shownms()
{
}

#endif // _LINUX_

/////////////////////////////////////////////////////异常捕获处理

//END OF FILE

