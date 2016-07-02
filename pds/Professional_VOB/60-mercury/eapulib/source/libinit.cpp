#include "eapuinst.h"
#include "eaputestinst.h"
//#include "kdvsys.h"
//#include "mcustruct.h"
//#include "mcuver.h"
//#include "eqpcfg.h"
s32  g_nPrintElog = MIXLOG_LEVER_CRIT;
CEapuMixerApp g_cEapuMixerApp;
CEapuMixerTestApp g_cEapuMixerTestApp;

u32 g_dwSaveChId = 0;
u32 g_dwSaveNum = 0;

API void emixhelp()
{
    ::OspPrintf(TRUE, FALSE, "\n\t\t KDV EMixer \n");
    ::OspPrintf(TRUE, FALSE, "Audio Mixer Compile Time     : %s, %s\n", __TIME__, __DATE__);
    ::OspPrintf(TRUE, FALSE, "Audio Mixer Verion       : %s\n", VER_EMIXER);
    ::OspPrintf(TRUE, FALSE, "emixsetlog : set print level.\n");
	::OspPrintf(TRUE, FALSE, "emixstatus : emixer's status.\n");
	::OspPrintf(TRUE, FALSE, "emixerver : emixer's version.\n");
    ::OspPrintf(TRUE, FALSE, "emixtau(UsrName, Pwd): emixer telnet author \n");
	::OspPrintf(TRUE, FALSE, "emixsettest (0: not producttest, !0: producttest): set or not producttest\n");
}

API void emixsave(u32 dwChid, u32 dwNum)
{
    g_dwSaveChId = dwChid;
	g_dwSaveNum = dwNum;
}


API void emixsetlog(int level)
{
    g_nPrintElog = level;
}

API void emixstatus()
{
    ::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWGROUP);
}

API void emixchnnlstatus()
{
    ::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_SHOWMT2CHNNL);
}

API void emixerver()
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
    OspPrintf( TRUE, FALSE, "EMixer Version: %s\n", gs_VersionBuf );    
	OspPrintf( TRUE, FALSE, "EMixer: %s  compile time: %s    %s\n", VER_EMIXER, __DATE__, __TIME__);
}

API void emixtau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

API void estopmixeqp(void)
{
	OspPost( MAKEIID(AID_MIXER, CInstance::DAEMON),  MCU_MIXER_STOPEQP_CMD );
}
API void estartmixeqp(void)
{
	OspPost( MAKEIID(AID_MIXER, CInstance::DAEMON),  MCU_MIXER_STARTEQP_CMD );
}
API void emixsettest(int level)
{
#ifndef WIN32
	STATUS wRet = 0;
	if (level == 0)
	{
		wRet = BrdClearE2promTestFlag();
		if ( wRet != 0 )
	    {
	        OspPrintf( TRUE, FALSE, "[emixsettest] BrdClearE2promTestFlag failed!\n" );
	    }
		else
		{
			OspPrintf( TRUE, FALSE, "[emixsettest] BrdClearE2promTestFlag sucessful!\n" );
		}
	}
	else
	{
		wRet = BrdSetE2promTestFlag();
		if ( wRet != 0 )
	    {
	        OspPrintf( TRUE, FALSE, "[emixsettest] BrdSetE2promTestFlag failed!\n" );
	    }
		else
		{
			OspPrintf( TRUE, FALSE, "[emixsettest] BrdSetE2promTestFlag sucessful!\n" );
		}
	}
#endif
	OspPrintf( TRUE, FALSE, "[emixsettest] BrdClearE2promTestFlag(%d) sucessful!\n",level );
}
#ifdef _LINUX_ 
static void EMixAPIEnableInLinux(void)
{
	/*lint -save -e611*/
    OspRegCommand("emixhelp",        (void*)emixhelp,         "emix help");
    OspRegCommand("emixsetlog",      (void*)emixsetlog,       "emix setlog");
    OspRegCommand("emixstatus",      (void*)emixstatus,       "emixstatus");
    OspRegCommand("emixerver",       (void*)emixerver,        "emixer ver");
    OspRegCommand("emixtau",         (void*)emixtau,          "emixer telnet author");
    OspRegCommand("emixsave",        (void*)emixsave,         "emixer save");
    OspRegCommand("emixsettest",     (void*)emixsettest,      "emixer settest");
	/*lint -restore*/
}
#endif
API void emixstart()
{
    ::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), MCU_MIXER_STARTMIX_REQ);
}
BOOL eapuInit(TEapuCfg * ptMixerCfg)
{    
#ifdef _LINUX_ 
	EMixAPIEnableInLinux();
	RegsterCommands();
#endif
	if (NULL == ptMixerCfg)
    {
		OspPrintf( TRUE, FALSE, "[eapuInit]NULL == ptMixerCfg\n");
        return FALSE;
    }

	if ( ptMixerCfg->IsProductTest() )
	{
		OspPrintf(TRUE,FALSE,"[eapuInit]Begin ProductTest!!!!\n");
		g_cEapuMixerTestApp.CreateApp( "EAPUTEST", EAPU_SERVER_APPID, APPPRI_MIXER, 512, 1<<20 );	// 创建Mixer应用
		::OspSetPrompt("EAPUAUTOTEST");
		::OspPost(MAKEIID(EAPU_SERVER_APPID, 1), EV_C_INIT); 
		return TRUE;
	}

	OspPrintf(TRUE,FALSE,"[eapuInit]Begin NormalTask!!!!\n");
	g_cEapuMixerApp.CreateApp( "EAPU", AID_MIXER, APPPRI_MIXER, 512, 1<<20 );	// 创建Mixer应用
	::OspSetPrompt("EAPU");
	::OspPost(MAKEIID(AID_MIXER, CInstance::DAEMON), EV_MIXER_INIT, ptMixerCfg, sizeof(TEapuCfg));
    
	/*lint -save -e537 -e760*/
    #undef OSPEVENT
    #define OSPEVENT(x, y) OspAddEventDesc(#x, y)
#ifdef _EVENT_MCU_MIXER_H_
    #undef _EVENT_MCU_MIXER_H_
    #include "eveapumixer.h"
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
    
    return TRUE;
}
