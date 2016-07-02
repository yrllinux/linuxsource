// Sink.cpp: implementation of the CSink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sample.h"
#include "Sink.h"
#include "recinterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSink::CSink()
{

}

CSink::~CSink()
{

}

STDMETHODIMP CSink::QueryInterface(const struct _GUID &iid,void ** ppv)
{
    *ppv=this;
    return S_OK;
}

ULONG __stdcall CSink::AddRef(void)
{	return 1;	}	// 做个假的就可以，因为反正这个对象在程序结束前是不会退出的

ULONG __stdcall CSink::Release(void)
{	return 0;	}	// 做个假的就可以，因为反正这个对象在程序结束前是不会退出的

STDMETHODIMP CSink::GetTypeInfoCount(unsigned int *)
{	return E_NOTIMPL;	}	// 不用实现，反正也不用

STDMETHODIMP CSink::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
{	return E_NOTIMPL;	}	// 不用实现，反正也不用

STDMETHODIMP CSink::GetIDsOfNames(const struct _GUID &,unsigned short ** ,unsigned int,unsigned long,long *)
{	return E_NOTIMPL;	}	// 不用实现，反正也不用

STDMETHODIMP CSink::Invoke(
                           long dispID,
                           const struct _GUID &,
                           unsigned long,
                           unsigned short,
                           struct tagDISPPARAMS * pParams,
                           struct tagVARIANT *,
                           struct tagEXCEPINFO *,
                           unsigned int *)
{		
    // 只需要实现这个就足够啦
    switch(dispID)	// 根据不同的dispID,完成不同的回调函数
    {
    case 1:
        {
            TRecConfStatus *ptStatus = (TRecConfStatus *)pParams->rgvarg[0].lVal;
            
            DWORD dwConfIdx = ptStatus->GetIndex();
            u32   dwState  = ptStatus->GetRecState();
            ::PostMessage(g_hMain, WM_CONFUPDATE, dwState, dwConfIdx);

            break;
        }
    case 2:
        
            break;
    default:
        break;
    }
    return S_OK;
}