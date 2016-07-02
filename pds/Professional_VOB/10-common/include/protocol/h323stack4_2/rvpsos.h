/***********************************************************************
Filename   : rvpsos.h
Description: config file for pSOS
************************************************************************
        Copyright (c) 2001 RADVISION Inc. and RADVISION Ltd.
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
#ifndef RV_PSOS_H
#define RV_PSOS_H

#if (RV_OS_VERSION == RV_OS_PSOS_2_0)
#include <string.h>
#endif

#define RVAPI
#define RVINTAPI
#define RVCOREAPI
#define RVCALLCONV

/* rvtime: Select timestamp interface to use */
#define RV_TIMESTAMP_TYPE RV_TIMESTAMP_PSOS

/* rvtime: Select clock interface to use */
#define RV_CLOCK_TYPE RV_CLOCK_PSOS

/* rvtm: Select tm (calendar time) interface to use */
#define RV_TM_TYPE RV_TM_PSOS

/* rv64ascii: Select 64 bit conversions to use */
#define RV_64TOASCII_TYPE RV_64TOASCII_MANUAL

/* rvsemaphore: Select semaphore interface to use */
#define RV_SEMAPHORE_TYPE RV_SEMAPHORE_PSOS
#define RV_SEMAPHORE_ATTRIBUTE_DEFAULT (SM_LOCAL | SM_PRIOR) /* flags (do NOT set BOUNDED/UNBOUNDED) */

/* rvmutex: Select mutex interface to use */
#if (RV_OS_VERSION == RV_OS_PSOS_2_0)
#define RV_MUTEX_TYPE RV_MUTEX_MANUAL
#define RV_MUTEX_ATTRIBUTE_DEFAULT 0
#else
#define RV_MUTEX_TYPE RV_MUTEX_PSOS
#define RV_MUTEX_ATTRIBUTE_DEFAULT (MU_LOCAL | MU_PRIOR | MU_PRIO_INHERIT) /* flags (do NOT set RECURSIVE/NORECURSIVE) */
#endif

/* rvthread: Select thread interface to use and set parameters */
#define RV_THREAD_TYPE RV_THREAD_PSOS
#define RV_THREAD_PRIORITY_DEFAULT 126
#define RV_THREAD_STACKSIZE_DEFAULT 32384
#define RV_THREAD_STACKSIZE_USEDEFAULT 0 /* No minimum stack size */
#define RV_THREAD_ATTRIBUTE_DEFAULT { T_LOCAL, T_PREEMPT | T_NOTSLICE | T_SUPV | T_ISR, 0 } /* flags, mode, tslice */
#if (RV_OS_VERSION == RV_OS_PSOS_2_0)
#define TASK_REGISTER_SLOT 7 /* PSOS reserves 8 (0-7) 32-bit register for each task, for the
                                application use. 7 is the last user register slot */
#endif

/* rvlock: Select lock interface to use */
#if (RV_OS_VERSION == RV_OS_PSOS_2_0)
#define RV_LOCK_TYPE RV_LOCK_MANUAL
#define RV_LOCK_ATTRIBUTE_DEFAULT 0
#else
#define RV_LOCK_TYPE RV_LOCK_PSOS
#define RV_LOCK_ATTRIBUTE_DEFAULT (MU_LOCAL | MU_PRIOR | MU_PRIO_INHERIT) /* flags (do NOT set RECURSIVE/NORECURSIVE) */
#endif

/* rvmemory: Select memory interface to use */
#define RV_MEMORY_TYPE RV_MEMORY_STANDARD

/* rvosmem: Select OS dynamic memory driver to use */
#if (RV_OS_VERSION == RV_OS_PSOS_2_0)
#define RV_OSMEM_TYPE RV_OSMEM_MALLOC
#else
#define RV_OSMEM_TYPE RV_OSMEM_PSOS
#endif

/* rvhost: Select network host interface to use */
#define RV_HOST_TYPE RV_HOST_PSOS

/* rvfdevent: File-descriptor events interface to use */
#define RV_SELECT_TYPE RV_SELECT_SELECT

/* rvsockets: Type of Sockets used in the system */
#define RV_SOCKET_TYPE RV_SOCKET_PSOS

/* rvportrange: Type of Port-range used in the system */
#define RV_PORTRANGE_TYPE RV_PORTRANGE_FAST

/* rvloglistener: Type of log listeners used in the system */
#define RV_LOGLISTENER_TYPE RV_LOGLISTENER_TERMINAL

/* rvstdio: Select stdio interface to use */
#define RV_STDIO_TYPE RV_STDIO_ANSI

/* rvassert: Select stdio interface to use */
#define RV_ASSERT_TYPE RV_ASSERT_ANSI


#endif /* RV_PSOS_H */
