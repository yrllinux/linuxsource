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

/**************************************************************************
 *
 * Select Interface.
 *
 * Provides absrtact selection of file descriptors upon read, write and
 *  timeouts per task.
 *
 * Supporting calls from different tasks (using thread local storage).
 *
 **************************************************************************/

#ifndef _RV_H323_SELI_H
#define _RV_H323_SELI_H

/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#include "rvtypes.h"
#include "rverror.h"


#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/

/* SELI events */
typedef enum
{
    seliEvRead = 0x01, /* READ event */
    seliEvWrite = 0x02 /* WRITE event */
} seliEvents;


/************************************************************************
 * seliCallback
 * purpose: Callback function returned on events
 * input  : fd      - Handle/file descriptor this event occured on
 *          sEvent  - Event that occured
 *          error   - RV_TRUE if there was an error in the event
 * output : none
 * return : none
 ************************************************************************/
typedef void (RVCALLCONV *seliCallback) (
    IN int          fd,
    IN seliEvents   sEvent,
    IN RvBool       error);


/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/


/************************************************************************
 * seliInit
 * purpose: Initialize a SELI interface.
 * input  : none
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliInit(void);

/************************************************************************
 * seliInit
 * purpose: End a SELI interface.
 * input  : none
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliEnd(void);

/************************************************************************
 * seliSelect
 * purpose: Block on the select() interface on some operating systems.
 *          Use parallel interfaces on other operating systems.
 *          An application should write a "while (1) seliSelect();"
 *          as its main loop.
 * input  : none
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliSelect(void);

/******************************************************************************
 * seliSelectUntil
 * ----------------------------------------------------------------------------
 * General: Block on the select() interface on some operating systems.
 *          Use parallel interfaces on other operating systems.
 *          This function also gives the application the ability to give a
 *          maximum delay to block for.
 *
 * Return Value: RV_OK on success, negative value on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  delay    - Maximum time to block on milliseconds.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV seliSelectUntil(IN RvUint32 delay);

/************************************************************************
 * seliCallOn
 * purpose: Ask the SELI interface for events on a given handle/file
 *          descriptor.
 * input  : fd              - Handle/File descriptor to wait on for events
 *          sEvents         - Events to wait for
 *          callbackFunc    - Function to call when the event occurs
 * output : none
 * return : RV_OK on success, negative value on failure
 ************************************************************************/
RVAPI RvStatus RVCALLCONV seliCallOn(
    IN int              fd,
    IN seliEvents       sEvents,
    IN seliCallback     callbackFunc);


int seliSetMaxDescs(IN int maxDescs);
int seliGetMaxDescs(void);



/* The following functions are only relevant for systems supporting the select() interface */
#if defined(FD_SETSIZE)

RvStatus seliSelectEventsRegistration(
    IN  int        fdSetLen,
    OUT int        *num,
    OUT fd_set     *rdSet,
    OUT fd_set     *wrSet,
    OUT fd_set     *exSet,
    OUT RvUint32   *timeOut);

RvStatus seliSelectEventsHandling(
    IN fd_set   *rdSet,
    IN fd_set   *wrSet,
    IN fd_set   *exSet,
    IN int      num,
    IN int      numEvents);

#endif  /* select() operating systems */


/* The following functions are only relevant for systems supporting the poll() interface */
#if (RV_OS_TYPE == RV_OS_TYPE_SOLARIS) || \
    (RV_OS_TYPE == RV_OS_TYPE_LINUX) || \
    (RV_OS_TYPE == RV_OS_TYPE_HPUX) || \
    (RV_OS_TYPE == RV_OS_TYPE_TRU64) || \
    (RV_OS_TYPE == RV_OS_TYPE_UNIXWARE) || \
    (RV_OS_TYPE == RV_OS_TYPE_EMBLINUX)

struct pollfd; /* This line is here to remove warnings from the compilation process */

RvStatus seliPollEventsRegistration(
    IN  int             len,
    OUT struct pollfd   *pollFdSet,
    OUT int             *num,
    OUT RvUint32        *timeOut);

RvStatus seliPollEventsHandling(
    IN struct pollfd    *pollFdSet,
    IN int              num,
    IN int              numEvents);

#endif /* poll() operating systems */




#if defined(RV_H323_COMPILE_WITH_DEAD_FUNCTIONS)
/* The following functions are here for backward compatability only.
   They have no purpose besides that. */
int seliSetMaxTasks(int maxTasks);
int seliGetMaxTasks(void);
#endif  /* defined(RV_H323_COMPILE_WITH_DEAD_FUNCTIONS) */


//add by yj
/*
 *	add by Tan guang
 */
RVAPI
void RVCALLCONV seliSetMultiThreads(int mt);

RVAPI
int RVCALLCONV RadInitSem();

RVAPI
int RVCALLCONV RadEnterCritic();

RVAPI
int RVCALLCONV RadExitCritic();

RVAPI
int RVCALLCONV RadEndSem();
//end



#ifdef __cplusplus
}
#endif

#endif  /*END OF: define _RV_H323_SELI_H*/
