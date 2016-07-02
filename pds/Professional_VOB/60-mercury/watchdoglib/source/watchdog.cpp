/*****************************************************************************
   模块名      : WatchDog API 实现
   文件名      : watchdog.cpp
   相关文件    : 
   文件实现功能: 启动WatchDog，向服务器注册自身状态。

   作者        : 顾振华
   版本        : V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/05/26  4.0         顾振华      创建
******************************************************************************/

//#include "kdvsys.h"
//#include "mcuconst.h"
#include "watchdogssn.h"
#include "bindwatchdog.h"
//#include "dsccomm.h"
#include "mcuconst.h"

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

CWatchDogSsnApp g_cWatchDogSsn;
BOOL32          g_bWDLog;

// 配置
struct TWDParam
{
    TWDParam()
    {
        bStart = FALSE;
    }
    BOOL32 bStart;
};
TWDParam g_tParam;

API void pwdmsg()
{
    g_bWDLog = TRUE;
}

API void npwdmsg()
{
    g_bWDLog = FALSE;
}

API void showwdstatus()
{
    g_cWatchDogSsn.PrintModuleState();
}
/*====================================================================
    函数名      ：WDStartWatchDog
    功能        ：启动看门狗
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2005/05/26  4.0         顾振华      创建
====================================================================*/
void ReadConfig()
{
#ifdef _LINUX_
    /*
    s8   achProfileName[32];
    BOOL bResult;
    s8   achDefStr[] = "Error String";
    s8   achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "modcfg.ini");

    bResult = GetRegKeyInt( achProfileName, "Modules", "WatchDogClient", 0, &nValue );
    */

    TBrdPosition tPosition; 
    STATUS status = BrdQueryPosition(&tPosition);
    if (status == OK)
    {
        if ( tPosition.byBrdID == BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ ||
             tPosition.byBrdID == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ || 
             tPosition.byBrdID == BRD_TYPE_DSC/*KDV8000B_MODULE*/ )
        {
            g_tParam.bStart = TRUE;        
        }
        else
        {
            g_tParam.bStart = FALSE;    
        }
    }
    else
    {
        g_tParam.bStart = FALSE;        
    }
    OspPrintf(TRUE, FALSE, "Now We are ready to start WatchDog & Brd(%d)? %s\n", 
              tPosition.byBrdID, 
              g_tParam.bStart ? "YES" : "NO" );
#endif
}

/*====================================================================
    函数名      ：WDStartWatchDog
    功能        ：启动看门狗
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2005/05/26  4.0         顾振华      创建
====================================================================*/
API BOOL32 WDStartWatchDog ( emModuleType enType)
{
#ifdef _LINUX_

    OspRegCommand("pwdmsg",             (void*)pwdmsg,  "Print WatchDog Client Msg");
    OspRegCommand("npwdmsg",            (void*)npwdmsg, "Not Print WatchDog Client Msg");
    OspRegCommand("showwdstatus",       (void*)showwdstatus, "Print WatchDog Module State");

    ReadConfig();
    if (!g_tParam.bStart)
    {
        // 用户配置为不启动。或者没有找到配置文件等，默认不启动
        return FALSE;
    }

    if ( ! IsOspInitd() )
	{
		OspInit(TRUE, 2500);
	}

    //Osp telnet 初始授权 [11/28/2006-zbq]
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

    g_cWatchDogSsn.CreateApp("WatchDogClntApp", WD_CLT_APP_ID, 160, 10, 10<<10);
    
    u8 byNewIdx = g_cWatchDogSsn.AddModule(enType);
    if (byNewIdx == 0)
    {
        return FALSE;
    }

    OspPost( MAKEIID(WD_CLT_APP_ID, 1), OSP_POWERON );

    return TRUE;
#else
    return FALSE;
#endif 
}


#ifdef WIN32
int main()
{
	/*lint -save -e522*/
    WDStartWatchDog( emMP );
    WDStartWatchDog( emPRS );
    WDStartWatchDog( emMTADP );
    Sleep(INFINITE);
	/*lint -restore*/

    return 1;
}
#endif

