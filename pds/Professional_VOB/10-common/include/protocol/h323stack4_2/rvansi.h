/************************************************************************
 File Name     : rvansi.h
 Description   :
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

#if !defined(RV_ANSI_H)
#define RV_ANSI_H

#ifdef __cplusplus
extern "C" {
#endif




#ifndef RV_ANSI_USECUSTOMSTRINGFORMATTING /* Never set */
#include <stdio.h>
#include <stdarg.h>
/* Use a supplied ANSI library */

/* Formatted Output */
#define Rv2Sprintf sprintf
#define Rv2Vsprintf vsprintf

/* Formatted Input */
#define Rv2Sscanf sscanf


#if (RV_OS_TYPE == RV_OS_TYPE_OSE)
#include <outfmt.h>
#endif

#if (RV_OS_TYPE == RV_OS_TYPE_VXWORKS) || (RV_OS_TYPE == RV_OS_TYPE_PSOS)  || \
    (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN && RV_TOOL_TYPE == RV_TOOL_TYPE_GNU) || \
    (RV_OS_TYPE == RV_OS_TYPE_TRU64) || \
	((RV_OS_TYPE == RV_OS_TYPE_MOPI) && !(defined(_WIN32)))

#include <ctype.h>
#include <stdlib.h>
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif


#if (RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_WINCE) || \
	((RV_OS_TYPE == RV_OS_TYPE_MOPI) && defined(_WIN32))
#define Rv2Snprintf _snprintf
#define Rv2Vsnprintf _vsnprintf

#else
#include <string.h>
#define Rv2Snprintf snprintf
#define Rv2Vsnprintf vsnprintf
#endif


#else
/* Use custom string formatting */

/* Formatted Output */

/********************************************************************************************
 * RvVsscanf
 * This function implements an ANSI vsscanf style function.
 * INPUT   : s      - The string to which to read from.
 *           format - A sprintf style format string.           
 * OUTPUT  : arg    - A variable argument list.
 * RETURN  : The number of characters read, or negative if an error occurred.
 */
static RvInt RvVsscanf(
    IN RvChar* s, 
    IN const RvChar* format, 
    OUT va_list arg);

/********************************************************************************************
 * RvSprintf
 * This function implements an ANSI sprintf style function.
 * INPUT   : s      - The string to which to write.
 *           format - A sprintf style format string.
 *           arg    - A variable argument list.
 * OUTPUT  : None
 * RETURN  : The number of characters written, or negative if an error occurred.
 */
RVCOREAPI RvInt RVCALLCONV Rv2Sprintf(
    IN RvChar* s, 
    IN const RvChar* format, ...);

/********************************************************************************************
 * RvVsprintf
 * This function implements an ANSI vsprintf style function.
 * INPUT   : s      - The string to which to write.
 *           format - A sprintf style format string.
 *           arg    - A variable argument list.
 * OUTPUT  : None
 * RETURN  : The number of characters written, or negative if an error occurred.
 */
RVCOREAPI RvInt RVCALLCONV Rv2Vsprintf(
    IN RvChar* s, 
    IN const RvChar* format, 
    IN va_list arg);

/********************************************************************************************
 * RvSscanf
 * This function implements an ANSI sprintf style function.
 * INPUT   : s      - The string to which to read.
 *           format - A sprintf style format string.
 *           arg    - A variable argument list.
 * OUTPUT  : None
 * RETURN  : The number input items converted, or EOF if an error occured.
 */
RVCOREAPI RvInt RVCALLCONV Rv2Sscanf(
    IN RvChar* s, 
    IN const RvChar* format, ...);

RVCOREAPI RvInt RVCALLCONV Rv2Snprintf(
    IN RvChar* s, 
    IN RvSize_t len, 
    IN const RvChar* format);

/********************************************************************************************
 * RvVsnprintf
 * This function implements an ANSI sprintf style function.
 * INPUT   : s      - The string to which to write.
 *           len    - length of the string.
 *           format - A sprintf style format string.
 *           arg    - A variable argument list.
 * OUTPUT  : None
 * RETURN  : The number input items converted, or EOF if an error occured.
 */
RVCOREAPI 
RvInt RVCALLCONV Rv2Vsnprintf(
    IN RvChar* s, 
    IN RvSize_t len, 
    IN const RvChar* format, 
    IN va_list arg);


/* Formatted Input */
/********************************************************************************************
 * RvScanf - This function implements an ANSI scanf style function.
 *
 * INPUT   : format	- A sscanf style format string.
 *			 ...	- A variable argument list.
 * OUTPUT  : none
 * RETURN  : The number input items converted, or RvEOF if an error occured.   
 */
RVCOREAPI int RVCALLCONV RvScanf(
    IN const char* format,
	INT				...);

#endif

#ifdef __cplusplus
}
#endif

#endif  /* Include guard */

