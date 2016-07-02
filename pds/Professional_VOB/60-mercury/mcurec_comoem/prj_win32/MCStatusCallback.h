// MCStatusCallback.h : Declaration of the CMCStatusCallback

#ifndef __MCSTATUSCALLBACK_H_
#define __MCSTATUSCALLBACK_H_

#include "resource.h"       // main symbols
#include "mcuCP.h"

/////////////////////////////////////////////////////////////////////////////
// CMCStatusCallback
class ATL_NO_VTABLE CMCStatusCallback : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMCStatusCallback, &CLSID_MCStatusCallback>,
	public IConnectionPointContainerImpl<CMCStatusCallback>,
	public IDispatchImpl<IMCStatusCallback, &IID_IMCStatusCallback, &LIBID_MCULib>,
	public CProxy_IMCStatusCallbackEvents< CMCStatusCallback >
{
public:
	CMCStatusCallback();

DECLARE_REGISTRY_RESOURCEID(IDR_MCSTATUSCALLBACK)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMCStatusCallback)
	COM_INTERFACE_ENTRY(IMCStatusCallback)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CMCStatusCallback)
CONNECTION_POINT_ENTRY(DIID__IMCStatusCallbackEvents)
END_CONNECTION_POINT_MAP()


// IMCStatusCallback
public:
};

#endif //__MCSTATUSCALLBACK_H_
