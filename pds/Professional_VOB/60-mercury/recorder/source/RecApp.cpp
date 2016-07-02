/*****************************************************************************
   模块名      : 录像机应用
   文件名      : RecApp.cpp
   相关文件    : 
   文件实现功能: 创建录像机应用
   作者        : 
   版本        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#include "ChnInst.h"
//#include "ConfAgent.h"
//#include "evrec.h"
#include "mcuver.h"
#include "RecApp.h"
//#include "KdvMediaNet.h"

#ifdef _RECSERV_
#include "RecServ_Def.h"
#include "RecServInst.h"
extern CRecServApp g_RecServApp;
#endif


extern CRecApp g_cRecApp;


/*====================================================================
    函数名	     ：UserInit
	功能		 ：录像机应用实例入口函数
	引用全局变量 ：无
    输入参数说明 ：ptConf - TCfgAgent指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
API void UserInit()
{
	#undef OSPEVENT
	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
	
#ifdef _EV_MCUEQP_H_
	#undef _EV_MCUEQP_H_
	#include "evmcueqp.h"
	#define _EV_MCUEQP_H_
#else
	#include "evmcueqp.h"
    #undef  _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
#endif

#ifdef EVREC_H
	#undef EVREC_H
	#include "evrec.h"
	#define EVREC_H
#else
	#include "evrec.h"
	#undef EVREC_H
#endif

#ifdef _DEBUG
    ::OspInit(TRUE, BRD_TELNET_PORT);
#else
    ::OspInit(FALSE, BRD_TELNET_PORT);
    
    //Osp telnet 初始授权 [11/28/2006-zbq]
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

#endif

   	// 初始化KdvLog模块
	Err_t err =LogInit(g_tCfg.m_szCfgFilename);
	if(LOG_ERR_NO_ERR != err)
	{
		printf("KdvLog Init failed, err is %d\n", err);
	}

	g_cRecApp.CreateApp( "Recorder", AID_RECORDER, APPPRI_REC );
    
#ifdef _RECSERV_
    if (OSP_OK != g_RecServApp.CreateApp( "RecServer", AID_REC_SERVER, APPPRI_REC ))
    {
        return;
    }    
#endif // end of _RECSERV_
	::OspSetPrompt("Rec");

	::recSetReleaseScreen();

    ::OspPost( MAKEIID(AID_RECORDER, CInstance::DAEMON), EV_REC_POWERON );
    ::OspPost( MAKEIID(AID_REC_SERVER, CInstance::DAEMON), EV_REC_POWERON );
    
    return;
}

/*=============================================================================
  函 数 名： RestartRecorder
  功    能： 重启录像机
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void RestartRecorder(HWND hMainDlg)
{
    ::OspPost( MAKEIID(AID_RECORDER, CInstance::DAEMON), EV_REC_RESTART );

    PostMessage( hMainDlg, WM_REC_RESTART, NULL, NULL );
}

/*====================================================================
    函数名	     ：recver
	功能		 ：打印recorder模块以及相关库的版本信息
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
API void recver()
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
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n" );
	::OspPrintf( TRUE, FALSE, "Recorder Compile Time : %s, %s\n", __TIME__, __DATE__ );
	::OspPrintf( TRUE, FALSE, "Recorder Verion       : %s\n", VER_RECORDER );
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n\n" );

	kdvmedianetver();
	rpver();
	ospver();
}

/*====================================================================
    函数名	     ：rechelp
	功能		 ：打印recorder模块帮助信息
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
API void rechelp()
{
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n" );
	::OspPrintf( TRUE, FALSE, "Recorder Compile Time : %s, %s\n", __TIME__, __DATE__ );
	::OspPrintf( TRUE, FALSE, "Recorder Verion       : %s\n", VER_RECORDER );
    ::OspPrintf( TRUE, FALSE, "=======================================================================\n" );

	::OspPrintf( TRUE, FALSE, "precmsg               : Print Record Message .\n");	
	::OspPrintf( TRUE, FALSE, "recStatusShow         : Show Recorder Status\n");
	::OspPrintf( TRUE, FALSE, "showdebuginfo         : Show Recorder debug info\n");

	::OspPrintf( TRUE, FALSE, "kdvmedianethelp       : \n");
	::OspPrintf( TRUE, FALSE, "rphelp                : \n");
    ::OspPrintf( TRUE, FALSE, "rectau(UsrName, Pwd)  : recorder telnet author.\n");

    ::OspPrintf( TRUE, FALSE, "recservhelp           : Show Helps about recorder server\n");
}

/*====================================================================
    函数名	     ：recSetDebugScreen
	功能		 ：设置屏幕打印级别
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
API void recscreen()
{
	::OspSetScrnLogLevel( 0, LOGLVL_DEBUG2 );
	::OspSetScrnLogLevel( AID_RECORDER, LOGLVL_DEBUG2 );
}

/*====================================================================
    函数名	     ：recSetReleaseScreen
	功能		 ：设置屏幕打印级别
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
API void recSetReleaseScreen()
{
	::OspSetScrnLogLevel( 0,  LOGLVL_IMPORTANT );
	::OspSetScrnLogLevel( AID_RECORDER,  LOGLVL_IMPORTANT );
}

/*====================================================================
    函数名	     : status show
	功能		 ：
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
API void recStatusShow()
{
	::OspPost( MAKEIID( AID_RECORDER,CInstance::DAEMON), EV_REC_STATUSSHOW);
}

/*====================================================================
    函数名	     ：recQuit
	功能		 ：recorder退出
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
API void recQuit()
{
	// [2013/1/7 zhangli]此接口暂时没有用到，不提供此接口功能。消息EV_REC_QUIT改用为录像机退出
// 	 OspSend(MAKEIID(AID_RECORDER, CInstance::DAEMON), EV_REC_QUIT, 
// 		            0, 0, 0,
// 					MAKEIID(INVALID_APP, INVALID_INS),
// 					INVALID_NODE,
// 					NULL, 0, NULL, 
// 					10000);
// 	::OspQuit();
}

/*====================================================================
    函数名	     : rectau
	功能		 ：
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    06/11/28    4.0         张宝卿          创建
====================================================================*/
API void rectau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
} 