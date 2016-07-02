/* $Id: ares.h,v 1.3 2000/09/21 19:15:48 ghudson Exp $ */

/* Copyright 1998 by the Massachusetts Institute of Technology.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#ifndef ARES__H
#define ARES__H

#if (RV_DNS_TYPE == RV_DNS_ARES)

#include "rvselect.h"
#include "rvtimestamp.h"


#if (RV_OS_TYPE == RV_OS_TYPE_SOLARIS)  || (RV_OS_TYPE == RV_OS_TYPE_LINUX)    || \
    (RV_OS_TYPE == RV_OS_TYPE_UNIXWARE) || (RV_OS_TYPE == RV_OS_TYPE_EMBLINUX) || \
    (RV_OS_TYPE == RV_OS_TYPE_TRU64)    || (RV_OS_TYPE == RV_OS_TYPE_HPUX)

#include <netinet/in.h>
#include <arpa/nameser.h>


#elif (RV_OS_TYPE == RV_OS_TYPE_VXWORKS)

#include <netinet/in.h>
#include <resolv/nameser.h>

#define	HFIXEDSZ    12
#define T_AAAA		0x1c
#define T_SRV		0x21
#define T_NAPTR		0x23


#elif (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN)

typedef	unsigned short	u_int16_t;
typedef	unsigned long   u_int32_t;
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>


#elif (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)

#define in_addr SCK_IP_ADDR_STRUCT

#endif


#if (RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS) || \
    (RV_OS_TYPE == RV_OS_TYPE_MOPI)

/*
 * Currently defined opcodes.
 */
typedef enum __ns_opcode {
	ns_o_query = 0,		/* Standard query. */
	ns_o_iquery = 1,	/* Inverse query (deprecated/unsupported). */
	ns_o_status = 2,	/* Name server status query (unsupported). */
				/* Opcode 3 is undefined/reserved. */
	ns_o_notify = 4,	/* Zone change notification. */
	ns_o_update = 5,	/* Zone update message. */
	ns_o_max = 6
} ns_opcode;

/*
 * Currently defined response codes.
 */
typedef	enum __ns_rcode {
	ns_r_noerror = 0,	/* No error occurred. */
	ns_r_formerr = 1,	/* Format error. */
	ns_r_servfail = 2,	/* Server failure. */
	ns_r_nxdomain = 3,	/* Name error. */
	ns_r_notimpl = 4,	/* Unimplemented. */
	ns_r_refused = 5,	/* Operation refused. */
	/* these are for BIND_UPDATE */
	ns_r_yxdomain = 6,	/* Name exists */
	ns_r_yxrrset = 7,	/* RRset exists */
	ns_r_nxrrset = 8,	/* RRset does not exist */
	ns_r_notauth = 9,	/* Not authoritative for zone */
	ns_r_notzone = 10,	/* Zone of record different from zone section */
	ns_r_max = 11,
	/* The following are TSIG extended errors */
	ns_r_badsig = 16,
	ns_r_badkey = 17,
	ns_r_badtime = 18
} ns_rcode;

/*
 * Define constants based on RFC 883, RFC 1034, RFC 1035
 */
#define NS_PACKETSZ	    512	    /* maximum packet size */
#define NS_MAXDNAME	    1025    /* maximum domain name */
#define NS_MAXCDNAME	255	    /* maximum compressed domain name */
#define NS_MAXLABEL	    63	    /* maximum length of domain label */
#define NS_HFIXEDSZ	    12	    /* #/bytes of fixed data in header */
#define NS_QFIXEDSZ	    4	    /* #/bytes of fixed data in query */
#define NS_RRFIXEDSZ	10	    /* #/bytes of fixed data in r record */
#define NS_INT32SZ	    4	    /* #/bytes of data in a u_int32_t */
#define NS_INT16SZ	    2	    /* #/bytes of data in a u_int16_t */
#define NS_INT8SZ	    1	    /* #/bytes of data in a u_int8_t */
#define NS_INADDRSZ	    4	    /* IPv4 T_A */
#define NS_IN6ADDRSZ	16	    /* IPv6 T_AAAA */
#define NS_CMPRSFLGS	0xc0	/* Flag bits indicating name compression. */
#define NS_DEFAULTPORT	53	    /* For both TCP and UDP. */

#define PACKETSZ		NS_PACKETSZ
#define MAXDNAME		NS_MAXDNAME
#define MAXCDNAME		NS_MAXCDNAME
#define MAXLABEL		NS_MAXLABEL
#define	HFIXEDSZ		NS_HFIXEDSZ
#define QFIXEDSZ		NS_QFIXEDSZ
#define RRFIXEDSZ		NS_RRFIXEDSZ
#define	INT32SZ			NS_INT32SZ
#define	INT16SZ			NS_INT16SZ
#define	INADDRSZ		NS_INADDRSZ
#define	IN6ADDRSZ		NS_IN6ADDRSZ
#define	INDIR_MASK		NS_CMPRSFLGS
#define NAMESERVER_PORT	NS_DEFAULTPORT

#define QUERY			ns_o_query

#undef  NOERROR
#define NOERROR			ns_r_noerror
#define FORMERR			ns_r_formerr
#define SERVFAIL		ns_r_servfail
#define NXDOMAIN		ns_r_nxdomain
#define NOTIMP			ns_r_notimpl
#define REFUSED			ns_r_refused
#define YXDOMAIN		ns_r_yxdomain
#define YXRRSET			ns_r_yxrrset
#define NXRRSET			ns_r_nxrrset
#define NOTAUTH			ns_r_notauth
#define NOTZONE			ns_r_notzone

#define MAXHOSTNAMELEN	64

#endif  /* RV_OS_TYPE_WIN32 or RV_OS_TYPE_NUCLEUS */


#ifndef C_IN
#define C_IN			1
#endif
#ifndef T_A
#define T_A				1
#endif
#ifndef T_AAAA
#define T_AAAA	        28
#endif
#ifndef T_SRV
#define T_SRV			33
#endif
#ifndef T_NAPTR
#define T_NAPTR			35
#endif


/* Default system values */
#define	DEFAULT_TIMEOUT		Rv2Int64Mul(RvInt64Const(1,0,5), RV_TIME64_NSECPERSEC)
#define DEFAULT_TRIES		4

/* Maximum DNS name length (RFC1035 2.3.4) */
#define RV_DNS_MAX_NAME_LEN 255

#define ARES_SUCCESS		0

/* Server error codes (ARES_ENODATA indicates no relevant answer) */
#define ARES_ENODATA		1
#define ARES_EFORMERR		2
#define ARES_ESERVFAIL		3
#define ARES_ENOTFOUND		4
#define ARES_ENOTIMP		5
#define ARES_EREFUSED		6

/* Locally generated error codes */
#define ARES_EBADQUERY		7
#define ARES_EBADNAME		8
#define ARES_EBADFAMILY		9
#define ARES_EBADRESP		10
#define ARES_ECONNREFUSED	11
#define ARES_ETIMEOUT		12
#define ARES_EOF		    13
#define ARES_EFILE		    14
#define ARES_ENOMEM		    15
#define ARES_EDESTRUCTION	16
#define ARES_ESERVICE   	17

/* Flag values */
#define ARES_FLAG_USEVC		    (1 << 0)
#define ARES_FLAG_PRIMARY	    (1 << 1)
#define ARES_FLAG_IGNTC		    (1 << 2)
#define ARES_FLAG_NORECURSE	    (1 << 3)
#define ARES_FLAG_STAYOPEN	    (1 << 4)
#define ARES_FLAG_NOSEARCH	    (1 << 5)
#define ARES_FLAG_NOALIASES	    (1 << 6)
#define ARES_FLAG_NOCHECKRESP	(1 << 7)

/* Option mask values */
#define ARES_OPT_FLAGS		    (1 << 0)
#define ARES_OPT_TIMEOUT	    (1 << 1)
#define ARES_OPT_TRIES		    (1 << 2)
#define ARES_OPT_NDOTS		    (1 << 3)
#define ARES_OPT_UDP_PORT	    (1 << 4)
#define ARES_OPT_TCP_PORT	    (1 << 5)
#define ARES_OPT_SERVERS	    (1 << 6)
#define ARES_OPT_DOMAINS	    (1 << 7)
#define ARES_OPT_LOOKUPS	    (1 << 8)


typedef struct RvDnsEngineStruct RvDnsEngine;

typedef enum {
	RV_DNS_REASON_GENERAL = 1,
    RV_DNS_REASON_SERVER_SPACE,
    RV_DNS_REASON_DOMAIN_SPACE,
	RV_DNS_REASON_QUERY_BUFF,
	RV_DNS_REASON_SQUERY_BUFF,
	RV_DNS_REASON_TCP_BUFF,
    RV_DNS_REASON_TCP_SEND_VECTOR
} RvDnsAllocReason;

typedef enum {
    /* these are symmetric codes: application <---> resolver */
	RV_DNS_HOST_IPV4_TYPE = 1,
	RV_DNS_HOST_IPV6_TYPE,
	RV_DNS_SRV_TYPE,
	RV_DNS_NAPTR_TYPE,
    /* one-way codes: status reports from DNS resolver to application */
    RV_DNS_STATUS_TYPE,
    RV_DNS_NEWQID_TYPE,
    RV_DNS_ENDOFLIST_TYPE
} RvDnsQueryType;


/* the following 2 structures are delivered to the user callback
   for each SRV / NAPTR record */
typedef struct {
	RvChar              targetName[RV_DNS_MAX_NAME_LEN+1];
	RvUint16            port;
	RvUint16            priority;
	RvUint16            weight;
} RvDnsSrvData;

typedef struct {
	RvUint16            order;
	RvUint16            preference;
	RvChar              service[RV_DNS_MAX_NAME_LEN+1];
	RvChar              nextName[RV_DNS_MAX_NAME_LEN+1];
} RvDnsNaptrData;

typedef struct {
    RvInt               recordNumber;
	RvDnsQueryType      dataType;
	union {
		RvAddress       hostAddress;
		RvDnsSrvData    dnsSrvData;
		RvDnsNaptrData  dnsNaptrData;
        RvStatus        status;
        RvUint16        queryId;
	} data;
} RvDnsData;



/*****************************************************************************
 *                           callbacks
 *****************************************************************************/


typedef void (*rvAresCallback) (
	IN  RvDnsEngine*        dnsEngine,
	IN  void*               context,
    IN  RvStatus            status,
    IN  RvUint16            queryId,
    IN  RvUint8*            queryResults,
    IN  RvInt               alen);


/********************************************************************************************
 * RvDnsMemAllocCB
 *
 * Allocates a block of memory (for query and result buffers).
 *
 * INPUT:
 *  context - the user private data provided by the caller of the send query function
 *  reason  - a code to identify the type of the required block
 *  size    - number of bytes to allocate
 * OUTPUT:
 *  memPtr  - the address of a pointer to the memory block
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
typedef RvStatus (*RvDnsMemAllocCB) (
	IN  void*               context,
    IN  RvDnsAllocReason    reason,
	IN  RvSize_t            size,
	OUT void**              memPtr);


/********************************************************************************************
 * RvDnsMemFreeCB
 *
 * Frees a memory block allocated by the RvDnsMemAllocCB callback.
 *
 * INPUT:
 *  context - the user private data provided by the caller of the DNS function,
 *            which calls this callback
 *  reason  - a code to identify the type of the released block
 *  memPtr  - the address of the memory block
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
typedef RvStatus (*RvDnsMemFreeCB) (
	IN  void*               context,
    IN  RvDnsAllocReason    reason,
	IN  void*               memPtr);


 /********************************************************************************************
 * RvDnsNewRecordCB
 *
 * Inserts a new DNS record data to the appropriate list.
 *
 * INPUT:
 *  context    - the user private data provided by the caller of the DNS function,
 *               which calls this callback
 *  queryId    - serial number of the query that originated this reply
 *  dnsData    - structure of type RvDnsData with a new record data to add.
 *               the dataType member indicates the type of data (IPv4/6, SRV or NAPTR)
 *               in the structure
 * RETURNS:
 *  RV_OK on success, other values on failure
 */
typedef RvStatus (*RvDnsNewRecordCB) (
	IN  void*               context,
    IN  RvUint16            queryId,
	IN  RvDnsData*          dnsData);



/* DNS engine */

struct RvDnsEngineStruct {
    /* Configuration data */
    int flags;
    RvInt64 timeout;
    int tries;
    int ndots;
    RvUint16 udp_port;
    RvUint16 tcp_port;
    char **domains;
    int ndomains;
    
    /* Server addresses and communications state */
    struct server_state *servers;
    int nservers;
    
    /* ID to use for next query */
    unsigned short next_id;
    
    /* Active queries */
    struct query *queries;

    /* Memory alloc/free callbacks */
    RvDnsMemAllocCB memAllocCB;
    RvDnsMemFreeCB memFreeCB;
    void*          memCtx;
    
    /* delivery callback for new data elements */
	RvDnsNewRecordCB newRecordCB;

    /* select engine, timer queue and log manager */
    RvSelectEngine *selectEngine;
    RvTimerQueue *timerQueue;
	RvLogMgr *logMgr;
	RvLogSource *dnsSource;

    /* guarding lock for channel structure */
    RvLock lock;
};


struct ares_options {
    int flags;
    RvInt64 timeout;
    int tries;
    int ndots;
    RvUint16 udp_port;
    RvUint16 tcp_port;
    RvAddress *servers;
    int nservers;
    char **domains;
    int ndomains;
#if 0
    char *lookups;
#endif
};

int ares_init(void);
int ares_construct(RvDnsEngine *dnsEngine, struct ares_options *options, int optmask);
int init_by_options(RvDnsEngine *channel, struct ares_options *options, int optmask);
void get_options(RvDnsEngine *channel, RvInt64 *timeout, int *tries, RvAddress *servers,
                 int *nservers, char **domains, int *ndomains);
void ares_destroy(RvDnsEngine *channel);

int ares_query(RvDnsEngine *channel, const char *name, int dnsclass, int type,
               rvAresCallback callback, void *usr_arg, void *mem_arg, unsigned short *id);
int ares_search(RvDnsEngine *channel, const char *name, int dnsclass,
                 int type, rvAresCallback callback, void *arg, unsigned short *id);

int ares_expand_name(const unsigned char *encoded, const unsigned char *abuf,
                     int alen, char *s, int strLen);

#endif /* RV_DNS_ARES */

#endif /* ARES__H */
