#include "watchdogcomm.h"
#include "watchdogssn.h"
#include "watchdog.h"
#ifdef WIN32
#include "exceptioncatch.h"
#endif
#include "commonlib.h"

extern CWDClientSsnAPP  g_cWDClientSsn;
extern BOOL32 g_bDebug;
BOOL32 g_bIsStart;
SEMHANDLE  g_semWDC;
//extern CLogManager g_cWdcLogMgr;

//add by zhanghb 091117 for kdvp
BOOL g_bIsCarrierApp;
//end add

API void turnondebug()
{
    g_bDebug = TRUE;
}

API void turnoff()
{
    g_bDebug = FALSE;
}

API void pms()
{
	g_cWDClientSsn.WDPrint();
}

API void startexceptcat(TMType emSER_type )
{
#ifdef WIN32
	u8 achFileName[128] = {0};
	switch(emSER_type)
	{
	case em_INVALIDMODULE:
		break;
	case em_TS:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","TS460TS");
		break;
	case em_GK:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","TSGK");
		break;
	case em_PROXY:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","TSPxy");
		break;
	case em_MMP:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","TSMmp");
		break;
	case em_MCU:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","TSMcu");
		break;
	case em_MP:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","MP");
		break;
	case em_MTADP:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","MTADP");
		break;
	case em_PRS:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","PRS");
		break;
	case em_TUI:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","TUI");
		break;
	case em_DSS:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","DSS");
		break;
	case em_MPS:
		sprintf((char*)achFileName,(const char*)"./%sExcepTion.txt","MPS");
		break;
	default:
		break;
	}
	CExceptionCatch::Start();
	CExceptionCatch::LogToFile((char*)achFileName);
	
#endif
}

BOOL32 WDStart(TMType emModuletype,BOOL bIsCarrier )
{
#ifdef _LINUX_

    OspRegCommand("turnondebug",             (void*)turnondebug,  "Print WatchDog Client Msg");
    OspRegCommand("turnoff",            (void*)turnoff, "Not Print WatchDog Client Msg");
    OspRegCommand("pms",       (void*)pms, "Print WatchDog Module State");
#endif
	if (g_bIsStart)
	{
		return FALSE;
	}
	g_bIsStart =TRUE;

    if ( ! IsOspInitd() )
	{
#if defined(WIN32) && !defined(_DEBUG)
     OspInit(FALSE);
#else          
     OspInit(TRUE);            
#endif	
	}

    
	
	TMType emSER_type = emModuletype;
    if ( (emSER_type == em_MCU) ||
		 (emSER_type == em_TS ) ||
		 (emSER_type == em_GK ))
    {
        g_semWDC = NULL;
        if( !OspSemBCreate(&g_semWDC) )
        {
            g_semWDC = NULL;
            printf("[WDC] Create SemBCreate fail\n");
            return FALSE;
        }
        
	    OspSemTake(g_semWDC);
    }

    // 目前只在MCU处添加日志, zgc, 2009-12-02
    if ( emSER_type == em_MCU )
    {
//         g_cWdcLogMgr.SetFileTitle( "mcuwdc" );
//         g_cWdcLogMgr.SetDirPath( DIR_LOG );
//         g_cWdcLogMgr.SetLogPrefix( "wdc" );
    }
	g_cWDClientSsn.CreateApp("WDClient",WD_CLT_APP_ID,80, 512, 20<<10);
	g_cWDClientSsn.InitModule(emSER_type);
	startexceptcat(emSER_type);
	OspPost( MAKEIID(WD_CLT_APP_ID, 1), OSP_POWERON );
	
    // 等待，直到向guard server取到license, zgc, 2009-04-30
    if ( emSER_type == em_MCU )
    {
        OspSemTake(g_semWDC);
        OspSemGive(g_semWDC);
    }
	//add by zhang 091117
	g_bIsCarrierApp = bIsCarrier;
	//end add 
	return TRUE;
}
void WDStop()
{
	OspQuit();
#ifdef WIN32
	CExceptionCatch::Free();
#endif
}
//void WDSetModuelState(TMState emState)
//{
//}

