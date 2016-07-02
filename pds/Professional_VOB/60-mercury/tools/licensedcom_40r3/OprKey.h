// OprKey.h : Declaration of the COprKey

#ifndef __OPRKEY_H_
#define __OPRKEY_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// COprKey
class ATL_NO_VTABLE COprKey : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COprKey, &CLSID_OprKey>,
	public IDispatchImpl<IOprKey, &IID_IOprKey, &LIBID_MCULICENSELib>
{
public:
	COprKey()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_OPRKEY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(COprKey)
	COM_INTERFACE_ENTRY(IOprKey)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IOprKey
public:
	STDMETHOD(WriteKey)(/*[in]*/ BSTR bsAuthCode, /*[in]*/ BSTR bsLicenseKey, /*[in]*/ BSTR bsLicenseContent,  /*[out, retval]*/  BSTR *pbsRetId);
};

#endif //__OPRKEY_H_
