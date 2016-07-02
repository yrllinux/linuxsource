/*

 NOTICE:
 This document contains information that is proprietary to RADVISION LTD..
 No part of this publication may be reproduced in any form whatsoever without
 written prior approval by RADVISION LTD..

  RADVISION LTD. reserves the right to revise this publication and make changes
  without obligation to notify any person of such revisions or changes.

    */

#ifndef _CM_CTRL_RTD_H
#define _CM_CTRL_RTD_H


#include "cmControl.h"


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * rtdInit
 * purpose: Initialize the round trip delay process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
int rtdInit(H245Control* ctrl);

/************************************************************************
 * rtdEnd
 * purpose: Stop the round trip delay process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void rtdEnd(IN H245Control* ctrl);

int roundTripDelayRequest(H245Control* ctrl, int message);
int roundTripDelayResponse(H245Control* ctrl, int message);

#ifdef __cplusplus
}
#endif

#endif /* _CM_CTRL_RTD_H */
