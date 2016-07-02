/*****************************************************************************
   模块名      : BAP HD
   文件名      : baphd.cpp
   相关文件    : 
   文件实现功能: BAP-HD(MT版本)业务封装
   作者        : 顾振华
   版本        : V4.5  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/

#include "osp.h"
#include "mcuconst.h"
#include "mcuver.h"
#include "eqplib.h"
#include "boardagent.h"
#include "bapagent.h"

#ifdef _LINUX_
#include "nipdebug.h"
#endif

#define APPNAME     (LPCSTR)"/usr/bin/kdvbap"
LPCSTR g_szAppName;

/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_
// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();
#endif //_LINUX_
/////////////////////////////////////////////////////异常捕获处理

API void bapstart()
{

	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("Bap: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("Bap: StartCaptureException Succeeded\n");
    }    
#endif
		
    //need to activate kdvsyslib.a and DataSwitch.a on VxWorks
#ifdef _VXWORKS_    
    Trim("Kdv Mcu 4.0");
#endif
			     
    if (!InitBrdAgent())
    {
        printf("Init Agent Failed!!!!!!\n");
        return;
    }
    else
    {
        printf("Init Agent Succeeded!\n");
    }
        
    // start up bitstream adapter server
    TEqpCfg tBasEqpCfg;
    tBasEqpCfg.dwConnectIP    = g_cBrdAgentApp.GetMpcIpA(); // single link
    tBasEqpCfg.wConnectPort   = g_cBrdAgentApp.GetMpcPortA();
    tBasEqpCfg.byEqpType      = EQP_TYPE_BAS;
    tBasEqpCfg.byEqpId        = g_cBrdAgentApp.GetEqpId();
    tBasEqpCfg.wMcuId         = LOCAL_MCUID;
    tBasEqpCfg.dwLocalIP      = g_cBrdAgentApp.GetBrdIpAddr();

    printf("Starting BAS HD ......\n");
    if (!basInit(&tBasEqpCfg))
    {
        printf("Init BAS failed!\r");
    }
    else
    {
        printf("Starting BAS success !!!\n");
    }
}

// API void bappver()
// {
//     OspPrintf(TRUE, TRUE, "vmphd version:%s\t", VER_MMP);
//     OspPrintf(TRUE, TRUE, "compile time:%s:%s\n", __DATE__, __TIME__);
// 
//     basver();
//     ospver();
//}


BOOL32 g_bQuit = FALSE;

#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
    g_bQuit = TRUE;
}
#endif

int main( int argc, char** argv )
{
    g_szAppName = APPNAME;
    printf("[AppEntry]Starting: %s\n", g_szAppName);

	bapstart();

#ifdef _LINUX_
    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
#endif

		
	while(!g_bQuit)
	{
	    OspDelay(1000);
	}   
    

    printf("Quit normally!\n");
	return 0;
}	


#ifdef _LINUX_
/////////////////////////////////////////////////////Linux 下异常捕获处理


/*=============================================================================
    函 数 名： StartCaptureException
    功    能： 设置进程异常捕获回调
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： s32 0-成功，非0-错误码
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/7/11   1.0			万春雷                  创建
=============================================================================*/
s32 StartCaptureException()
{
    return init_nipdebug(0, NULL, DIR_EXCLOG);
}

#endif // _LINUX

/////////////////////////////////////////////////////异常捕获处理
