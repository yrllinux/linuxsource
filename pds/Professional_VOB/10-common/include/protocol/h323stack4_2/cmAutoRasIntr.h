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

#ifndef _CM_AUTORAS_INTR_H
#define _CM_AUTORAS_INTR_H

#include "cmintr.h"
#include "cmAutoRas.h"
#include "cmutils.h"
#include "rvh323timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALT_GK_ARRAY_SIZE 20

/************************************************************************
 * autorasRegState
 * Registration state of the automatic RAS
 * autorasRegNotTried   - Haven't tried to register yet
 * autorasRegTrying     - Currently trying t register
 * autorasRegistered    - Registered to a gatekeeper
 * autorasRegFailed     - Failed to register to a gatekeeper
 ************************************************************************/
typedef enum
{
    autorasRegNotTried,
    autorasRegTrying,
    autorasRegistered,
    autorasRegFailed
} autorasRegState;


/************************************************************************
 * autorasCallState
 * Call state of the automatic RAS
 * autorasCallIdle          - Call not present
 * autorasCallConnecting    - ARQ was sent - no response received yet
 * autorasCallConnected     - Call is connected
 * autorasCallDisconnecting - Disconnecting the call at current moment
 ************************************************************************/
typedef enum
{
    autorasCallIdle,
    autorasCallConnecting,
    autorasCallConnected,
    autorasCallDisconnecting
} autorasCallState;


typedef struct
{
    RvPvtNodeId nodeID;
    RvUint8     prio;
} AltGKwithPrio;


typedef struct
{
    HAPP                    hApp; /* Application's stack handle */
    RvLogMgr*               logMgr; /* Log manager to use */
    HPVT                    hVal; /* Value tree handle */
    RvPvtNodeId             confNode; /* RAS configuration node */
    RvH323TimerPoolHandle   hTimers; /* Timers pool handle */

    cmRegState              state; /* State of the endpoint (registered, discovered or idle) */
    autorasRegState         internalState; /* Internal registration state of the endpoint */
    RvBool                  discoveryRequired; /* Indicates if the endpoint has to send a GRQ, after receiving
                                                  a RRJ with reason discoveryRequired */
    RvBool                  discoveryComplete; /* Indicates if the endpoint got a GCF for this registration */
    int                     regTries; /* Number of registration tries that have occured */
    RvTimer *               regTimer; /* Registration timer. Used mainly for lightweight RRQs */
    HRAS                    registrationTx; /* Registration transaction handle. Used for RRQ, GRQ */

    AltGKwithPrio           permAltGKs[ALT_GK_ARRAY_SIZE]; /* Node Id for Permanent Alternate Gatekeepers */
    AltGKwithPrio           tempAltGKs[ALT_GK_ARRAY_SIZE]; /* Node Id for Temporary Alternate Gatekeepers */
    int                     altGKInd; /* Index for Alt GKs array */
    RvBool                  isTempRdrn; /* State of redirection */

    cmiEvAutoRASEventT      event; /* Event handler called on automatic RAS events */
} autorasEndpoint;


/************************************************************************
 * autorasCall struct
 * Automatic RAS information related to a call.
 * This is wrapped into the same EMA that is used for the callElem of the
 ************************************************************************/
typedef struct
{
    HRAS                tx; /* Pending RAS transaction on call */
    HRAS                irrTx; /* Unsolicited IRR transactions */
    RvTimer *           timer; /* Timer handle used for RAS transactions on this call.
                                  1. For waiting for the endpoint to register on a GK
                                  2. For unsolicited IRR messages on this call */
    autorasCallState    callState; /* Indicates the current state of the RAS call */

    int                 altGKInd; /* Index for Alt GKs array */
    RvBool              isTempRdrn; /* State of redirection */
    RvBool              wasRejected; /* RV_TRUE iff got some rejection from any GK */
} autorasCall;


/************************************************************************
 * setRasAlternateGatekeeper
 * purpose: Called after a confirmation message, sets the current GK and
 *          updates the list of AltGKs.
 * input  : autoras       - Automatic RAS EndPoint instance
 *        : autorasC      - Automatic RAS Call instance (may be NULL)
 *          hsRas         - transaction handle
 *          messageNodeId - node of the AltGKs
 *          bUpdate       - RV_TRUE if we should update the RAS module
 * output : none
 * return : none
 ************************************************************************/
void setRasAlternateGatekeeper(
    IN autorasEndpoint*    autoras,
    IN autorasCall*        autorasC,
    IN HRAS                hsRas,
    IN RvPvtNodeId         messageNodeId,
    IN RvBool              bUpdate);


#ifdef __cplusplus
}
#endif

#endif  /* _CM_AUTORAS_INTR_H */
