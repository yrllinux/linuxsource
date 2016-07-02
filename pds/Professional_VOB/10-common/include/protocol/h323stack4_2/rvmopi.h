/***********************************************************************
Filename   : rvmopi.h
Description: configuration file for MOPI
************************************************************************
        Copyright (c) 2003 RADVISION Inc. and RADVISION Ltd.
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
#ifndef RV_MOPI_H
#define RV_MOPI_H

#include "global.h"
#include "mopi.h"

#define RVAPI
#define RVINTAPI
#define RVCOREAPI
#define RVCALLCONV


// The following REMARK adds a flag to lint.
// This is done especially for a costumer demand, since their compilation depends on lint output:
/*IGNORE lint -w1 */   // Disable warnings and infos from being outputed.
// Note that this flag should be turned back on when compilation of the common core
// and the stack above is done. (the way to do it: place /*lint +w1 */ on the
// last file that compiles in the stack project)


/* calculates offset of a field within a structure, in bytes */
//#define offsetof(s,m)   (size_t)&(((s *)0)->m)

/* completes a missing definition in MOPI */
typedef long time_t;

/* check: only on unitest? */
#define timeval             bsd_timeval

/* a list of embedded processes that are used with MOPI */
typedef enum
{
	MOPI_EMB_PROC_FIRST = 0,
	MOPI_EMB_PROC_SIP = MOPI_EMB_PROC_FIRST,
	MOPI_EMB_PROC_RTP,
	MOPI_EMB_PROC_LAST
} MOPI_EMB_PROC;



/* rvtime: Select timestamp interface to use */
#define RV_TIMESTAMP_TYPE RV_TIMESTAMP_MOPI

/* rvtime: Select clock interface to use */
#define RV_CLOCK_TYPE RV_CLOCK_MOPI

/* rvtm: Select tm (calendar time) interface to use */
#define RV_TM_TYPE RV_TM_MOPI

/* rv64ascii: Select 64 bit conversions to use */
#define RV_64TOASCII_TYPE RV_64TOASCII_STANDARD

/* rvsemaphore: Select semaphore interface to use */
    /* since MOPI doesn't support semaphores, we use the actual underlying platform,
       which is nucleus */
#if defined(_WIN32)
#define RV_SEMAPHORE_TYPE RV_SEMAPHORE_WIN32
#define RV_SEMAPHORE_ATTRIBUTE_DEFAULT 0 /* not used */
#else
#define RV_SEMAPHORE_TYPE RV_SEMAPHORE_NUCLEUS
#define RV_SEMAPHORE_ATTRIBUTE_DEFAULT NU_PRIORITY /* suspend_type */
#endif

/* rvmutex: Select mutex interface to use */
#define RV_MUTEX_TYPE RV_MUTEX_MOPI
#define RV_MUTEX_ATTRIBUTE_DEFAULT { 0 } /* not used */

/* rvthread: Select thread interface to use and set parameters */
#define RV_THREAD_TYPE RV_THREAD_MOPI
#define RV_THREAD_PRIORITY_DEFAULT 10
#define RV_THREAD_STACKSIZE_DEFAULT 0 /* Allow OS to allocate */
#define RV_THREAD_STACKSIZE_USEDEFAULT 0x100000 /* Under this stack size use default stack size */
#define RV_THREAD_ATTRIBUTE_DEFAULT { 0 } /* unused in mopi */

/* rvlock: Select lock interface to use */
#define RV_LOCK_TYPE RV_LOCK_MOPI
#define RV_LOCK_ATTRIBUTE_DEFAULT { 0 } /* unused in mopi */

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
#if defined(_WIN32)
#define RV_LOGLISTENER_TYPE RV_LOGLISTENER_FILE_AND_TERMINAL
#else
/* we use costumer's proprietary logging (Tracing with WResi) */
#define RV_LOGLISTENER_TYPE RV_LOGLISTENER_NONE
#endif

/* rvstdio: Select stdio interface to use */
#define RV_STDIO_TYPE RV_STDIO_ANSI

/* rvassert: Select stdio interface to use */
#define RV_ASSERT_TYPE RV_ASSERT_ANSI


#endif /* RV_MOPI_H */
