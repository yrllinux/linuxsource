#ifdef __cplusplus
extern "C" {
#endif



/*

 NOTICE:
 This document contains information that is proprietary to RADVISION LTD..
 No part of this publication may be reproduced in any form whatsoever without
 written prior approval by RADVISION LTD..

  RADVISION LTD. reserves the right to revise this publication and make changes
  without obligation to notify any person of such revisions or changes.

    */
#ifndef _CMICTRL_
#define _CMICTRL_

#include "cm.h"
#include "cmcrossreference.h"

typedef enum
{
    ctrlNotInitialized,
    ctrlInitialized,
    ctrlConnected,
    ctrlConference,
    ctrlEndSession
} controlState;


/************************************************************************
 * sendMessageH245
 * purpose: Send H245 message for a given call.
 * input  : hsCall  - Stack handle for the H245 call control
 *          message - root node ID of the message to send
 * output : none
 * return : todo: What's the return value
 ************************************************************************/
int sendMessageH245(IN HCONTROL ctrl, IN int message);

RVAPI int RVCALLCONV sendTpktForPinhole(IN HCALL hsCall, IN cmTpktChanHandleType type);


void h245ProcessIncomingMessage(HCONTROL ctrl, int message);

void getGoodAddressForCtrl(HCONTROL ctrl, cmTransportAddress* ta);
void getGoodAddressForCall(HCALL hCall, cmTransportAddress* ta);

BOOL addChannelForCtrl(HCONTROL ctrl,HCHAN ch);
BOOL deleteChannelForCtrl(HCONTROL ctrl,HCHAN ch);



int getQosParameters(IN HPVT hVal,
                     IN int qosElemId,
                     INOUT  cmQosCapability * cmQOSCapability);


int buildQosCapability(IN HPVT hVal,
                       IN int qosId,
                       IN cmRSVPParameters *rsvpParam,
                       IN cmATMParameters * atmParam,
                       IN cmNonStandardParam *nonStandard);


void initControl(HCONTROL ctrl, int lcnOut);
void startControl(HCONTROL ctrl);

/************************************************************************
 * stopControl
 * purpose: Stop the H245 Control connection for a call.
 * input  : ctrl    - Control object
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
void stopControl(IN HCONTROL ctrl);

/************************************************************************
 * closeChannels
 * purpose: Close any channels of a given control object.
 *          This function only notifies the application about closing the
 *          channels due to disconnection.
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void closeChannels(IN HCONTROL ctrl);

void cmH245Start(HAPP hApp);
void cmH245Stop(HAPP hApp);

controlState controlGetState(HCONTROL ctrl);

#endif
#ifdef __cplusplus
}
#endif
