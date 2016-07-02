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

#ifndef CMCTRL_H
#define CMCTRL_H


#include "transpcap.h"
#include "redencod.h"
#include "userinput.h"


#ifdef __cplusplus
extern "C" {
#endif


RV_DECLARE_HANDLE(HAPPCHAN);
RV_DECLARE_HANDLE(HCHAN);




/* This type is here only for backward compatibility */
typedef
int (*cmNewProtocolEH)(void);



typedef enum
{
    cmChannelStateDialtone,
    cmChannelStateRingBack,
    cmChannelStateConnected,
    cmChannelStateDisconnected,
    cmChannelStateIdle,
    cmChannelStateOffering
} cmChannelState_e;

typedef enum
{
    cmChannelStateModeOn,
    cmChannelStateModeOff,

    cmChannelStateModeDisconnectedLocal,
    cmChannelStateModeDisconnectedRemote,
    cmChannelStateModeDisconnectedMasterSlaveConflict,
    cmChannelStateModeDuplex,

    cmChannelStateModeDisconnectedReasonUnknown,
    cmChannelStateModeDisconnectedReasonReopen,
    cmChannelStateModeDisconnectedReasonReservationFailure,
	//add by daiqing for cb logicalchannelactive
	cmChannelStateModeActive,
	cmChannelStateModeInactive,

} cmChannelStateMode_e;



typedef enum
{
  cmCapReceive=1,
  cmCapTransmit=2,
  cmCapReceiveAndTransmit=3
} cmCapDirection;



/* cmCapRejectCause
 * ----------------------------------------------------------------------------
 * TerminalCapabilitySetReject message cause types.
 * This enumeration should be used with cmCallCapabilitiesReject().
 */
typedef enum
{
    cmCapRejectCauseUnspecified = 1,
    cmCapRejectCauseUndefinedTableEntry = 2,
    cmCapRejectCauseDescriptorExceeded = 3,
    cmCapRejectCauseTableEntryExceeded = 4
} cmCapRejectCause;


typedef enum
{
  cmCapEmpty=0, /*in case of an empty capability set*/
  cmCapAudio=1, /*in case of receiveAudioCapability,
                             transmitAudioCapability or
                             receiveAndTransmitAudioCapability */
  cmCapVideo=2, /*in case of receiveVideoCapability,
                             transmitVideoCapability or
                             receiveAndTransmitVideoCapability */
  cmCapData=3, /*in case of receiveDataApplicationCapability,
                            transmitDataApplicationCapability or
                            receiveAndTransmitDataApplicationCapability */
  cmCapNonStandard=4, /*in case of nonstandard capability */
  cmCapUserInput=5, /*in case of receiveUserInputCapability ,
                                 transmitUserInputCapability or
                                 receiveAndTransmitUserInputCapability */
  cmCapConference=6, /*in case of conferenceCapability */
  cmCapH235=7, /*in case of h235SecurityCapability */
  cmCapMaxPendingReplacementFor=8, /*in case of maxPendingReplacementFor capability */
  cmCapGeneric=9, /*in case of genericControlCapability capability */
  cmCapMultiplexedStream=10, /*in case of receiveMultiplexedStreamCapability,
                                          transmitMultiplexedStreamCapability or
                                          receiveAndTransmitMultiplexedStreamCapability */
  cmCapAudioTelephonyEvent=11, /*in case of receiveRTPAudioTelephonyEventCapability */
  cmCapAudioTone=12, /*in case of receiveRTPAudioToneCapability */
  cmCapFEC=13, /* Forward Error Correction */
  cmCapMultiplePayloadStream=14 /*in case of multiplePayloadStreamCapability */
} cmCapDataType;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
#include "faststart.h"
#endif


typedef struct
{
  char*             name;
  RvInt32           capabilityId; /* capabilityTableEntryNumber */
  int               capabilityHandle; /* capability item message tree (video/audio/data/nonStandard) */
  cmCapDirection    direction;
  cmCapDataType     type;
} cmCapStruct;


typedef enum
{
  cmCapAccept=1,
  cmCapReject
} cmCapStatus;


typedef enum
{
  cmMSSlave=1,
  cmMSMaster,
  cmMSError
} cmMSStatus;



typedef enum
{
  cmRequestCloseRequest, /* Received request to close channel */
  cmRequestCloseConfirm, /* Request to close this channel is confirmed */
  cmRequestCloseReject   /* Request to close this channel is rejected */
} cmRequestCloseStatus;


typedef enum
{
  cmMediaLoopRequest,    /* Received request for a media loop on a channel */
  cmMediaLoopConfirm,    /* Media loop confirmed */
  cmMediaLoopOff         /* Media loop on channel is canceled */
} cmMediaLoopStatus;



typedef struct
{
  char* data; /* general string formatted data (4 bytes/char) */
  int   length; /* in bytes */
} cmUserInputData;


typedef enum
{
  /* path in paranthesis indicates the nodeId syntax */
  cmReqModeRequest,    /* request mode (requestMode.requestedModes) */
  cmReqModeAccept,     /* request acknoledged (requestModeAck.response) */
  cmReqModeReject,     /* request rejected (requestModeReject.cause) */
  cmReqModeReleaseOut, /* Outgoing request released due to timeout (N/A) */
  cmReqModeReleaseIn   /* Incoming request released due to timeout (N/A) */
} cmReqModeStatus;


typedef enum
{
  cmMiscMpOn,  /* multipoint mode command is on (set) */
  cmMiscMpOff  /* multipoint mode command is off (cancel) */
} cmMiscStatus;


typedef struct
{
  char  name[32];
  RvInt32 entryId; /* ModeElement node id */
} cmReqModeEntry;



typedef enum
{
  cmControlStateConnected,
  cmControlStateConference,
  cmControlStateTransportConnected,
  cmControlStateTransportDisconnected,
  cmControlStateFastStart,
  cmControlStateFastStartComplete
} cmControlState;


typedef enum
{
  cmControlStateModeNull
} cmControlStateMode;


typedef enum
{
  cmCloseReasonUnknown=0,
  cmCloseReasonReopen   ,
  cmCloseReasonReservationFailure,
  cmCloseReasonNormal /* for request close only */
} cmCloseLcnReason;


typedef enum
{
  cmRejectReasonUnspecified=0,
  cmRejectReasonUnsuitableReverseParameters,
  cmRejectReasonDataTypeNotSupported,
  cmRejectReasonDataTypeNotAvailable,
  cmRejectReasonUnknownDataType,
  cmRejectReasonDataTypeALCombinationNotSupported,
  cmRejectReasonMulticastChannelNotAllowed,
  cmRejectReasonInsufficientBandwidth,
  cmRejectReasonSeparateStackEstablishmentFailed,
  cmRejectReasonInvalidSessionID,
  cmRejectReasonMasterSlaveConflict,
  cmRejectReasonWaitForCommunicationMode,
  cmRejectReasonInvalidDependentChannel,
  cmRejectReasonReplacementForRejected
} cmRejectLcnReason;



typedef enum
{
    cmVideoFreezePicture=0,
    cmVideoSendSyncEveryGOB,
    cmVideoSendSyncEveryGOBCancel,
    cmSwitchReceiveMediaOff,
    cmSwitchReceiveMediaOn,
    cmVideoFastUpdatePicture
} cmMiscellaneousCommand;


/* Enumeration of messages supported by cmEvCallControlMessage callback function */
typedef enum
{
    /* Conference Request */
    h245crqTerminalListRequest,
    h245crqMakeMeChair,
    h245crqCancelMakeMeChair,
    h245crqDropTerminal,
    h245crqRequestTerminalID,
    h245crqEnterH243Password,
    h245crqEnterH243TerminalID,
    h245crqEnterH243ConferenceID,
    h245crqRequestChairTokenOwner,
    h245crqRequestTerminalCertificate,
    h245crqBroadcastMyLogicalChannel,
    h245crqMakeTerminalBroadcaster,
    h245crqSendThisSource,
    h245crqRequestAllTerminalIDs,
    h245crqRemoteMCRequest,

    /* Conference Response*/
    h245crsMCTerminalIDResponse,
    h245crsTerminalIDResponse,
    h245crsConferenceIDResponse,
    h245crsPasswordResponse,
    h245crsTerminalListResponse,
    h245crsVideoCommandReject,
    h245crsTerminalDropReject,
    h245crsMakeMeChairResponse,
    h245crsChairTokenOwnerResponse,
    h245crsTerminalCertificateResponse,
    h245crsBroadcastMyLogicalChannelResponse,
    h245crsMakeTerminalBroadcasterResponse,
    h245crsSendThisSourceResponse,
    h245crsRequestAllTerminalIDsResponse,
    h245crsRemoteMCResponse,

    /* Conference Indications */
    h245ciSbeNumber,
    h245ciTerminalJoinedConference,
    h245ciTerminalLeftConference,
    h245ciSeenByAtLeastOneOther,
    h245ciCancelSeenByAtLeastOneOther,
    h245ciSeenByAll,
    h245ciCancelSeenByAll,
    h245ciTerminalYouAreSeeing,
    h245ciRequestForFloor,
    h245ciWithdrawChairToken,
    h245ciFloorRequested,
    h245ciTerminalYouAreSeeingInSubPictureNumber,
    h245ciVideoIndicateCompose,

    /* Conference Commands */
    h245ccBroadcastMyLogicalChannel,
    h245ccCancelBroadcastMyLogicalChannel,
    h245ccMakeTerminalBroadcaster,
    h245ccCancelMakeTerminalBroadcaster,
    h245ccSendThisSource,
    h245ccCancelSendThisSource,
    h245ccDropConference,
    h245ccSubstituteConferenceIDCommand,

    /* Miscellaneous Command */
    h245mcEqualiseDelay,
    h245mcZeroDelay,
    h245mcMaxH223MUXPDUsize,
    h245mcEncryptionUpdate,
    h245mcEncryptionUpdateRequest,
    h245mcProgressiveRefinementStart,
    h245mcProgressiveRefinementAbortOne,
    h245mcProgressiveRefinementAbortContinuous,
    h245mcVideoBadMBs,
    h245mcLostPicture,
    h245mcLostPartialPicture,
    h245mcRecoveryReferencePicture,

    /* Indication */
    h245iJitterIndication,
    h245iH223SkewIndication,
    h245iFunctionNotSupported,
    h245iFlowControlIndication,

    /* Miscellaneous Indication */
    h245miMultipointZeroComm,
    h245miCancelMultipointZeroComm,
    h245miMultipointSecondaryStatus,
    h245miCancelMultipointSecondaryStatus,
    h245miVideoIndicateReadyToActivate,
    h245miVideoNotDecodedMBs,

    /* Generic Messages */
    h245grqGenericRequest,
    h245grsGenericResponse,
    h245gcGenericCommand,
    h245giGenericIndication
} cmH245MessageType;


typedef enum
{
    cmH245ConflictUnansweredProposals
} cmH245ChannelConflictType;




#if (RV_H245_SUPPORT_H225_PARAMS == 1)
/* ------------------------- H.225 structures -------------------------- */

typedef struct
{
  RvUint8 mcuNumber; /* constraint: 0..192 */
  RvUint8 terminalNumber; /* constraint: 0..192 */
} cmTerminalLabel;




typedef struct
{
  HCHAN hsChan; /* outgoing: new reverse channel. incoming: keep the existing channel
           null: new not associated reverse channel. */

  /* ! set the following fields only for new outgoing channels */

  HCHAN hsChanAssociate; /* associate channel. FFS */

  char *channelName; /* channel name from configuration or NULL for handle setting. */
  RvInt32 channelDataTypeHandle; /* data type (HPVT) tree handle. channelName should be NULL. */

  cmTerminalLabel terminalLabel; /* Use 0xffff if label unavailable */

  char *sessionDescription;
  int sessionDescriptionLength; /* number of bytes in sessionDescription */

  cmTransportAddress rtpAddress;
  cmTransportAddress rtcpAddress;

    /* Used only in cmCallSetChannelsExt function */
  int uid; /* uniq id may be index in array.*/
  int dependency; /* uniq id of element,which describe the channel,that this channel is dependent on */
  /* if its value is equal -1 ,parameter is not used. */
  int redEncodingId; /* node id of redundancyEncoding pvt,built by user. */

} cmChannelStruct;

typedef struct
{
  int redundancyEncodingMethodId;
  int secondaryEncodingId;
} cmRedundancyEncoding;



typedef enum
{
  cmIssueJoin   =0,
  cmIssueQueryAndJoin,
  cmIssueCreate ,
  cmIssueInvite ,
  cmWaitForInvite,
  cmOriginateCall,
  cmWaitForCall,
  cmIssueQuery
} cmT120SetupProcedure;

/* -------------------------H.225 functions ---------------------------*/

RVAPI
int RVCALLCONV cmChannelSameSession(
             /* get the same session opposite channel of hsChan */
             IN     HCHAN           hsChan,
             OUT    HAPPCHAN*       haSameSession,
             OUT    HCHAN*          hsSameSession
             );

RVAPI int RVCALLCONV /* returns the session id for this channel */
cmChannelSessionId(
           /* get the session id of channel */
           IN   HCHAN           hsChan
           );



RVAPI int RVCALLCONV
cmChannelSetT120Setup(
             IN      HCHAN               hsChan,
             IN      cmT120SetupProcedure t120SetupProc);

RVAPI int RVCALLCONV
cmChannelGetT120Setup(
             IN      HCHAN               hsChan,
             OUT      cmT120SetupProcedure *t120SetupProc);


/* set FlowControlToZero on incoming channel during establising process
  used before answering */

RVAPI int RVCALLCONV
cmChannelSetFlowControlToZero(
             IN      HCHAN               hsChan,
             IN      RvBool              flowControl);



RVAPI
int RVCALLCONV cmChannelSetNSAPAddress(
        IN      HCHAN               hsChan,
        IN      const RvChar*       address,
        IN      int                 length,
        IN      RvBool              multicast);

///added by wangxiaoyi for 460 keepalivechannel call back funciton on 31 July 2010 
typedef int
(RVCALLCONV *cmEvChannelSetKeepAliveT)(
		IN      HAPPCHAN              haChan,
		IN      HCHAN                 hsChan,
        IN      int                   mesage);

RVAPI
int RVCALLCONV cmChannelSetATMVC(
        IN      HCHAN               hsChan,
        IN      int                 portNumber);


RVAPI
int RVCALLCONV cmChannelSetAddress(
        IN      HCHAN               hsChan,
        IN      RvUint32            ip,
        IN      RvUint16            port);

RVAPI
int RVCALLCONV cmChannelSetRTCPAddress(
        IN      HCHAN               hsChan,
        IN      RvUint32            ip,
        IN      RvUint16            port);

RVAPI
int RVCALLCONV cmChannelSetDynamicRTPPayloadType(
        IN      HCHAN           hsChan,
        IN      RvInt8          dynamicPayloadType);


RVAPI int RVCALLCONV
cmCallSetTerminalLabel(
               /* Set the remote endpoint terminal label. */
               IN      HCALL               hsCall,
               IN      cmTerminalLabel*    terminalLabel
               );

RVAPI int RVCALLCONV /* negative value on failure if terminal label is not defined for this terminal */
cmCallGetTerminalLabel(
               /* Get the local endpoint terminal label. */
               IN      HCALL               hsCall,
               OUT     cmTerminalLabel*    terminalLabel /* user allocated */
               );


/* When the conference becomes active, the master (MC) terminal shall call cmCallDeclareMC() to
   indicate the new conference status. */
RVAPI
int RVCALLCONV cmCallDeclareMC(
                 /* declare this terminal to be the MC of the call */
        IN      HCALL               hsCall
        );




/* When the conference becomes active, these functions provides the address of the Active MC. */
RVAPI
int RVCALLCONV cmCallGetMcTransportAddress(
                IN      HCALL               hsCall,
                OUT     cmTransportAddress* ta);


#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) || defined RV_H323_COMPILE_WITH_DEAD_FUNCTIONS
RVAPI
int RVCALLCONV cmCallGetMCAddress(
                /* is not supported when working with IPv6 */
        IN      HCALL               hsCall,
        OUT     RvUint32*           ip,
        OUT     RvUint16*           port);
#endif /* (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_ANY) || (RV_H323_COMPILE_WITH_DEAD_FUNCTIONS) */


/* The master (MC) terminal shall use cmCallSetChannels() to force the remote terminal to open
   the indicated channels for transmit. */
RVAPI
int RVCALLCONV cmCallSetChannels(
                 /* As an Active MC, set transfer channels for remote terminal */
        IN      HCALL               hsCall,
        IN      int                 channelSetSize, /* number of elements in channelSet */
        IN      cmChannelStruct     channelSet[]
        );

/* The master (MC) terminal shall use cmCallSetChannels() to force the remote terminal to open
   the indicated channels for transmit.Add few parameters specified in version 3,such as redEncoding
   and sessionDependancy */
RVAPI
int RVCALLCONV cmCallSetChannelsExt(
                 /* As an Active MC, set transfer channels for remote terminal */
        IN      HCALL               hsCall,
        IN      int                 channelSetSize, /* number of elements in channelSet */
        IN      cmChannelStruct     channelSet[]
        );

RVAPI
int RVCALLCONV cmChannelSetDuplexAddress(
                       /* Set address of duplex connection */
         IN     HCHAN               hsChan,
         IN     cmTransportAddress  address,
         IN     int                 externalReferenceLength,
         IN     const RvChar*       externalReference, /* NULL if not exists */
         IN     RvBool              isAssociated /* RV_TRUE if associated */
         );

RVAPI
int RVCALLCONV /* actual size of external reference or negative value on failure */
cmChannelGetDuplexAddress(
              /* Get address of duplex connection */
         IN     HCHAN               hsChan,
         OUT    cmTransportAddress* address, /* User allocated structure */
         IN     int                 externalReferenceLength, /* size of extRef buffer */
         OUT    char*               externalReference, /* User allocated buffer */
         OUT    RvBool*               isAssociated /* RV_TRUE if associated */
         );

/////////////////////////////////////////begin with added by wangxiaoyi on 2 Semptember 2010 for h460 keepalive channel
RVAPI
int RVCALLCONV cmChannelAttachH460Info(
									   IN      HCHAN               hsChan,
									   IN      int                 genericinfo /* genericinfo (HPVT) tree handle */
									   );
/////////////////////////////////////////end with added by wangxiaoyi on 2 Semptember 2010 for h460 keepalive channel

RVAPI int RVCALLCONV
cmChannelSetTransportCapability(
             IN      HCHAN               hsChan,
             IN      int                 transportCapId);

RVAPI int RVCALLCONV
cmChannelGetTransportCapabilityId(
             IN      HCHAN               hsChan);

RVAPI int RVCALLCONV
cmChannelSetRedundancyEncoding(
             IN      HCHAN               hsChan,
         IN     cmRedundancyEncoding * redundancyEncoding);

RVAPI int RVCALLCONV
cmChannelGetRedundancyEncoding(
             IN      HCHAN               hsChan,
         OUT     cmRedundancyEncoding * redundancyEncoding);


RVAPI int RVCALLCONV
cmChannelSetSource(
             IN      HCHAN               hsChan,
         IN     cmTerminalLabel *terminalLabel);

RVAPI int RVCALLCONV
cmChannelGetSource(
             IN      HCHAN               hsChan,
         OUT     cmTerminalLabel *terminalLabel);


RVAPI int RVCALLCONV
cmChannelSetDestination(
         IN      HCHAN               hsChan,
         IN     cmTerminalLabel *terminalLabel);

RVAPI int RVCALLCONV
cmChannelGetDestination(
         IN      HCHAN               hsChan,
         OUT     cmTerminalLabel     *terminalLabel);


RVAPI
int RVCALLCONV cmChannelMaxSkew(
        IN      HCHAN               hsChan1,
        IN      HCHAN               hsChan2,
        IN      RvUint32            skew);


/******************************************************************************
 * cmCallGetChannelBySessionId
 * ----------------------------------------------------------------------------
 * General: Find a channel of a call from its sessionId and direction.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall       - The call handle.
 *         sessionId    - Session ID to look for.
 *         origin       - RV_TRUE if we are the origin of the searched channel.
 *                        RV_FALSE if we are not the origin of the searched
 *                        channel.
 * Output: hsChan       - Channel that was found.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallGetChannelBySessionId(
       IN   HCALL       hsCall,
       IN   int         sessionId,
       IN   RvBool      origin,
       OUT  HCHAN       *hsChan);



/* ----------------------- H.225 Callbacks ---------------------------- */

typedef int
    (RVCALLCONV *cmEvChannelSetAddressT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      RvUint32            ip,
        IN      RvUint16            port);

typedef int
    (RVCALLCONV *cmEvChannelSetRTCPAddressT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      RvUint32            ip,
        IN      RvUint16            port);
//add by yj for ipv6
//typedef int
//   (RVCALLCONV *cmEvChannelGetParamT)(
//   IN          HAPPCHAN             haChan, 
//   IN          HCHAN                hsChan, 
 //  IN          cmChannelParam       eParam, 
//   IN          cmTransportAddress   RTAddr);
//end

typedef int
    (RVCALLCONV *cmEvChannelRTPDynamicPayloadTypeT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      RvInt8              dynamicPayloadType);

typedef int
    (RVCALLCONV *cmEvChannelGetRTCPAddressT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      RvUint32*           ip,
        IN      RvUint16*           port);

typedef int
    (RVCALLCONV * cmEvChannelFlowControlToZeroT)(
         IN      HAPPCHAN             haChan,
         IN      HCHAN                hsChan);

typedef int
    (RVCALLCONV *cmEvChannelSetNSAPAddressT)(
        IN      HAPPCHAN              haChan,
        IN      HCHAN                 hsChan,
        IN      char*                 address,
        IN      int                   length,
        IN      RvBool                multicast);


typedef int
    (RVCALLCONV *cmEvChannelSetATMVCT)(
        IN      HAPPCHAN              haChan,
        IN      HCHAN                 hsChan,
        IN      int                   portNumber);






#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */


/* -------------------------H.223 functions ---------------------------*/
#if (RV_H245_SUPPORT_H223_PARAMS == 1)

/******************************************************************************
 * cmH223ALLevel
 * ----------------------------------------------------------------------------
 * Definitions of Adaptation layer level.
 *****************************************************************************/
typedef enum
{
    cmH223ALLevelUnknown,
    cmH223ALLevel1,
    cmH223ALLevel2,
    cmH223ALLevel3
}cmH223ALLevel;

/******************************************************************************
 * cmH223ControlFieldSize
 * ----------------------------------------------------------------------------
 * Control field sizes that are used for retransmission in AL3.
 *****************************************************************************/
typedef enum
{
    cmH223ControlFieldSizeNone = 0,
    cmH223ControlFieldSize1Byte,
    cmH223ControlFieldSize2Bytes
}cmH223ControlFieldSize;

/* cmChannelAudioType
 * ------------------------------------------------------------------------
 * Audio types supported by the 3G-324M Toolkit.
 * Needed for the generation of the multiplex entries table.
 */
typedef enum
{
    cmChannelAudioUnknown  = -1,

    /* Audio codec types by their bitrate */
    cmChannelAudioAMR_4_75 = 0,
    cmChannelAudioAMR_5_15 = 1,
    cmChannelAudioAMR_5_90 = 2,
    cmChannelAudioAMR_6_70 = 3,
    cmChannelAudioAMR_7_40 = 4,
    cmChannelAudioAMR_7_95 = 5,
    cmChannelAudioAMR_10_2 = 6,
    cmChannelAudioAMR_12_2 = 7,
    cmChannelAudioG723_53  = 8,
    cmChannelAudioG723_63  = 9,

} cmChannelAudioType;



/******************************************************************************
 * cmChannelH223MaxSkew
 * ----------------------------------------------------------------------------
 * General: Build and send h223SkewIndication message.
 *          Note that cmEvChannelMaxSkew callback is used to notify the
 *          application that h223SkewIndication message was received.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan1      - The first channel to indicate skew for.
 *         hsChan2      - The second channel to indicate skew for.
 *         skew         - The skew number in milliseconds.
 * Output: none.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmChannelH223MaxSkew(
         IN     HCHAN           hsChan1,
         IN     HCHAN           hsChan2,
         IN     RvUint32        skew);


/******************************************************************************
 * cmCallMultiplexEntrySend
 * ----------------------------------------------------------------------------
 * General: Start multiplexEntrySend request.
 *          message NodeId is not deleted and not modified by this function.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall  - The call handle
 *         message - The root nodeId of the PVT tree to send.
 *                   Of type MultiplexEntrySend.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallMultiplexEntrySend(
               IN   HCALL       hsCall,
               IN   RvPvtNodeId message);


/******************************************************************************
 * cmCallMultiplexEntrySendResponse
 * ----------------------------------------------------------------------------
 * General: Send multiplexEntrySend response (ack or reject).
 *          message NodeId is not deleted and not modified by this function.
 *          This function can only be called if the H245 configuration
 *          states the use of manual response for multiplex entry table
 *          (i.e: h245.multiplexEntryTable.manualResponse is set).
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall  - The call handle
 *         isOk    - RV_TRUE if multiplexEntrySendAck is to be sent.
 *                   RV_FALSE if multiplexEntrySendReject is to be sent.
 *         message - The root nodeId of the PVT tree to send.
 *                   Should hold the MultiplexEntrySendAck.multiplexTableEntryNumber
 *                   if isOk==RV_TRUE,
 *                   or MultiplexEntrySendReject.rejectionDescriptions
 *                   if isOk==RV_FALSE.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmCallMultiplexEntrySendResponse(
               IN   HCALL       hsCall,
               IN   RvBool      isOk,
               IN   RvPvtNodeId message);


#endif /* RV_H245_SUPPORT_H223_PARAMS */


/* cmChannelParam
 * ------------------------------------------------------------------------
 * Defines the parameters of the channel.
 */
typedef enum
{
    cmChannelParamUnknown  = -1,

    cmChannelParamDataTypeNodeId,               /* RvPvtNodeId: PVT Node Id of the dataType of this channel */

    /*-- H.223 related parameters - Used by 3G-324M --*/
#if (RV_H245_SUPPORT_H223_PARAMS == 1)
    cmChannelParamH223AlLevel = 1000,       /* cmH223ALLevel: Adaptation Layer */
    cmChannelParamH223Segmentable,          /* RvBool: Defines whether the logical channel is segmentable or non-segmentable
                                               Should be set to RV_TRUE for video channels and RV_FALSE for audio channels */
    /* AL1 parameters only */
    cmChannelParamH223Al1Framed,            /* RvBool: Defines whether the bit stream is framed or unframed. In use by AL1 channels.*/
    /* AL2 parameters only */
    cmChannelParamH223Al2SequenceNumber,    /* RvBool: Defines whether to use sequence numbering. In use by AL2 channels.*/
    /* AL3 parameters only */
    cmChannelParamH223Al3ControlFieldSize,  /* cmH223ControlFieldSize: The size of MUX-SDU header. In use by AL3 channels.
                                               This value is stored in h223LogicalChannelParameters.adaptationLayerType.al3.controlFieldOctets.
                                               In order to use AL3 with retransmissions, this parameter must be set to cmH223ControlFieldSize1Byte or cmH223ControlFieldSize2Bytes. */
    cmChannelParamH223Al3SendBufferSizeForRetransmissions,  /* RvUint32: The size of all MUX-SDU saved concurrently when using AL3 retransmissions.
                                                               This value is stored in h223LogicalChannelParameters.adaptationLayerType.al3.sendBufferSize.
                                                               In order to use AL3 with retransmissions, this parameter must be set to a number bigger than zero.*/
    /* For Audio channels only */
    cmChannelParamH223AudioType,            /* cmChannelAudioType: The audio type of the channel */
    /* For Video and Data channels only */
    cmChannelParamH223BitRate,              /* RvUint32: The bit rate of the channel in units of 100 bits/s. */
    /* For All Audio and Video Channels */
    cmChannelParamH223IgnoreCorruptedAlSdu, /* RvBool: Defines whether we should pass corrupted AL-SDU to the user or ignore it.
                                                       Used especially for CRC errors. */
#endif

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    cmChannelParamH225LocalRtpAddress = 2000,   /* cmTransportAddress: Local RTP address for incoming channel */
    cmChannelParamH225LocalRtcpAddress,         /* cmTransportAddress: Local RTCP address for the channel */
    cmChannelParamH225RemoteRtpAddress,         /* cmTransportAddress: Remote RTP address for outgoing channel */
    cmChannelParamH225RemoteRtcpAddress,        /* cmTransportAddress: Remote RTCP address for the channel */
    cmChannelParamH225DynamicRTPPayloadType,    /* RvUint32: The dynamic RTP payload type used for this channel, or 0 if none */
    cmChannelParamH225ForceSecondary,           /* RvBool: RV_TRUE if we want to force the stack not to give session id 1-3 to this channel */
#endif

    cmChannelParamLast /* Here to make compilation a bit easier */
} cmChannelParam;


/******************************************************************************
 * cmChannelSetParam
 * ----------------------------------------------------------------------------
 * General: Obtains a specific channel parameter from the application.
 *          This function should be called after the call to cmChannelNew() and before the
 *          call to cmChannelOpen().
 *          See also cmChannelParam, cmChannelGetParam.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan      - The channel handle whose parameter is to be changed.
 *         eParam      - The parameter to be changed. The parameters are defined
 *                       in cmChannelParam enumeration.
 *         value       - If the parameter value is a simple integer value, this
 *                       is the value itself.
 *         strValue    - If the parameter value is a string, strValue
 *                       represents the parameter value.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmChannelSetParam(
         IN      HCHAN               hsChan,
         IN      cmChannelParam      eParam,
         IN      RvInt32             value,
         IN      const RvUint8       *strValue);

/******************************************************************************
 * cmChannelGetParam
 * ----------------------------------------------------------------------------
 * General: Gets a specific channel parameter for the application.
 *          See also cmChannelParam, cmChannelSetParam.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan      - The channel handle whose parameter has been requested.
 * Output: eParam      - The parameter requested. The list of parameters is
 *                       defined in cmCallParam enumeration.
 *         value       - If the parameter value is a simple integer,
 *                       this is the pointer to the parameter value.
 *         strValue    - If the parameter value is a string, strValue
 *                       represents the parameter itself.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmChannelGetParam(
         IN      HCHAN               hsChan,
         IN      cmChannelParam      eParam,
         OUT     RvInt32             *value,
         OUT     RvUint8             *strValue);

/******************************************************************************
 * cmChannelGetHandle
 * ----------------------------------------------------------------------------
 * General: Returns the application handle for a channel from the channel handle.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan      - The stack handle for the channel.
 * Output: haChan      - The returned application handle of the channel.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV
cmChannelGetHandle(
                IN  HCHAN hsChan,
                OUT HAPPCHAN* haChan);


/* Callback function prototypes____________________________________________________________________________*/






/* --- Control session callback functions --- */

typedef int
    (RVCALLCONV *cmEvCallCapabilitiesT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      cmCapStruct*        capabilities[]);

typedef int
    (RVCALLCONV *cmEvCallCapabilitiesExtT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      cmCapStruct***      capabilities[]);

typedef int
    (RVCALLCONV *cmEvCallNewChannelT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      HCHAN               hsChan,
        OUT     LPHAPPCHAN          lphaChan);

typedef int
    (RVCALLCONV *cmEvCallCapabilitiesResponseT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvUint32            status);

typedef int
    (RVCALLCONV *cmEvCallMasterSlaveStatusT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvUint32            status);

typedef int
    (RVCALLCONV *cmEvCallMultiplexEntryT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvPvtNodeId         descriptors);

typedef int
    (RVCALLCONV *cmEvCallMultiplexEntryResponseT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvBool              isACK,  /*if RV_TRUE = MESACK , RV_FALSE = MESReject*/
        IN      RvUint16            includedEntries,
        IN      RvPvtNodeId         descriptions);



typedef int
    (RVCALLCONV *cmEvCallRoundTripDelayT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvInt32             delay); /* positive: msec delay, negative: expiration */

typedef int
    (RVCALLCONV *cmEvCallUserInputT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvInt32             userInputId /* user input node id */
        );

typedef int
    (RVCALLCONV *cmEvCallRequestModeT)(
         /* Request to transmit the following modes preferences */
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      cmReqModeStatus     status,
        IN      RvInt32             nodeId /* pvt nodeId handle according to status */
        );


typedef int
    (RVCALLCONV *cmEvCallMiscStatusT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      cmMiscStatus        status);


typedef int
    (RVCALLCONV *cmEvCallControlStateChangedT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      cmControlState      state,
        IN      cmControlStateMode  stateMode);


typedef int
    (RVCALLCONV *cmEvCallMasterSlaveT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvUint32            terminalType,
        IN      RvUint32            statusDeterminationNumber);



//add by yj
typedef int
(RVCALLCONV *cmEvCallMultiplexCapabilityT)(
										   IN      HAPPCALL            haCall,
										   IN      HCALL               hsCall,
										   IN      int                 message
        );
//end





/********************************************************************************************
 * cmEvCallControlMessageT
 * purpose : Notification function for the application that an incoming H.245 message
 *           was received. The messages reported by this callback are not handled by
 *           internal state machines in the stack.
 * input   : haCall         - Application's handle for the call
 *           hsCall         - Stack's handle for the call
 *           message        - Node ID of the received message
 *           messageType    - Type of message received
 * output  : none
 ********************************************************************************************/
typedef int
    (RVCALLCONV *cmEvCallControlMessageT)(
        IN  HAPPCALL            haCall,
        IN  HCALL               hsCall,
        IN  HAPPCHAN            haChan,
        IN  HCHAN               hsChan,
        IN  RvPvtNodeId         message,
        IN  cmH245MessageType   messageType);


/********************************************************************************************
 * cmEvCallTerminalNumberAssignT
 * purpose : Notification function for the application that an incoming TerminalNumberAssign
 *           message was received.
 * input   : haCall         - Application's handle for the call
 *           hsCall         - Stack's handle for the call
 *           mcuNumber      - New MCU number
 *           terminalNumber - New terminal number
 * output  : none
 ********************************************************************************************/
typedef int
    (RVCALLCONV *cmEvCallTerminalNumberAssignT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        IN      RvUint8             mcuNumber,
        IN      RvUint8             terminalNumber);


typedef  struct
{
    cmEvCallCapabilitiesT           cmEvCallCapabilities;
    cmEvCallCapabilitiesExtT        cmEvCallCapabilitiesExt;
    cmEvCallNewChannelT             cmEvCallNewChannel;
    cmEvCallCapabilitiesResponseT   cmEvCallCapabilitiesResponse;
    cmEvCallMasterSlaveStatusT      cmEvCallMasterSlaveStatus;
    cmEvCallRoundTripDelayT         cmEvCallRoundTripDelay;
    cmEvCallUserInputT              cmEvCallUserInput;
    cmEvCallRequestModeT            cmEvCallRequestMode;
    cmEvCallMiscStatusT             cmEvCallMiscStatus;
    cmEvCallControlStateChangedT    cmEvCallControlStateChanged;
    cmEvCallMasterSlaveT            cmEvCallMasterSlave;
    cmEvCallControlMessageT         cmEvCallControlMessage;
    cmEvCallMultiplexEntryResponseT cmEvCallMultiplexEntryResponse;
    cmEvCallMultiplexEntryT         cmEvCallMultiplexEntry;
    cmEvCallTerminalNumberAssignT   cmEvCallTerminalNumberAssign;
	//add by yj
	cmEvCallMultiplexCapabilityT    cmEvCallMultiplexCapability;
	//end
} SCMCONTROLEVENT,*CMCONTROLEVENT;

typedef SCMCONTROLEVENT SCMSESSIONEVENT; /* for backward compatability */
typedef CMCONTROLEVENT CMSESSIONEVENT;




/* --- Control channels callback functions --- */


typedef int
    (RVCALLCONV *cmEvChannelStateChangedT)(
        IN      HAPPCHAN                haChan,
        IN      HCHAN                   hsChan,
        IN      cmChannelState_e        state,
        IN      cmChannelStateMode_e    stateMode);

typedef int
    (RVCALLCONV *cmEvChannelNewRateT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      RvUint32            rate);

typedef int
    (RVCALLCONV *cmEvChannelMaxSkewT)(
        IN      HAPPCHAN            haChan1,
        IN      HCHAN               hsChan1,
        IN      HAPPCHAN            haChan2,
        IN      HCHAN               hsChan2,
        IN      RvUint32            skew);

typedef int
    (RVCALLCONV *cmEvChannelParametersT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      char*               channelName,
        IN      HAPPCHAN            haChanSameSession,
        IN      HCHAN               hsChanSameSession,
        IN      HAPPCHAN            haChanAssociated,
        IN      HCHAN               hsChanAssociated,
        IN      RvUint32            rate);


typedef int
    (RVCALLCONV *cmEvChannelVideoFastUpdatePictureT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan);

typedef int
    (RVCALLCONV *cmEvChannelVideoFastUpdateGOBT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      int                 firstGOB,
        IN      int                 numberOfGOBs);

typedef int
    (RVCALLCONV *cmEvChannelVideoFastUpdateMBT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      int                 firstGOB,
        IN      int                 firstMB,
        IN      int                 numberOfMBs);

typedef int
    (RVCALLCONV *cmEvChannelHandleT)(
          /* provide data type tree handle for this channel */
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      int                 dataTypeHandle,
        IN      cmCapDataType       dataType);


typedef int
    (RVCALLCONV *cmEvChannelRequestCloseStatusT)(
        IN      HAPPCHAN              haChan,
        IN      HCHAN                 hsChan,
        IN      cmRequestCloseStatus  status);

typedef int
    (RVCALLCONV *cmEvChannelTSTOT)(
        IN      HAPPCHAN              haChan,
        IN      HCHAN                 hsChan,
        IN      RvInt8                isCommand, /* 0: indication, 1: command */
        IN      RvInt8                tradeoffValue); /* 0-31 trade off value */

typedef int
    (RVCALLCONV *cmEvChannelMediaLoopStatusT)(
        IN      HAPPCHAN              haChan,
        IN      HCHAN                 hsChan,
        IN      cmMediaLoopStatus     status);



typedef int
    (RVCALLCONV * cmEvChannelReplaceT)(
         IN     HAPPCHAN              haChan,
         IN     HCHAN                 hsChan,
         IN     HAPPCHAN              haReplacedChannel,
         IN     HCHAN                 hsReplacedChannel);



typedef int
    (RVCALLCONV * cmEvChannelMiscCommandT)(
         IN      HAPPCHAN             haChan,
         IN      HCHAN                hsChan,
         IN      cmMiscellaneousCommand miscCommand);

typedef int
    (RVCALLCONV * cmEvChannelTransportCapIndT)(
         IN      HAPPCHAN             haChan,
         IN      HCHAN                hsChan,
         IN      int                  nodeId);


/******************************************************************************
 * cmEvChannelRecvMessageT
 * ----------------------------------------------------------------------------
 * General: Notification function for the application that an incoming H.245
 *          message related to a specific channel was received by the stack and
 *          is about to be processed.
 * Remarks: This callback will be invoked after the callback indicating a new
 *          message for a call is invoked (cmEvCallRecvMessage in H.323 or
 *          Rv3G324mCallReceiveMessageEv in 3G-324M).
 *          This callback will not be invoked for Fast start messages in H.323.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  haChan   - Application's handle for the channel.
 *         hsChan   - Stack's handle for the channel.
 *         message  - PVT node ID of the message that was received.
 * Output: None.
 *****************************************************************************/
typedef int
    (RVCALLCONV*cmEvChannelRecvMessageT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      RvPvtNodeId         message);


/******************************************************************************
 * cmEvChannelSendMessageT
 * ----------------------------------------------------------------------------
 * General: Notification function for the application that an outgoing H.245
 *          message related to a specific channel is about to be sent by the
 *          stack and.
 * Remarks: This callback will be invoked before the callback indicating
 *          sending a message for a call is invoked (cmEvCallSendMessage in
 *          H.323 or Rv3G324mCallSendMessageEv in 3G-324M).
 *          This callback will not be invoked for Fast start messages in H.323.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  haChan   - Application's handle for the channel.
 *         hsChan   - Stack's handle for the channel.
 *         message  - PVT node ID of the message that is about to be sent.
 * Output: None.
 *****************************************************************************/
typedef int
    (RVCALLCONV*cmEvChannelSendMessageT)(
        IN      HAPPCHAN            haChan,
        IN      HCHAN               hsChan,
        IN      RvPvtNodeId         message);


/******************************************************************************
 * cmEvChannelMasterSlaveConflictT
 * ----------------------------------------------------------------------------
 * General: Notification function for the application that an incoming OLC 
 *          conflicts with an offered OLC. the application needs to decide
 *          what to do with it.
 * Input: haChan       - Application's handle for the channel.
 *        hsChan       - Stack's handle for the channel.
 *        conflictType - Type of conflict
 *        confChans    - array of handles of conflicting channels.
 *        numConfChans - number of channels in the array.
 *        message      - PVT node ID of the incoming OLC.
 * Output: None.
 * Return Value: positive - the SID to use;
 *               zero - use any SID
 *               negative - reject with reason masterSlaveConflict
 *****************************************************************************/
typedef int
    (RVCALLCONV *cmEvChannelMasterSlaveConflictT)(
        IN      HAPPCHAN                    haChan,
        IN      HCHAN                       hsChan,
        IN      cmH245ChannelConflictType   conflictType,
        IN      HCHAN                      *confChans,
        IN      int                         numConfChans,
        IN      RvPvtNodeId                 message);


typedef struct
{
    cmEvChannelStateChangedT            cmEvChannelStateChanged;
    cmEvChannelNewRateT                 cmEvChannelNewRate;
    cmEvChannelMaxSkewT                 cmEvChannelMaxSkew;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    cmEvChannelSetAddressT              cmEvChannelSetAddress;
    cmEvChannelSetRTCPAddressT          cmEvChannelSetRTCPAddress;
	//add by yj foripv6
//	cmEvChannelGetParamT                cmEvChannelGetParam;
    //end
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

    cmEvChannelParametersT              cmEvChannelParameters;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    cmEvChannelRTPDynamicPayloadTypeT   cmEvChannelRTPDynamicPayloadType;
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

    cmEvChannelVideoFastUpdatePictureT  cmEvChannelVideoFastUpdatePicture;
    cmEvChannelVideoFastUpdateGOBT      cmEvChannelVideoFastUpdateGOB;
    cmEvChannelVideoFastUpdateMBT       cmEvChannelVideoFastUpdateMB;
    cmEvChannelHandleT                  cmEvChannelHandle;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    cmEvChannelGetRTCPAddressT          cmEvChannelGetRTCPAddress;
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

    cmEvChannelRequestCloseStatusT      cmEvChannelRequestCloseStatus;
    cmEvChannelTSTOT                    cmEvChannelTSTO;
    cmEvChannelMediaLoopStatusT         cmEvChannelMediaLoopStatus;
    cmEvChannelReplaceT                 cmEvChannelReplace;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    cmEvChannelFlowControlToZeroT       cmEvChannelFlowControlToZero;
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

    cmEvChannelTransportCapIndT         cmEvChannelTransportCapInd;
    cmEvChannelMiscCommandT             cmEvChannelMiscCommand;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    cmEvChannelSetNSAPAddressT          cmEvChannelSetNSAPAddress;
    cmEvChannelSetATMVCT                cmEvChannelSetATMVC;
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

    cmEvChannelRecvMessageT             cmEvChannelRecvMessage;
    cmEvChannelSendMessageT             cmEvChannelSendMessage;

#if (RV_H245_SUPPORT_H225_PARAMS == 1)
    cmEvChannelMasterSlaveConflictT     cmEvChannelMasterSlaveConflict;
#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */
///added by wangxiaoyi for 460 keepalivechannel call back funciton on 31 July 2010 
	cmEvChannelSetKeepAliveT            cmEvChannelSetKeepAlive;
} SCMCHANEVENT,*CMCHANEVENT;







/* General_______________________________________________________________________________________________*/






RVAPI int RVCALLCONV /* Real number of channels in configuration or negative value on failure */
cmGetConfigChannels(
            /* build array containing the channel names as appear in
               the configuration. The strings are copied into array elements */
            IN  HAPP        hApp,
            IN  int         arraySize,
            IN  int         elementLength, /* sizeof each string in array */
            OUT char*       array[] /* allocated with elements */
            );

/************************************************************************
 * cmSetControlEventHandler
 * purpose: Used only for backward compatibility. Also known as cmSetSessionEventHandler.
 *          Installs the control event handler
 * input  : hApp            - Stack handle for the application
 *          cmControlEvent  - Pointer to array of pointers to general event callback functions
 *          size            - Size of *cmControlEvent in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmSetControlEventHandler(
        IN      HAPP                hApp,
        IN      CMCONTROLEVENT      cmControlEvent,
        IN      int                 size);

RVAPI
int RVCALLCONV cmGetControlEventHandler(
        IN      HAPP                hApp,
        OUT     CMCONTROLEVENT      cmControlEvent,
        IN      int                 size);

#define cmSetSessionEventHandler cmSetControlEventHandler /* for backward compatability */


/************************************************************************
 * cmSetChannelEventHandler
 * purpose: Installs the channel event handler
 * input  : hApp            - Stack handle for the application
 *          cmChannelEvent  - Pointer to array of pointers to general event callback functions
 *          size            - Size of *cmChannelEvent in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmSetChannelEventHandler(
        IN      HAPP                hApp,
        IN      CMCHANEVENT         cmChannelEvent,
        IN      int                 size);


RVAPI
int RVCALLCONV cmGetChannelEventHandler(
        IN      HAPP                hApp,
        OUT      CMCHANEVENT         cmChannelEvent,
        IN      int                 size);



/* returns the value-tree node id of h245 configuration root node */
RVAPI
RvInt32 RVCALLCONV cmGetH245ConfigurationHandle(
                IN  HAPP             hApp);


/* Control Session API_______________________________________________________________________________*/



#if (RV_H245_SUPPORT_H225_PARAMS == 1)

RVAPI
int RVCALLCONV cmCallCreateControlSession(
        IN   HCALL           hsCall,
        IN OUT   cmTransportAddress* addr);



RVAPI int RVCALLCONV
cmCallCloseControlSession(
              /* Close the H.245 PDL protocol tree */
              IN     HCALL               hsCall);


RVAPI int RVCALLCONV
cmCallHasControlSession(
              /* Checks if transport connected a control session for the call.
                 Control session may be tunneled */
              IN     HCALL               hsCall);

#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */

/* --- master/slave --- */


RVAPI
int RVCALLCONV cmCallMasterSlaveDetermine(
                    /* Start master slave determination request */
                    IN  HCALL       hsCall,
                    IN  int         terminalType
                    );
#define cmMasterSlaveDetermine cmCallMasterSlaveDetermine



//add by yj
RVAPI int RVCALLCONV cmCallMasterSlaveDetermineResponse(
														IN   HCALL       hsCall,
														IN   int        isMaster
									);
//end



RVAPI int RVCALLCONV /* master/slave status of this call */
cmCallMasterSlaveStatus(
            IN  HCALL       hsCall
            );


RVAPI int RVCALLCONV
cmCallMasterSlaveDetermineExt(
               /*  Start master slave determination request ,
          Acknowledge to master slave determination request
          set terminalType and determinationNumber .
          */
               IN   HCALL       hsCall,
               IN   int         terminalType,
               IN   int         determinationNumber);






/* --- capabilities --- */

RVAPI int RVCALLCONV /* remote terminal capability set node id, or negative value on failure */
cmCallGetRemoteCapabilities(
                /* Get latest remote terminal capability message */
                IN  HCALL       hsCall
                );


RVAPI int RVCALLCONV /* RV_TRUE or negative value on failure */
cmCallSendCapability(
             /* Send terminal capability set tree to remote terminal */
             /* Note: cap tree is not deleted! */
             IN  HCALL hsCall,
             IN  int termCapSet  /* local terminal capabiliy set Id */
             );


RVAPI int RVCALLCONV /* new terminalCapabilitySet node id. */
cmCallCapabilitiesBuild(
            /* Build terminalCapabilitySet value tree including the capability set
               and capability descriptors. */
            IN  HCALL           hsCall,
            IN      cmCapStruct*        capSet[],
            IN      cmCapStruct***      capDesc[]
            );

RVAPI int RVCALLCONV
cmCallCapabilitiesSend(
               IN      HCALL               hsCall,
               IN      cmCapStruct*        capSet[],
               IN      cmCapStruct***      capDesc[]
               );

RVAPI int RVCALLCONV
cmCallGetLocalCapabilities(
               /* translate local caps (from CESE_OUT machine) to application structures.
                  Usage:
                   cmCapStruct capA[capSetASize];
                   cmCapStruct *capSetA[capSetASize];
                   void* capDescA[capDescASize];
                   cmCapStruct** capSet;
                   cmCapStruct**** capDesc;
                   int i;

                   for (i=0; i<capSetASize; i++) capSetA[i] = capA+i;
                   cmCallGetLocalCapabilities(hsCall,
                                              capSetA, capSetSize,
                                              capDescA, capDescASize,
                              &capSet, &capDesc);
                              */

               IN      HCALL                hsCall,
               IN      cmCapStruct*         capSetA[], /* user allocated */
               IN      int                  capSetASize,
               IN      void*                capDescA[], /* user allocated */
               IN      int                  capDescASize,
               OUT     cmCapStruct**        capSet[],
               OUT     cmCapStruct****      capDesc[]
               );





/******************************************************************************
 * cmCallCapabilitiesAck
 * ----------------------------------------------------------------------------
 * General: Replies manually with an acknowledgement to a
 *          terminalCapabilitySet request.
 *
 * Return Value: Non-negative value on success, negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall           - The Stack handle for the call.
 *****************************************************************************/  
RVAPI int RVCALLCONV
cmCallCapabilitiesAck(
               IN HCALL             hsCall);


/******************************************************************************
 * cmCallCapabilitiesReject
 * ----------------------------------------------------------------------------
 * General: Replies manually with a rejection to a
 *          terminalCapabilitySet request.
 *
 * Return Value: RV_OK on success, negative value on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsCall           - The Stack handle for the call.
 *         cause            - Rejection cause for this message.
 *         highestEntry     - Highest number of entry that was processed from
 *                            the incoming terminalCapabilitySet message.
 *                            This parameter is only valid if cause is set to
 *                            cmCapRejectCauseTableEntryExceeded.
 *                            A 0 value indicates that none of the entries
 *                            were processed.
 *****************************************************************************/  
RVAPI RvStatus RVCALLCONV
cmCallCapabilitiesReject(
               IN HCALL             hsCall,
               IN cmCapRejectCause  cause,
               IN RvInt             highestEntry);



/* --- alive --- */

RVAPI
int RVCALLCONV cmCallRoundTripDelay(
                  /* Measure the round trip delay to the remote terminal */
                  /* Note: maxDelay=0 --> delay taken from configuration */
                  IN    HCALL       hsCall,
                  IN    RvInt32         maxDelay /* maximum waiting delay in seconds */
                  );



/* --- user input --- */


RVAPI int RVCALLCONV
cmCallSendUserInput(
            /* send user input msg: userInputId tree is deleted */
            /* Note: Select one of nonStandard or userData options */
            IN  HCALL hsCall,
            IN  RvInt32 userInputId /* indication.userInput tree node id */
            );

RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputBuildNonStandard(
                /* Build userUser message with non-standard data */
                IN  HAPP hApp,
                IN  cmNonStandardIdentifier *identifier,
                IN  const RvChar *data,
                IN  int dataLength /* in bytes */
                );

RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputBuildAlphanumeric(
                 /* Build userUser message with alphanumeric data */
                 IN  HAPP hApp,
                 IN  cmUserInputData *userData
                 );


/******************************************************************************
 * cmUserInputGet
 * ----------------------------------------------------------------------------
 * General: Creates a string of user data from data stored in a sub-tree.
 *          The data can be standard or Non standard.
 *
 * Return Value: If an error occurs, the function returns a negative value. 
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - The Stack handle for the application.
 *         userInputId  - The PVT node ID of the userInputIndicator sub-tree
 *                        that contains the user data.
 *         identifier   - The vendor and type of the non-standard information.
 *         dataLength   - The length of the data buffer in bytes.
 * Output: data         - Pointer to the data buffer for non-standard data.
 *         dataLength   - 0 for standard data, or the length of buffer for
 *                        non-standard data.
 *         userData     - Pointer to cmUserInputData.
 *****************************************************************************/  
RVAPI int RVCALLCONV
cmUserInputGet(
    IN    HAPP                      hApp,
    IN    RvPvtNodeId               userInputId,
    IN    cmNonStandardIdentifier   *identifier,
    OUT   RvChar                    *data,
    INOUT RvInt32                   *dataLength,
    OUT   cmUserInputData           *userData);


/* --- request mode --- */

RVAPI int RVCALLCONV
cmCallRequestMode(
          /* send request mode msg */
          /* Note: id is deleted when function returns */
          IN  HCALL hsCall,
          IN  RvInt32 modeDescriptorsId /* (requestMode.requestedModes) */
          );

RVAPI int RVCALLCONV
cmCallRequestModeAck(
             /* Acknowledge the request */
             IN  HCALL hsCall,
             IN  const RvChar* responseName /* (requestModeAck.response) */
             );

RVAPI int RVCALLCONV
cmCallRequestModeReject(
            /* Reject the request */
            IN  HCALL hsCall,
            IN  const RvChar* causeName /* requestModeReject.cause */
            );



RVAPI int RVCALLCONV/* request mode node id or negative value on failure */
cmRequestModeBuild(
           /* Build request mode msg */
           /* Note: entryId overrides name */
           IN  HAPP hApp,
           IN  cmReqModeEntry **modes[] /* modes matrix in preference order. NULL terminated arrays */
           );

RVAPI int RVCALLCONV/* RV_TRUE or negative value on failure */
cmRequestModeStructBuild(
             /* Build request mode matrix structure from msg */
             /* Note: entry name as in h.245 standard */
             IN  HAPP hApp,
             IN  RvInt32 descId, /* requestMode.requestedModes node id */
             IN  cmReqModeEntry **entries, /* user allocated entries */
             IN  int entriesSize, /* number of entries */
             IN  void **ptrs, /* pool of pointers to construct modes */
             IN  int ptrsSize, /* number of ptrs */

             /* modes matrix in preference order. NULL terminated arrays */
             OUT cmReqModeEntry ***modes[]
             );




/* Channels____________________________________________________________________________________*/





RVAPI int RVCALLCONV
cmChannelGetCallHandles(
            /* Get the stack and application call handles by the channel handle */
            IN  HCHAN hsChan,
            OUT HCALL *hsCall,
            OUT HAPPCALL *haCall
            );




RVAPI int RVCALLCONV
cmChannelNew(
         IN      HCALL               hsCall,
         IN      HAPPCHAN            haChan,
         OUT     LPHCHAN             lphsChan);

RVAPI int RVCALLCONV
cmChannelSetHandle(
           /* let haChan be the new application handle of this channel */
           IN HCHAN                    hsChan,
           IN HAPPCHAN                 haChan);



/************************************************************************
 * cmChannelOpen
 * purpose: Opens a new logical channel using the definitions in the
 *          Stack Configuration
 * input  : hsChan              - Channel to open
 *          channelName         - Name of the Stack Configuration settings
 *                                entry for the channel to be opened
 *          hsChanSameSession   - Stack handle for channel that belongs to
 *                                the same session (may be NULL)
 *          hsChanAssociated    - Stack handle of outgoing channel that is
 *                                to be associated with the newly created one
 *                                (may be NULL).
 *          rate                - Unused. Reserved for backward compatability
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmChannelOpen(
        IN      HCHAN               hsChan,
        IN      const RvChar*       channelName,
        IN      HCHAN               hsChanSameSession,
        IN      HCHAN               hsChanAssociated,
        IN      RvUint32            rate);

RVAPI
int RVCALLCONV cmChannelOpenDynamic(
        IN      HCHAN               hsChan,
        IN      int                 dataTypeHandle, /* data type (HPVT) tree handle */
        IN      HCHAN               hsChanSameSession,
        IN      HCHAN               hsChanAssociated,
        IN      RvBool              isDynamicPaloadType);


RVAPI
int RVCALLCONV cmChannelOn(
        IN      HCHAN               hsChan);

RVAPI
int RVCALLCONV cmChannelOff(
        IN      HCHAN               hsChan);

RVAPI
int RVCALLCONV cmChannelAnswer(
        IN      HCHAN               hsChan);

RVAPI
int RVCALLCONV cmChannelDrop(
        IN      HCHAN               hsChan);

RVAPI
int RVCALLCONV cmChannelClose(
         IN     HCHAN               hsChan);


RVAPI int RVCALLCONV
cmChannelRequestCloseReject(
                /* Reject the request for closing the outgoing channel. */
                IN  HCHAN       hsChan
                );


/* --- Duplex channels --- */
RVAPI
int RVCALLCONV cmChannelDuplex(
                 /* declare channel as duplex. Meaning full duplex. Composed of
                two uni-directional channels in opposite directions. */
        IN      HCHAN               hsChan);

RVAPI
int RVCALLCONV cmChannelIsDuplex(
                   /* Returns RV_TRUE if channel is duplex. RV_FALSE if not. and negative value
                  in case of error */
         IN     HCHAN               hsChan);







/* Channel commands_____________________________________________________________________________________*/





RVAPI
int RVCALLCONV cmChannelFlowControl(
        IN      HCHAN               hsChan,
        IN      RvUint32            rate);


//add by yj
/* hual add 2005-07-02 */
RVAPI
int RVCALLCONV cmChannelFlowControlIndication(
											  IN     HCHAN       hsChan,
											  IN     /*UINT32*/unsigned int      rate);
//end

RVAPI
int RVCALLCONV cmChannelGetOrigin(
        IN      HCHAN               hsChan,
        OUT     RvBool*             origin);


RVAPI
int RVCALLCONV cmChannelVideoFastUpdatePicture(
        IN      HCHAN               hsChan);

RVAPI
int RVCALLCONV cmChannelVideoFastUpdateGOB(
        IN      HCHAN               hsChan,
        IN      int                 firstGOB,
        IN      int                 numberOfGOBs);

RVAPI
int RVCALLCONV cmChannelVideoFastUpdateMB(
        IN      HCHAN               hsChan,
        IN      int                 firstGOB,
        IN      int                 firstMB,
        IN      int                 numberOfMBs);



RVAPI int RVCALLCONV
cmChannelTSTOCommand(
             /* Send temporal spatial trade off command. Request the remote terminal to change
              the tradeoff. */
             IN HCHAN hsChan, /* incoming channel */
             IN int tradeoffValue /* 0-31 */
             );

RVAPI int RVCALLCONV
cmChannelTSTOIndication(
             /* Send temporal spatial trade off indication. Indicates the current tradeoff value
              of the local terminal. */
             IN HCHAN hsChan, /* outgoing channel */
             IN int tradeoffValue /* 0-31 */
             );



/* Media loop____________________________________________________________________________*/

RVAPI int RVCALLCONV
cmChannelMediaLoopRequest(
              /* Request media loop on this channel */
              IN HCHAN hsChan /* outgoing channel */
              );

RVAPI int RVCALLCONV
cmChannelMediaLoopConfirm(
              /* Confirm media loop request on this channel */
              IN HCHAN hsChan /* incoming channel */
              );

RVAPI int RVCALLCONV
cmChannelMediaLoopReject(
             /* Reject media loop request on this channel */
             IN HCHAN hsChan /* incoming channel */
             );

RVAPI int RVCALLCONV
cmCallMediaLoopOff(
           /* Release all media loops in this call */
           IN HCALL hsCall
           );




/* replace exist channel with new */
RVAPI int RVCALLCONV
cmChannelReplace(
                     IN      HCHAN          hsChan, /* openning outgoing channel,that proposed to replaced exist channel*/
                     IN      HCHAN          hsChanReplace);/* exist outgoing channel,that must be replaced */



/******************************************************************************
 * cmIsChannelReplace
 * ----------------------------------------------------------------------------
 * General: Checks if the given channel (hsChan) is replacing an existing channel 
 *          and, if so, looks for that channel.  
 *
 * Return Value: If an error occurs, the function returns a negative value. 
 *		         1 - Channel is replacing an existing channel. 
 *               0 - Channel is not replacing an existing channel.
 * See Also:
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hsChan           - The Stack handle for the channel. 
 *	       haReplaceChannel - The application handle for the replaced channel.  	
 *         hsReplaceChannel - The stack handle for the replaced channel. 
 *****************************************************************************/  
RVAPI int RVCALLCONV
cmIsChannelReplace(
             IN     HCHAN           hsChan,
             OUT    HAPPCHAN*       haReplaceChannel,
             OUT    HCHAN*          hsReplaceChannel
             );


/* set establishing outgoing channel hsChan dependent on base channel with handle hsChanBase*/
RVAPI int RVCALLCONV
cmChannelSetDependency(
             IN      HCHAN               hsChan,
             IN      HCHAN               hsChanBase);


/* check if channel is dependent on another channel
return RV_TRUE if  it is and return in haBaseChannel and hsBaseChannel
handles of base channel.
*/
RVAPI int RVCALLCONV
cmChannelGetDependency(
                     IN     HCHAN           hsChan,
                     OUT    HAPPCHAN*       haBaseChannel,
                     OUT    HCHAN*          hsBaseChannel);






/* -------------------- close logical channels ----------------- */

RVAPI int RVCALLCONV
cmChannelDropReason(
          /*
        - Close outgoing channel
        - Reject incoming channel open request
        - New: Request to close incoming channel
        - On the other side the close lcn reason is passed via rejectStateMode to the
          cmEvChannelStateChanged callback function.
          */
            IN  HCHAN       hsChan,
            IN  cmCloseLcnReason reason
          );

/************************************************************************
 * cmChannelRejectReason
 * purpose: Reject incoming OpenLogicaChannel request
 * input  : hsChan      - Channel to check
 *          reason      - channel reject reason
 * return : Non-negative value on success
 *          Negative value on failure
 * remar  : Created to allow all reasons for OLC rejection.
 *          cmChannelDropReason will still work using reason 'unspecified'
 ************************************************************************/
RVAPI int RVCALLCONV cmChannelRejectReason(
    IN  HCHAN            hsChan,
    IN  cmRejectLcnReason reason);


RVAPI int RVCALLCONV
cmChannelRequestClose(
          /*
        - New: Request to close incoming channel
          */
          IN    HCHAN       hsChan,
        IN  cmCloseLcnReason reason,
        cmQosCapability * cmQOSCapability
          );

/* get on the outgoing side request to close channel ,reason and Qos capability of remoteendpoint */
RVAPI int RVCALLCONV
cmChannelGetRequestCloseParam(  IN  HCHAN       hsChan,
                                OUT  cmCloseLcnReason *reason,
                                OUT cmQosCapability * cmQOSCapability
                               );

/* Multipoint operations_______________________________________________________________________________*/




RVAPI int RVCALLCONV
cmCallStartConference(
              /* enter call conference mode */
              IN      HCALL               hsCall
              );

RVAPI int RVCALLCONV
cmCallCancelConference(
               /* cancel the call conference mode */
               IN      HCALL               hsCall
               );

//add by yj
RVAPI int RVCALLCONV
cmCallStartConferenceNoCallBack(
								/* enter call conference mode */
								IN      HCALL               hsCall
								);
//end

/*   multipoint mode set/cancel operations */

RVAPI int RVCALLCONV
cmCallMultipointCommand(
          /* Send multipoint command (on or off) message */
          IN  HCALL hsCall,
          IN  RvBool isModeOn /* RV_TRUE: mp mode (on), RV_FALSE: cancel mp mode (off) */
          );


RVAPI int RVCALLCONV
cmCallMultipointStatus(
          /* Get the multipoint mode status (on or off) */
          IN  HCALL hsCall,
          OUT RvBool *isModeOn /* RV_TRUE: mp mode (on), RV_FALSE: cancel mp mode (off) */
          );


RVAPI int RVCALLCONV
cmChannelSendTransportCapInd(
         IN     HCHAN       hsChan,
         IN     int         nodeId);  /* node Id of type TransportCapability */

RVAPI int RVCALLCONV
cmChannelSendMiscCommand(
         IN     HCHAN                  hsChan,
         IN     cmMiscellaneousCommand miscCommand);


/* delete pvt of remote capability to reduce memory*/
RVAPI
int RVCALLCONV cmFreeCapability(  IN HCALL hsCall);




RVAPI
RvInt32 RVCALLCONV cmChannelGetNumber(IN HCHAN hsChan);


/* This function will build a Multimedia System Control Message, and will return the node ID of the message, and
   the node ID to which the message itself (according to the messageType enumerations) should be inserted (if at
   all). the function will return 0 on success and -1 on faliure. */
RVAPI int RVCALLCONV
cmBuildMultimediaSystemControlMessage(IN  HAPP              hApp,
                                      IN  cmH245MessageType messageType,
                                      IN  int               lcn,
                                      OUT RvPvtNodeId *     messageNode,
                                      OUT RvPvtNodeId *     insertionNode);






#ifdef __cplusplus
}
#endif


#endif  /* CMCTRL_H */


