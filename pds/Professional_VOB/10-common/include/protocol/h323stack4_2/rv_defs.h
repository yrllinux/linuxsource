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

#ifndef _RV_DEFS_H
#define _RV_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif


#include "rvcommon.h"


/* --- transport address --- */

typedef enum
{
  cmTransportTypeIP, /* No route */
  cmTransportTypeIPStrictRoute,
  cmTransportTypeIPLooseRoute,
  cmTransportTypeNSAP,
  cmTransportTypeIPv6
} cmTransportType;

typedef enum
{
  cmDistributionUnicast,
  cmDistributionMulticast
} cmDistribution;


#if (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_IPV4_ONLY)

typedef struct
{
    cmTransportType type; /* Type of address - must always be cmTransportTypeIP */
    RvUint32        ip; /* IPv4 address */
    RvUint16        port;
    cmDistribution  distribution; /* Distribution type of this address. Valid for H.245 addresses */
} cmTransportAddress;



#elif (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_OLD)

#define cmRouteSize (7) /* for luck */

typedef struct
{
    RvUint16        length; /* length in bytes of route */
    RvUint32        ip; /* IPv4 address */
    RvUint16        port;
    cmTransportType type;
    RvUint32        route[cmRouteSize];
    cmDistribution  distribution; /* Distribution type of this address. Valid for H.245 addresses */
} cmTransportAddress;


#elif (RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)

#define cmRouteSize (7) /* for luck */

typedef struct
{
    RvUint32        ip; /* IPv4 address */
    RvUint16        length; /* length in bytes of route */
    RvUint32        route[cmRouteSize];
} ipAddressV4;

typedef struct
{
    RvUint8         ip[16]; /* IPv6 address */
    short           scopeId;
} ipAddressV6;

union ipAddress
{
    ipAddressV4 v4;
    ipAddressV6 v6;
};

typedef struct
{
    cmTransportType type;
    union ipAddress addr;
    RvUint16        port;
    cmDistribution  distribution; /* Distribution type of this address. Valid for H.245 addresses */
} cmTransportAddress;

/* example of usage:
cmTransportAddress TA;

TA.type =           Address TYPE.
TA.addr.v6.ip =     IP v6, or-
TA.addr.v4.ip =     IP v4, and maybe-
TA.addr.v4.route =  IP v4 route.
TA.port =           Port for any IP type
TA.distribution =   Distribution for any IP type
*/

#endif



typedef enum
{
    cmQ931setup = 5,
    cmQ931callProceeding = 2,
    cmQ931connect = 7,
    cmQ931alerting = 1,
    cmQ931releaseComplete = 90,
    cmQ931status = 125,
    cmQ931facility = 98,
    cmQ931statusEnquiry = 117,
    cmQ931statusInquiry = 117,
    cmQ931progress = 3,
    cmQ931setupAck = 13,
    cmQ931setupAcknowledge = 13,
    cmQ931information = 123,
    cmQ931notify = 110,
    cmQ931userInformation = 32
} cmCallQ931MsgType;

/* The stage at which it is allowed to transfer the H.245 address to the remote */
typedef enum
{
    cmTransH245Stage_setup,             /* Earliest H.245 possible, may send/act on addresses in all messages */
    cmTransH245Stage_callProceeding,    /* The remote can send the address in the C.P. message */
    cmTransH245Stage_alerting,          /* Only in the alerting message can the remote send the address */
    cmTransH245Stage_connect,           /* The remote must wait for the connect message (normal) */
    cmTransH245Stage_early,             /* our early H.245, sends addresses in setup and connect only */
    cmTransH245Stage_facility,          /* There will be no automatic sending of the address */
    cmTransH245Stage_noH245             /* no support for H.245, sends facility NoH245  */
} cmH245Stage;

/* the way to use annex E connection for Q.931 */
typedef enum
{
    cmTransUseAnnexE,                   /* Force the use of annex E */
    cmTransNoAnnexE,                    /* Force the use of TPKT */
    cmTransPreferedAnnexE,              /* Initiate a both annex E
                                           and TPKT with preference to annex E*/
    cmTransPreferedTPKT                 /* Initiate a both annex E
                                           and TPKT with preference to TPKT*/
} cmAnnexEUsageMode;



#ifdef __cplusplus
}
#endif


#endif  /* _RV_DEFS_H */
