/***********************************************************************
Filename   : rvunixware.h
Description: config file for UnixWare
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
#ifndef RV_UNIXWARE_H
#define RV_UNIXWARE_H

#define RVAPI
#define RVINTAPI
#define RVCOREAPI
#define RVCALLCONV

/* rvtime: Select timestamp interface to use */
#define RV_TIMESTAMP_TYPE RV_TIMESTAMP_UNIXWARE

/* rvtime: Select clock interface to use */
#define RV_CLOCK_TYPE RV_CLOCK_UNIXWARE

/* rvtm: Select tm (calendar time) interface to use */
#define RV_TM_TYPE RV_TM_POSIX

/* rv64ascii: Select 64 bit conversions to use */
#define RV_64TOASCII_TYPE RV_64TOASCII_STANDARD

/* rvsemaphore: Select semaphore interface to use */
#define RV_SEMAPHORE_TYPE RV_SEMAPHORE_POSIX
#define RV_SEMAPHORE_ATTRIBUTE_DEFAULT 0 /* 0 = not shared, otherwise shared */

/* rvmutex: Select mutex interface to use */
#define RV_MUTEX_TYPE RV_MUTEX_MANUAL
#define RV_MUTEX_ATTRIBUTE_DEFAULT 0 /* not used */

/* rvthread: Select thread interface to use and set parameters */
#define RV_THREAD_TYPE RV_THREAD_UNIXWARE
#define RV_THREAD_PRIORITY_DEFAULT 10
#define RV_THREAD_STACKSIZE_DEFAULT 0 /* Allow OS to allocate */
#define RV_THREAD_STACKSIZE_USEDEFAULT 0x100000 /* Under this stack size use default stack size */
#define RV_THREAD_ATTRIBUTE_DEFAULT { PTHREAD_SCOPE_SYSTEM, SCHED_OTHER, PTHREAD_EXPLICIT_SCHED }

/* rvlock: Select lock interface to use */
#define RV_LOCK_TYPE RV_LOCK_SOLARIS
#define RV_LOCK_ATTRIBUTE_DEFAULT { PTHREAD_MUTEX_NORMAL, PTHREAD_PROCESS_PRIVATE, PTHREAD_PRIO_NONE } /* type, pshared, protocol */

/* rvmemory: Select memory interface to use */
#define RV_MEMORY_TYPE RV_MEMORY_STANDARD

/* rvosmem: Select OS dynamic memory driver to use */
#define RV_OSMEM_TYPE RV_OSMEM_MALLOC

/* rvhost: Select network host interface to use */
#define RV_HOST_TYPE RV_HOST_POSIX

/* rvfdevent: File-descriptor events interface to use */
#define RV_SELECT_TYPE RV_SELECT_SELECT

/* rvsockets: Type of Sockets used in the system */
#define RV_SOCKET_TYPE RV_SOCKET_BSD

/* rvportrange: Type of Port-range used in the system */
#define RV_PORTRANGE_TYPE RV_PORTRANGE_FAST

/* rvloglistener: Type of log listeners used in the system */
#define RV_LOGLISTENER_TYPE RV_LOGLISTENER_FILE_AND_TERMINAL

/* rvstdio: Select stdio interface to use */
#define RV_STDIO_TYPE RV_STDIO_ANSI

/* rvassert: Select stdio interface to use */
#define RV_ASSERT_TYPE RV_ASSERT_ANSI


#endif /* RV_UNIXWARE_H */
