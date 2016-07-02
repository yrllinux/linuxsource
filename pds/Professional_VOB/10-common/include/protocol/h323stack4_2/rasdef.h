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

#ifndef _RASDEF_H
#define _RASDEF_H

#include "rpool.h"
#include "hash.h"
#include "ema.h"
#include "rvh323timer.h"
#include "rvlock.h"
#include "q931asn1.h"
#include "cmiAddOns.h"
#include "cmiras.h"

#ifdef __cplusplus
extern "C" {
#endif


#if defined(RV_DEBUG)
#define RV_RAS_DEBUG
#endif


/* Type of transaction */
#define RAS_OUT_TX    (1) /* Outgoing transaction */
#define RAS_IN_TX     (2) /* Incoming transaction */
#define RAS_PROTCONN  (3) /* RAS HPROTCONN handles */


/* rasTxMsgType enumeration */
typedef enum
{
    rasTxMsgRequest, /* Incoming request message */
    rasTxMsgReject, /* Incoming reject response */
    rasTxMsgConfirm, /* Incoming confirm response */
    rasTxMsgOther /* Other type (RIP, IRR,...) */
} rasTxMsgType;


/* rasTxState enumeration for both incoming and outgoing transactions */
typedef enum
{
    rasTxStateIdle, /* Idle state of transaction */
    rasTxStateReplySent, /* Incoming transaction's reply was sent */
    rasTxStateRipSent, /* Incoming transaction's RIP was sent */
    rasTxStateRequestSent, /* Outgoing transaction's request was sent */
    rasTxStateReplyReceived, /* Outgoing transaction's reply was received */
    rasTxStateRipReceived, /* Outgoing transaction's RIP was received */
    rasTxStateTimedOut /* Outgoing transaction has timedout */
} rasTxState;


/* rasResponseSet enumeration for incoming transactions */
typedef enum
{
    rasNoResponseSet, /* No response created for transaction */
    rasConfirmSet, /* Confirm response created for Teansaction */
    rasRejectSet /* Reject response created for Teansaction */
} rasResponseSet;


/************************************************************************
 * rasMessages enumeration
 * This enumeration holds all the supported message types known to the
 * RAS instance. The specific enumeration value is the index of the
 * message inside RasMessage CHOICE in the H225 ASN.1 definition.
 ************************************************************************/
typedef enum
{
    rasMsgGatekeeperRequest = 0,
    rasMsgGatekeeperConfirm,
    rasMsgGatekeeperReject,
    rasMsgRegistrationRequest,
    rasMsgRegistrationConfirm,
    rasMsgRegistrationReject,
    rasMsgUnregistrationRequest,
    rasMsgUnregistrationConfirm,
    rasMsgUnregistrationReject,
    rasMsgAdmissionRequest,
    rasMsgAdmissionConfirm,
    rasMsgAdmissionReject,
    rasMsgBandwidthRequest,
    rasMsgBandwidthConfirm,
    rasMsgBandwidthReject,
    rasMsgDisengageRequest,
    rasMsgDisengageConfirm,
    rasMsgDisengageReject,
    rasMsgLocationRequest,
    rasMsgLocationConfirm,
    rasMsgLocationReject,
    rasMsgInfoRequest,
    rasMsgInfoRequestResponse,
    rasMsgNonStandardMessage,
    rasMsgUnknownMessageResponse,
    rasMsgRequestInProgress,
    rasMsgResourcesAvailableIndicate,
    rasMsgResourcesAvailableConfirm,
    rasMsgInfoRequestAck,
    rasMsgInfoRequestNak,
    rasMsgServiceControlIndication,
    rasMsgServiceControlResponse,
    rasMsgLast /* Must be the last in enum */
} rasMessages;


/********************************************************************************************
 * Description of THREAD_RasLocalStorage
 * This structure holds thread related information for RAS
 ********************************************************************************************/
typedef struct
{
    RvUint32    bufferSize; /* The size of allocated buffer. We might need that when we'll have to realloc */
    RvUint8*    buffer; /* The encoding/decoding buffer */
} THREAD_RasLocalStorage;


/************************************************************************
 * rasMessageInfoStruct
 * This struct holds static information about a specific message type.
 *
 ************************************************************************/
typedef struct
{
    int                 trType; /* Indication of the transaction type: Incoming or Outgoing.
                                   Can be RAS_OUT_TX, RAS_IN_TX, RV_ERROR_UNKNOWN */
    rasTxMsgType        msgType; /* Type of the message */
    cmRASTransaction    transaction; /* The transaction that this message is associated with
                                        IRR is handled separately. */
} rasMessageInfoStruct;



/* Static informatino about the message types we're dealing with */
extern const rasMessageInfoStruct rasMessageInfo[];



/************************************************************************
 * rasOutTx
 * Outgoing transaction struct
 *
 ************************************************************************/
typedef struct
{
    void*               hashValue; /* Handle to the hash value in the outgoing hash table */
    rasTxState          state; /* Current state of the transaction */
    cmRASTransaction    transactionType; /* Type of transaction we're dealing with */
    cmTransportAddress  destAddress; /* Destination address of this transaction */
    RvInt16             index; /* Index of connection to use for messages */
    RvInt16             isMulticast; /* Is this a multicast transaction or a unicast one */
    RvInt16             retryCount; /* Number of retries left for this transaction */
    RvInt16             timeout; /* Timeout in seconds between retries */
    RvPvtNodeId         txProperty; /* Property DB of the transaction - part of the API */
    HRPOOLELEM          encodedMsg; /* Encoded message handle in RPOOL */
    RvTimer*            timer; /* Timer handle used for retries */
    cmiEvRASResponseT   evResponse; /* Callback that is called on responses. Always NULL for manual RAS. */
    HCALL               hsCall; /* Stack's call handle for this transaction. NULL if not applicable */
    void*               hMsgContext; /* Message context of the incoming response message.
                                        This parameter is used by security applications
                                        that want to pass information for later scrutiny
                                        of the application. */
} rasOutTx;


/************************************************************************
 * rasInTx
 * Incoming transaction struct
 *
 ************************************************************************/
typedef struct rasInTx_tag rasInTx;
struct rasInTx_tag
{
    void*               hashValue; /* Handle to the hash value in the outgoing hash table */
    rasTxState          state; /* Current state of the transaction */
    cmRASTransaction    transactionType; /* Type of transaction we're dealing with */
    cmTransportAddress  destAddress; /* Destination address of this transaction */
    RvInt16             index; /* Index of connection to use for sending replies */
    RvInt8              isMulticast; /* Is this a multicast transaction or a unicast one */
    RvInt8              isAutomaticRAS; /* RV_TRUE if this is an automatic RAS transaction */
    RvInt32             stopTime; /* Absolute stop time of this transaction.
                                     After that time, this transaction should be closed.
                                     Calculated in seconds. */
    RvInt32             ripStopTime; /* Absolute stop time of RIP message on this transaction.
                                        Used to calculate a retransmission of a RIP message.
                                        Calculated in seconds. */
    RvPvtNodeId         txProperty; /* Property DB of the transaction - part of the API */
    rasResponseSet      responseSet; /* Indicates if a response message was already built or not */
    HRPOOLELEM          encodedMsg; /* Encoded message handle in RPOOL of the reply */
    HCALL               hsCall; /* Call handle associated with the transaction. NULL if not applicable */
    void*               hMsgContext; /* Message context of the incoming request message.
                                        This parameter is used by security applications
                                        that want to pass information for later scrutiny
                                        of the application. */
    rasInTx*            next; /* Next transaction in the closed transactions list.
                                 If this is set to NULL, then the transaction wasn't
                                 closed by the application */
};


/************************************************************************
 * rasInTxKey
 * Incoming transaction key struct
 * This is the key used for the hash table
 *
 ************************************************************************/
typedef struct
{
    RvUint32            seqNumber; /* Sequence number of the incoming transaction */
    rasMessages         msgType; /* Type of message invoking this incoming transaction */
    cmTransportAddress  address; /* Address of sender */
} rasInTxKey;


/************************************************************************
 * rasDecoderStruct struct
 * This struct holds information needed for fast decoding of the first
 * 2 fields inside a RAS message. It is calculated on initialization of
 * the RAS instance.
 *
 ************************************************************************/
typedef struct
{
    RvUint32    firstExtField; /* Number of the first extended field in the list
                                  if fields of RasMessage CHOICE */
    RvUint32    numOfMessages; /* Number of RasMessage types in the CHOICE */
    RvUint32*   numOfOptFields; /* Number of the optional fields inside each type of RasMessage */
} rasDecoderStruct;



/************************************************************************
 * rasModule struct
 * This struct holds all the necessary information of the RAS.
 * It is returned by cmiRASInitialize() as the module handle.
 *
 ************************************************************************/
typedef struct
{
    HAPP                    app; /* Stack instance handle */
    RvLogMgr*               logMgr; /* Log manager to use */
    RvLogSource             log; /* Log handle to use for messages */
    RvLogSource             logChan; /* Log handle of the ASN.1 messages */
    RvLogSource             logChanWire; /* Log handle of the ASN.1 messages sent to wire*/
    RvPvtNodeId             confNode; /* Configuration node id for RAS parameters */
    RvUint32                requestSeqNum; /* Last sequence number used for outgoing transactions */
    rasDecoderStruct        decoder; /* Information needed to decode the first 2 fields of
                                        any RAS message. */
    RvUint32                bufferSize;  /* Maximum size of a message in bytes */
    RvBool                  isGatekeeper; /* RV_TRUE if we should act as a GK */

    RvH323TimerPoolHandle   timers; /* Timer pool used for outgoing transactions retransmissions */
    RVHCAT                  hCat; /* CAT instance handle */
    HPVT                    hVal; /* Value tree used for property db and ASN.1 messages */
    HPST                    synMessage; /* RAS message syntax tree */
    HPST                    synProperty; /* RAS property database syntax tree */
    HHASH                   inHash; /* Incoming transactions hash table */
    HEMA                    inRa; /* Incoming transactions array */
    HHASH                   outHash; /* Outgoing transactions hash table */
    HEMA                    outRa; /* Outgoing transactions array */
    HRPOOL                  messages; /* Stored messages for retransmissions */

    /* Various lock objects */
    RvLock                  lockInHash; /* Lock handle of incoming hash table */
    RvLock                  lockOutHash; /* Lock handle of outgoing hash table */
    RvLock                  lockGarbage; /* Lock handle of garbage collection mechanism */

    RvPvtNodeId             defaultMessages[cmRASMaxTransaction][4];
                            /* Default RAS messages to set for outgoing requests and replies. */
                            /* We set the array up to 4 although we only have
                               Request,Confirm,Reject (=3) to allow easier
                               handling and clearer understanding of the code.
                               This doesn't take a lot of memory, as it is
                               allocated once per stack instance. */


    char                    epId[256]; /* Endpoint identifier buffer. Used when the endpoint is registered */
    int                     epIdLen; /* Length of the endpoint identifier in bytes */
    char                    gkId[256]; /* Gatekeeper identifier buffer. Used when the endpoint is registered */
    int                     gkIdLen; /* Length of the gatekeeper identifier in bytes */
    RvPvtNodeId             termAliasesNode; /* Terminal Aliases used for this terminal, as configured by the
                                                application and updated in RCF messages */

	//add by yj [20120114]
	char					userName[128];
	char					passWord[128];
	//end [20120114]

    /* The next 2 parameters are used for garbage collection mechanism of
       the incoming transactions database */
    rasInTx*                firstTx; /* First transaction in list of incoming transactions */
    rasInTx*                lastTx; /* Last transaction in list of incoming transactions */

    SCMRASEVENT             evApp; /* Application callback functions on RAS events */
    SCMAUTORASEVENT         evAutoRas; /* Application callback functions on Automatic RAS events */
    cmiEvRASEPRequestT      evEpRequest; /* Callback function for RAS requests not related to calls */
    cmiEvRASCallRequestT    evCallRequest; /* Callback function for RAS requests related to calls */
    cmiEvRasMessageToSend   evSendMessage;
    cmiEvRASNewCallT        evRASNewCall;

    RvPvtNodeId             gatekeeperRASAddress; /* Gatekeeper's RAS address node ID */
    RvPvtNodeId             gatekeeperCallSignalAddress; /* Gatekeeper's Q931 address node ID */

    /* H.235 (security) related callbacks */
    cmiEvRASNewRawMessageT          cmiEvRASNewRawMessage; /* Called when incoming message needs to be decoded */
    cmiEvRASSendRawMessageT         cmiEvRASSendRawMessage; /* Called when outgoing message needs to be encoded */
    cmiEvRASReleaseMessageContextT  cmiEvRASReleaseMessageContext; /* Called when a message can be discarded */

    /* Resource enumeration values allocated by watchdog */
    RvUint32                rasTimersResourceVal;
} rasModule;



#ifdef __cplusplus
}
#endif

#endif  /* _RASDEF_H */

