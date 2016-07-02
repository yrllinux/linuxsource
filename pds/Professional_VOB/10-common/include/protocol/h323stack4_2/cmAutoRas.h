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

#ifndef _CM_AUTORAS_H
#define _CM_AUTORAS_H

#include "rvh323timer.h"
#include "cm.h"
#include "cmCrossReference.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * cmiAutoRasEvent enum
 * Events that are notified to the CM by automatic RAS.
 ************************************************************************/
typedef enum
{
    cmiAutoRasEvGotRCF, /* Indication about incoming RCF */
    cmiAutoRasEvCallDropForced, /* Indication about incoming DRQ on call */
    cmiAutoRasEvCallDropped, /* Indication about incoming DCF, DRJ on call */
    cmiAutoRasEvRateChanged, /* Indication about incoming BRQ or BCF on call */
    cmiAutoRasEvGotACF, /* Indication about incoming ACF */
    cmiAutoRasEvCantRegister, /* Inability to register while trying to make a call */
    cmiAutoRasEvFailedOnARQ, /* Couldn't create an ARQ transaction */
    cmiAutoRasEvCallRejected, /* Indication about incoming ARJ */
    cmiAutoRasEvTimedout, /* Indication about timeout on transaction */
    cmiAutoRasEvPrepareIRR /* Ask the CM to fill in an unsolicited IRR message information */
} cmiAutoRasEvent;


/************************************************************************
 * cmiEvAutoRASEventT
 * purpose: Automatic RAS event handler for notifying the CM about specific
 *          events in RAS.
 * input  : hsCall  - Call handle of the transaction (NULL if not applicable)
 *          hsRas   - RAS transaction handle
 *          event   - Event that occured
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
typedef int (RVCALLCONV *cmiEvAutoRASEventT) (
    IN HCALL            hsCall,
    IN HRAS             hsRas,
    IN cmiAutoRasEvent  event);






/************************************************************************
 * cmiAutoRASInit
 * purpose: Initialize the RAS module and all the network related with
 *          RAS.
 * input  : hApp        - Application's stack handle
 *          logMgr      - Log manager used by the stack
 *          hTimers     - Timers pool used by the stack
 *          hVal        - Value tree handle
 *          rasConfNode - RAS configuration tree
 *          evFunc      - Event handler to set
 *          logMgr      - Log manager to use
 * output : none
 * return : Automatic RAS handle created on success
 *          NULL on failure
 ************************************************************************/
HAUTORASMGR cmiAutoRASInit(
    IN HAPP                     hApp,
    IN RvH323TimerPoolHandle    hTimers,
    IN HPVT                     hVal,
    IN int                      rasConfNode,
    IN cmiEvAutoRASEventT       evFunc,
    IN RvLogMgr*                logMgr);


/************************************************************************
 * cmiAutoRASStart
 * purpose: Start the RAS module and all the network related with
 *          RAS.
 * input  : hApp    - Application's stack handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASStart(IN HAPP hApp);


/************************************************************************
 * cmiAutoRASEnd
 * purpose: End the automatic RAS module
 * input  : hApp        - Application's stack handle
 * output : none
 * return : none
 ************************************************************************/
void cmiAutoRASEnd(IN HAPP hApp);


/************************************************************************
 * cmiAutoRAS
 * purpose: Returns the RAS mode
 *
 * input  : hApp    - Application's stack handle
 * output : none
 * return : RV_TRUE for automatic RAS
 *          RV_FALSE for manual RAS
 ************************************************************************/
RvBool cmiAutoRAS(IN HAPP hApp);



#ifdef __cplusplus
}
#endif

#endif  /* _CM_AUTORAS_H */
