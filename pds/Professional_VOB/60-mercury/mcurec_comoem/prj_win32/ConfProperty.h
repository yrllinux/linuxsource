// ConfProperty.h : Declaration of the CConfProperty

#ifndef __CONFPROPERTY_H_
#define __CONFPROPERTY_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CConfProperty
class ATL_NO_VTABLE CConfProperty : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConfProperty, &CLSID_ConfProperty>,
	public IDispatchImpl<IConfProperty, &IID_IConfProperty, &LIBID_MCULib>
{
public:
	CConfProperty()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CONFPROPERTY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CConfProperty)
	COM_INTERFACE_ENTRY(IConfProperty)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IConfProperty
public:
    STDMETHOD(GetConfAttrib)(/*[in]*/ DWORD dwConfIndex, /*[out]*/ long ptConfProperty);
	STDMETHOD(GetRecAttrib)(/*[in]*/ DWORD dwConfIndex, /*[out]*/ long ptRecProperty);	
};

#endif //__CONFPROPERTY_H_
