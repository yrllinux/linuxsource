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

#ifndef _CMQ931_
#define _CMQ931_

#include "cmCrossReference.h"
#include "cmQ931.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    cs_Null                     =0,
    cs_Call_initiated           =1,
    cs_Overlap_sending          =2,
    cs_Outgoing_call_proceeding =3,
    cs_Call_delivered           =4,
    cs_Call_present             =6,
    cs_Call_received            =7,
    cs_Connect_request          =8,
    cs_Incoming_call_proceeding =9,
    cs_Active                   =10,
    cs_Disconnect_request       =11,
    cs_Disconnect_indication    =12,
    cs_Suspend_request          =15,
    cs_Resume_request           =17,
    cs_Release_request          =19,
    cs_Overlap_receiving        =25
} callStateE;


int q931CallCreate(HQ931 call, int t301, int t302, int t303, int t304, int t310, int t322);
int q931CallClose(HQ931 call);
callStateE q931GetCallState(HQ931 call);

RvStatus q931CallDial(IN HQ931 call, IN RvPvtNodeId message);
int q931CallInfo(HQ931 call, int message);
int q931CallMoreInfo(HQ931 call,int message);

/************************************************************************
 * q931CallCallProceeding
 * purpose: Send Q931 CallProceeding message on a call
 * input  : call    - Stack handle for the Q931 call
 *          message - CallProceeding message to send
 *                    if set to -1, then create the message from the call's
 *                    property database
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallCallProceeding(IN HQ931 call, IN int message);

/************************************************************************
 * q931CallProgress
 * purpose: Send Q931 Progress message on a call
 * input  : call    - Stack handle for the Q931 call
 *          message - Progress message to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallProgress(IN HQ931 call, IN int message);

/************************************************************************
 * q931CallNotify
 * purpose: Send Q931 Notify message on a call
 * input  : call    - Stack handle for the Q931 call
 *          message - Notify message to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallNotify(IN HQ931 call, IN int message);

/************************************************************************
 * q931CallAccept
 * purpose: Send Q931 Alerting message on a call
 * input  : call    - Stack handle for the Q931 call to accept
 *          message - Alerting message to send
 *                    if set to -1, then create the message from the call's
 *                    property database
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallAccept(IN HQ931 call, IN int message);

/************************************************************************
 * q931CallAnswer
 * purpose: Send Q931 Connect message on a call
 * input  : call    - Stack handle for the Q931 call to connect
 *          message - Connect message to send
 *                    if set to -1, then create the message from the call's
 *                    property database
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallAnswer(IN HQ931 call, IN int message);

/************************************************************************
 * q931CallDrop
 * purpose: Drops a Q931 connection of a call
 * input  : call        - Stack handle for the Q931 call
 *          message     - Release message to send
 *                        If -1, then message will be created
 *          causeValue  - Release cause
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallDrop(IN HQ931 call, IN int message, IN int causeValue);

/************************************************************************
 * q931CallStatusEnquiry
 * purpose: Send a Q931 STATUS ENQUIRY message
 * input  : call        - Stack handle for the Q931 call
 *          message     - StatusEnquiry message to send
 *                        If -1, then message will be created
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallStatusEnquiry(IN HQ931 call, IN int message);

int q931CallFacility(HQ931 call,int message);

/************************************************************************
 * q931CallAddressComplete
 * purpose: Indication on an incoming call that the Overlap Sending
 *          procedure has finished and we have a full address.
 * input  : call    - Stack handle for the Q931 call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931CallAddressComplete(IN HQ931 call);

/************************************************************************
 * q931DecodingFailure
 * purpose: Handle incoming Q931 messages that can't be decoded
 *          This automatically sends back a STATUS message
 * input  : callE       - Stack handle for the Q931 call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931DecodingFailure(IN HQ931 call);

/************************************************************************
 * q931ProcessMessage
 * purpose: Handle incoming Q931 messages
 * input  : call    - Stack handle for the Q931 call
 *          message - Node Id of the massage to handle
 *                    This function doesn't delete the message's node Id,
 *                    but modifies its value
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int q931ProcessMessage(IN HQ931 call, IN int message);

int q931SimulateSetup(HQ931 call);


#ifdef __cplusplus
}
#endif

#endif  /* _CMQ931_ */
