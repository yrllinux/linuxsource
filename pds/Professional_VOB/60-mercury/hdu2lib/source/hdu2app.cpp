/*****************************************************************************
模块名      : 高清解码卡
文件名      : hdu2app.cpp
相关文件    : hdu2app.h
文件实现功能: HDU2环境初始化
作者        : 田志勇
版本        : V1.0  Copyright(C) 2008-2010 KDV, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7			田志勇        创建   
******************************************************************************/
#include "hdu2eventid.h"
#include "hdu2inst.h"
#include "hdu2app.h"
#include "hdu2test.h"
#define  CURRENT_HDU2_MODE_NORMAL    1
#define  CURRENT_HDU2_MODE_PROTEST   2

/*lint -save -e843*/
/*lint -save -e765*/
u8 g_byCurMode = CURRENT_HDU2_MODE_NORMAL;//1 : 非生产测试模式 2 : 生产测试模式
u8 g_byHduPrintLevel = HDU_LVL_KEYSTATUS;

/*=============================================================================
函 数 名： hdu2info
功    能： 显示状态
算法实现： 
全局变量： 
参    数： 
返 回 值： 
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7         田志勇      创建
=============================================================================*/
API void hdu2info(void)
{
	switch(g_byCurMode)
	{
	case CURRENT_HDU2_MODE_PROTEST:
		{
			::OspPost(MAKEIID( AID_HDUAUTO_TEST,CHdu2TestClient::DAEMON ), EV_HDU_STATUSSHOW);
			for (u8 byInstId = 1 ;byInstId <= MAXNUM_HDU_CHANNEL;byInstId++)
			{
				::OspPost(MAKEIID( AID_HDUAUTO_TEST,byInstId ), EV_HDU_STATUSSHOW);
			}
		}
		break;
	case CURRENT_HDU2_MODE_NORMAL:
		{
			::OspPost(MAKEIID( AID_HDU,CInstance::DAEMON ), EV_HDU_STATUSSHOW);
			for (u8 byInstId = 1 ;byInstId <= MAXNUM_HDU_CHANNEL;byInstId++)
			{
				::OspPost(MAKEIID( AID_HDU,byInstId ), EV_HDU_STATUSSHOW);
			}
		}
		break;
	default:
		break;
	}
}

/*=============================================================================
函 数 名： hdutau
功    能： telnet授权
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7			田志勇        创建  
=============================================================================*/
API void hdu2tau( LPCSTR lpszUsrName/* = NULL */, LPCSTR lpszPwd /* = NULL */ )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
    return;
}
extern BOOL32 g_bPrintFrameInfo;
/*=============================================================================
函 数 名： pf
功    能： 开启打印接收帧信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7			田志勇        创建  
=============================================================================*/
API void pf()
{
	g_bPrintFrameInfo = TRUE;
}
/*=============================================================================
函 数 名： npf
功    能： 关闭打印接收帧信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7			田志勇        创建    
=============================================================================*/
API void npf()
{
	g_bPrintFrameInfo = FALSE;
}

/*=============================================================================
	函数  : sethdu2lvl
	功能  : 打印
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
  -----------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人      修改内容
	2013/03/11				陈兵        创建
=============================================================================*/
API BOOL32 sethdu2lvl(u8 byLevel)
{
	byLevel = (byLevel >= HDU_LVL_GENERAL) ? HDU_LVL_GENERAL : byLevel;

	OspPrintf(TRUE, FALSE, "------------HDU2INFO----------\n");
	OspPrintf(TRUE, FALSE, "Old Hdu2Info lvl: %d\n", g_byHduPrintLevel);

	g_byHduPrintLevel = byLevel;
	if (byLevel >= HDU_LVL_DETAIL)
	{
		logsetlvl(LOG_LVL_DETAIL);
	}
	else
	{
		logsetlvl(LOG_LVL_KEYSTATUS);
	}

	OspPrintf(TRUE, FALSE, "New Hdu2Info lvl: %d\n", g_byHduPrintLevel);

	return 0;
}

API BOOL32 showlvl()
{
	OspPrintf(TRUE, FALSE, "------------HDU2INFO----------\n");
	OspPrintf(TRUE, FALSE, "Current Hdu2Info lvl: %d\n", g_byHduPrintLevel);

	return 0 ;
}

API BOOL32 showmode()
{
	for (u8 byIndex=1; byIndex<=MAXNUM_HDU_CHANNEL; byIndex++)
	{
		OspPost(MAKEIID( AID_HDU, byIndex), EVENT_HDU_SHOWMODWE);	
	}

	return 0;
}

API BOOL32 clearmode(u8 byIndex)
{
	byIndex = byIndex == 0 ? 1 : 2;
	OspPost(MAKEIID( AID_HDU, byIndex), EVENT_HDU_CLEARMODWE);		
	return 0;
}
/*=====================================================================
函数  : hdu2libhelp
功能  : 查看HDU2主业务帮助命令
输入  : 无
输出  : 无
返回  : void
注    :
----------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7			田志勇        创建   
=====================================================================*/
API void hdu2libhelp(void)
{	
	OspPrintf( TRUE, FALSE,"---- All the help selection as follows ----\n");
	OspPrintf( TRUE, FALSE,"hdu2libver : not printf recved frames info\n");
    OspPrintf( TRUE, FALSE,"  hdu2info : hdu show\n");
    OspPrintf( TRUE, FALSE,"   hdu2tau : hdu telnet author\n");
	OspPrintf( TRUE, FALSE,"        pf : printf recved frames info\n");
	OspPrintf( TRUE, FALSE,"       npf : not printf recved frames info\n");
	OspPrintf( TRUE, FALSE," sethdu2lvl: set hdu2 outinfo level\n");
	OspPrintf( TRUE, FALSE," showlvl   : show hdu2 level\n");
	OspPrintf( TRUE, FALSE," showmode  : printf hdu2 ChnnlMode\n");
	OspPrintf( TRUE, FALSE," clearmode : clear hdu2 ChnnlMode\n");
}
/*=====================================================================
函数  : hdu2libver
功能  : 查看HDU2外壳帮助命令
输入  : 无
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
API void hdu2libver(void)
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
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
    }
    OspPrintf( TRUE, FALSE, "HDU2LIB Version: %s\n", gs_VersionBuf );
	OspPrintf(TRUE, FALSE, "\thdu2 version : %s compile time: %s    %s\n", VER_HDU2, __TIME__, __DATE__);
}
/*=============================================================================
函 数 名： HduAPIEnableInLinux
功    能： 注册linux调试接口
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7			田志勇        创建  
=============================================================================*/
static void Hdu2LibAPIEnableInLinux(void)
{
	/*lint -save -e537 -e760*/
	#undef OSPEVENT
	#define OSPEVENT(x, y) OspAddEventDesc(#x, y)
	#ifdef _EVENET_HDU2_H_
	#undef _EVENET_HDU2_H_
	#include "hdu2eventid.h"
	#define _EVENET_HDU2_H_
	#else
	#include "hdu2eventid.h"
	#undef _EVENET_HDU2_H_
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
/*lint -save -e611*/
#ifdef _LINUX_
	OspRegCommand("hdu2libhelp",     (void*)hdu2libhelp,    "hdu2libhelp info");
	OspRegCommand("hdu2libver",      (void*)hdu2libver,     "hdu2libver info");
    OspRegCommand("hdu2info",        (void*)hdu2info,       "hdu2 info show");
    OspRegCommand("hdu2tau",         (void*)hdu2tau,        "hdu2 telnet author");
	OspRegCommand("pf",              (void*)pf,             "printf recved frames info");
	OspRegCommand("npf",             (void*)npf,            "not printf recved frames info");
	OspRegCommand("sethdu2lvl",		 (void*)sethdu2lvl,		"printf hdu2 info");
	OspRegCommand("showlvl",		 (void*)showlvl,		"show hdu2 level");
	OspRegCommand("showmode",		 (void*)showmode,		"printf hdu2 ChnnlMode");
	OspRegCommand("clearmode",		 (void*)clearmode,		"clear hdu2 ChnnlMode 0 or 1");
	
#endif
	/*lint -restore */
}
/*=============================================================================
函 数 名： hdu2Init
功    能： hdu2初始化
算法实现： 
全局变量： g_cHduApp
参    数： TEqpCfg*
返 回 值： BOOL 
------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7			田志勇        创建  
=============================================================================*/
BOOL32 hdu2Init(THduCfg* ptCfg)
{
	if (NULL == ptCfg)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[hdu2Init]NULL == ptCfg!\n");
		return FALSE;
	}
    Hdu2LibAPIEnableInLinux();
	s32 dwRet = 0;
	if ( ptCfg->GetAutoTest() )
	{
		g_byCurMode = CURRENT_HDU2_MODE_PROTEST;
		OspSetPrompt("hdu2test");
		dwRet = g_cHdu2TestApp.CreateApp("hdu2test", AID_HDUAUTO_TEST, APPPRI_HDUAUTO);	//创建HDUAUTOTEST应用
		if ( OSP_OK != dwRet )
		{
			Hdu2Print(HDU_LVL_ERROR,  "[hdu2Init]hdu2test.CreateApp Failed! dwRet = %d\n", dwRet );
			return FALSE;
		}
		dwRet = ::OspPost(MAKEIID(AID_HDUAUTO_TEST, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(THduCfg));
		if ( OSP_OK != dwRet )
		{
			Hdu2Print(HDU_LVL_ERROR,   "[hdu2Init]OspPost Msg(OSP_POWERON) Failed! dwRet = %d\n", dwRet );
			return FALSE;
		}
	}
	else
	{
		g_byCurMode = CURRENT_HDU2_MODE_NORMAL;
		if( 0 == ptCfg->dwConnectIP && 0 == ptCfg->dwConnectIpB )// 验证Ip地址的有效性
		{
			Hdu2Print(HDU_LVL_ERROR,  "[hdu2Init] The mcu's ip are all 0\n");
			return FALSE;
		}
		if(ptCfg->dwConnectIP == ptCfg->dwConnectIpB)// 过滤一下同一Ip
		{
			ptCfg->wConnectPortB = 0;
		}
		OspSetPrompt("hdu2");
		dwRet = g_cHdu2App.CreateApp("hdu2", AID_HDU, APPPRI_HDU);	//创建HDU应用
		if ( OSP_OK != dwRet )
		{
			Hdu2Print(HDU_LVL_ERROR,  "[hdu2Init]hdu2.CreateApp Failed! dwRet = %d\n", dwRet );
			return FALSE;
		}
		dwRet = ::OspPost(MAKEIID(AID_HDU, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(THduCfg));
		if ( OSP_OK != dwRet )
		{
			Hdu2Print(HDU_LVL_ERROR,  "[hdu2Init]OspPost Msg(OSP_POWERON) Failed! dwRet = %d\n", dwRet );
			return FALSE;
		}
	}

	//zjj20120107升级成功标志提前到注册mcu成功前
	/*
#ifdef _LINUX_
    s8 byRet = BrdSetSysRunSuccess();
    if ( byRet == UPDATE_VERSION_ROLLBACK )
    {
        Hdu2Print(HDU_LVL_WARNING,  "[hdu2Init] Update failed, rollback version!\n" );
    }
    else if ( byRet == SET_UPDATE_FLAG_FAILED )
    {
        Hdu2Print(HDU_LVL_ERROR,  "[hdu2Init] BrdSetSysRunSuccess() failed,So Reboot!!\n" );
		return FALSE;
    }
	else if ( byRet == UPDATE_VERSION_SUCCESS )
	{
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[hdu2Init] BrdSetSysRunSuccess() succeed!\n" );
	}    
    else
	{
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[hdu2Init] BrdSetSysRunSuccess(), NO_UPDATE_OPERATION\n");
	}
#endif
*/
	OspStatusMsgOutSet(FALSE);
	return TRUE;
}
