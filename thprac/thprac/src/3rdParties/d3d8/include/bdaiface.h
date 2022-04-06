
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0338 */
/* Compiler settings for bdaiface.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
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

#ifndef __bdaiface_h__
#define __bdaiface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBDA_NetworkProvider_FWD_DEFINED__
#define __IBDA_NetworkProvider_FWD_DEFINED__
typedef interface IBDA_NetworkProvider IBDA_NetworkProvider;
#endif 	/* __IBDA_NetworkProvider_FWD_DEFINED__ */


#ifndef __IBDA_EthernetFilter_FWD_DEFINED__
#define __IBDA_EthernetFilter_FWD_DEFINED__
typedef interface IBDA_EthernetFilter IBDA_EthernetFilter;
#endif 	/* __IBDA_EthernetFilter_FWD_DEFINED__ */


#ifndef __IBDA_IPV4Filter_FWD_DEFINED__
#define __IBDA_IPV4Filter_FWD_DEFINED__
typedef interface IBDA_IPV4Filter IBDA_IPV4Filter;
#endif 	/* __IBDA_IPV4Filter_FWD_DEFINED__ */


#ifndef __IBDA_IPV6Filter_FWD_DEFINED__
#define __IBDA_IPV6Filter_FWD_DEFINED__
typedef interface IBDA_IPV6Filter IBDA_IPV6Filter;
#endif 	/* __IBDA_IPV6Filter_FWD_DEFINED__ */


#ifndef __IBDA_DeviceControl_FWD_DEFINED__
#define __IBDA_DeviceControl_FWD_DEFINED__
typedef interface IBDA_DeviceControl IBDA_DeviceControl;
#endif 	/* __IBDA_DeviceControl_FWD_DEFINED__ */


#ifndef __IBDA_PinControl_FWD_DEFINED__
#define __IBDA_PinControl_FWD_DEFINED__
typedef interface IBDA_PinControl IBDA_PinControl;
#endif 	/* __IBDA_PinControl_FWD_DEFINED__ */


#ifndef __IBDA_SignalProperties_FWD_DEFINED__
#define __IBDA_SignalProperties_FWD_DEFINED__
typedef interface IBDA_SignalProperties IBDA_SignalProperties;
#endif 	/* __IBDA_SignalProperties_FWD_DEFINED__ */


#ifndef __IBDA_Topology_FWD_DEFINED__
#define __IBDA_Topology_FWD_DEFINED__
typedef interface IBDA_Topology IBDA_Topology;
#endif 	/* __IBDA_Topology_FWD_DEFINED__ */


#ifndef __IBDA_VoidTransform_FWD_DEFINED__
#define __IBDA_VoidTransform_FWD_DEFINED__
typedef interface IBDA_VoidTransform IBDA_VoidTransform;
#endif 	/* __IBDA_VoidTransform_FWD_DEFINED__ */


#ifndef __IBDA_NullTransform_FWD_DEFINED__
#define __IBDA_NullTransform_FWD_DEFINED__
typedef interface IBDA_NullTransform IBDA_NullTransform;
#endif 	/* __IBDA_NullTransform_FWD_DEFINED__ */


#ifndef __IBDA_FrequencyFilter_FWD_DEFINED__
#define __IBDA_FrequencyFilter_FWD_DEFINED__
typedef interface IBDA_FrequencyFilter IBDA_FrequencyFilter;
#endif 	/* __IBDA_FrequencyFilter_FWD_DEFINED__ */


#ifndef __IBDA_AutoDemodulate_FWD_DEFINED__
#define __IBDA_AutoDemodulate_FWD_DEFINED__
typedef interface IBDA_AutoDemodulate IBDA_AutoDemodulate;
#endif 	/* __IBDA_AutoDemodulate_FWD_DEFINED__ */


#ifndef __IBDA_DigitalDemodulator_FWD_DEFINED__
#define __IBDA_DigitalDemodulator_FWD_DEFINED__
typedef interface IBDA_DigitalDemodulator IBDA_DigitalDemodulator;
#endif 	/* __IBDA_DigitalDemodulator_FWD_DEFINED__ */


#ifndef __IBDA_IPSinkControl_FWD_DEFINED__
#define __IBDA_IPSinkControl_FWD_DEFINED__
typedef interface IBDA_IPSinkControl IBDA_IPSinkControl;
#endif 	/* __IBDA_IPSinkControl_FWD_DEFINED__ */


#ifndef __IEnumPIDMap_FWD_DEFINED__
#define __IEnumPIDMap_FWD_DEFINED__
typedef interface IEnumPIDMap IEnumPIDMap;
#endif 	/* __IEnumPIDMap_FWD_DEFINED__ */


#ifndef __IMPEG2PIDMap_FWD_DEFINED__
#define __IMPEG2PIDMap_FWD_DEFINED__
typedef interface IMPEG2PIDMap IMPEG2PIDMap;
#endif 	/* __IMPEG2PIDMap_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "strmif.h"
#include "BdaTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IBDA_NetworkProvider_INTERFACE_DEFINED__
#define __IBDA_NetworkProvider_INTERFACE_DEFINED__

/* interface IBDA_NetworkProvider */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_NetworkProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fd501041-8ebe-11ce-8183-00aa00577da2")
    IBDA_NetworkProvider : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PutSignalSource( 
            /* [in] */ ULONG ulSignalSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSignalSource( 
            /* [out][in] */ ULONG *pulSignalSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNetworkType( 
            /* [out][in] */ GUID *pguidNetworkType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutTuningSpace( 
            /* [in] */ REFGUID guidTuningSpace) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTuningSpace( 
            /* [out][in] */ GUID *pguidTuingSpace) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterDeviceFilter( 
            /* [in] */ IUnknown *pUnkFilterControl,
            /* [out][in] */ ULONG *ppvRegisitrationContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnRegisterDeviceFilter( 
            /* [in] */ ULONG pvRegistrationContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_NetworkProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_NetworkProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_NetworkProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_NetworkProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *PutSignalSource )( 
            IBDA_NetworkProvider * This,
            /* [in] */ ULONG ulSignalSource);
        
        HRESULT ( STDMETHODCALLTYPE *GetSignalSource )( 
            IBDA_NetworkProvider * This,
            /* [out][in] */ ULONG *pulSignalSource);
        
        HRESULT ( STDMETHODCALLTYPE *GetNetworkType )( 
            IBDA_NetworkProvider * This,
            /* [out][in] */ GUID *pguidNetworkType);
        
        HRESULT ( STDMETHODCALLTYPE *PutTuningSpace )( 
            IBDA_NetworkProvider * This,
            /* [in] */ REFGUID guidTuningSpace);
        
        HRESULT ( STDMETHODCALLTYPE *GetTuningSpace )( 
            IBDA_NetworkProvider * This,
            /* [out][in] */ GUID *pguidTuingSpace);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterDeviceFilter )( 
            IBDA_NetworkProvider * This,
            /* [in] */ IUnknown *pUnkFilterControl,
            /* [out][in] */ ULONG *ppvRegisitrationContext);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegisterDeviceFilter )( 
            IBDA_NetworkProvider * This,
            /* [in] */ ULONG pvRegistrationContext);
        
        END_INTERFACE
    } IBDA_NetworkProviderVtbl;

    interface IBDA_NetworkProvider
    {
        CONST_VTBL struct IBDA_NetworkProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_NetworkProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_NetworkProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_NetworkProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_NetworkProvider_PutSignalSource(This,ulSignalSource)	\
    (This)->lpVtbl -> PutSignalSource(This,ulSignalSource)

#define IBDA_NetworkProvider_GetSignalSource(This,pulSignalSource)	\
    (This)->lpVtbl -> GetSignalSource(This,pulSignalSource)

#define IBDA_NetworkProvider_GetNetworkType(This,pguidNetworkType)	\
    (This)->lpVtbl -> GetNetworkType(This,pguidNetworkType)

#define IBDA_NetworkProvider_PutTuningSpace(This,guidTuningSpace)	\
    (This)->lpVtbl -> PutTuningSpace(This,guidTuningSpace)

#define IBDA_NetworkProvider_GetTuningSpace(This,pguidTuingSpace)	\
    (This)->lpVtbl -> GetTuningSpace(This,pguidTuingSpace)

#define IBDA_NetworkProvider_RegisterDeviceFilter(This,pUnkFilterControl,ppvRegisitrationContext)	\
    (This)->lpVtbl -> RegisterDeviceFilter(This,pUnkFilterControl,ppvRegisitrationContext)

#define IBDA_NetworkProvider_UnRegisterDeviceFilter(This,pvRegistrationContext)	\
    (This)->lpVtbl -> UnRegisterDeviceFilter(This,pvRegistrationContext)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_NetworkProvider_PutSignalSource_Proxy( 
    IBDA_NetworkProvider * This,
    /* [in] */ ULONG ulSignalSource);


void __RPC_STUB IBDA_NetworkProvider_PutSignalSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_NetworkProvider_GetSignalSource_Proxy( 
    IBDA_NetworkProvider * This,
    /* [out][in] */ ULONG *pulSignalSource);


void __RPC_STUB IBDA_NetworkProvider_GetSignalSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_NetworkProvider_GetNetworkType_Proxy( 
    IBDA_NetworkProvider * This,
    /* [out][in] */ GUID *pguidNetworkType);


void __RPC_STUB IBDA_NetworkProvider_GetNetworkType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_NetworkProvider_PutTuningSpace_Proxy( 
    IBDA_NetworkProvider * This,
    /* [in] */ REFGUID guidTuningSpace);


void __RPC_STUB IBDA_NetworkProvider_PutTuningSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_NetworkProvider_GetTuningSpace_Proxy( 
    IBDA_NetworkProvider * This,
    /* [out][in] */ GUID *pguidTuingSpace);


void __RPC_STUB IBDA_NetworkProvider_GetTuningSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_NetworkProvider_RegisterDeviceFilter_Proxy( 
    IBDA_NetworkProvider * This,
    /* [in] */ IUnknown *pUnkFilterControl,
    /* [out][in] */ ULONG *ppvRegisitrationContext);


void __RPC_STUB IBDA_NetworkProvider_RegisterDeviceFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_NetworkProvider_UnRegisterDeviceFilter_Proxy( 
    IBDA_NetworkProvider * This,
    /* [in] */ ULONG pvRegistrationContext);


void __RPC_STUB IBDA_NetworkProvider_UnRegisterDeviceFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_NetworkProvider_INTERFACE_DEFINED__ */


#ifndef __IBDA_EthernetFilter_INTERFACE_DEFINED__
#define __IBDA_EthernetFilter_INTERFACE_DEFINED__

/* interface IBDA_EthernetFilter */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_EthernetFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71985F43-1CA1-11d3-9CC8-00C04F7971E0")
    IBDA_EthernetFilter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMulticastListSize( 
            /* [out][in] */ ULONG *pulcbAddresses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutMulticastList( 
            /* [in] */ ULONG ulcbAddresses,
            /* [size_is][in] */ BYTE pAddressList[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMulticastList( 
            /* [out][in] */ ULONG *pulcbAddresses,
            /* [size_is][out] */ BYTE pAddressList[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutMulticastMode( 
            /* [in] */ ULONG ulModeMask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMulticastMode( 
            /* [out] */ ULONG *pulModeMask) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_EthernetFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_EthernetFilter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_EthernetFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_EthernetFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastListSize )( 
            IBDA_EthernetFilter * This,
            /* [out][in] */ ULONG *pulcbAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *PutMulticastList )( 
            IBDA_EthernetFilter * This,
            /* [in] */ ULONG ulcbAddresses,
            /* [size_is][in] */ BYTE pAddressList[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastList )( 
            IBDA_EthernetFilter * This,
            /* [out][in] */ ULONG *pulcbAddresses,
            /* [size_is][out] */ BYTE pAddressList[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *PutMulticastMode )( 
            IBDA_EthernetFilter * This,
            /* [in] */ ULONG ulModeMask);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastMode )( 
            IBDA_EthernetFilter * This,
            /* [out] */ ULONG *pulModeMask);
        
        END_INTERFACE
    } IBDA_EthernetFilterVtbl;

    interface IBDA_EthernetFilter
    {
        CONST_VTBL struct IBDA_EthernetFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_EthernetFilter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_EthernetFilter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_EthernetFilter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_EthernetFilter_GetMulticastListSize(This,pulcbAddresses)	\
    (This)->lpVtbl -> GetMulticastListSize(This,pulcbAddresses)

#define IBDA_EthernetFilter_PutMulticastList(This,ulcbAddresses,pAddressList)	\
    (This)->lpVtbl -> PutMulticastList(This,ulcbAddresses,pAddressList)

#define IBDA_EthernetFilter_GetMulticastList(This,pulcbAddresses,pAddressList)	\
    (This)->lpVtbl -> GetMulticastList(This,pulcbAddresses,pAddressList)

#define IBDA_EthernetFilter_PutMulticastMode(This,ulModeMask)	\
    (This)->lpVtbl -> PutMulticastMode(This,ulModeMask)

#define IBDA_EthernetFilter_GetMulticastMode(This,pulModeMask)	\
    (This)->lpVtbl -> GetMulticastMode(This,pulModeMask)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_EthernetFilter_GetMulticastListSize_Proxy( 
    IBDA_EthernetFilter * This,
    /* [out][in] */ ULONG *pulcbAddresses);


void __RPC_STUB IBDA_EthernetFilter_GetMulticastListSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_EthernetFilter_PutMulticastList_Proxy( 
    IBDA_EthernetFilter * This,
    /* [in] */ ULONG ulcbAddresses,
    /* [size_is][in] */ BYTE pAddressList[  ]);


void __RPC_STUB IBDA_EthernetFilter_PutMulticastList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_EthernetFilter_GetMulticastList_Proxy( 
    IBDA_EthernetFilter * This,
    /* [out][in] */ ULONG *pulcbAddresses,
    /* [size_is][out] */ BYTE pAddressList[  ]);


void __RPC_STUB IBDA_EthernetFilter_GetMulticastList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_EthernetFilter_PutMulticastMode_Proxy( 
    IBDA_EthernetFilter * This,
    /* [in] */ ULONG ulModeMask);


void __RPC_STUB IBDA_EthernetFilter_PutMulticastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_EthernetFilter_GetMulticastMode_Proxy( 
    IBDA_EthernetFilter * This,
    /* [out] */ ULONG *pulModeMask);


void __RPC_STUB IBDA_EthernetFilter_GetMulticastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_EthernetFilter_INTERFACE_DEFINED__ */


#ifndef __IBDA_IPV4Filter_INTERFACE_DEFINED__
#define __IBDA_IPV4Filter_INTERFACE_DEFINED__

/* interface IBDA_IPV4Filter */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_IPV4Filter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71985F44-1CA1-11d3-9CC8-00C04F7971E0")
    IBDA_IPV4Filter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMulticastListSize( 
            /* [out][in] */ ULONG *pulcbAddresses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutMulticastList( 
            /* [in] */ ULONG ulcbAddresses,
            /* [size_is][in] */ BYTE pAddressList[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMulticastList( 
            /* [out][in] */ ULONG *pulcbAddresses,
            /* [size_is][out] */ BYTE pAddressList[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutMulticastMode( 
            /* [in] */ ULONG ulModeMask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMulticastMode( 
            /* [out] */ ULONG *pulModeMask) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_IPV4FilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_IPV4Filter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_IPV4Filter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_IPV4Filter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastListSize )( 
            IBDA_IPV4Filter * This,
            /* [out][in] */ ULONG *pulcbAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *PutMulticastList )( 
            IBDA_IPV4Filter * This,
            /* [in] */ ULONG ulcbAddresses,
            /* [size_is][in] */ BYTE pAddressList[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastList )( 
            IBDA_IPV4Filter * This,
            /* [out][in] */ ULONG *pulcbAddresses,
            /* [size_is][out] */ BYTE pAddressList[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *PutMulticastMode )( 
            IBDA_IPV4Filter * This,
            /* [in] */ ULONG ulModeMask);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastMode )( 
            IBDA_IPV4Filter * This,
            /* [out] */ ULONG *pulModeMask);
        
        END_INTERFACE
    } IBDA_IPV4FilterVtbl;

    interface IBDA_IPV4Filter
    {
        CONST_VTBL struct IBDA_IPV4FilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_IPV4Filter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_IPV4Filter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_IPV4Filter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_IPV4Filter_GetMulticastListSize(This,pulcbAddresses)	\
    (This)->lpVtbl -> GetMulticastListSize(This,pulcbAddresses)

#define IBDA_IPV4Filter_PutMulticastList(This,ulcbAddresses,pAddressList)	\
    (This)->lpVtbl -> PutMulticastList(This,ulcbAddresses,pAddressList)

#define IBDA_IPV4Filter_GetMulticastList(This,pulcbAddresses,pAddressList)	\
    (This)->lpVtbl -> GetMulticastList(This,pulcbAddresses,pAddressList)

#define IBDA_IPV4Filter_PutMulticastMode(This,ulModeMask)	\
    (This)->lpVtbl -> PutMulticastMode(This,ulModeMask)

#define IBDA_IPV4Filter_GetMulticastMode(This,pulModeMask)	\
    (This)->lpVtbl -> GetMulticastMode(This,pulModeMask)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_IPV4Filter_GetMulticastListSize_Proxy( 
    IBDA_IPV4Filter * This,
    /* [out][in] */ ULONG *pulcbAddresses);


void __RPC_STUB IBDA_IPV4Filter_GetMulticastListSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV4Filter_PutMulticastList_Proxy( 
    IBDA_IPV4Filter * This,
    /* [in] */ ULONG ulcbAddresses,
    /* [size_is][in] */ BYTE pAddressList[  ]);


void __RPC_STUB IBDA_IPV4Filter_PutMulticastList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV4Filter_GetMulticastList_Proxy( 
    IBDA_IPV4Filter * This,
    /* [out][in] */ ULONG *pulcbAddresses,
    /* [size_is][out] */ BYTE pAddressList[  ]);


void __RPC_STUB IBDA_IPV4Filter_GetMulticastList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV4Filter_PutMulticastMode_Proxy( 
    IBDA_IPV4Filter * This,
    /* [in] */ ULONG ulModeMask);


void __RPC_STUB IBDA_IPV4Filter_PutMulticastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV4Filter_GetMulticastMode_Proxy( 
    IBDA_IPV4Filter * This,
    /* [out] */ ULONG *pulModeMask);


void __RPC_STUB IBDA_IPV4Filter_GetMulticastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_IPV4Filter_INTERFACE_DEFINED__ */


#ifndef __IBDA_IPV6Filter_INTERFACE_DEFINED__
#define __IBDA_IPV6Filter_INTERFACE_DEFINED__

/* interface IBDA_IPV6Filter */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_IPV6Filter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E1785A74-2A23-4fb3-9245-A8F88017EF33")
    IBDA_IPV6Filter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMulticastListSize( 
            /* [out][in] */ ULONG *pulcbAddresses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutMulticastList( 
            /* [in] */ ULONG ulcbAddresses,
            /* [size_is][in] */ BYTE pAddressList[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMulticastList( 
            /* [out][in] */ ULONG *pulcbAddresses,
            /* [size_is][out] */ BYTE pAddressList[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutMulticastMode( 
            /* [in] */ ULONG ulModeMask) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMulticastMode( 
            /* [out] */ ULONG *pulModeMask) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_IPV6FilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_IPV6Filter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_IPV6Filter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_IPV6Filter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastListSize )( 
            IBDA_IPV6Filter * This,
            /* [out][in] */ ULONG *pulcbAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *PutMulticastList )( 
            IBDA_IPV6Filter * This,
            /* [in] */ ULONG ulcbAddresses,
            /* [size_is][in] */ BYTE pAddressList[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastList )( 
            IBDA_IPV6Filter * This,
            /* [out][in] */ ULONG *pulcbAddresses,
            /* [size_is][out] */ BYTE pAddressList[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *PutMulticastMode )( 
            IBDA_IPV6Filter * This,
            /* [in] */ ULONG ulModeMask);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastMode )( 
            IBDA_IPV6Filter * This,
            /* [out] */ ULONG *pulModeMask);
        
        END_INTERFACE
    } IBDA_IPV6FilterVtbl;

    interface IBDA_IPV6Filter
    {
        CONST_VTBL struct IBDA_IPV6FilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_IPV6Filter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_IPV6Filter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_IPV6Filter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_IPV6Filter_GetMulticastListSize(This,pulcbAddresses)	\
    (This)->lpVtbl -> GetMulticastListSize(This,pulcbAddresses)

#define IBDA_IPV6Filter_PutMulticastList(This,ulcbAddresses,pAddressList)	\
    (This)->lpVtbl -> PutMulticastList(This,ulcbAddresses,pAddressList)

#define IBDA_IPV6Filter_GetMulticastList(This,pulcbAddresses,pAddressList)	\
    (This)->lpVtbl -> GetMulticastList(This,pulcbAddresses,pAddressList)

#define IBDA_IPV6Filter_PutMulticastMode(This,ulModeMask)	\
    (This)->lpVtbl -> PutMulticastMode(This,ulModeMask)

#define IBDA_IPV6Filter_GetMulticastMode(This,pulModeMask)	\
    (This)->lpVtbl -> GetMulticastMode(This,pulModeMask)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_IPV6Filter_GetMulticastListSize_Proxy( 
    IBDA_IPV6Filter * This,
    /* [out][in] */ ULONG *pulcbAddresses);


void __RPC_STUB IBDA_IPV6Filter_GetMulticastListSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV6Filter_PutMulticastList_Proxy( 
    IBDA_IPV6Filter * This,
    /* [in] */ ULONG ulcbAddresses,
    /* [size_is][in] */ BYTE pAddressList[  ]);


void __RPC_STUB IBDA_IPV6Filter_PutMulticastList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV6Filter_GetMulticastList_Proxy( 
    IBDA_IPV6Filter * This,
    /* [out][in] */ ULONG *pulcbAddresses,
    /* [size_is][out] */ BYTE pAddressList[  ]);


void __RPC_STUB IBDA_IPV6Filter_GetMulticastList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV6Filter_PutMulticastMode_Proxy( 
    IBDA_IPV6Filter * This,
    /* [in] */ ULONG ulModeMask);


void __RPC_STUB IBDA_IPV6Filter_PutMulticastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPV6Filter_GetMulticastMode_Proxy( 
    IBDA_IPV6Filter * This,
    /* [out] */ ULONG *pulModeMask);


void __RPC_STUB IBDA_IPV6Filter_GetMulticastMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_IPV6Filter_INTERFACE_DEFINED__ */


#ifndef __IBDA_DeviceControl_INTERFACE_DEFINED__
#define __IBDA_DeviceControl_INTERFACE_DEFINED__

/* interface IBDA_DeviceControl */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_DeviceControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FD0A5AF3-B41D-11d2-9C95-00C04F7971E0")
    IBDA_DeviceControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE StartChanges( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckChanges( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitChanges( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChangeState( 
            /* [out][in] */ ULONG *pState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_DeviceControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_DeviceControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_DeviceControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_DeviceControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *StartChanges )( 
            IBDA_DeviceControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *CheckChanges )( 
            IBDA_DeviceControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *CommitChanges )( 
            IBDA_DeviceControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetChangeState )( 
            IBDA_DeviceControl * This,
            /* [out][in] */ ULONG *pState);
        
        END_INTERFACE
    } IBDA_DeviceControlVtbl;

    interface IBDA_DeviceControl
    {
        CONST_VTBL struct IBDA_DeviceControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_DeviceControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_DeviceControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_DeviceControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_DeviceControl_StartChanges(This)	\
    (This)->lpVtbl -> StartChanges(This)

#define IBDA_DeviceControl_CheckChanges(This)	\
    (This)->lpVtbl -> CheckChanges(This)

#define IBDA_DeviceControl_CommitChanges(This)	\
    (This)->lpVtbl -> CommitChanges(This)

#define IBDA_DeviceControl_GetChangeState(This,pState)	\
    (This)->lpVtbl -> GetChangeState(This,pState)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_DeviceControl_StartChanges_Proxy( 
    IBDA_DeviceControl * This);


void __RPC_STUB IBDA_DeviceControl_StartChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DeviceControl_CheckChanges_Proxy( 
    IBDA_DeviceControl * This);


void __RPC_STUB IBDA_DeviceControl_CheckChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DeviceControl_CommitChanges_Proxy( 
    IBDA_DeviceControl * This);


void __RPC_STUB IBDA_DeviceControl_CommitChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DeviceControl_GetChangeState_Proxy( 
    IBDA_DeviceControl * This,
    /* [out][in] */ ULONG *pState);


void __RPC_STUB IBDA_DeviceControl_GetChangeState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_DeviceControl_INTERFACE_DEFINED__ */


#ifndef __IBDA_PinControl_INTERFACE_DEFINED__
#define __IBDA_PinControl_INTERFACE_DEFINED__

/* interface IBDA_PinControl */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_PinControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0DED49D5-A8B7-4d5d-97A1-12B0C195874D")
    IBDA_PinControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPinID( 
            /* [out][in] */ ULONG *pulPinID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPinType( 
            /* [out][in] */ ULONG *pulPinType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_PinControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_PinControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_PinControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_PinControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPinID )( 
            IBDA_PinControl * This,
            /* [out][in] */ ULONG *pulPinID);
        
        HRESULT ( STDMETHODCALLTYPE *GetPinType )( 
            IBDA_PinControl * This,
            /* [out][in] */ ULONG *pulPinType);
        
        END_INTERFACE
    } IBDA_PinControlVtbl;

    interface IBDA_PinControl
    {
        CONST_VTBL struct IBDA_PinControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_PinControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_PinControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_PinControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_PinControl_GetPinID(This,pulPinID)	\
    (This)->lpVtbl -> GetPinID(This,pulPinID)

#define IBDA_PinControl_GetPinType(This,pulPinType)	\
    (This)->lpVtbl -> GetPinType(This,pulPinType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_PinControl_GetPinID_Proxy( 
    IBDA_PinControl * This,
    /* [out][in] */ ULONG *pulPinID);


void __RPC_STUB IBDA_PinControl_GetPinID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_PinControl_GetPinType_Proxy( 
    IBDA_PinControl * This,
    /* [out][in] */ ULONG *pulPinType);


void __RPC_STUB IBDA_PinControl_GetPinType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_PinControl_INTERFACE_DEFINED__ */


#ifndef __IBDA_SignalProperties_INTERFACE_DEFINED__
#define __IBDA_SignalProperties_INTERFACE_DEFINED__

/* interface IBDA_SignalProperties */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_SignalProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D2F1644B-B409-11d2-BC69-00A0C9EE9E16")
    IBDA_SignalProperties : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PutNetworkType( 
            /* [in] */ REFGUID guidNetworkType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNetworkType( 
            /* [out][in] */ GUID *pguidNetworkType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutSignalSource( 
            /* [in] */ ULONG ulSignalSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSignalSource( 
            /* [out][in] */ ULONG *pulSignalSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutTuningSpace( 
            /* [in] */ REFGUID guidTuningSpace) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTuningSpace( 
            /* [out][in] */ GUID *pguidTuingSpace) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_SignalPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_SignalProperties * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_SignalProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_SignalProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *PutNetworkType )( 
            IBDA_SignalProperties * This,
            /* [in] */ REFGUID guidNetworkType);
        
        HRESULT ( STDMETHODCALLTYPE *GetNetworkType )( 
            IBDA_SignalProperties * This,
            /* [out][in] */ GUID *pguidNetworkType);
        
        HRESULT ( STDMETHODCALLTYPE *PutSignalSource )( 
            IBDA_SignalProperties * This,
            /* [in] */ ULONG ulSignalSource);
        
        HRESULT ( STDMETHODCALLTYPE *GetSignalSource )( 
            IBDA_SignalProperties * This,
            /* [out][in] */ ULONG *pulSignalSource);
        
        HRESULT ( STDMETHODCALLTYPE *PutTuningSpace )( 
            IBDA_SignalProperties * This,
            /* [in] */ REFGUID guidTuningSpace);
        
        HRESULT ( STDMETHODCALLTYPE *GetTuningSpace )( 
            IBDA_SignalProperties * This,
            /* [out][in] */ GUID *pguidTuingSpace);
        
        END_INTERFACE
    } IBDA_SignalPropertiesVtbl;

    interface IBDA_SignalProperties
    {
        CONST_VTBL struct IBDA_SignalPropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_SignalProperties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_SignalProperties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_SignalProperties_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_SignalProperties_PutNetworkType(This,guidNetworkType)	\
    (This)->lpVtbl -> PutNetworkType(This,guidNetworkType)

#define IBDA_SignalProperties_GetNetworkType(This,pguidNetworkType)	\
    (This)->lpVtbl -> GetNetworkType(This,pguidNetworkType)

#define IBDA_SignalProperties_PutSignalSource(This,ulSignalSource)	\
    (This)->lpVtbl -> PutSignalSource(This,ulSignalSource)

#define IBDA_SignalProperties_GetSignalSource(This,pulSignalSource)	\
    (This)->lpVtbl -> GetSignalSource(This,pulSignalSource)

#define IBDA_SignalProperties_PutTuningSpace(This,guidTuningSpace)	\
    (This)->lpVtbl -> PutTuningSpace(This,guidTuningSpace)

#define IBDA_SignalProperties_GetTuningSpace(This,pguidTuingSpace)	\
    (This)->lpVtbl -> GetTuningSpace(This,pguidTuingSpace)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_SignalProperties_PutNetworkType_Proxy( 
    IBDA_SignalProperties * This,
    /* [in] */ REFGUID guidNetworkType);


void __RPC_STUB IBDA_SignalProperties_PutNetworkType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_SignalProperties_GetNetworkType_Proxy( 
    IBDA_SignalProperties * This,
    /* [out][in] */ GUID *pguidNetworkType);


void __RPC_STUB IBDA_SignalProperties_GetNetworkType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_SignalProperties_PutSignalSource_Proxy( 
    IBDA_SignalProperties * This,
    /* [in] */ ULONG ulSignalSource);


void __RPC_STUB IBDA_SignalProperties_PutSignalSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_SignalProperties_GetSignalSource_Proxy( 
    IBDA_SignalProperties * This,
    /* [out][in] */ ULONG *pulSignalSource);


void __RPC_STUB IBDA_SignalProperties_GetSignalSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_SignalProperties_PutTuningSpace_Proxy( 
    IBDA_SignalProperties * This,
    /* [in] */ REFGUID guidTuningSpace);


void __RPC_STUB IBDA_SignalProperties_PutTuningSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_SignalProperties_GetTuningSpace_Proxy( 
    IBDA_SignalProperties * This,
    /* [out][in] */ GUID *pguidTuingSpace);


void __RPC_STUB IBDA_SignalProperties_GetTuningSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_SignalProperties_INTERFACE_DEFINED__ */


#ifndef __IBDA_Topology_INTERFACE_DEFINED__
#define __IBDA_Topology_INTERFACE_DEFINED__

/* interface IBDA_Topology */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_Topology;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A14EE835-0A23-11d3-9CC7-00C04F7971E0")
    IBDA_Topology : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNodeTypes( 
            /* [out][in] */ ULONG *pulcNodeTypes,
            /* [in] */ ULONG ulcNodeTypesMax,
            /* [size_is][out][in] */ ULONG rgulNodeTypes[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNodeInterfaces( 
            /* [in] */ ULONG ulNodeType,
            /* [out][in] */ ULONG *pulcInterfaces,
            /* [in] */ ULONG ulcInterfacesMax,
            /* [size_is][out][in] */ GUID *rgguidInterfaces[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPinTypes( 
            /* [out][in] */ ULONG *pulcPinTypes,
            /* [in] */ ULONG ulcPinTypesMax,
            /* [size_is][out][in] */ ULONG rgulPinTypes[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTemplateConnections( 
            /* [out][in] */ ULONG *pulcConnections,
            /* [in] */ ULONG ulcConnectionsMax,
            /* [size_is][out][in] */ BDA_TEMPLATE_CONNECTION rgConnections[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreatePin( 
            /* [in] */ ULONG ulPinType,
            /* [out][in] */ ULONG *pulPinId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeletePin( 
            /* [in] */ ULONG ulPinId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
            /* [in] */ ULONG ulPinId,
            /* [in] */ AM_MEDIA_TYPE *pMediaType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMedium( 
            /* [in] */ ULONG ulPinId,
            /* [in] */ REGPINMEDIUM *pMedium) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateTopology( 
            /* [in] */ ULONG ulInputPinId,
            /* [in] */ ULONG ulOutputPinId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetControlNode( 
            /* [in] */ ULONG ulInputPinId,
            /* [in] */ ULONG ulOutputPinId,
            /* [in] */ ULONG ulNodeType,
            /* [out][in] */ IUnknown **ppControlNode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_TopologyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_Topology * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_Topology * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_Topology * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetNodeTypes )( 
            IBDA_Topology * This,
            /* [out][in] */ ULONG *pulcNodeTypes,
            /* [in] */ ULONG ulcNodeTypesMax,
            /* [size_is][out][in] */ ULONG rgulNodeTypes[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *GetNodeInterfaces )( 
            IBDA_Topology * This,
            /* [in] */ ULONG ulNodeType,
            /* [out][in] */ ULONG *pulcInterfaces,
            /* [in] */ ULONG ulcInterfacesMax,
            /* [size_is][out][in] */ GUID *rgguidInterfaces[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *GetPinTypes )( 
            IBDA_Topology * This,
            /* [out][in] */ ULONG *pulcPinTypes,
            /* [in] */ ULONG ulcPinTypesMax,
            /* [size_is][out][in] */ ULONG rgulPinTypes[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *GetTemplateConnections )( 
            IBDA_Topology * This,
            /* [out][in] */ ULONG *pulcConnections,
            /* [in] */ ULONG ulcConnectionsMax,
            /* [size_is][out][in] */ BDA_TEMPLATE_CONNECTION rgConnections[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *CreatePin )( 
            IBDA_Topology * This,
            /* [in] */ ULONG ulPinType,
            /* [out][in] */ ULONG *pulPinId);
        
        HRESULT ( STDMETHODCALLTYPE *DeletePin )( 
            IBDA_Topology * This,
            /* [in] */ ULONG ulPinId);
        
        HRESULT ( STDMETHODCALLTYPE *SetMediaType )( 
            IBDA_Topology * This,
            /* [in] */ ULONG ulPinId,
            /* [in] */ AM_MEDIA_TYPE *pMediaType);
        
        HRESULT ( STDMETHODCALLTYPE *SetMedium )( 
            IBDA_Topology * This,
            /* [in] */ ULONG ulPinId,
            /* [in] */ REGPINMEDIUM *pMedium);
        
        HRESULT ( STDMETHODCALLTYPE *CreateTopology )( 
            IBDA_Topology * This,
            /* [in] */ ULONG ulInputPinId,
            /* [in] */ ULONG ulOutputPinId);
        
        HRESULT ( STDMETHODCALLTYPE *GetControlNode )( 
            IBDA_Topology * This,
            /* [in] */ ULONG ulInputPinId,
            /* [in] */ ULONG ulOutputPinId,
            /* [in] */ ULONG ulNodeType,
            /* [out][in] */ IUnknown **ppControlNode);
        
        END_INTERFACE
    } IBDA_TopologyVtbl;

    interface IBDA_Topology
    {
        CONST_VTBL struct IBDA_TopologyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_Topology_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_Topology_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_Topology_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_Topology_GetNodeTypes(This,pulcNodeTypes,ulcNodeTypesMax,rgulNodeTypes)	\
    (This)->lpVtbl -> GetNodeTypes(This,pulcNodeTypes,ulcNodeTypesMax,rgulNodeTypes)

#define IBDA_Topology_GetNodeInterfaces(This,ulNodeType,pulcInterfaces,ulcInterfacesMax,rgguidInterfaces)	\
    (This)->lpVtbl -> GetNodeInterfaces(This,ulNodeType,pulcInterfaces,ulcInterfacesMax,rgguidInterfaces)

#define IBDA_Topology_GetPinTypes(This,pulcPinTypes,ulcPinTypesMax,rgulPinTypes)	\
    (This)->lpVtbl -> GetPinTypes(This,pulcPinTypes,ulcPinTypesMax,rgulPinTypes)

#define IBDA_Topology_GetTemplateConnections(This,pulcConnections,ulcConnectionsMax,rgConnections)	\
    (This)->lpVtbl -> GetTemplateConnections(This,pulcConnections,ulcConnectionsMax,rgConnections)

#define IBDA_Topology_CreatePin(This,ulPinType,pulPinId)	\
    (This)->lpVtbl -> CreatePin(This,ulPinType,pulPinId)

#define IBDA_Topology_DeletePin(This,ulPinId)	\
    (This)->lpVtbl -> DeletePin(This,ulPinId)

#define IBDA_Topology_SetMediaType(This,ulPinId,pMediaType)	\
    (This)->lpVtbl -> SetMediaType(This,ulPinId,pMediaType)

#define IBDA_Topology_SetMedium(This,ulPinId,pMedium)	\
    (This)->lpVtbl -> SetMedium(This,ulPinId,pMedium)

#define IBDA_Topology_CreateTopology(This,ulInputPinId,ulOutputPinId)	\
    (This)->lpVtbl -> CreateTopology(This,ulInputPinId,ulOutputPinId)

#define IBDA_Topology_GetControlNode(This,ulInputPinId,ulOutputPinId,ulNodeType,ppControlNode)	\
    (This)->lpVtbl -> GetControlNode(This,ulInputPinId,ulOutputPinId,ulNodeType,ppControlNode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_Topology_GetNodeTypes_Proxy( 
    IBDA_Topology * This,
    /* [out][in] */ ULONG *pulcNodeTypes,
    /* [in] */ ULONG ulcNodeTypesMax,
    /* [size_is][out][in] */ ULONG rgulNodeTypes[  ]);


void __RPC_STUB IBDA_Topology_GetNodeTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_GetNodeInterfaces_Proxy( 
    IBDA_Topology * This,
    /* [in] */ ULONG ulNodeType,
    /* [out][in] */ ULONG *pulcInterfaces,
    /* [in] */ ULONG ulcInterfacesMax,
    /* [size_is][out][in] */ GUID *rgguidInterfaces[  ]);


void __RPC_STUB IBDA_Topology_GetNodeInterfaces_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_GetPinTypes_Proxy( 
    IBDA_Topology * This,
    /* [out][in] */ ULONG *pulcPinTypes,
    /* [in] */ ULONG ulcPinTypesMax,
    /* [size_is][out][in] */ ULONG rgulPinTypes[  ]);


void __RPC_STUB IBDA_Topology_GetPinTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_GetTemplateConnections_Proxy( 
    IBDA_Topology * This,
    /* [out][in] */ ULONG *pulcConnections,
    /* [in] */ ULONG ulcConnectionsMax,
    /* [size_is][out][in] */ BDA_TEMPLATE_CONNECTION rgConnections[  ]);


void __RPC_STUB IBDA_Topology_GetTemplateConnections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_CreatePin_Proxy( 
    IBDA_Topology * This,
    /* [in] */ ULONG ulPinType,
    /* [out][in] */ ULONG *pulPinId);


void __RPC_STUB IBDA_Topology_CreatePin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_DeletePin_Proxy( 
    IBDA_Topology * This,
    /* [in] */ ULONG ulPinId);


void __RPC_STUB IBDA_Topology_DeletePin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_SetMediaType_Proxy( 
    IBDA_Topology * This,
    /* [in] */ ULONG ulPinId,
    /* [in] */ AM_MEDIA_TYPE *pMediaType);


void __RPC_STUB IBDA_Topology_SetMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_SetMedium_Proxy( 
    IBDA_Topology * This,
    /* [in] */ ULONG ulPinId,
    /* [in] */ REGPINMEDIUM *pMedium);


void __RPC_STUB IBDA_Topology_SetMedium_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_CreateTopology_Proxy( 
    IBDA_Topology * This,
    /* [in] */ ULONG ulInputPinId,
    /* [in] */ ULONG ulOutputPinId);


void __RPC_STUB IBDA_Topology_CreateTopology_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_Topology_GetControlNode_Proxy( 
    IBDA_Topology * This,
    /* [in] */ ULONG ulInputPinId,
    /* [in] */ ULONG ulOutputPinId,
    /* [in] */ ULONG ulNodeType,
    /* [out][in] */ IUnknown **ppControlNode);


void __RPC_STUB IBDA_Topology_GetControlNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_Topology_INTERFACE_DEFINED__ */


#ifndef __IBDA_VoidTransform_INTERFACE_DEFINED__
#define __IBDA_VoidTransform_INTERFACE_DEFINED__

/* interface IBDA_VoidTransform */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_VoidTransform;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71985F46-1CA1-11d3-9CC8-00C04F7971E0")
    IBDA_VoidTransform : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_VoidTransformVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_VoidTransform * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_VoidTransform * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_VoidTransform * This);
        
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            IBDA_VoidTransform * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IBDA_VoidTransform * This);
        
        END_INTERFACE
    } IBDA_VoidTransformVtbl;

    interface IBDA_VoidTransform
    {
        CONST_VTBL struct IBDA_VoidTransformVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_VoidTransform_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_VoidTransform_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_VoidTransform_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_VoidTransform_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IBDA_VoidTransform_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_VoidTransform_Start_Proxy( 
    IBDA_VoidTransform * This);


void __RPC_STUB IBDA_VoidTransform_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_VoidTransform_Stop_Proxy( 
    IBDA_VoidTransform * This);


void __RPC_STUB IBDA_VoidTransform_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_VoidTransform_INTERFACE_DEFINED__ */


#ifndef __IBDA_NullTransform_INTERFACE_DEFINED__
#define __IBDA_NullTransform_INTERFACE_DEFINED__

/* interface IBDA_NullTransform */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_NullTransform;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DDF15B0D-BD25-11d2-9CA0-00C04F7971E0")
    IBDA_NullTransform : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_NullTransformVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_NullTransform * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_NullTransform * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_NullTransform * This);
        
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            IBDA_NullTransform * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IBDA_NullTransform * This);
        
        END_INTERFACE
    } IBDA_NullTransformVtbl;

    interface IBDA_NullTransform
    {
        CONST_VTBL struct IBDA_NullTransformVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_NullTransform_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_NullTransform_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_NullTransform_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_NullTransform_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IBDA_NullTransform_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_NullTransform_Start_Proxy( 
    IBDA_NullTransform * This);


void __RPC_STUB IBDA_NullTransform_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_NullTransform_Stop_Proxy( 
    IBDA_NullTransform * This);


void __RPC_STUB IBDA_NullTransform_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_NullTransform_INTERFACE_DEFINED__ */


#ifndef __IBDA_FrequencyFilter_INTERFACE_DEFINED__
#define __IBDA_FrequencyFilter_INTERFACE_DEFINED__

/* interface IBDA_FrequencyFilter */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_FrequencyFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71985F47-1CA1-11d3-9CC8-00C04F7971E0")
    IBDA_FrequencyFilter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE put_Autotune( 
            /* [in] */ ULONG *pulTransponder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Frequency( 
            /* [in] */ ULONG *pulFrequency) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Frequency( 
            /* [out][in] */ ULONG *pulFrequency) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Polarity( 
            /* [in] */ ULONG *pulPolarity) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Polarity( 
            /* [out][in] */ ULONG *pulPolarity) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Range( 
            /* [in] */ ULONG *pulRange) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Range( 
            /* [out][in] */ ULONG *pulRange) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_FrequencyFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_FrequencyFilter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_FrequencyFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_FrequencyFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *put_Autotune )( 
            IBDA_FrequencyFilter * This,
            /* [in] */ ULONG *pulTransponder);
        
        HRESULT ( STDMETHODCALLTYPE *put_Frequency )( 
            IBDA_FrequencyFilter * This,
            /* [in] */ ULONG *pulFrequency);
        
        HRESULT ( STDMETHODCALLTYPE *get_Frequency )( 
            IBDA_FrequencyFilter * This,
            /* [out][in] */ ULONG *pulFrequency);
        
        HRESULT ( STDMETHODCALLTYPE *put_Polarity )( 
            IBDA_FrequencyFilter * This,
            /* [in] */ ULONG *pulPolarity);
        
        HRESULT ( STDMETHODCALLTYPE *get_Polarity )( 
            IBDA_FrequencyFilter * This,
            /* [out][in] */ ULONG *pulPolarity);
        
        HRESULT ( STDMETHODCALLTYPE *put_Range )( 
            IBDA_FrequencyFilter * This,
            /* [in] */ ULONG *pulRange);
        
        HRESULT ( STDMETHODCALLTYPE *get_Range )( 
            IBDA_FrequencyFilter * This,
            /* [out][in] */ ULONG *pulRange);
        
        END_INTERFACE
    } IBDA_FrequencyFilterVtbl;

    interface IBDA_FrequencyFilter
    {
        CONST_VTBL struct IBDA_FrequencyFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_FrequencyFilter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_FrequencyFilter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_FrequencyFilter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_FrequencyFilter_put_Autotune(This,pulTransponder)	\
    (This)->lpVtbl -> put_Autotune(This,pulTransponder)

#define IBDA_FrequencyFilter_put_Frequency(This,pulFrequency)	\
    (This)->lpVtbl -> put_Frequency(This,pulFrequency)

#define IBDA_FrequencyFilter_get_Frequency(This,pulFrequency)	\
    (This)->lpVtbl -> get_Frequency(This,pulFrequency)

#define IBDA_FrequencyFilter_put_Polarity(This,pulPolarity)	\
    (This)->lpVtbl -> put_Polarity(This,pulPolarity)

#define IBDA_FrequencyFilter_get_Polarity(This,pulPolarity)	\
    (This)->lpVtbl -> get_Polarity(This,pulPolarity)

#define IBDA_FrequencyFilter_put_Range(This,pulRange)	\
    (This)->lpVtbl -> put_Range(This,pulRange)

#define IBDA_FrequencyFilter_get_Range(This,pulRange)	\
    (This)->lpVtbl -> get_Range(This,pulRange)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_FrequencyFilter_put_Autotune_Proxy( 
    IBDA_FrequencyFilter * This,
    /* [in] */ ULONG *pulTransponder);


void __RPC_STUB IBDA_FrequencyFilter_put_Autotune_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_FrequencyFilter_put_Frequency_Proxy( 
    IBDA_FrequencyFilter * This,
    /* [in] */ ULONG *pulFrequency);


void __RPC_STUB IBDA_FrequencyFilter_put_Frequency_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_FrequencyFilter_get_Frequency_Proxy( 
    IBDA_FrequencyFilter * This,
    /* [out][in] */ ULONG *pulFrequency);


void __RPC_STUB IBDA_FrequencyFilter_get_Frequency_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_FrequencyFilter_put_Polarity_Proxy( 
    IBDA_FrequencyFilter * This,
    /* [in] */ ULONG *pulPolarity);


void __RPC_STUB IBDA_FrequencyFilter_put_Polarity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_FrequencyFilter_get_Polarity_Proxy( 
    IBDA_FrequencyFilter * This,
    /* [out][in] */ ULONG *pulPolarity);


void __RPC_STUB IBDA_FrequencyFilter_get_Polarity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_FrequencyFilter_put_Range_Proxy( 
    IBDA_FrequencyFilter * This,
    /* [in] */ ULONG *pulRange);


void __RPC_STUB IBDA_FrequencyFilter_put_Range_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_FrequencyFilter_get_Range_Proxy( 
    IBDA_FrequencyFilter * This,
    /* [out][in] */ ULONG *pulRange);


void __RPC_STUB IBDA_FrequencyFilter_get_Range_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_FrequencyFilter_INTERFACE_DEFINED__ */


#ifndef __IBDA_AutoDemodulate_INTERFACE_DEFINED__
#define __IBDA_AutoDemodulate_INTERFACE_DEFINED__

/* interface IBDA_AutoDemodulate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_AutoDemodulate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DDF15B12-BD25-11d2-9CA0-00C04F7971E0")
    IBDA_AutoDemodulate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE put_AutoDemodulate( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_AutoDemodulateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_AutoDemodulate * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_AutoDemodulate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_AutoDemodulate * This);
        
        HRESULT ( STDMETHODCALLTYPE *put_AutoDemodulate )( 
            IBDA_AutoDemodulate * This);
        
        END_INTERFACE
    } IBDA_AutoDemodulateVtbl;

    interface IBDA_AutoDemodulate
    {
        CONST_VTBL struct IBDA_AutoDemodulateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_AutoDemodulate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_AutoDemodulate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_AutoDemodulate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_AutoDemodulate_put_AutoDemodulate(This)	\
    (This)->lpVtbl -> put_AutoDemodulate(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_AutoDemodulate_put_AutoDemodulate_Proxy( 
    IBDA_AutoDemodulate * This);


void __RPC_STUB IBDA_AutoDemodulate_put_AutoDemodulate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_AutoDemodulate_INTERFACE_DEFINED__ */


#ifndef __IBDA_DigitalDemodulator_INTERFACE_DEFINED__
#define __IBDA_DigitalDemodulator_INTERFACE_DEFINED__

/* interface IBDA_DigitalDemodulator */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_DigitalDemodulator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF30F379-985B-4d10-B640-A79D5E04E1E0")
    IBDA_DigitalDemodulator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE put_ModulationType( 
            /* [in] */ ModulationType *pModulationType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_ModulationType( 
            /* [out][in] */ ModulationType *pModulationType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_InnerFECMethod( 
            /* [in] */ FECMethod *pFECMethod) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_InnerFECMethod( 
            /* [out][in] */ FECMethod *pFECMethod) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_InnerFECRate( 
            /* [in] */ BinaryConvolutionCodeRate *pFECRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_InnerFECRate( 
            /* [out][in] */ BinaryConvolutionCodeRate *pFECRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_OuterFECMethod( 
            /* [in] */ FECMethod *pFECMethod) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_OuterFECMethod( 
            /* [out][in] */ FECMethod *pFECMethod) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_OuterFECRate( 
            /* [in] */ BinaryConvolutionCodeRate *pFECRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_OuterFECRate( 
            /* [out][in] */ BinaryConvolutionCodeRate *pFECRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_SymbolRate( 
            /* [in] */ ULONG *pSymbolRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_SymbolRate( 
            /* [out][in] */ ULONG *pSymbolRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_SpectralInversion( 
            /* [in] */ SpectralInversion *pSpectralInversion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_SpectralInversion( 
            /* [out][in] */ SpectralInversion *pSpectralInversion) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_DigitalDemodulatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_DigitalDemodulator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_DigitalDemodulator * This);
        
        HRESULT ( STDMETHODCALLTYPE *put_ModulationType )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ ModulationType *pModulationType);
        
        HRESULT ( STDMETHODCALLTYPE *get_ModulationType )( 
            IBDA_DigitalDemodulator * This,
            /* [out][in] */ ModulationType *pModulationType);
        
        HRESULT ( STDMETHODCALLTYPE *put_InnerFECMethod )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ FECMethod *pFECMethod);
        
        HRESULT ( STDMETHODCALLTYPE *get_InnerFECMethod )( 
            IBDA_DigitalDemodulator * This,
            /* [out][in] */ FECMethod *pFECMethod);
        
        HRESULT ( STDMETHODCALLTYPE *put_InnerFECRate )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ BinaryConvolutionCodeRate *pFECRate);
        
        HRESULT ( STDMETHODCALLTYPE *get_InnerFECRate )( 
            IBDA_DigitalDemodulator * This,
            /* [out][in] */ BinaryConvolutionCodeRate *pFECRate);
        
        HRESULT ( STDMETHODCALLTYPE *put_OuterFECMethod )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ FECMethod *pFECMethod);
        
        HRESULT ( STDMETHODCALLTYPE *get_OuterFECMethod )( 
            IBDA_DigitalDemodulator * This,
            /* [out][in] */ FECMethod *pFECMethod);
        
        HRESULT ( STDMETHODCALLTYPE *put_OuterFECRate )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ BinaryConvolutionCodeRate *pFECRate);
        
        HRESULT ( STDMETHODCALLTYPE *get_OuterFECRate )( 
            IBDA_DigitalDemodulator * This,
            /* [out][in] */ BinaryConvolutionCodeRate *pFECRate);
        
        HRESULT ( STDMETHODCALLTYPE *put_SymbolRate )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ ULONG *pSymbolRate);
        
        HRESULT ( STDMETHODCALLTYPE *get_SymbolRate )( 
            IBDA_DigitalDemodulator * This,
            /* [out][in] */ ULONG *pSymbolRate);
        
        HRESULT ( STDMETHODCALLTYPE *put_SpectralInversion )( 
            IBDA_DigitalDemodulator * This,
            /* [in] */ SpectralInversion *pSpectralInversion);
        
        HRESULT ( STDMETHODCALLTYPE *get_SpectralInversion )( 
            IBDA_DigitalDemodulator * This,
            /* [out][in] */ SpectralInversion *pSpectralInversion);
        
        END_INTERFACE
    } IBDA_DigitalDemodulatorVtbl;

    interface IBDA_DigitalDemodulator
    {
        CONST_VTBL struct IBDA_DigitalDemodulatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_DigitalDemodulator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_DigitalDemodulator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_DigitalDemodulator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_DigitalDemodulator_put_ModulationType(This,pModulationType)	\
    (This)->lpVtbl -> put_ModulationType(This,pModulationType)

#define IBDA_DigitalDemodulator_get_ModulationType(This,pModulationType)	\
    (This)->lpVtbl -> get_ModulationType(This,pModulationType)

#define IBDA_DigitalDemodulator_put_InnerFECMethod(This,pFECMethod)	\
    (This)->lpVtbl -> put_InnerFECMethod(This,pFECMethod)

#define IBDA_DigitalDemodulator_get_InnerFECMethod(This,pFECMethod)	\
    (This)->lpVtbl -> get_InnerFECMethod(This,pFECMethod)

#define IBDA_DigitalDemodulator_put_InnerFECRate(This,pFECRate)	\
    (This)->lpVtbl -> put_InnerFECRate(This,pFECRate)

#define IBDA_DigitalDemodulator_get_InnerFECRate(This,pFECRate)	\
    (This)->lpVtbl -> get_InnerFECRate(This,pFECRate)

#define IBDA_DigitalDemodulator_put_OuterFECMethod(This,pFECMethod)	\
    (This)->lpVtbl -> put_OuterFECMethod(This,pFECMethod)

#define IBDA_DigitalDemodulator_get_OuterFECMethod(This,pFECMethod)	\
    (This)->lpVtbl -> get_OuterFECMethod(This,pFECMethod)

#define IBDA_DigitalDemodulator_put_OuterFECRate(This,pFECRate)	\
    (This)->lpVtbl -> put_OuterFECRate(This,pFECRate)

#define IBDA_DigitalDemodulator_get_OuterFECRate(This,pFECRate)	\
    (This)->lpVtbl -> get_OuterFECRate(This,pFECRate)

#define IBDA_DigitalDemodulator_put_SymbolRate(This,pSymbolRate)	\
    (This)->lpVtbl -> put_SymbolRate(This,pSymbolRate)

#define IBDA_DigitalDemodulator_get_SymbolRate(This,pSymbolRate)	\
    (This)->lpVtbl -> get_SymbolRate(This,pSymbolRate)

#define IBDA_DigitalDemodulator_put_SpectralInversion(This,pSpectralInversion)	\
    (This)->lpVtbl -> put_SpectralInversion(This,pSpectralInversion)

#define IBDA_DigitalDemodulator_get_SpectralInversion(This,pSpectralInversion)	\
    (This)->lpVtbl -> get_SpectralInversion(This,pSpectralInversion)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_put_ModulationType_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [in] */ ModulationType *pModulationType);


void __RPC_STUB IBDA_DigitalDemodulator_put_ModulationType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_get_ModulationType_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [out][in] */ ModulationType *pModulationType);


void __RPC_STUB IBDA_DigitalDemodulator_get_ModulationType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_put_InnerFECMethod_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [in] */ FECMethod *pFECMethod);


void __RPC_STUB IBDA_DigitalDemodulator_put_InnerFECMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_get_InnerFECMethod_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [out][in] */ FECMethod *pFECMethod);


void __RPC_STUB IBDA_DigitalDemodulator_get_InnerFECMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_put_InnerFECRate_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [in] */ BinaryConvolutionCodeRate *pFECRate);


void __RPC_STUB IBDA_DigitalDemodulator_put_InnerFECRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_get_InnerFECRate_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [out][in] */ BinaryConvolutionCodeRate *pFECRate);


void __RPC_STUB IBDA_DigitalDemodulator_get_InnerFECRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_put_OuterFECMethod_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [in] */ FECMethod *pFECMethod);


void __RPC_STUB IBDA_DigitalDemodulator_put_OuterFECMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_get_OuterFECMethod_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [out][in] */ FECMethod *pFECMethod);


void __RPC_STUB IBDA_DigitalDemodulator_get_OuterFECMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_put_OuterFECRate_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [in] */ BinaryConvolutionCodeRate *pFECRate);


void __RPC_STUB IBDA_DigitalDemodulator_put_OuterFECRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_get_OuterFECRate_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [out][in] */ BinaryConvolutionCodeRate *pFECRate);


void __RPC_STUB IBDA_DigitalDemodulator_get_OuterFECRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_put_SymbolRate_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [in] */ ULONG *pSymbolRate);


void __RPC_STUB IBDA_DigitalDemodulator_put_SymbolRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_get_SymbolRate_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [out][in] */ ULONG *pSymbolRate);


void __RPC_STUB IBDA_DigitalDemodulator_get_SymbolRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_put_SpectralInversion_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [in] */ SpectralInversion *pSpectralInversion);


void __RPC_STUB IBDA_DigitalDemodulator_put_SpectralInversion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_DigitalDemodulator_get_SpectralInversion_Proxy( 
    IBDA_DigitalDemodulator * This,
    /* [out][in] */ SpectralInversion *pSpectralInversion);


void __RPC_STUB IBDA_DigitalDemodulator_get_SpectralInversion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_DigitalDemodulator_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_bdaiface_0388 */
/* [local] */ 

typedef /* [public] */ 
enum __MIDL___MIDL_itf_bdaiface_0388_0001
    {	KSPROPERTY_IPSINK_MULTICASTLIST	= 0,
	KSPROPERTY_IPSINK_ADAPTER_DESCRIPTION	= KSPROPERTY_IPSINK_MULTICASTLIST + 1,
	KSPROPERTY_IPSINK_ADAPTER_ADDRESS	= KSPROPERTY_IPSINK_ADAPTER_DESCRIPTION + 1
    } 	KSPROPERTY_IPSINK;



extern RPC_IF_HANDLE __MIDL_itf_bdaiface_0388_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bdaiface_0388_v0_0_s_ifspec;

#ifndef __IBDA_IPSinkControl_INTERFACE_DEFINED__
#define __IBDA_IPSinkControl_INTERFACE_DEFINED__

/* interface IBDA_IPSinkControl */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBDA_IPSinkControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DC8E2-4050-11d3-8F4B-00C04F7971E2")
    IBDA_IPSinkControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMulticastList( 
            /* [out][in] */ unsigned long *pulcbSize,
            /* [out][in] */ BYTE **pbBuffer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAdapterIPAddress( 
            /* [out][in] */ unsigned long *pulcbSize,
            /* [out][in] */ BYTE **pbBuffer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBDA_IPSinkControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBDA_IPSinkControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBDA_IPSinkControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBDA_IPSinkControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMulticastList )( 
            IBDA_IPSinkControl * This,
            /* [out][in] */ unsigned long *pulcbSize,
            /* [out][in] */ BYTE **pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *GetAdapterIPAddress )( 
            IBDA_IPSinkControl * This,
            /* [out][in] */ unsigned long *pulcbSize,
            /* [out][in] */ BYTE **pbBuffer);
        
        END_INTERFACE
    } IBDA_IPSinkControlVtbl;

    interface IBDA_IPSinkControl
    {
        CONST_VTBL struct IBDA_IPSinkControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBDA_IPSinkControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBDA_IPSinkControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBDA_IPSinkControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBDA_IPSinkControl_GetMulticastList(This,pulcbSize,pbBuffer)	\
    (This)->lpVtbl -> GetMulticastList(This,pulcbSize,pbBuffer)

#define IBDA_IPSinkControl_GetAdapterIPAddress(This,pulcbSize,pbBuffer)	\
    (This)->lpVtbl -> GetAdapterIPAddress(This,pulcbSize,pbBuffer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBDA_IPSinkControl_GetMulticastList_Proxy( 
    IBDA_IPSinkControl * This,
    /* [out][in] */ unsigned long *pulcbSize,
    /* [out][in] */ BYTE **pbBuffer);


void __RPC_STUB IBDA_IPSinkControl_GetMulticastList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBDA_IPSinkControl_GetAdapterIPAddress_Proxy( 
    IBDA_IPSinkControl * This,
    /* [out][in] */ unsigned long *pulcbSize,
    /* [out][in] */ BYTE **pbBuffer);


void __RPC_STUB IBDA_IPSinkControl_GetAdapterIPAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBDA_IPSinkControl_INTERFACE_DEFINED__ */


#ifndef __IEnumPIDMap_INTERFACE_DEFINED__
#define __IEnumPIDMap_INTERFACE_DEFINED__

/* interface IEnumPIDMap */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumPIDMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("afb6c2a2-2c41-11d3-8a60-0000f81e0e4a")
    IEnumPIDMap : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG cRequest,
            /* [size_is][out][in] */ PID_MAP *pPIDMap,
            /* [out] */ ULONG *pcReceived) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG cRecords) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumPIDMap **ppIEnumPIDMap) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumPIDMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumPIDMap * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumPIDMap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumPIDMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumPIDMap * This,
            /* [in] */ ULONG cRequest,
            /* [size_is][out][in] */ PID_MAP *pPIDMap,
            /* [out] */ ULONG *pcReceived);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumPIDMap * This,
            /* [in] */ ULONG cRecords);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumPIDMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumPIDMap * This,
            /* [out] */ IEnumPIDMap **ppIEnumPIDMap);
        
        END_INTERFACE
    } IEnumPIDMapVtbl;

    interface IEnumPIDMap
    {
        CONST_VTBL struct IEnumPIDMapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumPIDMap_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumPIDMap_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumPIDMap_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumPIDMap_Next(This,cRequest,pPIDMap,pcReceived)	\
    (This)->lpVtbl -> Next(This,cRequest,pPIDMap,pcReceived)

#define IEnumPIDMap_Skip(This,cRecords)	\
    (This)->lpVtbl -> Skip(This,cRecords)

#define IEnumPIDMap_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumPIDMap_Clone(This,ppIEnumPIDMap)	\
    (This)->lpVtbl -> Clone(This,ppIEnumPIDMap)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumPIDMap_Next_Proxy( 
    IEnumPIDMap * This,
    /* [in] */ ULONG cRequest,
    /* [size_is][out][in] */ PID_MAP *pPIDMap,
    /* [out] */ ULONG *pcReceived);


void __RPC_STUB IEnumPIDMap_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPIDMap_Skip_Proxy( 
    IEnumPIDMap * This,
    /* [in] */ ULONG cRecords);


void __RPC_STUB IEnumPIDMap_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPIDMap_Reset_Proxy( 
    IEnumPIDMap * This);


void __RPC_STUB IEnumPIDMap_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumPIDMap_Clone_Proxy( 
    IEnumPIDMap * This,
    /* [out] */ IEnumPIDMap **ppIEnumPIDMap);


void __RPC_STUB IEnumPIDMap_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumPIDMap_INTERFACE_DEFINED__ */


#ifndef __IMPEG2PIDMap_INTERFACE_DEFINED__
#define __IMPEG2PIDMap_INTERFACE_DEFINED__

/* interface IMPEG2PIDMap */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IMPEG2PIDMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("afb6c2a1-2c41-11d3-8a60-0000f81e0e4a")
    IMPEG2PIDMap : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE MapPID( 
            /* [in] */ ULONG culPID,
            /* [in] */ ULONG *pulPID,
            /* [in] */ MEDIA_SAMPLE_CONTENT MediaSampleContent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnmapPID( 
            /* [in] */ ULONG culPID,
            /* [in] */ ULONG *pulPID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPIDMap( 
            /* [out] */ IEnumPIDMap **pIEnumPIDMap) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMPEG2PIDMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMPEG2PIDMap * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMPEG2PIDMap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMPEG2PIDMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *MapPID )( 
            IMPEG2PIDMap * This,
            /* [in] */ ULONG culPID,
            /* [in] */ ULONG *pulPID,
            /* [in] */ MEDIA_SAMPLE_CONTENT MediaSampleContent);
        
        HRESULT ( STDMETHODCALLTYPE *UnmapPID )( 
            IMPEG2PIDMap * This,
            /* [in] */ ULONG culPID,
            /* [in] */ ULONG *pulPID);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPIDMap )( 
            IMPEG2PIDMap * This,
            /* [out] */ IEnumPIDMap **pIEnumPIDMap);
        
        END_INTERFACE
    } IMPEG2PIDMapVtbl;

    interface IMPEG2PIDMap
    {
        CONST_VTBL struct IMPEG2PIDMapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMPEG2PIDMap_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMPEG2PIDMap_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMPEG2PIDMap_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMPEG2PIDMap_MapPID(This,culPID,pulPID,MediaSampleContent)	\
    (This)->lpVtbl -> MapPID(This,culPID,pulPID,MediaSampleContent)

#define IMPEG2PIDMap_UnmapPID(This,culPID,pulPID)	\
    (This)->lpVtbl -> UnmapPID(This,culPID,pulPID)

#define IMPEG2PIDMap_EnumPIDMap(This,pIEnumPIDMap)	\
    (This)->lpVtbl -> EnumPIDMap(This,pIEnumPIDMap)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMPEG2PIDMap_MapPID_Proxy( 
    IMPEG2PIDMap * This,
    /* [in] */ ULONG culPID,
    /* [in] */ ULONG *pulPID,
    /* [in] */ MEDIA_SAMPLE_CONTENT MediaSampleContent);


void __RPC_STUB IMPEG2PIDMap_MapPID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMPEG2PIDMap_UnmapPID_Proxy( 
    IMPEG2PIDMap * This,
    /* [in] */ ULONG culPID,
    /* [in] */ ULONG *pulPID);


void __RPC_STUB IMPEG2PIDMap_UnmapPID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMPEG2PIDMap_EnumPIDMap_Proxy( 
    IMPEG2PIDMap * This,
    /* [out] */ IEnumPIDMap **pIEnumPIDMap);


void __RPC_STUB IMPEG2PIDMap_EnumPIDMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMPEG2PIDMap_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


