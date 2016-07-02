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

#ifndef _TRANSPORTINT_H
#define _TRANSPORTINT_H

#include "rvinternal.h"
#include "rvlock.h"
#include "rvmutex.h"
#include "rvlog.h"
#include "ema.h"
#include "hash.h"
#include "rpool.h"
#include "rvh323timer.h"
#include "cat.h"
#include "pvaltree.h"
#include "annexe.h"
#include "tpkt.h"
#include "transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TPKT header for messages sent on TCP */
#define TPKT_HEADER_SIZE 4

/* Size of the header of a message stored in RPOOL. This header holds the pointer to the
   next message we want to send on the same host/session */
#define MSG_HEADER_SIZE sizeof(void*)

#define TRANS_RPOOL_CHUNK_SIZE 512


/* Types of connection */
typedef enum
{
    cmTransTPKTAnnexECompetition = 0,
    cmTransTPKTConn,
    cmTransAnnexEConn
} TRANSINTCONNTYPE;

typedef enum
{
    hostIdle,
    hostListenning,
    hostConnecting,
    hostListenningConnecting,
    hostConnected,
    hostDoubleConnected,
    hostBusy,
    hostClosing,
    hostClosed
} HOSTSTATE;

typedef enum
{
    tunnNo,
    tunnPossible,
    tunnOffered,
    tunnApproved,
    tunnRejected
} TUNNSTATE;

typedef enum
{
    parllNo,
    parllOffered,
    parllApproved
} PARTUNNSTATE;

typedef enum
{
    fsNo,
    fsOffered,
    fsApproved,
    fsRejected
} FSSTATE;


typedef struct
{
    RvUint32    bufferSize;
    RvUint8*    buffer;
} transThreadCoderLocalStorage;

typedef struct _cmTransSession cmTransSession;
typedef struct _cmTransHost cmTransHost;

struct _cmTransHost
{
    TRANSCONNTYPE       type;
    RvBool              closeOnNoSessions;
    RvBool              isMultiplexed;
    RvBool              remoteCloseOnNoSessions;
    RvBool              remoteIsMultiplexed;
    RvBool              hostIsApproved;
    RvBool              incoming; /* RV_TRUE for incoming hosts */
    RvBool              reported;
    RvBool              dummyHost;
    RvUint8             counter; /* counter for close attempts on annex E hosts */
    RvUint8             remoteH245Version; /* H.245 Version of the remote side if this is an H.245 host */
    cmAnnexEUsageMode   annexEUsageMode;
    HOSTSTATE           state; /* State of the host */
    cmTransportAddress  remoteAddress;
    cmTransportAddress  localAddress;
    HTPKT               hTpkt;
    HTPKT               h245Listen;
    void                *hashEntry;
    cmTransSession      *firstSession;
    HRPOOLELEM          firstMessage; /* Current message that needs to be sent on the host */
    HRPOOLELEM          lastMessage; /* Last message we have to send on this host */
    HRPOOLELEM          incomingMessage;
    HRPOOLELEM          incomingListeningMessage; /* Incoming message on a listening connection of H.245.
                                                     We need 2 separate incoming messages for times where we
                                                     have 2 H.245 connections due to conflicts. */
    RvTimer             *connectTimer;  /* Mark interval between event "accept" and message arrival */

    /* Empty Q931 multiplex hosts list */
    cmTransHost         *prevEmptyHost; /* Previous Q931 multiplexed empty host */
    cmTransHost         *nextEmptyHost; /* Next Q931 multiplexed empty host */

	//add by daiqing 20100728
	RvBool				if460Internal;
	//end
};


struct _cmTransSession
{
    cmTransHost         *Q931Connection;
    cmTransHost         *annexEConnection;
    cmTransHost         *H245Connection;
    /*
    RvBool              isTunnelingSupported;
    RvBool              isParallelTunnelingSupported;
    RvBool              notEstablishControl;
    RvBool              closeOnNoSessions;
    RvBool              isMultiplexed;
    RvBool              firstMessageSent;
    RvBool              connectedToHost;
    RvBool              reportedH245Connect;
    RvBool              reportedQ931Connect;
    RvBool              openControlWasAsked;
    RvBool              switchToSeparateWasAsked;
    RvBool              outgoing;
    RvBool              holdTunneling;
    RvBool              forceTunneledMessage;
    RvBool              needToSendFacilityStartH245;
    RvBool              notFirstTunnelingMessage;
    RvBool              hostWasSet;
    RvBool              openH245Stage;
    RvBool              zeroTimerOnH245Connection; RV_TRUE if the zero-timer should notify about H245 being connected
    RvBool              zeroTimerOnAnnexEConnection; RV_TRUE if the zero-timer should notify about AnnexE being connected
    RvBool              zeroTimerOnQ931Connection; RV_TRUE if the zero-timer should notify about Q931 being connected
    */
    RvUint32            flags;
    RvInt               annexEListeningIndex;
    TUNNSTATE           tunnelingState;
    PARTUNNSTATE        parallelTunnelingState;
    FSSTATE             faststartState;
    cmAnnexEUsageMode   annexEUsageMode;
    cmH245Stage         h245Stage;
    RvUint8             callId[16];
    RvUint16            CRV;
    void                *hashEntry;
    cmTransSession      *nextSession;
    cmTransSession      *prevSession;
    cmTransSession      *nextPending;
    cmTransSession      *prevPending;
    HRPOOLELEM          firstMessage;
    HRPOOLELEM          lastMessage;
    RvPvtNodeId         h450Element;
    RvPvtNodeId         h450AlertElement;
    RvPvtNodeId         annexMElement;
    RvPvtNodeId         annexLElement;

    /* The following are used for zero-timer. They allow us to send a callback to
       the application as if we got it from the network. We use a single timer with
       several booleans on it to allow better memory consumption. */
    RvTimer*            zeroTimer; /* Zero timer object to use */
	//add by daiqing 20100728
	RvBool				if460Internal;
	RvBool              ifPeer460Internal;
	//end
};


typedef struct
{
    HAPPATRANS              hAppATrans;
    RvMutex                 lockSessionsList; /* Lock used to protect the list on the session elements */
                                              /* Must be a mutex since closeHostInternal is reentrant */
    RvLock                  lockHash; /* Lock used to protect access to the hash tables */
    RvLock                  lockGarbage; /* Empty multiplexed hosts garbage collection lock */

    RvLogMgr*               logMgr; /* Log manager to use */
    RvLogSource             hLog; /* Transport log source */
    RvLogSource             hTPKTCHAN; /* TPKTCHAN log source */
    RvLogSource             hTPKTWIRE; /* TPKTWIRE log source */

    HPVT                    hPvt; /* ValTree to use for ASN.1 messages */
    HPST                    synProtQ931; /* Syntax tree of Q931Message */
    HPST                    synProtH245; /* Syntax tree for MultimediaSystemControlMessage */
    HPST                    synProtH450; /* Syntax tree for h4501SupplementaryService */

    HRPOOL                  messagesRPool; /* RPOOL storing messages before they are processed and sent */

    HEMA                    hEmaSessions; /* EMA holding the sessions. A session is a call */
    HEMA                    hEmaHosts; /* EMA holding the hosts. A host is a TCP or UDP socket connection */
    HHASH                   hHashHosts;
    HHASH                   hHashSessions;
    TRANSSESSIONEVENTS      sessionEventHandlers;
    TRANSHOSTEVENTS         hostEventHandlers;
    HTPKTCTRL               tpktCntrl;
    HANNEXE                 annexECntrl;
    RvInt                   maxListeningAddresses; /* Maximum number of multiple listening addresses */
    RvInt                   numTPKTListen; /* Number of hTPKTListen addresses currently used */
    cmTransHost             **hTPKTListen; /* Listening Q931 TPKT connections */
    RvSize_t                codingBufferSize;
    cmTransSession          *pendingList;
    cmTransportAddress      localAddress;
    RvH323TimerPoolHandle   hTimers;
    RvBool                  annexESupported;
    cmTransHost             *oldestEmptyQ931MuxHost; /* The oldest Q931 multiplexing host that has no calls. This one
                                                       will be disconnected if we don't have enough hosts. */
    cmTransHost             *recentEmptyQ931MuxHost; /* Last Q931 multiplexing host that has no calls. This one
                                                        is the last one to be disconnected. */

    /* Resource values as given by the watchdog */
    RvUint32                timersResourceVal;

    RvUint32                maxUsedNumOfMessagesInRpool;
    RvUint32                curUsedNumOfMessagesInRpool;
} cmTransGlobals;


#define sizeof_hostKey 12
typedef struct
{
    RvUint32        ipNum;
    RvUint32        port;
    RvBool          type;
} hostKey;


#define sizeof_sessionKey 4
typedef struct
{
    RvInt32     CRV;
} sessionKey;




/* This list holds the positions of all the fields inside the flags field of a callElem. */
typedef enum
{
    eTrans_isTunnelingSupported = 0,
    eTrans_isParallelTunnelingSupported,
    eTrans_notEstablishControl,
    eTrans_closeOnNoSessions,
    eTrans_isMultiplexed,
    eTrans_firstMessageSent,
    eTrans_connectedToHost,
    eTrans_reportedH245Connect,
    eTrans_reportedQ931Connect,
    eTrans_openControlWasAsked,
    eTrans_switchToSeparateWasAsked,
    eTrans_outgoing,
    eTrans_holdTunneling,
    eTrans_forceTunneledMessage,
    eTrans_needToSendFacilityStartH245,
    eTrans_notFirstTunnelingMessage,
    eTrans_hostWasSet,
    eTrans_openH245Stage,
    eTrans_zeroTimerOnH245Connection,
    eTrans_zeroTimerOnAnnexEConnection,
    eTrans_zeroTimerOnQ931Connection,
    eTrans_callTerminating,

    /* Last enumeration value */
    e_lastSessionFlag /* This should always be the last flag - it is used for validity checks */
} sessionElemEnumPosition;


#define trans_field_mask(_field) (RvUint32)(1<<((RvUint32)eTrans_##_field))

/* Set and get macros for fields inside callInfo.flags */
#define m_sessionset(_session,_field,_value) \
    ((_session)->flags = ( ((_session)->flags  & ~(trans_field_mask(_field))) | (trans_field_mask(_field) & ((RvUint32)(_value) << (eTrans_##_field))) ))
#define m_sessionget(_session,_field) \
    (((_session)->flags  & (trans_field_mask(_field))) >> (eTrans_##_field))


/* Make sure we don't have too many flags */
#if (e_lastSessionFlag > 32)
#error Too many flags for the element _cmTransSession.flags
#endif



#ifdef __cplusplus
}
#endif


#endif  /* _TRANSPORTINT_H */
