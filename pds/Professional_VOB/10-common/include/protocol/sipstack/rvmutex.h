/* rvmutex.h - rvmutex header file */
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
 *		This module provides recursive locking functions to use specifically
 *		for locking code sections.
 *
 ***********************************************************************/

#ifndef RV_MUTEX_H
#define RV_MUTEX_H

#include "rvccore.h"
#include "rvlock.h"
#include "rvlog.h"

#if !defined(RV_MUTEX_TYPE) || ((RV_MUTEX_TYPE != RV_MUTEX_SOLARIS) && \
    (RV_MUTEX_TYPE != RV_MUTEX_LINUX) && (RV_MUTEX_TYPE != RV_MUTEX_VXWORKS) && \
    (RV_MUTEX_TYPE != RV_MUTEX_PSOS) && (RV_MUTEX_TYPE != RV_MUTEX_WIN32_MUTEX) && \
    (RV_MUTEX_TYPE != RV_MUTEX_WIN32_CRITICAL) && (RV_MUTEX_TYPE != RV_MUTEX_MOPI) && \
	(RV_MUTEX_TYPE != RV_MUTEX_MANUAL) && (RV_MUTEX_TYPE != RV_MUTEX_NONE))
#error RV_MUTEX_TYPE not set properly
#endif

#if !defined(RV_MUTEX_ATTRIBUTE_DEFAULT)
#error RV_MUTEX_ATTRIBUTE_DEFAULT not set properly
#endif

/* Get include files and define RvMutex and RvMutexAttr for each OS */
#if (RV_MUTEX_TYPE == RV_MUTEX_SOLARIS) || (RV_MUTEX_TYPE == RV_MUTEX_LINUX)
/* They're both posix, but they're not exactly the same, so we need 2 settings */
#include <pthread.h>
typedef struct {
  pthread_mutex_t mtx;
  RvInt32         count;
} RvMutex;

/************************************************************************
 * RvMutexAttr:
 *	OS specific attributes and options used for mutexes. 
 *	See definitions in rvmutex.h
 *  along with the default values in rvccoreconfig.h for more information.
 ************************************************************************/
typedef struct {
	/* These correspond to attributes in the pthread_mutexattr struct that we let users set */
	int pshared;  /* used only by Solaris */
	int protocol; /* used only by Solaris */
} RvMutexAttr;
#endif
#if (RV_MUTEX_TYPE == RV_MUTEX_VXWORKS)
#include <vxWorks.h>
#include <semLib.h>

typedef struct {
	SEM_ID		mtx;
	RvUint32	count;
} RvMutex;

typedef int RvMutexAttr;  /* options to semMCreate */
#endif
#if (RV_MUTEX_TYPE == RV_MUTEX_PSOS)
#include <psos.h>

typedef struct {
	unsigned long	mtx;
	RvUint32		count;
} RvMutex;

typedef unsigned long RvMutexAttr; /* flags to mu_create (Don't set RECURSIVE/NORECURSIVE) */

#endif
#if (RV_MUTEX_TYPE == RV_MUTEX_WIN32_MUTEX)

typedef struct {
	HANDLE		mtx;
	RvUint32	count;
} RvMutex;

typedef int RvMutexAttr; /* not used */

#endif
#if (RV_MUTEX_TYPE == RV_MUTEX_WIN32_CRITICAL)

typedef struct {
	CRITICAL_SECTION	mtx;
	RvUint32			count;
} RvMutex;

typedef DWORD RvMutexAttr; /* spin count (use only on Win2000 and newer) */

#endif

#if (RV_MUTEX_TYPE == RV_MUTEX_MOPI)
#include <mopi.h>

typedef struct {
	T_MOPI_Mutex		mtx;
	RvUint32			count;
} RvMutex;

typedef int RvMutexAttr; /* not used */

#endif

#if (RV_MUTEX_TYPE == RV_MUTEX_MANUAL)
#include "rvsemaphore.h"
#include "rvthread.h"


/************************************************************************
 * RvMutex:
 *	A recursive mutex object 
 ************************************************************************/
typedef struct {
	RvSemaphore		handle;
	RvSemaphore		lock;
	RvThreadId		owner;
	RvUint32		count;
	RvUint32		waiters;
} RvMutex;

typedef int RvMutexAttr; /* not used, any semaphore attributes will apply */

#endif
#if (RV_MUTEX_TYPE == RV_MUTEX_NONE)
typedef RvInt RvMutex;     /* Dummy types, used to prevent warnings. */
typedef RvInt RvMutexAttr; /* not used */
#endif

#if defined(__cplusplus)
extern "C" {
#endif 

/* Prototypes: See documentation blocks below for details. */
/********************************************************************************************
 * RvMutexInit - Initializes the Mutex module.
 *
 * Must be called once (and only once) before any other functions in the module are called.
 *
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : Always RV_OK
 */
RvStatus RvMutexInit(void);

/********************************************************************************************
 * RvMutexEnd - Shuts down the Mutex module. 
 *
 * Must be called once (and only once) when no further calls to this module will be made.
 *
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : Always RV_OK
 */
RvStatus RvMutexEnd(void);
/********************************************************************************************
 * RvLockSourceConstruct - Constructs lock module log source.
 *
 * Constructs log source to be used by common core when printing log from the 
 * lock module. This function is applied per instance of log manager.
 * 
 * INPUT   : logMgr - log manager instance
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code. 
 */
RvStatus RvMutexSourceConstruct(
	IN RvLogMgr	*logMgr);

#if (RV_MUTEX_TYPE != RV_MUTEX_NONE)
/********************************************************************************************
 * RvMutexConstruct - Creates a recursive mutex object. 
 *
 * INPUT   : logMgr	- log manager instance
 * OUTPUT  : mu		- Pointer to mutex object to be constructed.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RVCALLCONV RvMutexConstruct(
	IN  RvLogMgr*  logMgr,
	OUT RvMutex*   mu);

/********************************************************************************************
 * RvMutexDestruct - Destroys a recursive mutex object.
 *
 * note: Never destroy a mutex object which has a thread suspended on it.
 *
 * INPUT   : mu		- Pointer to recursive mutex object to be destructed.
 *			 logMgr	- log manager instance
 * OUTPUT  : 
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RVCALLCONV RvMutexDestruct(
	IN  RvMutex*   mu,
	IN  RvLogMgr*  logMgr);


/********************************************************************************************
 * RvMutexLock - Aquires a recursive mutex.
 *
 * Will suspend the calling task until the mutex is available.
 *
 * INPUT   : mu		- Pointer to mutex object to be aquired.
 *			 logMgr	- log manager instance
 * OUTPUT  : 
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RVCALLCONV RvMutexLock(
	IN  RvMutex*   mu,
	IN  RvLogMgr*  logMgr);


/********************************************************************************************
 * RvMutexUnlock - Unlocks a recursive mutex.
 *
 * INPUT   : mu     - Pointer to mutex object to be unlocked.
 *           logMgr - log manager instance
 * OUTPUT  :
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RVCALLCONV RvMutexUnlock(
	IN  RvMutex*   mu,
	IN  RvLogMgr*  logMgr);


/********************************************************************************************
 * RvMutexSetAttr
 *
 * Sets the options and attributes to be used when creating and using mutex objects.
 * note: Non-reentrant function. Do not call when other threads may be calling rvmutex functions.
 * note: These attributes are global and will effect all mutex functions called thereafter.
 * note: The default values for these attributes are set in rvccoreconfig.h.
 *
 * INPUT   : mu		- Pointer to mutex object to be unlocked.
 *			 logMgr	- log manager instance
 * OUTPUT  : 
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RvMutexSetAttr(
	IN RvMutexAttr *attr,
	IN RvLogMgr    *logMgr);

/********************************************************************************************
 * RvMutexGetLockCounter - Returns the number of times the mutex has been locked by
 *                         this thread.
 *
 * INPUT   : mu     - Pointer to mutex object
 *           logMgr - log manager instance
 * OUTPUT  : lockCnt - lock counter to be returned
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RvMutexGetLockCounter(
	IN  RvMutex		*mu,
	IN  RvLogMgr	*logMgr,
	OUT RvInt32		*lockCnt);

/********************************************************************************************
 * RvMutexRelease - Unlocks a mutex recursively until the mutex is released completely.
 *
 * INPUT   : mu     - Pointer to mutex object
 *           logMgr - log manager instance
 * OUTPUT  : lockCnt - lock counter to be returned
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RvMutexRelease(
    IN  RvMutex     *mu,
    IN  RvLogMgr    *logMgr,
    OUT RvInt32     *lockCnt);

/********************************************************************************************
 * RvMutexRestore - Locks a mutex recursively lockCnt times (restores a mutex to its
 *                  previously saved state).
 *
 * INPUT   : mu     - Pointer to mutex object
 *           logMgr - log manager instance
 * OUTPUT  : lockCnt - the original lock counter
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RvMutexRestore(
    IN  RvMutex     *mu,
    IN  RvLogMgr    *logMgr,
    IN  RvInt32     lockCnt);

#else

/* If none is set then none of these functions do anything */
#define RvMutexConstruct(_lg,_m) (*(_m) = RV_OK)
#define RvMutexDestruct(_m,_lg) (*(_m) = RV_OK)
#define RvMutexLock(_m,_lg) (*(_m) = RV_OK)
#define RvMutexUnlock(_m,_lg) (*(_m) = RV_OK)
#define RvMutexSetAttr(_m,_lg) (*(_m) = RV_OK)
#define RvMutexGetLockCounter(_m,_lg,_c) (*(_m) = RV_OK)
#define RvMutexRelease(_m,_lg,_c) (*(_m) = RV_OK)
#define RvMutexRestore(_m,_lg,_c) (*(_m) = RV_OK)
#endif

#if defined(__cplusplus)
}
#endif 

#endif
