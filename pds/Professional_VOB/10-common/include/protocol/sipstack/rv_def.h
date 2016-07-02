/*
*********************************************************************************
*                                                                               *
* NOTICE:                                                                       *
* This document contains information that is confidential and proprietary to    *
* RADVision LTD.. No part of this publication may be reproduced in any form     *
* whatsoever without written prior approval by RADVision LTD..                  *
*                                                                               *
* RADVision LTD. reserves the right to revise this publication and make changes *
* without obligation to notify any person of such revisions or changes.         *
*********************************************************************************
*/


/*********************************************************************************
 *                              <RV_DEF.h>
 *
 *
 *    Author                         Date
 *    ------                        ------
 *    Udi Tirosh                   Feb 2004
 *********************************************************************************/


#ifndef RV_DEF_H
#define RV_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "rvtypes.h"
#include "rverror.h"

#if defined(RV_DEPRECATED_CORE)
/*-----------------------------------------------------------------------*/
/*                          MACRO DEFINITIONS                            */
/*-----------------------------------------------------------------------*/
/*****************************************************************************
 *  Deprecated handles, will be moved to the common core
 *****************************************************************************/
#ifndef RV_SELI_HANDLE
#define RV_SELI_HANDLE
/* Handle to SELI instance */
RV_DECLARE_HANDLE(RV_SELI_Handle);
#endif /*RV_SELI_HANDLE*/

#ifndef RV_LOG_TYPES
#define RV_LOG_TYPES
/* Handle to Log instance */
RV_DECLARE_HANDLE(RV_LOG_Handle);
#endif

/*****************************************************************************
 *  OLD handles, will NOT moved to the common core
 *****************************************************************************/
#ifndef RV_TIMER_POOL_HANDLE
#define RV_TIMER_POOL_HANDLE
/* Handle to timer pool instance */
RV_DECLARE_HANDLE(RV_TIMER_PoolHandle);
#endif /*RV_TIMER_POOL_HANDLE*/

#ifndef RV_THREAD_TYPES
#define RV_THREAD_TYPES
/* Handle to a thread instance */
RV_DECLARE_HANDLE(RV_THREAD_Handle);
#endif /*RV_THREAD_TYPES*/

/*****************************************************************************
 *  RV_Status -- status codes retruned from API functions.
 *  Note: only general status codes are defined here,
 *****************************************************************************/


#define RV_Success              RV_OK                               /* Success - no error, operation finished successfully. */

/* Error Codes  */
#define RV_Failure              RV_ERROR_UNKNOWN                    /* General failure (more details can be found on      */
#define RV_IllegalAction        RV_ERROR_ILLEGAL_ACTION             /* The requested action is illegal (usually illegal
                                                                       action in the current state)                      */
#define RV_InvalidHandle        RV_ERROR_INVALID_HANDLE             /* Input handle parameter is null or defected         */
#define RV_InvalidParameter     RV_ERROR_BADPARAM                   /* A given parameter to function is illegal defected  */
#define RV_OutOfResources       RV_ERROR_OUTOFRESOURCES             /* The function can not be executed due to a limit of
                                                                       resources - mutex, timers, messages, etc.          */
#define RV_NetworkProblem       RV_ERROR_NETWORK_PROBLEM            /* Action failed due to network problems              */
#define RV_TryAgain             RV_ERROR_TRY_AGAIN                  /* Action can't be completed, try again later         */


typedef int RV_Status;

/* The following macro is used to define type of the type VOID * in a way that
   each VOID * type that is defined using this MACRO will be different than
   other types that are defined by this macro ( such a way will enable the
   compiler to warn us from Ilegal calls between two differents types even
   though that both of them need to be defined as VOID *.This macro is used to
   define hanldes in all stack modules. */

#ifndef DECLARE_VOID_POINTER
#define DECLARE_VOID_POINTER RV_DECLARE_HANDLE
#endif /* DECLARE_VOID_POINTER */
/* Definition of minimum and maximum macros */
#ifndef RV_MIN
#define RV_MIN RvMin
#endif /* RV_MIN */

#ifndef RV_MAX
#define RV_MAX RvMax
#endif /* RV_MAX */


/*-----------------------------------------------------------------------*/
/*                          TYPE DEFINITIONS                             */
/*-----------------------------------------------------------------------*/

/*==============================================================
 * Basic Data Types
 *=============================================================*/
#ifndef RV_BASIC_TYPES
#define RV_BASIC_TYPES
typedef RvUint32    RV_UINT32;
typedef RvUint16    RV_UINT16;
typedef RvUint8     RV_UINT8;
typedef RvInt32     RV_INT32;
typedef RvInt16     RV_INT16;
typedef RvInt8      RV_INT8;

typedef RvUint      RV_UINT;
typedef RvInt       RV_INT;
typedef RvUint8     RV_BYTE;

typedef RvUint8     RV_UCHAR;
typedef RvChar      RV_CHAR;
#endif /*RV_BASIC_TYPES*/

/*=============================================================
 *  Boolean Type and Values
 *============================================================*/
typedef RvBool    RV_BOOL;

#ifndef RV_TRUE
#define RV_TRUE  1
#endif /* TRUE */

#ifndef RV_FALSE
#define RV_FALSE 0
#endif /* FALSE */



/*****************************************************************************
 *                          Core stuff                                       *
 *****************************************************************************/

#endif /*RV_DEPRECATED_CORE */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* RV_DEF_H */

