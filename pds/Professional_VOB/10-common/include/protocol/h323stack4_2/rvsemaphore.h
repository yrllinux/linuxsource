/* rvsemaphore.h - rvsemaphore header file */
/************************************************************************
      Copyright (c) 2001,2002 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/

/**********************************************************************
 *
 *	DESCRIPTION:	
 *		This module provides semaphore functions.
 *
 ***********************************************************************/

#ifndef RV_SEMAPHORE_H
#define RV_SEMAPHORE_H

#include "rvccore.h"
#include "rvlog.h"

#if !defined(RV_SEMAPHORE_TYPE) || ((RV_SEMAPHORE_TYPE != RV_SEMAPHORE_POSIX) && \
    (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_VXWORKS) && (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_PSOS) && \
    (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_OSE) && (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_NUCLEUS) && \
    (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_WIN32) && (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_SYMBIAN) &&\
    (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_NONE))
#error RV_SEMAPHORE_TYPE not set properly
#endif

#if !defined(RV_SEMAPHORE_ATTRIBUTE_DEFAULT)
#error RV_SEMAPHORE_ATTRIBUTE_DEFAULT not set properly
#endif


/**********************************************************************
 *
 *	RvSemaphore:	
 *		A counting semaphore object.
 *  RvSemaphoreAttr
 *      OS specific attributes and options used for semaphore. 
 *		See definitions in rvsemaphore.h along with the default 
 *		values in rvccoreconfig.h for more information.
 *
 ***********************************************************************/

/* Get include files and define RvSemaphore and RvSemaphoreAttr for each OS */
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_POSIX)
#include <semaphore.h>
typedef sem_t RvSemaphore;
typedef int RvSemaphoreAttr; /* 0 = not shared, otherwise shared */
#endif
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_VXWORKS)
#include <vxWorks.h>
#include <semLib.h>
typedef SEM_ID RvSemaphore;
typedef int RvSemaphoreAttr; /* options to semCCreate */
#endif
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_PSOS)
#include <psos.h>
typedef unsigned long RvSemaphore;
typedef unsigned long RvSemaphoreAttr; /* flags to sm_create (Don't set BOUNDED/UNBOUNDED). */
#if (RV_OS_VERSION == RV_OS_PSOS_2_0)
#define SM_UNBOUNDED 0
#endif
#endif
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_OSE)
#include <ose.h>
typedef SEMAPHORE *RvSemaphore;
typedef int RvSemaphoreAttr; /* not used */
#endif
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_NUCLEUS)
#include <nucleus.h>
typedef NU_SEMAPHORE RvSemaphore;
typedef OPTION RvSemaphoreAttr; /* suspend_type */
#endif
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_WIN32)
typedef HANDLE RvSemaphore;
typedef int RvSemaphoreAttr; /* not used */
#endif
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_SYMBIAN)
#include "rvsymbianinf.h"
typedef void* RvSemaphore;
typedef int RvSemaphoreAttr; /* not used */
#endif
#if (RV_SEMAPHORE_TYPE == RV_SEMAPHORE_NONE)
typedef RvInt RvSemaphore;     /* Dummy types, used to prevent warnings. */
typedef RvInt RvSemaphoreAttr; /* not used */
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* Prototypes and macros */

/********************************************************************************************
 * RvSemaphoreInit - Initializes the Semaphore module.
 *
 * Must be called once (and only once) before any other functions in the module are called.
 *
 * INPUT   : none
 * OUTPUT  : None
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RvStatus Rv2SemaphoreInit(void);

/********************************************************************************************
 * RvSemaphoreEnd - Shuts down the Semaphore module.
 *
 * Must be called once (and only once) when no further calls to this module will be made.
 *
 * INPUT   : none
 * OUTPUT  : None
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RvStatus Rv2SemaphoreEnd(void);

/********************************************************************************************
 * RvSemaphoreSourceConstruct - constructs semaphore module log source
 *
 * The semaphore module log source is constructed per log manager instance.
 *
 * INPUT   : logMgr  - log manager instances
 * OUTPUT  : None
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RvStatus Rv2SemaphoreSourceConstruct(
	IN RvLogMgr* logMgr);


#if (RV_SEMAPHORE_TYPE != RV_SEMAPHORE_NONE)

/********************************************************************************************
 * RvSemaphoreConstruct - Creates a counting semaphore object.
 *
 * note: The maximum value of a sempahore is OS and architecture dependent.
 *
 * INPUT   : statcount	- Initial value of the semaphore.
 *			 logMgr		- log manager instances
 * OUTPUT  : sema		- Pointer to lock object to be constructed.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus Rv2SemaphoreConstruct(
	IN  RvUint32	startcount, 
	IN  RvLogMgr	*logMgr, 
	OUT RvSemaphore *sema);


/********************************************************************************************
 * RvSemaphoreDestruct - Destroys a counting semaphore object.
 *
 * note: Never destroy a semaphore object which has a thread suspended on it.
 *
 * INPUT   : sema		- Pointer to semaphore object to be destructed.
 *			 logMgr		- log manager instances
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2SemaphoreDestruct(
	IN RvSemaphore	*sema,
	IN RvLogMgr		*logMgr);


/********************************************************************************************
 * RvSemaphorePost - Increments the semaphore.
 *
 * note: The maximum value of a sempahore is OS and architecture dependent.
 *
 * INPUT   : sema		- Pointer to semaphore object to be incremented.
 *			 logMgr		- log manager instances
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus Rv2SemaphorePost(
	IN RvSemaphore	*sema,
	IN RvLogMgr		*logMgr);

/********************************************************************************************
 * RvSemaphoreWait - Decrements a semaphore.
 *
 * If the semaphore is 0, it will suspend the calling task until it can.
 *
 * INPUT   : sema		- Pointer to semaphore object to be decremented.
 *			 logMgr		- log manager instances
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus Rv2SemaphoreWait(
	IN RvSemaphore	*sema,
	IN RvLogMgr		*logMgr);

/********************************************************************************************
 * RvSemaphoreTryWait - Try to decrement a semaphore.
 *
 * If the semaphore is 0, it will not suspend the calling task. Instead, this function
 * returns RV_ERROR_TRY_AGAIN error code.
 *
 * INPUT   : sema       - Pointer to semaphore object to be decremented.
 *           logMgr     - log manager instances
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus Rv2SemaphoreTryWait(
    IN RvSemaphore  *sema,
    IN RvLogMgr     *logMgr);


/********************************************************************************************
 * RvSemaphoreSetAttr
 *
 * Sets the options and attributes to be used when creating and using semaphore objects.
 * note: Non-reentrant function. Do not call when other threads may be calling rvsemaphore functions.
 * note: These attributes are global and will effect all semaphore functions called thereafter.
 * note: The default values for these attributes are set in rvccoreconfig.h.
 *
 * INPUT   : attr		- Pointer to OS specific semaphore attributes to begin using.
 *           logMgr		- log manager instances
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SemaphoreSetAttr(
	IN RvSemaphoreAttr *attr,
	IN RvLogMgr		   *logMgr);

#else
/* If none is set then none of these functions do anything */
#define Rv2SemaphoreConstruct(_c,_lg,_s) (*(_s) = RV_OK)
#define Rv2SemaphoreDestruct(_s,_lg) (*(_s) = RV_OK)
#define Rv2SemaphorePost(_s,_lg) (*(_s) = RV_OK)
#define Rv2SemaphoreWait(_s,_lg) (*(_s) = RV_OK)
#define Rv2SemaphoreTryWait(_s,_lg) (*(_s) = RV_OK)
#define Rv2SemaphoreSetAttr(_s,_lg) (*(_s) = RV_OK)
#endif


#if defined(__cplusplus)
}
#endif

#endif
