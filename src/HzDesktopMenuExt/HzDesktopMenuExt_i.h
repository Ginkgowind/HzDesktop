

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for HzDesktopMenuExt.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __HzDesktopMenuExt_i_h__
#define __HzDesktopMenuExt_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IHzDesktopMenuObject_FWD_DEFINED__
#define __IHzDesktopMenuObject_FWD_DEFINED__
typedef interface IHzDesktopMenuObject IHzDesktopMenuObject;

#endif 	/* __IHzDesktopMenuObject_FWD_DEFINED__ */


#ifndef __HzDesktopMenuObject_FWD_DEFINED__
#define __HzDesktopMenuObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class HzDesktopMenuObject HzDesktopMenuObject;
#else
typedef struct HzDesktopMenuObject HzDesktopMenuObject;
#endif /* __cplusplus */

#endif 	/* __HzDesktopMenuObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IHzDesktopMenuObject_INTERFACE_DEFINED__
#define __IHzDesktopMenuObject_INTERFACE_DEFINED__

/* interface IHzDesktopMenuObject */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHzDesktopMenuObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4ae74c54-5fc0-452b-ae40-b0d0dbb34573")
    IHzDesktopMenuObject : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IHzDesktopMenuObjectVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHzDesktopMenuObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHzDesktopMenuObject * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHzDesktopMenuObject * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHzDesktopMenuObject * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHzDesktopMenuObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHzDesktopMenuObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHzDesktopMenuObject * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IHzDesktopMenuObjectVtbl;

    interface IHzDesktopMenuObject
    {
        CONST_VTBL struct IHzDesktopMenuObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHzDesktopMenuObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHzDesktopMenuObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHzDesktopMenuObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHzDesktopMenuObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHzDesktopMenuObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHzDesktopMenuObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHzDesktopMenuObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHzDesktopMenuObject_INTERFACE_DEFINED__ */



#ifndef __HzDesktopMenuExtLib_LIBRARY_DEFINED__
#define __HzDesktopMenuExtLib_LIBRARY_DEFINED__

/* library HzDesktopMenuExtLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_HzDesktopMenuExtLib;

EXTERN_C const CLSID CLSID_HzDesktopMenuObject;

#ifdef __cplusplus

class DECLSPEC_UUID("94e8f84e-15de-4897-9100-b4aecc8e8861")
HzDesktopMenuObject;
#endif
#endif /* __HzDesktopMenuExtLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


