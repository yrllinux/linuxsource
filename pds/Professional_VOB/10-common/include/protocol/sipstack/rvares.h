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
#if !defined(RV_DNS_TYPE) || \
    ((RV_DNS_TYPE != RV_DNS_NONE) && (RV_DNS_TYPE != RV_DNS_ARES))
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
RvStatus RvAresInit(void);


/********************************************************************************************
 * RvAresEnd
 *
 * Shutdown the DNS module.
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RvStatus RvAresEnd(void);


/********************************************************************************************
 * RvAresSourceConstruct
 *
 * Constructs a Log Source object for the DNS module.
 *
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RvStatus RvAresSourceConstruct(RvLogMgr* logMgr);



#if (RV_DNS_TYPE == RV_DNS_ARES)

/********************************************************************************************
 * RvAresConstruct
 *
 * Constructs a DNS engine, allocates memory for the DNS server state structures,
 * allocates memory for TCP input data and sets the user callback routine which will
 * be called upon answers arrival.
 *
 * INPUT:
 *  selectEngine - select engine constructed by the user previously and will be
 *                 used to receive transport events from DNS servers
 *  newItemCB    - a user callback routine to handle DNS replies
 *  maxServers   - the maximum number of DNS servers that will be configured
 *                 will be set to the actual number of servers found in the system repository
 *  maxDomains   - the maximum number of domain strings that will be configured
 *                 will be set to the actual number of domains found in the system repository
 *  tcpBuffLen   - length of the TCP buffer that will be used to receive DNS replies
 *  logMgr       - handle of the log manager for this instance
 * OUTPUT:
 *  dnsEngine    - a structure of type RvDnsEngine allocated by the user
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV RvAresConstruct(
	IN    RvSelectEngine*    selectEngine,
	IN    RvDnsNewRecordCB   newRecordCB,
    INOUT RvInt*             maxServers,
    INOUT RvInt*             maxDomains,
    IN    RvInt              tcpBuffLen,
	IN    RvLogMgr*          logMgr,
	OUT   RvDnsEngine*       dnsEngine);


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
RVCOREAPI RvStatus RVCALLCONV RvAresDestruct(
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
RVCOREAPI RvStatus RVCALLCONV RvAresSetParams(
	IN  RvDnsEngine*        dnsEngine,
    IN  RvInt               timeoutInSec,
    IN  RvInt               nTries,
    IN  RvAddress*          dnsServList,
    IN  RvInt               nServers,
    IN  RvChar**            dnsDomains,
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
RVCOREAPI RvStatus RVCALLCONV RvAresGetParams(
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
 * Sends a DNS query to one of the pre-configured DNS servers and returns immediately
 * before replies are received.
 *
 * INPUT:
 *  dnsEngine   - a DNS engine structure constructed previously
 *  dnsQuery    - type of query (enum value: IPv4/6, SRV or NAPTR)
 *  dnsName     - the name of the domain to search for its DNS records
 *  as_is       - indicates whether ARES will use the "suffix algorithm" or send a query
 *                with the provided name as-is
 *  queryBuffer - a buffer of qbufLen bytes long to be used by ARES as a work area
 *                to generate the query.
 *                the buffer must not be corrupted by the user until a reply is received
 *  qbufLen     - specifies the length in bytes of queryBuffer.
 *                if length is too small an error is returned and the required value is
 *                set into this parameter.
 *                Note: this notification may occurred iteratively more than once
 *  context     - a user private data. will be delivered to the user callback
 * OUTPUT:
 *  queryId     - the query id; generated by ARES; enables the user to cancel transactions
 *                and also delivered to the user callback together with the context param.
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV RvAresSendQuery(
	IN    RvDnsEngine*      dnsEngine,
	IN    RvDnsQueryType    dnsQuery,
    IN    const RvChar*     dnsName,
    IN    RvBool            as_is,
    IN    void*             queryBuffer,
    INOUT RvInt*            qbufLen,
	IN    void*             context,
    OUT   RvUint32*         queryId);


/********************************************************************************************
 * RvAresCancelQuery
 *
 * Asks ARES to ignore replies for a query which has been already sent.
 * The query buffer provided by the user is no longer valid to ARES.
 *
 * INPUT:
 *  dnsEngine   - a DNS engine structure constructed previously
 *  queryId     - the query id supplied by ARES when the query was sent
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
RVCOREAPI RvStatus RVCALLCONV RvAresCancelQuery(
	IN  RvDnsEngine*        dnsEngine,
    IN  RvUint32            queryId);


#if defined(__cplusplus)
}
#endif

#endif /* RV_DNS_NONE */

#endif /* _RV_DNS_H */
