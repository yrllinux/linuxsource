/*****************************************************************************
   模块名      : mpu2lib
   文件名      : main.cpp
   相关文件    : 
   文件实现功能: mpulib主程序入口
   作者        : 周文
   版本        : V4.5  Copyright(C) 2008-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      创建
   2009/7/30	4.6         xl			support autotest and add API
   2011/11/30   4.7         zhouyiliang   mpu2
******************************************************************************/
#include "osp.h"
#include "mpu2inst.h"
#include "eqplib.h"
#include "evmcueqp.h"
#include "mcuver.h"
#include "evmpu2.h"

#include "boardagentbasic.h"
#include "mpuautotest.h"
#include "mpu2vmphwtest.h"

// #ifdef _LINUX_
// #include "nipwrapper.h"
// #endif

CMpu2VmpApp g_cMpu2VmpApp;
CMpu2BasApp  g_cMpu2BasApp;

TResBrLimitTable g_tResBrLimitTable;

//extern BOOL32 g_bPauseSend;
BOOL32 g_bPauseSend = FALSE;

extern BOOL32 g_bPauseSS;

void APIEnableInLinux( void );


//extern void RegsterCommands();



BOOL Mpu2Init(TMpu2Cfg* ptCfg/*, u8 byWorkMode, u8 byEqpIdB*/)
{
	if (NULL == ptCfg)
	{
		ErrorLog("[Mpu2Init]ptCfg: null pointer!!\n");
		return FALSE;
	}
    APIEnableInLinux();
//    RegsterCommands();


	//auto test route:
	if(ptCfg->GetIsAutoTest())
	{
		g_wKdvlogMid = MID_VMPEQP_COMMON;
		KeyLog("[Mpu2Init]Begin Autotest!!!!\n");
	//	printf("[MpuInit]Begin Autotest!!!!\n");
		if ( ptCfg->GetMpu2WorkMode() == TYPE_MPU2VMP_ENHANCED || 
			 ptCfg->GetMpu2WorkMode() == TYPE_MPU2VMP_BASIC 
			)
		{
			if( OSP_OK == g_cMpuVmpTestApp.CreateApp( "MPU2VmpTest", AID_MPU2, APPPRI_VMP, 512, 1<<20 ) )
			{
				g_cMpuVmpTestApp.m_byWorkMode = ptCfg->GetMpu2WorkMode();
				g_cMpuVmpTestApp.m_byVmpEqpNum = ptCfg->GetMpu2EqpNum();
				OspPost( MAKEIID(AID_MPU2, 1), EV_C_INIT, ptCfg, sizeof(TMpu2Cfg) );
			}
		}
		else //mpu2 不测bas，只测vmp
		{
			ErrorLog("[mpu2init] error workmode:%d in autotest\n",ptCfg->GetMpu2WorkMode());
			
		}

	}
	else //normal route
	{
		
		u8 byWorkMode = ptCfg->GetMpu2WorkMode();
		//读分辨率码率限制信息表
		//如果模拟mpu，读mpu表
		if ( TYPE_MPUSVMP == byWorkMode || TYPE_MPUBAS == byWorkMode || TYPE_MPUBAS_H == byWorkMode)
		{
			
			BOOL32 bReadTable =	g_tResBrLimitTable.ReadResBrLimitTable(BRD_TYPE_MPU);
			if ( !bReadTable )
			{
//				printf("g_tResBrLimitTable.ReadResBrLimitTable() error,no configfile or defaulttable\n");
				ErrorLog("g_tResBrLimitTable.ReadResBrLimitTable() error,no configfile or defaulttable\n");

			}
				
		}
		else
		{
			BOOL32 bReadTable =	g_tResBrLimitTable.ReadResBrLimitTable(BRD_TYPE_MPU2);
			if ( !bReadTable )
			{
//				printf("g_tResBrLimitTable.ReadResBrLimitTable() error,no configfile or defaulttable\n");
				ErrorLog("g_tResBrLimitTable.ReadResBrLimitTable() error,no configfile or defaulttable\n");
			}
	
		}
		
		switch ( byWorkMode )
		{
		case TYPE_MPU2VMP_BASIC:
		case TYPE_MPU2VMP_ENHANCED:
		case TYPE_MPUSVMP:
			if( OSP_OK == g_cMpu2VmpApp.CreateApp( "MPU2", AID_MPU2, APPPRI_VMP, 512, 1<<20 ) )
			{
				g_wKdvlogMid = MID_VMPEQP_COMMON;
				g_cMpu2VmpApp.m_byWorkMode = byWorkMode;
				g_cMpu2VmpApp.m_byVmpEqpNum = ptCfg->GetMpu2EqpNum();
				OspPost( MAKEIID(AID_MPU2, CInstance::DAEMON), EV_VMP_INIT, ptCfg, sizeof(TMpu2Cfg) );
			}
			
			break;
		case TYPE_MPU2BAS_BASIC:
		case TYPE_MPU2BAS_ENHANCED:
		case TYPE_MPUBAS:
		case TYPE_MPUBAS_H:
			if( OSP_OK == g_cMpu2BasApp.CreateApp( "MPU2", AID_MPU2, APPPRI_BAS, 512, 1<<20 ) )
			{
				//对于mpu2 模拟 mpu 时需要对外设个数严格校验
				if ( TYPE_MPUBAS == byWorkMode ||
					 TYPE_MPUBAS_H == byWorkMode
					)
				{
					if (ptCfg->GetMpu2EqpNum() != 1)
					{
						ErrorLog("[Mpu2Init]eqp num is illegal:%d\n",ptCfg->GetMpu2EqpNum() );
						printf("[Mpu2Init]eqp num is illegal:%d\n",ptCfg->GetMpu2EqpNum() );
						return FALSE;
					}
				}
				g_wKdvlogMid = MID_BASEQP_COMMON;
				g_cMpu2BasApp.SetBasWorkMode(byWorkMode);
				OspPost( MAKEIID(AID_MPU2, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(TMpu2Cfg) );
				printf("[Mpu2Init]send OSP_POWERON to start bas:%d\n",byWorkMode);
			}
			break;

			
		default:
			break;
		}
	}

	//zjj20120107 升级成功设置放到注册mcu成功之前做设置
/*
#ifdef _LINUX_
    s32 nRet = BrdSetSysRunSuccess();
    if ( nRet == UPDATE_VERSION_ROLLBACK )
    {
		WarningLog("[Mpu2Init]Update failed, rollback version!\n" );
       // mpulog( MPU_CRIT, " Update failed, rollback version!\n" );
    }
    else if ( nRet == SET_UPDATE_FLAG_FAILED )
    {
       ErrorLog("[Mpu2Init] BrdSetSysRunSuccess() failed!\n" );
	//	mpulog( MPU_CRIT, " BrdSetSysRunSuccess() failed!\n" );
	   BrdHwReset();
	   return FALSE;
    }
    else
	{
		//OspPrintf(TRUE, FALSE, "BrdSetSysRunSuccess, ret.%d\n", nRet );
		KeyLog( "[Mpu2Init]BrdSetSysRunSuccess, ret.%d\n", nRet );
	}
#endif
*/
    return TRUE;



}
/*lint -save -e765*/
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

	OspPrintf( TRUE, FALSE, "mpu2 Version: %s\n", gs_VersionBuf );
	OspPrintf( TRUE, FALSE, "\tmpu2 version : %s, compile time:%s:%s\n", VER_MPU2, __DATE__, __TIME__ );
}

API void mpuhelp()
{
//    OspPrintf( TRUE, FALSE, "mpu version:%s\t", VER_MPU );
//    OspPrintf( TRUE, FALSE, "compile time:%s:%s\n", __DATE__, __TIME__ );
//    OspPrintf( TRUE, FALSE, "mpustatus    ---- Display mpu status\n" );
//	OspPrintf( TRUE, FALSE, "showbasdebug ---- Show mpu debug\n" );
//	OspPrintf( TRUE, FALSE, "showvmpdebug ---- Show mpu debug\n" );
//    OspPrintf( TRUE, FALSE, "showmpuparam ---- Show mpu param\n" );
//    OspPrintf( TRUE, FALSE, "setmpulog    ---- setmpulog\n" );
//	OspPrintf( TRUE, FALSE, "setautotest  ---- 1: test mode\t 0: normal mode\n");
//	OspPrintf( TRUE, FALSE, "testvmp      ---- test vmp mode\n");
//	OspPrintf( TRUE, FALSE, "testbas      ---- test bas mode\n");
//	OspPrintf( TRUE, FALSE, "setvmpfr     ---- Set VMP Frame Rate\n");
//	OspPrintf( TRUE, FALSE, "resumess     ---- Set VMP Smooth Send Starting\n");
//	OspPrintf( TRUE, FALSE, "pausess      ---- Set VMP Smooth Send Pausing\n");
// 	OspPrintf( TRUE, FALSE, "showsmoothstatus   ----Show VMP SmoothSend's Flag\n");
    OspPrintf( TRUE, FALSE,  "mpu version:%s\t", VER_MPU2 );
    OspPrintf( TRUE, FALSE,  "compile time:%s:%s\n", __DATE__, __TIME__ );
    OspPrintf( TRUE, FALSE,  "mpustatus    ---- Display mpu status\n" );
	OspPrintf( TRUE, FALSE, "showbasdebug ---- Show mpu debug\n" );
	OspPrintf( TRUE, FALSE,  "showvmpdebug ---- Show mpu debug\n" );
    OspPrintf( TRUE, FALSE,  "showmpuparam ---- Show mpu param\n" );
    OspPrintf( TRUE, FALSE,  "setmpulog    ---- setmpulog\n" );
	OspPrintf( TRUE, FALSE,  "setautotest  ---- 1: test mode\t 0: normal mode\n");
	OspPrintf( TRUE, FALSE,  "testvmp      ---- test vmp mode\n");
	//KeyLog( "testbas      ---- test bas mode\n");
	OspPrintf( TRUE, FALSE,  "setvmpfr     ---- Set VMP Frame Rate\n");
	OspPrintf( TRUE, FALSE,  "resumess     ---- Set VMP Smooth Send Starting\n");
	OspPrintf( TRUE, FALSE,  "pausess      ---- Set VMP Smooth Send Pausing\n");
	OspPrintf( TRUE, FALSE,  "showsmoothstatus   ----Show VMP SmoothSend's Flag\n");
	
	OspPrintf( TRUE, FALSE,  "setautotestserverip   ----Set vmp autotest serverip 4 params no(.)eg:192 168 100 1\n");
}

API void mpu2status(u8 byEqpIdx = 0)
{
    CServMsg cServMsg;
	if (0 == byEqpIdx || byEqpIdx > MAXNUM_MPU2_EQPNUM ) 
	{
		OspPost( MAKEIID(AID_MPU2, CInstance::DAEMON), EV_VMP_DISPLAYALL, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    }
    else
	{
		OspPost( MAKEIID(AID_MPU2, byEqpIdx), EV_VMP_DISPLAYALL, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}
}

API void showbasdebug()
{
    OspPost( MAKEIID(AID_MPU2, CInstance::DAEMON), EV_BAS_SHOWDEBUG );	
}

API void showvmpdebug()
{
    OspPost( MAKEIID(AID_MPU2, 1), EV_VMP_SHOWDEBUG ); //debug配置两个instance都一样，所以只post给一个就可以
}

API void setmpulog( u8 byLevel = MPU_CRIT )
{
	logenablemod(g_wKdvlogMid);
    if (byLevel > MPU_DETAIL)
    {
        byLevel = MPU_DETAIL;
    }
	logsetlvl(byLevel + 1);
	logsetdst(4);	//输出到屏幕
    g_nmpubaslog = byLevel;
    return;
}

API void showmpuparam(u8 byEqpIdx = 0)
{
	CServMsg cServMsg;
	if (0 == byEqpIdx || byEqpIdx > MAXNUM_MPU2_EQPNUM ) 
	{
		OspPost( MAKEIID(AID_MPU2, CInstance::DAEMON), EV_VMP_SHOWPARAM, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    }
    else
	{
		OspPost( MAKEIID(AID_MPU2, byEqpIdx), EV_VMP_SHOWPARAM, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}

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
		WarningLog("VMP SmoothSend's Flag is Pausing!\n");
		// OspPrintf(TRUE,FALSE,"VMP SmoothSend's Flag is Pausing!\n");
	}
	else
	{
		WarningLog("VMP SmoothSend's Flag is Starting!\n");
	 //	OspPrintf(TRUE,FALSE,"VMP SmoothSend's Flag is Starting!\n");
	}
}


API void basshow()
{
    ::OspPost(MAKEIID( AID_MPU2, CInstance::EACH ), EV_BAS_SHOW);
}

API void setvmpfr(u8 byFrameRate)
{
	::OspPost( MAKEIID(AID_MPU2, 1), EV_VMP_SETFRAMERATE_CMD, &byFrameRate, sizeof(byFrameRate) );
}

/*lint -save -e715*/
//设置是否硬件自动检测
API void setautotest( s32 nIsAutoTest )  
{
#ifdef _LINUX_
	if( nIsAutoTest != 0)
	{
		BrdSetE2promTestFlag();
		//OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 1\n" );
		WarningLog("[setautotest] Set e2prom test flag 1\n" );
	}
	else 
	{
		BrdClearE2promTestFlag();
		WarningLog("[setautotest] Set e2prom test flag 0\n" );
		//OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 0\n" );
	}
#endif
	
	
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
    s32  sdwTest = g_cMpu2VmpApp.m_byWorkMode;	
	
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );

    if( !bRet )  
	{
		ErrorLog("[testvmp] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
		return;
	}
	KeyLog("Change to VMP Test Mode! Now you can reboot!\n");
	//reset
// #ifndef WIN32
// 	OspDelay(2000);
// 	BrdHwReset();
// #endif
    return;
}


//API void testbas(void)
//{
//	//转bas模式测试
//	s8   achProfileName[64] = {0};
//    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
//    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
//	
//	BOOL32 bRet;
//    s32  sdwTest = TYPE_MPUBAS;	
//    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );
//    if( !bRet )  
//	{
//		printf( "[RestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
//		return;
//	}
//	
//	OspPrintf(TRUE, FALSE, "Change to VMP Test Mode! Now you can reboot!\n");
//}

API void deldebugini(void)
{
#ifdef _LINUX_
	s8    achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	if( ERROR == unlink(achProfileName) )
	{
		ErrorLog("delete brdcfgdebug.ini failed!\n");
	}
	else
	{	
		KeyLog("brdcfgdebug.ini has been deleted!\n");
	}
#endif

}


API void setautotestserverip(u8 IpFirst,u8 IpSec,u8 IpThird,u8 IpForth)
{
	s8 lpServerIp[16]={0};
	sprintf(lpServerIp,"%d.%d.%d.%d",IpFirst,IpSec,IpThird,IpForth);
	
	memcpy(g_cMpuVmpTestApp.m_achServerIp,lpServerIp,strlen(lpServerIp)+1 );
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
    OspRegCommand("mpustatus",          (void*)mpu2status,           "mpustatus");
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
	//OspRegCommand("testbas",			(void*)testbas,				"testbas");
	OspRegCommand("deldebugini",		(void*)deldebugini,			"deldebugini");
	OspRegCommand("setvmpfr",			(void*)setvmpfr,			"setvmpfr");
	OspRegCommand("setautotestserverip",(void*)setautotestserverip,	"setautotestserverip");
#endif
}
/*lint -restore*/
//END OF FILE


