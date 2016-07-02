// ConfProperty.cpp : Implementation of CConfProperty
#include "stdafx.h"
#include "Mcu.h"
#include "ConfProperty.h"
#include "mcu2recitf.h"
#include "ConfData.h"

/////////////////////////////////////////////////////////////////////////////
// CConfProperty

// 处理用户对数据的请求
STDMETHODIMP CConfProperty::GetConfAttrib(DWORD dwConfIndex, long ptConfProperty)
{    
	// TODO: Add your implementation code here
    RecComLog(LOGLV_WARN, "CConfProperty::GetConfAttrib() for conf.%d\n", dwConfIndex);
    if (!g_cConfData.IsInited())
    {
        return E_UNEXPECTED;
    }
    
    TConfProperty *ptOut = (TConfProperty*)ptConfProperty;
    if ( NULL == ptOut )    
    {
        return E_POINTER;
    }

    if ( !g_cConfData.GetAt(dwConfIndex, ptOut) )
    {        
        RecComLog(LOGLV_EXC, "CConfProperty::GetConfAttrib failed for dwConfIndex: %d\n", dwConfIndex);
        return S_FALSE;
    }
    
	return S_OK;
}

STDMETHODIMP CConfProperty::GetRecAttrib(DWORD dwConfIndex, long ptRecProperty)
{
	// TODO: Add your implementation code here
    RecComLog(LOGLV_WARN, "CConfProperty::GetRecAttrib() for conf.%d\n", dwConfIndex);
    if (!g_cConfData.IsInited())
    {
        return E_UNEXPECTED;
    }
    
    TRecProperty *ptOut = (TRecProperty*)ptRecProperty;
    if ( NULL == ptOut )    
    {
        return E_POINTER;
    }
    if ( !g_cConfData.GetAt(dwConfIndex, ptOut) )
    {
        RecComLog(LOGLV_EXC, "CConfProperty::GetRecAttrib failed for dwConfIndex: %d\n", dwConfIndex);
        return S_FALSE;
    }

	return S_OK;
}
