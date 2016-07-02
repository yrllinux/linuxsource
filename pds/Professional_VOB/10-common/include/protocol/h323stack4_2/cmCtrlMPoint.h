/*

 NOTICE:
 This document contains information that is proprietary to RADVISION LTD..
 No part of this publication may be reproduced in any form whatsoever without
 written prior approval by RADVISION LTD..

  RADVISION LTD. reserves the right to revise this publication and make changes
  without obligation to notify any person of such revisions or changes.

    */


#ifndef _CMCTRL_MPOINT_H
#define _CMCTRL_MPOINT_H

#ifdef __cplusplus
extern "C" {
#endif




#if (RV_H245_SUPPORT_H225_PARAMS == 1)
int mcLocationIndication(H245Control* ctrl, int message);
#endif  /* (RV_H245_SUPPORT_H225_PARAMS == 1) */


int multipointConference(H245Control* ctrl,int lcn);
int cancelMultipointConference(H245Control* ctrl,int lcn);


/************************************************************************
 * conferenceMessage
 * purpose: Deal with various incoming H245 conference messages
 *          This function uses the callback to the application to handle
 *          the message
 * input  : ctrl        - H245 control object of the call
 *          lcn         - Logical channel of this message.
 *                        0 if no channel is related to this call
 *          message     - root node ID of the message to handle
 *          messaheType - Type of message
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int conferenceMessage(
    IN H245Control*         ctrl,
    IN int                  lcn,
    IN int                  message,
    IN cmH245MessageType    messageType);


int communicationModeCommand(H245Control* ctrl, int message);


#ifdef __cplusplus
}
#endif

#endif  /* _CMCTRL_MPOINT_H */
