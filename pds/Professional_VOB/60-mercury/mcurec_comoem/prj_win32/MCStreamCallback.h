// MCStreamCallback.h : Declaration of the CMCStreamCallback

#ifndef __MCSTREAMCALLBACK_H_
#define __MCSTREAMCALLBACK_H_

#include "resource.h"       // main symbols
#include "mcuCP.h"

/////////////////////////////////////////////////////////////////////////////
// CMCStreamCallback
class ATL_NO_VTABLE CMCStreamCallback : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMCStreamCallback, &CLSID_MCStreamCallback>,
	public IConnectionPointContainerImpl<CMCStreamCallback>,
	public IDispatchImpl<IMCStreamCallback, &IID_IMCStreamCallback, &LIBID_MCULib>,
	public CProxy_IMCStreamCallbackEvents< CMCStreamCallback >
{
public:
	CMCStreamCallback();

DECLARE_REGISTRY_RESOURCEID(IDR_MCSTREAMCALLBACK)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMCStreamCallback)
	COM_INTERFACE_ENTRY(IMCStreamCallback)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CMCStreamCallback)
CONNECTION_POINT_ENTRY(DIID__IMCStreamCallbackEvents)
END_CONNECTION_POINT_MAP()


// IMCStreamCallback
public:
};

#endif //__MCSTREAMCALLBACK_H_
