/***********************************************************************
Filename   : rvloglistener.h
Description: rvloglistener header file - message log listener
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

#ifndef RV_LOG_LISTENER_H
#define RV_LOG_LISTENER_H


#include "rvccore.h"
#include "rvstdio.h"
#include "rvlog.h"


/* Error checks to make sure configuration has been done properly */
#if !defined(RV_LOGLISTENER_TYPE) || ((RV_LOGLISTENER_TYPE != RV_LOGLISTENER_WIN32) && \
    (RV_LOGLISTENER_TYPE != RV_LOGLISTENER_FILE_AND_TERMINAL) && \
    (RV_LOGLISTENER_TYPE != RV_LOGLISTENER_TERMINAL) && \
    (RV_LOGLISTENER_TYPE != RV_LOGLISTENER_NONE))
#error RV_LOGLISTENER_TYPE not set properly
#endif
/* End of configuration error checks */



#if ((RV_LOGLISTENER_TYPE == RV_LOGLISTENER_WIN32) || \
    (RV_LOGLISTENER_TYPE == RV_LOGLISTENER_FILE_AND_TERMINAL))
typedef struct
{
#if (RV_LOGLISTENER_TYPE == RV_LOGLISTENER_WIN32)
    HANDLE      openedFile; /* Handle of the currently opened file */
#else
    RvFILE*     openedFile; /* Handle of the currently opened file */
#endif
    RvChar      baseFilename[256]; /* Base name of files */
    RvUint32    numFiles; /* Number of files in cycle */
    RvUint32    curFileNum; /* Current file number being used */
    RvUint32    maxFileSize; /* Maximum size of file in cycle */
    RvUint32    curSize; /* Current size of file */
    RvBool      flushLines; /* If RV_TRUE, then on each message, the logfile is flushed */
    RvBool      openError; /* Indication of errors - on error, we stop logging to the log files */
#if (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN) && (RV_THREADNESS_TYPE == RV_THREADNESS_MULTI)
    RvChar* logMessage;
    RvUint32 logMessageSize;
    void* logThread;  /* RvThread    */
    void* message;    /* RvSemaphore */
    void* logIsReady; /* RvSemaphore */
#endif
} LogFileListener;
#endif

/********************************************************************************************
 * RvLogListener
 * Holds information about the listeners supported by the core.
 * This includes only the type of the listener.
 * We allow only one type of each listener to be constructed - this is only the case for the
 * listeners that the core implements. Applications can write their own listeners and use
 * them as desired.
 ********************************************************************************************/
#if (RV_LOGLISTENER_TYPE != RV_LOGLISTENER_NONE)
typedef struct
{
    RvLogMgr*       logMgr; /* Log manager this listener uses */
    int             listenerType; /* Type of listener constructed */
#if ((RV_LOGLISTENER_TYPE == RV_LOGLISTENER_WIN32) || \
    (RV_LOGLISTENER_TYPE == RV_LOGLISTENER_FILE_AND_TERMINAL))
	LogFileListener logFileListener;
#endif
} RvLogListener;

#else

/* Dummy declaration for a log listener, since we're not supporting any */
typedef RvInt RvLogListener;

#endif  /* (RV_LOGLISTENER_TYPE != RV_LOGLISTENER_NONE) */



#if defined(__cplusplus)
extern "C" {
#endif

/********************************************************************************************
 * RvLogListenerInit - inits log listener module
 *
 * This function should be called only once in the process
 *
 * INPUT   : none
 * OUTPUT  : None.
 * RETURN  : always RV_OK
 */
RvStatus Rv2LogListenerInit(void);

/********************************************************************************************
 * RvLogListenerEnd - shut down log listener module
 *
 * This function should be called only once in the process
 *
 * INPUT   : none
 * OUTPUT  : None.
 * RETURN  : always RV_OK
 */
RvStatus Rv2LogListenerEnd(void);


#if (RV_LOGLISTENER_TYPE != RV_LOGLISTENER_NONE)

/********************************************************************************************
 * RvLogListenerConstructTerminal
 *
 * Construct a log listener that sends log messages to the terminal, using
 * standard output or standard error
 *
 * INPUT   : listener   - Listener to construct
 *           logMgr     - Log manager to listen to
 *           stdOut     - RV_TRUE for stdout, RV_FALSE for stderr
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other values on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogListenerConstructTerminal(
    IN  RvLogListener*  listener,
    IN  RvLogMgr*       logMgr,
    IN  RvBool          stdOut);


/********************************************************************************************
 * RvLogListenerDestructTerminal - Destruct the terminal listener
 *
 * INPUT   : listener   - Listener to destruct
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other values on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogListenerDestructTerminal(
    IN  RvLogListener*  listener);


/********************************************************************************************
 * RvLogListenerConstructLogfile
 *
 * Construct a log listener that sends log messages to a file
 *
 * INPUT   : listener           - Listener to construct
 *           logMgr             - Log manager to listen to
 *           fileName           - Name of the logfile
 *           numFiles           - Number of cyclic files to use
 *           fileSize           - Size of each file in cycle in bytes
 *                                This parameter is only applicable if numFiles > 1
 *           flushEachMessage   - RV_TRUE if we want to flush each message written to the
 *                                logfile
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other values on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogListenerConstructLogfile(
    IN  RvLogListener*  listener,
    IN  RvLogMgr*       logMgr,
    IN  const RvChar*   fileName,
    IN  RvUint32        numFiles,
    IN  RvUint32        fileSize,
    IN  RvBool          flushEachMessage);


/********************************************************************************************
 * RvLogListenerLogfileGetCurrentFilename
 *
 * Get the filename of the current file being written.
 *
 * INPUT   : listener       - Listener to check
 *           fileNameLength - Maximum length of filename
 * OUTPUT  : fileName       - Filename of the current file being written
 * RETURN  : RV_OK on success, other values on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogListenerLogfileGetCurrentFilename(
    IN  RvLogListener*  listener,
    IN  RvUint32        fileNameLength,
    OUT RvChar*         fileName);


/********************************************************************************************
 * RvLogListenerDestructLogfile - Destruct the logfile listener
 *
 * INPUT   : listener   - Listener to destruct
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other values on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogListenerDestructLogfile(
    IN  RvLogListener*  listener);


/********************************************************************************************
 * RvLogListenerConstructDebug
 *
 * Construct a log listener that sends log messages to the debug window of Visual C.
 * This one is only applicable for Win32 applications.
 *           
 * INPUT   : listener   - Listener to construct
 *           logMgr     - Log manager to listen to
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other values on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogListenerConstructDebug(
    IN  RvLogListener*  listener,
    IN  RvLogMgr*       logMgr);


/********************************************************************************************
 * RvLogListenerDestructDebug - Destruct the debug listener
 *
 * INPUT   : listener   - Listener to destruct
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other values on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogListenerDestructDebug(
    IN  RvLogListener*  listener);


#else
/* No internal listeners - use macros instead */
#define Rv2LogListenerConstructTerminal(_listener, _logMgr, _stdOut) (*_listener = RV_OK)
#define Rv2LogListenerDestructTerminal(_listener) (*_listener = RV_OK)
#define Rv2LogListenerConstructLogfile(_listener, _logMgr, _fileName, _numFiles, _fileSize, _flushEachMessage) \
    (*_listener = RV_OK)
#define Rv2LogListenerDestructLogfile(_listener) (*_listener = RV_OK)
#define Rv2LogListenerConstructDebug(_listener, _logMgr) (*_listener = RV_OK)
#define Rv2LogListenerDestructDebug(_listener) (*_listener = RV_OK)

#endif  /* (RV_LOGLISTENER_TYPE != RV_LOGLISTENER_NONE) */


#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif  /* RV_LOG_LISTENER_H */
