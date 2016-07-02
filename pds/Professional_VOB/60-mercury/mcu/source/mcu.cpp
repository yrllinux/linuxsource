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

/*lint -save -esym(578, sym)*/
/*lint -save -e592*/
/*lint -save -e560*/
/*lint -save -e765*/ 
/*lint -save -e714*/
/*lint -save -e843*/
#include "evmcu.h"
#include "mcuvc.h"
#include "mcuver.h"
#include "mcsssn.h"
#include "mcuguard.h"
#include "mtadpssn.h"
#include "mpssn.h"
#include "mcucfg.h"
#include "dcsssn.h"
#include "eqpssn.h"
#include "dataswitch.h"
#include "nplusmanager.h"
#include "vcsssn.h"
#include "evmcumt.h"
#include "commonlib.h"

#if defined(WIN32) || defined(_WIN32)
#define MCU_KDVLOG_CONF_FILE	(s8*)"./conf/kdvlog_mcu.ini"
#elif defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#define MCU_KDVLOG_CONF_FILE	(s8*)"/opt/mcu/conf/kdvlog_mcu.ini"
#else
#define MCU_KDVLOG_CONF_FILE	(s8*)"/usr/etc/config/conf/kdvlog_mcu.ini"
#endif

// MPC2 支持
#if defined(_VXWORKS_)
    #include <inetLib.h>
    #include "brdDrvLib.h"
#elif defined(_LINUX_)
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
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

//Elf32_Sym *get_symtab(Elf32_Sym *symtab, int tabsize, int addr);
Elf32_Shdr * read_elf_section(FILE *file, Elf32_Ehdr *ehdr);
void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,   int *size);
void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size);
int  read_elf_head(FILE *file, Elf32_Ehdr *ehdr);
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr);
int printf_symbl_name(int pc);

Elf32_Sym *sym = NULL;
char *strtab = NULL;
int symsize = 0;
int strsize = 0;

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

// pclint (Warning -- bit-wise operation uses (compatible) enum's)
/*lint -save -e655*/
	OspSetScrnTrcFlag( AID_MCU_VC, TRCSTATE|TRCEVENT|TRCTIMER );
/*lint -restore*/
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

// pclint (Warning -- bit-wise operation uses (compatible) enum's)
/*lint -save -e655*/
	OspSetFileTrcFlag( AID_MCU_VC, TRCSTATE|TRCEVENT|TRCTIMER );
/*lint -restore*/

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



API void GetCompileTime(s8 *psztime)
{
	if (psztime == NULL)
	{
		return;
	}
	
	sprintf(psztime, "%s %s", __DATE__, __TIME__);
}

void* ReadAddr(void*)
{
	// error 429: (Warning -- Custodial pointer 'pTempAddr' (line 336) has not been freed or returned)
	// 为通过pclint检测,这里禁止对429错误
	/*lint -save -e429*/

	//启动地址薄服务  
	CAddrBook* pTempAddr = NULL;

#ifdef _UTF8
    pTempAddr = new CAddrBook(MAX_ADDRENTRY, MAX_ADDRGROUP, emCodingModeUTF8);
#else
	pTempAddr = new CAddrBook(MAX_ADDRENTRY, MAX_ADDRGROUP);
#endif

	if(NULL == pTempAddr)
	{
		printf("[ReadAddr]memory allocation for address book failed!\n");
	}
	else
	{
		if(!pTempAddr->LoadAddrBook(GetAddrBookPath()))
		{
// 			delete pTempAddr;
			printf("[ReadAddr]LoadAddrBook() from %s failed!\n", GetAddrBookPath());
		}

		OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), MCU_ADDRINITIALIZED_NOTIFY, &pTempAddr, sizeof(pTempAddr));
		printf("[ReadAddr]Notify mcsssn addressbook has initialized\n");
	}

	return NULL;
	// 恢复pclint对e429的检查
	/*lint -restore*/
}

/*====================================================================
    函数名      ：Handle8KhmEthSwitch
    功能        ：处理8000H-M的网口切换问题。
	注意		：只适用于当前版本，如果kdv和kdm业务放到同一个网卡上，将不需要此接口
    算法实现    ：
    引用全局变量：
    输入参数说明：int sig, 传入的信号，由主线程处理
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013-6-6    4.7         彭国锋         创建
====================================================================*/
void Handle8KhmEthSwitch(int sig)
{
#ifdef _LINUX_
	if ( sig == SIGUSR2)
	{
		StaticLog("[%s]mcu recv sigal SIGUSR2:%d\n", __func__, sig);
		OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), EV_CHANGE_ETH_NOTIF);
	}
	else
	{
		StaticLog("[%s]recv sig:%d\n", __func__, sig);
	}
#endif
	return;
}
#if defined(_LINUX_) && ( defined(_8KE_) || defined(_8KH_) || defined(_8KI_) )
void my_sigproc(int signo)
{
	mculogflush();
	OspDelay(1000);

	u8 byCurEthUse = 0;
	s8 abyBuf[2] = {0};
	s8 achFileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achFileName, "%s", "/opt/ethchoice" );
	if (0 == access (achFileName, F_OK))
	{
		FILE *hFile  = fopen(achFileName, "rb");
		if ( NULL != hFile )
		{
			s32 nFileLen = 0;
			fseek(hFile, 0, SEEK_END);
			nFileLen = ftell(hFile);
			if (nFileLen > 0)
			{
				fseek(hFile, 0, SEEK_SET);
				fread(abyBuf, 1, 1, hFile);
				StaticLog("[my_sigproc]read /opt/ethchoice:  %s\n",abyBuf);
				printf("[my_sigproc]read /opt/ethchoice : %s\n",abyBuf);
				byCurEthUse = atoi(abyBuf);
			}
			else
			{
				StaticLog("[my_sigproc]read /opt/ethchoice len err len:%d\n",nFileLen);
				printf("[my_sigproc]read /opt/ethchoice len err len:%d\n",nFileLen);
			}
			fclose(hFile);
			hFile = NULL;
			
		}
		else
		{

			StaticLog("[my_sigproc]access /opt/ethchoice failed\n");
		}
		printf("eth shift to %d! \n", byCurEthUse);
		StaticLog("eth shift to %d! \n", byCurEthUse);
		g_cMcuAgent.SetInterface(byCurEthUse);
		OspPost( MAKEIID(AID_MCU_CONFIG,1), AGT_MCU_CFG_ETHCHANGE_NOTIF);
	}
	else
	{
		printf("eth shifting, but ethchoice cannot be confirmed!\n");
	}

// 	printf("eth shift to %d! \n", byCurEthUse);
// 	u8 byOrgEthChoice = (g_cMcuAgent.GetInterface() == 0)?1:0;
// 	g_cMcuAgent.SetInterface(byOrgEthChoice);
// 	OspPost( MAKEIID(AID_MCU_CONFIG,1), AGT_MCU_CFG_ETHCHANGE_NOTIF);
	
}
#endif

API void userinit()
{
	logsetport( 5677,1 );
	Err_t err = LogInit(MCU_KDVLOG_CONF_FILE);
	logenablemod(MID_MCU_CONF);
	if(LOG_ERR_NO_ERR != err)
	{
		printf("KdvLog Init failed, err is %d\n", err);
	}
	s32  nRet = 0;
	// 设置进程异常捕获回调
#ifdef _LINUX_
    nRet = StartCaptureException();
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
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
    if (BrdInit() == ERROR)
	{
		printf("\nBrdInit() return ERROR,Quit now!\n");
        return;
	}

#endif
#endif
    
    BOOL32 bRet = CreateConfStoreFile();
    if ( !bRet )
    {
        return;
    }

	//尝试从调试配置文件中读出本mcu侦听连接的端口，尝试失败采用默认值
	g_cMcuVcApp.GetBaseInfoFromDebugFile();
	g_cMcuVcApp.GetDri2E1CfgTableFromFile();

	u16 wMcuTelnetPort = g_cMcuVcApp.GetMcuTelnetPort();
	u16 wMcuListenPort = g_cMcuVcApp.GetMcuListenPort();
	 
	//OSP initialization
	BOOL32 bInitRet = FALSE;
    #ifdef WIN32
		wMcuTelnetPort = 0;
        bInitRet = OspInit( g_bOpenTelnetWin32, wMcuTelnetPort ,"MCU");
	
        OspRegistModule("kdv323.dll");
    #else
		BOOL32 bTelnetEn = g_cMcuVcApp.IsTelnetEnable();
		printf("telnet enable is：%d\n", bTelnetEn);
        bInitRet = OspInit( bTelnetEn, wMcuTelnetPort,"MCU" );
    #endif	
	if ( FALSE == bInitRet )
	{
		printf("OspInit failed!\n");
		StaticLog("OspInit Failed!\n");
		return;
	}
	g_cMcuVcApp.SetMcuStartTick( OspTickGet() );

#if defined(_8KE_) 
	OspSetPrompt("mcu_8000e");
#endif
#if	defined(_8KH_)
	OspSetPrompt("mcu_8000h");
#endif
#if defined(_8KI_)
	OspSetPrompt("mcu_8000i");
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

    StaticLog( "MCU Version: %s\n", GetMcuVersion() );

	AddEventStr();

//    s8 szLogFile[KDV_MAX_PATH];
//    sprintf( szLogFile, "%s/mcu.log", DIR_DATA );
//	  OspOpenLogFile( szLogFile, 1000, 5 );

    u8 byMcuType = GetMcuPdtType();

//#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	//创建连接用的监听端口
	OspCreateTcpNode( 0, wMcuListenPort);
//#endif	
	//初始化主备板倒换业务设置，并设置主备用状态
	TMSStartParam tMSStartParam;
	tMSStartParam.m_byMcuType = byMcuType;
	tMSStartParam.m_wMcuListenPort = wMcuListenPort;
	tMSStartParam.m_bMSDetermineType = g_cMcuVcApp.GetMSDetermineType();
	BOOL32 bMpcEnable = g_cMSSsnApp.InitMSConfig(tMSStartParam);

	//MCU代理，设置为主备用状态
	McuAgentInit(byMcuType, bMpcEnable);       //agent reads config file
	
	//创建连接用的监听端口
/*#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u32 dwMcuIp  = g_cMcuAgent.GetMpcIp();
	printf("use dwMcuIp :%x to OspCreateTcpNode!!\n", dwMcuIp);
	OspCreateTcpNode( dwMcuIp, wMcuListenPort, TRUE);
#endif*/

	//设置mcu编译时间
	s8 achCompileTime[MAX_ALARM_TIME_LEN + 1];	//mcu编译时间
	GetCompileTime(achCompileTime);
	g_cMcuAgent.SetMcuCompileTime(achCompileTime);

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
    
    // [3/18/2013 liaokang] 支持主备N+1备双备份
//     //N+1备份 与 热备份互斥
//     if (!g_cMSSsnApp.IsDoubleLink())
//     {
    //初始化N+1备份状态
    BOOL32 bNPlusMode = g_cNPlusApp.InitNPlusState();
    if (bNPlusMode)
    {
        nRet = g_cNPlusApp.CreateApp("McuNPlus", AID_MCU_NPLUSMANAGER, APPPRI_MCUNPLUS, 600, 512<<10);
        if (OSP_OK != nRet )
        {
            StaticLog("[userinit]g_cNPlusApp.CreateApp McuNPlus failed!\n ");
        }
        //启动消息放在模板读取成功时发送，避免N+1回滚时找不到对应模板，读取不到相应的预案信息 [6/5/2012 chendaiwei]
        //OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), OSP_POWERON);
    }
//    }    

	//MCU业务
#ifdef _X86_    
	printf("set McuVc stack size 100M\n");
	StaticLog( "set McuVc stack size 100M\n");
	nRet = g_cMcuVcApp.CreateApp("McuVc", AID_MCU_VC, APPPRI_MCUVC, 4000, 100<<20/*512<<10*/);//!!!!!!!
#else
    nRet = g_cMcuVcApp.CreateApp("McuVc", AID_MCU_VC, APPPRI_MCUVC, 4000, 2<<20);
#endif
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cMcuVcApp.CreateApp McuVc failed!\n ");
	}

#ifndef _X86_

	//MCU守卫
	nRet = g_cMcuGuardApp.CreateApp("McuGuard", AID_MCU_GUARD, APPPRI_MCUGUARD, 100);
    g_cMcuGuardApp.SetPriority(APPPRI_MCUGUARD);
#elif defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	printf("create McuGuard App!\n");
	nRet = g_cMcuGuardApp.CreateApp("McuGuard", AID_MCU_GUARD, APPPRI_MCUGUARD, 100);
#endif
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cMcuGuardApp.CreateApp McuGuard failed!\n ");
	}

	//MCU会议控制台会话
	nRet = g_cMcsSsnApp.CreateApp( "McSsn", AID_MCU_MCSSN, APPPRI_MCUSSN, 2000, 500<<10 );
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cMcsSsnApp.CreateApp McSsn failed!\n ");
	}
	//MCU外设会话
	nRet = g_cEqpSsnApp.CreateApp( "EqpSsn", AID_MCU_PERIEQPSSN, APPPRI_MCUSSN, 2000, 500<<10 );
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cMcsSsnApp.CreateApp EqpSsn failed!\n ");
	}
	//终端适配会话
	nRet = g_cMtAdpSsnApp.CreateApp("MtAdpSsn", AID_MCU_MTADPSSN, APPPRI_MCUSSN, 4000, 500<<10);
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cMtAdpSsnApp.CreateApp MtAdpSsn failed!\n ");
	}
	//MCU的MP会话
	nRet = g_cMpSsnApp.CreateApp("MpSsn", AID_MCU_MPSSN, APPPRI_MCUSSN, 2000 );
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cMpSsnApp.CreateApp MpSsn failed!\n ");
	}
    //mcu 配置界面化
	nRet = g_cMcuCfgApp.CreateApp("McuConfig", AID_MCU_CONFIG, APPPRI_MCUCONFIG, 100, 500<<10);
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cMcuCfgApp.CreateApp McuConfig failed!\n ");
	}
	//mcu T120集成
	nRet = g_cDcsSsnApp.CreateApp( "DcsSsn", AID_MCU_DCSSSN, APPPRI_MCUSSN, 200, 500<<10 );
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cDcsSsnApp.CreateApp DcsSsn failed!\n ");
	}
	//VCS会话
	nRet = g_cVcsSsnApp.CreateApp( "VcSsn", AID_MCU_VCSSN, APPPRI_MCUSSN, 2000, 500<<10 );
	if (OSP_OK != nRet )
	{
		StaticLog("[userinit]g_cVcsSsnApp.CreateApp VcSsn failed!\n ");
	}


	//大容量地址簿的读取耗时较长，单独起一个线程负责地址簿的读取
	OspTaskCreate(ReadAddr, "ReadAddrThread", APPPRI_MCUSSN, 200<<10, 0, 0);

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
			StaticLog("Local_Mp_Service dont startup under doublelinking mode\n");
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
			StaticLog( "Local_Mtadp_Service dont startup under doublelinking mode\n");
		}
	}
    
	//不定时打印状态
	OspStatusMsgOutSet(FALSE);

	char achMcuAlias[32];
	g_cMcuAgent.GetMcuAlias(achMcuAlias, sizeof(achMcuAlias));
	StaticLog( "MCU: %s DoubleLink.%d Init Success!\n", achMcuAlias, g_cMSSsnApp.IsDoubleLink());

	// 初始化
	OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), OSP_POWERON);
    OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), OSP_POWERON);
	OspPost(MAKEIID(AID_MCU_VCSSN, CInstance::DAEMON), OSP_POWERON);
	OspPost(MAKEIID(AID_MCU_GUARD, 1), OSP_POWERON);

//#endif
    // win32/redhat only for test
#if defined(WIN32) /*|| ( defined(_X86_) && !defined(_8KE_) )*/
    g_cMcuVcApp.SetLicenseNum(MAXNUM_MCU_MT);
	g_cMcuVcApp.SetVCSAccessNum(MAXNUM_MCU_VC);
	g_cMcuVcApp.SetLicenseHDMtNum(MAXNUM_MCU_MT);
	g_cMcuVcApp.SetLicensePCMtNum(MAXNUM_MCU_MT);
	g_cMcuVcApp.SetAudMtLicenseNum(MAXNUM_MCU_MT);	
#else
	// license
	printf("begin to send MCUGD_GETLICENSE_DATA as linux!\n");
	OspPost( MAKEIID( AID_MCU_GUARD, 1), MCUGD_GETLICENSE_DATA/*, NULL, 0 */);	

#endif

	
	//启动外设
	//存在启用主备倒换服务下将不启动内嵌local_eqp_service
	if (FALSE == g_cMSSsnApp.IsDoubleLink())
	{
		StartLocalEqp();
	}
	else
	{
		StaticLog( "Local_Eqp_Service dont startup under doublelinking mode\n");
	}

	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	// [3/6/2010 xliang] 写业务启动配置文件，表业务已经起来
	g_cMcuAgent.SetMpcUpateStatus(1);

#ifdef _LINUX_
	// [4/28/2010 xliang] ftp 服务器搭建
	chdir("/opt/mcu");
	s8    achFileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achFileName, "%s", KDV8KE_FTPSETUP_SCRIPT);
	if(0 == access (achFileName, F_OK))
	{
		s8    achFile[MAXLEN_MCU_FILEPATH] = {0};
		sprintf(achFile, "./%s", KDV8KE_FTPSETUP_SCRIPT);
		chmod(achFile, S_IREAD|S_IWRITE|S_IEXEC);
		system(achFile);
	}

	// [3/15/2011 xliang] 捕获网口切换信号
	if ( SIG_ERR == signal(SIGUSR1, my_sigproc) )
	{
		perror("signal error!");
	}
#ifdef _8KH_
	// 8000H-M捕获网卡切换信号 [pengguofeng 6/5/2013]
	if ( SIG_ERR == signal(SIGUSR2, Handle8KhmEthSwitch) )
	{
		perror("signal error!:SIGUSR2");
	}
	else
	{
		printf("register signal:%d SIGUSR2 OK\n",SIGUSR2);
	}
#endif

#ifdef _8KI_
	SetTelnetDown();
	SetSshDown();
#endif

#endif


	
#endif

//[liu lijiu][2010/10/28]设置USBKEY用户授权验证的flag.
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef _LINUX_
	g_cMcuVcApp.SetLicesenUSBKEYFlag();
	printf("USBKEY data is valid!\n");
#endif  //_LINUX_ ends
#endif  //_8KE_ ends
//end
	return;
}

API void userquit()
{
	// [6/29/2011 liuxu] 退出前先flush
	mculogflush();

	if ( g_cMcuAgent.GetIsUseMpcTranData() )
	{
		//存在启用主备倒换服务下将不启动内嵌local_mp_service
		if (FALSE == g_cMSSsnApp.IsDoubleLink())
		{
			StopLocalMp();
		}
	}

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
	// [6/29/2011 liuxu] 重启前先flush
	mculogflush();
	OspDelay(50);

#ifndef WIN32
	userquit();
    BrdHwReset();
	printf("[Reboot]cmd req reboot\n");	
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
	case MCU_TYPE_KDV8000E:        // [12/03/2010 miaoqingsong] 增加8000G型号
		strcpy(szMcuType, "8000G");
		break;
	case MCU_TYPE_KDV8000H:
		strcpy(szMcuType, "8000H");
		break;
	case MCU_TYPE_KDV8000I:
		strcpy(szMcuType, "8000I");
		break;
	case MCU_TYPE_KDV800L:
		strcpy(szMcuType, "800L");
		break;
	case MCU_TYPE_KDV8000H_M:
		strcpy(szMcuType,"8000H-M");
		break;
    default:
        strcpy(szMcuType, "WIN32");
        break;
    }
    StaticLog( "MCU(%s) Version: %s\n", 
               szMcuType, GetMcuVersion()  );
    
	StaticLog( "Module Ver: %s  Compile Time: %s, %s\n", 
               VER_MCU, __TIME__, __DATE__ );
	ospver();
	mcuagtver();
	dsver();
    StaticLog( "\n");
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
	StaticLog( "MCU消息分类打印命令列表:\n" ); 
	StaticLog( "1.  pmtmsg:       打印与终端的自定义消息.\n" );
	StaticLog( "1.1 npmtmsg:      关闭打印与终端的自定义消息.\n" );
	StaticLog( "2.  pmtadpmsg:    打印发向MtAdp的自定义消息.\n" );
	StaticLog( "2.1 npmtadpmsg:   关闭打印发向MtAdp的自定义消息.\n" );
	StaticLog( "3.  pmpmgrmsg:    打印与MP的消息.\n" );
	StaticLog( "3.1 npmpmgrmsg:   关闭打印与MP的消息.\n" );
	StaticLog( "4.  peqpmsg:      打印与外设的消息.\n" );
	StaticLog( "4.1 npeqpmsg:     关闭打印与外设的消息.\n" );
	StaticLog( "5.  pmcsmsg:      打印与会控的消息.\n" );
	StaticLog( "5.1 npmcsmsg:     关闭打印与会控的消息.\n" );
	StaticLog( "6.  pcallmsg:     打印终端呼叫信息.\n" );
	StaticLog( "6.1 npcallmsg:    关闭打印终端呼叫信息.\n" );
	StaticLog( "7.  pmmcumsg:     打印级联业务信息.\n" );
	StaticLog( "7.1 npmmcumsg:    关闭打印级联业务信息.\n" );
	StaticLog( "8.  pgdmsg:       打印GUARD信息.\n" );
	StaticLog( "8.1 npgdmsg:      关闭打印GUARD信息.\n" );	
	StaticLog( "9.  pcfgmsg:      打印配置界面化信息.\n" );
	StaticLog( "9.1 npcfgmsg:     关闭打印配置界面化信息.\n" );
	StaticLog( "10.  pdcsmsg:     打印DCS与MCU的DCS会话的消息.\n" );
	StaticLog( "10.1 npdcsmsg:    关闭打印DCS与MCU的DCS会话的消息.\n" );	
	StaticLog( "11.  pmcudcsmsg:  打印MCU的DCS会话与MCU业务的信息.\n" );
	StaticLog( "11.1 npmcudcsmsg: 关闭打印MCU的DCS会话与MCU业务的信息.\n" );
    StaticLog( "12.  ppfmmsg:     打印8000B性能限制消息.\n");
    StaticLog( "12.1 nppfmmsg:    关闭打印8000B性能限制消息.\n");
    StaticLog( "13.  pvcsmsg:     打印与VCS的消息.\n");
    StaticLog( "13.1 npvcsmsg:    关闭打印VCS的消息.\n");

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
	StaticLog( "会议信息打印命令列表:\n" ); 
	StaticLog( "1. showconfall:    打印会议所有信息.\n" );
	StaticLog( "2. showconfinfo:   打印会议信息.\n" );
	StaticLog( "3. showconfswitch: 打印会议交换信息.\n" );
	StaticLog( "4. showmtmonitor:  打印终端监控信息.\n" );
	StaticLog( "5. showconfmt:     打印会议终端信息.\n" );
	StaticLog( "6. showconfeqp:    打印会议外设信息.\n" );
	StaticLog( "7. showmtstat:     打印终端状态.\n" );
	StaticLog( "8. showtemplate:   打印会议模板信息.\n" );
	StaticLog( "9. showconfmap:    打印会议ID映射表信息.\n" );
	StaticLog( "10. showbas:		打印会议的适配信息.\n" );
	StaticLog( "11. showbasgrp:	打印会议需要适配的群组.\n");
    StaticLog( "12. showbasall:    打印MCU所有的HD-BAS信息\n");
	StaticLog( "13. showconfmix:   打印会议混音信息.\n" );
	StaticLog( "14. showconfspy:   打印多回传信息.\n" );
	StaticLog( "15. showbaslist:   打印bas链表信息.\n");
	StaticLog( "16. showconfmcu:   打印会议MCU信息.\n" );
	StaticLog( "17. showprs:       打印会议prs信息.\n");
	StaticLog( "18. showtoken:     打印临牌环信息.\n");
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
    函数名      ：ssl
    功能        ：打印发言人列表
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/21/2010  4.6         张宝卿         创建
====================================================================*/
API void ssl( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPSPEAKLIST ) );
}

API void psl( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPPRTSPEAKLIST ) );
}

API void npsl( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPNPRTSPEAKLIST ) );
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




API void flowctrl(u8 byConfIdx, u8 byMtId, u16 wBand)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
	if ( NULL == pVcInst )
	{
		StaticLog( "ConfIdx.%d doesn't exist, try again ...\n", byConfIdx );
		return;
    }
	else
	{
		u16 wInsId = pVcInst->GetInsID();
		CServMsg cServMsg;
		cServMsg.SetMsgBody(&byMtId, sizeof(byMtId));
		cServMsg.CatMsgBody((u8*)&wBand, sizeof(wBand));

		OspPost(MAKEIID(AID_MCU_VC, wInsId), MCU_MT_DBG_FLOWCTRLCMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			
    }
}
/*====================================================================
    函数名      ：svam
    功能        ：打印会议VMP前适配成员信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/01/20    4.6         xl		      create
====================================================================*/
API void svam( void)
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPVMPADAPT ) ); 
}

/*====================================================================
    函数名      ：smstimer
    功能        ：set sms timer or show sms timer
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wTimer		为0表显示当前短消息每发送100条延时多少ms再发；大于等于0则表设置延时值
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/01/20    4.6         xl		      create
====================================================================*/
API void smstimer(u16 wTimer = 0)
{
	//usage print
	/*
		+------------------------------------------------------------------------------------------+
		|  smstimer [wTimer]                                                                       |
		|  If wTimer is 0, we will show u the timer; otherwise we will set the value to sms timer  |
		+------------------------------------------------------------------------------------------+
	*/
	StaticLog( "Usage for this API--smstimer:\n");
	s8 achFormat[]	= "smstimer [wTimer]";
	s8 achNote[]	= "If wTimer is 0, we will show u the timer; otherwise we will set the value to sms timer";
	
	const s32 nFormatLen = strlen(achFormat);
	const s32 nNodeLen = strlen(achNote);
	u8 byLen = (u8)max( nFormatLen, nNodeLen );
	u8 byLoop =0;
	StaticLog( "+");
	for(; byLoop< ( byLen + 4 ); byLoop ++)
	{
		StaticLog( "-");
	}
	StaticLog( "+\n");

	StaticLog( "|  %-*s  |\n", byLen,achFormat);
	StaticLog( "|  %-*s  |\n", byLen, achNote); 

	StaticLog( "+");
	for(byLoop =0; byLoop< byLen + 4; byLoop ++)
	{
		StaticLog( "-");
	}
	StaticLog( "+\n");

	
	u16 wParam = wTimer;
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( wParam, DUMPSMSTIMER ) ); 
}

/*====================================================================
    函数名      showmcothertable
    功能        ：打印会议其他终端信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2011/07/27				周嘉麟		   创建
====================================================================*/
API void showmcothertable()
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPMCOTHERTABLE ) );        
}
/*====================================================================
    函数名      showvcsextracfg
    功能        ：打印vcs预案信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2011/07/27				周嘉麟		   创建
====================================================================*/
API void showvcsextracfg()
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCFGEXTRADATA ) );     
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
            StaticLog("Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            StaticLog("ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
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

/*====================================================================
    函数名      ：showconfmCU
    功能        ：打印会议所有mcu信息
    算法实现    ：默认参数打印所有会议的所有终端的基本信息，不含能力级；
                  byConfIdx!=0，打印该会议混音信息
                  byConfIdx ==0 打印全部会议混音信息
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	10/08/02    4.6         周晶晶         创建
====================================================================*/
API void showconfmcu( void )
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCONFMCU ) ); 
}
/*====================================================================
    函数名      ：showconfmix
    功能        ：打印会议混音信息
    算法实现    ：默认参数打印所有会议的所有终端的基本信息，不含能力级；
                  byConfIdx!=0，打印该会议混音信息
                  byConfIdx ==0 打印全部会议混音信息
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
====================================================================*/
API void showconfmix( u8 byConfIdx = 0 )
{
    if ( 0 != byConfIdx )
    {
        if ( byConfIdx > MAX_CONFIDX )
        {
            StaticLog( "Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            StaticLog( "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
            return;
        }
        else
        {
            u16 wInsId = pVcInst->GetInsID();
            OspInstDump( AID_MCU_VC, wInsId, MAKEDWORD( 0, DUMPCONFMIX ) );
        }
    }
    else
    {
        OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPCONFMIX ) );        
    }
}

/*====================================================================
    函数名      ：showconfmix
    功能        ：打印会议多回传信息
    算法实现    ：默认参数打印所有会议的所有终端的基本信息，不含能力级；
                  byConfIdx!=0，打印该会议混音信息
                  byConfIdx ==0 打印全部会议混音信息
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
====================================================================*/
API void showconfspy( u8 byConfIdx = 0 )
{
    if ( 0 != byConfIdx )
    {
        if ( byConfIdx > MAX_CONFIDX )
        {
            StaticLog( "Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            StaticLog( "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
            return;
        }
        else
        {
            u16 wInsId = pVcInst->GetInsID();
            OspInstDump( AID_MCU_VC, wInsId, MAKEDWORD( 0, DUMPMULTISPY ) );
        }
    }
    else
    {
        OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPMULTISPY ) );        
    }
}



API void changemtres( u8 byConfIdx = 0, u8 byMtIdx = 0, u8 byRes = 3 )
{
    if ( 0 != byConfIdx )
    {
        if ( byConfIdx > MAX_CONFIDX )
        {
            StaticLog( "Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            StaticLog( "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
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
        StaticLog( "confIdx.%d is illegal, try again ...\n" );
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
        StaticLog( "Unexpected confIdx.%d, try again ...\n", byConfIdx );
        return;
    }
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if ( NULL == pVcInst )
    {
        StaticLog( "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
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
        StaticLog( "Unexpected confIdx.%d, try again ...\n", byConfIdx );
        return;
    }
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if ( NULL == pVcInst )
    {
        StaticLog( "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
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

API void showprs()
{
    OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( 0, DUMPPRSINFO ) );    
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPPRSINFO ) ); 
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
            StaticLog( "Unexpected confIdx.%d, try again ...\n", byConfIdx );
            return;
        }
        CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if ( NULL == pVcInst )
        {
            StaticLog( "ConfIdx.%d is an unexist conf, try again ...\n", byConfIdx );
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
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
		TPeriEqpStatus * ptStatus = &tStatus;
		if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
        if (g_cMcuAgent.IsEqpBasHD(byEqpId) )
#endif 
        {
			u8 byChnIdx = 0;
			u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
			
			if (byEqpType == TYPE_MAU_NORMAL || byEqpType == TYPE_MAU_H263PLUS)
			{
				if ( byEqpType== TYPE_MAU_NORMAL)
				{
					StaticLog( "BAS.%d(mau-normal) chan status:\n", byEqpId);
				}
				if (byEqpType == TYPE_MAU_H263PLUS)
				{
					StaticLog( "BAS.%d(mau-h263p) chan status:\n", byEqpId);
				}
				for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
				{
					StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->GetStatus());
				}
				
				for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_DVCHN; byChnIdx++)
				{
					StaticLog( "\tds <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx)->GetStatus());
				}
			}
			else if (byEqpType == TYPE_MPU)
			{
				StaticLog( "BAS.%d(mpu) chan status:\n", byEqpId);
				for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_VCHN; byChnIdx++)
				{
					StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetStatus());
				}
			}
			else if(byEqpType == TYPE_MPU_H)
			{
				StaticLog( "BAS.%d(mpu_h) chan status:\n", byEqpId);
				for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_H_CHN; byChnIdx++)
				{
					StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->GetStatus());
				}
			}
			else if(byEqpType == TYPE_8KE_BAS)
			{
				byChnIdx = 0;
				StaticLog( "BAS.%d(8kebas) chan status:\n", byEqpId);
				StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
					ptStatus->m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
					ptStatus->m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
					ptStatus->m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(byChnIdx)->GetStatus());
			}
			else if(byEqpType == TYPE_8KH_BAS)
			{
				byChnIdx = 0;
				OspPrintf(TRUE, FALSE, "BAS.%d(8khbas) chan status:\n", byEqpId);
				OspPrintf(TRUE, FALSE, "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
					ptStatus->m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
					ptStatus->m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
					ptStatus->m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(byChnIdx)->GetStatus());
			}
			else if(byEqpType == TYPE_8KI_VID_BAS)
			{
				byChnIdx = 0;
				OspPrintf(TRUE, FALSE, "BAS.%d(8kibas) chan status:\n", byEqpId);
				OspPrintf(TRUE, FALSE, "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
					ptStatus->m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
					ptStatus->m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(byChnIdx)->GetReservedType(),
					ptStatus->m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(byChnIdx)->GetStatus());
			}
			else if ( TYPE_MPU2_BASIC == byEqpType )
			{
				StaticLog( "BAS.%d(mpu2_basic) chan status:\n", byEqpId);
				for (byChnIdx = 0; byChnIdx < MAXNUM_MPU2_BASIC_CHN; byChnIdx++)
				{
					StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tHdBas.tStatus.tMpu2Bas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpu2Bas.GetVidChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpu2Bas.GetVidChnStatus(byChnIdx)->GetStatus());
				}
			}
			else if ( TYPE_MPU2_ENHANCED == byEqpType )
			{
				StaticLog( "BAS.%d(mpu2_enhanced) chan status:\n", byEqpId);
				for (byChnIdx = 0; byChnIdx < MAXNUM_MPU2_EHANCED_CHN; byChnIdx++)
				{
					StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tHdBas.tStatus.tMpu2Bas.GetVidChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpu2Bas.GetVidChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tHdBas.tStatus.tMpu2Bas.GetVidChnStatus(byChnIdx)->GetStatus());
				}
			}			
        }
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
		else if ( g_cMcuAgent.IsEqpBasAud(byEqpId)  )
#endif
		{
			u8 byAudType = tStatus.m_tStatus.tAudBas.GetEqpType();
			if (byAudType == TYPE_APU2_BAS )
			{
				StaticLog( "BAS.%d(apu2) chan status:\n", byEqpId);				
				for (u8 byChnIdx = 0; byChnIdx < MAXNUM_APU2_BASCHN; byChnIdx++)
				{
					StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tAudBas.m_tAudBasStaus.m_tApu2BasChnStatus.GetAudChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tAudBas.m_tAudBasStaus.m_tApu2BasChnStatus.GetAudChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tAudBas.m_tAudBasStaus.m_tApu2BasChnStatus.GetAudChnStatus(byChnIdx)->GetStatus());
				}
			}
			else if( byAudType == TYPE_8KI_AUD_BAS )
			{
				StaticLog( "BAS.%d(8kiaudbas) chan status:\n", byEqpId);				
				for (u8 byChnIdx = 0; byChnIdx < MAXNUM_8KI_AUD_BASCHN; byChnIdx++)
				{
					StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
						ptStatus->m_tStatus.tAudBas.m_tAudBasStaus.m_t8KIAudBasStatus.GetAudChnStatus(byChnIdx)->GetChnIdx(),
						ptStatus->m_tStatus.tAudBas.m_tAudBasStaus.m_t8KIAudBasStatus.GetAudChnStatus(byChnIdx)->GetReservedType(),
						ptStatus->m_tStatus.tAudBas.m_tAudBasStaus.m_t8KIAudBasStatus.GetAudChnStatus(byChnIdx)->GetStatus());
				}
			}
		}
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
		else
		{
			StaticLog( "BAS.%d(vpu) chan status:\n", byEqpId);
			for (u8 byChnIdx = 0; byChnIdx < MAXNUM_VPU_OUTPUT; byChnIdx++)
			{
				StaticLog( "\tmv <chnIdx.%d, Res.%d, Status.%d>\n",
					byChnIdx,
					ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].IsReserved(),
					ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].GetStatus());			
			}
		}
#endif
	}
}

API BOOL32 showtw(u32 dwHduId)
{
	if ( !dwHduId || (dwHduId >= HDUID_MIN && dwHduId <= HDUID_MAX) )
	{
		OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( (u8)dwHduId, DUMPTWINFO ) );
		return TRUE;
	}
	else
	{
		StaticLog("EqpId is no valid\n");
		return FALSE;
	}
}

API void showbaslist()
{
	OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( 0, DUMPBASLIST ) );	
}

API void showbasall()
{
    OspInstDump( AID_MCU_VC, CInstance::DAEMON, MAKEDWORD( 0, DUMPBASINFO ) );
}

API void showtoken()
{
	OspInstDump( AID_MCU_VC, CInstance::EACH, MAKEDWORD( 0, DUMPTOKENOWNER ) ); 
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
	StaticLog( "Data Information In McSsn:\n" );
	StaticLog( "INSTID NODE \n" );
	StaticLog( "------ ---- \n" );
	
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
        StaticLog( "LocalNPlusState: %s, Configed backup MCU.0x%x\n", 
                                         achState, tLocalInfo.GetNPlusMcuIp() );
    }
    else if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        strcpy(achState, "N-Mode-Connected");
        StaticLog( "LocalNPlusState: %s, Configed backup MCU.0x%x\n", 
                                         achState, tLocalInfo.GetNPlusMcuIp() );
    }
    else if (MCU_NPLUS_SLAVE_IDLE == g_cNPlusApp.GetLocalNPlusState())
    {
        strcpy(achState, "1-Mode-IDLE");
        StaticLog( "LocalNPlusState: %s RtdTime:%d RtdNum:%d\n", 
                  achState, g_cNPlusApp.GetRtdTime(), g_cNPlusApp.GetRtdNum());
    }
    else if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        strcpy(achState, "1-Mode-Switched");
        StaticLog( "LocalNPlusState: %s RtdTime:%d RtdNum:%d, switched ip<0x%x>\n", 
            achState, g_cNPlusApp.GetRtdTime(), g_cNPlusApp.GetRtdNum(), g_cNPlusApp.GetMcuSwitchedIp() );
    }
    else
    {
        strcpy(achState, "NONE N+1");
    }
    OspInstDump( AID_MCU_NPLUSMANAGER, CInstance::EACH, 0 );
}

/*=============================================================================
函 数 名： shownpluscap
功    能： 打印n＋1备份能力集信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/02/09  4.0			陈代伟                  创建
=============================================================================*/
API void shownpluscap()
{
    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
	TNPlusEqpCapEx tLocalCapEx = g_cNPlusApp.GetMcuEqpCapEx();

	tLocalCap.Print();
	tLocalCapEx.Print();
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
	g_cVCSUsrManage.CmdLine( 0 );
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
	StaticLog( "\nData Information In EqpSsn and DcsSsn:\n" );

	StaticLog( "INSTID EQPID NODE TYPE   ALIAS\n" );
	StaticLog( "------ ----- ---- ----   -------\n" );
	OspInstDump( AID_MCU_PERIEQPSSN, CInstance::EACH, 0 );
    OspInstDump( AID_MCU_DCSSSN,     CInstance::EACH, 0 );
	StaticLog( "________________________________\n" );
    
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
        StaticLog( "Mcu has registered to GK!\n" );
    }
    else
    {
        StaticLog( "Mcu not register to GK!\n" );
    }
}


/*====================================================================
    函数名      ：sshctrl
    功能        ：开关ssh连接
    算法实现    ：
    引用全局变量 u8 byStart ：1 开启ssh服务，0：停掉ssh服务
    输入参数说明：
    返回值说明  ：int: 0: ok; -1: failed
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    1/4/2013    1.0         彭国锋        创建
====================================================================*/
API int sshctrl(u8 byStart)
{
	if ( byStart == 1)
	{
		int nRet = system("service sshd restart");
		if ( 0 == nRet )
		{
			StaticLog("start sshd succeed!\n");
		}
		else
		{
			StaticLog("start sshd ret:%d,errorNo:%d!\n",nRet,errno);
		}
	}
	else if ( byStart == 0)
	{
		int nRet = system("service sshd stop");
		if ( 0 == nRet )
		{
			StaticLog("stop sshd succeed!\n");
		}
		else
		{
			StaticLog("stop sshd ret:%d,errorNo:%d!\n",nRet,errno);
		}
		
		errno = 0;
		nRet = system("pkill sshd"); //杀死当前所有的ssh连接
		if ( 0 == nRet )
		{
			StaticLog("pkill sshd succeed!\n");
		}
		else
		{
			StaticLog("pkill sshd ret:%d,errorNo:%d!\n",nRet,errno);
		}
	}
	else
	{
		StaticLog("invalid parameter: %d\n", byStart);
		return -1; //err
	}

	return 0; // OK
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

//模拟测试E1接口 注释保留[11/6/2012 chendaiwei]
/*
API void setmcue1bw(u32 dwlocalDri2Ip,u16 wRealBW1, u8 byE1cfgNum1,u16 wRealBW2, u8 byE1cfgNum2)
{
	//虚端口带宽
    u16 wBandWidthEachPort[MAXNUM_E1PORT] = {0};
	wBandWidthEachPort[0] = wRealBW1;
	wBandWidthEachPort[1] = wRealBW2;
    //虚端口配置E1对数
    u8 byPortE1Num[MAXNUM_E1PORT] ={0};
	byPortE1Num[0] = byE1cfgNum1;
	byPortE1Num[1] = byE1cfgNum2;
	dwlocalDri2Ip = htonl(dwlocalDri2Ip);
    
	u8 byBuf[sizeof(wBandWidthEachPort)+sizeof(byPortE1Num)+sizeof(u32)] = {0};
	memcpy(&byBuf[0],&wBandWidthEachPort[0],sizeof(wBandWidthEachPort));
	memcpy(&byBuf[sizeof(wBandWidthEachPort)],&byPortE1Num[0],sizeof(byPortE1Num));
	memcpy(&byBuf[sizeof(wBandWidthEachPort)+sizeof(byPortE1Num)],&dwlocalDri2Ip,sizeof(dwlocalDri2Ip));

	OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), AGT_SVC_E1BANDWIDTH_NOTIF,&byBuf[0],sizeof(wBandWidthEachPort)+sizeof(byPortE1Num)+sizeof(u32));
}

API void setmte1bw(u8 byMtId, u16 wBandwidth)
{
	wBandwidth = htons(wBandwidth);

	CServMsg cServMsg;
	cServMsg.SetSrcMtId(byMtId);
	cServMsg.SetEventId(MT_MCU_BANDWIDTH_NOTIF);
	cServMsg.SetMsgBody((u8*)&wBandwidth,sizeof(wBandwidth));
	cServMsg.CatMsgBody((u8*)&wBandwidth,sizeof(wBandwidth));

	OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), cServMsg.GetEventId(),cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
}
*/

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
	StaticLog("\nshowmp()-----show mp info\n");
	StaticLog("ssw(%d)--------show switch info\n");
	StaticLog("showbridge()---show switch bridge info\n");

	return;
}

// guzh  [12/13/2006] NPLUS测试
API void nplusreset()
{
    OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), VC_NPLUS_RESET_NOTIF);
}

// [9/19/2011 liuxu] kdvlog flush时添加信息
API void mculogflush()
{
	StaticLog("\n*******************************************************************\n");

	mcuver();
	StaticLog("\n*******************************************************************\n");

	showconfinfo();
	StaticLog("\n*******************************************************************\n");
	
	showconfmt();
	StaticLog("\n*******************************************************************\n");
	
	showmcothertable();
	StaticLog("\n*******************************************************************\n");

	showmtstat();
	StaticLog("\n*******************************************************************\n");

	ssw(2);
	StaticLog("\n*******************************************************************\n");
	
	eqp(0);
	StaticLog("\n*******************************************************************\n");
	
	showbas();
	StaticLog("\n*******************************************************************\n");
	
	showtw(0);
	StaticLog("\n*******************************************************************\n");

	showconfspy();
	StaticLog("\n*******************************************************************\n");
	
	showvcsextracfg();
	StaticLog("\n*******************************************************************\n");

	showdebug();
	
	OspDelay(20);
	logflush();
	OspDelay(20);
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
	StaticLog( "MCU Ver: %s   Compile Time: %s, %s\n", VER_MCU, __TIME__, __DATE__ );
    // StaticLog( "Max Ongoing Conf Supported : %d\n", g_cMcuVcApp.m_byMaxOngoConfSupported );
    // telnet 授权
    StaticLog( "mcutau(UsrName,Pwd): mcu telnet author.\n" );

	StaticLog( "\nFollowing are all debug commands in MCU module:\n" );
	StaticLog( "mcuver:             print version of current MCU.\n" );
	StaticLog( "Reboot:             Reboot current MCU.\n\n" );
    StaticLog( "showdebug:          print mcu debug and performance info.\n" );
	
	StaticLog( "showconfprint:      show conf print command.\n" );
	StaticLog( "showprint:          show mcu print command.\n" );
	StaticLog( "mpmanagerhelp:      show mpmanager print command.\n\n" );

	//master-slave
	StaticLog( "msdebug(nDbgLvl):   set the debug level of master-slave.\n" );
	StaticLog( "showmsstate:        show master-slave state.\n" );
    StaticLog( "setmschecktime:     set master-slave check time.\n\n");

    //n+1
    StaticLog( "shownplus:          show nplus state.\n\n");
    StaticLog( "nplusreset:         Reset Nplus Backup server to idle. All conf will lose.\n\n");
    StaticLog( "shownpluscap:       show local mcu eqp capbility.\n\n");
	 
    // mcu guard
    StaticLog( "mcuguarddisable:    disable mcu guard reboot.\n" );
    StaticLog( "mcuguardenable:     enable mcu guard reboot.\n" );
	StaticLog( "showlicense:        show license.\n" );
	
	//mcs
	StaticLog( "showmc:             print information of all mcs.\n" );
	StaticLog( "showmcsrc:          print information of all mcs rcv_or_snd_channel.\n" );
	StaticLog( "showalluser:        show all user mcs can use.\n\n" );

	//cri/dri reg info
	StaticLog( "showdri:            print information of all cri/dri.\n" );
	StaticLog( "showmtadp:          print information of all mtadp module.\n" );
	StaticLog( "showmp:             print information of all mp module.\n\n" );

	//eqp reg info
	StaticLog( "showeqp:            print information of all eqp module.\n" );
	StaticLog( "eqp(EqpId):         show Eqp status with param EqpId.\n\n" );
	
	//mcu->gk info
	StaticLog( "showgk:             print register GK info.\n\n" );

	StaticLog("sshctrl(byStart):	start or stop ssh connection service\n");

    //addr
    StaticLog( "showaddrbook:       print address book info.\n\n" );

	//依赖模块
	StaticLog( "umhelp:             print help of userlist book.\n" );
	StaticLog( "addrhelp:           print help of address book.\n" );
	StaticLog( "mcuagenthelp:       print help of mcuagent module.\n" );
	StaticLog( "osphelp:            print help of osp module.\n\n" );
	
	//可选模块
	StaticLog( "dshelp:             print help of dataswitch module.\n" );
	StaticLog( "mphelp:             print help of mp module.\n" );
	StaticLog( "mtadphelp:          print help of mtadp module.\n" );
	StaticLog( "vmphelp:            print help of vmp module.\n" );
	StaticLog( "mixhelp:            print help of mixer module.\n" );
	StaticLog( "prshelp:            print help of prs module.\n" );
	StaticLog( "netbufhelp:         print help of netbuf module.\n" );
	StaticLog( "kdvmedianethelp:    print help of medianet module.\n\n" );

    //级联测试
    //StaticLog( "showsmcucasport:    print SMCU cascade listen port(for debug only).\n");

	StaticLog( "sconftotemp(conf name string):	save conf as template.\n\n");
	StaticLog( "smstimer:			 show or set sms delay time");
    
	return;
}

#ifdef _LINUX_
API void querynipstate(void)
{
    s32 nRet = BrdQueryNipState();
    StaticLog( "BrdQueryNipState ret.%d\n", nRet);
}
#endif

void McuAPIRegInLinux(void)
{
#ifdef _LINUX_    
// 将void func()转为void* func时pclint会出错
/*lint -save -e611*/
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
    OspRegCommand("shownpluscap",       (void*)shownpluscap,    "Show NPlus local mcu eqp capbility");

	OspRegCommand("svam",				(void*)svam,			"Show VMP adapter channel info");
    OspRegCommand("showconfall",        (void*)showconfall,     "show conf all info");
    OspRegCommand("showconfinfo",       (void*)showconfinfo,    "show conf info");
    OspRegCommand("showconfswitch",     (void*)showconfswitch,  "show conf switch");    
    OspRegCommand("showmtmonitor",      (void*)showmtmonitor,   "show mt monitor");    
    OspRegCommand("showconfmt",         (void*)showconfmt,      "show conf mt");    
	OspRegCommand("showconfmcu",        (void*)showconfmcu,     "show conf mcu");
    OspRegCommand("showconfeqp",        (void*)showconfeqp,     "show conf eqp");
	OspRegCommand("showconfmix",        (void*)showconfmix,     "show conf mix");
	OspRegCommand("showconfspy",        (void*)showconfspy,     "show conf spy of Local Info and SMcu Info");
    OspRegCommand("showmtstat",         (void*)showmtstat,      "show mt stat");    
    OspRegCommand("showtemplate",       (void*)showtemplate,    "show template");    
    OspRegCommand("showconfmap",        (void*)showconfmap,     "show conf map");
	OspRegCommand("showbas",			(void*)showbas,         "show bas info");
	OspRegCommand("showtw",			    (void*)showtw,          "show tw info");
	OspRegCommand("showprs",			(void*)showprs,         "show prs info");
	OspRegCommand("showtoken",			(void*)showtoken,       "show token info");
	OspRegCommand("showbaslist",		(void*)showbaslist,     "show bas list info");
    OspRegCommand("showbasall",			(void*)showbasall,      "show bas info");
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

	OspRegCommand("pspymsg",           (void*)pspymsg,		"close print multispy msg");
	OspRegCommand("npspymsg",           (void*)npspymsg,    "print multispy msg");

	OspRegCommand("sconftotemp",		(void*)sconftotemp, "save conf to be a template");

    OspRegCommand("addrhelp",           (void*)addrhelp,    "Addrbook help");
    OspRegCommand("addrbookver",        (void*)addrbookver, "Addrbook version");
    OspRegCommand("prtaddrbook",        (void*)prtaddrbook, "Addrbook Print");
    OspRegCommand("addrlogon",          (void*)addrlogon,   "Addrbook Debug Enable");
    OspRegCommand("addrlogoff",         (void*)addrlogoff,  "Addrbook Debug Disable");   
	OspRegCommand("ssl",				(void*)ssl,			"Show apply speaker list");    
	OspRegCommand("psl",				(void*)psl,			"Print apply speaker list");    
	OspRegCommand("npsl",				(void*)npsl,		"Stop print apply speaker list");
	OspRegCommand("flowctrl",			(void*)flowctrl,    "send flowcontrol cmd to mt");
	OspRegCommand("smstimer",			(void*)smstimer,    "set or show sms timerOut");
	OspRegCommand("sswi",				(void*)sswi,		"show switch (detailed)info");
	OspRegCommand("showmcothertable",	(void*)showmcothertable, "showmcothertable info");
	OspRegCommand("showvcsextracfg",	(void*)showvcsextracfg, "showvcsextracfg info");
	OspRegCommand("mculogflush",		(void*)mculogflush, "mcu log flush into disk");
	OspRegCommand("sshctrl",			(void*)sshctrl, "start or stop ssh connection service");
// 	OspRegCommand("setmcue1bw",			(void*)setmcue1bw,  "set Mmcu(smcu) e1 bandwidth");
// 	OspRegCommand("setmte1bw",			(void*)setmte1bw,  "set mt e1 bandwidth");
/*lint -restore*/
#endif
}

#ifdef _LINUX_

BOOL32 g_bQuit = FALSE;
void OspDemoQuitFunc( int nSignalNum )
{
	g_bQuit = TRUE;

	printf("nSingnalNum:%d\n",nSignalNum);
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

#ifdef _LINUX12_
	s8 byRet = BrdSetSysRunSuccess();
	switch (byRet)
	{
	case NO_UPDATE_OPERATION:
		printf("[BrdSetSysRunSuccess]start mcu normally\n");
		break;
	case UPDATE_VERSION_ROLLBACK:
		printf("[BrdSetSysRunSuccess]fail to update, rollback version\n");
		break;
	case UPDATE_VERSION_SUCCESS:
		printf("[BrdSetSysRunSuccess]succeed to update\n");
		break;		
	case SET_UPDATE_FLAG_FAILED:
		printf("[BrdSetSysRunSuccess]fail to set run flag\n");
		break;
	default:
		printf("[BrdSetSysRunSuccess]bad callback param\n");
		break;
	}
#endif //_LINUX12_

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


BOOL32 CtrlHandler(DWORD fdwCtrlType)
{
	switch( fdwCtrlType )
	{
		// Handle the CTRL-C signal.
	case CTRL_C_EVENT:
		// CTRL-CLOSE: confirm that the user wants to exit.
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		{
			userquit();
			//PostMessage(g_hwndMain, WM_CLOSE, 1, NULL);
		}
		break;
		
	default:
		{
			
		}
		break;
	}
	
	return FALSE;
}

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

	if ( NULL == g_hwndMain )
    {
		MessageBox (NULL, "Create window error!", CLASS_MCU, MB_ICONERROR) ;
		return -1;
    }else
	{

    #ifdef _DEBUG
        // debug 不显示    
        ShowWindow(g_hwndMain, SW_HIDE);
    #else
        ShowWindow(g_hwndMain, nCmdShow);
    #endif // _DEBUG
    
	UpdateWindow(g_hwndMain);
	
	}


    // 启动 Console
    if (g_bOpenTelnetWin32)
    {
        AllocConsole();
        freopen("CONOUT$","w+t",stdout);          
        freopen("CONIN$","r+t",stdin);

		if( SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) )
		{
			printf( "\nThe Control Handler is installed.\n" );
			printf( "\n -- Now try pressing Ctrl+C or Ctrl+Break, or" );
			printf( "\n try logging off or closing the console...\n" );
			printf( "\n(...waiting in a loop for events...)\n\n" );
		}

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
	case WM_CLOSE:
		{
			GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);

			// 退出系统
// 			if (g_bOpenTelnetWin32)
// 			{
// 				FreeConsole();
// 			}
			
			//userquit();
			
			return 0;
		}

    case WM_DESTROY:
		{
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
/*lint -save -esym(530, regs)*/
/*lint -save -e578*/
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
	
	BOOL32 bLoop = TRUE;
	while(bLoop)
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

/*lint -restore*/
/*lint -restore*/
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
		if ( (s32)syml->st_name >= strsize)
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


	// [6/29/2011 liuxu] 异常发生时，需要flush
	mculogflush();
	OspDelay(200);

	/* sigment fault is critical fault, we should exit right now */
	exit(1);
}

void install_sigaction()
{
	struct sigaction sigact;

	sigemptyset (&sigact.sa_mask);
	//memset(&sigact, 0, sizeof (struct sigaction));
	sigact.sa_flags = (s32)(SA_ONESHOT | SA_SIGINFO);
	sigact.sa_sigaction = my_sigaction;
	sigaction(SIGSEGV , &sigact, NULL);	
}



/* get the function'symbol that the "addr" is in the function or the same of the function */
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr)
{	
/*lint -save -e574*/
	for (;tabsize > 0; tabsize -= (int)sizeof(Elf32_Sym), symtab ++)
	{		
#if 1
		if (ELF32_ST_TYPE(symtab->st_info) != STT_FUNC || symtab->st_shndx == 0)/* we only find the function symbol and not a und  */	
			continue;
		if (!symtab->st_value)
			continue;
		if (addr >= (int)symtab->st_value && addr < (int)(symtab->st_size+symtab->st_value))
			return symtab;	
#else
		if (ELF32_ST_TYPE(symtab->st_info) == STT_FUNC)/* we only find the function symbol */	
			printf("name %s addr %x size %x\r\n", (strtab+symtab->st_name), symtab->st_value, symtab->st_size);
#endif			
	}
	return NULL;
/*lint -restore*/
}	


Elf32_Sym *get_symbol(Elf32_Sym *symtab, int tabsize, int addr)
{
	for (;tabsize > 0; tabsize -= (int)sizeof(Elf32_Sym), symtab ++)
		if ((int)symtab->st_value == addr)
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
		free(pshdr);
		printf("fseek error\r\n");
		return NULL;
	}	
	/*lint -save -e40*/
	if (fread(pshdr, s_size, 1, file) != 1)
	{
		free(pshdr);
		printf("read file error in func %s\r\n", __func__);
		return NULL;
	}
	/*lint -restore*/

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
	
	/*lint -save -e40*/
	sym = (void *)malloc(tb_size);
	if (sym ==NULL)
	{	
		printf("malloc error in func %s\r\n", __func__);
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
		printf("fseek error\r\n");
		free(sym);
		return NULL;
	}	

	if (fread(sym, tb_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		free(sym);
		return NULL;
	}
	/*lint -restore*/	

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
	/*lint -save -e40*/
	sym = (void *)malloc(tb_size);
	if (sym ==NULL)
	{	
		printf("malloc error in func %s\r\n", __func__);
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
		printf("fseek error\r\n");
		free(sym);
		return NULL;
	}	

	if (fread(sym, tb_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		free(sym);
		return NULL;
	}	
	/*lint -restore*/
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


#endif // _LINUX_

/////////////////////////////////////////////////////异常捕获处理

//END OF FILE

