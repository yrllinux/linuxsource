/*****************************************************************************
   模块名      : MpwLib多画面复合电视墙
   文件名      : mpwapp.cpp
   相关文件    : 
   文件实现功能: 实现mpw的启动，调试接口
   作者        : john
   版本        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/09/25  1.0         john         创建
******************************************************************************/
#include "mpwcfg.h"
#include "mpwinst.h"
#include "evmpw.h"
#include "mcuver.h"

extern CMpwApp g_cMpwApp;

extern s32 g_nMpwlog;

void MpwAPIEnableInLinux(void);

/*=============================================================================
  函 数 名： MpwInit
  功    能： 启动Mpw
  算法实现： 
  全局变量： 
  参    数： TVmpCfg& tVmpCfg
  返 回 值： BOOL32：TRUE：成功，FALSE：失败 
=============================================================================*/
BOOL32 MpwInit(TVmpCfg& tVmpCfg)
{
    MpwAPIEnableInLinux();
    
    if(0 == tVmpCfg.dwConnectIP && 0 == tVmpCfg.dwConnectIpB)
    {
        OspPrintf(TRUE, FALSE, "[Mpw] The mcu's ip are all 0\n");
        return FALSE;
    }

    if(tVmpCfg.dwConnectIP == tVmpCfg.dwConnectIpB)
    {
        tVmpCfg.dwConnectIpB = 0;
        tVmpCfg.wConnectPortB = 0;
    }

    g_cMpwApp.CreateApp( "MpwInst", AID_VMPTW, APPPRI_MPW );
    OspPrintf( TRUE, FALSE, "[Mpw] Initalizing the Mpw !\n" );

    #undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)
#ifdef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
    #include "evmcueqp.h"
    #define _EV_MCUEQP_H_
#else
    #include "evmcueqp.h"
    #undef _EV_MCUEQP_H_
#endif
    
    OspPost( MAKEIID(AID_VMPTW, 1), EV_VMPTW_INIT, &tVmpCfg, sizeof(tVmpCfg) );
    return TRUE;
}

/*=============================================================================
  函 数 名： mpwvershow
  功    能： 显示版本
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
=============================================================================*/
API void mpwvershow(void)
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
        sprintf(gs_VersionBuf, ":%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
	OspPrintf( TRUE, FALSE, "MPW Version: %s\n", gs_VersionBuf);
    OspPrintf( TRUE, FALSE, "[Help] Mpw version: %s\n", VER_MPW );
    OspPrintf( TRUE, FALSE, "[Help] Mpw compile time: %s %s\n", __DATE__, __TIME__ );
    return;
}

/*=============================================================================
  函 数 名： mpwhelpshow
  功    能： 帮助
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
=============================================================================*/
API void mpwhelp(void)
{
    OspPrintf( TRUE, FALSE, "[Help] mpwshowstatus --  show mpw status\n" );
    OspPrintf( TRUE, FALSE, "[Help] mpwvershow    --  show mpw version\n" );
    OspPrintf( TRUE, FALSE, "[Help] pmpwmsg       --  print mpw message\n" );
    OspPrintf( TRUE, FALSE, "[Help] npmpwmsg      --  not print mpw message\n" );
    OspPrintf( TRUE, FALSE, "[Help] mpwtau(UsrName, Pwd) -- MPW telnet author\n" );
    return;
}

/*=============================================================================
  函 数 名： mpwshowstatus
  功    能： 显示mpw状态
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
=============================================================================*/
API void mpwshowstatus(void)
{
    OspPost( MAKEIID(AID_VMPTW, 1), EV_VMPTW_DISPLAYALL, NULL, 0 );
    return;
}

/*=============================================================================
  函 数 名： mpwtau
  功    能： MPW 的telnet授权
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
=============================================================================*/
API void mpwtau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

/*=============================================================================
  函 数 名： mpwtest
  功    能： 打印调试信息
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
=============================================================================*/
API void pmpwmsg(void)
{
    g_nMpwlog = 1;
    return;
}

/*=============================================================================
  函 数 名： npmpwmsg
  功    能： 关闭调试打印
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
=============================================================================*/
API void npmpwmsg(void)
{
    g_nMpwlog = 0;
    return;
}


/*=============================================================================
函 数 名： MpwAPIEnableInLinux
功    能： 注册linux调试接口
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/15  4.0			许世林                  创建
=============================================================================*/
void MpwAPIEnableInLinux(void)
{
#ifdef _LINUX_    
    OspRegCommand("mpwvershow",        (void*)mpwvershow,       "mpwvershow");
    OspRegCommand("mpwhelp",           (void*)mpwhelp,          "mpwhelpshow");
    OspRegCommand("mpwshowstatus",     (void*)mpwshowstatus,    "mpwshowstatus");
    OspRegCommand("pmpwmsg",           (void*)pmpwmsg,          "pmpwmsg");
    OspRegCommand("npmpwmsg",          (void*)npmpwmsg,         "npmpwmsg");
    OspRegCommand("mpwtau",            (void*)mpwtau,           "mpw telnet author");
#endif
}
