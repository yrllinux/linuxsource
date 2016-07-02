/*****************************************************************************
模块名      : 高清解码卡
文件名      : hduapp.cpp
相关文件    : hduapp.h
文件实现功能: HDU环境初始化
作者        : 江乐斌
版本        : V1.0  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/11/26              江乐斌       创建    
******************************************************************************/
#undef OSPEVENT
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
#include "hduinst.h"
//#include "evhduid.h"
//#include "mcuver.h"
#include "hduapp.h"
#include "hduhwautotest.h"
#include "hduhwautotest_evid.h"

s32 g_nHduPLog = 0;
/*=============================================================================
函 数 名： hduInit
功    能： hdu初始化
算法实现： 
全局变量： g_cHduApp
参    数： TEqpCfg*
返 回 值： BOOL 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
BOOL hduInit(THduCfg* ptCfg)
{    
	if (NULL == ptCfg)
	{
		printf("[hduInit] ptCfg is Null!\n");
		return FALSE;
	}
	printf("enter hduInit()!\n");
    HduAPIEnableInLinux();

	if ( ptCfg->GetAutoTest() )
	{
		g_cHduAutoTestApp.CreateApp("hduauto", AID_HDUAUTO_TEST, APPPRI_HDUAUTO);	//创建HDUAUTOTEST应用
	}
	else
	{
		// 验证Ip地址的有效性
		if( 0 == ptCfg->dwConnectIP && 0 == ptCfg->dwConnectIpB )
		{
			OspPrintf(TRUE, FALSE, "[hdu] The mcu's ip are all 0\n");
			return FALSE;
		}
		// 过滤一下同一Ip
		if(ptCfg->dwConnectIP == ptCfg->dwConnectIpB)
		{
			ptCfg->dwConnectIpB = 0;
			ptCfg->wConnectPortB = 0;
		}
		g_cHduApp.CreateApp("hdu", AID_HDU, APPPRI_HDU);	//创建HDU应用
	}
	
    // 加载事件声明
//  	#undef OSPEVENT
// 	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
// 
// #ifdef _EVENET_HDU_H_
// 	#undef _EVENET_HDU_H_
// 	#include "evhduid.h"
// 	#define _EVENET_HDU_H_
// #else
// 	#include "evhduid.h"
//     #undef _EVENET_HDU_H_
// #endif
// 
// #ifdef _EV_MCUEQP_H_
// 	#undef _EV_MCUEQP_H_
// 	#include "evmcueqp.h"
// 	#define _EV_MCUEQP_H_
// #else
// 	#include "evmcueqp.h"
//     #undef  _EV_MCUEQP_H_
// #endif
	
	OspStatusMsgOutSet(FALSE);

	if ( ptCfg->GetAutoTest() )
	{
//		OspTaskDelay(10000);

		printf( "[hduautotest] enter auto test!\n" );
		// 开始上电初始化
		u32 dwRet = 0;
		dwRet = ::OspPost(MAKEIID(AID_HDUAUTO_TEST, CInstance::DAEMON), EV_C_INIT, ptCfg, sizeof(THduCfg));
        if ( 0 == dwRet )
        {
			printf( "[hduInit] OspPost Sucecced! dwRet = %d\n", dwRet );
        }
		else
		{
            printf( "[hduInit] OspPost failed!\n dwRet = %d\n", dwRet );
		}
	}
	else
	{
		printf( "[hdu]  enter hdu app!\n" );
		// 开始上电初始化
		::OspPost(MAKEIID(AID_HDU, CInstance::DAEMON), OSP_POWERON, ptCfg, sizeof(THduCfg));
	}

	//zjj20120107升级成功标志提前到注册mcu成功前
/*
#ifdef _LINUX_
    s16 wRet = BrdSetSysRunSuccess();
    if ( wRet == UPDATE_VERSION_ROLLBACK )
    {
        printf( "[hduInit] Update failed, rollback version!\n" );
    }
    else if ( wRet == SET_UPDATE_FLAG_FAILED )
    {
        printf( "[hduInit] BrdSetSysRunSuccess() failed!\n" );
    }
	else if ( wRet == UPDATE_VERSION_SUCCESS )
	{
        printf( "[hduInit] BrdSetSysRunSuccess() succeed!\n" );
	}    
    else
	{
		printf( "[hduInit] BrdSetSysRunSuccess(), NO_UPDATE_OPERATION\n");
	}
#endif
*/

    if ( ptCfg->GetAutoTest() )
	{
		OspSetPrompt("hduauto");
	}
	else
	{
		OspSetPrompt("hdu");
	}

	return TRUE;
}

/*=============================================================================
函 数 名： HduAPIEnableInLinux
功    能： 注册linux调试接口
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
void HduAPIEnableInLinux(void)
{
	//lint -save -e611
#ifdef _LINUX_  
    OspRegCommand("hduhelp",        (void*)hduhelp,         "hdu help");
    OspRegCommand("sethdulog",      (void*)sethdulog,       "set hdulog(0,1,2)");
    OspRegCommand("hdushow",        (void*)hdushow,         "hdu show");
    OspRegCommand("hduinfo",        (void*)hduinfo,         "hdu info");
    OspRegCommand("hduver",         (void*)hduver,          "hdu ver");
    OspRegCommand("hdutau",         (void*)hdutau,          "hdu telnet author");
    OspRegCommand("setmode",        (void*)sethdumode,      "set output mode");
    OspRegCommand("showmode",       (void*)showmode,        "show output mode");
    OspRegCommand("setautotest",    (void*)setautotest,      "change auto test");
	RegsterCommands();
#endif
	//lint -restore
}

/*=============================================================================
函 数 名： hduver
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void hduver(void)
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
    OspPrintf( TRUE, FALSE, "HDU Version: %s\n", gs_VersionBuf );
	OspPrintf(TRUE, FALSE, "\thdu version : %s compile time: %s    %s\n", VER_HDU, __TIME__, __DATE__);
}


/*=============================================================================
函 数 名： hduhelp
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void hduhelp(void)
{
    OspPrintf( TRUE, FALSE, "[Hdu]: ---- All the help selection as follows ----\n");
	hduver();
	OspPrintf( TRUE, FALSE, "\thdushow     : HDU status show \n" );
	OspPrintf( TRUE, FALSE, "\tsethdulog(i): Set debug level on screen. 0-nprint, 1/2-print\n\n" );
    OspPrintf( TRUE, FALSE, "\thdutau(UsrName, Pwd) : HDU telnet author \n" );
}


/*=============================================================================
函 数 名： sethdulog
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void sethdulog(int nlvl)
{
    g_nHduPLog = nlvl;   
    return;
}

/*=============================================================================
函 数 名： hduscreen
功    能： 设置屏幕打印级别
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void hduscreen(void)
{
    sethdulog(1);
	return;
}

/*=============================================================================
函 数 名： hdushow
功    能： 显示状态
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void hdushow(void)
{
	::OspPost(MAKEIID( AID_HDU,CInstance::EACH ), EV_HDU_STATUSSHOW);
    return;
}

/*=============================================================================
函 数 名： hduinfo
功    能： 显示配置信息
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API	void hduinfo(void)
{
	::OspPost(MAKEIID( AID_BAS,CInstance::EACH ), EV_HDU_CONFIGSHOW);
    return;
}

/*=============================================================================
函 数 名： hdutau
功    能： telnet授权
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void hdutau( LPCSTR lpszUsrName/* = NULL */, LPCSTR lpszPwd /* = NULL */ )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
    return;
}

/*=============================================================================
函 数 名： showmode
功    能： 显示hdu输入输出制式
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void showmode(void)
{
	THduModePort tModePort1, tModePort2;
	memset(&tModePort1, 0x0, sizeof(tModePort1));
	memset(&tModePort2, 0x0, sizeof(tModePort2));
	g_cHduApp.m_tCfg.GetHduModePort(0, tModePort1);
	g_cHduApp.m_tCfg.GetHduModePort(1, tModePort2);
				
	OspPrintf(TRUE, FALSE," m_byOutPortType1 = %d, m_byOutModeType1 = %d\nm_byOutPortType2 = %d, m_byOutModeType2 = %d\n",
					tModePort1.GetOutPortType(), tModePort1.GetOutModeType(),
					tModePort2.GetOutPortType(), tModePort2.GetOutModeType()
		       );
	return;
}

/*=============================================================================
函 数 名： sethdumode
功    能： 设置hdu输入输出制式
算法实现： 
全局变量： 
参    数： u8 byNum  ：hdu通道号  0或1
           u8 byPort ：VGA或YPbPr
		   u8 byMode
		   u8 byPlayScale--->0表示4:3  1表示16:9
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void sethdumode( u8 byNum, u8 byPort, u8 byMode, u8 byPlayScale)
{
	THduModePort tModePort;
	memset(&tModePort, 0x0, sizeof(tModePort));
	tModePort.SetOutModeType(byMode);
	tModePort.SetOutPortType(byPort);
	tModePort.SetZoomRate( byPlayScale );
	g_cHduApp.m_tCfg.SetHduModePort(byNum, tModePort);
	::OspPost(MAKEIID( AID_HDU, byNum+1 ), EV_HDU_SETMODE);
    return;
}

/*=============================================================================
函 数 名： setautotest
功    能： 设置是否硬件自动检测
算法实现： 
全局变量： 
参    数： s32 :测试标志：0 --> 不自动测试，1 --> 自动测试
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/27  4.6			江乐斌                  创建
=============================================================================*/
API void setautotest( s32 nIsAutoTest )
{
    ::OspPost( MAKEIID( AID_HDUAUTO_TEST, CHduAutoTestClient::DAEMON ), EV_C_CHANGEAUTOTEST_CMD, &nIsAutoTest, sizeof(s32) );
    return;
}


