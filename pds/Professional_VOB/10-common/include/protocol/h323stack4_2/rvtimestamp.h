/* rvtimestamp.h - rvtimestamp header file */
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
/***********************************************************************
 *	DESCRIPTION:
 *	This module provides functions for accessing a timestamp. Timestamp values
 *  are guaranteed to be linear (will never go backwards) and will effectively
 *  never wrap (it will actually wrap in about 292 years).
 ************************************************************************/
#ifndef RV_TIMESTAMP_H
#define RV_TIMESTAMP_H

#include "rvccore.h"
#include "rvlog.h"
#if (RV_OS_TYPE != RV_OS_TYPE_WINCE) && (RV_OS_TYPE != RV_OS_TYPE_MOPI)
#include <time.h>
#endif
#include "rvtime.h"

#if !defined(RV_TIMESTAMP_TYPE) || ((RV_TIMESTAMP_TYPE != RV_TIMESTAMP_USER_DEFINED) && \
    (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_LINUX) && (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_VXWORKS_NORMAL) && \
    (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_VXWORKS_TIMESTAMP) && (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_PSOS) && \
    (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_OSE) && (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_NUCLEUS) && \
    (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_SOLARIS) && (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_WIN32) && \
    (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_POSIX) && (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_UNIXWARE) && \
    (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_EMBLINUX) && (RV_TIMESTAMP_TYPE != RV_TIMESTAMP_SYMBIAN) && \
	(RV_TIMESTAMP_TYPE != RV_TIMESTAMP_MOPI))
#error RV_TIMESTAMP_TYPE not set properly
#endif

/* Module specific error codes (-512..-1023). See rverror.h dor more details */
#define RV_TIMESTAMP_ERROR_NOCPUINFO -512 /* Linux only: /proc/cpuinfo file not found */
#define RV_TIMESTAMP_ERROR_NOTSC     -513 /* Linux only: CPU does not have TSC */
#define RV_TIMESTAMP_ERROR_NOMHZ     -514 /* Linux only: can't find CPU speed in /proc/cpuinfo */
#define RV_TIMESTAMP_ERROR_ZEROSPEED -515 /* CPU frequency of 0 reported */

#if defined(__cplusplus)
extern "C" {
#endif

/********************************************************************************************
 * RvTimestampInit - Initializes the Timestamp module.
 *
 * Must be called once (and only once) before any other functions in the module are called.
 *
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RvStatus Rv2TimestampInit(void);

/********************************************************************************************
 * RvTimestampEnd - Shuts down the Timestamp module. 
 *
 * Must be called once (and only once) when no further calls to this module will be made.
 *
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code.
 */
RvStatus Rv2TimestampEnd(void);

/********************************************************************************************
 * RvTimestampSourceConstruct - Constructs timestamp module log source.
 *
 * Constructs log source to be used by common core when printing log from the 
 * timestamp module. This function is applied per instance of log manager.
 * 
 * INPUT   : logMgr - log manager instance
 * OUTPUT  : none
 * RETURN  : RV_OK if successful otherwise an error code. 
 */
RvStatus Rv2TimestampSourceConstruct(
	IN RvLogMgr	*logMgr);

/********************************************************************************************
 * RvTimestampGet - Gets a timestamp value in nanoseconds. 
 *
 * Values returned by subsequent calls are guaranteed to be linear 
 * (will never go backwards) and will never wrap.
 *
 * INPUT   : logMgr - log manager instance
 * OUTPUT  : none
 * RETURN  : Nanosecond timestamp.
 */
RVCOREAPI 
RvInt64 RVCALLCONV Rv2TimestampGet(
	IN  RvLogMgr	*logMgr);

/********************************************************************************************
 * RvTimestampResolution - Gets the resolution of the timestamp in nanoseconds.
 *
 * INPUT   : none
 * OUTPUT  : logMgr - log manager instance
 * RETURN  : Resolution of the timestamp in nanoseconds.
 */
RVCOREAPI 
RvInt64 RVCALLCONV Rv2TimestampResolution(
	IN  RvLogMgr	*logMgr);

#if defined(__cplusplus)
}
#endif

#endif
