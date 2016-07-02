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


#ifndef _CMINTERNAL_H
#define _CMINTERNAL_H

#include "rvinternal.h"
#include "rvlock.h"
#include "rvmutex.h"
#include "rvthread.h"
#include "rvrandomgenerator.h"
#include "psyntree.h"
#include "pvaltree.h"
#include "cm.h"
#include "cmcallcid.h"
#include "rvh323timer.h"
#include "rvh323connection.h"
#include "cat.h"
#include "ra.h"
#include "ema.h"
#include "tpkt.h"
#include "transport.h"
#include "cmiras.h"
#include "cmiAddOns.h"
#include "cmCrossReference.h"
#include "cmmib.h"
#include "rvwatchdog.h"
#include "cmH245.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ifE(a) if(a)(a)


typedef struct
{
    RvInt32 nameId;
} fieldNames;


typedef enum
{
    makeCall                        =1,
    useGKCallSignalAddressToMakeCall=2,
    answerCall                      =4,
    useGKCallSignalAddressToAnswer  =8
}preGrantedArqFlags;

typedef enum
{
    ctrlReqNo,
    ctrlReqConnect,
    ctrlReqSeparate
}controlFlags;

typedef enum
{
    pmodeFullProperty,
    pmodeCopySingleMessages,
    pmodeDeleteSingleMessages,
    pmodeDoNotUseProperty
} propertyMode;


/* Enumeration of syntax trees needed to hold information for activating cmParam
   functions without the use of a properties database */
typedef enum
{
    cmParamSynSOAliasAddress,
    cmParamSynAliasAddress,
    cmParamSynCalledPartyNumber,
    cmParamSynCalledSubaddress,
    cmParamSynCallingPartyNumber,
    cmParamSynCallingSubaddress,
    cmParamSynEndpointType,
    cmParamSynConferenceGoal,
    cmParamSynCallType,
    cmParamSynUserData,
    cmParamSynNonStandardParameter,
    cmParamSynReleaseCompleteReason,
    cmParamSynFacilityReason,
    cmParamSynLast
} cmParamSyn;



typedef struct __cmElem
{
    HH245                       hh245; /* handle to the H.245 element, must be the first parameter, as 
                                        the H.245 treats the callElement handle as a pointer to a pointer
                                        to the H.245 element. */
    /* Global callbacks of H.323 */
    SCMEVENT                    cmMyEvent;
    SCMCALLEVENT                cmMyCallEvent;

	SCMMSGEVENT					cmMyMessageEvent;//add by yj for 460server

    /* control */
    SCMSESSIONEVENT             cmMySessionEvent;
    SCMCHANEVENT                cmMyChannelEvent;
    SCMPROTOCOLEVENT            cmMyProtocolEvent;
    CMPROTOCOLEVENT             cmMyProtocolEventReplacement;
    int                         cmMyProtocolEventReplacementSize;

    /* H.235 callbacks */
    cmEvCallNewRawMessageT          cmEvCallNewRawMessage;
    cmEvCallRecvRawMessageT         cmEvCallRecvRawMessage;
    cmEvCallSendRawMessageT         cmEvCallSendRawMessage;
    cmEvCallReleaseMessageContextT  cmEvCallReleaseMessageContext;

    cmNewCallbackEH             newCallback;

    RvThread                    threadInformation; /* Memory space for the current thread information.
                                                      This will only be used of the current thread
                                                      wan't constructed before cmInitialize() was called. */
    RvThread*                   pThreadInfo; /* Current thread information */

    RvSelectEngine*             selectEngine; /* FD select engine */
    RvH323TimerPoolHandle       hTimers; /* Timers pool */
    HAPPPARTNER                 hAppPartner;
    propertyMode                callPropertyMode;   /* Mode used for the property database of
                                                       the stack */

    RvPvtNodeId h245Conf; /* H.245 configuration node id */
    RvPvtNodeId q931Conf; /* Q.931 configuration node id */
    RvPvtNodeId rasConf;  /* RAS configuration node id */

    HPST        synProtH245; /* Syntax of H.245 messages (MultimediaSystemControlMessage) */
    HPST        synProtQ931; /* Syntax of Q.931 messages (Q931Message) */
    HPST        synProtRAS; /* Syntax of RAS messages (RASMessage) */

    HPST        synAnnexL;
    HPST        synAnnexM;

    /* Log sources used by the CM and the log manager */
    RvLogMgr    *logMgr;
    RvLogSource log;
    RvLogSource logAPI;
    RvLogSource logCB;
    RvLogSource logERR;
    RvLogSource logTPKT;
    RvLogSource logTPKTWIRE;
    RvLogSource logConfig;
    RvLogSource logAppl;
    RvLogSource logWatchdog;

    int         level; /* Level of calls for CMAPI and CMAPICB functions */

    RvBool      gatekeeper; /* RV_TRUE if we're running as a gatekeeper,
                               RV_FALSE if we're running as an endpoint */

    HPST        hSyn;
    HPST        hAddrSyn;
    HPST        hAddrSynH245;
    HPST        synOLC;
    HPVT        hVal;               /* PVT used by the stack */
    HCFG        hCfg;               /* Configuration of the stack */
    RvBool      start;              /* RV_TRUE if stack is running, RV_FALSE if only initialized */
    RvBool      cidAssociate;
    RvBool      bCheckParamLength;  /* RV_TRUE if we check for length inside struct parameters on get() API functions */
    RvChar      delimiter;
    RvBool      diffSrcAddressInSetupAndARQ;
    HAPPAPP     haApp;              /* Application's handle for the stack's instance */
    RvLock      appLock; /* Lock used to protect parts of this struct */
    RVHCAT      hCat;

    RvRandomGenerator               randomGenerator; /* Random numbers generator */
    RvH323UniqueIdGeneratorHandle   guidGenerator; /* Globally Unique ID generator to use */

    cmTransportAddress  localAddress; /* The address used by the stack for outgoing calls and for listening sockets */
    HPST            synConfQ931;
    HPST            synConfH245;
    HPST            h245TransCap;
    HPST            h245RedEnc;
    HPST            h245DataType;
    HPST            synKeySync; /* KeySyncMaterial syntax. Used for H.235 secured channels */

    HTPKTCTRL       hTpktCtrl;
    HAPPTRANS       hTransport;

    /*Calls*/
    HEMA        hCalls;
    int         crv;
    int         appDB;      /* PVT root node if application's default messages */
    int         busy;       /* Number of calls currently connected or being connected */
    int         t302;
    int         t304;
    int         postControlDisconnectionDelay;
    int         numberOfConnectingCalls; /* Number of calls that are currently before the
                                            cmCallConnected state. It is used to check if
                                            any new calls can be serviced. */

    /*Endpoint*/
    cmTransportAddress  gkAnnexEAddress; /* Gatekeeper's Annex E address */
    int                 rasGranted;
    int                 rasGrantedProtocol;
    int                 irrFrequencyInCall;
    RvPortRange*        portRange; /* Port range used by the stack */
    
    /*RAS*/
    HRASMGR             rasManager;
    HAUTORASMGR         hAutoRas;
    RvBool              manualRAS; /* RV_TRUE if we're working with manual RAS,
                                      RV_FALSE if we're working with automatic RAS */
    HPST                synConfRAS;
    RvInt               maxListeningAddresses; /* Maximum number of available listening addresses of each type */
    RvInt               numRasListeningSockets; /* Number of sockets used for RAS */
    HEMA                rasListeningSockets; /* Sockets used for RAS network messages */

    HPST        hRASSyn;

    RvPvtNodeId rasAddresses; /* SEQUENCE OF TransportAddress holding all listening RAS addresses */
    RvPvtNodeId q931TpktAddresses; /* SEQUENCE OF TransportAddress holding all listening TPKT addresses */
    RvPvtNodeId q931AnnexEAddresses; /* SEQUENCE OF TransportAddress holding all listening Annex E addresses */

    /* MIB - H341 */
    HSTATISTIC  hStatistic;     /* Current stack's statistics */
    MibEventT   mibEvent;       /* MIB Callbacks to call */
    HMibHandleT mibHandle;      /* Handle of MIB to use */

    /* Other */
    int         encodeBufferSize;
    RvMutex     applicationLock;    /* Application specific lock. Used for cmMeiEnter/Exit */

    /* Watchdog */
    RvWatchdog  watchdog; /* Watchdog module we're using */
    RvTimer     watchdogTimer; /* Timer activating the watchdog at a set interval */
    RvBool      watchdogTimerUsed; /* Do we have a running watchdog timer? */

    /* Syntax types used by cmGetSynTreeByRootName for backward compatability */
    HPST        synQ931UU;
    HPST        synTerminalLabel;
    HPST        synGkCrypto;
    HPST        synGkPwdCert;

    HPST        synParams[cmParamSynLast];

    /* Resource enumerations allocated by watchdog */
    RvUint32    cmTimersResourceVal;
    RvUint32    cmValTreeResourceVal;
} cmElem;




typedef enum
{
    cmRasStateInit,
    cmRasStateAwaiting,
    cmRasStateIdle
}rasStateFlags;


fieldNames* getParamFieldName(
           IN  cmCallParam       param);

int cmFastStartChannelsConnect( HCALL hsCall);
int cmCallPreCallBack(HAPP hApp);


/************************************************************************
 * cmIsRoutedCall
 * purpose: Check if the call is routed or direct. Used to create perCallInfo
 *          inside IRR messages by the ras module.
 * input  : hsCall  - Call handle to check
 * output : none
 * return : RV_TRUE if call is routed
 ************************************************************************/
RvBool cmIsRoutedCall(IN HCALL hsCall);


void cmLock(IN HAPP hApp);
void cmUnlock(IN HAPP hApp);

int cmCallTryControlAfterACF(
    IN  HCALL hsCall
    );
                             

#ifdef __cplusplus
}
#endif

#endif  /* _CMINTERNAL_H */

