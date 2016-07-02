/*****************************************************************************
模块名      : watchdogssn
文件名      : watchssn.h
创建时间    : 2009年 02月 23日
实现功能    : 
作者        : 张洪彬
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/02/23  1.0         张洪彬        创建
******************************************************************************/
#include "watchdogssn.h"
#include "commonlib.h"

CWDClientSsnAPP  g_cWDClientSsn;
BOOL32 g_bDebug;
CLogManager g_cWdcLogMgr;

extern SEMHANDLE  g_semWDC;
extern BOOL g_bIsCarrierApp;

void WatchDogPrint( s8 * pszFmt, ... )
{
    if (g_bDebug)
    {
	    s8 achPrintBuf[255];

        s32 nBufLen;
        va_list argptr;
	    s32 nLen = sprintf( achPrintBuf, "[WDClnt]:" );
        va_start( argptr, pszFmt );    
        nBufLen = vsprintf( achPrintBuf + nLen, pszFmt, argptr );   
        OspPrintf( nLen, FALSE, achPrintBuf );
        va_end(argptr); 	
    }
}
//////////////////////////////////////////////////////////////////////////////
CWatchDogClientSsnInst::CWatchDogClientSsnInst()
{
	g_bDebug = FALSE;
}

CWatchDogClientSsnInst::~CWatchDogClientSsnInst()
{

}

void CWatchDogClientSsnInst::DaemonInstanceEntry(CMessage *const pMsg, CApp *pcApp)
{
	switch(pMsg->event)
	{
	case WD_APP_WDC_REG_REQ:
		DaemProcAPPREG( pMsg, pcApp);
		break;
	case EV_CHECKAPP_HB_REQ:
		DaemPingEachAPP();
		break;
	case WD_APP_WDC_HB_ACK:
		DaemProcHBACK(*(u8*)pMsg->content);
		break;
	case EV_CHECKAPP_HB_OUT:
		DaemProcHBTOUT();		
		break;
	case WD_MCUGD_WDC_REBOOT_CMD:
		if (em_MCU == g_cWDClientSsn.m_tModuleParameter.emType ||
			em_MPS == g_cWDClientSsn.m_tModuleParameter.emType || 
			em_TUI == g_cWDClientSsn.m_tModuleParameter.emType )
		{
			::OspPost(g_cWDClientSsn.m_dwServIID,WD_MCUCLI_GUARD_REBOOT_REQ,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID, DAEMON));
		}
		break;
	case WD_MCUGD_WDC_SHUTDOWN_CMD:
		if (em_MCU == g_cWDClientSsn.m_tModuleParameter.emType ||
			em_MPS == g_cWDClientSsn.m_tModuleParameter.emType || 
			em_TUI == g_cWDClientSsn.m_tModuleParameter.emType )
		{
			::OspPost(g_cWDClientSsn.m_dwServIID,WD_MCUCLI_GUARD_SHUTDOWN_REQ,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID, DAEMON));
		}
		break;
	case WD_MCUGD_WDC_SOFTREBOOT_CMD:
		if (em_MCU == g_cWDClientSsn.m_tModuleParameter.emType ||
			em_MPS == g_cWDClientSsn.m_tModuleParameter.emType || 
			em_TUI == g_cWDClientSsn.m_tModuleParameter.emType)
		{
			::OspPost(g_cWDClientSsn.m_dwServIID,WD_MCUCLI_GUARD_SOFTRESTART_REQ,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID, DAEMON));
		}
	case WD_MCUGD_WDC_GETKEY_REQ:
		OspPost(pMsg->srcid,WD_WDC_MCUGD_GETKEY_ACK,&g_cWDClientSsn.m_tLicenseInfo,sizeof(TLicenseMsg),0,MAKEIID(WD_CLT_APP_ID,DAEMON));			
		break;
	case WD_MCUGD_WDC_UPDATE_CMD:
		if (em_MCU == g_cWDClientSsn.m_tModuleParameter.emType)
		{
			OspPost(g_cWDClientSsn.m_dwServIID,WD_MCUCLI_GUARD_UPDATE_REQ,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID, DAEMON));
		}
		break;
	case WD_MCUGD_WDC_GETMDINFO_REQ:
		if (em_MCU == g_cWDClientSsn.m_tModuleParameter.emType)
		{
			OspPost(pMsg->srcid,WD_WDC_MCUGD_GETMDINFO_ACK,0,0,0,MAKEIID(WD_CLT_APP_ID, DAEMON));
			TOthersMState tOthersMState;
			memcpy(&tOthersMState,g_cWDClientSsn.m_achOtherMdInfo,sizeof(g_cWDClientSsn.m_achOtherMdInfo));
			OspPost(pMsg->srcid,WD_WDC_MCUGD_GETMDINFO_NOTIFY,&tOthersMState,sizeof(TOthersMState),0,MAKEIID(WD_CLT_APP_ID,DAEMON));
		}
		break;
	case WD_SER_CLI_GETOTERSTATE_NOTFY:
		if (em_MCU == g_cWDClientSsn.m_tModuleParameter.emType)
		{
			TOthersMState tOthersMState;
			memcpy(&tOthersMState,g_cWDClientSsn.m_achOtherMdInfo,sizeof(g_cWDClientSsn.m_achOtherMdInfo));
			OspPost(g_cWDClientSsn.m_tCheckAppData.m_adwAPPNodeIID[em_MCU_GUARD_APP - 1] ,
				    WD_WDC_MCUGD_GETMDINFO_NOTIFY,
					&tOthersMState,sizeof(TOthersMState),0,
 					MAKEIID(WD_CLT_APP_ID,DAEMON));
		}
		break;
	default:
		break;
	}
}
void CWatchDogClientSsnInst::DaemProcAPPREG(CMessage *const pMsg, CApp *pcApp)
{
	OspSemTake(g_cWDClientSsn.m_hCheckAPPSem);
	u32* pbyAppBite = (u32*)pMsg->content;
	g_cWDClientSsn.m_tCheckAppData.AddAPPReg(*pbyAppBite);
	g_cWDClientSsn.m_tCheckAppData.AddAPPNodeInfo(*pbyAppBite,pMsg->srcnode,pMsg->srcid);
	OspPost(pMsg->srcid,WD_WDC_APP_REG_ACK,0,0,0,MAKEIID(WD_CLT_APP_ID,DAEMON));
	if (g_cWDClientSsn.m_tCheckAppData.IsRegOK())
	{
		SetTimer(EV_CHECKAPP_HB_REQ,TIMER_CHECK_OTHERAPP);
	}
	OspSemGive(g_cWDClientSsn.m_hCheckAPPSem);
	return;
}
void CWatchDogClientSsnInst::DaemPingEachAPP()
{
	OspSemTake(g_cWDClientSsn.m_hCheckAPPSem);
	for (u8 Index =0; Index < 8;Index++)
	{
		if ((g_cWDClientSsn.m_tCheckAppData.m_adwAPPNodeIID[Index] != INVALID_INS) 
			&& (g_cWDClientSsn.m_tCheckAppData.m_adwAPPNodeIID[Index] != 0))
		{
			post(g_cWDClientSsn.m_tCheckAppData.m_adwAPPNodeIID[Index],WD_WDC_APP_HB_REQ,0,0);
		}
	}
	SetTimer(EV_CHECKAPP_HB_OUT,TIMER_CHECK_OTHERAPP_OUT);
	OspSemGive(g_cWDClientSsn.m_hCheckAPPSem);
	return;
}
void CWatchDogClientSsnInst::DaemProcHBACK(u8 byAPPBite)
{
	OspSemTake(g_cWDClientSsn.m_hCheckAPPSem);
	g_cWDClientSsn.m_tCheckAppData.ADDAPPState(byAPPBite);
	if (g_cWDClientSsn.m_tCheckAppData.IsAPPStateNormal())
	{
		g_cWDClientSsn.m_tCheckAppData.SetStateZero();
		KillTimer(EV_CHECKAPP_HB_OUT);
		SetTimer(EV_CHECKAPP_HB_REQ,TIMER_CHECK_OTHERAPP);
	}
	OspSemGive(g_cWDClientSsn.m_hCheckAPPSem);
	return;
}
void CWatchDogClientSsnInst::DaemProcHBTOUT()
{
	OspSemTake(g_cWDClientSsn.m_hCheckAPPSem);
	g_cWDClientSsn.m_tCheckAppData.AddTimeOut();
	u8 byTimeOutApp;
	if(g_cWDClientSsn.m_tCheckAppData.IsTimeOut(&byTimeOutApp))
	{
		g_cWDClientSsn.m_tCheckAppData.SetStateZero();
		//向guard发送线程阻塞消息
		post(g_cWDClientSsn.m_dwServIID,WD_CLI_SERV_APPBLOCK,0,0,g_cWDClientSsn.m_dwServNode);
	}
	SetTimer(EV_CHECKAPP_HB_REQ,TIMER_CHECK_OTHERAPP);
	OspSemGive(g_cWDClientSsn.m_hCheckAPPSem);
	return;

}
void CWatchDogClientSsnInst::InstanceEntry(CMessage *const pcMsg)
{
	switch(pcMsg->event)
	{
	case OSP_POWERON:
		//启动 设置timer
		SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_CONNECT_POWERON );
		break;
	case OSP_DISCONNECT:
		//重连 timer超时
		{
			u32 dwDisconNode = *(u32*)pcMsg->content;

            // 只有MCU需要记录日志, zgc, 2009-12-02
            if ( g_cWDClientSsn.m_tModuleParameter.emType == em_MCU )
            {
                g_cWdcLogMgr.InsertLogItem( "Receive OSP_DISCONNECT, remote node = %d!\n", dwDisconNode );
                g_cWdcLogMgr.WriteLogInfo();
                g_cWdcLogMgr.ClearLogTable();
            }
			if (INVALID_NODE != dwDisconNode)
			{
				WatchDogPrint("WDClient disconnected with serv,it will reconnect after 3 seconds !\n");
				OspDisconnectTcpNode(dwDisconNode);
				g_cWDClientSsn.m_dwServNode = INVALID_NODE;
			}
			SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_CONNECT_DISC );
		}
		break;
	case EV_CONNECTWDSERV_TIMER:
		//链接服务器端
		ProcConnectWDServ();
		break;
	case WD_SER_CLI_MODULECON_ACK:
	case EV_SER_CLI_MODULECON_NACK:
		//连接回应
		ProcConnectWDServReply(pcMsg);
		break;
	case EV_CONNECTWDSERV_TIMEOUT:
		KillTimer(EV_CONNECTWDSERV_TIMEOUT);
		if (INVALID_NODE != g_cWDClientSsn.m_dwServNode)
		{
			//断链
			OspDisconnectTcpNode(g_cWDClientSsn.m_dwServNode);
		}
		break;
	case WD_SER_CLI_HB_REQ:
		//回复ack，上报状态。
		ProcHBInfo();
		break;
	case WD_CLI_GUARD_QUIT:
		ProcWDSQuitMsg();
		break;
	case WD_SER_CLI_REPORTCLISTATE_ACK:
	case WD_SER_CLI_REPORTCLISTATE_NACK:
		//nothing
		break;
	case WD_GAURD_MCUCLI_GETKEY_ACK:
		PrcoMcuGetKeyAck(pcMsg);
		break;
	case WD_SER_CLI_GETOTERSTATE_ACK:
	case WD_SER_CLI_GETOTERSTATE_NOTFY:
		ProcMcuGetOtherMDACK(pcMsg);
		break;
	default:
		break;
	}
	return;
}
void CWatchDogClientSsnInst::ProcMcuGetOtherMDACK(CMessage *const pcMsg)
{
	if (em_MCU != g_cWDClientSsn.m_tModuleParameter.emType)
	{
		return;
	}
	//TModuleParameter* ptOtherMDInfo = (TModuleParameter*)pcMsg->content;
	if (pcMsg->event == WD_SER_CLI_GETOTERSTATE_NOTFY )
	{
		for (u8 loop =0;loop<WD_MODULE_NUM;loop++)
		{
			g_cWDClientSsn.m_achOtherMdInfo[loop].emState = em_STOP;
			g_cWDClientSsn.m_achOtherMdInfo[loop].emType = em_INVALIDMODULE;
		}
		memcpy(&g_cWDClientSsn.m_achOtherMdInfo,(TModuleParameter*)pcMsg->content,pcMsg->length);
		OspPost(MAKEIID(WD_CLT_APP_ID, DAEMON),pcMsg->event,0,0);
	}
	return;
}
void CWatchDogClientSsnInst::PrcoMcuGetKeyAck(CMessage *const pcMsg)
{
	TLicenseMsg* ptLiceninfo = (TLicenseMsg*)pcMsg->content;
	memcpy(&g_cWDClientSsn.m_tLicenseInfo,ptLiceninfo,sizeof(TLicenseMsg));
    
    // 取得license后释放信号量, zgc, 2009-04-30
    OspSemGive(g_semWDC);
	return;
}
void CWatchDogClientSsnInst::ProcWDSQuitMsg()
{
	OspPrintf(TRUE,FALSE,"Recv the SrvQuitMsg!\n");
	OspQuit();
	if (g_bIsCarrierApp)
	{
		switch(g_cWDClientSsn.m_tModuleParameter.emType)
		{
		case em_TS:
			KillPByName((const u8*)MD_NAME_CARRIER_KDVTS);
			break;
		case em_GK:
			KillPByName((const u8*)MD_NAME_8000E_GK);			
			break;
		case em_PROXY:
			KillPByName((const u8*)MD_NAME_8000E_PXY);
			break;
		case em_MMP:
			KillPByName((const u8*)MD_NAME_8000E_MMP);
			break;
		case em_MCU:
			KillPByName((const u8*)MD_NAME_CARRIER_MCU);
			break;
		case em_MP:
			KillPByName((const u8*)MD_NAME_CARRIER_MP);
			break;
		case em_MTADP:
			KillPByName((const u8*)MD_NAME_CARRIER_MTADP);
			break;
		case em_PRS:
			KillPByName((const u8*)MD_NAME_CARRIER_PRS);
			break;
		case em_TUI:
			KillPByName((const u8*)MD_NAME_CARRIER_TUI);
			break;
		case em_DSS:
			KillPByName((const u8*)MD_NAME_CARRIER_DSSERVER);
			break;
		case em_MPS:
			KillPByName((const u8*)MD_NAME_CARRIER_MPS);
			break;
		default:
			break;
		}
	}
	else
	{
		switch(g_cWDClientSsn.m_tModuleParameter.emType)
		{
		case em_TS:
			KillPByName((const u8*)MD_NAME_8000E_TS);
			break;
		case em_GK:
			KillPByName((const u8*)MD_NAME_8000E_GK);			
			break;
		case em_PROXY:
			KillPByName((const u8*)MD_NAME_8000E_PXY);
			break;
		case em_MMP:
			KillPByName((const u8*)MD_NAME_8000E_MMP);
			break;
		case em_MCU:
			KillPByName((const u8*)MD_NAME_8000E_MCU);
			break;
		case em_DSS:
			KillPByName((const u8*)MD_NAME_8000E_DSSERVER);
			break;
		case em_MPS:
			KillPByName((const u8*)MD_NAME_KDVE_MPS);
			break;
		default:
			break;
		}
	}

	return;
}
void CWatchDogClientSsnInst::KillPByName(const u8* pbyPName)
{
#ifdef _LINUX_
	FILE *fp;
	char buffer[200], cmd[20];
	char pid[20], temp[20], proc[20];

	if ((fp = popen("ps -e", "r")) != NULL)
	{
		fgets(buffer, 128, fp);
		while (fgets(buffer, 128, fp))
		{
			sscanf(buffer, "%s%s%s%s", pid, temp, temp, proc);
			if (!strcmp(proc, (const char*)pbyPName))
			{
				sprintf(cmd, "kill -9 %s", pid);
				//puts(cmd);
				system(cmd);
				break;
			}
		}
		pclose(fp);
	}
#else
	void   *hSnapshot   =   CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,   0);  
	PROCESSENTRY32   ppe;  
	ppe.dwSize   =   sizeof(PROCESSENTRY32);  
	Process32First(hSnapshot,   &ppe);  
	do  
	{  
	  if   (strcmp(ppe.szExeFile,   (const char*)pbyPName)   ==   0)//在这里判断进程名称  
	  {  
			  void   *hProcess   =   OpenProcess(PROCESS_ALL_ACCESS,   false,   ppe.th32ProcessID);  
			  //获得进程句柄  
			  TerminateProcess(hProcess,   0);//终止该进程  
	  }  
	}  
	while   (Process32Next(hSnapshot,   &ppe));  
	CloseHandle(hSnapshot); 
	
	
#endif
//	OspDelay(500);
// 	OspQuit();
	return;
}
void CWatchDogClientSsnInst::ProcHBInfo()
{
	::OspPost(g_cWDClientSsn.m_dwServIID,WD_CLI_SER_HB_ACK,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID, 1));
	OspDelay(200);
	::OspPost(g_cWDClientSsn.m_dwServIID,
		WD_CLI_SER_REPORTCLISTATE_REQ,
		(u8*)&g_cWDClientSsn.m_tModuleParameter,
		sizeof(TModuleParameter),
		g_cWDClientSsn.m_dwServNode,
		MAKEIID(WD_CLT_APP_ID, 1));
	return;
	
}
void CWatchDogClientSsnInst::ProcConnectWDServ()
{
	WatchDogPrint("WDClient send EV_CONNECTWDSERV_TIMER msg to serv......!\n");
	if (INVALID_NODE == g_cWDClientSsn.m_dwServNode)
	{
		u32 dwSerTempNode;
		dwSerTempNode = ::OspConnectTcpNode(WD_SER_IP,WD_SER_TCP_PORT);
		if (INVALID_NODE != dwSerTempNode)
		{
			g_cWDClientSsn.m_dwServNode = dwSerTempNode;
		}
		else
		{
			SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_CONNECT_DISC );
			return;
		}
		::OspNodeDiscCBReg(g_cWDClientSsn.m_dwServNode,GetAppID(),GetInsID());
        OspSetHBParam( g_cWDClientSsn.m_dwServNode, DEF_OSPDISC_HBTIME, DEF_OSPDISC_HBNUM );
	}

    // 只有MCU需要记录日志, zgc, 2009-12-02
    if ( g_cWDClientSsn.m_tModuleParameter.emType == em_MCU )
    {
        g_cWdcLogMgr.InsertLogItem( "Send WD_CLI_SER_MODULECON_REQ to server!\n" );
        g_cWdcLogMgr.WriteLogInfo();
        g_cWdcLogMgr.ClearLogTable();
    }
	::OspPost(MAKEIID(WD_SER_APP_ID,CInstance::DAEMON),
		WD_CLI_SER_MODULECON_REQ,
		(u8*)&g_cWDClientSsn.m_tModuleParameter,
		sizeof(TModuleParameter),
		g_cWDClientSsn.m_dwServNode,
		MAKEIID(WD_CLT_APP_ID, 1));
	SetTimer(EV_CONNECTWDSERV_TIMEOUT,TIMER_CONNECT_DISC);
}
void CWatchDogClientSsnInst::ProcConnectWDServReply(CMessage *const pcMsg)
{
	switch(pcMsg->event)
	{
	case EV_SER_CLI_MODULECON_NACK:
		//暂无
		break;
	case WD_SER_CLI_MODULECON_ACK:
		KillTimer(EV_CONNECTWDSERV_TIMEOUT);
		WatchDogPrint("WDClient received EV_SC_MODULECON_ACK msg!\n");
		//WatchDogPrint("WDClient send EV_CS_SETMODULESTATE_CMD msg!\n");
		g_cWDClientSsn.m_tModuleParameter.emState =em_RUNNING;
		g_cWDClientSsn.m_dwServIID = pcMsg->srcid;
		
		//获得license信息，本地保存
		if (em_MCU == g_cWDClientSsn.m_tModuleParameter.emType)
		{
            // 只有MCU需要记录日志, zgc, 2009-12-02
            if ( g_cWDClientSsn.m_tModuleParameter.emType == em_MCU )
            {
                g_cWdcLogMgr.InsertLogItem( "Receive WD_SER_CLI_MODULECON_ACK!\n" );
                g_cWdcLogMgr.WriteLogInfo();
                g_cWdcLogMgr.ClearLogTable();
            }
			::OspPost(g_cWDClientSsn.m_dwServIID,WD_MCUCLI_GAURD_GETKEY_REQ,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID, 1));
			::OspPost(g_cWDClientSsn.m_dwServIID,WD_CLI_SER_GETOTERSTATE_REQ,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID,1));
		}
		else if ( (em_TS == g_cWDClientSsn.m_tModuleParameter.emType) ||
			      (em_GK == g_cWDClientSsn.m_tModuleParameter.emType) )
		{
			::OspPost(g_cWDClientSsn.m_dwServIID,WD_MCUCLI_GAURD_GETKEY_REQ,0,0,g_cWDClientSsn.m_dwServNode,MAKEIID(WD_CLT_APP_ID, 1));
		}
        else    // 非MCU的WDC，这里就释放信号量, zgc, 2009-04-30
        {
            OspSemGive(g_semWDC);
        }
		
		break;
	default:
		break;
	}
}
///////////////////////////////////////////////////////////////////////
BOOL32 CWatchDogClientModuleDate::InitModule(TMType emType)
{
	if (emType == em_INVALIDMODULE)
	{
		return FALSE;
	}
	m_tModuleParameter.emType = emType;
	m_tModuleParameter.emState = em_STOP;
	m_tCheckAppData.m_eMType = emType;
    if ( !OspSemBCreate( &m_hCheckAPPSem ))
    {
        OspPrintf(TRUE,FALSE,"Create CheckAAPP sem Fail!\n");
        WDStop();
    }
	return TRUE;

}
void CWatchDogClientModuleDate::WDPrint()
{
	OspPrintf(TRUE,FALSE,"WDC data info:\n");
	OspPrintf(TRUE,FALSE,"Module: %d, State: %d\n", m_tModuleParameter.emType, m_tModuleParameter.emState);
    u32 dwIdx = 0;
    OspPrintf( TRUE, FALSE, "Localinfo: ");
    for ( dwIdx = 0; dwIdx < sizeof(m_tLicenseInfo.m_achLocalInfo); dwIdx++ )
    {
        if ( dwIdx % 30 == 0 )
        {
            OspPrintf( TRUE, FALSE, "\n" );
        }
        OspPrintf( TRUE, FALSE, "%02x ", m_tLicenseInfo.m_achLocalInfo[dwIdx] );
    }
    OspPrintf( TRUE, FALSE, "\n" );
    OspPrintf( TRUE, FALSE, "\n" );
    
    OspPrintf( TRUE, FALSE, "ExpireData: ");
    for ( dwIdx = 0; dwIdx < 16; dwIdx++ )
    {
        OspPrintf( TRUE, FALSE, "%x ", m_tLicenseInfo.m_achExpireDate[dwIdx] );
    }
    OspPrintf( TRUE, FALSE, "\n" );
    
    OspPrintf( TRUE, FALSE, "LicenseSN: ");
    for ( dwIdx = 0; dwIdx < 64; dwIdx++ )
    {
        OspPrintf( TRUE, FALSE, "%x ", m_tLicenseInfo.m_achLicenseSN[dwIdx] );
    }
    OspPrintf( TRUE, FALSE, "\n" );
    
    OspPrintf( TRUE, FALSE, "AccessNum: %d\n", m_tLicenseInfo.m_wMcuAccessNum );
    OspPrintf( TRUE, FALSE, "LicenseErrorCode: %d\n", m_tLicenseInfo.m_wLicenseErrorCode );
    OspPrintf( TRUE, FALSE, "IsValid: %d\n", m_tLicenseInfo.m_bLicenseIsValid );

}
