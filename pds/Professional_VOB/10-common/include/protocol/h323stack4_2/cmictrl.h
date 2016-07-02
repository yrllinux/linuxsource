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

#ifndef _CMI_CTRL_
#define _CMI_CTRL_

#include "cmControl.h"
#include "cmctrl.h"

#ifdef __cplusplus
extern "C" {
#endif




void clearControl(HCONTROL ctrl, RvBool callInitiator, int controlOffset);
void initControl(HCONTROL ctrl, RvH245ChannelParamsType eParamsType, RvInt32 terminalType);
void startControl(HCONTROL ctrl);

/******************************************************************************
 * endControl
 * ----------------------------------------------------------------------------
 * General: Clear up any resources used by the control object of a given call,
 *          without sending anything on to the network.
 *          This is used by the stopControl() function and when we want to
 *          switch from a dummy control to a real one in the GK.
 *
 * Return Value: None.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  ctrl     - Control object to use.
 * Output: None.
 *****************************************************************************/
void endControl(IN HCONTROL ctrl);

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



/************************************************************************
 * sendMessageH245
 * purpose: Send H245 message for a given call.
 * input  : hsCall  - Stack handle for the H245 call control
 *          message - root node ID of the message to send.
 *                    This function won't delete this node ID.
 * output : none
 * return : RV_OK on success
 *          Other on failure
 ************************************************************************/
RvStatus sendMessageH245(
    IN  HCONTROL     ctrl,
    IN  RvPvtNodeId  message,
    OUT RvBool*      sentOnDummy);


/************************************************************************
 * sendMessageH245Chan
 * purpose: Send H245 message for a given channel.
 * input  : ctrl    - Stack handle for the H245 call control
 *          hsChan  - Channel handle for the H245 channel
 *          message - root node ID of the message to send
 * output : none
 * return : RV_OK on success
 *          Other on failure
 ************************************************************************/
RvStatus sendMessageH245Chan(
    IN  HCONTROL     ctrl,
    IN  HCHAN        hsChan,
    IN  RvPvtNodeId  message,
    OUT RvBool*      sentOnDummy);


void h245ProcessIncomingMessage(HCONTROL ctrl, int message);


#if (RV_H245_SUPPORT_H225_PARAMS == 1)
void getGoodAddressForCtrl(HCONTROL ctrl, cmTransportAddress* ta);
void getGoodAddressForCall(HCALL hCall, cmTransportAddress* ta);
#endif /* (RV_H245_SUPPORT_H225_PARAMS == 1) */


int getQosParameters(IN HPVT hVal,
                     IN int qosElemId,
                     INOUT  cmQosCapability * cmQOSCapability);


int buildQosCapability(IN HPVT hVal,
                       IN int qosId,
                       IN cmRSVPParameters *rsvpParam,
                       IN cmATMParameters * atmParam,
                       IN cmNonStandardParam *nonStandard);



int cmiReportControl(
    IN HCONTROL             ctrl,
    IN cmControlState       state,
    IN cmControlStateMode   stateMode);




#ifdef __cplusplus
}
#endif

#endif  /* _CMI_CTRL_ */

