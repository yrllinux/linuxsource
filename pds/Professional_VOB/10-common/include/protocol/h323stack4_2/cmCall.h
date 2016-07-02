/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/


#ifndef _CMCALL_
#define _CMCALL_

#include "transport.h"
#include "rvh323timer.h"
#include "cat.h"
#include "cmControl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of states in the call's state queue.
 * We do our best not to have a state-change of a call from within another state-change
 * callback to give the best backward-compatibility we can and to reduce call-stack nesting
 */
#define RV_H323CALL_STATE_MAX (5)

/* Dummy state definition, as held inside the queue */
#define RV_H323CALL_STATE_DUMMY (0xff)

/* The following definition allows debugging the stack while the flags are used
   as fields inside callElem instead of a bitfield. It should be commented out even
   in regular debug mode. */
/* #define RV_H323CALL_DEBUG_FLAGS */


/************************************************************************
 * cmCallParamEnum
 * Enumeration value of parameters as properties stored inside a call when
 * the property DB is not used as a single root with several messages
 * beneith it.
 * Each group of parameters that belong to the same type of message is
 * written one after the other, so we know when we're done with the
 * parameters of a specific message type.
 ************************************************************************/
typedef enum
{
    /* SETUP */
        cmEnumSetupSendingComplete   ,
        cmEnumInformationTransferCapability ,
        cmEnumDisplay                ,
        cmEnumCalledPartyNumber      ,
        cmEnumCalledPartySubAddress  ,
        cmEnumCallingPartyNumber     ,
        cmEnumCallingPartySubAddress ,
        cmEnumFullSourceAddress      ,
        cmEnumFullDestinationAddress ,
        cmEnumDestExtraCallInfo      ,
        cmEnumExtension              ,

        cmEnumFullSourceInfo         ,

        cmEnumDestCallSignalAddress  ,
        cmEnumSrcCallSignalAddress   ,
        cmEnumSetupH245Address       ,

        cmEnumActiveMc               ,
        cmEnumConferenceGoal         ,
        cmEnumCallType               ,

        cmEnumUserUser               ,

        cmEnumSetupNonStandardData   ,
        cmEnumSetupNonStandard       ,

        cmEnumSetupFastStart         ,
        cmEnumSetupCanOverlapSending ,

    /* CALL PROCEEDING */
        cmEnumCallProcH245Address    ,
        cmEnumCallProceedingNonStandardData,
        cmEnumCallProceedingNonStandard,

        cmEnumCallProcFastStart      ,

    /* ALERTING */
        cmEnumAlertingH245Address    ,
        cmEnumAlertingNonStandardData,
        cmEnumAlertingNonStandard    ,
        cmEnumAlertingFastStart      ,

    /* CONNECT */
        cmEnumH245Address            ,
        cmEnumConnectDisplay         ,
        cmEnumConnectUserUser        ,
        cmEnumConnectNonStandardData ,
        cmEnumConnectNonStandard     ,
        cmEnumConnectFastStart       ,
        cmEnumConnectConnectedAddress,

    /* RELEASE COMPLETE */
        cmEnumReleaseCompleteReason  ,
        cmEnumReleaseCompleteCause   ,
        cmEnumReleaseCompleteNonStandardData,
        cmEnumReleaseCompleteNonStandard,

    /* FACILITY */
        cmEnumAlternativeAddress     ,
        cmEnumAlternativeAliasAddress,
        cmEnumAlternativeDestExtraCallInfo
                                           ,
        cmEnumAlternativeExtension   ,
        cmEnumFacilityCID            ,
        cmEnumFacilityReason         ,

    /* LAST ITEM - used to calculate size on compilation */
        cmEnumLast
} cmCallParamEnum;



/* This list holds the positions of all the fields inside the flags field of a callElem. */
typedef enum
{
    e_remoteVersion = 0,
    e_newCIDRequired = e_remoteVersion+5,

    e_callInitiator,
    e_dummyRAS,
    e_automaticRAS,
    e_enableOverlapSending,
    e_remoteCanOverlapSend,
    e_multiRate,
    e_overrideCID,
    e_gatekeeperRouted,

    e_notified,
    e_callWithoutQ931,
    e_h245Tunneling,
    e_notEstablishControl,
    e_isParallelTunneling,
    e_h245Stage,
    e_isMultiplexed = e_h245Stage+5,
    e_shutdownEmptyConnection,
    e_controlDisconnected,
    e_preservedCall,
    e_callDialInvoked,
    e_sendCallProceeding,
    e_sendAlerting,
    e_setTerminalType,

    /* Last enumeration value */
    e_lastCallFlag /* This should always be the last flag - it is used for validity checks */
} callElemEnumPosition;



/* This list holds the mask to use for each of the flags. There are fields that are not
   boolean, whose flags have a bigger mask than those of the boolean fields. */
typedef enum
{
    s_remoteVersion             = 0x1f,
    s_newCIDRequired            = 1,

    s_callInitiator             = 1,
    s_dummyRAS                  = 1,
    s_automaticRAS              = 1,
    s_enableOverlapSending      = 1,
    s_remoteCanOverlapSend      = 1,
    s_multiRate                 = 1,
    s_overrideCID               = 1,
    s_gatekeeperRouted          = 1,

    s_notified                  = 1,
    s_callWithoutQ931           = 1,
    s_h245Tunneling             = 1,
    s_notEstablishControl       = 1,
    s_isParallelTunneling       = 1,
    s_h245Stage                 = 0x1f,
    s_isMultiplexed             = 1,
    s_shutdownEmptyConnection   = 1,
    s_controlDisconnected       = 1,
    s_preservedCall             = 1,
    s_callDialInvoked           = 1,
    s_sendCallProceeding        = 1,
    s_sendAlerting              = 1,
    s_setTerminalType           = 1
} callElemEnumSize;


#ifdef RV_H323CALL_DEBUG_FLAGS

#define m_callset(_call,_field,_value) (_call)->flags.##_field = _value
#define m_callget(_call,_field) (_call)->flags.##_field

#else

#define RV_H323CALL_FLAG_TYPE RvUint32

/* field_mask(field) defines the mask to use on the flags to handle the given field
 * example:
 *  field_mask(h245Stage);
 * The above returns the mask for the h245Stage boolean field inside callInfo.flags
 */
#define field_mask(_field) (RV_H323CALL_FLAG_TYPE)(s_##_field<<((RvUint32)e_##_field))

/* Set and get macros for fields inside callInfo.flags */
#define m_callset(_call,_field,_value) \
    ((_call)->flags = ( ((_call)->flags  & ~(field_mask(_field))) | (field_mask(_field) & ((RV_H323CALL_FLAG_TYPE)(_value) << (e_##_field))) ))
#define m_callget(_call,_field) \
    (((_call)->flags  & (field_mask(_field))) >> (e_##_field))

#endif  /* RV_H323CALL_DEBUG_FLAGS */



/************************************************************************
 * callElem
 * Call element. Holds the information about the call.
 * Check cmCrossReference.c for details about the calls database.
 ************************************************************************/
typedef struct __callElem {
#ifdef RV_H323CALL_DEBUG_FLAGS
    struct
    {
        int                     remoteVersion;
        RvBool                  newCIDRequired;         /* RV_TRUE if we need to create a new CID.
                                                           This happens when we're forwarding the call */
        RvBool                  callInitiator;          /* RV_TRUE if this is the initiator of the call, RV_FALSE otherwise */
        RvBool                  dummyRAS;               /* There is no RAS in this call at the moment */
        RvBool                  automaticRAS;           /* RV_TRUE if this call supports automatic RAS */
        RvBool                  enableOverlapSending;
        RvBool                  remoteCanOverlapSend;
        RvBool                  multiRate;
        RvBool                  overrideCID;            /* Indication that the CID parameter should be overriden in outgoing messages
                                                           when sendCallMessage() is called. This parameter is false only when
                                                           cmCallJoin() is called. */
        RvBool                  gatekeeperRouted;       /* RV_TRUE if callModel is gatekeeperRouted */

        RvBool                  notified;               /* RV_TRUE if application was already notified about this new incoming call */
        RvBool                  callWithoutQ931;        /* RV_TRUE if call started with incoming ARQ and no SETUP was received yet */
        RvBool                  control;                /* RV_TRUE if there is an H245 TCP connection for this call */
        RvBool                  h245Tunneling;
        RvBool                  notEstablishControl;
        RvBool                  isParallelTunneling;
        cmH245Stage             h245Stage;
        RvBool                  isMultiplexed;
        RvBool                  shutdownEmptyConnection;
        RvBool                  fastStartFinished;
        RvBool                  controlDisconnected;
        RvBool                  preservedCall;
        RvBool                  callDialInvoked;        /* RV_TRUE if cmCallDial() was already called for outgoing calls */
        RvBool                  sendCallProceeding;
        RvBool                  sendAlerting;
        RvBool                  setTerminalType;        /* Are we setting the terminal type manually? */
    } flags;
#else
    RV_H323CALL_FLAG_TYPE   flags;
#endif  /* RV_H323CALL_DEBUG_FLAGS */

    HSTRANSSESSION          hsTransSession;
    cmConferenceGoalType    conferenceGoal;
    cmCallState_e           state;
    cmCallStateMode_e       stateMode;


    RvUint8                 callId[16];             /* CallIdentifier of the call */
    RvUint8                 cId[16];                /* ConferenceIdentifier of the call */

    void*                   hMsgContext;            /* Message context for last message received on call. Used for security checks */
    int                     crv;                    /* Q931 CRV of the call */
    int                     rascrv;                 /* RAS CRV of the call */

    int                     rate;
    int                     newRate;

    RvTimer*                timer;
    cmPreGrantedArqUse      preGrantedArqUse;
    cmAnnexEUsageMode       annexE;
    RVHCATCALL              hCatCall;               /* CallAssociationTable handle for this call */
    RvPvtNodeId             routeCallSignalAddress;
    RvPvtNodeId             callSignalAddress;      /* The address of destination of the callSignaling connection */
                                                    /* The remote for outgoing call */
                                                    /* The local for incoming call */
    RvPvtNodeId             remoteCallSignalAddress;/* The remote callSignaling address for the incoming call */
    RvPvtNodeId             annexECallSignalAddress;
    RvPvtNodeId             destinationInfo;

    HCHAN                   firstChan; /* First channel for this call. Others are linked to it */

    /* parameters */
    RvPvtNodeId             property;               /* PVT root of the property database for the call */
    RvPvtNodeId             paramNodeId[cmEnumLast];/* Parameters that are used if we're running without a property DB */
    RvPvtNodeId             lastSentMessageNodeId;  /* NodeID of the last send message. This parameter is used when we're
                                                       running without a property DB */

    RvUint8                 msdTerminalType;           /* Terminal type to use for MSD procedures */

    /* state queue - this is a cyclic queue */
    RvUint8                 q_nextState; /* Index of the next state we should handle */
    RvUint8                 q_numStates; /* Number of state currently in the queue */
    RvUint8                 q_states[RV_H323CALL_STATE_MAX];
    RvUint8                 q_stateModes[RV_H323CALL_STATE_MAX];

	//add by daiqing 20100722 for 460 kdv
	int						if460Internal;
	int                     ifSendFacility;
	//end

} callElem;




/************************************************************************
 * notifyControlState
 * purpose: Notify the application about the state of the control
 *          This function checks the control state and uses the call state
 *          notification function.
 * input  : hsCall         - Stack handle for the call
 *          state          - The control state
 *          bIsMesFinished - Unused
 * output : none
 * return : none
 ************************************************************************/
void notifyControlState(IN HCALL             hsCall,
                        IN H245ControlState  state,
                        IN RvBool            bIsMesFinished);

/************************************************************************
 * cmCallInAnsweringState
 * purpose: Check to see if we could be in the middle of answering a
 *          FS proposal
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : RV_TRUE - call state right, RV_FALSE - wrong call state
 ************************************************************************/
RvBool RVCALLCONV cmCallInAnsweringState(IN HCALL hsCall);

/************************************************************************
 * callStartOK
 * purpose: Deal with an outgoing or incoming call in the Q931 side, after
 *          RAS was done (or skiped)
 * input  : call    - Stack handle for the call to dial
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callStartOK(IN callElem* call);


int callStartRoute(callElem*call);
int callStartError(callElem*call);
int callIncompleteAddress(callElem*call);

/************************************************************************
 * callDial
 * purpose: Dials a call or answers incoming Setup requests.
 *          This function checks if we have to ARQ the GK on this call or
 *          not and continues the call setup process.
 * input  : call    - Stack handle for the call to dial
 * output : none
 * return : negative on error
 ************************************************************************/
int callDial(IN callElem* call);

/************************************************************************
 * rasCallDrop
 * purpose: Call drop was initiated by an incoming DRQ message from the
 *          gatekeeper
 * input  : call      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasCallDrop(IN callElem* call);

/************************************************************************
 * callStopOK
 * purpose: We've finished dropping the call.
 *          Notify the application we've reached the Idle state on this call
 * input  : hsCall      - Stack handle for the call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int callStopOK(IN callElem* call);

int callStopError(callElem*call);

/************************************************************************
 * callNewRate
 * purpose: Notify the application about the rate of the call
 * input  : call      - Stack handle for the call
 * output : none
 * return : none
 ************************************************************************/
void callNewRate(IN callElem* call);

/************************************************************************
 * sendCallMessage
 * purpose: Send Q931 message for a given call.
 *          This function also adds specific call-related information
 * input  : hsCall      - Stack handle for the Q931 call.
 *          message     - Root node ID of the message to send.
 *          q931type    - Message type we're dealing with.
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus sendCallMessage(
    IN HCALL                hsCall,
    IN RvPvtNodeId          message,
    IN cmCallQ931MsgType    q931type);

/************************************************************************
 * sendMessageForH245
 * purpose: Send H245 message for a given call.
 * input  : hsCall  - Stack handle for the call
 *          message - root node ID of the message to send
 * output : sentOnDummy - true iff the message was "sent" on a dummy host.
 * return : Non-negative value on success
 *          Negative value on failure
************************************************************************/
int sendMessageForH245(IN HCALL hsCall, IN int message, OUT RvBool * sentOnDummy);

/************************************************************************
 * sendMessageForH245Chan
 * purpose: Send H245 message for a given channel.
 * input  : hsCall  - Stack handle for the call
 *          hsChan  - Channel handle for the H245 channel
 *          message - root node ID of the message to send
 * output : sentOnDummy - true iff the message was "sent" on a dummy host.
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int sendMessageForH245Chan(
    IN  HCALL    hsCall,
    IN  HCHAN    hsChan,
    IN  int      message,
    OUT RvBool*  sentOnDummy);

/************************************************************************
 * sendMessage
 * purpose: Send TCP (Q931/H245) message for a given call.
 *          This function also adds specific call-related information
 * input  : hsCall  - Stack handle for the call
 *          vNodeId - root node ID of the message to send
 *          type    - Type of channel to send on
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus sendMessage(IN HCALL hsCall, IN RvPvtNodeId vNodeId, IN cmTpktChanHandleType type);

int cmEvRASNewCall(HAPP hApp, RVHCATCALL* lphCatCall, catStruct* callObj);

/************************************************************************
 * cmIndicate
 * purpose: Indicate and process an incoming Q931 message
 * input  : call    - Stack handle for the call
 *          message - Message root node
 *          msgType - Type of the message
 * output : none
 * return : Non-negative value on success
 *          negative value on failure
 ************************************************************************/
int cmIndicate(IN HCALL call, IN RvPvtNodeId message, IN cmCallQ931MsgType msgType);


/************************************************************************
 * callMibGetSession
 * purpose: Get the session of a handle for the MIB.
 *          This function is used to access the transport layer for MIB
 *          specific information
 * input  : call    - Stack handle for the call
 * output : none
 * return : Session of the call on success
 *          NULL on failure
 ************************************************************************/
HSTRANSSESSION callMibGetSession(IN HCALL call);

/**********************************************************************
* cmSetupEnd
* This function is responsible for the end of the scenario of receiving
* a setup message. It was taken out of the cmSetup in order to give the
* endpoint time to receive ACF before sending AutoConnect (when autoRas
* and autoConnect are on).
************************************************************************/
int cmSetupEnd(IN callElem* call);



/* Callback functions for transport. Set in cmGeneral.c */
TRANSERR cmEvTransNewSession(IN  HAPPTRANS        hsTrans,
                             IN  HAPPATRANS       haTrans,
                             IN  HSTRANSSESSION   hsTransSession,
                             OUT HATRANSSESSION   *haTransSession,
                             IN  int              message,
                             OUT int              *cause,
                             OUT RvPstFieldId     *reasonNameId);
TRANSERR cmEvTransConnectionOnSessionClosed(IN HSTRANSSESSION hsTransSession,
                                            IN HATRANSSESSION haTransSession,
                                            IN TRANSCONNTYPE  type);
TRANSERR cmEvTransSessionNewConnection( IN HSTRANSSESSION   hsTransSession,
                                        IN HATRANSSESSION   haTransSession,
                                        IN TRANSCONNTYPE    type);
TRANSERR cmEvTransNewMessage(IN HSTRANSSESSION          hsTransSession,
                             IN HATRANSSESSION          haTransSession,
                             IN RvH323ConnectionType    type,
                             IN int                     message,
                             IN void                    *hMsgContext,
                             IN RvBool                  isTunnelled,
                             IN int                     index);
TRANSERR cmEvTransBadMessage(IN HSTRANSSESSION          hsTransSession,
                             IN HATRANSSESSION          haTransSession,
                             IN RvH323ConnectionType    type,
                             RvUint8                    *msg,
                             int                        msgSize,
                             RvBool                     outgoing);
TRANSERR cmEvTransNewH450Message(IN HSTRANSSESSION  hsTransSession,
                                 IN HATRANSSESSION  haTransSession,
                                 IN int             msg,
                                 IN int             msgSize,
                                 IN int             msgType);
TRANSERR cmEvTransNewAnnexMMessage(IN HSTRANSSESSION    hsTransSession,
                                   IN HATRANSSESSION    haTransSession,
                                   IN int               annexMElement,
                                   IN int               msgType);
TRANSERR cmEvTransNewAnnexLMessage(IN HSTRANSSESSION    hsTransSession,
                                   IN HATRANSSESSION    haTransSession,
                                   IN int               annexLElement,
                                   IN int               msgType);
RvBool cmEvTransWantsToStartControl(IN HSTRANSSESSION    hsTransSession,
                                    IN HATRANSSESSION    haTransSession);
TRANSERR cmEvTransTunnelingWasRejected(IN HSTRANSSESSION    hsTransSession,
                                       IN HATRANSSESSION    haTransSession,
                                       IN RvBool            restartTimers);

TRANSERR cmEvTransHostConnected(IN HSTRANSHOST   hsTransHost,
                                IN HATRANSHOST   haTransHost,
                                IN TRANSCONNTYPE type,
                                IN RvBool        isOutgoing);
TRANSERR cmEvTransHostClosed(   IN HSTRANSHOST hsTransHost,
                                IN HATRANSHOST haTransHost,
                                IN RvBool      wasConnected);
TRANSERR cmEvTransNewRawMessage(IN  HSTRANSHOST             hsTransHost,
                                IN  HATRANSHOST             haTransHost,
                                IN  RvH323ConnectionType    type,
                                INOUT int                   pvtNode,
                                IN  RvUint8                 *msg,
                                IN  int                     msgSize,
                                OUT int                     *decoded,
                                OUT void                    **hMsgContext);
TRANSERR cmEvTransSendRawMessage(   IN  HSTRANSHOST     hsTransHost,
                                    IN  HATRANSHOST     haTransHost,
                                    IN  HSTRANSSESSION  hsTransSession,
                                    IN  HATRANSSESSION  haTransSession,
                                    IN  int             pvtNode,
                                    IN  int             size,
                                    OUT RvUint8         *msg,
                                    OUT int             *msgSize);
TRANSERR cmEvTransHostListen(
            IN HSTRANSHOST          hsTransHost,
            IN HATRANSHOST          haTransHost,
            IN TRANSCONNTYPE        type,
            IN cmTransportAddress   *address);
TRANSERR cmEvTransHostListening(
            IN HSTRANSHOST          hsTransHost,
            IN HATRANSHOST          haTransHost,
            IN TRANSCONNTYPE        type,
            IN cmTransportAddress   *address,
            IN RvBool               error);
TRANSERR cmEvTransHostConnecting(
            IN HSTRANSHOST          hsTransHost,
            IN HATRANSHOST          haTransHost,
            IN TRANSCONNTYPE        type,
            IN cmTransportAddress   *address);

//add by daiqing 20100720 for 460
int cmCallSendGenericIndication(IN HCALL hsCall);
#ifdef __cplusplus
}
#endif

#endif  /* _CMCALL_ */

