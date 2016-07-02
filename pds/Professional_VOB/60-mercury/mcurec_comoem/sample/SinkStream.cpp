// SinkStream.cpp: implementation of the CSinkStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sample.h"
#include "SinkStream.h"
#include "recinterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSinkStream::CSinkStream()
{

}

CSinkStream::~CSinkStream()
{

}

STDMETHODIMP CSinkStream::QueryInterface(const struct _GUID &iid,void ** ppv)
{
    *ppv=this;
    return S_OK;
}

ULONG __stdcall CSinkStream::AddRef(void)
{	return 1;	}	// 做个假的就可以，因为反正这个对象在程序结束前是不会退出的

ULONG __stdcall CSinkStream::Release(void)
{	return 0;	}	// 做个假的就可以，因为反正这个对象在程序结束前是不会退出的

STDMETHODIMP CSinkStream::GetTypeInfoCount(unsigned int *)
{	return E_NOTIMPL;	}	// 不用实现，反正也不用

STDMETHODIMP CSinkStream::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
{	return E_NOTIMPL;	}	// 不用实现，反正也不用

STDMETHODIMP CSinkStream::GetIDsOfNames(const struct _GUID &,unsigned short ** ,unsigned int,unsigned long,long *)
{	return E_NOTIMPL;	}	// 不用实现，反正也不用

STDMETHODIMP CSinkStream::Invoke(
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
            TStreamProperty *ptStream = (TStreamProperty*)pParams->rgvarg[0].lVal;
            /*
            CString cstr;
            cstr.Format("收到码流: (%u, %u, %u)回调！\n", 
                        ptStream->GetConfIndex(),
                        ptStream->GetStreamIndex(),
                        ptStream->GetFrameHead()->GetTStamp());

            printf(cstr);
            //AfxMessageBox(cstr);
            */
            ::SendMessage(g_hMain, WM_STREAMUPDATE, (WPARAM)ptStream, NULL);
            
            break;
        }
    case 2:
        
        break;
    default:
        break;
    }
    return S_OK;
}