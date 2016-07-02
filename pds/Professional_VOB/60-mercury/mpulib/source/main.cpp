/*****************************************************************************
   模块名      : mpulib
   文件名      : main.cpp
   相关文件    : 
   文件实现功能: mpulib主程序入口
   作者        : 周文
   版本        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      创建
   2009/7/30	4.6         xl			support autotest and add API
******************************************************************************/
#include "osp.h"
#include "mpuinst.h"
#include "eqplib.h"
#include "evmcueqp.h"
#include "mcuver.h"
#include "evmpu.h"

#include "boardagentbasic.h"
#include "mpuvmphwtest_evid.h"
#include "mpuautotest.h"
#include "mpuvmphwtest.h"

// #ifdef _LINUX_
// #include "nipwrapper.h"
// #endif

CMpuSVmpApp g_cMpuSVmpApp;
//CMpuEVpuApp g_cMpuEVpuApp;
//CMpuDVmpApp g_cMpuDVmpApp;
//CMpuEBapApp g_cMpuEBapApp;
CMpuBasApp  g_cMpuBasApp;

TResBrLimitTable g_tResBrLimitTable;

//extern BOOL32 g_bPauseSend;
BOOL32 g_bPauseSend = FALSE;

extern BOOL32 g_bPauseSS;

void APIEnableInLinux( void );
/*lint -save -e526*/
extern void RegsterCommands();


/*lint -save -e765*/

BOOL MpuInit(TMpuCfg* ptCfg, u8 byWorkMode/*, u8 byEqpIdB*/)
{
    APIEnableInLinux();
    RegsterCommands();
    
	//auto test route:
	if(ptCfg->IsAutoTest())
	{
		printf("[MpuInit]Begin Autotest!!!!\n");
		if( TYPE_MPUSVMP == byWorkMode )
		{
			printf("[MpuInit]Begin vmpTest!!!\n");
			if( OSP_OK == g_cMpuVmpTestApp.CreateApp( "MPUVmpTest", AID_MPU, APPPRI_VMP, 512, 1<<20 ) )
			{
				OspPost( MAKEIID(AID_MPU, 1), EV_C_INIT, ptCfg, sizeof(TMpuCfg) );
			}
			
		}
		else if( TYPE_MPUBAS == byWorkMode)
		{
			printf("[MpuInit]Begin BasTest!!!\n");
			if( OSP_OK == g_cMpuBasTestApp.CreateApp( "MPUBasTest", AID_MPU, APPPRI_BAS, 512, 1<<20 ) )
			{
				g_cMpuBasApp.m_byWorkMode = byWorkMode;
				g_cMpuBasApp.m_wMcuRcvStartPort = MPUTEST_BAS_SENDSVRPORT;
				OspPost(MAKEIID(AID_MPU, CInstance::DAEMON), EV_C_INIT, ptCfg, sizeof(TMpuCfg) );
			}
			
		}
		else
		{
			printf("wrong mode while testing!\n");
		}
		
	}
	else //normal route
	{
		if( 0 == ptCfg->dwConnectIP )
		{
			mpulog( MPU_CRIT, "[MpuInit]0 == ptCfg->dwConnectIP\n" );
			return FALSE;
		}
		//读分辨率码率限制信息表
		BOOL32 bReadTalbe = g_tResBrLimitTable.ReadResBrLimitTable( BRD_TYPE_MPU );
		if (!bReadTalbe)
		{
			printf("g_tResBrLimitTable.ReadResBrLimitTable() error,no configfile or defaulttable\n");
			mpulog( MPU_CRIT,"g_tResBrLimitTable.ReadResBrLimitTable() error,no configfile or defaulttable\n");
		}

		switch ( byWorkMode )
		{
		case TYPE_MPUSVMP:
			if( OSP_OK == g_cMpuSVmpApp.CreateApp( "MPU", AID_MPU, APPPRI_VMP, 512, 1<<20 ) )
			{
				g_cMpuSVmpApp.m_byWorkMode = byWorkMode;
				OspPost( MAKEIID(AID_MPU, 1), EV_VMP_INIT, ptCfg, sizeof(TMpuCfg) );
			}
			
			break;
		case TYPE_MPUBAS:
		case TYPE_MPUBAS_H:
			if( OSP_OK == g_cMpuBasApp.CreateApp( "MPU", AID_MPU, APPPRI_BAS, 512, 1<<20 ) )
			{
				g_cMpuBasApp.m_byWorkMode = byWorkMode;
				OspPost( MAKEIID(AID_MPU, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(TMpuCfg) );
			}
			break;
// 		case TYPE_MPUDVMP:
// 			g_cMpuDVmpApp.CreateApp( "MPU", AID_MPU, APPPRI_VMP, 512, 1<<20 );
// 			g_cMpuDVmpApp.m_byWorkMode = byWorkMode;
// 			OspPost( MAKEIID(AID_MPU, 1), EV_VMP_INIT, ptCfg, sizeof(TVmpCfg) );
// 			
// 			ptCfg->byEqpId = byEqpIdB;
// 			ptCfg->wRcvStartPort = ptCfg->wRcvStartPortB;
// 			OspPost( MAKEIID(AID_MPU, 2), EV_VMP_INIT, ptCfg, sizeof(TVmpCfg) );
// 			break;
// 		case TYPE_EVPU:
// 			g_cMpuEVpuApp.CreateApp( "MPU", AID_MPU, APPPRI_VMP, 512, 1<<20 );
// 			g_cMpuEVpuApp.m_byWorkMode = byWorkMode;
// 			OspPost( MAKEIID(AID_MPU, 1), EV_VMP_INIT, ptCfg, sizeof(TVmpCfg) );
// 			break;
		
// 		case TYPE_EBAP:
// 			g_cMpuEBapApp.CreateApp( "MPU", AID_MPU, APPPRI_BAS, 512, 1<<20 );
// 			g_cMpuEBapApp.m_byWorkMode = byWorkMode;
// 			OspPost( MAKEIID(AID_MPU, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(TVmpCfg) );
// 			break;
			
		default:
			break;
		}
	}

// 加载事件声明
//      #undef OSPEVENT
//      #define OSPEVENT(x, y) OspAddEventDesc(#x, y)

// #ifdef _EV_MCUEQP_H_
// 	#undef _EV_MCUEQP_H_
// 	    #include "evmcueqp.h"
// 	#define _EV_MCUEQP_H_
// #else
// 	    #include "evmcueqp.h"
//     #undef  _EV_MCUEQP_H_
// #endif

    
// #ifdef _EVENT_MPUBAS_H_
//     #undef _EVENT_MPUBAS_H_
//         #include "evmpu.h"
//     #define _EVENT_MPUBAS_H_
// #else
//         #include "evmpu.h"
//     #undef  _EVENT_MPUBAS_H_
// #endif


#ifdef _LINUX_
    s32 nRet = BrdSetSysRunSuccess();
    if ( nRet == UPDATE_VERSION_ROLLBACK )
    {
        mpulog( MPU_CRIT, " Update failed, rollback version!\n" );
    }
    else if ( nRet == SET_UPDATE_FLAG_FAILED )
    {
        mpulog( MPU_CRIT, " BrdSetSysRunSuccess() failed!\n" );
    }
    OspPrintf(TRUE, FALSE, "BrdSetSysRunSuccess, ret.%d\n", nRet );
#endif
    
    return TRUE;
}

API void mpuver()
{
    s8 gs_VersionBuf[128] = {0};
    {
        strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
        
        s8 achMon[16] = {0};
        unsigned int byDay = 0;
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
    OspPrintf( TRUE, FALSE, "mpu Version: %s\n", gs_VersionBuf );
    OspPrintf( TRUE, FALSE, "\tmpu version : %s, compile time:%s:%s\n", VER_MPU, __DATE__, __TIME__ );
}

API void mpuhelp()
{
    OspPrintf( TRUE, FALSE, "mpu version:%s\t", VER_MPU );
    OspPrintf( TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__ );
    OspPrintf( TRUE, FALSE, "mpustatus    ---- Display mpu status\n" );
	OspPrintf( TRUE, FALSE, "showbasdebug ---- Show mpu debug\n" );
	OspPrintf( TRUE, FALSE, "showvmpdebug ---- Show mpu debug\n" );
    OspPrintf( TRUE, FALSE, "showmpuparam ---- Show mpu param\n" );
    OspPrintf( TRUE, FALSE, "setmpulog    ---- setmpulog\n" );
	OspPrintf( TRUE, FALSE, "setautotest  ---- 1: test mode\t 0: normal mode\n");
	OspPrintf( TRUE, FALSE, "testvmp      ---- test vmp mode\n");
	OspPrintf( TRUE, FALSE, "testbas      ---- test bas mode\n");
	OspPrintf( TRUE, FALSE, "setvmpfr     ---- Set VMP Frame Rate\n");
	OspPrintf( TRUE, FALSE, "resumess     ---- Set VMP Smooth Send Starting\n");
	OspPrintf( TRUE, FALSE, "pausess      ---- Set VMP Smooth Send Pausing\n");
	OspPrintf( TRUE, FALSE, "showsmoothstatus   ----Show VMP SmoothSend's Flag\n");
}

API void mpustatus()
{
    CServMsg cServMsg;
    cServMsg.SetMsgBody();
    OspPost( MAKEIID(AID_MPU, 1), EV_VMP_DISPLAYALL, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
}

API void showbasdebug()
{
    OspPost( MAKEIID(AID_MPU, CInstance::DAEMON), EV_BAS_SHOWDEBUG );	
}

API void showvmpdebug()
{
    OspPost( MAKEIID(AID_MPU, 1), EV_VMP_SHOWDEBUG );
}

API void setmpulog( u8 byLevel = MPU_CRIT )
{
    if (byLevel > MPU_DETAIL)
    {
        byLevel = MPU_DETAIL;
    }
    g_nmpubaslog = byLevel;
    return;
}

API void showmpuparam(void)
{
	CServMsg cServMsg;
    cServMsg.SetMsgBody();
    OspPost(MAKEIID(AID_MPU, 1), EV_VMP_SHOWPARAM, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}

API void pausesend(void)
{
    g_bPauseSend = TRUE;
}

API void resumesend(void)
{
    g_bPauseSend = FALSE;
}

API void pausess(void)
{
    g_bPauseSS = TRUE;
}

API void resumess(void)
{
    g_bPauseSS = FALSE;
}

API void showsmoothstatus(void)
{
	if (TRUE == g_bPauseSS)
	{
		 OspPrintf(TRUE,FALSE,"VMP SmoothSend's Flag is Pausing!\n");
	}
	else
	{
	 	OspPrintf(TRUE,FALSE,"VMP SmoothSend's Flag is Starting!\n");
	}
}


API void basshow()
{
    ::OspPost(MAKEIID( AID_MPU, CInstance::EACH ), EV_BAS_SHOW);
}

API void setvmpfr(u8 byFrameRate)
{
	::OspPost( MAKEIID(AID_MPU, 1), EV_VMP_SETFRAMERATE_CMD, &byFrameRate, sizeof(byFrameRate) );
}

/*lint -save -e715*/
//设置是否硬件自动检测
API void setautotest( s32 nIsAutoTest )  
{
#ifdef _LINUX_
	if( nIsAutoTest != 0)
	{
		BrdSetE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 1\n" );
	}
	else 
	{
		BrdClearE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 0\n" );
	}
#endif
	
	//::OspPost( MAKEIID( AID_MPU, CMpuBasTestClient::DAEMON ), EV_C_CHANGEAUTOTEST_CMD, &nIsAutoTest, sizeof(s32) );
    return;
}
/*lint -restore*/

API void testvmp(void)
{
	//转vmp模式测试
	s8   achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	BOOL32 bRet;
    s32  sdwTest = TYPE_MPUSVMP;	
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );

    if( !bRet )  
	{
		printf( "[RestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
		return;
	}

	OspPrintf(TRUE, FALSE, "Change to VMP Test Mode! Now you can reboot!\n");
	//reset
// #ifndef WIN32
// 	OspDelay(2000);
// 	BrdHwReset();
// #endif
    return;
}


API void testbas(void)
{
	//转bas模式测试
	s8   achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	BOOL32 bRet;
    s32  sdwTest = TYPE_MPUBAS;	
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );
    if( !bRet )  
	{
		printf( "[RestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
		return;
	}
	
	OspPrintf(TRUE, FALSE, "Change to VMP Test Mode! Now you can reboot!\n");
}

API void deldebugini(void)
{
#ifdef _LINUX_
	s8    achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	if( ERROR == unlink(achProfileName) )
	{
		OspPrintf(TRUE, FALSE, "delete brdcfgdebug.ini failed!\n");
	}
	else
	{	
		OspPrintf(TRUE, FALSE, "brdcfgdebug.ini has been deleted!\n");
	}
#endif

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
/*lint -save -e611*/
void APIEnableInLinux( void )
{
#ifdef _LINUX_     
    // 待续
    OspRegCommand("mpustatus",          (void*)mpustatus,           "mpustatus");
    OspRegCommand("setmpulog",          (void*)setmpulog,           "setmpulog");
 	OspRegCommand("showmpuparam",		(void*)showmpuparam,		"showmpuparam");
    OspRegCommand("mpuver",             (void*)mpuver,              "vmpver");
    OspRegCommand("mpuhelp",            (void*)mpuhelp,             "mpuhelp");
	OspRegCommand("showbasdebug",		(void*)showbasdebug,		"showbasdebug");
	OspRegCommand("showvmpdebug",		(void*)showvmpdebug,		"showvmpdebug");
    OspRegCommand("pausesend",          (void*)pausesend,           "pausesend");
    OspRegCommand("resumesend",         (void*)resumesend,          "resumesend");
    OspRegCommand("pausess",            (void*)pausess,             "pausess");
    OspRegCommand("resumess",           (void*)resumess,            "resumess");
	OspRegCommand("showsmoothstatus",   (void*)showsmoothstatus,    "showsmoothstatus");
    OspRegCommand("basshow",            (void*)basshow,             "basshow");
	OspRegCommand("setautotest",		(void*)setautotest,			"setautotest");
	OspRegCommand("testvmp",			(void*)testvmp,				"testvmp");
	OspRegCommand("testbas",			(void*)testbas,				"testbas");
	OspRegCommand("deldebugini",		(void*)deldebugini,			"deldebugini");
	OspRegCommand("setvmpfr",			(void*)setvmpfr,			"setvmpfr");
#endif
}
/*lint -restore*/
//END OF FILE


