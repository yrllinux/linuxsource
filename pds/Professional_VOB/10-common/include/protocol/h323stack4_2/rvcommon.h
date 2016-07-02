/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/


/************************************************************************
 *
 * rvcommon.h
 *
 *
 * Common definitions for RADVISION H.323 Protocol Stack project
 *
 ************************************************************************/

#ifndef __RVCOMMON_H
#define __RVCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rvtypes.h"
#include "rverror.h"


/* Declare cmTransportAddress with support for IPv4 only. This allows better use of memory
   resources than RV_H323_TRANSPORT_ADDRESS_OLD, since it uses less memory for the struct
   cmTransportAddress. */
#define RV_H323_TRANSPORT_ADDRESS_IPV4_ONLY 0

/* Declare cmTransportAddress as it was prior to version 4.1 */
#define RV_H323_TRANSPORT_ADDRESS_OLD 1

/* Declare cmTransportAddress with support of any address */
#define RV_H323_TRANSPORT_ADDRESS_ANY 2


/************************************************************************
 * RV_H323_TRANSPORT_ADDRESS
 * H.323 Protocol Stack support for the TransportAddress type declared
 * by the ASN.1 definitions.
 * This definition specifies how the stack compiles its
 * cmTransportAddress type.
 ************************************************************************/
#if (RV_NET_TYPE == RV_NET_IPV6)
#define RV_H323_TRANSPORT_ADDRESS RV_H323_TRANSPORT_ADDRESS_ANY
#else
#define RV_H323_TRANSPORT_ADDRESS RV_H323_TRANSPORT_ADDRESS_OLD
#endif





/************************************************************************
 * Remove this definition if you want to reduce the footprint size of the
 * H.323 protocol stack by removing any unnecessary functions from the
 * code.
 * This includes functions that had meaning in the past and can be
 * discarded.
 ************************************************************************/
#define RV_H323_COMPILE_WITH_DEAD_FUNCTIONS


/************************************************************************
 * Here for backward compatability...
 * Applications that use the definitions written below should add
 * RV_H323_VERSION_DEFINITIONS with the version value to their compiler
 * definitions when compiling the H.323 Protocol Stack.
 *
 * Valid values for RV_H323_VERSION_DEFINITIONS are:
 * 410
 * 400
 * 309
 *
 * We preserve this part for backward compatibility.
 * Application writers are encouraged to remove this definition and use
 * the new definitions. The new definitions insure that the stack doesn't
 * declare anything that might be declared by the operating system or
 * other libraries.
 ************************************************************************/
#if defined(RV_H323_VERSION_DEFINITIONS)

#if (RV_H323_VERSION_DEFINITIONS <= 309)
/* ------------------------------- Versions earlier than 4.0 --------------------------- */
/* These versions declared CALLCONV and ERROR */

#ifdef CALLCONV
#undef CALLCONV
#endif
#define CALLCONV RVCALLCONV

#ifdef ERROR
#undef ERROR
#endif
#define ERROR RVERROR
#endif  /* (RV_H323_VERSION_DEFINITIONS <= 309) */


#if (RV_H323_VERSION_DEFINITIONS <= 400)
/* ------------------------------- Versions earlier than 4.1 --------------------------- */
/* Removed most of the declarations that might be declared in OS or other libraries */

/* Included by previous versions */
#if (RV_OS_TYPE == RV_OS_TYPE_SOLARIS) || \
    (RV_OS_TYPE == RV_OS_TYPE_LINUX) || \
    (RV_OS_TYPE == RV_OS_TYPE_HPUX) || \
    (RV_OS_TYPE == RV_OS_TYPE_TRU64) || \
    (RV_OS_TYPE == RV_OS_TYPE_UNIXWARE)
#include <unistd.h>
#include <memory.h>
#endif

#if (RV_OS_TYPE == RV_OS_TYPE_WIN32) || \
    (RV_OS_TYPE == RV_OS_TYPE_WINCE)
#include <memory.h>
#include <malloc.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* Make sure address struct is set in a correct manner */
#if (RV_H323_TRANSPORT_ADDRESS != RV_H323_TRANSPORT_ADDRESS_OLD)
#error RV_H323_TRANSPORT_ADDRESS must be set to OLD for support of versions earlier than 4.1
#endif


#if defined(NOLOGSUPPORT)
#undef RV_LOGMASK
#define RV_LOGMASK RV_LOGLEVEL_NONE
#endif  /* defined(NOLOGSUPPORT) */


#ifdef SUCCESS
#undef SUCCESS
#endif
#define SUCCESS 1

#ifndef OK
#define OK 0
#endif

#ifndef RVERROR
#define RVERROR RV_ERROR_UNKNOWN
#endif

#ifndef RESOURCES_PROBLEM
#define RESOURCES_PROBLEM RV_ERROR_OUTOFRESOURCES
#endif

#ifndef OBJECT_WAS_NOT_FOUND
#define OBJECT_WAS_NOT_FOUND RV_ERROR_BADPARAM
#endif

#ifndef DECLARE_OPAQUE
#define DECLARE_OPAQUE(name)    typedef struct { int unused; } name##__ ; \
                typedef const name##__ * name; \
                typedef name*  LP##name;
#endif

/* variable types */

#ifndef TRUE
#define TRUE RV_TRUE
#endif

#ifndef FALSE
#define FALSE RV_FALSE
#endif

#if ((RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_WINCE))
/* Seems like in Microsoft BOOL is typedef'ed so we can't just check with ifndef on it */
#ifndef TRUE
#define BOOL RvBool
#endif
#else
/* VxWorks and other embedded OS's sometimes define BOOL instead of us... */
#ifndef BOOL
#define BOOL RvBool
#endif
#endif

#ifndef UINTPTR
#if (RV_ARCH_BITS == RV_ARCH_BITS_32)
#define UINTPTR RvUint32
#else
#define UINTPTR RvUint64
#endif
#endif  /* UINTPTR */

#ifndef INTPTR
#if (RV_ARCH_BITS == RV_ARCH_BITS_32)
#define INTPTR RvInt32
#else
#define INTPTR RvInt64
#endif
#endif  /* INTPTR */

#ifndef UINT
#define UINT RvUint
#endif

#ifndef UINT32
#define UINT32 RvUint32
#endif

#ifndef UINT16
#define UINT16 RvUint16
#endif

#ifndef UINT8
#define UINT8 RvUint8
#endif

#ifndef BYTE
#define BYTE RvUint8
#endif

#ifndef INT32
#define INT32 RvInt32
#endif

#ifndef INT16
#define INT16 RvInt16
#endif

#ifndef INT8
#define INT8 RvInt8
#endif

#ifndef INT
#define INT RvInt
#endif

#endif  /* (RV_H323_VERSION_DEFINITIONS <= 400) */

#endif  /* defined(RV_H323_VERSION_DEFINITIONS) */


#ifdef __cplusplus
}
#endif

#endif  /* __RVCOMMON_H */

