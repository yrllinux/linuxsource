/***********************************************************************
Filename   : rvares.h
Description: Interface functions for DNS resolving services
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
#ifndef _RV_DNS_H
#define _RV_DNS_H

#include "rvaddress.h"
#include "rvlog.h"
#include "ares.h"


/* Error checks to make sure configuration has been done properly */
#if !defined(RV_DNS_TYPE) || ((RV_DNS_TYPE != RV_DNS_NONE) && \
    (RV_DNS_TYPE != RV_DNS_WIN32) && (RV_DNS_TYPE != RV_DNS_BIND) && (RV_DNS_TYPE != RV_DNS_ARES))
#error RV_DNS_TYPE not set properly
#endif


#if defined(__cplusplus)
extern "C" {
#endif


/********************************************************************************************
 * RvAresInit
 *
 * Initializes the DNS module.
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RvStatus Rv2AresInit(void);


/********************************************************************************************
 * RvAresEnd
 *
 * Shutdown the DNS module.
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RvStatus Rv2AresEnd(void);


/********************************************************************************************
 * RvAresSourceConstruct
 *
 * Constructs a Log Source object for the DNS module.
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RvStatus Rv2AresSourceConstruct(RvLogMgr* logMgr);



#if (RV_DNS_TYPE == RV_DNS_ARES)

/********************************************************************************************
 * RvAresConstruct
 *
 * Constructs a DNS engine with user callback routines provided by the caller.
 *
 * INPUT:
 *  selectEngine - select engine constructed by the user previously and will be
 *                 used to receive transport events from DNS servers
 *  memAllocCB   - a user callback routine to serve memory requests initiated
 *                 by the DNS resolver
 *  memFreeCB    - a user callback routine to free memory blocks supplied by memAllocCB
 *  newItemCB    - a user callback routine to handle DNS replies
 *  logMgr       - handle of the log manager for this instance
 *  memctx       - memory context for memory callbacks
 * OUTPUT:
 *  dnsEngine    - a structure of type RvDnsEngine allocated by the user
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV Rv2AresConstruct(
	IN  RvSelectEngine*      selectEngine,
	IN  RvDnsMemAllocCB      memAllocCB,
	IN  RvDnsMemFreeCB       memFreeCB,
	IN  RvDnsNewRecordCB     newRecordCB,
    IN  void*                memctx,
	IN  RvLogMgr*            logMgr,
	OUT RvDnsEngine*         dnsEngine);


/********************************************************************************************
 * RvAresDestruct
 *
 * Destructs a DNS engine.
 *
 * INPUT:
 *  dnsEngine  - a DNS engine structure
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV Rv2AresDestruct(
	IN RvDnsEngine*         dnsEngine);


/********************************************************************************************
 * RvAresSetParams
 *
 * Change the default parameters for a DNS engine.
 *
 * INPUT:
 *  dnsEngine    - a DNS engine structure
 *  timeoutInSec - timeout in seconds for un-answered query
 *  nTries       - number of tries before canceling a query
 *  dnsServList  - array of DNS server addresses
 *  nServers     - number of DNS server addresses
 *  dnsDomains   - array of DNS domain suffixes
 *  nDomains     - number of DNS domain suffixes
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV Rv2AresSetParams(
	IN  RvDnsEngine*        dnsEngine,
    IN  RvInt               timeoutInSec,
    IN  RvInt               nTries,
    IN  RvAddress*          dnsServList,
    IN  RvInt               nServers,
    IN  char**              dnsDomains,
    IN  RvInt               nDomains);


/********************************************************************************************
 * RvAresGetParams
 *
 * Retrieve the configuration parameters for a DNS engine.
 *
 * INPUT:
 *  dnsEngine    - a DNS engine structure
 * OUTPUT:
 *  timeoutInSec - timeout in seconds for un-answered query
 *  nTries       - number of tries before canceling a query
 *  dnsServList  - array of DNS server addresses
 *  nServers     - number of DNS server addresses
 *  dnsDomains   - array of DNS domain suffixes
 *  nDomains     - number of DNS domain suffixes
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV Rv2AresGetParams(
    IN     RvDnsEngine*     dnsEngine,
    OUT    RvInt*           timeoutInSec,
    OUT    RvInt*           nTries,
    OUT    RvAddress*       dnsServList,
    INOUT  RvInt*           nServers,
    OUT    char**           dnsDomains,
    INOUT  RvInt*           nDomains);


/********************************************************************************************
 * RvAresSendQuery
 *
 * Sends a DNS query and returns immediately.
 *
 * INPUT:
 *  dnsEngine   - a DNS engine structure constructed previously
 *  dnsQuery    - type of query (enum value: IPv4/6, SRV or NAPTR)
 *  dnsName     - the name of the domain to search for its DNS records
 *  context     - a user private data. will be delivered to the user callbacks
 * OUTPUT:
 *  queryId     - the query id; generated by ARES
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV Rv2AresSendQuery(
	IN  RvDnsEngine*        dnsEngine,
	IN  RvDnsQueryType      dnsQuery,
    IN  RvChar*             dnsName,
    IN  RvBool              as_is,
	IN  void*               context,
    OUT RvUint16*           queryId);

#endif /* RV_DNS_NONE */
#if defined(__cplusplus)
}
#endif
#endif /* _RV_DNS_H */
