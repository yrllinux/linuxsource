/*****************************************************************************
   模块名      : 多画面显示
   文件名      : twinst.h
   相关文件    : 
   文件实现功能: 多画面显示初始化
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
 -----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/20     		   zhangsh	
******************************************************************************/
#include "tvwall.h"
#include "twinst.h"
//#include "tvwall.h"
//#include "kdvsys.h"
//#include "mcustruct.h"
#include "evtvwall.h"
#include "mcuver.h"

CTvWallApp g_cTvWallApp;

void TvWallAPIEnableInLinux(void);

BOOL twInit(TEqpCfg* ptCfg)
{
    TvWallAPIEnableInLinux();

	OspOpenLogFile("tw.log", 500, 3);

	g_cTvWallApp.CreateApp("TvWall", AID_TVWALL, TVWALL_PRI);	//创建电视墙应用

 	#undef OSPEVENT
	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )

#ifdef _EVENET_TVWALL_H_
	#undef _EVENET_TVWALL_H_
	//#include "evtvwall.h"
	//#define _EVENET_TVWALL_H_
#else
	#include "evtvwall.h"
    #undef _EVENET_TVWALL_H_
#endif

#ifdef _EV_MCUEQP_H_
	#undef _EV_MCUEQP_H_
	#include "evmcueqp.h"
	#define _EV_MCUEQP_H_
#else
	#include "evmcueqp.h"
    #undef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
#endif

	OspStatusMsgOutSet(FALSE);

	//jlb[10/17/2008] 代码冗余，整理
	//CServMsg cServMsg;
	//cServMsg.SetMsgBody(NULL, 0);	
	::OspPost(MAKEIID(AID_TVWALL, 1), EV_TVWALL_INIT, ptCfg, sizeof(TEqpCfg));

	return TRUE;
}

API void twver()
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
	OspPrintf( TRUE, FALSE, "TVWALL Version: %s\n", gs_VersionBuf);
    ::OspPrintf(TRUE, FALSE, "tw version : %s compile time : %s    %s\n", VER_TW, __TIME__, __DATE__);
}

API void twhelp()
{
	::OspPrintf(TRUE, FALSE, "\n\t\t KDV TV Wall \n");
	::OspPrintf(TRUE, FALSE, "TV Wall Compile Time : %s, %s\n", __TIME__, __DATE__);
	::OspPrintf(TRUE, FALSE, "TV Wall Verion       : %s\n\n", VER_TW);
    ::OspPrintf(TRUE, FALSE, "twsetlog(1:print, 0: not print)  :Print the deubg infomation\n");
	::OspPrintf(TRUE, FALSE, "twShow               :Show TV Wall Status\n");
	::OspPrintf(TRUE, FALSE, "twCfgShow            :Show TV Wall Config\n");
    ::OspPrintf(TRUE, FALSE, "twtau(UsrName, Pwd)  :TVWall telnet author\n");
}

API void twShow()
{ 
	::OspPost(MAKEIID(AID_TVWALL, 1), EV_TVWALL_STATUSSHOW);
}

API void twtau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

API void twCfgShow()
{
	::OspPost(MAKEIID(AID_TVWALL, 1), EV_TVWALL_CONFIGSHOW);
}

extern s32 g_nlog;
API void  twsetlog(int nlvl)
{
    g_nlog = nlvl;
}


/*=============================================================================
函 数 名： TvWallAPIEnableInLinux
功    能： 注册调试接口
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/15  4.0			许世林                  创建
=============================================================================*/
void TvWallAPIEnableInLinux(void)
{
	//lint -save -e611
#ifdef _LINUX_      
    OspRegCommand("twver",          (void*)twver,           "twver");
    OspRegCommand("twhelp",         (void*)twhelp,          "twhelp");
    OspRegCommand("twShow",         (void*)twShow,          "twShow");
    OspRegCommand("twCfgShow",      (void*)twCfgShow,       "twCfgShow");
    OspRegCommand("twsetlog",       (void*)twsetlog,        "twsetlog");
    OspRegCommand("twtau",          (void*)twtau,           "tvwall telnet author");
#endif
	//lint -restore
}

// 设置屏幕打印机别
//API void twscreen()
//{
//
//	::OspSetScrnLogLevel(0, LOGLVL_DEBUG2);
//	::OspSetScrnLogLevel(AID_TVWALL, LOGLVL_DEBUG2);
//}

//API void twSetReleaseScreen()
//{
//	::OspSetScrnLogLevel(0,  LOGLVL_IMPORTANT);
//	::OspSetScrnLogLevel(AID_TVWALL,  LOGLVL_IMPORTANT);
//}


///////////////////////////////////////////////////////////////////////////////////
//
// call CKdvMediaRcv to load it's lib
//
//#include "kdvmedianet.h"
//static void TvWallLoadLib()
//{
//    CKdvMediaRcv HereInstallMediaNetLib;
//}
