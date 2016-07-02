/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Oct 20 16:28:15 2006
 */
/* Compiler settings for .\mculicense.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __mculicense_h__
#define __mculicense_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IOprKey_FWD_DEFINED__
#define __IOprKey_FWD_DEFINED__
typedef interface IOprKey IOprKey;
#endif 	/* __IOprKey_FWD_DEFINED__ */


#ifndef __OprKey_FWD_DEFINED__
#define __OprKey_FWD_DEFINED__

#ifdef __cplusplus
typedef class OprKey OprKey;
#else
typedef struct OprKey OprKey;
#endif /* __cplusplus */

#endif 	/* __OprKey_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IOprKey_INTERFACE_DEFINED__
#define __IOprKey_INTERFACE_DEFINED__

/* interface IOprKey */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOprKey;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B2C0C385-31E4-44B1-BD82-95D642B4DB94")
    IOprKey : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteKey( 
            /* [in] */ BSTR bsAuthCode,
            /* [in] */ BSTR bsLicenseKey,
            /* [in] */ BSTR bsLicenseContent,
            /* [retval][out] */ BSTR __RPC_FAR *pbsRetId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOprKeyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOprKey __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOprKey __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOprKey __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOprKey __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOprKey __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOprKey __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOprKey __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteKey )( 
            IOprKey __RPC_FAR * This,
            /* [in] */ BSTR bsAuthCode,
            /* [in] */ BSTR bsLicenseKey,
            /* [in] */ BSTR bsLicenseContent,
            /* [retval][out] */ BSTR __RPC_FAR *pbsRetId);
        
        END_INTERFACE
    } IOprKeyVtbl;

    interface IOprKey
    {
        CONST_VTBL struct IOprKeyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOprKey_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOprKey_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOprKey_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOprKey_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOprKey_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOprKey_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOprKey_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOprKey_WriteKey(This,bsAuthCode,bsLicenseKey,bsLicenseContent,pbsRetId)	\
    (This)->lpVtbl -> WriteKey(This,bsAuthCode,bsLicenseKey,bsLicenseContent,pbsRetId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOprKey_WriteKey_Proxy( 
    IOprKey __RPC_FAR * This,
    /* [in] */ BSTR bsAuthCode,
    /* [in] */ BSTR bsLicenseKey,
    /* [in] */ BSTR bsLicenseContent,
    /* [retval][out] */ BSTR __RPC_FAR *pbsRetId);


void __RPC_STUB IOprKey_WriteKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOprKey_INTERFACE_DEFINED__ */



#ifndef __MCULICENSELib_LIBRARY_DEFINED__
#define __MCULICENSELib_LIBRARY_DEFINED__

/* library MCULICENSELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MCULICENSELib;

EXTERN_C const CLSID CLSID_OprKey;

#ifdef __cplusplus

class DECLSPEC_UUID("6E7700B3-802D-4354-B999-4B24FFF0E40F")
OprKey;
#endif
#endif /* __MCULICENSELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
