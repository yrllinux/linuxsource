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
//#include "mcuver.h"
#include "eqplib.h"
//#include "boardagent.h"
#include "mpuagent.h"

#ifdef _LINUX_
#include "nipdebug.h" 
#endif

#define APPNAME     (LPCSTR)"/usr/bin/kdvmpu"
LPCSTR g_szAppName;

/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_
// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();
#endif //_LINUX_
/////////////////////////////////////////////////////异常捕获处理

/*lint -save -e765*/
API void mpustart()
{
	
    BrdMpuAPIEnableInLinux();

	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if ( 0 != nRet )
	{
		printf( "[mpu][error] StartCaptureException Error(%d), Ignored.\n", nRet );
	}
    else
    {
        printf( "[mpu] StartCaptureException Succeed\n" );
    }    
#endif

	s8 achCommand[] = "cp /usr/bin/dft_16to9.bmp /usr/etc/config/dft_16to9.bmp";
	s32 nReturn = 0;
	nReturn = system( achCommand );
	printf( "[mpustart] Call system() return: %d \n", nReturn );
					     
    if ( !InitBrdAgent() )
    {
        printf( "[mpu][error] Init Agent Failed!\n" );
        return;
    }
    else
    {
        printf( "[mpu] Init Agent Succeeded!\n" );
    }

    //TEqpCfg tBasEqpCfg;
	TMpuCfg tBasEqpCfg;
	
	// xliang [5/27/2009] autotest
	tBasEqpCfg.SetAutoTest( g_cBrdAgentApp.GetIsTest() );

	if( g_cBrdAgentApp.GetIsTest())
	{
		//Set MPU work Mode
		s8    achProfileName[64] = {0};
		memset((void*)achProfileName, 0x0, sizeof(achProfileName));
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
		
		s32  dwTestMode = 0;
		BOOL32 bRet = GetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, 0, &dwTestMode );
        /*lint -save -esym(438, pFile)*/
		if( !bRet )  
		{
			printf( "[BrdAgent] profile doesn't exist while reading %s%s! So create automatically!\n", SECTION_BoardDebug, KEY_TestMode );
			// create new directory and file
#if defined(WIN32)
			CreateDirectory(DIR_CONFIG, NULL);
#elif defined(_VXWORKS_)
			mkdir(DIR_CONFIG);
#elif defined(_LINUX_)
			mkdir(DIR_CONFIG,0777);
			chmod(DIR_CONFIG,0777);
#endif
			char achFullName[64];
			sprintf(achFullName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
			char pbyBuf[] = "[BoardDebug]\r\nTestMode = 11\n";
			int dwInBufLen = strlen(pbyBuf)+1;
			FILE *pFile = fopen(achFullName, "wb");
			if (NULL != pFile)
			{
				if (0 != dwInBufLen)
				{
					fwrite(pbyBuf, 1, dwInBufLen, pFile);
#ifdef WIN32    //不作多余的 fflush 操作
					fflush(pFile);
#endif
				}
				fclose(pFile);
				pFile = NULL;
			}
			else
			{
				printf("Create file brdcfgdebug.ini failed\n");
				
			}
			dwTestMode = TYPE_MPUSVMP;		//默认起SVMP模式进行生产测试
		}
        /*lint -restore*/
		if( TYPE_MPUSVMP == dwTestMode )
		{
			printf("Begin VMP test mode!\n");
			g_cBrdAgentApp.SetEqpRcvStartPort( 40700 );
		}
		else if (TYPE_MPUBAS == dwTestMode )
		{
			printf("Begin BAS test mode!\n");
			g_cBrdAgentApp.SetEqpRcvStartPort( 40600 );
		}
		else
		{
			printf("Unexpected Mode:%d!\n", dwTestMode);
			return;
		}	
		g_cBrdAgentApp.SetWorkMode((u8)dwTestMode);
	}
	
	tBasEqpCfg.dwConnectIP    = g_cBrdAgentApp.GetMpcIpA(); 
	tBasEqpCfg.dwConnectIpB   = g_cBrdAgentApp.GetMpcIpB();
    tBasEqpCfg.wConnectPort   = g_cBrdAgentApp.GetMpcPortA();
    tBasEqpCfg.wMcuId         = LOCAL_MCUID;
    tBasEqpCfg.dwLocalIP      = g_cBrdAgentApp.GetBrdIpAddr();
    tBasEqpCfg.byEqpId        = g_cBrdAgentApp.GetEqpId();
    tBasEqpCfg.wRcvStartPort  = g_cBrdAgentApp.GetEqpRcvStartPort();
    // 这个wRcvStartPortB设计的奇怪，但正好可以派派上用场（DVmp）
    tBasEqpCfg.wRcvStartPortB = g_cBrdAgentApp.GetEqpRcvStartPort2();
    // 这里的tBasEqpCfg不填alias，移交mcu处理

	if ( TYPE_MPUSVMP == g_cBrdAgentApp.GetWorkMode() )
    {
        tBasEqpCfg.byEqpType = EQP_TYPE_VMP;
    }
    else if ( TYPE_MPUDVMP == g_cBrdAgentApp.GetWorkMode() )
    {
        tBasEqpCfg.byEqpType = EQP_TYPE_VMP;
    }
    else if ( TYPE_EVPU == g_cBrdAgentApp.GetWorkMode() )
    {
        tBasEqpCfg.byEqpType = EQP_TYPE_VMP;
    }
    else if ( TYPE_MPUBAS == g_cBrdAgentApp.GetWorkMode() 
				|| TYPE_MPUBAS_H == g_cBrdAgentApp.GetWorkMode())
    {
        tBasEqpCfg.byEqpType = EQP_TYPE_BAS;
    }
    else if ( TYPE_EBAP == g_cBrdAgentApp.GetWorkMode() )
    {
        tBasEqpCfg.byEqpType = EQP_TYPE_BAS;
    }
    else
    {
        printf( "[mpu][error] g_cBrdAgentApp.GetWorkMode() wrong!\n" );
    }

    printf( "[mpu] Starting mpu...\n" );
    if ( !MpuInit( &tBasEqpCfg, g_cBrdAgentApp.GetWorkMode()/*, g_cBrdAgentApp.GetEqpIdB()*/ ) )
    {
        printf( "[mpu] Init MPU failed!\r" );
    }
    else
    {
        printf( "[mpu] Starting MPU success !!!\n" );
    }
}


BOOL32 g_bQuit = FALSE;

#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
	printf("OspDemoQuitFunc:%d\n",nSignalNum);
    g_bQuit = TRUE;
}
#endif

int main( /*int argc, char** argv*/ )
{
    OspTaskDelay( 3 * 1000 ); 
    g_szAppName = APPNAME;
    printf( "[mpu] Starting: %s\n", g_szAppName );

#ifdef _LINUX_
    s8 byRet = BrdSetSysRunSuccess();
    if ( byRet == UPDATE_VERSION_ROLLBACK )
    {
        OspPrintf( TRUE, FALSE, "[mpu][error] Update failed, rollback version!\n" );
    }
    else if ( byRet == SET_UPDATE_FLAG_FAILED )
    {
        OspPrintf( TRUE, FALSE, "[mpu][error] BrdSetSysRunSuccess() failed!\n" );
    }
    OspPrintf(TRUE, FALSE, "[mpu] BrdSetSysRunSuccess(%d)\n", byRet );
#endif

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
