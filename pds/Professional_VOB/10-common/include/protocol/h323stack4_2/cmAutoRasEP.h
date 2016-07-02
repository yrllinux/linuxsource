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

/************************************************************************
 * cmAutoRasEP
 * -----------
 * This file provides the endpoint part of the automatic RAS module.
 * This includes the following features:
 *
 * - Automatic discovery and registration to a gatekeeper
 * - Unregistration of a gatekeeper
 * - Lightweight RRQs (timeToLive feature)
 * - Sending NSM and RAI messages
 * - Automatic responses to IRQ messages
 * - Enabling manual RAS to work with automatic RAS for endpoint related
 *   transactions
 ************************************************************************/


#ifndef _CM_AUTORAS_EP_H
#define _CM_AUTORAS_EP_H

#include "cmintr.h"
#include "cmAutoRasIntr.h"
#include "cmAutoRasAltGK.h"

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * cmiAutoRASEPStart
 * purpose: Start the endpoint part of the automatic RAS.
 *          This function sets internal callbacks with the RAS module and
 *          initializes some autoRAS related variables.
 * input  : hAutoRas - Automatic RAS instance
 * output : none
 * return : none
 ************************************************************************/
void cmiAutoRASEPStart(IN HAUTORASMGR   hAutoRas);

/************************************************************************
 * cmiAutoRASGetEndpointID
 * purpose: Retrieves the EndpointID stored in the ras
 *
 * input  : hApp    - Application's stack handle
 *          eId     - pointer to the buffer for endpoint ID
 *                    buffer should be big enough for longest EID
 *                    possible (256 byte)
 * output : none
 * return : The length of EID in bytes on success
 *          Negative value on failure
 ************************************************************************/
int cmiAutoRASGetEndpointID(
    IN  HAPP    hApp,
    IN  void*   eId);

/************************************************************************
 * autoRasChangeState
 * purpose: Notify the application about the state of the registration
 * input  : autoras     - Automatic RAS instance
 *          regEvent    - Registration event being notified
 *                        can be a negative value if event is not caused
 *                        by any message but by a timeout
 *          message     - Message that caused the event, or node ID of
 *                        the new permanent alternate GK in such an event.
 *                        Negative value if not applicable
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
int autoRasChangeState(
    IN  autorasEndpoint*    autoRas,
    IN  cmRegEvent          regEvent,
    IN  int                 message);


/************************************************************************
 * cmiAutoRASGetRegState
 * purpose: returns the registration state
 * input  : hApp - Application's handle of the stack
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns the registration state.
 ************************************************************************/
cmRegState cmiAutoRASGetRegState(IN HAPP hApp);


#ifdef __cplusplus
}
#endif

#endif  /* _CM_AUTORAS_EP_H */
