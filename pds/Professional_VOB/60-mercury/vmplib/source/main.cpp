/*****************************************************************************
   模块名      : 画面复合器配置
   文件名      : main.cpp
   创建时间    : 2003年 12月 4日
   实现功能    : 
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#include "osp.h"
#include "vmpinst.h"
//#include "codeccommon.h"
//#include "mcustruct.h"
//#include "evmcueqp.h"
#include "eqpcfg.h"
#include "mcuver.h"

extern s32 g_nVmplog;
CVmpApp g_cVMPApp;
/*lint -save -e611 -e765*/
void VmpAPIEnableInLinux(void);

BOOL vmpinit(TVmpCfg * ptVmpCfg)
{
    VmpAPIEnableInLinux();
    
    if( 0 == ptVmpCfg->dwConnectIP && 0 == ptVmpCfg->dwConnectIpB )
    {
        OspPrintf(TRUE, FALSE, "\n[Vmp]The param error\n");
        return FALSE;
    }

    if( ptVmpCfg->dwConnectIP == ptVmpCfg->dwConnectIpB )
    {
        ptVmpCfg->dwConnectIpB  = 0;
        ptVmpCfg->wConnectPortB = 0;
    }

    g_cVMPApp.CreateApp("VMP", AID_VMP, APPPRI_VMP, 512, 1<<20);

    OspPrintf(TRUE, FALSE, "\nEnter vmpinit\n");

    /*#undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)
#ifdef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
//    #include "evmcueqp.h"
    #define _EV_MCUEQP_H_
#else
    #include "evmcueqp.h"
    #undef _EV_MCUEQP_H_
#endif*/

    ::OspPost(MAKEIID(AID_VMP, 1), EV_VMP_INIT, ptVmpCfg, sizeof(TVmpCfg));

    return TRUE;
}

API void vmpstatus()
{
    CServMsg cServMsg;
    cServMsg.SetMsgBody();
    OspPost(MAKEIID(AID_VMP, 1), EV_VMP_DISPLAYALL, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}

API void pvmpmsg(void)
{
    g_nVmplog = 1;
}

API void npvmpmsg(void)
{
    g_nVmplog = 0;
}

API void vmpver(void)
{
    s8 gs_VersionBuf[128] = {0};
    {
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
    }
    OspPrintf( TRUE, FALSE, "Vmp Version: %s\n", gs_VersionBuf );
    OspPrintf(TRUE, FALSE, "vmp version:%s\t",VER_VMP);
    OspPrintf(TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__);
}

API void vmptau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

API void vmphelp()
{
    OspPrintf(TRUE, FALSE, "vmp version:%s\t",VER_VMP);
    OspPrintf(TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__);
    OspPrintf(TRUE, FALSE, "vmpstatus  ---- Display vmp status\n");
    OspPrintf(TRUE, FALSE, "pvmpmsg    ---- Display the debug information.\n");
    OspPrintf(TRUE, FALSE, "npvmpmsg   ---- Not display the debug information.\n");
    OspPrintf(TRUE, FALSE, "vmptau(UsrName, Pwd) ---- VMP telnet author.\n");
    OspPrintf(TRUE, FALSE, "setBBOut(BOOL32 bOut)---- Set back board out or not\n.");
	OspPrintf(TRUE, FALSE, "showvmpparam		 ---- Show vmp param\n");
    // OspPrintf(TRUE, FALSE, "vmpdiagnosis-----Run diagnose, if some error occurs please run it,information will give\n");
}

API void vmpdiagnosis()
{
    OspPrintf(TRUE, FALSE, "[Diagnos]No diagnose supported!\n");
    return;
//     OspPrintf(TRUE, FALSE, "[Diagnos]Unknown hardware version\n");
//     OspPrintf(TRUE, FALSE, "[Diagnos]Unknown logic version\n");
    /*OspPrintf(TRUE, FALSE, "[Diagnos]Current use map total number is %u\n", mapnum);
    	for (int i = 0 ; i < mapnum ; i++)
    {
	    OspPrintf(TRUE, FALSE, "[Diagnos]Now we begin to show map-%u is running or halt:\n",mapnum - i -1);
	    showmap(mapnum - i -1);
	    OspDelay(3000);
    }*/
//     OspPrintf(TRUE, FALSE, "[Diagnos]if some map is halt,it maybe you MediaCtrl is not match,please check it!\n");
//     OspPrintf(TRUE, FALSE, "[Diagnos]Now we begin to check GPDB connect between maps......\n");
    /*	for (i=0 ; i < mapnum ; i++)
    {
	    EqtMapPrtEnable(mapnum - i -1);
	    OspDelay(1000);
    }
    */
//     OspPrintf(TRUE, FALSE, "[Diagnos]if allways print queue is full or other error,it maybe you MediaCtrl is not match or hardware fault,call for help!\n");
//     OspPrintf(TRUE, FALSE, "[Diagnos]That's all we can do at present,maybe you need advanced command to diagnose,call for help!\n");
}

// 打印业务合成参数, zgc, 2008-03-19
API void showvmpparam(void)
{
	CServMsg cServMsg;
    cServMsg.SetMsgBody();
    OspPost(MAKEIID(AID_VMP, 1), EV_VMP_SHOWPARAM, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}
// 测试不重启停止VMP活动, zgc, 2007/04/24
API void stopvmpeqp(void)
{
	OspPost( MAKEIID(AID_VMP, 1), MCU_VMP_STOPMAP_CMD );
}

/*=============================================================================
 函 数 名： setBBOut
 功    能： 是否使用备板输出
 算法实现： 
 全局变量： 
 参    数： void
 返 回 值： void 
 ----------------------------------------------------------------------
 修改记录    ：
 日  期		版本		修改人		走读人    修改内容
 2006/8/15  4.0			许世林                  创建
=============================================================================*/
API void setBBOut( u8 byOut )
{
    if ( byOut > 1 )
    {
        byOut = 1;
    }
    CServMsg cServMsg;
    cServMsg.SetMsgBody( (u8*)&byOut, sizeof(u8) );
    OspPost( MAKEIID(AID_VMP, 1), EV_VMP_BACKBOARD_OUT_CMD, 
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
}

/*=============================================================================
函 数 名： VmpAPIEnableInLinux
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

void VmpAPIEnableInLinux(void)
{
#ifdef _LINUX_      
    OspRegCommand("vmpstatus",          (void*)vmpstatus,           "vmpstatus");
	OspRegCommand("showvmpparam",		(void*)showvmpparam,		"showvmpparam");
    OspRegCommand("pvmpmsg",            (void*)pvmpmsg,             "pvmpmsg");
    OspRegCommand("npvmpmsg",           (void*)npvmpmsg,            "npvmpmsg");
    OspRegCommand("vmpver",             (void*)vmpver,              "vmpver");
    OspRegCommand("vmphelp",            (void*)vmphelp,             "vmphelp");
    OspRegCommand("vmpdiagnosis",       (void*)vmpdiagnosis,        "vmpdiagnosis");
    OspRegCommand("vmptau",             (void*)vmptau,              "vmp telnet author");
    OspRegCommand("setBBOut",           (void*)setBBOut,            "set back board out or not");
	// 测试不重启停止VMP活动, zgc, 2007/04/24
//	OspRegCommand("stopvmpeqp",         (void*)stopvmpeqp,          "stop vmp map");
#endif
}
/*lint -restore*/

//API void vmpsetlogfile()
//{
//    ::OspSetFileLogLevel( 0, LOGLVL_IMPORTANT );
//    ::OspSetFileLogLevel( AID_VMP, LOGLVL_IMPORTANT );
//}
