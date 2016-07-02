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
#ifndef _CMSIZE_H
#define _CMSIZE_H

#include "cm.h"

#ifdef RV_H323_COMPILE_WITH_DEAD_FUNCTIONS

#ifdef __cplusplus
extern "C" {
#endif


RVAPI int RVCALLCONV
cmSizeCurProtocols(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxProtocols(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurProcs(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxProcs(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurEvents(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxEvents(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurTimers(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxTimers(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurUdpChans(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxUdpChans(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurTpktChans(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxTpktChans(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurChannels(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxChannels(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurMessages(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxMessages(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeCurChanDescs(HAPP hApp);

RVAPI int RVCALLCONV
cmSizeMaxChanDescs(HAPP hApp);

#ifdef __cplusplus
}
#endif

#endif  /* RV_H323_COMPILE_WITH_DEAD_FUNCTIONS */


#endif  /* _CMSIZE_H */

