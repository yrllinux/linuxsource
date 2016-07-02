/*****************************************************************************
   模块名      : 混音器
   文件名      : audiomixercpp
   创建时间    : 2003年 12月 9日
   实现功能    : 
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/09    3.0         zmy         创建
******************************************************************************/
#include "audmixinst.h"
#include "audmixer.h"
//#include "kdvsys.h"
//#include "mcustruct.h"
#include "mcuver.h"
//#include "evmixer.h"
//#include "eqpcfg.h"

API void mixhelp()
{
    ::OspPrintf(TRUE, FALSE, "\n\t\t KDV Mixer \n");
    ::OspPrintf(TRUE, FALSE, "Audio Mixer Compile Time : %s, %s\n", __TIME__, __DATE__);
    ::OspPrintf(TRUE, FALSE, "Audio Mixer Verion       : %s\n\n", VER_MIXER);

    ::OspPrintf(TRUE, FALSE, "mixsetlog (1: print, 0: not print): Print LOGLVL_DEBUG2 Level Info On Screen .\n");
    ::OspPrintf(TRUE, FALSE, "mixgrpstatus        : Show Audio Mixer Status \n");
    ::OspPrintf(TRUE, FALSE, "MmpAgentSelfTest    : Show agent param \n");
    ::OspPrintf(TRUE, FALSE, "mixtau(UsrName, Pwd): mixer telnet author \n");
    ::OspPrintf(TRUE, FALSE, "mixchnnlstatus        : mixer mt to channel status \n");
}

// 设置屏幕打印机别
extern s32 g_nPrintlog;

API void mixsetlog(int level)
{
    g_nPrintlog = level;
}

API void mixgrpstatus()
{
    ::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWGROUP);
}

API void mixchnnlstatus()
{
    ::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWMT2CHNNL);
}

API void mixerver()
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
    OspPrintf( TRUE, FALSE, "Mixer Version: %s\n", gs_VersionBuf );    
	OspPrintf( TRUE, FALSE, "Mixer: %s  compile time: %s    %s\n", VER_MIXER, __DATE__, __TIME__);
}

API void mixtau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

//test, zgc, 2007/04/24
API void stopmixeqp(void)
{
	OspPost( MAKEIID(AID_MIXER, CInstance::DAEMON),  MCU_MIXER_STOPEQP_CMD );
}
API void startmixeqp(void)
{
	OspPost( MAKEIID(AID_MIXER, CInstance::DAEMON),  MCU_MIXER_STARTEQP_CMD );
}
/*=============================================================================
函 数 名： MixAPIEnableInLinux
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
void MixAPIEnableInLinux(void)
{
	/*lint -save -e611*/
#ifdef _LINUX_  
    OspRegCommand("mixhelp",        (void*)mixhelp,         "mix help");
    OspRegCommand("mixsetlog",      (void*)mixsetlog,       "mix setlog");
    OspRegCommand("mixgrpstatus",   (void*)mixgrpstatus,    "mix grp status");
    OspRegCommand("mixerver",       (void*)mixerver,        "mixer ver");
    OspRegCommand("mixtau",         (void*)mixtau,          "mixer telnet author");
    OspRegCommand("mixchnnlstatus", (void*)mixchnnlstatus,  "mix channel status");
	//test, zgc, 2007/04/24
//	OspRegCommand("stopmixeqp",		(void*)stopmixeqp,		"stop mix eqp");
//	OspRegCommand("startmixeqp",	(void*)startmixeqp,		"start mix eqp");
#endif
	/*lint -restore*/
}

/*lint -save -e765 -e714*/
BOOL mixInit(TAudioMixerCfg * ptMixerCfg)
{
	/*lint -save -e522*/
    MixAPIEnableInLinux();
	/*lint -restore*/
    
    if (NULL == ptMixerCfg || (0 == ptMixerCfg->dwConnectIP && 0 == ptMixerCfg->dwConnectIpB) )
    {
        return FALSE;
    }
    
    if( ptMixerCfg->dwConnectIP == ptMixerCfg->dwConnectIpB )
    {
        ptMixerCfg->dwConnectIpB  = 0;
        ptMixerCfg->wConnectPortB = 0;
    }

    g_cMixerApp.CreateApp( "AudMix", AID_MIXER, APPPRI_MIXER, 512, 1<<20 );	// 创建Mixer应用

    // ::OspSetPrompt("AMix");

	/*lint -save -e537 -e760*/
    #undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)
    
#ifdef _EVENT_MCU_MIXER_H_
    #undef _EVENT_MCU_MIXER_H_
    #include "evmixer.h"
    #define _EVENT_MCU_MIXER_H_
#else
    #include "evmixer.h"
    #undef _EVENT_MCU_MIXER_H_
#endif

#ifdef _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
    #include "evmcueqp.h"
    #define _EV_MCUEQP_H_
#else
    #include "evmcueqp.h"
    #undef  _EV_MCUEQP_H_
    #undef _EV_MCUEQP_H_
#endif
	/*lint -restore*/

    OspStatusMsgOutSet(FALSE);

    ::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_INIT, ptMixerCfg, sizeof(*ptMixerCfg));

    return TRUE;
}
