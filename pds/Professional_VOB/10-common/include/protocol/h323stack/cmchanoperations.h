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


#ifndef _CMCHANOPERATIONS_
#define _CMCHANOPERATIONS_

int flowControlCommand(controlElem*ctrl, int message);
int maintenanceLoopRequest(controlElem*ctrl, int message);
int maintenanceLoopAck(controlElem*ctrl, int message);
int maintenanceLoopReject(controlElem*ctrl, int message);
int maintenanceLoopOffCommand(controlElem*ctrl, int message);
int videoFreezePicture(controlElem* ctrl, int lcn);
int videoSendSyncEveryGOB(controlElem* ctrl, int lcn);
int videoSendSyncEveryGOBCancel(controlElem* ctrl, int lcn);
int switchReceiveMediaOff(controlElem* ctrl, int lcn);
int switchReceiveMediaOn(controlElem* ctrl, int lcn);
int videoTemporalSpatialTradeOff(controlElem* ctrl,int message, int lcn);
int videoTemporalSpatialTradeOffInd(controlElem* ctrl,int message, int lcn);
int videoFastUpdatePicture(controlElem* ctrl, int lcn);
int videoFastUpdateGOB(controlElem* ctrl,int message, int lcn);
int videoFastUpdateMB(controlElem* ctrl,int message, int lcn);
int h2250MaximumSkewIndication(controlElem* ctrl,int message);
int logicalChannelActive(controlElem* ctrl, int lcn);
int logicalChannelInactive(controlElem* ctrl, int lcn);
int transportCapability(controlElem* ctrl,int message, int lcn);

#endif
#ifdef __cplusplus
}
#endif
