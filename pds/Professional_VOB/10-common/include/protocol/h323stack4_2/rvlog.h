/***********************************************************************
Filename   : rvlog.h
Description: log handling
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

#ifndef RV_LOG_H
#define RV_LOG_H

#include "rvlockinternal.h"
#include "rvtypes.h"
#include "rverror.h"


#if defined(__cplusplus)
extern "C" {
#endif


/* Module specific error codes (-512..-1023). See rverror.h for more details */
#define RV_LOG_ERROR_RECURSION -512 /* Logging an error might cause an endless recursion */
#define RV_LOG_ERROR_ALREADY_REGISTERED -513 /* Listener is already registered to this log object */

/* Log message id's: in opposite to log level values these are index numbers */
#define RV_LOGID_EXCEP      0
#define RV_LOGID_ERROR      1
#define RV_LOGID_WARNING    2
#define RV_LOGID_INFO       3
#define RV_LOGID_DEBUG      4
#define RV_LOGID_ENTER      5
#define RV_LOGID_LEAVE      6
#define RV_LOGID_SYNC       7


/* Log manager object */
typedef struct RvLogMgrInternal RvLogMgr;


/* Source of log messages */
typedef struct RvLogSourceInternal* RvLogSource;


/* Type of message being logged */
typedef RvUint8 RvLogMessageType;


/* Log information associated with a log message. This struct is given
   to the printing function on each log message. */
typedef struct RvLogRecordInternal RvLogRecord;


/********************************************************************************************
 * RvLogPrintCb - Callback that is executed whenever a message has to be logged somewhere
 *
 * The log record holds a pointer to the message to log. This message has
 * a set amount of bytes before it allocated but unused to allow better
 * formatting of messages. That amount is RV_LOG_RESERVED_BYTES.
 *
 * INPUT   : logRecord  - Information related with the logged message
 *           userData   - User related data, given when the printing function was set
 * OUTPUT  : None
 * RETURN  : None
 */
typedef void
    (RVCALLCONV* RvLogPrintCb)(
        IN RvLogRecord* logRecord,
        IN void*        userData);


/* Maximum number of listeners for a logging object */
#define RV_LOG_MAX_LISTENERS RvInt32Const(10)

/* Amount of reserved bytes before message when inside the log callback */
#define RV_LOG_RESERVED_BYTES RvInt32Const(139)

/* Maximum size of each message, without the reserved bytes */
#define RV_LOG_MESSAGE_SIZE RvInt32Const(1024)

/* Maximum number of message types */
#define RV_LOG_NUM_MESSAGE_TYPES 8


#include "rvloginternal.h"



/* Prototypes and macros */

RvStatus Rv2LogInit(void);
RvStatus Rv2LogEnd(void);


/********************************************************************************************
 * RvLogConstruct
 *
 * Create a log object. Only a single such object is used by the core and the
 * stacks on top of it.
 * 
 * INPUT   : logMgr - Log manager to construct
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogConstruct(IN RvLogMgr* logMgr);


/********************************************************************************************
 * RvLogDestruct - Kill a log object
 *
 * INPUT   : logMgr - Log manager to destruct
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogDestruct(IN RvLogMgr* logMgr);


/********************************************************************************************
 * RvLogRegisterListener - Set a listener function to the log.
 *
 * Multiple listeners can be set on each log
 * object. The listener is used to actually log the messages.
 *
 * INPUT   : logMgr     - Log manager to use
 *           listener   - Listener function, called on each message
 *           userData   - User data set as a parameter of the listener function
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogRegisterListener(
    IN RvLogMgr*    logMgr,
    IN RvLogPrintCb listener,
    IN void*        userData);


/********************************************************************************************
 * RvLogUnregisterListener - Unset a listener function from the log.
 *
 * INPUT   : logMgr     - Log manager to use
 *           listener   - Listener function, called on each message
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogUnregisterListener(
    IN RvLogMgr*    logMgr,
    IN RvLogPrintCb listener);


/********************************************************************************************
 * RvLogSourceConstruct - Create a new source of messages in a log manager
 *
 * INPUT   : logMgr     - Log manager
 *           source     - Source of messages to create
 *           name       - Name of the source
 *           description- Description of the source
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogSourceConstruct(
    IN RvLogMgr*        logMgr,
    IN RvLogSource*     source,
    IN const RvChar*    name,
    IN const RvChar*    description);


/********************************************************************************************
 * RvLogSourceDestruct - Delete a source of messages from a log manager
 *
 * INPUT   : source     - Source of messages to delete
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogSourceDestruct(
    IN RvLogSource*     source);


/********************************************************************************************
 * RvLogGetSourceByName
 *
 * purpose : Get the source for a specific log source name
 * input   : logMgr     - Log manager
 *           name       - Name of the source to find
 * output  : source     - Source found on success
 * return  : RV_OK on success, other on failure
 ********************************************************************************************/
RVCOREAPI RvStatus RVCALLCONV Rv2LogGetSourceByName(
    IN  RvLogMgr*       logMgr,
    IN  const RvChar*   name,
    OUT RvLogSource*    source);


/********************************************************************************************
 * RvLogIsSelected
 *
 * Check to see if a specific message type should be sent to the log by a given source
 *
 * INPUT   : source         - Source of message to log
 *           messageType    - Type of the message to log
 * OUTPUT  : None
 * RETURN  : RV_TRUE if message should be logged, RV_FALSE otherwise
 */
RVCOREAPI
RvBool RVCALLCONV Rv2LogIsSelected(
    IN RvLogSource*     source,
    IN RvLogMessageType messageType);


/********************************************************************************************
 * RvLogSetLevel
 *
 * Set the level of logging, while leaving the masks of all log sources without a change.
 * 
 * INPUT   : logMgr - Log manager
 *           level  - 0 stop logging, 1 log by the masks of the sources, 2 log everything
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogSetLevel(
    IN RvLogMgr*    logMgr,
    IN RvInt32      level);


/********************************************************************************************
 * RvLogSetGlobalMask
 *
 * Set the mask of messages to log on all the sources of the log object
 *
 * INPUT   : logMgr         - Log manager
 *           messageMask    - Type of the messages to log
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogSetGlobalMask(
    IN RvLogMgr*        logMgr,
    IN RvLogMessageType messageMask);


/********************************************************************************************
 * RvLogSourceSetMask
 *
 * Set the mask of messages to log for a specific source
 *
 * INPUT   : source         - Source of messages to set
 *           messageMask    - Type of the messages to log
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2LogSourceSetMask(
    IN RvLogSource*     source,
    IN RvLogMessageType messageMask);


/********************************************************************************************
 * RvLogSourceGetMask - Get the mask of messages to log for a specific source
 *
 * INPUT   : source         - Source of messages to get
 * OUTPUT  : None
 * RETURN  : Message mask of messages that are logged
 */
RVCOREAPI
RvLogMessageType RVCALLCONV Rv2LogSourceGetMask(
    IN RvLogSource*     source);


/********************************************************************************************
 * RvLogSourceGetName - Get the name for a specific log source
 *
 * INPUT   : source         - Source of messages to get
 * OUTPUT  : None
 * RETURN  : Name of the source on success, NULL on failure
 */
RVCOREAPI const RvChar* RVCALLCONV Rv2LogSourceGetName(
    IN RvLogSource*     source);


/********************************************************************************************
 * RvLogXXX
 *
 * Log a text message with variable amount of arguments, to the relevant
 * logging level.These macros should be used and not RvLogTextXXX functions!
 * example - RvLogDebug((RvLogSource*)source, ((RvLogSource*)source, "Example %d", i));
 *
 * INPUT   : source     - Source of message to log
 *           line       - Formatted string to log
 * OUTPUT  : None
 * RETURN  : None
 */
#if defined(RV_LOG_COMPILER_WARNINGS)
#define RvLogExcep(source, funcParams) sprintf funcParams
#define RvLogError(source, funcParams) sprintf funcParams
#define RvLogWarning(source, funcParams) sprintf funcParams
#define RvLogInfo(source, funcParams) sprintf funcParams
#define RvLogDebug(source, funcParams) sprintf funcParams
#define RvLogEnter(source, funcParams) sprintf funcParams
#define RvLogLeave(source, funcParams) sprintf funcParams
#define RvLogSync(source, funcParams) sprintf funcParams
#else

#define RvLogExcep(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_EXCEP)) Rv2LogTextExcep funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextExcep(
    IN  RvLogSource*        source,
    IN  const char*         line, ...);

#define RvLogError(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_ERROR)) Rv2LogTextError funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextError(
    IN  RvLogSource*        source,
    IN  const char*         line, ...);

#define RvLogWarning(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_WARNING)) Rv2LogTextWarning funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextWarning(
    IN  RvLogSource*        source,
    IN  const char*         line, ...);

#define RvLogInfo(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_INFO)) Rv2LogTextInfo funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextInfo(
    IN  RvLogSource*        source,
    IN  const char*         line, ...);

#define RvLogDebug(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_DEBUG)) Rv2LogTextDebug funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextDebug(
    IN  RvLogSource*        source,
    IN  const char*         line, ...);

#define RvLogEnter(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_ENTER)) Rv2LogTextEnter funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextEnter(
    IN  RvLogSource*        source,
    IN  const char*         line, ...);

#define RvLogLeave(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_LEAVE)) Rv2LogTextLeave funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextLeave(
    IN  RvLogSource*        source,
    IN  const char*         line, ...);

#define RvLogSync(source, funcParams) \
{if ((source != NULL) && Rv2LogIsSelected(source, RV_LOGLEVEL_SYNC)) Rv2LogTextSync funcParams;}
RVCOREAPI RvStatus RVCALLCONV Rv2LogTextSync(
    IN  RvLogSource*        source,
	IN  const char*         line, ...);

#endif

/********************************************************************************************
 * RvLogRecordGetXXX - Retrieves specific information from a log record. 
 *
 * This set of functions should be used inside the listener functions when messages have 
 * to be logged.
 *
 * INPUT   : logRecord  - Log record to check
 * OUTPUT  : None
 * RETURN  : Desired field
 */
#define RvLogRecordGetThread(logRecord) ((logRecord)->threadName)
#define RvLogRecordGetSource(logRecord) ((logRecord)->source)
#define RvLogRecordGetMessageType(logRecord) ((logRecord)->messageType)
#define RvLogRecordGetText(logRecord) ((logRecord)->text)


/********************************************************************************************
 * RvLogSourcePrintInterfacesData - Prints information of the interfaces data to the log.
 *
 * The interfaces data string should be given from the function RvCCoreInterfaces().
 *
 * INPUT   : source             - Source of messages to log to
 *           interfacesString   - Strings to log, as given by RvCCoreInterfaces()
 * OUTPUT  : None
 * RETURN  : None
 */
#if (RV_LOGMASK & RV_LOGLEVEL_INFO)
RVCOREAPI void RVCALLCONV Rv2LogSourcePrintInterfacesData(
    IN RvLogSource*     source,
    IN const RvChar*    interfacesString);
#else
#define Rv2LogSourcePrintInterfacesData(_source, _interfacesString)
#endif

/********************************************************************************************
 * RvLogGetMessageCount - Returns the number of messages of a specified type that
 *                        sent to log.
 *
 * INPUT   : logMgr         - The log manager
 *           messageType    - The required message type
 * OUTPUT  : None
 * RETURN  : None
 */
RVCOREAPI RvInt RVCALLCONV Rv2LogGetMessageCount(
    IN RvLogMgr*        logMgr,
    IN RvInt            messageType);

/********************************************************************************************
 * RvLogGetMessageCount - Configures the log manager to print the thread id instead
 *                        of the thread name.
 *
 * INPUT   : logMgr     - The log manager
 * OUTPUT  : None
 * RETURN  : None
 */
RVCOREAPI void RVCALLCONV Rv2LogPrintThreadId(
    IN RvLogMgr*        logMgr);


/* The checks below removes functions and macros by the specified log mask we're using.
   This allows us to compile the code with partial logging (for example, only compile with
   error messages being able to log) */

#if (!(RV_LOGMASK & RV_LOGLEVEL_EXCEP))
#undef RvLogExcep
#undef Rv2LogTextExcep
#define Rv2LogTextExcep
#define RvLogExcep(source, funcParams)
#endif

#if (!(RV_LOGMASK & RV_LOGLEVEL_ERROR))
#undef RvLogError
#undef Rv2LogTextError
#define Rv2LogTextError
#define RvLogError(source, funcParams)
#endif

#if (!(RV_LOGMASK & RV_LOGLEVEL_WARNING))
#undef RvLogWarning
#undef Rv2LogTextWarning
#define Rv2LogTextWarning
#define RvLogWarning(source, funcParams)
#endif

#if (!(RV_LOGMASK & RV_LOGLEVEL_INFO))
#undef RvLogInfo
#undef Rv2LogTextInfo
#define Rv2LogTextInfo
#define RvLogInfo(source, funcParams)
#endif

#if (!(RV_LOGMASK & RV_LOGLEVEL_DEBUG))
#undef RvLogDebug
#undef Rv2LogTextDebug
#define Rv2LogTextDebug
#define RvLogDebug(source, funcParams)
#endif

#if (!(RV_LOGMASK & RV_LOGLEVEL_ENTER))
#undef RvLogEnter
#undef Rv2LogTextEnter
#define Rv2LogTextEnter
#define RvLogEnter(source, funcParams)
#endif

#if (!(RV_LOGMASK & RV_LOGLEVEL_LEAVE))
#undef RvLogLeave
#undef Rv2LogTextLeave
#define Rv2LogTextLeave
#define RvLogLeave(source, funcParams)
#endif


#if (!(RV_LOGMASK & RV_LOGLEVEL_SYNC))
#undef RvLogSync
#undef Rv2LogTextSync
#define Rv2LogTextSync
#define RvLogSync(source, funcParams)
#endif


#if (RV_LOGMASK == RV_LOGLEVEL_NONE)
#undef Rv2LogIsSelected
#define Rv2LogIsSelected(source, messageType) RV_FALSE
#endif

//add by yj
enum
{
	RV_PEXP,
		RV_PINFO,
		RV_PDEBUG,
		RV_ALL
};
//end

#if defined(__cplusplus)
}
#endif

#endif /* RV_LOG_H */
