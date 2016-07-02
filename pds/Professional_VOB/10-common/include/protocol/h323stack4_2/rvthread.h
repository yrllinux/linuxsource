/***********************************************************************
Filename   : rvthread.h
Description: rvthread header file
************************************************************************
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

/***********************************************************************
This module provides functions for creating and manipulating threads.
Threads need to be constructed, created, and then started. Between being
constructed and created, parameters such as stack size and special attributes
may be set. The only parameter that may be changed after creating a thread is
the priority.
Pre-existing threads may use the RvThreadConstructFromUserThread function to
create a thread handle so that thread specific variables and other functions
which require a thread handle can be used. Since these threads are already
running, attributes of these threads can not be changed and some OS's do not
even allow the priority to be changed. Destructing a thread created in this
manner does not delete the thread itself, it just returns it to the state
it was in when RvThreadConstructFromUserThread was called. In addition
RvThreadDestruct MUST be called before such a thread is allowed to exit.
Functions which don't require a thread handle (except RvThreadCurrent) may
be called from any thread, even if a handle was not constructed.
***********************************************************************/

#ifndef RV_THREAD_H
#define RV_THREAD_H

#include "rvccore.h"
#include "rvlock.h"
#include "rvsemaphore.h"
#include "rvtime.h"
#include "rvlog.h"

/* Error checks to make sure configuration has been done properly */
#if !defined(RV_THREAD_TYPE) || ((RV_THREAD_TYPE != RV_THREAD_POSIX) && \
    (RV_THREAD_TYPE != RV_THREAD_SOLARIS) && (RV_THREAD_TYPE != RV_THREAD_VXWORKS) && \
    (RV_THREAD_TYPE != RV_THREAD_PSOS) && (RV_THREAD_TYPE != RV_THREAD_OSE) && \
    (RV_THREAD_TYPE != RV_THREAD_NUCLEUS) && (RV_THREAD_TYPE != RV_THREAD_WIN32) && \
    (RV_THREAD_TYPE != RV_THREAD_WINCE) && (RV_THREAD_TYPE != RV_THREAD_UNIXWARE) && \
    (RV_THREAD_TYPE != RV_THREAD_SYMBIAN) && (RV_THREAD_TYPE != RV_THREAD_MOPI) && \
	(RV_THREAD_TYPE != RV_THREAD_NONE))
#error RV_THREAD_TYPE not set properly
#endif


#if (RV_THREAD_TYPE != RV_THREAD_NONE)

#if !defined(RV_THREAD_PRIORITY_DEFAULT)
#error RV_THREAD_PRIORITY_DEFAULT not set properly
#endif

#if !defined(RV_THREAD_STACKSIZE_DEFAULT)
#error RV_THREAD_STACKSIZE_DEFAULT not set properly
#endif

#if !defined(RV_THREAD_STACKSIZE_USEDEFAULT)
#error RV_THREAD_STACKSIZE_USEDEFAULT not set properly
#endif

#if !defined(RV_THREAD_ATTRIBUTE_DEFAULT)
#error RV_THREAD_ATTRIBUTE_DEFAULT not set properly
#endif

#if !defined(RV_THREAD_MAX_NAMESIZE) || (RV_THREAD_MAX_NAMESIZE < 1)
#error RV_THREAD_MAX_NAMESIZE not set properly
#endif

#if !defined(RV_THREAD_MAX_VARS) || (RV_THREAD_MAX_VARS < 1)
#error RV_THREAD_MAX_VARS not set properly
#endif

#if !defined(RV_THREAD_MAX_VARNAMESIZE) || (RV_THREAD_MAX_VARNAMESIZE < 1)
#error RV_THREAD_MAX_VARNAMESIZE not set properly
#endif

#endif /* (RV_THREAD_TYPE != RV_THREAD_NONE) */

/* End of configuration error checks */

/* Module specific error codes (-512..-1023). See rverror.h dor more details */
#define RV_THREAD_ERROR_RUNNING -512 /* Thead is running and cannot be destructed or started */
#define RV_THREAD_ERROR_NOTSTARTED -513 /* Thead has never been started */
#define RV_THREAD_ERROR_CREATED -514 /* Thread has already been created */
#define RV_THREAD_ERROR_NOTCREATED -515 /* Thread has not been created */
#define RV_THREAD_ERROR_DESTRUCTING -516 /* Another thread is already destructing this one */
#define RV_THREAD_ERROR_NOVARS -517 /* No more thread specific variable available */
#define RV_THREAD_ERROR_BADTHREAD -518 /* Current thread never constructed */
#define RV_THREAD_ERROR_USERAPP -519 /* Operation not allowed on user application thread */

/* Module specific Warning codes (512..1023). See rverror.h for more details */
#define RV_THREAD_WARNING_NOTFOUND 512 /* Variable find operation did not find a match */


/* Get include files and define types for each OS. RvThreadId, */
/* RvThreadAttr, and RvThreadBlock need to be defined along */
/* with priority range and direction. RvThreadBlock is only used */
/* internally. */
#if (RV_THREAD_TYPE == RV_THREAD_NONE)

typedef RvInt RvThreadBlock;

/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef RvInt RvThreadId;

/********************************************************************************************
 * RvThreadAttr
 * OS specific attributes and options used for threads. See definitions in rvthread.h
 * along with the default values in rvccoreconfig.h for more information.
 ********************************************************************************************/
typedef struct
{
    RvInt unused;
} RvThreadAttr;
#define RV_THREAD_PRIORITY_MAX 0
#endif

#if (RV_THREAD_TYPE == RV_THREAD_POSIX) || (RV_THREAD_TYPE == RV_THREAD_SOLARIS) || \
    (RV_THREAD_TYPE == RV_THREAD_UNIXWARE)
#include <pthread.h>
typedef pthread_attr_t RvThreadBlock; /* We use block for attributes since we don't want them touched directly */

/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef pthread_t RvThreadId;

/********************************************************************************************
 * RvThreadAttr
 * OS specific attributes and options used for threads. See definitions in rvthread.h
 * along with the default values in rvccoreconfig.h for more information.
 ********************************************************************************************/
typedef struct {
    /* These correspond to attributes in the pthread_attr struct that we let users set */
    int contentionscope;
    int policy;
    int inheritsched;
#if (RV_THREAD_TYPE == RV_THREAD_SOLARIS)
    size_t guardsize; /* Solaris extention to standard attributes (use (size_t)(-1) for default*/
#endif
} RvThreadAttr;
#define RV_THREAD_PRIORITY_MAX 0
#define RV_THREAD_PRIORITY_MIN 20
#define RV_THREAD_PRIORITY_INCREMENT (-1)
#endif

#if (RV_THREAD_TYPE == RV_THREAD_VXWORKS)
#include <vxWorks.h>
#include <taskLib.h>
typedef WIND_TCB RvThreadBlock;

/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef int RvThreadId;

/********************************************************************************************
 * RvThreadAttr
 * OS specific attributes and options used for threads. See definitions in rvthread.h
 * along with the default values in rvccoreconfig.h for more information.
 ********************************************************************************************/
typedef int RvThreadAttr; /* options parameter to taskInit */
#define RV_THREAD_PRIORITY_MAX 2
#define RV_THREAD_PRIORITY_MIN 254
#define RV_THREAD_PRIORITY_INCREMENT (-1)
#endif

#if (RV_THREAD_TYPE == RV_THREAD_PSOS)
#include <psos.h>

/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef unsigned long RvThreadId;

/********************************************************************************************
 * RvThreadAttr
 * OS specific attributes and options used for threads. See definitions in rvthread.h
 * along with the default values in rvccoreconfig.h for more information.
 ********************************************************************************************/
typedef struct {
    unsigned long flags; /* t_create flags */
    unsigned long mode;  /* t_start mode */
    unsigned long tslice; /* tslice for t_tslice */
} RvThreadAttr;
#if (RV_OS_VERSION == RV_OS_PSOS_2_0)
typedef void* RvThreadBlock;
#else
/* Diab tool - for PowerPC */
typedef struct tinfo RvThreadBlock; /* pSOS doesn't give any access to actual TCB */
#endif
#define RV_THREAD_PRIORITY_MAX 240
#define RV_THREAD_PRIORITY_MIN 1
#define RV_THREAD_PRIORITY_INCREMENT 1
#endif

#if (RV_THREAD_TYPE == RV_THREAD_OSE)
#include <ose.h>
typedef PROCESS *RvThreadBlock;

/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef PROCESS RvThreadId;

/********************************************************************************************
 * RvThreadAttr
 * OS specific attributes and options used for threads. See definitions in rvthread.h
 * along with the default values in rvccoreconfig.h for more information.
 ********************************************************************************************/
typedef struct { /* parameters for create_process */
    enum PROCESS_TYPE proc_type;
    OSTIME timeslice;
    PROCESS block;
    struct OS_redir_entry *redir_table;
    OSVECTOR vector;
    OSUSER user;
} RvThreadAttr;
#define RV_THREAD_PRIORITY_MAX 1
#define RV_THREAD_PRIORITY_MIN 31
#define RV_THREAD_PRIORITY_INCREMENT (-1)
#endif

#if (RV_THREAD_TYPE == RV_THREAD_NUCLEUS)
#include <nucleus.h>
#if defined(RV_DEBUG)
typedef NU_TASK RvThreadBlock;
/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef NU_TASK *RvThreadId;
#else
#include <tc_defs.h>
typedef TC_TCB RvThreadBlock;
/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef TC_TCB *RvThreadId;
#endif

/********************************************************************************************
 * RvThreadAttr
 * OS specific attributes and options used for threads. See definitions in rvthread.h
 * along with the default values in rvccoreconfig.h for more information.
 ********************************************************************************************/
typedef struct { /* parameters for NU_Create_task */
    UNSIGNED time_slice;
    OPTION preempt;
} RvThreadAttr;
#define RV_THREAD_PRIORITY_MAX 4
#define RV_THREAD_PRIORITY_MIN 255
#define RV_THREAD_PRIORITY_INCREMENT (-1)
#endif


#if (RV_THREAD_TYPE == RV_THREAD_MOPI)
    /* although we compile SIP stack in thread-safe mode, we do not need thread control
       functions, since SIP will run only in single thread with MOPI */

/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef T_MOPI_CepId RvThreadId;
typedef int RvThreadAttr; /* options parameter to taskInit */
typedef RvInt RvThreadBlock;

#endif


#if (RV_THREAD_TYPE == RV_THREAD_WIN32) || (RV_THREAD_TYPE == RV_THREAD_WINCE)
typedef HANDLE RvThreadBlock; /* use the tcb as the handle reference */

/********************************************************************************************
 * RvThreadId
 * An OS specific thread ID. Used to identify threads regardless
 * of whether or not a thread handle has been constructed for it.
 ********************************************************************************************/
typedef DWORD RvThreadId;

/********************************************************************************************
 * RvThreadAttr
 * OS specific attributes and options used for threads. See definitions in rvthread.h
 * along with the default values in rvccoreconfig.h for more information.
 ********************************************************************************************/
typedef struct {
    BOOL disablepriorityboost; /* value for SetThreadPriorityBoost (Win NT 4.0 or newer only) */
    DWORD affinitymask;        /* value for SetThreadAffinityMask */
    DWORD idealprocessor;      /* value for SetThreadIdealProcessor (Win NT 4.0 or newer only) */
} RvThreadAttr;
#define RV_THREAD_PRIORITY_MAX 2
#define RV_THREAD_PRIORITY_MIN (-2)
#define RV_THREAD_PRIORITY_INCREMENT 1
#endif

#if (RV_THREAD_TYPE == RV_THREAD_SYMBIAN)
typedef void* RvThreadBlock;
typedef int RvThreadId;
typedef int RvThreadAttr; /* options parameter to taskInit */
#define RV_THREAD_PRIORITY_MAX 500
#define RV_THREAD_PRIORITY_MIN 100
#define RV_THREAD_PRIORITY_INCREMENT 100
#endif

/* Forward declaration */
typedef struct RvThread_s RvThread;

/********************************************************************************************
 * RvThreadFunc
 * This is the function that should be called as the main function of a thread.
 * PARAMS: th - Pointer to the thread structure of the current thread.
 *         data - User parameter that was passed to RvThreadConstruct.
 ********************************************************************************************/
typedef void (*RvThreadFunc)(RvThread *, void *);

/********************************************************************************************
 * RvThreadVarFunc
 * This callback is called when a thread which has a thread specific variable
 * associated with it exits. This callback is set when creating the thread
 * specific variable.
 * PARAMS: th - Pointer to the thread structure of the current thread.
 *         data - The current value of the thread specific variable for this thread.
 *         index - The index of the thread specific variable itself.
 * NOTE  :  The callback will execute within thread that is exiting.
 *          For threads constructed with RvThreadConstructFromUserThread, the callback will
 *          actually occur when the thread calls RvThreadDestruct.
 *          The callback function may not make calls to RvThreadConstruct, RvThreadDestruct,
 *          RvThreadConstructFromUserThread, RvThreadCreateVar, or RvThreadDeleteVar.
 ********************************************************************************************/
typedef void (*RvThreadVarFunc)(RvThread *, void *, RvUint32 index);


/* Typedef of RvThread is above. */
/********************************************************************************************
 * RvThread
 * A thread object.
 ********************************************************************************************/
struct RvThread_s {
    RvInt32 state;                        /* Current thread state */
    RvChar name[RV_THREAD_MAX_NAMESIZE];  /* Full name of thread */
    void *stackaddr;                      /* pointer to memory for thread's stack space */
    void *stackstart;                     /* actual pointer to start of stack (told to OS) */
    RvInt32 reqstacksize;                 /* requested size of stack */
    RvInt32 realstacksize;                /* size of stack told to the OS (starting at stackstart) */
    RvInt32 stacksize;                    /* actual size of stack memory (pointed to by stackaddr) */
    RvBool stackallocated;                /* set to RvTrue if stack was allocated internally */
    RvBool waitdestruct;                  /* set to RvTrue if destruct is waiting for thread to exit */
    RvLock datalock;                      /* lock for access to thread structure */
    RvSemaphore exitsignal;               /* used to signal thread completion */
    RvThreadFunc func;                    /* function to call in thread */
    void *data;                           /* data parameter to be passed to func */
    RvInt32 priority;                     /* priority of thread */
    RvThreadAttr attr;                    /* OS specific thread attributes */
    RvThreadId id;                        /* OS id of thread */
    RvThreadBlock tcb;                    /* OS specific task control block or pointer to it */
    RvBool autodelete;                    /* Are we automatically deleting this thread if we found out it was stopped? Default is RV_FALSE */
    RvThreadFunc exitfunc;                /* function to call when thread exits */
    void *exitdata;                       /* data parameter to be passed to exitfunc */
	void* logMgr;                         /* Log manager of the current instance */
	RvLogSource* threadSource;		      /* log source, to be used for the thread module instance */
    void *vars[RV_THREAD_MAX_VARS];       /* Values of thread specific variables */
};

#define RvThreadGetLogManager(th) (th != NULL ? (RvLogMgr*)((th)->logMgr) : NULL)


#if defined(__cplusplus)
extern "C" {
#endif


/********************************************************************************************
 * RvThreadInit
 * Initializes the Thread module. Must be called once (and
 * only once) before any other functions in the module are called.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RvStatus Rv2ThreadInit(void);

/********************************************************************************************
 * RvThreadEnd
 * Shuts down the Thread module. Must be called once (and
 * only once) when no further calls to this module will be made.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RvStatus Rv2ThreadEnd(void);

RvStatus Rv2ThreadSourceConstruct(RvLogMgr* logMgr);

/********************************************************************************************
 * RvThreadConstruct
 * Constructs a thread object. No actual thread is created yet. All settings
 * for this thread use the defaults defined in rvccoreconfig.h so that it is
 * not necessary to set every parameter if the default ones are acceptable for
 * this thread.
 * INPUT   : func - Pointer to function which will executed in the new thread.
 *           data - User defined data which will be passed to func when it is executed.
 * OUTPUT  : th - Pointer to thread object to be constructed.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadConstruct(
    IN RvThreadFunc func, 
    IN void *data, 
    IN RvLogMgr* logMgr, 
    OUT RvThread *th);

/********************************************************************************************
 * RvThreadConstructFromUserThread
 * Constructs a thread object and associates it with an existing thread that
 * was created externally (not using RvThreadConstruct). It should be called
 * from the thread itself. Threads which have made this call must call
 * RvThreadDestruct before exiting.
 * INPUT   : None.
 * OUTPUT  : th - Pointer to thread object to be constructed.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadConstructFromUserThread(
    IN RvLogMgr* logMgr, 
    OUT RvThread *th);

/********************************************************************************************
 * RvThreadDestruct
 * Destruct a thread object. Threads can not be destructed until they have
 * exited so if the thread has not exited this function will wait until
 * it does before returning.
 * Threads created with RvThreadConstructFromUserThread are a special case
 * where RvThreadDestruct MUST be called from the thread itself before
 * exiting (and obviously won't wait for the thread to exit).
 * INPUT   : th - Pointer to thread object to be destructed.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : RvThreadDestruct may only be called once on each thread. Thus
 *           it may not be called simultaneously from multiple threads (with the
 *           same thread to destruct).
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadDestruct(
    IN RvThread *th);

/********************************************************************************************
 * RvThreadCreate
 * Create the actual thread. The thread will be created by the OS and
 * allocate all needed resources but will not begin executing.
 * INPUT   : th - Pointer to thread object to be destructed.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : Some operating systems do not allow threads to be started in the
 *           suspended state so the physical thread will not appear until
 *           RvThreadStart is called.}
 *           If the call fails, RvThreadDestruct should be called in order to properly clean up.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadCreate(
    IN RvThread *th);

/********************************************************************************************
 * RvThreadStart
 * Start thread execution.
 * INPUT   : th - Pointer to thread object to be destructed.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : If the call fails, RvThreadDestruct should be called in order to properly clean up.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadStart(
    IN RvThread *th);

/********************************************************************************************
 * RvThreadSetExitFunc
 * INPUT   : func - Pointer to function which will executed when thread exits.
 *           data - User defined data which will be passed to func when it is executed.
 *           th   - Pointer to thread object.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSetExitFunc(
    IN RvThread *th,
    IN RvThreadFunc func, 
    IN void *data);

/********************************************************************************************
 * RvThreadCurrent
 * Get the thread handle of the current thread.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : A pointer to the thread object of the current thread.
 * NOTE    : If the thread was not created with this Thread module or was
 *           not attach to a thread object with RvThreadConstructFromUserThread
 *           then this function will return NULL.
 */
RVCOREAPI 
RvThread * RVCALLCONV Rv2ThreadCurrent(void);

/********************************************************************************************
 * RvThreadCurrentId
 * Get the OS specific thread ID of the current thread.
 * INPUT   : None.
 * OUTPUT  : None.
 * RETURN  : The thread ID of the current thread.
 * NOTE    : This works for all threads regardless of how they were created.
 */
RVCOREAPI 
RvThreadId RVCALLCONV Rv2ThreadCurrentId(void);

/********************************************************************************************
 * RvThreadGetId
 * Get the OS specific thread ID of a given thread.
 * INPUT   : th - Pointer to thread object.
 * OUTPUT  : None.
 * RETURN  : The thread ID of the thread.
 * NOTE    : This works for all threads regardless of how they were created.
 */
RVCOREAPI
RvThreadId RVCALLCONV Rv2ThreadGetId(
    IN RvThread *th);

/********************************************************************************************
 * RvThreadIdEqual
 * Compares two thread IDs.
 * INPUT   : id1 - Thread ID.
 *           id2 - Thread ID.
 * OUTPUT  : None.
 * RETURN  : RV_TRUE if threads are the same, otherwise RV_FALSE.
 */
RVCOREAPI 
RvBool RVCALLCONV Rv2ThreadIdEqual(
    IN RvThreadId id1, 
    IN RvThreadId id2);

/********************************************************************************************
 * RvThreadSleep
 * Suspends the current thread for the requested amount of time.
 * INPUT   : t - Pointer to RvTime structure containing amount of time to sleep.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : This works for all threads regardless of how they were created.
 *           The exact time of suspension is based on the operating system and the
 *           resolution of the system clock.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSleep(
    IN const RvTime *t,
    IN RvLogMgr* logMgr);

/********************************************************************************************
 * RvThreadNanosleep
 * Suspends the current thread for the requested amount of time.
 * INPUT   : nsecs - Time to sleep in nanoseconds.
 *			 logMgr - log manager structure
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : This works for all threads regardless of how they were created.
 *           The exact time of suspension is based on the operating system and the
 *           resolution of the system clock.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadNanosleep( 
    IN RvInt64 nsecs,
	IN RvLogMgr* logMgr);

/********************************************************************************************
 * RvThreadGetOsName
 * Gets the name of a thread as seen by the operating system.
 * INPUT   : id - Thread ID.
 *           buf - Pointer to buffer where the name will be copied.
 *           size - Size of the buffer.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : This works for all threads regardless of how they were created.
 *           If the buffer is too small for the name the name will be truncated.
 *           Only works on threads that exist (are executing).
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadGetOsName(
    IN RvThreadId id, 
    IN RvInt32 size,
    OUT RvChar *buf);

/********************************************************************************************
 * RvThreadGetName
 * Gets the name of a thread.
 * INPUT   : th - Pointer to thread object to get the name of.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : The pointer returned is only valid until the thread is destructed.
 */
RVCOREAPI 
RvChar * RVCALLCONV Rv2ThreadGetName(
    IN RvThread *th);

/********************************************************************************************
 * RvThreadSetName
 * Sets the name of a thread.
 * INPUT   : th - Pointer to thread object to set the name of.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : A copy of the string passed in is made. The maximum size of the
 *           the string is RV_THREAD_MAX_NAMESIZE and names that are too long
 *           will be truncated.
 *           The name can not be changed once a thread has been created.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSetName(
    IN RvThread *th, 
    IN const RvChar *name);

/********************************************************************************************
 * RvThreadSetAutoDelete
 * Auto-deletion parameter cannot be changed after thread is created. This function should
 * only be called for threads constructed with RvThreadConstructFromUserThread().
 * INPUT   : th - Pointer to thread object to set the name of.
 *           autoDelete - RV_TRUE to allow autoDelete, otherwise RV_FALSE.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSetAutoDelete(
    IN RvThread *th, 
    IN RvBool autoDelete);

/********************************************************************************************
 * RvThreadSetStack
 * Sets the stack information for a thread.
 * INPUT   : th - Pointer to thread object to set the stack information for.
 *           stackaddr - Address of memory to use for stack (NULL means to allocate it).
 *           stacksize - Size of the stack.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : The stack information can not be changed once a thread has been created.
 *           If stackaddr is set to NULL, the amount of stack space used for internal overhead
 *           will be allocated in addition to the requested stack size.
 *           If stackaddr is set to NULL and the stacksize is set to 0, the default
 *           stack settings will be used (see rvccoreconfig.h).
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSetStack(
    IN RvThread *th, 
    IN void *stackaddr, 
    IN RvInt32 stacksize);

/********************************************************************************************
 * RvThreadSetPriority
 * Sets the stack priority for a thread.
 * INPUT   : th - Pointer to thread object to set the stack priority of.
 *           priority - Priority to set thread to.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSetPriority(
    IN RvThread *th, 
    IN RvInt32 priority);

/********************************************************************************************
 * RvThreadGetAttr
 * Gets the current attributes for a thread.
 * INPUT   : th - Pointer to thread object to get the attributes of.
 *           attr - Pointer to OS specific thread attributes structure where the values will be copied.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : Information about the attributes structure and its settings can be found in
 *           the rvccoreconfig.h and rvthread.h file.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadGetAttr(
    IN RvThread *th, 
    OUT RvThreadAttr *attr);

/********************************************************************************************
 * RvThreadSetAttr
 * Sets the attributes for a thread.
 * INPUT   : th - Pointer to thread object to set the attributes of.
 *           attr - Pointer to OS specific thread attributes to begin using (NULL = use defaults).
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : The attributes can not be changed once a thread has been created.
 *           The default values for these attributes are set in rvccoreconfig.h. 
 *           Further information about these attributes can be found in that file and
 *           the rvthread.h file.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSetAttr(
    IN RvThread *th, 
    IN const RvThreadAttr *attr);

/********************************************************************************************
 * RvThreadCreateVar
 * Creates a thread specific variable for all threads.
 * INPUT   : exitfunc - Pointer to a function which will be called for this variable when a thread exits (NULL = none).
 *           name - Pointer to a name string to identify the variable (NULL = no name).
 *           index - Pointer to a location to store the index of the variable created.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : Only threads constructed with RvThreadConstruct or RvThreadConstructFromUserThread
 *           may use thread specific variables.
 *           For threads created with RvThreadConstructFromUserThread, the exit function
 *           will be called when the thread calls RvThreadDestruct.
 *           The maximum number of thread specific variables is RV_THREAD_MAX_VARS and is
 *           configured in the rvccoreconfig.h file.
 *           A copy of the name string passed in is made. The maximum size of the
 *           the string is RV_THREAD_MAX_VARNAMESIZE and names that are too long
 *           will be truncated.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadCreateVar(
    IN RvThreadVarFunc exitfunc, 
    IN const RvChar *name, 
    IN RvLogMgr* logMgr, 
    OUT RvUint32 *index);

/********************************************************************************************
 * RvThreadDeleteVar
 * Deletes a thread specific variable for all threads.
 * INPUT   : exitfunc - Pointer to a function which will be called for this variable when a thread exits (NULL = none).
 *           index - Index of thread specific variable to be deleted.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : Exit functions for the variable will NOT be called.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadDeleteVar(
    IN RvUint32 index, 
    IN RvLogMgr* logMgr);

/********************************************************************************************
 * RvThreadSetVar
 * Sets the value of a thread specific variable for the current thread.
 * INPUT   : index - Index of thread specific variable to be set.
 *           value - Value to set variable to.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : Only threads constructed with RvThreadConstruct or RvThreadConstructFromUserThread
 *           may use thread specific variables.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadSetVar(
    IN RvUint32 index, 
    IN void *value, 
    IN RvLogMgr* logMgr);

/********************************************************************************************
 * RvThreadGetVar
 * Gets the value of a thread specific variable for the current thread.
 * INPUT   : index - Index of thread specific variable to be retrieved.
 *           value - Pointer to a location to store the current value of the variable.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : Only threads constructed with RvThreadConstruct or RvThreadConstructFromUserThread
 *           may use thread specific variables.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadGetVar(
    IN RvUint32 index, 
    IN RvLogMgr* logMgr, 
    OUT void **value);

/********************************************************************************************
 * RvThreadFindVar
 * Finds the index of a thread specific variable by its name.
 * INPUT   : name - Pointer to a name string of the variable to find.
 *           index - Pointer to a location to store the index of the variable found.
 * OUTPUT  : None.
 * RETURN  : RV_OK if successful otherwise an error code.
 * NOTE    : If multiple variables have the same name, the first one found
 *           will be returned.
 */
RVCOREAPI 
RvStatus RVCALLCONV Rv2ThreadFindVar(
    IN const RvChar *name, 
    IN RvLogMgr* logMgr, 
    OUT RvUint32 *index);

/********************************************************************************************
 * RvThreadJoin
 * Finds the index of a thread specific variable by its name.
 * INPUT   : th - Pointer to thread object to be 
 * OUTPUT  : *pResult FALSE if thread calls to RvThreadJoin to itself
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2ThreadJoin(
	IN RvThread *th, 
	OUT RvBool *pResult);


#if (RV_OS_TYPE == RV_OS_TYPE_MOPI)
RVCOREAPI
RvThread * RVCALLCONV Rv2ThreadGetSpecificThreadPtr(MOPI_EMB_PROC proc);

#endif

#if defined(__cplusplus)
}
#endif

#endif
