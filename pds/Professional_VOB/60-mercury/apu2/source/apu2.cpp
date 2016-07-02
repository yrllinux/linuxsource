#include "osp.h"
#include "eqplib.h"
#include "apu2agent.h"
#include "libinit.h"


#define APPNAME     (LPCSTR)"/usr/bin/kdvapu2"

#ifdef _LINUX_
#include "nipdebug.h"
#endif


#if defined(WIN32) || defined(_WIN32)
#define APU2_KDVLOG_CONF_FILE	(s8*)"./conf/kdvlog_apu2.ini"
#else
#define APU2_KDVLOG_CONF_FILE	(s8*)"/usr/etc/config/conf/kdvlog_apu2.ini"
#endif

#ifdef _LINUX_
// 开始捕捉入口函数
// Ret - 0 正常返回
static s32 StartCaptureException()
{
	return init_nipdebug( 0, NULL, DIR_EXCLOG );
}
#endif

/*lint -save -e843*/
BOOL32 g_bQuit = FALSE;
#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
	OspPrintf(TRUE, FALSE, "[OspDemoQuitFunc] nSignalNum.%d!\n",nSignalNum);
    g_bQuit = TRUE;
}
#endif

/*====================================================================
函数名      ：apu2start
功能        ：apu2启动
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
API void apu2start()
{
    // 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
    if ( 0 != nRet )
    {
        printf( "[apu2start]StartCaptureException() Error(%d), Ignored.\n", nRet );
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[apu2start]StartCaptureException() Error(%d), Ignored.\n", nRet );
    }
    else
    {
        printf( "[apu2start]StartCaptureException() Succeed\n" );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[apu2start]StartCaptureException() Succeed\n");
    }    
#endif             
    if ( !InitBrdAgent() )
    {
        printf( "[apu2start]InitBrdAgent() Failed!\n" );
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[apu2start]InitBrdAgent() Failed!\n" );
        return;
    }
    else
    {
        printf( "[apu2start]InitBrdAgent() Succeeded!\n" );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[apu2start]InitBrdAgent() Succeeded!\n" );
    }
	
    TApu2EqpCfg   tApu2cfg;
    tApu2cfg = g_cBrdAgentApp.GetApu2Cfg();
    if (!tApu2cfg.m_bIsProductTest)
    {
		//混音器
        for (u16 wIndex = 0; wIndex < min(MAXNUM_APU2_MIXER, tApu2cfg.m_byMixerNum); wIndex++)
        {
            tApu2cfg.m_acMixerCfg[wIndex].dwLocalIP	    = ntohl(g_cBrdAgentApp.GetBrdIpAddr());
            tApu2cfg.m_acMixerCfg[wIndex].dwConnectIP   = g_cBrdAgentApp.GetMpcIpA();
			tApu2cfg.m_acMixerCfg[wIndex].dwConnectIpB  = g_cBrdAgentApp.GetMpcIpB();
            tApu2cfg.m_acMixerCfg[wIndex].wConnectPort  = g_cBrdAgentApp.GetMpcPortA();
            tApu2cfg.m_acMixerCfg[wIndex].achAlias[MAXLEN_EQP_ALIAS - 1] = '\0';
        }
		//适配器
		for (u16 wLoop = 0; wLoop < MAXNUM_APU2_BAS; wLoop++)
        {
            tApu2cfg.m_acBasCfg[wLoop].dwLocalIP	  = ntohl(g_cBrdAgentApp.GetBrdIpAddr());
            tApu2cfg.m_acBasCfg[wLoop].dwConnectIP   = g_cBrdAgentApp.GetMpcIpA();
			tApu2cfg.m_acBasCfg[wLoop].dwConnectIpB  = g_cBrdAgentApp.GetMpcIpB();
            tApu2cfg.m_acBasCfg[wLoop].wConnectPort  = g_cBrdAgentApp.GetMpcPortA();
            tApu2cfg.m_acBasCfg[wLoop].achAlias[MAXLEN_EQP_ALIAS - 1] = '\0';
        }
    }
	else
	{
		printf("[PRODUCTTEST]Start read config!\n");
		BOOL32 bRet = FALSE;
		s8    achDefStr[] = "10.1.1.1";
	    s8    achReturn[MAX_VALUE_LEN + 1]  = {0};
		s8 achProfileName[64] = {0};
		memset((void*)achProfileName, 0x0, sizeof(achProfileName));
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
		bRet = GetRegKeyString( achProfileName, SECTION_BoardDebug, KEY_ServerIpAddr, 
								achDefStr, achReturn, MAX_VALUE_LEN + 1 );
		if( bRet )  
		{
			printf("[PRODUCTTEST]Config read success!\n");
			tApu2cfg.m_acMixerCfg[0].dwConnectIP = ntohl(inet_addr(achReturn));
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[apu2start] Success profile while reading %s**%s**%s***[%s]!\n",
										achProfileName, SECTION_BoardDebug, KEY_ServerIpAddr, achReturn );
		}
		else
		{
			printf("[PRODUCTTEST]Config read failed!\n");
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[apu2start] Wrong profile while reading %s**%s**%s!\n", 
												achProfileName,SECTION_BoardDebug, KEY_ServerIpAddr );
			tApu2cfg.m_acMixerCfg[0].dwConnectIP = ntohl(inet_addr("10.1.1.1"));
		}
		tApu2cfg.m_acMixerCfg[0].wConnectPort = 60000;
		tApu2cfg.m_acMixerCfg[0].m_byMemberNum = MAXNUM_MIXER_CHNNL; 
	}
	
    if (!apu2Init(&tApu2cfg))
    {
        printf( "[apu2start]eapuInit Failed!\n" );
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[apu2start]eapuInit Failed!\n");
    }
	else
    {
        printf( "[apu2start]eapuInit Successed!\n" );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[apu2start]eapuInit Successed!\n");
    } 

	return;
}

/*====================================================================
函数名      ：main
功能        ：函数入口
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
/*lint -save -e715*/
int main(int argc, char** argv)
{
	Err_t err = LogInit(APU2_KDVLOG_CONF_FILE);
	logenablemod(MID_MCU_EQP);
	//logenablemod(MID_MCU_MIXER);
	logenablemod(MID_MCU_BAS);
	logsetlvl(LOG_LVL_KEYSTATUS);

	if (LOG_ERR_NO_ERR != err)
	{
		printf("[main]LogInit() failed! ERRCODE = [%d]\n",err);
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[main]LogInit() failed! ERRCODE = [%d]\n",err);
	}

	OspTaskDelay( 1 * 1000 ); 

    printf( "[main] Starting: %s\n", APPNAME );
	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[main] Starting: %s\n", APPNAME);

#ifdef _LINUX_
    s8 byRet = BrdSetSysRunSuccess();
    if ( byRet == UPDATE_VERSION_ROLLBACK )
    {
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[main]BrdSetSysRunSuccess() Update failed, rollback version!\n");
    }
    else if ( byRet == SET_UPDATE_FLAG_FAILED )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[main]BrdSetSysRunSuccess() Setting failed!\n");
    }
	else
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[main]BrdSetSysRunSuccess(%d)\n", byRet );
	}
#endif
    apu2start();
#ifdef _LINUX_
    //注册退出清除函数
    OspRegQuitFunc( OspDemoQuitFunc );
#endif
    while( !g_bQuit )
    {
        OspDelay( 1000 );
    }   
    printf( "[main] Quit normally!\n" );
	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[main] Quit normally!\n" );

    return 0;
}
/*lint -restore*/
/*====================================================================
函数名      ：help
功能        ：帮助
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
/*lint -save -e765 -e714*/
API void help(void)
{
	StaticLog("API as follows:\n");
	StaticLog("peqpmsg     ----------->  show eqp print\n");
	StaticLog("npeqpmsg    -----------> not show eqp print\n");
}	
/*====================================================================
函数名      ：papu2msg
功能        ：放开混音打印
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
API void peqpmsg(void)
{
    logenablemod(MID_MCU_EQP);
}

/*====================================================================
函数名      ：npapu2msg
功能        ：关闭混音打印
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
API void npeqpmsg( void )
{
    logdisablemod(MID_MCU_EQP);
}


