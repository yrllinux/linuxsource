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

#ifndef _RV_MTI_H
#define _RV_MTI_H

#include "rvcommon.h"

#ifdef __cplusplus
extern "C" {
#endif


RV_DECLARE_HANDLE(HTI);
RV_DECLARE_HANDLE(HSTIMER);
RV_DECLARE_HANDLE(HAPPTIMER);


#define HTI_NULL_TIMER ((HTI)(-1))


typedef void(RVCALLCONV*LPMTIMEREVENTHANDLER)(IN void* context);


RVAPI RvUint32 RVCALLCONV timerGetTimeInMilliseconds(void);


RVAPI RvUint32 RVCALLCONV timerGetTimeInSeconds(void);


RVAPI HSTIMER RVCALLCONV
mtimerInit(IN int maxTimers, IN HAPPTIMER appHndl);

RVAPI int RVCALLCONV
mtimerEnd(IN HSTIMER timer);

RVAPI HTI RVCALLCONV
mtimerSet(
    IN HSTIMER              timer,
    IN LPMTIMEREVENTHANDLER eventHandler,
    IN void*                context,
    IN RvUint32             timeOut  /* 1 msec units */
    );

RVAPI int RVCALLCONV
mtimerReset(
    IN HSTIMER  timer,
    IN HTI      tElem);




#ifdef RV_H323_COMPILE_WITH_DEAD_FUNCTIONS

RVAPI int RVCALLCONV
mtimerEndByHandle(IN HAPPTIMER appHndl);


RVAPI HTI RVCALLCONV
mtimerSetByHandle(
    IN HAPPTIMER            appHndl,
    IN LPMTIMEREVENTHANDLER eventHandler,
    IN void*                context,
    IN RvUint32             timeOut);


RVAPI int RVCALLCONV
mtimerResetByValue(
    IN HSTIMER              timer,
    IN LPMTIMEREVENTHANDLER eventHandler,
    IN void*                context);

RVAPI int RVCALLCONV
mtimerResetByHandle(
    IN HAPPTIMER    appHndl,
    IN HTI          tElem);



/************************************************************************
 * mtimerSetMaxTimerSets
 * purpose: To change the maximum number of timer sets (mtimerInit calls)
 *          allowed per thread (the default is 16)
 * input  : timerSets   - the maximum number of timer sets
 * output : none
 * return : none
 ************************************************************************/
RVAPI void RVCALLCONV mtimerSetMaxTimerSets(IN int timerSets);

#endif  /* RV_H323_COMPILE_WITH_DEAD_FUNCTIONS */


#ifdef __cplusplus
}
#endif

#endif  /* _RV_MTI_H */
