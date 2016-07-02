#ifndef NET_SNMP_SESSION_API_H
#define NET_SNMP_SESSION_API_H

    /**
     *  Library API routines concerned with specifying and using SNMP "sessions"
     *    including sending and receiving requests.
     */

#include "snmp_linux/types.h"

    /*
     *  For the initial release, this will just refer to the
     *  relevant UCD header files.
     *    In due course, the routines relevant to this area of the
     *  API will be identified, and listed here directly.
     *
     *  But for the time being, this header file is a placeholder,
     *  to allow application writers to adopt the new header file names.
     */

#include "snmp_linux/snmp_api.h"
#include "snmp_linux/snmp_client.h"
#include "snmp_linux/asn1.h"
#include "snmp_linux/callback.h"

#include "snmp_linux/snmp_transport.h"
#include "snmp_linux/snmpcallbackdomain.h"
#ifdef SNMP_TRANSPORT_UNIX_DOMAIN
#include "snmp_linux/snmpunixdomain.h"
#endif
#ifdef SNMP_TRANSPORT_UDP_DOMAIN
#include "snmp_linux/snmpudpdomain.h"
#endif
#ifdef SNMP_TRANSPORT_TCP_DOMAIN
#include "snmp_linux/snmptcpdomain.h"
#endif
#ifdef SNMP_TRANSPORT_UDPIPV6_DOMAIN
#include "snmpUDPIPv6Domain.h"
#endif
#ifdef SNMP_TRANSPORT_TCPIPV6_DOMAIN
#include "snmpTCPIPv6Domain.h"
#endif
#ifdef SNMP_TRANSPORT_IPX_DOMAIN
#include "snmpIPXDomain.h"
#endif
#ifdef SNMP_TRANSPORT_AAL5PVC_DOMAIN
#include "snmpAAL5PVCDomain.h"
#endif

#include "ucd_compat.h"

#endif                          /* NET_SNMP_SESSION_API_H */
