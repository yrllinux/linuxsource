// MCCtrl.cpp : Implementation of CMCCtrl
#include "stdafx.h"
#include "Mcu.h"
#include "MCCtrl.h"
#include "mcu2recitf.h"
#include "ConfData.h"

/////////////////////////////////////////////////////////////////////////////
// CMCCtrl


STDMETHODIMP CMCCtrl::Init(long ptCfgInfo)
{
	// TODO: Add your implementation code here
    RecComLog(LOGLV_WARN, "CMCCtrl::Init()\n");

    if (g_cConfData.IsInited())
    {
        return E_UNEXPECTED;
    }

    if ( !IsOspInitd() )
    {
#ifdef _DEBUG
        OspInit(TRUE);
#else
        OspInit(FALSE);
#endif
    }
    OspRegistModule("mcu.dll");
    
    // 保存参数
    TRecCfgInfo *ptCfg = (TRecCfgInfo*)ptCfgInfo;
    if (NULL == ptCfg)
    {
        return E_POINTER;
    }

    // guzh [11/15/2007] 
    ptCfg->SetEqpId(17);
    g_cConfData.SetConfig(ptCfg);

    // 初始化录像库
    BOOL bInitOk = FALSE;   
    u8 byErr;
    bInitOk = RecInit(*ptCfg, byErr);
    if (!bInitOk)
    {
        g_cConfData.SetLastError(byErr);
        return E_FAIL;
    }

    // 安装回调
    SetRecStartCB(CBRecStart);
    SetRecStopCB(CBRecStop);
    SetRecPauseCB(CBRecPause);
    SetRecResumeCB(CBRecResume);
    SetRecStreamCB(CBRecStream);
    
    g_cConfData.SetInited();

    RecComLog(LOGLV_EXC, "CMCCtrl::Init OK!\n");
    
    return S_OK;
}

STDMETHODIMP CMCCtrl::Term()
{
	// TODO: Add your implementation code here
    if (!g_cConfData.IsInited())
    {
        return S_OK;//E_UNEXPECTED;
    }
    
    u8 byErr;
    BOOL bRet = RecStop(byErr);

    if ( IsOspInitd() )
    {
        OspQuit();
    }

    if (bRet)
    {
        return S_OK;
    }
    else
    {
        g_cConfData.SetLastError(byErr);
        return E_FAIL;
    }	
}

STDMETHODIMP CMCCtrl::ReconfigMcuIpAddr(unsigned long dwNewIpAddr)
{
    RecComLog(LOGLV_WARN, "CMCCtrl::ReconfigMcuIpAddr(0x%x)\n", dwNewIpAddr);
	// TODO: Add your implementation code here
    if (!g_cConfData.IsInited())
    {
        return E_UNEXPECTED;
    }

    TRecCfgInfo tCfg;
    g_cConfData.GetConfig(&tCfg);
    tCfg.SetMcuIp(dwNewIpAddr);
    g_cConfData.SetConfig(&tCfg);

    u8 byErr;
    u8 byNeedRestart;
    BOOL bRet = RecModifyCfg(tCfg, byErr, byNeedRestart);
    if (bRet)
    {
        return S_OK;
    }
    else
    {
        g_cConfData.SetLastError(byErr);
        return E_FAIL;        
    }
}

STDMETHODIMP CMCCtrl::ReconfigMcuIpPort(unsigned short wNewIpPort)
{
    RecComLog(LOGLV_WARN, "CMCCtrl::ReconfigMcuIpPort(%d)\n", wNewIpPort);
    // TODO: Add your implementation code here
    if (!g_cConfData.IsInited())
    {
        return E_UNEXPECTED;
    }
    
    TRecCfgInfo tCfg;
    g_cConfData.GetConfig(&tCfg);
    tCfg.SetMcuPort(wNewIpPort);
    g_cConfData.SetConfig(&tCfg);
    
    u8 byErr;
    u8 byNeedRestart;
    BOOL bRet = RecModifyCfg(tCfg, byErr, byNeedRestart);
    if (bRet)
    {
        return S_OK;
    }
    else
    {
        g_cConfData.SetLastError(byErr);
        return E_FAIL;        
    }
}

STDMETHODIMP CMCCtrl::ReconfigRecId(unsigned char byNewId)
{
    RecComLog(LOGLV_WARN, "CMCCtrl::ReconfigRecId(%d)\n", byNewId);
    // TODO: Add your implementation code here
    if (!g_cConfData.IsInited())
    {
        return E_UNEXPECTED;
    }
    
    TRecCfgInfo tCfg;
    g_cConfData.GetConfig(&tCfg);
    tCfg.SetEqpId(byNewId);
    g_cConfData.SetConfig(&tCfg);
    
    u8 byErr;
    u8 byNeedRestart;
    BOOL bRet = RecModifyCfg(tCfg, byErr, byNeedRestart);
    if (bRet)
    {
        return S_OK;
    }
    else
    {
        g_cConfData.SetLastError(byErr);
        return E_FAIL;        
    }
}

STDMETHODIMP CMCCtrl::GetLastError(long ptMcuStatus)
{
    RecComLog(LOGLV_WARN, "CMCCtrl::GetLastError()\n");
    // TODO: Add your implementation code here
    if (ptMcuStatus == NULL)
    {
        return E_POINTER;
    }

    TRecConfStatus *ptStatus = (TRecConfStatus*)ptMcuStatus;
    ptStatus->SetRecState( g_cConfData.IsInited() ? emRecStateRecording : emRecStateStop );
    ptStatus->SetIndex(0);
    ptStatus->SetErrCode(g_cConfData.GetLastError());

    g_cConfData.SetLastError(emRecSucceed);
        
    return S_OK;
}