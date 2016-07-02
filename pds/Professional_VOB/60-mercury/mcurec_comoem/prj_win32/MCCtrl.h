// MCCtrl.h : Declaration of the CMCCtrl

#ifndef __MCCTRL_H_
#define __MCCTRL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMCCtrl
class ATL_NO_VTABLE CMCCtrl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMCCtrl, &CLSID_MCCtrl>,
	public IDispatchImpl<IMCCtrl, &IID_IMCCtrl, &LIBID_MCULib>
{
public:
	CMCCtrl()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MCCTRL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMCCtrl)
	COM_INTERFACE_ENTRY(IMCCtrl)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IMCCtrl
public:
    STDMETHOD(Init)(/*[in]*/ long ptCfgInfo);	
	STDMETHOD(Term)();	
    STDMETHOD(ReconfigMcuIpAddr)(/*[in]*/ unsigned long dwNewIpAddr);
    STDMETHOD(ReconfigMcuIpPort)(/*[in]*/ unsigned short wNewIpPort);
    STDMETHOD(ReconfigRecId)(/*[in]*/ unsigned char byNewId);
    STDMETHOD(GetLastError)(/*[in]*/ long ptMcuStatus);	    
};

#endif //__MCCTRL_H_
