#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DNS_COMMON_H_
#define _DNS_COMMON_H_

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



#define DNS__16BIT(p)			(UINT16)(((UINT8)(p)[0] << 8) | (UINT8)(p)[1])
#define DNS__32BIT(p)			(((UINT8)(p)[0] << 24) | ((UINT8)(p)[1] << 16) | \
                                ((UINT8)(p)[2] << 8) | (UINT8)(p)[3])
#define DNS__SET16BIT(p, v)		(((p)[0] = (UINT8)(((v) >> 8) & 0xff)), \
                                ((p)[1] = (UINT8)((v) & 0xff)))
#define DNS__SET32BIT(p, v)		(((p)[0] = ((v) >> 24) & 0xff), \
                                ((p)[1] = ((v) >> 16) & 0xff), \
                                ((p)[2] = ((v) >> 8) & 0xff), \
                                ((p)[3] = (v) & 0xff))

/* Macros for parsing a DNS header */
#define DNS_HEADER_QID(h)		DNS__16BIT(h)
#define DNS_HEADER_QR(h)		(((h)[2] >> 7) & 0x1)
#define DNS_HEADER_OPCODE(h)		(((h)[2] >> 3) & 0xf)
#define DNS_HEADER_AA(h)		(((h)[2] >> 2) & 0x1)
#define DNS_HEADER_TC(h)		(((h)[2] >> 1) & 0x1)
#define DNS_HEADER_RD(h)		((h)[2] & 0x1)
#define DNS_HEADER_RA(h)		(((h)[3] >> 7) & 0x1)
#define DNS_HEADER_Z(h)			(((h)[3] >> 4) & 0x7)
#define DNS_HEADER_RCODE(h)		((h)[3] & 0xf)
#define DNS_HEADER_QDCOUNT(h)		DNS__16BIT((h) + 4)
#define DNS_HEADER_ANCOUNT(h)		DNS__16BIT((h) + 6)
#define DNS_HEADER_NSCOUNT(h)		DNS__16BIT((h) + 8)
#define DNS_HEADER_ARCOUNT(h)		DNS__16BIT((h) + 10)

/* Macros for constructing a DNS header */
#define DNS_HEADER_SET_QID(h, v)	DNS__SET16BIT(h, v)
#define DNS_HEADER_SET_QR(h, v)		((h)[2] |= (((v) & 0x1) << 7))
#define DNS_HEADER_SET_OPCODE(h, v)	((h)[2] |= (((v) & 0xf) << 3))
#define DNS_HEADER_SET_AA(h, v)		((h)[2] |= (((v) & 0x1) << 2))
#define DNS_HEADER_SET_TC(h, v)		((h)[2] |= (((v) & 0x1) << 1))
#define DNS_HEADER_SET_RD(h, v)		((h)[2] |= (((v) & 0x1)))
#define DNS_HEADER_SET_RA(h, v)		((h)[3] |= (((v) & 0x1) << 7))
#define DNS_HEADER_SET_Z(h, v)		((h)[3] |= (((v) & 0x7) << 4))
#define DNS_HEADER_SET_RCODE(h, v)	((h)[3] |= (((v) & 0xf)))
#define DNS_HEADER_SET_QDCOUNT(h, v)	DNS__SET16BIT((h) + 4, v)
#define DNS_HEADER_SET_ANCOUNT(h, v)	DNS__SET16BIT((h) + 6, v)
#define DNS_HEADER_SET_NSCOUNT(h, v)	DNS__SET16BIT((h) + 8, v)
#define DNS_HEADER_SET_ARCOUNT(h, v)	DNS__SET16BIT((h) + 10, v)

/* Macros for parsing the fixed part of a DNS question */
#define DNS_QUESTION_TYPE(q)		DNS__16BIT(q)
#define DNS_QUESTION_CLASS(q)		DNS__16BIT((q) + 2)

/* Macros for constructing the fixed part of a DNS question */
#define DNS_QUESTION_SET_TYPE(q, v)	DNS__SET16BIT(q, v)
#define DNS_QUESTION_SET_CLASS(q, v)	DNS__SET16BIT((q) + 2, v)

/* Macros for parsing the fixed part of a DNS resource record */
#define DNS_RR_TYPE(r)			DNS__16BIT(r)
#define DNS_RR_CLASS(r)			DNS__16BIT((r) + 2)
#define DNS_RR_TTL(r)			DNS__32BIT((r) + 4)
#define DNS_RR_LEN(r)			DNS__16BIT((r) + 8)

/* Macros for constructing the fixed part of a DNS resource record */
#define DNS_RR_SET_TYPE(r)		DNS__SET16BIT(r, v)
#define DNS_RR_SET_CLASS(r)		DNS__SET16BIT((r) + 2, v)
#define DNS_RR_SET_TTL(r)		DNS__SET32BIT((r) + 4, v)
#define DNS_RR_SET_LEN(r)		DNS__SET16BIT((r) + 8, v)

#endif // _DNS_COMMON_H_


#ifdef __cplusplus
}
#endif
