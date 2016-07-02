#include "osp.h"
#include "mcuconst.h"
#include "eapuagent.h"

#ifdef _LINUX_
#include "nipdebug.h"
#endif

#define APPNAME     (LPCSTR)"/usr/bin/kdveapu"

/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_
// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();
#endif //_LINUX_
/////////////////////////////////////////////////////异常捕获处理
API void eapustart()
{
    BrdEapuAPIEnableInLinux();
    // 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
    if ( 0 != nRet )
    {
        printf( "[eapustart]StartCaptureException() Error(%d), Ignored.\n", nRet );
    }
    else
    {
        printf( "[eapustart]StartCaptureException() Succeed\n" );
    }    
#endif             
    if ( !InitBrdAgent() )//板向MCU注册  板位置：板ID，板层号，板槽号  板IP  网口位置选择  操作系统版本类型
    {
        printf( "[eapustart]InitBrdAgent() Failed!\n" );
        return;
    }
    else
    {
        printf( "[eapustart]InitBrdAgent() Succeeded!\n" );
    }

    TEapuCfg   tEapucfg;
    tEapucfg = g_cBrdAgentApp.GetEapuCfg();
    if (!tEapucfg.IsProductTest())
    {
        OspPrintf( TRUE, FALSE, "m_byMixerNum = [%d]\n",tEapucfg.m_byMixerNum);
        for (u16 wIndex = 0; wIndex < min(MAXNUM_EAPU_MIXER,tEapucfg.m_byMixerNum); wIndex++)
        {
            tEapucfg.m_tEapuMixerCfg[wIndex].dwLocalIP     = ntohl(g_cBrdAgentApp.GetBrdIpAddr());
            tEapucfg.m_tEapuMixerCfg[wIndex].dwConnectIP   = g_cBrdAgentApp.GetMpcIpA();
			tEapucfg.m_tEapuMixerCfg[wIndex].dwConnectIpB  = g_cBrdAgentApp.GetMpcIpB();
            tEapucfg.m_tEapuMixerCfg[wIndex].wConnectPort  = g_cBrdAgentApp.GetMpcPortA();
            tEapucfg.m_tEapuMixerCfg[wIndex].achAlias[MAXLEN_EQP_ALIAS - 1] = '\0';
        }
    }
	
	if ( g_cBrdAgentApp.IsSimuApu() )
	{
		tEapucfg.m_byIsSimuAPU = 1;
	}
	else
	{
		tEapucfg.m_byIsSimuAPU = 0;
	}

    if (!eapuInit(&tEapucfg))
    {
        printf( "[eapustart]eapuInit Failed!\n" );
    }
	else
    {
        printf( "[eapustart]eapuInit Successed!\n" );
    }
	return;
}
/*lint -save -e843*/
static BOOL32 g_bQuit = FALSE;

#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
    g_bQuit = TRUE;
}
#endif

int main()
{
    OspTaskDelay( 1 * 1000 ); 
    printf( "[main] Starting: %s\n", APPNAME );
#ifdef _LINUX_
    s8 byRet = BrdSetSysRunSuccess();
    if ( byRet == UPDATE_VERSION_ROLLBACK )
    {
        OspPrintf( TRUE, FALSE, "[main]BrdSetSysRunSuccess() Update failed, rollback version!\n" );
    }
    else if ( byRet == SET_UPDATE_FLAG_FAILED )
    {
        OspPrintf( TRUE, FALSE, "[main]BrdSetSysRunSuccess() Setting failed!\n" );
    }
	else
	{
		OspPrintf(TRUE, FALSE, "[main]BrdSetSysRunSuccess(%d)\n", byRet );
	}
#endif
    eapustart();
#ifdef _LINUX_
    //注册退出清除函数
    OspRegQuitFunc( OspDemoQuitFunc );
#endif
    while( !g_bQuit )
    {
        OspDelay( 1000 );
    }   
    printf( "[main] Quit normally!\n" );
    return 0;
}    


#ifdef _LINUX_
/////////////////////////////////////////////////////Linux 下异常捕获处理

s32 StartCaptureException()
{
    return init_nipdebug( 0, NULL, DIR_EXCLOG );
}

#endif // _LINUX


API void peapumsg(void)
{
    g_cBrdAgentApp.m_bPrintBrdLog = TRUE;
}

API void npeapumsg( void )
{
    g_cBrdAgentApp.m_bPrintBrdLog = FALSE;
}


//注册Linux调试接口
void BrdEapuAPIEnableInLinux(void)
{
	/*lint -save -e611*/
#ifdef _LINUX_  
    OspRegCommand("eapustart",       (void*)eapustart,        "eapustart");
    OspRegCommand("peapumsg",        (void*)peapumsg,         "peapumsg");
    OspRegCommand("npeapumsg",        (void*)npeapumsg,        "npeapumsg");
#endif
	/*lint -restore*/
}


/////////////////////////////////////////////////////异常捕获处理
