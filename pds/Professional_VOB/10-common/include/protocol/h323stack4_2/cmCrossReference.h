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

#ifndef _CM_CROSS_REFERENCE_H
#define _CM_CROSS_REFERENCE_H

#include "ema.h"
#include "rvwatchdog.h"
#include "cat.h"
#include "cmControl.h"
#include "cm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RAS module handle */
RV_DECLARE_HANDLE(HRASMGR);

/* Automatic RAS module handle */
RV_DECLARE_HANDLE(HAUTORASMGR);

/* Automatic RAS call handle */
RV_DECLARE_HANDLE(HAUTORASCALL);


RV_DECLARE_HANDLE(HQ931);




#define RAS_OUT_TX    (1)
#define RAS_IN_TX     (2)
/* #define CALL          (3) - defined in cmH245.h as H225CALL */
/* #define CHAN          (4) - defined in cmH245.h */




/************************************************************************
 *
 *                       Stack instance related
 *
 ************************************************************************/


/************************************************************************
 * cmiInitCalls
 * purpose: Constructs the EMA for the call elements
 * input  : hApp     - Stack's application handle
 *          maxCalls - number of calls to allocate
 * output : none
 * return : EMA-elements array pointer on success
 *          NULL on failure
 ************************************************************************/
HEMA cmiInitCalls(HAPP hApp,int maxCalls);

/************************************************************************
 * cmiEndCalls
 * purpose: Destructs the EMA of the call elements
 * input  : calls    - the EMA pointer created by cmiInitCalls()
 * output : none
 * return : none
 ************************************************************************/
void cmiEndCalls(HEMA calls);


/************************************************************************
 * cmiInitChannels
 * purpose: Constructs the EMA for the channel elements
 * input  : hApp        - Stack's application handle
 *          maxCalls    - number of calls to allocated
 *          maxChannels - number of channels to allocate per call
 * output : none
 * return : EMA-elements array pointer on success
 *          NULL on failure
 ************************************************************************/
HEMA cmiInitChannels(HAPP hApp,int maxCalls,int maxChannels);

/************************************************************************
 * cmiEndChannels
 * purpose: Destructs the EMA of the channel elements
 * input  : channels    - the EMA pointer created by cmiInitCalls()
 * output : none
 * return : none
 ************************************************************************/
void cmiEndChannels(HEMA channels);

/************************************************************************
 * cmiGetWatchdogHandle
 * purpose: Get the watchdog object from the stack's instance
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Watchdog module pointer on success
 *          NULL on failure
 ************************************************************************/
RVAPI RvWatchdog* RVCALLCONV cmiGetWatchdogHandle(IN HAPP hApp);

/************************************************************************
 * cmiGetWatchdogLogSource
 * purpose: Get the log source for watchdog printouts
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Pointer to watchdog log source on success
 *          NULL on failure
 ************************************************************************/
RvLogSource* cmiGetWatchdogLogSource(IN HAPP hApp);


/************************************************************************
 * cmiGetRasHandle
 * purpose: Get the RAS module object from the stack's instance
 * input  : hApp    - Stack's application handle
 * output : none
 * return : RAS module handle on success
 *          NULL on failure
 ************************************************************************/
HRASMGR cmiGetRasHandle(IN HAPP hApp);


/************************************************************************
 * cmiGetAutoRasHandle
 * purpose: Get the Automatic RAS module object from the stack's instance
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Automatic RAS module handle on success
 *          NULL on failure
 ************************************************************************/
HAUTORASMGR cmiGetAutoRasHandle(IN HAPP hApp);


/************************************************************************
 * cmiGetRasHooks
 * purpose: Get the hook functions set by the application of the CM
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Pointer to the hook functions set by the application
 ************************************************************************/
CMPROTOCOLEVENT cmiGetRasHooks(IN HAPP hApp);







/************************************************************************
 *
 *                              Call related
 *
 ************************************************************************/


/************************************************************************
 * cmiGetQ931
 * purpose: Get the Q.931 call handle from the stack's call handle
 * input  : hsCall  - Stack's call handle
 * output : none
 * return : Q.931 call handle on success
 ************************************************************************/
HQ931 cmiGetQ931(IN HCALL hsCall);

/************************************************************************
 * cmiGetByQ931
 * purpose: Get the stack's call handle from the Q.931 call handle
 * input  : qCall   - Q.931's call handle
 * output : none
 * return : Stack call handle on success
 ************************************************************************/
HCALL cmiGetByQ931(IN HQ931 qCall);

/************************************************************************
 * cmiGetChannelListForCtrl
 * purpose: Get the call's channel list from the control handle
 * input  : ctrl   - Call control handle
 * output : none
 * return : Handle of the first channel in the list, NULL if none
 ************************************************************************/
HCHAN cmiGetChannelListForCtrl(IN HCONTROL ctrl);

/************************************************************************
 * cmiGetChannelListForCall
 * purpose: Get the call's channel list from the stack call handle
 * input  : call   - Stack call handle
 * output : none
 * return : Handle of the first channel in the list, NULL if none
 ************************************************************************/
HCHAN cmiGetChannelListForCall(IN HCALL call);

/************************************************************************
 * cmiSetChannelListForCtrl
 * purpose: Set the call's channel list for the control handle
 * input  : ctrl   - Call control handle
 *          ch     - Handle of the first channel in the list
 * output : none
 * return : none
 ************************************************************************/
void cmiSetChannelListForCtrl(IN HCONTROL ctrl, IN HCHAN ch);

/************************************************************************
 * cmiSetChannelListForCall
 * purpose: Set the call's channel list for the stack call handle
 * input  : call   - Stack call handle
 *          ch     - Handle of the first channel in the list
 * output : none
 * return : none
 ************************************************************************/
void cmiSetChannelListForCall(IN HCALL call, IN HCHAN ch);


/************************************************************************
 * cmiGetCatForCall
 * purpose: Get the CAT call handle from the stack's call handle
 * input  : hsCall  - Stack's call handle
 * output : none
 * return : Cat call handle on success
 *          NULL on failure
 ************************************************************************/
RVHCATCALL cmiGetCatForCall(IN HCALL hsCall);


/************************************************************************
 * cmiGetAutoRas
 * purpose: Get the Automatic RAS call object from the stack's call handle
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Automatic RAS call handle on success
 *          NULL on failure
 ************************************************************************/
HAUTORASCALL cmiGetAutoRas(IN HCALL hsCall);


/************************************************************************
 * cmiGetByAutoRas
 * purpose: Get the stack's call handle from the Automatic RAS call object
 * input  : hApp    - Stack's application handle
 * output : none
 * return : Stack's call handle
 *          NULL on failure
 ************************************************************************/
HCALL cmiGetByAutoRas(IN HAUTORASCALL aras);


#ifdef __cplusplus
}
#endif

#endif  /* _CM_CROSS_REFERENCE_H */

