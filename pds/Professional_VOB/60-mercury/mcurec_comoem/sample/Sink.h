// Sink.h: interface for the CSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINK_H__C4BFB506_F808_456A_8EAB_1DE8054829CB__INCLUDED_)
#define AFX_SINK_H__C4BFB506_F808_456A_8EAB_1DE8054829CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSink : public IDispatch  
{
public:
	CSink();
	virtual ~CSink();


    STDMETHODIMP QueryInterface(const struct _GUID &iid,void ** ppv);
    ULONG __stdcall AddRef(void);
    ULONG __stdcall Release(void);
    
    STDMETHODIMP Invoke(
        long dispID,
        const struct _GUID &,
        unsigned long,
        unsigned short,
        struct tagDISPPARAMS * pParams,
        struct tagVARIANT *,
        struct tagEXCEPINFO *,
        unsigned int *);

    STDMETHODIMP CSink::GetTypeInfoCount(unsigned int *);
    STDMETHODIMP CSink::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** );
    STDMETHODIMP CSink::GetIDsOfNames(const struct _GUID &,unsigned short ** ,unsigned int,unsigned long,long *);
    

};

#endif // !defined(AFX_SINK_H__C4BFB506_F808_456A_8EAB_1DE8054829CB__INCLUDED_)
