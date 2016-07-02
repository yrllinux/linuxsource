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

#ifndef _AnnexE_H_
#define _AnnexE_H_

#include "rvinternal.h"
#include "rvh323connection.h"
#include "rvh323timer.h"
#include "rpool.h"

#ifdef __cplusplus
extern "C" {
#endif /* _cplusplus */


RV_DECLARE_HANDLE( HANNEXE );       /* Annex E instance handle                      */
RV_DECLARE_HANDLE( HAPPANNEXE );    /* Application handle of the Annex E instance   */

typedef enum {
    annexEStatusNormal,
    annexEStatusMemoryProblem,
    annexEStatusResourceProblem,
    annexEStatusSocketProblem,
    annexEStatusWouldBlock,
    annexEStatusAddressAlreadyInUse,
    annexEStatusMustBeStopped,
    annexEStatusMustBeStarted,
    annexEStatusBadParameter
} annexEStatus;

/* Event Handler Function Prototypes */
typedef annexEStatus (RVCALLCONV* annexEEvRestartT)(
    IN  HANNEXE                 hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  cmTransportAddress*     pSourceAddress
);

typedef annexEStatus (RVCALLCONV* annexEEvNewMessageT)(
    IN  HANNEXE                 hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  RvInt                   localAddressIndex,
    IN  cmTransportAddress*     ramoteAddress,
    IN  void*                   pMessage,
    IN  int                     nSize
);

typedef annexEStatus (RVCALLCONV* annexEEvConnectionBrokenT)(
    IN  HANNEXE                 hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  cmTransportAddress*     ta,
    IN  int                     localAddrIndex
);

typedef annexEStatus (RVCALLCONV* annexEEvUseOtherAddressT)(
    IN  HANNEXE                 hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  cmTransportAddress*     ta,
    IN  cmTransportAddress*     pNewAddress,
    IN  int                     localAddrIndex
);

typedef annexEStatus (RVCALLCONV* annexEEvNotSupportedT)(
    IN  HANNEXE                 hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  cmTransportAddress*     ta,
    IN  int                     localAddrIndex
);

typedef annexEStatus (RVCALLCONV* annexEEvWriteableT)(
    IN  HANNEXE                 hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  cmTransportAddress*     ta,
    IN  int                     localAddrIndex
);

/* Callback Structure Definition */
typedef struct {
    annexEEvRestartT            AnnexEEvRestart;
    annexEEvNewMessageT         AnnexEEvNewMessage;
    annexEEvConnectionBrokenT   AnnexEEvConnectionBroken;
    annexEEvUseOtherAddressT    AnnexEEvUseOtherAddress;
    annexEEvNotSupportedT       AnnexEEvNotSupported;
    annexEEvWriteableT          AnnexEEvWriteable;
} annexEEvents;

/* package init & done functions
****************************************************************************
***************************************************************************/

annexEStatus annexEInit(
    OUT HANNEXE*                hAnnexE,
    IN  HAPPANNEXE              hAppAnnexE,
    IN  int                     nMaxNodes,
    IN  int                     nMaxSupportedMessageSize,
    IN  int                     nMaxStoredPackets,
    IN  RvInt                   nMaxListeningAddresses,
    IN  RvH323TimerPoolHandle   timersPool,
    IN  RvSelectEngine*         selEngine,
    IN  RvPortRange*            portRange,
    IN  RvLogMgr*               logMgr);

annexEStatus annexEEnd(
    IN  HANNEXE                 hAnnexE
);

/* event handler & protocol parameter functions
****************************************************************************
***************************************************************************/

annexEStatus annexESetProtocolParams(
    IN  HANNEXE                 hAnnexE,
    IN  int                     t_R1,
    IN  int                     t_R2,
    IN  int                     n_R1,
    IN  int                     t_IMA1,
    IN  int                     n_IMA1,
    IN  int                     t_DT
);

annexEStatus annexEAddLocalAddress(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     pLocalAddress
);

annexEStatus annexEGetLocalAddress(
    IN  HANNEXE                 hAnnexE,
    IN  RvInt                   index,
    OUT cmTransportAddress*     pLocalAddress
);

annexEStatus annexEGetLocalAddressIndex(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     pLocalAddress,
    OUT RvUint8*                index);

annexEStatus annexESetEventHandler(
    IN  HANNEXE                 hAnnexE,
    IN  annexEEvents*           pAnnexEEventHandler
);

HRPOOLELEM annexESwapLastMessageToSend(
    IN  HANNEXE                 hAnnexE,
    IN  HRPOOLELEM              message
);

HRPOOLELEM annexEGetMessageToSend(
    IN  HANNEXE                 hAnnexE
);

/* package start & stop functions
****************************************************************************
***************************************************************************/

annexEStatus annexEStart(
    IN  HANNEXE                 hAnnexE
);

annexEStatus annexEStop(
    IN  HANNEXE                 hAnnexE
);

/* package state functions
****************************************************************************
***************************************************************************/

annexEStatus annexEGetHAStateSize(
    IN  HANNEXE                 hAnnexE,
    OUT RvUint32*               pnSize
);

annexEStatus annexEGetHAState(
    IN  HANNEXE                 hAnnexE,
    OUT void*                   pState,
    IN  int                     nSize
);

annexEStatus annexESetHAState(
    IN  HANNEXE                 hAnnexE,
    IN  void*                   pState,
    IN  int                     nSize
);

/* send 'Restart' message
****************************************************************************
***************************************************************************/

annexEStatus annexERestart(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     pDestAddress
);

/* AnnexE API functions
****************************************************************************
***************************************************************************/


annexEStatus annexESendMessage(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     ta,
    IN  cmTransportAddress*     locAddr,
    IN  RvUint16                CRV,
    IN  void*                   message,
    IN  int                     size,
    IN  RvBool                  fAckHint,
    IN  RvBool                  fReplyHint,
    IN  RvBool                  fSendHint
);

annexEStatus annexECloseNode(
    IN  HANNEXE                 hAnnexE,
    IN  cmTransportAddress*     ta,
    IN  cmTransportAddress*     locAddr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AnnexE_H_ */
