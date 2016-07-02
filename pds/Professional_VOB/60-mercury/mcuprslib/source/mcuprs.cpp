/*****************************************************************************
   模块名      : 重传(Packet Resend)实现
   文件名      : mcuprsinst.cpp
   创建时间    : 2003年 12月 4日
   实现功能    : include : /include/mcuprsinst.h
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   04/3/25	   1.0	       zhangsh	   create
   06/03/21    4.0         张宝卿      代码优化  
******************************************************************************/
#include "mcuprsinst.h"
#include "mcuprsguard.h"
//#include "kdvtype.h"
//#include "osp.h"
#include "mcuver.h"
//#include "mcustruct.h"
//#include "evmcueqp.h"
//#include "eqpcfg.h"

#ifdef _LINUX_
#include "dsccomm.h"
#include "bindwatchdog.h"
//#include <sys/types.h>
//#include <dirent.h>
#endif

#ifdef _VXWORKS_
#include <dirent.h>
#endif

#ifdef WIN32
#include <windows.h>
//#include <conio.h>
#include <stdio.h>
#endif

//extern CMcuPrsApp g_cMcuPrsApp;
extern s32  g_nprslog;
//extern CMcuPrsGuardApp g_cMcuPrsGuardApp;
extern s32  g_nprsguardlog;

BOOL32 CreatPrsDir(const s8* lpszDirPath)
{
	if ( NULL == lpszDirPath )
	{
		return FALSE;
	}
	
	s8 achDirPath[64];
	memset(achDirPath, 0, sizeof(achDirPath));
	sprintf(achDirPath, "%s", lpszDirPath);
	
#ifdef _LINUX_
	s32 nRet = 0;

	s8 ch = '/';
	s8 *pchDest = NULL;
	pchDest = strrchr( achDirPath, ch );

    DIR* pdir = opendir(achDirPath);
    if ( pdir != NULL )
	{
		closedir(pdir);
		chmod(achDirPath, 0777);
		return TRUE;
	}
	
	if ( pchDest == NULL || pchDest == achDirPath )
	{
		nRet = mkdir( achDirPath, 0777 );
		if( 0 != nRet )
		{
			printf("make diretory<%s> Error :%s\n",achDirPath,strerror(errno)); 
            return FALSE;
		}
	}
	else
	{
		*pchDest = '\0';
		if ( CreatPrsDir(achDirPath) )
		{
			nRet = mkdir( lpszDirPath, 0777 );
			if( 0 != nRet )
			{
				printf("make diretory<%s> Error :%s\n",lpszDirPath,strerror(errno));  
				return FALSE;
			}
        }
		else
		{
			return FALSE;
		}
	}		                
#endif
#ifdef _VXWORKS_
	mkdir( ( s8* )lpszDirPath );
#endif
#ifdef WIN32
	if ( !CreateDirectory( DIR_CONFIG, NULL ) )
	{
		return FALSE;
	}
#endif
	return TRUE;
}

#ifdef WIN32
API bool prsinit(TPrsCfg* tPrsCfg)
#else
/*lint -save -e714*/
bool prsinit(TPrsCfg* tPrsCfg)
#endif
{
	/*lint -save -e750*/
    #undef OSPEVENT	
    #define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
    
#ifdef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
    //#include "evmcueqp.h"
    #define _EV_MCUEQP_H_
#else
    //#include "evmcueqp.h"
    #undef _EV_MCUEQP_H_
#endif
	
    if( 0 == tPrsCfg->dwConnectIP && 0 == tPrsCfg->dwConnectIpB )
    {
        LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON, "[Error] The Mcu A and B's Ip are all 0 !\n");
        return FALSE;
    }
    if( tPrsCfg->dwConnectIP == tPrsCfg->dwConnectIpB )
    {
        tPrsCfg->dwConnectIpB  = 0;
        tPrsCfg->wConnectPortB = 0;
    }

    g_cMcuPrsApp.CreateApp( "MCUPRS", AID_PRS, APPPRI_PRS );
	g_cMcuPrsGuardApp.CreateApp( "MCUPRSGUARD", AID_PRS_GUARD, APPPRI_PRS );
	
    TPrsCfg tPrsInfo = *(TPrsCfg *)tPrsCfg;
	LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"send EV_PRS_INIT ip:0X%x,port:%d\n",tPrsInfo.dwConnectIP, tPrsInfo.wConnectPort);
	printf("send EV_PRS_INIT ip:0X%x,port:%d\n",tPrsInfo.dwConnectIP, tPrsInfo.wConnectPort);
    /*s32 nRet = */OspPost( MAKEIID(AID_PRS, 1), EV_PRS_INIT, (u8 *)&tPrsInfo, sizeof(TPrsCfg) );
	// 增加prs守卫, zgc, 2008-04-07
	OspPost( MAKEIID(AID_PRS_GUARD, 1), OSP_POWERON );
#ifdef _LINUX_
	WDStartWatchDog( emPRS );
#endif	
    //LINUX下需将API注册  
    PrsAPIEnableInLinux();
    
    return TRUE;
	
}

/*=============================================================================
  函 数 名： prsver
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
=============================================================================*/
API void prsver(void)
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
	OspPrintf( TRUE, FALSE, "PRS Version: %s\n", gs_VersionBuf);
	OspPrintf(TRUE, TRUE, "prs version:%s\t",VER_PRS);
	OspPrintf(TRUE, TRUE, "compile time:%s:%s\n", __DATE__, __TIME__);
}

/*=============================================================================
  函 数 名： prshelp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void prshelp()
{
	OspPrintf( TRUE, TRUE, "prs version:%s\t", VER_PRS );
	OspPrintf( TRUE, TRUE, "compile time:%s:%s\n", __DATE__, __TIME__ );
	OspPrintf( TRUE, TRUE, "prsstatus ---- Display All channel information\n" );
	OspPrintf( TRUE, TRUE, "pprsmsg   ---- Display message/params while running or not\n" );
    OspPrintf( TRUE, TRUE, "npprsmsg  ---- not Display message/params while running or not\n" );
	OspPrintf( TRUE, TRUE, "pprsguardmsg   ---- Display prsguard message/params while running or not\n" );
    OspPrintf( TRUE, TRUE, "npprsguardmsg  ---- not Display prsguard message/params while running or not\n" );
    OspPrintf( TRUE, TRUE, "prstau(UsrName,Pwd) ---- Prs telnet author\n");
}

/*=============================================================================
  函 数 名： prsstatus
  功    能： 显示Prs状态
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void prsstatus()
{
	OspPost( MAKEIID(AID_PRS, 1), EV_PRS_STATUS, NULL, 0 );
}

/*=============================================================================
  函 数 名： prstau
  功    能： Prs应用telnet授权
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void prstau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

/*=============================================================================
  函 数 名： pprsmsg
  功    能： 打印Prs信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void pprsmsg()
{
	g_nprslog = 1;
}

/*=============================================================================
  函 数 名： npprsmsg
  功    能： 不打印Prs信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void npprsmsg()
{
    g_nprslog = 0;
}

/*=============================================================================
  函 数 名： pprsguardmsg
  功    能： 打印Prs守卫信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void pprsguardmsg()
{
	g_nprsguardlog = 1;
}

/*=============================================================================
  函 数 名： npprsguardmsg
  功    能： 不打印Prs守卫信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void npprsguardmsg()
{
    g_nprsguardlog = 0;
}

void PrsAPIEnableInLinux()
{
	/*lint -save -e611*/
#ifdef _LINUX_    
    OspRegCommand("prsver",    (void*)prsver,    "prs version command");
    OspRegCommand("prshelp",   (void*)prshelp,   "prs help command");    
    OspRegCommand("pprsmsg",   (void*)pprsmsg,   "prs msg print command");
    OspRegCommand("npprsmsg",  (void*)npprsmsg,  "prs msg not print command");
	OspRegCommand("pprsguardmsg",   (void*)pprsguardmsg,   "prs guard msg print command");
    OspRegCommand("npprsguardmsg",  (void*)npprsguardmsg,  "prs guard msg not print command");
    OspRegCommand("prsstatus", (void*)prsstatus, "prs status show command");
    OspRegCommand("prstau",    (void*)prstau,    "prs telnet author");
#endif
	/*lint -restore*/
}