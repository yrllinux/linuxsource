/*****************************************************************************
   模块名      : MPU2 单板主控
   文件名      : mpu2.cpp
   相关文件    : 
   文件实现功能: mpu2业务封装
   作者        : zhouyiliang
   版本        : V4.7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/

#include "osp.h"
#include "mcuconst.h"
#include "eqplib.h"
#include "mpu2agent.h"

#ifdef _LINUX_
#include "nipdebug.h" 
#endif

/*lint -save -e765*/

#if defined(WIN32) || defined(_WIN32)
#define MPU2_KDVLOG_CONF_FILE	(s8*)"./conf/kdvlog_mpu2.ini"
#else
#define MPU2_KDVLOG_CONF_FILE	(s8*)"/usr/etc/config/conf/kdvlog_mpu2.ini"
#endif

#define APPNAME     (LPCSTR)"/usr/bin/kdvmpu2"
LPCSTR g_szAppName;

/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_
// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();
#endif //_LINUX_
/////////////////////////////////////////////////////异常捕获处理

API void mpustart()
{
	
    BrdMpuAPIEnableInLinux();
	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if ( 0 != nRet )
	{

		printf( "[mpu][error] StartCaptureException Error(%d), Ignored.\n", nRet );
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"[mpu2][error] StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
		LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "[mpu2] StartCaptureException Succeed\n" );
        printf( "[mpu] StartCaptureException Succeed\n" );
    }    
#endif

	s8 achCommand[] = "mv /usr/bin/dft_16to9.bmp /usr/etc/config/dft_16to9.bmp";
	s32 nReturn = 0;
	nReturn = system( achCommand );
	LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,  "[mpustart] Call system() return: %d \n", nReturn );


	//zjj20120107 升级成功设置放到注册mcu成功之前做设置
#ifdef _LINUX_
    nRet = BrdSetSysRunSuccess();
    if ( nRet == UPDATE_VERSION_ROLLBACK )
    {
		LogPrint(LOG_LVL_WARNING,MID_PUB_ALWAYS,  "[Mpu2Init]Update failed, rollback version!\n" );		
    }
    else if ( nRet == SET_UPDATE_FLAG_FAILED )
    {
	   LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"[Mpu2Init] BrdSetSysRunSuccess() failed!\n" );	   
	   return;
    }
    else
	{
		LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"[Mpu2Init]BrdSetSysRunSuccess, ret.%d\n", nRet );		
	}
#endif
					     
    if ( !InitBrdAgent() )
    {
        LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,  "[mpu2][error] Init Agent Failed!\n" );
        return;
    }
    else
    {
        LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,  "[mpu2] Init Agent Succeeded!\n" );
		printf("[mpu2] Init Agent Succeeded!\n" );
    }



	//生产测试模式，保存测试的testmode，并将workmode填为mpu2basic或者enhanced
	if( g_cBrdAgentApp.GetIsAutoTest())
	{
		//Set MPU2 work Mode
		u8 byTestMode = TYPE_MPU2VMP_BASIC;
		BOOL32 bHasEcard = FALSE;
#ifdef _LINUX_
		if ( BrdMpu2HasEcard() == 1)
		{
			bHasEcard = TRUE;
		}
		
#endif
		if ( bHasEcard ) //插子卡
		{
			byTestMode = TYPE_MPU2VMP_ENHANCED;		//插子卡，则起ENHANCE模式
		}
		else
		{
			byTestMode = TYPE_MPU2VMP_BASIC;		//不插子卡，测BASIC模式
		}
			
		
			
		g_cBrdAgentApp.SetMpu2WorkMode(byTestMode);
	}
	

	

    LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "[mpu2] Starting mpu2...\n" );
	printf("[mpu2] Starting mpu2...\n" );

	//pclint修改
	TMpu2Cfg tMpu2Cfg = g_cBrdAgentApp.GetMpu2Cfg();
    if ( !Mpu2Init( &tMpu2Cfg ) )
    {
       printf("[mpu2] Init MPU2 failed!\r" );
        LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "[mpu2] Init MPU2 failed!\r" );
    }
    else
    {
		printf("[mpu2] Starting MPU2 success !!!\n" );
        LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS, "[mpu2] Starting MPU2 success !!!\n" );
    }
}


BOOL32 g_bQuit = FALSE;

/*lint -save -e715*/
#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
    g_bQuit = TRUE;
	printf("[OspDemoQuitFunc]sig:%d come!\n",nSignalNum);
}
#endif

int main( int argc, char** argv )
{
	Err_t err = LogInit(MPU2_KDVLOG_CONF_FILE);
	
	if(LOG_ERR_NO_ERR != err)
	{
		printf("KdvLog Init failed, err is %d\n", err);
	}
 //   logsetdst(LOG_DST_SERIAL);
    g_szAppName = APPNAME;
    printf( "[mpu2] Starting: %s\n", g_szAppName );
	LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"[mpu] Starting: %s\n", g_szAppName);
	
//#ifdef _LINUX_
//    s8 byRet = BrdSetSysRunSuccess();
//    if ( byRet == UPDATE_VERSION_ROLLBACK )
//    {
//        OspPrintf( TRUE, FALSE, "[mpu][error] Update failed, rollback version!\n" );
//    }
//    else if ( byRet == SET_UPDATE_FLAG_FAILED )
//    {
//        OspPrintf( TRUE, FALSE, "[mpu][error] BrdSetSysRunSuccess() failed!\n" );
//    }
//    OspPrintf(TRUE, FALSE, "[mpu] BrdSetSysRunSuccess(%d)\n", byRet );
//#endif
	OspSetPrompt("mpu2");
	mpustart();

#ifdef _LINUX_
    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
#endif
		
	while( !g_bQuit )
	{
	    OspDelay( 1000 );
	}   
    
    printf( "[mpu] Quit normally!\n" );
	LogPrint(LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"[mpu2] Quit normally!\n");

	return 0;
}	
/*lint -restore*/

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
    return init_nipdebug( 0, NULL, DIR_EXCLOG );
}

#endif // _LINUX


API void pmpumsg(void)
{
	g_cBrdAgentApp.m_bPrintBrdLog = TRUE;
}

API void npmpumsg( void )
{
	g_cBrdAgentApp.m_bPrintBrdLog = FALSE;
}


//注册Linux调试接口
/*lint -save -e611*/
void BrdMpuAPIEnableInLinux(void)
{
#ifdef _LINUX_  
    OspRegCommand("mpustart",       (void*)mpustart,        "mpu start");
	OspRegCommand("pmpumsg",		(void*)pmpumsg,         "pmpumsg");
	OspRegCommand("npmpumsg",		(void*)npmpumsg,        "npmpumsg");
#endif
}
/*lint -restore*/

/////////////////////////////////////////////////////异常捕获处理
