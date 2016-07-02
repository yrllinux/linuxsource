/***********************************************************************
        Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#ifndef CM_H
#define CM_H

#include "rvcommon.h"
#include "rv_defs.h"
#include "coder.h"
#include "pvaltree.h"
#include "ci.h"
#include "cmH245GeneralDefs.h"


//add by yj
#define VER_RADSTACK  ( const char * )"h323stacklib 40.01.01.01.091211(CBB)"
//end

#if (RV_H245_SUPPORT_H225_PARAMS != 1)
#error H.245 package must support H.225!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Handles declaration */
RV_DECLARE_HANDLE(HAPPAPP);
RV_DECLARE_HANDLE(HPROTCONN);
RV_DECLARE_HANDLE(HPROTOCOL);
RV_DECLARE_HANDLE(HAPPCONN);
RV_DECLARE_HANDLE(HAPPPARTNER);


typedef enum
{
    cmQ931TpktChannel=0,
    cmH245TpktChannel
} cmTpktChanHandleType;

typedef enum
{
    cmRasUdpChannel=0,
    cmRasUdpChannelMCast
} cmUdpChanHandleType;

typedef struct
{
    RvUint8     t35CountryCode;
    RvUint8     t35Extension;
    RvUint16    manufacturerCode;
} cmNonStandard;

typedef struct
{
    cmNonStandard   info;
    int             productLen;
    int             versionLen;
    char            productID[256];
    char            versionID[256];
} cmVendor;

//add by yj for rrq(username & password) [20130410]
typedef struct
{
	char		userName[128];
	char		passWord[128];
} cmRegKeyParam;
//end [20130410]

typedef enum
{
    cmAliasTypeE164=cmTransportTypeIPLooseRoute+5,
    cmAliasTypeH323ID,
    cmAliasTypeEndpointID,
    cmAliasTypeGatekeeperID,
    cmAliasTypeURLID,
    cmAliasTypeTransportAddress,
    cmAliasTypeEMailID,
    cmAliasTypePartyNumber,
    cmAliasTypeLast /* Last element in enumeration - must be the last one */
} cmAliasType;


typedef enum
{
            /* the numbering plan is according to
               Recommendations E.163 and E.164.*/
    cmPartyNumberPublicUnknown,
        /* if used number digits carry prefix indicating type
           of number according to national recommendations*/
    cmPartyNumberPublicInternationalNumber  ,
    cmPartyNumberPublicNationalNumber   ,
    cmPartyNumberPublicNetworkSpecificNumber,
        /* not used, value reserved */
    cmPartyNumberPublicSubscriberNumber ,
    cmPartyNumberPublicAbbreviatedNumber    ,
        /* valid only for called party number at the outgoing
           access, network substitutes appropriate number. */

    cmPartyNumberDataPartyNumber,
        /* not used, value reserved */
    cmPartyNumberTelexPartyNumber,
        /* not used, value reserved*/
    cmPartyNumberPrivateUnknown,
    cmPartyNumberPrivateLevel2RegionalNumber,
    cmPartyNumberPrivateLevel1RegionalNumber,
    cmPartyNumberPrivatePISNSpecificNumber,
    cmPartyNumberPrivateLocalNumber,
    cmPartyNumberPrivateAbbreviatedNumber,
    cmPartyNumberNationalStandardPartyNumber
} cmPartyNumberType;

typedef struct
{
    cmAliasType         type;
    RvUint16            length; /* string in bytes */
    char*               string;
    cmPartyNumberType   pnType;
    cmTransportAddress  transport;
} cmAlias;

typedef enum
{
    cmEndpointTypeTerminal,
    cmEndpointTypeGateway,
    cmEndpointTypeMCU,
    cmEndpointTypeGK,
    cmEndpointTypeUndefined,
    cmEndpointTypeSET
} cmEndpointType;

typedef enum
{
    cmCallTypeP2P,
    cmCallTypeOne2N,
    cmCallTypeN2One,
    cmCallTypeN2Nw
} cmCallType;


typedef enum
{
    cmCallModelTypeDirect,
    cmCallModelTypeGKRouted
} cmCallModelType;

typedef enum
{
    cmTransportQOSEndpointControlled,
    cmTransportQOSGatekeeperControlled,
    cmTransportQOSNoControl
} cmTransportQOS;


/* These files are included only here and not in the beginning since they need some of the
   declarations within this file */
#include "cmras.h"
#include "cmctrl.h"
#include "cmhook.h"
#include "faststart.h"
#include "rvh323watchdog.h"



/************************************************************************
 * cmPreGrantedArqUse
 * Indicates the way PreGranted ARQ is used by the EP for a call, even when the
 * gatekeeper (using preGrantedArq parameter) allows direct calls.
 ************************************************************************/
typedef enum
{
    cmPreGrantedArqDirect=0, /* PreGranted ARQ feature is used for both direct and routed calls. */
    cmPreGrantedArqRouted /* PreGranted ARQ feature is used only for routed calls. */
}cmPreGrantedArqUse;

typedef enum
{
    cmCallStateInit=-1,
    cmCallStateDialtone,
    cmCallStateProceeding,
    cmCallStateRingBack,
    cmCallStateConnected,
    cmCallStateDisconnected,
    cmCallStateIdle,
    cmCallStateOffering,
    cmCallStateTransfering,
    cmCallStateIncompleteAddress = cmCallStateTransfering + 3,
    cmCallStateWaitAddressAck
} cmCallState_e;

typedef enum
{
    cmCallStateModeDisconnectedBusy,
    cmCallStateModeDisconnectedNormal,
    cmCallStateModeDisconnectedReject,
    cmCallStateModeDisconnectedUnreachable,
    cmCallStateModeDisconnectedUnknown,
    cmCallStateModeDisconnectedLocal,

    cmCallStateModeConnectedControl, /* not used. reserved for backward compatability */
    cmCallStateModeConnectedCallSetup,
    cmCallStateModeConnectedCall,
    cmCallStateModeConnectedConference,

    /* The next 3 state modes are here for backward compatability */
    cmCallStateModeControlConnected=cmCallStateModeConnectedControl,
    cmCallStateModeCallSetupConnected,
    cmCallStateModeCallConnected,

    cmCallStateModeOfferingCreate=cmCallStateModeConnectedConference+1,
    cmCallStateModeOfferingInvite,
    cmCallStateModeOfferingJoin,
    cmCallStateModeOfferingCapabilityNegotiation,
    cmCallStateModeOfferingCallIndependentSupplementaryService,

    cmCallStateModeDisconnectedIncompleteAddress,

    cmCallStateModeTransferingForwarded,
    cmCallStateModeTransferingRouteToMC,
    cmCallStateModeTransferingRouteToGatekeeper
} cmCallStateMode_e;

typedef enum
{
    cmReasonTypeNoBandwidth,
    cmReasonTypeGatekeeperResource,
    cmReasonTypeUnreachableDestination,
    cmReasonTypeDestinationRejection,
    cmReasonTypeInvalidRevision,
    cmReasonTypeNoPermision,
    cmReasonTypeUnreachableGatekeeper,
    cmReasonTypeGatewayResource,
    cmReasonTypeBadFormatAddress,
    cmReasonTypeAdaptiveBusy,
    cmReasonTypeInConf,
    cmReasonTypeUndefinedReason,
    cmReasonTypeRouteCallToGatekeeper,
    cmReasonTypeCallForwarded,
    cmReasonTypeRouteCallToMC,
    cmReasonTypeFacilityCallDeflection,
    cmReasonTypeSecurityDenied,
    cmReasonTypeCalledPartyNotRegistered,
    cmReasonTypeCallerNotregistered,
    cmReasonTypeConferenceListChoice,
    cmReasonTypeStartH245,
    cmReasonTypeNewConnectionNeeded,
    cmReasonTypeNoH245,
    cmReasonTypeNewTokens,                  /* FacilityReason */
    cmReasonTypeFeatureSetUpdate,           /* FacilityReason */
    cmReasonTypeForwardedElements,          /* FacilityReason */
    cmReasonTypeTransportedInformation      /* FacilityReason */
} cmReasonType;



typedef enum
{
    cmCreate,
    cmJoin,
    cmInvite,
    cmCapabilityNegotiation,
    cmCallIndependentSupplementaryService,
    cmLastCG
} cmConferenceGoalType;


/************************************************************************
 * cmInformationTransferCapability
 * The Information transfer capability that is part of the Bearer Capability
 * element of Q931 messages.
 * cmITCSpeech              - Speech
 * cmITCUnrestricted        - Unrestricted digital information
 * cmITCRestricted          - Restricted digital information
 * cmITCAudio               - 3.1 kHz audio
 * cmITCUnrestrictedTones   - Unrestricted digital information with tones/announcements
 * cmITCVideo               - Video
 ************************************************************************/
typedef enum
{
    cmITCSpeech=0,
    cmITCUnrestricted=8,
    cmITCRestricted=9,
    cmITCAudio=16,
    cmITCUnrestrictedTones=17,
    cmITCVideo=24
} cmInformationTransferCapability;

typedef enum
{
    cmParamFirst=0x1000,
    cmParamSourceAddress,                   /* cmAlias */
    cmParamSourceInfo,                      /* cmEndpointType enum */
    cmParamDestinationAddress,              /* cmAlias */
    cmParamDestCallSignalAddress,           /* cmTransportAddress */
    cmParamSrcCallSignalAddress,            /* cmTransportAddress */
    cmParamDestExtraCallInfo,               /* cmAlias */
    cmParamActiveMc,                        /* RvBool */
    cmParamCID,                             /* char[16] */
    cmParamConferenceGoal,                  /* cmConferenceGoalType enum */
    cmParamCallType,                        /* cmCallType */
    cmParamSetupH245Address,                /* cmTransportAddress */
    cmParamH245Address,                     /* cmTransportAddress */
    cmParamDestinationInfo,                 /* cmEndpointType enum */
    cmParamReleaseCompleteReason,           /* cmReasonType */
    cmParamReleaseCompleteCause,            /* int */
    cmParamRate,                            /* int */
    cmParamRequestedRate,                   /* int */
    cmParamInformationTransferCapability,   /* cmInformationTransferCapability enum */
    cmParamMultiRate,                       /* RvBool */
    cmParamCalledPartyNumber,               /* cmAlias */
    cmParamCalledPartySubAddress,           /* cmAlias */
    cmParamCallingPartyNumber,              /* cmAlias */
    cmParamCallingPartySubAddress,          /* cmAlias */
    cmParamExtension,                       /* cmAlias */
    cmParamExtention = cmParamExtension,    /* cmAlias - reserved for backward compatability */
    cmParamAlternativeAddress,              /* cmTransportAddress */
    cmParamAlternativeAliasAddress,         /* cmAlias */
    cmParamFacilityReason,                  /* cmReasonType */
    cmParamDestinationIpAddress,            /* cmTransportAddress - physical address, used to determine actual destination of call */
    cmParamRemoteIpAddress,                 /* cmTransportAddress */
    cmParamCRV,                             /* int */
    cmParamRASCRV,                          /* int */
    cmParamCallSignalAddress,               /* cmTransportAddress */
    cmParamEstablishH245,                   /* RvBool */
    cmParamDisplay,                         /* char*        */
    cmParamFacilityCID,                     /* char[16] */
    cmParamConnectDisplay,                  /* char*        */
    cmParamUserUser,                        /* char*        */
    cmParamConnectUserUser,                 /* char*        */
    cmParamFullSourceInfo,                  /* RvPvtNodeId  */
    cmParamFullDestinationInfo,             /* RvPvtNodeId  */
    cmParamSetupNonStandardData,            /* RvPvtNodeId  */
    cmParamCallProceedingNonStandardData,   /* RvPvtNodeId  */
    cmParamAlertingNonStandardData,         /* RvPvtNodeId  */
    cmParamConnectNonStandardData,          /* RvPvtNodeId  */
    cmParamReleaseCompleteNonStandardData,  /* RvPvtNodeId  */
    cmParamSetupNonStandard,                /* cmNonStandardParam */
    cmParamCallProceedingNonStandard,       /* cmNonStandardParam */
    cmParamAlertingNonStandard,             /* cmNonStandardParam */
    cmParamConnectNonStandard,              /* cmNonStandardParam */
    cmParamReleaseCompleteNonStandard,      /* cmNonStandardParam */
    cmParamAlternativeDestExtraCallInfo,    /* cmAlias */
    cmParamAlternativeExtension,            /* cmAlias */
    cmParamAlternativeExtention = cmParamAlternativeExtension, /* cmAlias - reserved for backward compatability */
    cmParamSetupFastStart,                  /* RvPvtNodeId  */
    cmParamConnectFastStart,                /* RvPvtNodeId  */
    cmParamEarlyH245,                       /* RvBool */
    cmParamCallID,                          /* char[16] */
    cmParamPreGrantedArq,                   /* cmPreGrantedArqUse */
    cmParamAlertingFastStart,               /* RvPvtNodeId  */
    cmParamCallProcFastStart,               /* RvPvtNodeId  */
    cmParamAlertingH245Address,             /* cmTransportAddress*/
    cmParamCallProcH245Address,             /* cmTransportAddress*/
    cmParamCanOverlapSending,               /* RvBool */
    cmParamIncompleteAddress,               /* Reserved for backward compatability - not used*/
    cmParamH245Tunneling,                   /* RvBool */
    cmParamFastStartInUseObsolete,          /* Reserved for backward compatability - not used*/
    cmParamSetupCanOverlapSending,          /* RvBool */
    cmParamSetupSendingComplete,            /* RvBool */
    cmParamFullSourceAddress,               /* RvPvtNodeId  */
    cmParamFullDestinationAddress,          /* RvPvtNodeId  */
    cmParamRouteCallSignalAddress,          /* cmTransportAddress*/
    cmParamH245Stage,                       /* cmH245Stage */
    cmParamH245Parallel,                    /* RvBool */
    cmParamShutdownEmptyConnection,         /* RvBool */
    cmParamIsMultiplexed,                   /* RvBool */
    cmParamAnnexE,                          /* cmAnnexEUsageMode enum */
    cmParamDestinationAnnexEAddress,        /* cmTransportAddress - physical address used to determine actual destination of call */
    cmParamConnectedAddress,                /* cmAlias */
    cmParamFullDestExtraCallInfo,           /* RvPvtNodeId  */
    cmParamFullExtension,                   /* RvPvtNodeId  */
    cmParamSourceIpAddress,                 /* cmTransportAddress - physical address, used to determine address from which to initiate the call */
    cmParamState,                           /* cmCallState_e enum */
    cmParamStateMode,                       /* cmCallStateMode_e enum */
    cmParamAutomaticRAS,                    /* RvBool - are we using automatic RAS or manual RAS for this call? */
    cmParamMsdTerminalType                  /* RvInt32 - terminal type to use for MSD procedures. Should be set before H.245 control is created
                                               for the call. This parameter can only be set. */
} cmCallParam;


typedef enum
{
    cmParamLocalCallSignalAddress,          /* cmTransportAddress */
    cmParamLocalAnnexEAddress,              /* cmTransportAddress */
    cmParamLocalRASAddress                  /* cmTransportAddress */
} cmParam;


typedef int
(*cmNewCallbackEH)(void);


typedef enum
{
   cmIdle,
   cmDiscovered,
   cmRegistered
} cmRegState;

typedef enum
{
   cmGatekeeperConfirm,
   cmGatekeeperReject,
   cmRegistrationConfirm,
   cmRegistrationReject,
   cmUnregistrationRequest,
   cmUnregistrationConfirm,
   cmUnregistrationReject,
   cmNonStandardMessage,
   cmResourceAvailabilityConfirmation,
   cmPermanentAlternateGatekeeperConfirmation,
   cmSearchingAlternateGatekeeper
} cmRegEvent;

typedef struct
{
    RvUint8 cmCodingStandard;
    RvUint8 cmSpare;
    RvUint8 cmLocation;
    RvUint8 cmRecomendation; /*  optional set to -1 if not used */
    RvUint8 cmCauseValue;
} cmCallCauseInfo;

typedef struct
{
    RvUint8 cmCodingStandard;
    RvUint8 cmCallStateValue;
} cmCallStateInfo;

typedef struct
{
    cmCallCauseInfo  callCauseInfo;
    cmCallStateInfo callStateInfo;
    char display[128];
} cmCallStatusMessage;



typedef int
    (RVCALLCONV *cmEvNewCallT)(
        IN      HAPP                hApp,
        IN      HCALL               hsCall,
        OUT     LPHAPPCALL          lphaCall);


typedef int
    (RVCALLCONV *cmEvRegEventT)(
        IN      HAPP                hApp,
        IN      cmRegState          regState,
        IN      cmRegEvent          regEvent,
        IN      RvPvtNodeId         regEventHandle);

//add by daiqing 20100723 for 460 kdv
typedef int
(RVCALLCONV *cmEv460NewCallT)(
							  IN      HAPP                hApp,
							  IN      HCALL               hsCall,
							  OUT     LPHAPPCALL          lphaCall);
//end

typedef  struct
{
    cmEvNewCallT cmEvNewCall;
    cmEvRegEventT cmEvRegEvent;
	//add by daiqing 20100723 for 460 kdv
	cmEv460NewCallT cmEv460NewCall;
	//end
} SCMEVENT,*CMEVENT;

//add by yj for 460server 
typedef int
(RVCALLCONV *cmEvMessageChangeT)( IN HCALL hsCall, IN OUT int message);

typedef  struct
{
	cmEvMessageChangeT cmEvMessageChange;
} SCMMSGEVENT,*CMMSGEVENT;
//end

typedef int
    (RVCALLCONV *cmEvCallStateChangedT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      cmCallState_e       state,
        IN      cmCallStateMode_e   stateMode);


typedef int
    (RVCALLCONV *cmEvCallNewRateT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvUint32            rate);

typedef int
    (RVCALLCONV*cmEvCallInfoT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      char*               display,
        IN      char*               userUser,
        IN      int                 userUserSize);

typedef int
    (RVCALLCONV*cmEvCallFacilityT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 handle,
        OUT IN  RvBool              *proceed);

typedef int
    (RVCALLCONV * cmEvCallFastStartSetupT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        OUT IN  cmFastStartMessage  *fsMessage);

typedef int
    (RVCALLCONV*cmEvCallFastStartT)(
        IN  HAPPCALL                haCall,
        IN  HCALL                   hsCall);


typedef int
    (RVCALLCONV * cmEvCallStatusT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        OUT IN  cmCallStatusMessage *callStatusMsg);

typedef int
    (RVCALLCONV * cmEvCallUserInfoT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 nodeId);

typedef int
    (RVCALLCONV * cmEvCallH450SupplServT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      cmCallQ931MsgType   msgType,
        IN      int                 nodeId,
        IN      int                 size);

typedef int
    (RVCALLCONV * cmEvCallIncompleteAddressT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall);

typedef int
    (RVCALLCONV * cmEvCallAdditionalAddressT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      char*               address,
        IN      RvBool              sendingComplete);


/********************************************************************************************
 * cmEvCallProgressT
 * purpose : Notification function for the application that a Q931 Progress message was
 *           received on a call.
 * input   : haCall         - Application's handle for the call
 *           hsCall         - Stack's handle for the call
 *           handle         - The PVT root node ID of the Progress message
 * output  : none
 ********************************************************************************************/
typedef int
    (RVCALLCONV*cmEvCallProgressT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 handle);

/********************************************************************************************
 * cmEvCallNotiftyT
 * purpose : Notification function for the application that a Q931 Notify message was
 *           received on a call.
 * input   : haCall         - Application's handle for the call
 *           hsCall         - Stack's handle for the call
 *           handle         - The PVT root node ID of the Notify message
 * output  : none
 ********************************************************************************************/
typedef int
    (RVCALLCONV*cmEvCallNotifyT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 handle);

/********************************************************************************************
 * cmEvCallNewAnnexLMessageT
 * purpose : Notification function for the application that there has been a new Annex L
 *           message inside an incoming Q931 message.
 *           Applications that want to implement Annex L capabilities should implement
 *           this function.
 * input   : haCall         - Application's handle for the call
 *           hsCall         - Stack's handle for the call
 *           annexLElement  - PVT node ID of the Annex L element inside a Q931 message
 *           msgType        - The type of the message from which we extracted the Annex L
 *                            message
 * output  : none
 * note    : cmCallGetAnnexLMessage can be used to analyze the Annex L message
 ********************************************************************************************/
typedef int
    (RVCALLCONV*cmEvCallNewAnnexLMessageT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 annexLElement,
        IN      int                 msgType);

/********************************************************************************************
 * cmEvCallNewAnnexMMessageT
 * purpose : Notification function for the application that there has been a new Annex M
 *           message inside an incoming Q931 message.
 *           Applications that want to implement Annex M capabilities should implement
 *           this function.
 * input   : haCall         - Application's handle for the call
 *           hsCall         - Stack's handle for the call
 *           annexMElement  - PVT node ID of the Annex M element inside a Q931 message
 *           msgType        - The type of the message from which we extracted the Annex M
 *                            message
 * output  : none
 * note    : cmCallGetAnnexMMessage can be used to analyze the Annex M message
 ********************************************************************************************/
typedef int
    (RVCALLCONV*cmEvCallNewAnnexMMessageT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 annexMElement,
        IN      int                 msgType);

/********************************************************************************************
 * cmEvCallRecvMessageT
 * purpose : Notification function for the application that an incoming Q931 or H245 message
 *           was received by the stack and is about to be processed.
 * input   : haCall     - Application's handle for the call
 *           hsCall     - Stack's handle for the call
 *           message    - PVT node ID of the message that was received
 * output  : none
 ********************************************************************************************/
typedef int
    (RVCALLCONV*cmEvCallRecvMessageT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 message);

/********************************************************************************************
 * cmEvCallSendMessageT
 * purpose : Notification function for the application that an outgoing Q931 or H245 message
 *           is about to be sent by the stack.
 * input   : haCall     - Application's handle for the call
 *           hsCall     - Stack's handle for the call
 *           message    - PVT node ID of the message that is about to be sent
 * output  : none
 ********************************************************************************************/
typedef int
    (RVCALLCONV*cmEvCallSendMessageT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      int                 message);


/******************************************************************************
 * cmEvCallStatusEnquiryTimeoutT
 * ----------------------------------------------------------------------------
 * General: Notification function for the application that an outgoing Q931
 *          StatusEnquiry message has timedout and no Status response has been
 *          received on it.
 *          StatusEnquiry messages are sent by cmCallStatusEnquiry().
 *
 * Return Value: None
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  haCall     - Application's handle for the call.
 *         hsCall     - Stack's handle for the call.
 * Output: None.
 *****************************************************************************/
typedef void
    (RVCALLCONV*cmEvCallStatusEnquiryTimeoutT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall);





typedef  struct
{
    cmEvCallStateChangedT           cmEvCallStateChanged;
    cmEvCallNewRateT                cmEvCallNewRate;
    cmEvCallInfoT                   cmEvCallInfo;
    void*                           cmCallNonStandardParam; /* This callback is obsolete. Here for backward compatability */
    cmEvCallFacilityT               cmEvCallFacility;
    cmEvCallFastStartSetupT         cmEvCallFastStartSetup;
    cmEvCallStatusT                 cmEvCallStatus;
    cmEvCallUserInfoT               cmEvCallUserInfo;
    cmEvCallH450SupplServT          cmEvCallH450SupplServ;
    cmEvCallIncompleteAddressT      cmEvCallIncompleteAddress;
    cmEvCallAdditionalAddressT      cmEvCallAdditionalAddress;
    void*                           cmEvCallTunnNewMessage; /* This callback is obsolete. Here for backward compatability */
    cmEvCallFastStartT              cmEvCallFastStart;
    cmEvCallProgressT               cmEvCallProgress;
    cmEvCallNotifyT                 cmEvCallNotify;
    cmEvCallNewAnnexLMessageT       cmEvCallNewAnnexLMessage;
    cmEvCallNewAnnexMMessageT       cmEvCallNewAnnexMMessage;
    cmEvCallRecvMessageT            cmEvCallRecvMessage;
    cmEvCallSendMessageT            cmEvCallSendMessage;
    cmEvCallStatusEnquiryTimeoutT   cmEvCallStatusEnquiryTimeout;
} SCMCALLEVENT,*CMCALLEVENT;



/********************************************************************************************
 * cmStartUp
 * purpose : This function should be called prior to any cmInit() or cmInitialize() calls.
 *           It must be called when several cmInitialize() functions are called from
 *           different threads at the same time.
 * input   : none
 * output  : none
 * return  : RV_OK on success, or a negative value on failure
 ********************************************************************************************/
RVAPI
RvStatus RVCALLCONV cmStartUp(void);


/********************************************************************************************
 * cmShutdown
 * purpose : This function should be called when all stack instances where destructed using
 *           cmEnd(), before shutting down the application.
 *           It should be called only if cmStartUp() was called.
 * input   : none
 * output  : none
 * return  : RV_OK on success, or a negative value on failure
 * notes   : cmShutdown() must be called from the same thread that called cmStartUp().
 ********************************************************************************************/
RVAPI
RvStatus RVCALLCONV cmShutdown(void);


/************************************************************************
 * cmInitialize
 * purpose: Initializes the Conference Manager instance.
 *          This function must be called before any other H.323 function
 *          except cmGetVersion().
 * input  : name    - Configuration file name to use
 * output : lphApp  - Application handle created for the initialized
 *                    stack instance
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmInitialize(IN const RvChar * name, OUT LPHAPP lphApp);

/************************************************************************
 * cmInit
 * purpose: Initializes the Conference Manager instance.
 *          This function must be called before any other H.323 function
 *          except cmGetVersion().
 * input  : name    - Configuration file name to use
 * output : none
 * return : Stack's application handle on success
 *          NULL on failure
 ************************************************************************/
RVAPI
HAPP RVCALLCONV cmInit(IN const RvChar * name);

/************************************************************************
 * cmEnd
 * purpose: Shuts down the Conference Manager instance and releases all
 *          resources that were in use by the Conference Manager instance.
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 * notes  : cmEnd() must be called from the same thread that called cmInitialize().
 ************************************************************************/
RVAPI
int RVCALLCONV cmEnd(
        IN   HAPP        hApp);


/************************************************************************
 * cmStart
 * purpose: Starts the stack's activity
 *
 *          This function is only applicable when system.manualStart key
 *          is defined in the configuration. In manualStart mode cmInitialize()
 *          function does not automatically start Stack activity and accesses only
 *          the "system" configuration tree. To start the Stack use cmStart() function.
 *          The application may change configuration settings between cmInitialize()
 *          and cmStart() using cmGetXXXConfigurationHandle() functions.
 *
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmStart(
        IN  HAPP        hApp);

/************************************************************************
 * cmStop
 * purpose: Stops the stack's activity
 *
 *          After the Stack is stopped by cmStop(), the application may change
 *          configuration settings and then use cmStart() to start Stack
 *          activity again.
 *
 * input  : hApp    - Stack handle for the application
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmStop(
        IN  HAPP        hApp);


/************************************************************************
* cmSetH460Feature
* purpose: set support h460
* input  : bSupportH460  - support h460
* output : none
* return : none
************************************************************************/
RVAPI
void RVCALLCONV cmSetH460Feature(IN RvBool bSupportH460);

//begin with added by wangxiaoyi on 20 September 2010
/************************************************************************
* cmSetH460MediaTraversalServer
* purpose: set h460 MediaTraversalServer. It is global.
* input  : BOOL bMediaTraversalServer
* output : none
* return : none
************************************************************************/
RVAPI
void RVCALLCONV cmSetH460MediaTraversalServer(IN RvBool bMediaTraversalServer);

/************************************************************************
* cmSetH460SupportTransMultiMedia
* purpose: set h460 SupportTransMultiMedia. It is global.
* input  : BOOL bSupportTransMultiMedia
* output : none
* return : none
************************************************************************/
RVAPI
void RVCALLCONV cmSetH460SupportTransMultiMedia(IN RvBool bSupportTransMultiMedia);
//end with added by wangxiaoyi on 20 September 2010

/************************************************************************
* cmSetSendFacility
* purpose: set send facility
* input  : bSendFacility  - send facility after acr
* output : none
* return : none
************************************************************************/
RVAPI
void RVCALLCONV cmSetInternal(HCALL hsCall , IN RvBool bInternal);

/*end*/

/************************************************************************
 * cmRegister
 * purpose: Registers the endpoint with the gatekeeper.
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp    - Application's handle of the stack
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmRegister(
        IN  HAPP        hApp);

/************************************************************************
 * cmUnregister
 * purpose: Unregisters the endpoint from the Gatekeeper
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp    - Application's handle of the stack
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmUnregister(
        IN  HAPP        hApp);


/************************************************************************
 * cmGetRASConfigurationHandle
 * purpose: Gets the root Node ID of the RAS configuration tree.
 *          The application can then get and change configuration parameters
 *          for the control procedures.
 * input  : hApp     - Application's stack handle
 * output : none
 * return : The PVT Node ID of the RASConfiguration subtree on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV cmGetRASConfigurationHandle(
        IN  HAPP             hApp);



/************************************************************************
 * cmSendNonStandardMessage
 * purpose: Sends a nonstandard message to the Gatekeeper
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp    - Application's handle of the stack
 *          nsParam - Nonstandard parameter to be used in non standard message
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI
int RVCALLCONV cmSendNonStandardMessage(
     IN      HAPP                hApp,
     IN      cmNonStandardParam* nsParam);

/************************************************************************
 * cmGetVersion
 * purpose: Returns the version of the Conference Manager in use.
 * input  : none
 * output : none
 * return : Pointer to the string representing the version of the
 *          Conference Manager. For example, "3.0.0.0" or "2.5".
 ************************************************************************/
RVAPI
char* RVCALLCONV cmGetVersion(void);

/************************************************************************
 * cmGetVersionName
 * purpose: Returns the version of the Conference Manager in use.
 * input  : buff    - Buffer to set the version in
 *          length  - Maximum length of the buffer in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmGetVersionName(
        IN    char*   buff,
        IN    int     length);


RVAPI
int RVCALLCONV
cmSetPreCallbackEvent(
        IN      HAPP                hApp,
        IN      cmNewCallbackEH     newCallback
        );

/************************************************************************
 * cmSetGenEventHandler
 * purpose: Installs the general event handler
 * input  : hApp    - Stack handle for the application
 *          cmEvent - Pointer to array of pointers to general event callback functions
 *          size    - Size of *cmEvent in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmSetGenEventHandler(
        IN      HAPP                hApp,
        IN      CMEVENT             cmEvent,
        IN      int                 size);

RVAPI
int RVCALLCONV cmGetGenEventHandler(
        IN      HAPP                hApp,
        IN      CMEVENT             cmEvent,
        IN      int                 size);


/************************************************************************
 * cmSetCallEventHandler
 * purpose: Installs the call event handler
 * input  : hApp        - Stack handle for the application
 *          cmCallEvent - Pointer to array of pointers to general event callback functions
 *          size        - Size of *cmCallEvent in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmSetCallEventHandler(
        IN      HAPP                hApp,
        IN      CMCALLEVENT         cmCallEvent,
        IN      int                 size);

RVAPI
int RVCALLCONV cmGetCallEventHandler(
        IN      HAPP                hApp,
        OUT     CMCALLEVENT         cmCallEvent,
        IN      int                 size);

//add by yj for 460server
//修改转发消息的回调
RVAPI
int RVCALLCONV cmSetMSGEventHandler(
									IN      HAPP                hApp,
									IN      CMMSGEVENT			cmMessageEvent,
									IN      int                 size);

RVAPI
int RVCALLCONV cmGetMSGEventHandler(
									IN      HAPP                hApp,
									OUT     CMMSGEVENT			cmMessageEvent,
									IN      int                 size);
//end

//add by daiqing 20100723 for 460 kdv
RVAPI
int RVCALLCONV cmCallSendSetup(IN  HCALL       hsCall,
							   IN  RvUint32      maxRate,
							   IN  RvUint32      minRate,
							   IN  char*       destAddress,
							   IN  char*       srcAddress,
							   IN  char*       display,
							   IN  char*       userUser,
							   IN  int         userUserSize);
//end

/************************************************************************
 * cmCallNew
 * purpose: Creates a new call object that belongs to a particular Stack instance.
 *          This function does not launch any action on the network. It only causes
 *          the application and the Stack to exchange handles.
 * input  : hApp        - Stack handle for the application
 *          haCall      - Application's handle for the call
 * output : lphsCall    - Stack handle for the call to dial
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallNew(
        IN   HAPP        hApp,
        IN   HAPPCALL    haCall,
        OUT  LPHCALL     lphsCall);


/************************************************************************
 * cmCallDial
 * purpose: Dials a call. This function together with cmCallSetParam()
 *          can be used instead of cmCallMake(). cmCallSetParam() should
 *          be called before cmCallDial() and the required parameters of
 *          the call should be set
 * input  : hsCall      - Stack handle for the call to dial
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallDial(
        IN  HCALL       hsCall);


/************************************************************************
 * cmCallMake
 * purpose: Starts a new call setup with the given parameters
 * input  : hsCall      - Stack handle for the new call
 *          maxRate     - Maximum rate allowed for the new call
 *          minRate     - Minimum rate allowed for the new call.
 *                        This parameter is not currently in use, so set it to zero (0).
 *          destAddress - Called party address list
 *          srcAddress  - Calling party address list
 *          display     - String representing display information for reporting
 *                        to the called party
 *          userUser    - String representing user-to-user information for reporting
 *                        to the called party
 *          userUserSize- Length of the string representing user-to-user information
 *                        to report to the called party
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallMake(
        IN  HCALL           hsCall,
        IN  RvUint32        maxRate,
        IN  RvUint32        minRate,
        IN  const RvChar*   destAddress,
        IN  const RvChar*   srcAddress,
        IN  const RvChar*   display,
        IN  const RvChar*   userUser,
        IN  int             userUserSize);


/******************************************************************************
 * cmCallSetAddressesFromStrings
 * ----------------------------------------------------------------------------
 * General: This function can be used to set the call object's dialing
 *          properties from string variables, in the same manner that
 *          cmCallMake() parses and sets these parameters.
 *
 * Return Value: If an error occurs, the function returns a negative value.
 *               If no error occurs, the function returns RV_OK.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall       - Call object to modify.
 *         destAddress  - Called party address list.
 *                        If this parameter's value is NULL, it is ignored.
 *         srcAddress   - Calling party address list.
 *                        If this parameter's value is NULL, it is ignored.
 * Output: None.
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmCallSetAddressesFromStrings(
        IN  HCALL           hsCall,
        IN  const RvChar*   destAddress,
        IN  const RvChar*   srcAddress);


RVAPI
int RVCALLCONV cmCallFacility(
           IN HCALL     hsCall,
           IN int       handle);

RVAPI
int RVCALLCONV cmSetCallHandle(
        IN HCALL                    hsCall,
        IN HAPPCALL                 haCall);


RVAPI
int RVCALLCONV cmCallGetDisplayInfo(
        IN  HCALL           hsCall,
        OUT char*           display,
        IN  int             displaySize);

RVAPI
int RVCALLCONV cmCallGetUserUserInfo(
        IN  HCALL           hsCall,
        OUT char*           userUser,
        IN  int             userUserSize);

RVAPI
int RVCALLCONV cmCallAnswerExt(
        IN  HCALL           hsCall,
        IN  const RvChar*   display,
        IN  const RvChar*   userUser,
        IN  int             userUserSize);

RVAPI
int RVCALLCONV cmCallAnswerDisplay(
        IN  HCALL           hsCall,
        IN  const RvChar*   display);

/************************************************************************
 * cmCallAnswer
 * purpose: Answers an incoming call
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallAnswer(
        IN    HCALL       hsCall);


/************************************************************************
 * cmCallAccept
 * purpose: Accepts an incoming call and sends Q.931 ALERTING message to
 *          calling party
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallAccept(
        IN    HCALL       hsCall);


/************************************************************************
 * cmCallDrop
 * purpose: Drops a call
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallDrop(IN  HCALL       hsCall);


RVAPI
int RVCALLCONV cmCallDropParam(
        IN  HCALL           hsCall,
        IN  cmReasonType    cmReason);


RVAPI
int RVCALLCONV cmCallClose(
        IN      HCALL               hsCall);

RVAPI
int RVCALLCONV cmCallGetCallingPartyId(
        IN      HCALL               hsCall,
        OUT     char*               callerId,
        IN      int                 size);

#define cmCallGetCallerId cmCallGetCallingPartyId

RVAPI
int RVCALLCONV cmCallGetCalledPartyId(
        IN      HCALL               hsCall,
        OUT     char*               calledPartyId,
        IN      int                 size);

RVAPI
int RVCALLCONV cmCallGetRate(
        IN      HCALL               hsCall,
        OUT     RvUint32*           rate);

RVAPI
int RVCALLCONV cmCallGetOrigin(
        IN      HCALL               hsCall,
        OUT     RvBool*             origin);

RVAPI
int RVCALLCONV cmCallSetRate(
        IN      HCALL               hsCall,
        IN      RvUint32            rate);


RVAPI
int RVCALLCONV cmCallForward(
        IN  HCALL           hsCall,
        IN  const RvChar    *destAddress);


RVAPI
int RVCALLCONV cmCallGetNumOfParams(
        IN      HCALL               hsCall,
        IN      cmCallParam         param);


RVAPI
int RVCALLCONV cmCallGetParam(
        IN      HCALL               hsCall,
        IN      cmCallParam         param,
        IN      int                 index,
        INOUT   RvInt32*            value,
        OUT     char *              svalue);

RVAPI
int RVCALLCONV cmCallSetParam(
        IN      HCALL               hsCall,
        IN      cmCallParam         param,
        IN      int                 index,
        IN      RvInt32             value,
        IN      const RvChar *      svalue);


/************************************************************************
 * cmCallDeleteParam
 * purpose: Delete a parameter that was set in the call.
 * input  : hsCall      - Stack handle for the call
 *          param       - Parameter to delete
 *          index       - Index of parameter to delete (1-based)
 *                        Should be 0 for non-array parameters
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallDeleteParam(
        IN      HCALL               hsCall,
        IN      cmCallParam         param,
        IN      int                 index);


/************************************************************************
 * cmCallGetMessage
 * purpose: Get a Q931 message for the given call. It creates the necessary
 *          message or just take it from the property database of the call.
 *          This function cannot be used when using DoNotUseProperty
 *          property mode on the calls (determined through the configuration).
 *          It is suggested to use this function when trying to add elements
 *          to an outgoing Q931 message that are not given in cmCallSetParam()
 * input  : hsCall      - Stack's call handle
 *          messageType - Type of message to get
 * output : none
 * return : PVT nodeId of the message on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV cmCallGetMessage(
    IN  HCALL               hsCall,
    IN  cmCallQ931MsgType   messageType);


RVAPI
HPVT RVCALLCONV cmGetValTree(
                IN  HAPP             hApp);



RVAPI
HPST RVCALLCONV cmGetSynTreeByRootName(
                IN  HAPP            hApp,
                IN  char*           name);

RVAPI
RvInt32 RVCALLCONV cmGetQ931ConfigurationHandle(
                IN  HAPP            hApp);


RVAPI
int RVCALLCONV cmCallJoin(
                IN      HCALL               hsCall,
                IN      HCALL               hsSameConferenceCall);

RVAPI
int RVCALLCONV cmCallInvite(
                IN      HCALL               hsCall,
                IN      HCALL               hsSameConferenceCall);

RVAPI
int RVCALLCONV cmCallIsSameConference(
        IN  HCALL           hsCall,
        IN  HCALL           hsAnotherCall);

RVAPI char* RVCALLCONV
cmGetProtocolMessageName(
                IN      HAPP                hApp,
                IN      int                 msg);



RVAPI
int RVCALLCONV cmCallGetRemoteEndpointInfoHandle(
        IN  HCALL       hsCall);

RVAPI
int RVCALLCONV cmCallGetLocalEndpointInfoHandle(
        IN  HCALL       hsCall);

RVAPI
int RVCALLCONV cmGetEndpointInfoHandle(
        IN  HAPP        hApp);



/******************************************************************************
 * cmGetParam
 * ----------------------------------------------------------------------------
 * General: Gets a specific Stack instance parameter for the application.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - The Stack handle for the application.
 *         param    - The requested parameter.
 *         index    - The instance of the parameter specified in param.
 *                    The index of a single-instance parameter is 0.
 *                    The index of a multiple-instance parameter is 1-based.
 *         value    - If the parameter value is a simple integer, the input
 *                    value of this parameter is ignored.
 *                    If the parameter value is a structure, value represents
 *                    the maximum length of svalue in bytes.
 * Output: value    - If the parameter value is a simple integer, value is the
 *                    pointer to the parameter value.
 *                    If the parameter value is a structure, value represents
 *                    the length of the parameter.
 *         svalue   - If the parameter value is a structure, svalue
 *                    (structured value) represents the pointer to the parameter
 *                    itself.
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmGetParam(
    IN      HAPP         hApp,
    IN      cmParam      param,
    IN      RvInt        index,
    INOUT   RvInt32*     value,
    OUT     RvChar*      svalue);


RVAPI int RVCALLCONV
cmSetHandle(
        IN      HAPP                hApp,
        IN      HAPPAPP     haApp);

RVAPI int RVCALLCONV
cmGetHandle(
        IN      HAPP                hApp,
        IN      HAPPAPP*        haApp);

/* nonStandard handling_______________________________________________________________*/

RVAPI
int RVCALLCONV cmCallConnectControl(
                IN      HCALL               hsCall
                );

RVAPI
int RVCALLCONV
cmCallSw2SeparateH245(IN      HCALL               hsCall);

RVAPI
int RVCALLCONV cmCallSetIndependentSupplementaryService(
                IN      HCALL               hsCall
                );

/******************************************************************************
 * cmCallSetControlRemoteAddress
 * ----------------------------------------------------------------------------
 * General: Manually sets the remote address to which the stack will try
 *          to connect the H.245 connection.
 *
 * Return Value: RV_OK - if successful.
 *               Negative on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall   - TStack handle for the call
 *         addr     - The remote address
 * Output: none
 *****************************************************************************/
RVAPI
int RVCALLCONV cmCallSetControlRemoteAddress(
                IN      HCALL               hsCall,
                IN      cmTransportAddress  *addr
                );

/************************************************************************
 * cmCallSendSuppServiceManually
 * purpose: Sends an H.450 APDU on the next Q.931 message that the
 *          application initiates. It will not initiate a FACILITY message
 *          on its own as cmCallSendSupplamentaryService() might send.
 * input  : hsCall      - Stack handle for the call
 *          buffer      - Buffer in which the APDU has been placed
 *          bufferSize  - Size of the buffer
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallSendSuppServiceManually(
    IN  HCALL       hsCall,
    IN  void*       buffer,
    IN  int         bufferSize);

#define CM_H450_NO_FLAGS 0x00000000
#define CM_H450_FORCE    0x00000001
#define CM_H450_ALERTING 0x00000002

/************************************************************************
 * cmCallSendSupplementaryService
 * purpose: Sends an H.450 APDU. This function takes a buffer of a
 *          Supplementary Service and puts an H.450 message in the buffer
 * input  : hsCall      - Stack handle for the call
 *          buffer      - Buffer in which the APDU has been placed
 *          bufferSize  - Size of the buffer
 *          flags:
 *             CM_H450_NO_FLAGS - will send on the next outgoing message, and will
 *                         initiate a facility messgae if call is connected.
 *             CM_H450_FORCE    - will force the stack to send a facility message
 *                         with the APDU.
 *             CM_H450_ALERTING - will send the APDU only with an alerting or a
 *                         connect message
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallSendSupplementaryService(
    IN  HCALL       hsCall,
    IN  void*       buffer,
    IN  int         bufferSize,
    IN  RvBool      force);


RVAPI
RvInt32 RVCALLCONV cmGetLocalCallSignalAddress(
                IN  HAPP             hApp,
                OUT cmTransportAddress*  tr);


/************************************************************************
 * cmGetLocalAnnexEAddress
 * purpose: Gets the local AnnexE address
 *          Annex E is used to send Q931 messages on UDP.
 * input  : hApp    - Application's stack handle
 * output : tr      - The local Annex E transport address.
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV cmGetLocalAnnexEAddress(
               IN   HAPP                hApp,
               OUT  cmTransportAddress* tr);


/************************************************************************
 * cmGetLocalRASAddress
 * purpose: Gets the local RAS address
 * input  : hApp    - Application's stack handle
 * output : tr      - The local RAS signal transport address.
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
RvInt32 RVCALLCONV cmGetLocalRASAddress(
                IN  HAPP             hApp,
                OUT cmTransportAddress*  tr);

RVAPI
RvInt32 RVCALLCONV cmGetGKCallSignalAddress(
                IN  HAPP             hApp,
                OUT cmTransportAddress*  tr);

RVAPI
RvInt32 RVCALLCONV cmGetGKRASAddress(
                IN  HAPP             hApp,
                OUT cmTransportAddress*  tr);

RVAPI
HPROTCONN RVCALLCONV cmGetRASConnectionHandle(
                IN  HAPP             hApp);



RVAPI
int RVCALLCONV cmAlias2Vt(
            IN      HPVT                valH,
            IN      cmAlias*            alias,
            IN      int                 nodeId);


/************************************************************************
 * cmVt2Alias
 * purpose: Converts a PVT subtree of AliasAddress type into an Alias
 * input  : hVal    - The PVT handle
 *          nodeId  - PVT subtree of AliasAddress type we want to convert
 * output : alias   - Alias struct converted. The application is responsible
 *                    to supply the string field inside the cmAlias struct
 *                    with enough allocated size.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV cmVt2Alias(
    IN  HPVT       hVal,
    OUT cmAlias*   alias,
    IN  int        nodeId);


/******************************************************************************
 * cmGetDefaultQ931Messages
 * ----------------------------------------------------------------------------
 * General: Gets the PVT node ID of all default values of Q931 messages that
 *          are used for new incoming and outgoing calls.
 *          The application can use this API to modify the defaults set by the
 *          Stack automatically to its likings (such a change might be removing
 *          the presentationIndicator and screeningIndicator that are set on
 *          messages by default).
 *          It is strongly advised to modify these values before starting to
 *          dial or receive calls, since the way the modifications are handled
 *          for active calls is indeterminate.
 *
 * Return Value: PVT node ID of the default Q931 messages.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp    - Application's handle of the stack
 * Output: None.
 *****************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV cmGetDefaultQ931Messages(
    IN  HAPP    hApp);


/******************************************************************************
 * cmGetProperty
 * ----------------------------------------------------------------------------
 * General: Gets the PVT node ID of the Property database of the protocol
 *          created by the Stack.
 *
 * Return Value: PVT node ID of the Property database.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsProtocol   - Protocol handle.
 *                        Should be of one of the following types:
 *                        HCALL, HRAS.
 * Output: None.
 *****************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV cmGetProperty(
    IN  HPROTOCOL   hsProtocol);


/******************************************************************************
 * cmGetAHandle
 * ----------------------------------------------------------------------------
 * General: Gets the application handle of the Stack instance associated
 *          with the protocol.
 *
 * Return Value: Application handle of the Stack instance associated with the
 *               protocol.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsProtocol   - Protocol handle.
 *                        Should be of one of the following types:
 *                        HCALL, HRAS, HCHAN, HPROTCONN.
 * Output: None.
 *****************************************************************************/
RVAPI HAPP RVCALLCONV cmGetAHandle(
    IN  HPROTOCOL   hsProtocol);


/******************************************************************************
 * cmCallStatusEnquiry
 * ----------------------------------------------------------------------------
 * General: Builds and sends the STATUS ENQUIRY message to the other side.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall   - The Stack handle for the call.
 *         display  - The text part of the STATUS ENQUIRT message.
 *                    This parameter is a NULL terminated string.
 *                    Setting the display parameter to NULL will send out the
 *                    STATUS ENQUIRY message without the display Information
 *                    Element.
 * Output: None.
 *****************************************************************************/
RVAPI int RVCALLCONV cmCallStatusEnquiry(
    IN  HCALL           hsCall,
    IN  const RvUint8   *display);


/*
cmGetH225RemoteVersion
return H225 version number of remote side via OUT parameter
version.
version is user allocated string*/

RVAPI
int RVCALLCONV cmGetH225RemoteVersion(IN HCALL        hsCall,OUT char * version);

/*
cmCallUserInformationCreate
create root of information message .Build all must parameters.
Add display and nonstandard parameter if they are and return root id to user to
do changes if it need
*/
RVAPI
int RVCALLCONV cmCallUserInformationCreate(
    IN HCALL                hsCall,
    IN const RvChar         *display,
    IN cmNonStandardParam   *param);

/*
cmCallUserInformationSend
send user information message
*/
RVAPI
int RVCALLCONV cmCallUserInformationSend(IN HCALL hsCall, IN int nodeId);

/*
cmCallUserInformationGet
Get rootId of the user information message and retrieve display and nonStandard parameter
Root Id of incoming user information message is passed to callback function of  cmEvCallUserInfoT type
if it is set by user.
*/
RVAPI
int RVCALLCONV cmCallUserInformationGet(IN HAPP hApp,IN int nodeId,
                      OUT char *display,IN int displaySize,
                      OUT cmNonStandardParam* param);

/************************************************************************
 * cmCallSendCallProceeding
 * purpose: Sends Q.931 CALL PROCEEDING message
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallSendCallProceeding(IN HCALL hsCall);


/* delete pvt of connect,setup,alerting and callProceeding from call property  to reduce memory
 can be run only in connected state.All information from this messages will be lost after runing
this function.*/
RVAPI
int RVCALLCONV cmFreeProperty(IN HCALL hsCall);


RVAPI
int RVCALLCONV cmCallOverlapSending(
    IN HCALL        hsCall,
    IN const RvChar *address);



RVAPI
int RVCALLCONV cmCallIncompleteAddress(IN HCALL hsCall);


RVAPI
int RVCALLCONV cmCallAddressComplete(IN HCALL hsCall);

RVAPI
int RVCALLCONV cmCallOverlapSendingExt(
    IN HCALL        hsCall,
    IN const RvChar *address,
    IN RvBool       sendingComplete);

/************************************************************************
 * cmSendRAI
 * purpose: Sends a RAI message to the gatekeeper.
 *          It is only applicable when the RAS.manualRAS key is not
 *          defined in the configuration (automatic RAS mode).
 *          It reads registration information from RAS.registrationInfo
 *          configuration key.
 * input  : hApp                    - Application's handle of the stack
 *          almoustOutOfResources   - Indicates that it is or is not almost
 *                                    out of resources
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RVAPI int RVCALLCONV
cmSendRAI(
       IN      HAPP                hApp,
       IN      RvBool   almostOutOfResources);

#define cmCallSetHandle cmSetCallHandle
RVAPI
int RVCALLCONV cmSetCallHandle(IN HCALL hsCall, IN HAPPCALL haCall);


/************************************************************************
 * cmCallGetHandle
 * purpose: Returns the application handle for a call from the call handle.
 * input  : hsCall      - Stack handle for the call
 * output : haCall      - Application handle for the call
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmCallGetHandle(IN HCALL hsCall, OUT HAPPCALL* haCall);


/************************************************************************
 * cmGetConfiguration
 * purpose: Gives access to the session's configuration context.
 *          The returned HCFG should be considered read-only, and should
 *          never be destroyed.
 * input  : hApp    - Stack instance handle
 * output : none
 * return : HCFG handle of the configuration on success
 *          NULL on failure
 ************************************************************************/
RVAPI
HCFG RVCALLCONV cmGetConfiguration(IN HAPP hApp);


/************************************************************************
 * cmGetDelimiter
 * purpose: Returns the character used as a delimiter between fields,
 *          such as in the destAddress and srcAddress fields in the
 *          cmCallMake function.
 * input  : hApp    - Stack instance handle
 * output : none
 * return : Delimiter character used
 ************************************************************************/
RVAPI char RVCALLCONV
cmGetDelimiter(HAPP hApp);


RVAPI int RVCALLCONV
cmSetPartnerHandle(HAPP hApp, HAPPPARTNER hAppPartner);

RVAPI int RVCALLCONV
cmGetPartnerHandle(HAPP hApp, HAPPPARTNER* hAppPartner);


/************************************************************************************
 *
 * cmCallMultiplex
 *
 * Purpose: To specify call's Q.931 multiplex behavior.
 *          This function call marks the call as supporting the Q.931 multiplexing
 *
 * Input:   hsCall                  - The stack handle to the call
 *          hsSameConnectionCall    - The stack handle to the call which Q.931
 *                                    connection is the prefered connection for the hsCall
 *                                    If this parameter is not NULL and hsSameConnectionCall
 *                                    has Q.931 connection, which remote address equal to the
 *                                    hsCall's desireable Q.931 connection remote address then
 *                                    the same Q.931 connection will be used for both calls.
 *
 * Output:  None.
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/

RVAPI
int RVCALLCONV cmCallMultiplex(
            IN      HCALL               hsCall,
            IN      HCALL               hsSameConnectionCall);


/************************************************************************************
 * progressInd_IE enum
 * Progress indication information element inside Q931.
 * The PROGRESS element can use tag value 30 or 31. When creating a progress record,
 * the application can specify which of the tags to use.
 * use30asPI_IE     - Uses tag value 30 for the ProgressIndication
 * use31asPI_IE     - Uses tag value 31 for the ProgressIndication
 * use30_31asPI_IE  - Use both tags. ie - create them both
 ************************************************************************************/
typedef enum
{
    use30asPI_IE=1,
    use31asPI_IE=2,
    use30_31asPI_IE=3
} progressInd_IE;


/************************************************************************************
 *
 * cmCallProgressCreate
 *
 * Purpose: To create PROGRESS message and fill it with the specified parameters
 *
 * Input:   hsCall                  - The stack handle to the call
 *          display                 - The display field
 *          cause                   - The causeValue (-1 if not used)
 *          pi_IE                   - Indicator whether IE to use for progressIndicator IE
 *                                    30 , 31 or both
 *          progressDescription     - The value of the progressIndicator's progressDescription (-1 if not used)
 *          notificationDescription - The value of the notifyIndicator's notifyDescription (-1 if not used)
 *          param                   - The nonStandardParam to be put into the message (NULL if not used)
 *
 * Output:  None.
 *
 * Returned value: The root node ID of the created progress message on success
 *                 This root value should be passed to cmCallProgress() function.
 *                 Negative value on failure
 *
 **************************************************************************************/
RVAPI
int RVCALLCONV cmCallProgressCreate(
        IN HCALL                hsCall,
        IN const RvChar*        display,
        IN int                  cause,
        IN progressInd_IE       pi_IE,
        IN int                  progressDescription,
        IN int                  notificationDescription,
        IN cmNonStandardParam*  param);


/************************************************************************************
 *
 * cmCallProgressGet
 *
 * Purpose: To decode PROGRESS message
 *
 * Input:   hApp                    - The stack instance handle
 *          message                 - Progress message to decode
 *          displaySize             - The display field size in bytes.
 *
 * Output:  display                 - The display field (user allocated memory)
 *          cause                   - The causeValue (-1 if absent)
 *          pi_IE                   - Indicator whether IE to use for progressIndicator IE
 *                                    30 , 31 or both
 *          progressDescription     - The value of the progressIndicator's progressDescription (-1 if absent)
 *          notificationDescription - The value of the notifyIndicator's notifyDescription (-1 if absent)
 *          param                   - The nonStandardParam from the message
 *                                    (param->length==RV_ERROR_UNKNOWN if it wasn't there)
 *          ** Any of the output fields can be passed as NULL if the
 *             application doesn't care about their values
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/
RVAPI
int RVCALLCONV cmCallProgressGet(
        IN  HAPP                hApp,
        IN  int                 message,
        OUT char*               display,
        IN  int                 displaySize,
        OUT int*                cause,
        OUT progressInd_IE*     pi_IE,
        OUT int*                progressDescription,
        OUT int*                notificationDescription,
        OUT cmNonStandardParam* param);


/************************************************************************************
 *
 * cmCallNotifyCreate
 *
 * Purpose: To create NOTIFY message and fill it with the specified parameters
 *
 * Input:   hsCall                  - The stack handle to the call
 *          display                 - The display field
 *          notificationDescription - The value of the notifyIndicator's notifyDescription (-1 if not used)
 *          param                   - The nonStandardParam to be put into the message
 *
 * Output:  None.
 *
 * Returned value: The root node ID of the created notify message on success
 *                 This root value should be passed to cmCallNotify() function.
 *                 Negative value on failure
 *
 **************************************************************************************/
RVAPI
int RVCALLCONV cmCallNotifyCreate(
        IN HCALL                hsCall,
        IN const RvChar*        display,
        IN int                  notificationDescription,
        IN cmNonStandardParam*  param);


/************************************************************************************
 *
 * cmCallNotifyGet
 *
 * Purpose: To decode NOTIFY message
 *
 * Input:   hApp                    - The stack instance handle
 *          message                 - Progress message to decode
 *          displaySize             - The display field size in bytes.
 *
 * Output:  display                 - The display field (user allocated memory)
 *          notificationDescription - The value of the notifyIndicator's notifyDescription (-1 if absent)
 *          param                   - The nonStandardParam from the message
 *                                    (param->length==RV_ERROR_UNKNOWN if it wasn't there)
 *          ** Any of the output fields can be passed as NULL if the
 *             application doesn't care about their values
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/
RVAPI
int RVCALLCONV cmCallNotifyGet(
        IN  HAPP                hApp,
        IN  int                 message,
        OUT char*               display,
        IN  int                 displaySize,
        OUT int*                notificationDescription,
        OUT cmNonStandardParam* param);


/************************************************************************************
 *
 * cmCallProgress
 *
 * Purpose: To send PROGRESS message
 *
 * Input:   hsCall                  - The stack handle to the call
 *          message                 - The pvt nodeId of the PROGRESS message to send
 *
 * Output:  None.
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/
RVAPI
int RVCALLCONV cmCallProgress(
                            IN HCALL        hsCall,
                            IN RvPvtNodeId  message);

/************************************************************************************
 *
 * cmCallNotify
 *
 * Purpose: To send NOTIFY message
 *
 * Input:   hsCall                  - The stack handle to the call
 *          message                 - The pvt nodeId of the NOTIFY message to send
 *
 * Output:  None.
 *
 * Returned value: A nonnegative value on success
 *
 **************************************************************************************/
RVAPI
int RVCALLCONV cmCallNotify(
                            IN HCALL        hsCall,
                            IN RvPvtNodeId  message);


/********************************************************************************************
 * cmCallCreateAnnexLMessage
 * purpose : This function creates an Annex L message. It actually creates a PVT node of
 *           type StimulusControl, and allows setting the values of this type.
 * input   : hApp           - Stack instance handle
 *           isText         - RV_TRUE if it's a text message
 *                            RV_FALSE otherwise
 *                          - This is a field inside the structure of StimulusControl (=Annex L)
 *           h248Message    - The buffer of the Annex L message
 *                            NULL if not needed
 *           h248MessageLength  - Length of the message in bytes
 *           nonStandard    - Non standard data associated with the Annex L message
 *                            NULL if not needed
 * output  : none
 * return  : PVT node of the annex L message on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallCreateAnnexLMessage(
    IN HAPP                 hApp,
    IN RvBool               isText,
    IN const RvChar         *h248Message,
    IN int                  h248MessageLength,
    IN cmNonStandardParam   *nonStandard);


/********************************************************************************************
 * cmCallSendAnnexLMessage
 * purpose : This function sends an Annex L message on the network.
 * input   : hsCall     - Call to send the message in
 *           message    - PVT node of the message to send (of type StimulusControl)
 *           force      - RV_TRUE if the message must be sent immediately on a Facility message
 *                        RV_FALSE if we want to send it on the next Q931 message
 * output  : none
 * return  : Non-negative value on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallSendAnnexLMessage(
    IN HCALL                hsCall,
    IN int                  message,
    IN RvBool               force);


/********************************************************************************************
 * cmCallGetAnnexLMessage
 * purpose : This function checks the values of an Annex L message.
 *           Annex L messages are piggy-backed on top of Q931 messages, inside a field
 *           called StimulusControl.
 *           This function should be used when cmEvCallNewAnnexLMessage is called.
 * input   : hApp           - Stack instance handle
 *           nodeId         - PVT Node ID of the Annex L message. It's of type StimulusControl
 *           h248MessageLength  - Maximum length of the message buffer in bytes
 * output  : isText         - RV_TRUE if it's a text message
 *                            RV_FALSE otherwise
 *                          - This is a field inside the structure of StimulusControl (=Annex L)
 *           h248Message    - The buffer of the Annex L message
 *           h248MessageLength  - Length of the message in bytes
 *           nonStandard    - Non standard data associated with the Annex L message
 * return  : Non-negative value on success
 *           Negative value on failure
 * Note    : Any of the output fields may be set to NULL.
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallGetAnnexLMessage(
    IN    HAPP                hApp,
    IN    int                 nodeId,
    OUT   RvBool*             isText,
    OUT   char*               h248Message,
    INOUT int*                h248MessageLength,
    OUT   cmNonStandardParam* nonStandard);




/********************************************************************************************
 * cmTunnelledProtocolID struct
 * Protocol identifier of a tunneled message. This structure is used for Annex M messages.
 * It comes to simplify the use of the TunnelledProtocol type in the ASN.1 definition.
 ********************************************************************************************/
typedef struct
{
    char tunnelledProtocolObjectID[128]; /* Object identifier of the tunneled protocol.
                                            This is a NULL terminated string of the identifier.
                                            Its length is 0 if the id of the tunneled protocol
                                            uses an alternate ID and not an object ID. */
    char protocolType[64]; /* Type inside the alternate ID of the tunneled protocol.
                              Only used when tunnelledProtocolObjectID is of length 0.
                              This field is not NULL terminated. */
    int  protocolTypeLength; /* Length of protocolType in bytes.
                                Only used when tunnelledProtocolObjectID is of length 0. */
    char protocolVariant[64]; /* Variant inside the alternate ID of the tunneled protocol.
                                 Only used when tunnelledProtocolObjectID is of length 0.
                                 This field is not NULL terminated. */
    int  protocolVariantLength; /* Length of protocolVariant in bytes.
                                   Only used when tunnelledProtocolObjectID is of length 0. */
    char subIdentifier[64]; /* Sub identifier of the tunneled protocol. */
    int  subIdentifierLength; /* Length of subIdentifier in bytes
                                 Doesn't exist in the TunneledProtocol type of set to 0. */
} cmTunnelledProtocolID;



/********************************************************************************************
 * cmOctetString struct
 * This structure is used by some of the API functions of the CM. It allows setting several
 * octet strings at once, by using an array of this struct.
 ********************************************************************************************/
typedef struct
{
    char* message; /* message string buffer. If set to NULL, it indicates the last one in the
                      array of such elements. */
    int   size; /* Size of the message string */
} cmOctetString;


/********************************************************************************************
 * cmCallCreateAnnexMMessage
 * purpose : This function creates an Annex M message. It actually creates a PVT node of
 *           type tunnelledSignallingMessage, and allows setting the values of this type.
 * input   : hApp               - Stack instance handle
 *           tunnellingRequired - RV_TRUE if tunneling of the messages is required
 *                                RV_FALSE otherwise
 *           tunnelledProtocolID- The identifier of the protocol being tunneled
 *                                This is a structure of type cmTunnelledProtocolID.
 *           messageContent     - The message being tunneled. It is an array of strings with
 *                                variable length. Last element in this array must point
 *                                to a NULL string.
 *           nonStandard        - Non standard data associated with the Annex M message
 * output  : none
 * return  : PVT node of the annex M message on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallCreateAnnexMMessage(
    IN HAPP                     hApp,
    IN RvBool                   tunnellingRequired,
    IN cmTunnelledProtocolID*   tunnelledProtocolID,
    IN cmOctetString*           messageContent,
    IN cmNonStandardParam*      nonStandard);


/********************************************************************************************
 * cmCallSendAnnexMMessage
 * purpose : This function sends an Annex M message on the network.
 * input   : hsCall     - Call to send the message in
 *           message    - PVT node of the message to send (of type tunnelledSignallingMessage)
 *           force      - RV_TRUE if the message must be sent immediatly on a Facility message
 *                        RV_FALSE if we want to send it on the next Q931 message
 * output  : none
 * return  : Non-negative value on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallSendAnnexMMessage(
    IN HCALL                hsCall,
    IN int                  message,
    IN RvBool               force);


/********************************************************************************************
 * cmCallGetAnnexMMessage
 * purpose : This function checks the values of an Annex M message.
 *           Annex M messages are piggy-backed on top of Q931 messages, inside a field
 *           called tunnelledSignallingMessage.
 *           This function should be used when cmEvCallNewAnnexMMessage is called.
 *           To get the actual tunneled message, use cmCallGetAnnexMMessageContent
 * input   : hApp           - Stack instance handle
 *           nodeId         - PVT Node ID of the Annex M message.
 * output  : tunnellingRequired - RV_TRUE if tunneling of the messages is required
 *                                RV_FALSE otherwise
 *           tunnelledProtocolID- The identifier of the protocol being tunneled
 *                                This is a structure of type cmTunnelledProtocolID.
 *           nonStandard        - Non standard data associated with the Annex M message
 * return  : Non-negative value on success
 *           Negative value on failure
 * Note    : Any of the output fields may be set to NULL.
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallGetAnnexMMessage(
    IN  HAPP                    hApp,
    IN  int                     nodeId,
    OUT RvBool*                 tunnellingRequired,
    OUT cmTunnelledProtocolID*  tunnelledProtocolID,
    OUT cmNonStandardParam*     nonStandard);


/********************************************************************************************
 * cmCallGetAnnexMMessageContent
 * purpose : This function checks the values of an Annex M message.
 *           Annex M messages are piggy-backed on top of Q931 messages, inside a field
 *           called tunnelledSignallingMessage.
 *           This function should be used when cmEvCallNewAnnexMMessage is called.
 *           To get more information about the specific message being tunneled, use
 *           cmCallGetAnnexMMessage.
 * input   : hApp                   - Stack instance handle
 *           nodeId                 - PVT Node ID of the Annex M message.
 *           index                  - Specific tunneled message to get (1-based)
 *           messageContentLength   - Maximum length of buffer in bytes
 * output  : messageContentLength   - Length of message
 *           messageContent         - The message buffer itself
 * return  : Non-negative value on success
 *           Negative value on failure
 * Note    : Any of the output fields may be set to NULL.
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallGetAnnexMMessageContent(
    IN    HAPP                hApp,
    IN    int                 nodeId,
    IN    int                 index,
    INOUT int*                messageContentLength,
    OUT   char*               messageContent);


/********************************************************************************************
 * cmCallSimulateMessage
 * purpose : "Push" a message into the stack as if it was received from the network.
 * input   : hsCall     - Call the message is received on
 *           message    - PVT node ID of the received message
 *                        This node must be a Q931 or H245 message.
 * output  : none
 * return  : Non-negative value on success
 *           Negative value on failure
 ********************************************************************************************/
RVAPI
int RVCALLCONV cmCallSimulateMessage(
                         IN HCALL               hsCall,
                         IN int                 message);


/************************************************************************
 * cmEmGetCommonSyntax
 * purpose: Returns the syntax of common.asn.
 *          This syntax holds the system's configuration syntax
 *          The syntax is the buffer passed to pstConstruct as syntax.
 * input  : none
 * output : none
 * return : Syntax of common.asn
 ************************************************************************/
RVAPI
RvUint8* RVCALLCONV cmEmGetCommonSyntax(void);


/************************************************************************
 * cmEmGetQ931Syntax
 * purpose: Returns the syntax of Q931/H225 ASN.1
 *          This syntax holds the Q931 and H225 standard's ASN.1 syntax
 *          The syntax is the buffer passed to pstConstruct as syntax.
 * input  : none
 * output : none
 * return : Syntax of Q931/H225 ASN.1
 ************************************************************************/
RVAPI
RvUint8* RVCALLCONV cmEmGetQ931Syntax(void);


/************************************************************************
 * cmMeiEnter
 * purpose: Enters critical section for the specified stack instance.
 * input  : hApp        - Stack's application handle
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmMeiEnter(IN HAPP hApp);


/************************************************************************
 * cmMeiExit
 * purpose: Exits critical section for the specified stack instance.
 * input  : hApp        - Stack's application handle
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmMeiExit(IN HAPP hApp);


/************************************************************************
 * cmThreadAttach
 * purpose: Indicate that the current running thread can be used to catch
 *          events from the network.
 *          Threads that called cmInitialize() don't have to use this
 *          function.
 *          Although this function has hApp as a parameter, the result
 *          will take place for all stack instances
 * input  : hApp        - Stack's application handle
 *          threadId    - Unused
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmThreadAttach(HAPP hApp,RvUint32 threadId);


/************************************************************************
 * cmThreadDetach
 * purpose: Indicate that the current running thread cannot be used to catch
 *          events from the network.
 *          Although this function has hApp as a parameter, the result
 *          will take place for all stack instances
 * input  : hApp        - Stack's application handle
 *          threadId    - Unused
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmThreadDetach(HAPP hApp,RvUint32 threadId);

/************************************************************************
 * cmLogMessage
 * purpose: Writes a user message into the log file under the APPL filter
 * input  : hApp        - Stack's application handle
 *          line        - The message to be printed (must be null-terminated)
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmLogMessage(IN HAPP hApp, IN const char *line);


/************************************************************************
 * cmString2TransportAddress
 * purpose: Converts a regular string to a cmTransportAddress struct
 * input  : string   - Regular string, in one of two formas:
 *                     IPv4: xxx.xxx.xxx.xxx:xxxx
 *                     IPv6: [HHHH:HHHH:HHHH::HHHH:HHHH]:xxxx
 * output : addr     - address matching (parsing) the given string
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV cmString2TransportAddress(
    IN  const RvChar*       string,
    OUT cmTransportAddress* addr);


/************************************************************************
 * cmTransportAddress2String
 * purpose: Converts a cmTransportAddress struct to a regular string
 * input  : addr     - address to be turnes into a string
 *          len      - the length of buffer "string" in bytes
 * output : string   - string representation, in one of two formas:
 *                     IPv4: xxx.xxx.xxx.xxx:xxxx
 *                     IPv6: [HHHH:HHHH:...:HHHH:HHHH]:xxxx
 *                     string is expected to be of enough size to hold
 *                     a full IPv6 address (55 bytes should do it)
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV cmTransportAddress2String(
    IN  cmTransportAddress* addr,
    IN  RvSize_t            len,
    OUT RvChar*             string);


/************************************************************************************
 * cmCompareTransportAddresses
 *
 * Purpose: Compare two addresses according to their type.
 *
 * Input: addr1, addr2 - the cmTransportAddress to compare.
 *
 * Output: None.
 *
 * Returned Value: RV_TRUE - the addresses are identical; RV_FALSE - Otherwise
 **************************************************************************************/
RVAPI RvBool RVCALLCONV
cmCompareTransportAddresses(
    IN cmTransportAddress *addr1, 
    IN cmTransportAddress *addr2);


/******************************************************************************
 * cmTransportAddress2Vt
 * ----------------------------------------------------------------------------
 * General: Converts a cmTransportAddress struct to a PVT node Id.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal     - The PVT handle.
 *         address  - Transport address to convert.
 *         nodeId   - The node Id to build the converted transport address in.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmTransportAddress2Vt(
    IN  HPVT                hVal,
    IN  cmTransportAddress* address,
    IN  RvPvtNodeId         nodeId);


/******************************************************************************
 * cmVt2TransportAddress
 * ----------------------------------------------------------------------------
 * General: Converts a PVT node Id of type TransportAddress to a
 *          cmTransportAddress struct.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hVal     - The PVT handle.
 *         nodeId   - The node Id of the transport address to convert (of type
 *                    TransportAddress).
 * Output: address  - Resulting transport address struct.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV cmVt2TransportAddress(
    IN  HPVT                hVal,
    IN  RvPvtNodeId         nodeId,
    OUT cmTransportAddress* address);



//add by yj
RVAPI
int RVCALLCONV cmGetLCNFromHchan(HCHAN hsChan);

#define cmRadDebugCfgIgnoreDRQ  1
RVAPI
void RVCALLCONV cmInitDebugCfg(IN int type, IN void* data, IN int len);

RVAPI void RVCALLCONV radhelp();
RVAPI void RVCALLCONV radver();

RVAPI int RVCALLCONV radPrintRootNodes( IN HAPP hApp, IN RvBool bAllNode );//add by yj [20120918]

RVAPI
int RVCALLCONV cmLightRegister( 
		IN  HAPP			hApp);

//begin with added by wangxiaoyi on 20 September 2010
typedef struct 
{
	RvBool    bMediaTraversalServer;
	RvBool    bSupportTransmitMultiplexedMedia;  
}cmH460Config;
//end with added by wangxiaoyi on 20 September 2010

//add by daiqing 20100720 for 460
typedef struct __cmStackConfig
{
	/*UINT32*/RvUint32      NATAddress;
	/*BOOL*/  RvBool       Support460;
	cmH460Config th460Config;
}cmStackConfig;

extern cmStackConfig g_tNATAddress;

//add by daiqing 20100720 for 460
/************************************************************************
* cmSetStackConfig
* purpose: set stack config. It is global.
* input  : ptStackConfig  - stack config
* output : none
* return : none
************************************************************************/
RVAPI
void RVCALLCONV cmSetStackConfig(IN cmStackConfig *ptStackConfig);
/*end*/
RVAPI
int RVCALLCONV cmCallQ931Connect(IN HCALL hsCall, IN cmTransportAddress* remote);
//end

//add by yj for 460server
RVAPI
int RVCALLCONV cmGetMessageNodeByHandle( IN HCALL hsCall, IN int messagetype);
RVAPI
int RVCALLCONV cm460sendH245Message( IN HCALL hsCall, IN int vNodeId );
//end

/******************************************************************************
 * cmGetHchanFromLCN
 * ----------------------------------------------------------------------------
 * General: Get hChan from Logical Channel Num. Add by Jacky Wei
 * Return Value: HCHAN handle  - if successful.
 *               NULL on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  	hsCall     	- The HCALL handle.
 *         	LogicalNum 	- The Logical Channel Number of the CALL
 *			bIn			- call in or out
 * Output: HCHAN  		- Resulting HCHAN handle.
 *****************************************************************************/
RVAPI 
HCHAN RVCALLCONV cmGetHchanFromLCN(HCALL hsCall, int LogicalNum, int bIn);


#ifdef __cplusplus
}
#endif


#endif  /* CM_H */
