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





int requestMode(H245Control* ctrl, int message);
int requestModeAck(H245Control* ctrl, int message);
int requestModeReject(H245Control* ctrl, int message);
int requestModeRelease(H245Control* ctrl, int message);

/************************************************************************
 * rmInit
 * purpose: Initialize the request mode process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
int rmInit(H245Control* ctrl);

/************************************************************************
 * rmEnd
 * purpose: Stop the request mode process on a control channel
 * input  : ctrl    - Control object
 * output : none
 * return : none
 ************************************************************************/
void rmEnd(IN H245Control* ctrl);


#ifdef __cplusplus
}
#endif
