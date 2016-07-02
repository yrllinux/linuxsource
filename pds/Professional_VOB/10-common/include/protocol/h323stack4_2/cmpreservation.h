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


#ifndef _CMPRESERVATION_
#define _CMPRESERVATION_

#include "rvcommon.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    RvH323HaError = -3,
    RvH323HaImpossible = -2,
    RvH323HaNotEnoughBuffer = -1,
    RvH323HaOK = 0,
    RvH323HaTimerWarning = 1
} RvH323HaResult;

RV_DECLARE_HANDLE(RvH323HaHOST);

typedef RvStatus (*RvH323HaActivateCallCB) (
    IN  HAPP               hApp,
    IN  HCALL              hsCall,
    IN  HAPPCALL           haCall,
    IN  cmCallState_e      state,
    IN  cmCallStateMode_e  stateMode,
    OUT RvH323HaHOST       *annexEHost,
    OUT RvH323HaHOST       *q931Host,
    OUT RvH323HaHOST       *h245Host);

typedef RvStatus (*RvH323HaActivateChannelCB) (
    IN  HAPP        hApp,
    IN  HCALL       hsCall,
    IN  HAPPCALL    haCall,
    IN  HCHAN       hsChan,
    OUT HAPPCHAN    *haChan);


/************************************************************************************
 * RvH323HaGetCallBuffer
 * purpose: Turn a given call, with its property, session, and channels into a
 *          buffer.
 * input  : hsCall  - The call to be turned into a buffer
 *          buffer  - The buffer to use
 *          length  - The length of buffer
 * output : length  - The length of buffer needed/used
 * return : RvH323HaError           - some error has occured, m.p. with locking the call
 *          RvH323HaImpossible      - call is in a delicate state and cannot be preserved
 *          RvH323HaNotEnoughBuffer - buffer is too short to set call it (see length)
 *          RvH323HaOK              - call was preserved
 *          RvH323HaTimerWarning    - call was preserved, but existing timers were not
 ************************************************************************************/
RVAPI RvH323HaResult RVCALLCONV RvH323HaGetCallBuffer(
    IN    HCALL      hsCall,
    OUT   RvUint8  * buffer,
    INOUT RvSize_t * length);


/************************************************************************************
 * RvH323HaAddCall
 * purpose: Turn a given call buffer produced by RvH323HaGetCallBuffer to a call
 *          object, session and channels.
 * input  : hApp    - The application instance the call is to be added to
 *          haCall  - Application handle to the call
 *          buffer  - The call buffer produced by RvH323HaGetCallBuffer
 *          length  - The length of buffer
 * output : none
 * return : negative on error
 ************************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323HaAddCall(
    IN HAPP      hApp,
    IN HAPPCALL  haCall,
    IN RvUint8 * buffer,
    IN RvSize_t  length);


/************************************************************************************
 * RvH323HaDelCall
 * purpose: Delete a previously inserted call.
 * input  : hApp    - The application instance the call is to be added to
 *          haCall  - Application handle to the call
 * output : none
 * return : negative on error
 ************************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323HaDelCall(
    IN HAPP      hApp,
    IN HAPPCALL  haCall);

/************************************************************************************
 * RvH323HaActivateCalls
 * purpose: Activate all the prviously inseted calls.
 * input  : hApp           - The application instance the call has been added to
 *          activateCallCB - Callback to call for each call
 *          activateChanCB - Callback to call for each channel
 * output : none
 * return : negative on error
************************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323HaActivateCalls(
    IN HAPP                       hApp,
    IN RvH323HaActivateCallCB     activateCallCB,
    IN RvH323HaActivateChannelCB  activateChanCB);

/************************************************************************************
 * RvH323HaGetAnnexEHost
 * purpose: Get the Application's Annex E Host handle, for use in another call.
 * input  : hApp          - The application instance the call has been added to
 *          hsCall        - The call handle for which the host is needed
 *          remoteAddress - The remote address to send to
 * output : hHost         - Handle to the Annex E host
 * return : negative on error
 ************************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323HaGetAnnexEHost(
    IN  HAPP                hApp,
    IN  HCALL               hsCall,
    IN  cmTransportAddress  *remoteAddress,
    OUT RvH323HaHOST        *hHost);

/************************************************************************************
 * RvH323HaGetAnnexEHost
 * purpose: Get a Multiplexed Host handle, for use in another call.
 * input  : hApp                 - The application instance the call has been added to
 *          hsCall               - The call handle for which the host is needed
 *          hsSameConnectionCall - A call whose connection we would like to use (optional)
 *          remoteAddress        - The remote address the host should be connected to
 * output : hHost                - Handle to the Multiplexed host
 * return : negative on error
 ************************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323HaGetMuxHost(
    IN  HAPP                hApp,
    IN  HCALL               hsCall,
    IN  HCALL               hsSameConnectionCall,
    IN  cmTransportAddress  *remoteAddress,
    OUT RvH323HaHOST        *hHost);


RVAPI int RVCALLCONV sendTpktForPinhole(IN HCALL hsCall, IN cmTpktChanHandleType type);//add by yj for pinhole [20120830]

#ifdef __cplusplus
}
#endif
#endif /*_CMPRESERVATION_*/

