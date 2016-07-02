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





/* multipoint mode command on/off____________________________________________________________ */
int conferenceIndication(H245Control* ctrl, int message);
int conferenceCommand(H245Control* ctrl, int message);
int miscellaneousCommand(H245Control* ctrl, int lcn, int message);
int indication(H245Control* ctrl, int lcn, int message);
int miscellaneousIndication(H245Control* ctrl, int lcn, int message);
int multipointModeCommand(H245Control* ctrl,int lcn);
int cancelMultipointModeCommand(H245Control* ctrl,int lcn);

#ifdef __cplusplus
}
#endif
