#ifndef _SNMPUNIXDOMAIN_H
#define _SNMPUNIXDOMAIN_H

#ifdef SNMP_TRANSPORT_UNIX_DOMAIN

#ifdef __cplusplus
extern          "C" {
#endif

#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_SYS_UN_H
#include <sys/un.h>
#endif

#include "snmp_linux/snmp_transport.h"
#include "snmp_linux/asn1.h"
#include "snmp_linux/vacm.h"
/*
 * The SNMP over local socket transport domain is identified by
 * transportDomainLocal as defined in RFC 3419.
 */
typedef struct _com2SecUnixEntry {
    char            community[VACMSTRINGLEN];
    char            sockpath[sizeof(struct sockaddr_un)];
    unsigned long   pathlen;
    char            secName[VACMSTRINGLEN];
    char            contextName[VACMSTRINGLEN];
    struct _com2SecUnixEntry *next;
} com2SecUnixEntry;

#define TRANSPORT_DOMAIN_LOCAL	1,3,6,1,2,1,100,1,13
extern oid netsnmp_UnixDomain[];

netsnmp_transport *netsnmp_unix_transport(struct sockaddr_un *addr,
                                          int local);
void netsnmp_unix_agent_config_tokens_register(void);
void netsnmp_unix_parse_security(const char *token, char *param);
int netsnmp_unix_getSecName(void *opaque, int olength,
                            const char *community,
                            size_t community_len, char **secName,
                            char **contextName);


/*
 * "Constructor" for transport domain object.  
 */

void            netsnmp_unix_ctor(void);

#ifdef __cplusplus
}
#endif
#endif                          /*SNMP_TRANSPORT_UNIX_DOMAIN */

#endif/*_SNMPUNIXDOMAIN_H*/
