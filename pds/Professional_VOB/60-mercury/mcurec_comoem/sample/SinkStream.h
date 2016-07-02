// SinkStream.h: interface for the CSinkStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINKSTREAM_H__BF867262_BC47_4040_AF46_67C88D27AA7F__INCLUDED_)
#define AFX_SINKSTREAM_H__BF867262_BC47_4040_AF46_67C88D27AA7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSinkStream : public IDispatch  
{
public:
	CSinkStream();
	virtual ~CSinkStream();

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
    
    STDMETHODIMP GetTypeInfoCount(unsigned int *);
    STDMETHODIMP GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** );
    STDMETHODIMP GetIDsOfNames(const struct _GUID &,unsigned short ** ,unsigned int,unsigned long,long *);
    
};

#endif // !defined(AFX_SINKSTREAM_H__BF867262_BC47_4040_AF46_67C88D27AA7F__INCLUDED_)
