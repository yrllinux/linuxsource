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

/* todo: resolve timers issue as part of the exported DLL functions */

#ifndef _RVH323TIMER_H
#define _RVH323TIMER_H

#include "rvstdio.h"
#include "rvtimer.h"
#include "rvtime.h"
#include "ra.h"
#include "rvinternal.h"


#ifdef __cplusplus
extern "C" {
#endif


RV_DECLARE_HANDLE(RvH323TimerPoolHandle);

/************************************************************************
 * RvH323TimerConstruct
 * purpose: Create a timers array.
 * input  : maxTimers   - Maximum number of timers needed
 *          logMgr      - Log manager to use for timers
 * output : timersQueue - Queue of timers these timers are in
 * return : pointer to timers handle on success, NULL o.w.
 ************************************************************************/
RVAPI RvH323TimerPoolHandle RVCALLCONV RvH323TimerConstruct(
    IN    int               maxTimers,
    IN    RvLogMgr*         logMgr,
    OUT   RvTimerQueue**    timersQueue);


/************************************************************************
 * RvH323TimerDestruct
 * purpose: Create a timers array.
 * input  : timers          - pointer to timers array
 * output : none
 * return : none
 ************************************************************************/
RVAPI void RVCALLCONV RvH323TimerDestruct(
    IN RvH323TimerPoolHandle    timersH);


/************************************************************************
 * RvH323TimerStartWithType
 * purpose: Set a timer of the stack.
 * input  : timers          - pointer to timers array
 *          eventHandler    - Callback to call when timer expires
 *          context         - Context to use as parameter for callback function
 *          timeOut         - Timeout of timer in nanoseconds (0 is not ignored)
 *          timerType       - Type of timer: RV_TIMER_TYPE_ONESHOT, RV_TIMER_TYPE_PERIODIC
 * output : None
 * return : Pointer to timer on success, NULL o.w.
 ************************************************************************/
RVAPI RvTimer* RVCALLCONV RvH323TimerStartWithType(
    IN    RvH323TimerPoolHandle timersH,
    IN    RvTimerFunc           eventHandler,
    IN    void*                 context,
    IN    RvInt64               timeOut,
    IN    RvInt                 timerType);


/************************************************************************
 * RvH323TimerStart
 * purpose: Set a timer of the stack, reseting its value if it had one
 *          previously.
 * input  : timers          - pointer to timers array
 *          eventHandler    - Callback to call when timer expires
 *          context         - Context to use as parameter for callback function
 *          timeOut         - Timeout of timer in milliseconds
 * output : None
 * return : pointer to timer on success, NULL o.w.
 ************************************************************************/
#define RvH323TimerStart(_timersH, _eventHandler, _context, _timeOut) \
    ( ((_timeOut) <= 0) ? NULL :                                                 \
      RvH323TimerStartWithType((_timersH), (_eventHandler), (_context),         \
      Rv2Int64Mul(Rv2Int64FromRvInt(_timeOut), RV_TIME64_NSECPERMSEC), RV_TIMER_TYPE_ONESHOT) )


/************************************************************************
 * RvH323TimerStartPeriodic
 * purpose: Set a periodic timer for the stack
 * input  : timers          - pointer to timers array
 *          eventHandler    - Callback to call when timer expires
 *          context         - Context to use as parameter for callback function
 *          timeOut         - Timeout of timer in milliseconds
 * output : None
 * return : pointer to timer on success, NULL o.w.
 ************************************************************************/
#define RvH323TimerStartPeriodic(_timersH, _eventHandler, _context, _timeOut) \
    ( ((_timeOut) <= 0) ? NULL :                                                 \
      RvH323TimerStartWithType((_timersH), (_eventHandler), (_context),         \
      Rv2Int64Mul(Rv2Int64FromRvInt(_timeOut), RV_TIME64_NSECPERMSEC), RV_TIMER_TYPE_PERIODIC) )


/************************************************************************
 * RvH323TimerCancel
 * purpose: Reset a timer if it's set
 *          Used mainly for call timers.
 * input  : timer   - Timer to reset
 * output : timer   - Timer's value after it's reset
 * return : RV_OK on success, other on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323TimerCancel(
    IN    RvH323TimerPoolHandle timersH,
    INOUT RvTimer               **timer);


/************************************************************************
 * RvH323TimerClear
 * purpose: Clear a timer from the array
 * input  : timers  - pointer to timers array
 *          timer   - Timer to reset
 * output : none
 * return : RV_OK on success, other on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323TimerClear(
    IN    RvH323TimerPoolHandle timersH,
    IN    RvTimer               **timer);


/************************************************************************
 * RvH323TimerStatistics
 * purpose: Get timer pool statistics
 * input  : timers      - pointer to timers array
 * output : statistics  - Statistics information about the timer pool
 * return : RV_OK on success, other on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV RvH323TimerStatistics(
    IN    RvH323TimerPoolHandle timersH,
    OUT   RvRaStatistics*       statistics);



#ifdef __cplusplus
}
#endif

#endif  /* _RVH323TIMER_H */


