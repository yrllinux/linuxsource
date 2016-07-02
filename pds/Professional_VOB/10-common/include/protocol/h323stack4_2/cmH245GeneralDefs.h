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

#ifndef GENERAL_DEFS_H
#define GENERAL_DEFS_H

#include "rvtypes.h"
#include "pvaltree.h"


#ifdef __cplusplus
extern "C" {
#endif

/* definitions for supporting H.225 or H.223 */
#define RV_H245_SUPPORT_H225_PARAMS 1
#define RV_H245_SUPPORT_H223_PARAMS 0


/* This file contains definitions that were declared in the CM module. These definitions
   cannot be anymore in the CM because they are needed in the H.245 module. */
RV_DECLARE_HANDLE(HAPP);
RV_DECLARE_HANDLE(HAPPCALL);
RV_DECLARE_HANDLE(HCALL);


typedef struct
{
  /* Set one of the following choices */

  /* 1. object id */
  char object[128]; /* in object identifier ASN format: "0 1 2" */
  int objectLength; /* in bytes. <=0 if not set */

  /* 2. h.221 id */
  RvUint8 t35CountryCode;
  RvUint8 t35Extension;
  RvUint16 manufacturerCode;
} cmNonStandardIdentifier;



typedef struct
{
    cmNonStandardIdentifier   info;
    int               length;
    char*             data;
} cmNonStandardParam;


/* nonStandard handling_______________________________________________________________*/

RVAPI int RVCALLCONV /* RV_TRUE or negative value on failure */
cmNonStandardParameterCreate(
                 /* Create NonStandardParameter tree */
                 IN  HPVT hVal,
                 IN  int rootId, /* nonStandardData */

                 IN  cmNonStandardIdentifier *identifier,
                 IN  const RvChar *data,
                 IN  int dataLength /* in bytes */
                 );

RVAPI int RVCALLCONV /* RV_TRUE or negative value on failure */
cmNonStandardParameterGet(
              /* Convert NonStandardParameter tree to C structures */
              IN  HPVT hVal,
              IN  int rootId, /* nonStandardData */

              OUT cmNonStandardIdentifier *identifier,
              OUT char *data,
              INOUT RvInt32 *dataLength /* (in bytes) IN: data allocation. OUT: correct size */
              );

/* General functions */

/************************************************************************
 * cmEmGetH245Syntax
 * purpose: Returns the syntax of h245.asn.
 *          This syntax holds the H245 standard's ASN.1 syntax
 *          The syntax is the buffer passed to pstConstruct as syntax.
 * input  : none
 * output : none
 * return : Syntax of h245.asn
 ************************************************************************/
RVAPI
RvUint8* RVCALLCONV cmEmGetH245Syntax(void);

#include "cmctrl.h"

/* Mib functions */
/************************************************************************************
 *
 * MIB Related
 * This part handles the communication between the MIB module of the stack and the
 * SNMP application (applSnmp).
 * Applications that want to use applSnmp should NOT use this API functions, as they
 * are used by applSnmp.
 *
 ************************************************************************************/
RV_DECLARE_HANDLE(HMibHandleT);

typedef int (*h341AddNewCallT )(HMibHandleT mibHandle,HCALL hsCall);
typedef void (*h341DeleteCallT  )(HMibHandleT mibHandle,HCALL hsCall);

typedef int (*h341AddControlT  )(HMibHandleT mibHandle,HCALL hsCall);
typedef void (*h341DeleteControlT  )(HMibHandleT mibHandle,HCALL hsCall);

typedef int (*h341AddNewLogicalChannelT  )(HMibHandleT mibHandle,HCHAN hsChan);
typedef void (*h341DeleteLogicalChannelT )(HMibHandleT mibHandle,HCHAN hsChan);


typedef struct
{
    h341AddNewCallT h341AddNewCall;
    h341DeleteCallT h341DeleteCall;
    h341AddControlT h341AddControl;
    h341DeleteControlT h341DeleteControl;
    h341AddNewLogicalChannelT h341AddNewLogicalChannel;
    h341DeleteLogicalChannelT h341DeleteLogicalChannel;
} MibEventT;


/************************************************************************
 * cmMibEventSet
 * purpose: Set MIB notifications from the stack to an SNMP application
 * input  : hApp        - Stack's application handle
 *          mibEvent    - Event callbacks to set
 *          mibHandle   - Handle of MIB instance for the stack
 * output : none
 * return : Non-negative on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmMibEventSet(IN HAPP hApp, IN MibEventT* mibEvent, IN HMibHandleT mibHandle);



#ifdef __cplusplus
}
#endif


#endif  /* GENERAL_DEFS_H */
