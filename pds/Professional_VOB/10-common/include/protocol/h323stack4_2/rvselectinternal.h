/***********************************************************************
Filename   : rvselectinternal.h
Description: internal address module definitions
             These definitions shouldn't be used outside of the
             address module directly
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

#ifndef _RV_SELECT_INTERNAL_H
#define _RV_SELECT_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************


            type definitions written in this file should not
            be accessed directly from outside the core. They
            may vary greatly between core versions!


 ********************************************************************/

#include "rvccore.h"
#include "rvtimer.h"
#include "rvthread.h"

#if (RV_NET_TYPE != RV_NET_NONE)

#define RV_SELECT_PREEMPTION_NONE 0 /* No preemption mechanism at all */
#define RV_SELECT_PREEMPTION_PIPE 1 /* Use a pipe as a preemption mechanism */
#define RV_SELECT_PREEMPTION_SOCKET 2 /* Use a socket as a preemption mechanism */
#define RV_SELECT_PREEMPTION_SYMBIAN 3 /* Use a Symbian specific preemption mechanism */
#define RV_SELECT_PREEMPTION_MOPI 4		/* Use Mopi's process messaging mechanism */

/* Check by interface which premption we need to use to stop blocking */

#if (RV_THREADNESS_TYPE == RV_THREADNESS_SINGLE)
/* No preemption when running single threaded */
#define RV_SELECT_PREEPEMTION_TYPE RV_SELECT_PREEMPTION_NONE

#elif (RV_OS_TYPE == RV_OS_TYPE_WIN32)
/* No need for preemption in Win32 */
#define RV_SELECT_PREEPEMTION_TYPE RV_SELECT_PREEMPTION_NONE

#elif (RV_OS_TYPE == RV_OS_TYPE_WINCE)  || (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)|| \
      (RV_OS_TYPE == RV_OS_TYPE_PSOS)   || (RV_OS_TYPE == RV_OS_TYPE_OSE)    || \
      (RV_OS_TYPE == RV_OS_TYPE_VXWORKS)
/* These operating systems don't support pipe() commands, so we have to use sockets instead */
#define RV_SELECT_PREEPEMTION_TYPE RV_SELECT_PREEMPTION_SOCKET

#elif (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN)
#define RV_SELECT_PREEPEMTION_TYPE RV_SELECT_PREEMPTION_SYMBIAN

#elif (RV_OS_TYPE == RV_OS_TYPE_MOPI)
#define RV_SELECT_PREEPEMTION_TYPE RV_SELECT_PREEMPTION_MOPI

#else
/* We can use a pipe() command */
#define RV_SELECT_PREEPEMTION_TYPE RV_SELECT_PREEMPTION_PIPE

#endif


#if ((RV_SELECT_TYPE == RV_SELECT_WIN32_WSA) || (RV_SELECT_TYPE == RV_SELECT_WIN32_COMPLETION))

#include <winsock2.h>

/* We use Windows specific values so we don't have to convert them */
#define RV_SELECT_READ       FD_READ
#define RV_SELECT_WRITE      FD_WRITE
#define RV_SELECT_ACCEPT     FD_ACCEPT
#define RV_SELECT_CONNECT    FD_CONNECT
#define RV_SELECT_CLOSE      FD_CLOSE



#elif (	(RV_SELECT_TYPE == RV_SELECT_SELECT)  || (RV_SELECT_TYPE == RV_SELECT_POLL) || \
    	(RV_SELECT_TYPE == RV_SELECT_DEVPOLL) || (RV_SELECT_TYPE == RV_SELECT_SYMBIAN))

#if (RV_OS_TYPE == RV_OS_TYPE_WINCE)
#include <winsock.h>

#elif (RV_OS_TYPE == RV_OS_TYPE_VXWORKS)
#include <time.h>
#include <types.h>
#include <selectLib.h>

#elif (RV_OS_TYPE == RV_OS_TYPE_PSOS)
#include <time.h>
#include <types.h>
#include <pna.h>

#elif (RV_OS_TYPE == RV_OS_TYPE_OSE)
#include <sys/time.h>
#include <sys/types.h>
#include <inet.h>

#elif (RV_OS_TYPE == RV_OS_TYPE_MOPI)
#include <enipsock.h>  
#include <esockth.h>

#elif (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)
#include <inc/nu_net.h>
/*#include <sys/select.h>*/

#elif (RV_OS_TYPE == RV_OS_TYPE_SOLARIS) || \
      (RV_OS_TYPE == RV_OS_TYPE_LINUX) || \
      (RV_OS_TYPE == RV_OS_TYPE_EMBLINUX) || \
      (RV_OS_TYPE == RV_OS_TYPE_TRU64) || \
      (RV_OS_TYPE == RV_OS_TYPE_UNIXWARE)
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#elif (RV_OS_TYPE == RV_OS_TYPE_HPUX)
#include <sys/time.h>
#endif

#if (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)
/* =========== NUCLEUS =========== */
#define RV_FD_ZERO	NU_FD_Init
#define RV_FD_SET	NU_FD_Set
#define RV_FD_ISSET NU_FD_Check
#define RV_FD_CLR	NU_FD_Reset
#define fd_set		FD_SET

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 2048

#elif (RV_OS_TYPE == RV_OS_TYPE_MOPI)
/* ============= MOPI ============ */

#define FD_SETSIZE          256 /* todo: check...*/
#define fd_set              sock_fd_set
#define timeval             bsd_timeval

int RV_FD_ISSET(SOCKET socket, const fd_set* fd);
void RV_FD_ZERO(fd_set* a);
void RV_FD_CLR(SOCKET socket, fd_set* fd);
void RV_FD_SET(SOCKET socket, fd_set* fd);

#else
/* ======== ALL OTHER OS's ======= */
#define RV_FD_ZERO FD_ZERO
#define RV_FD_SET FD_SET
#define RV_FD_ISSET FD_ISSET
#define RV_FD_CLR FD_CLR

#endif



#define RV_SELECT_READ       RvUint16Const(0x01)
#define RV_SELECT_WRITE      RvUint16Const(0x02)
#define RV_SELECT_ACCEPT     RvUint16Const(0x04)
#define RV_SELECT_CONNECT    RvUint16Const(0x08)
#define RV_SELECT_CLOSE      RvUint16Const(0x10)


#endif  /* RV_SELECT_TYPEs */


#if (RV_SELECT_TYPE == RV_SELECT_POLL)
#include <sys/poll.h>

#elif (RV_SELECT_TYPE == RV_SELECT_DEVPOLL)
#include <sys/ioctl.h>
#include <sys/poll.h>
#if (RV_OS_TYPE == RV_OS_TYPE_SOLARIS)
#include <sys/devpoll.h>
#else
#include <devpoll.h>
#endif

#endif





struct RvSelectFdInternal
{
    RvSocket        fd;               /* File descriptor for this event */
    RvSelectCb      callback;         /* Callback function - should be type casted when used */
    RvSelectEvents  events;           /* Events we're waiting on - needed for translation */
	RvBool          closedByTcpPeer;  /* TRUE if TCP peer sent shutdown request */

#if (RV_SELECT_TYPE == RV_SELECT_POLL)
    RvInt           fdArrayIndex; /* Index in fdArray of the select engine used by this fd */
#endif

#if (RV_SELECT_TYPE == RV_SELECT_DEVPOLL)
    short           devpollEvents; /* /dev/poll events we're using. Done here to reduce the
                                      number of writes to /dev/poll */
#endif
	RvLogMgr        *logMgr;       /* log manager instance. Important for case when two
								   stacks running in the same process use same select engine
								   but separate log managers */
    RvSelectFd		*nextInBucket; /* Next file descriptor in the same bucket */
};










/********************************************************************************************
 * RvSelectEngineInternal
 * Fd Events engine declaration. This struct's members should not be modified or accessed
 * directly by users of the core.
 ********************************************************************************************/
struct RvSelectEngineInternal
{
#if (RV_SELECT_TYPE == RV_SELECT_WIN32_WSA)
    HWND					hNetEventsWnd;		/* Network events window. All events are
												received by it
												Must not being changed after engine
												initialization. */
    RvBool					exitMessageLoop;	/* RV_TRUE if we want to exit the current
												message loop */
    HANDLE					threadHandle;		/* Thread handle of this fd engine.
												Must not being changed after engine
												initialization. */
    DWORD					threadId;			/* Thread ID handling this events engine.
												Must not being changed after engine
												initialization. */
#elif (RV_SELECT_TYPE == RV_SELECT_WIN32_COMPLETION)
    HANDLE					completionPort;		/* Handle of the completion port we use
												for this engine.
												Must not being changed after engine
												initialization. */
#elif (RV_SELECT_TYPE == RV_SELECT_SELECT)
    RvUint32				maxFd;				/* Maximum number of fd's supported.
												Must not being changed after engine
												initialization */
    RvUint32				maxFdInSelect;		/* Maximum fd set in the fdset's
												of the select() */
    fd_set					rdSet;				/* Read fd's we're currently waiting for */
    fd_set					wrSet;				/* Write fd's we're currently waiting for */
    RvBool					waiting;			/* RV_TRUE if we're blocked in a select()
												call */
	RvUint64				nsecTimeout;		/* timeout required by rvtimer module */
	RvThread				*selectThread;		/* Thread where select was used last time */

#if (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)
    fd_set					simulatedRdSet;		/* Read fd's that were simulated */
    fd_set					simulatedWrSet;		/* Write fd's that were simulated */
    RvUint32				simulatedNumFd;		/* number of fd's that were simulated in simulated sets */
    RvLock					simulatedSetLock;	/* just to make sure we wont clear any new events */
#endif /* (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS) */

#elif (RV_SELECT_TYPE == RV_SELECT_POLL)
    RvUint32				maxFd;				/* Maximum number of fd's supported.
												Must not being changed after engine
												initialization */
    RvUint32				maxFdInPoll;		/* Number of file descriptors being polled */
    struct pollfd*			fdArray;			/* File descriptors we're polling */
	struct pollfd*			tmpFdArray;			/* temporrary file descriptors array,
												used to call 'poll' without locking
												select engine */
    RvBool					waiting;			/* RV_TRUE if we're blocked in a poll() call*/
	RvUint64				nsecTimeout;		/* timeout required by rvtimer module */
	RvThread				*selectThread;		/* Thread where select was used last time */

#elif (RV_SELECT_TYPE == RV_SELECT_DEVPOLL)
    RvUint32				maxFd;				/* Maximum number of fd's supported
												Must not being changed after engine
												initialization. */
    RvUint32				maxFdInDevPoll;		/* Number of FDs currently used. */
    int						fdDevPoll;			/* /dev/poll's file descriptor */
    struct pollfd*			fdArray;			/* File descriptors we're polling */
	struct pollfd*			tmpFdArray;			/* temporrary file descriptors array,
												used to call 'poll' without locking
												select engine */
    RvBool					waiting;			/* RV_TRUE if we're blocked in a /dev/poll ioctl() call */
	RvUint64				nsecTimeout;		/* timeout required by rvtimer module */
	RvThread				*selectThread;		/* Thread where select was used last time */
    RvSemaphore             devPollWrite;       /* used as a mutex to synchronize
                                                manipulation of /dev/poll */
#elif (RV_SELECT_TYPE == RV_SELECT_SYMBIAN)
	void					*to;				/* Time out active object, avtual type is pointer to class RvSymSelectTO */
	RvUint32 				preemptionMessage;  /* notifies that thread where main loop is running required preemption */
#endif

    /* Bucket-hash parameters */
    RvUint32				maxHashSize;		/* Size of the bucket hash to use */
    RvSelectFd**			hash;				/* Bucket hash to use for the file descriptors */
	RvSelectPreemptionCb	preemptionCb;		/* Callback used to notify stack about non
												RvSelectEmptyPreemptMsg preemption events
												Must not being changed after engine
												initialization. */
	void					*preemptionUsrCtx; /* User context used to call the preemptionCd
												Must not being changed after engine
												initialization. */

#if (RV_SELECT_PREEPEMTION_TYPE == RV_SELECT_PREEMPTION_SOCKET) || \
    (RV_SELECT_PREEPEMTION_TYPE == RV_SELECT_PREEMPTION_PIPE)
    RvSelectFd				preemptionFd;		/* Information about the pipe's read file descriptor
												Must not being changed after engine
												initialization. */
#endif

#if (RV_SELECT_PREEPEMTION_TYPE == RV_SELECT_PREEMPTION_SOCKET)
    RvSocket				preemptionSocket;	/* Socket used to preempt a select() call of the thread
												Must not being changed after engine
												initialization. */
    RvAddress				localAddress;		/* Address we're listening on for preemption.
												Must not being changed after engine
												initialization. */

#elif (RV_SELECT_PREEPEMTION_TYPE == RV_SELECT_PREEMPTION_PIPE)
    int						preemptionPipeFd[2]; /* Pipe used to preempt a select() call of the thread
												Must not being changed after engine
												initialization. */

#elif (RV_SELECT_PREEPEMTION_TYPE == RV_SELECT_PREEMPTION_MOPI)
	RvUint32				mopiProcessId;		/* process ID to which preemption messages 
												   will be sent */
#endif

	RvInt32                 usageCnt;           /* select engine usage counter */
	RvTimerQueue			tqueue;			    /* timer queue that asked for the last timeout */
	RvSize_t                maxTimers;          /* maximum required number of timers.
												tqueue size may be more than the maxTimers.*/
	RvTimerFunc		        timeOutCb;			/* callback, applied when timeout event occures */
	void		            *timeOutCbContext;	/* callback user data */
	RvLogMgr*				logMgr;				/* log manager used to print log from the select module.
												Must not being changed after engine
												initialization. */
    RvLock					lock;				/* Select engine instance protection lock.
												Must not being changed after engine
												initialization. */
	RvUint64                wakeupTime;         /* Absolute time when select should be waked up */
};


#ifdef __cplusplus
}
#endif

#endif /* (RV_NET_TYPE != RV_NET_NONE) */

#endif  /* _RV_SELECT_INTERNAL_H */
