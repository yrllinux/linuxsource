#ifndef __PARSE_TCP_H__
#define __PARSE_TCP_H__


typedef	u_int32_t	tcp_seq;
/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
struct tcphdr {
	u_int16_t	th_sport;		/* source port */
	u_int16_t	th_dport;		/* destination port */
	tcp_seq		th_seq;			/* sequence number */
	tcp_seq		th_ack;			/* acknowledgement number */
	u_int8_t	th_offx2;		/* data offset, rsvd */
	u_int8_t	th_flags;
	u_int16_t	th_win;			/* window */
	u_int16_t	th_sum;			/* checksum */
	u_int16_t	th_urp;			/* urgent pointer */
};

#define TH_OFF(th)	(((th)->th_offx2 & 0xf0) >> 4)

#ifndef TELNET_PORT
#define TELNET_PORT             23
#endif
#ifndef BGP_PORT
#define BGP_PORT                179
#endif
#define NETBIOS_SSN_PORT        139
#ifndef PPTP_PORT
#define PPTP_PORT	        1723
#endif
#define BEEP_PORT               10288
#ifndef NFS_PORT
#define NFS_PORT	        2049
#endif
#define MSDP_PORT	        639
#define RPKI_RTR_PORT	        2222 /* experimental up until sidr-wg registers a well-known port */
#define LDP_PORT                646
#ifndef SMB_PORT
#define SMB_PORT                445
#endif
#ifndef HTTP_PORT
#define HTTP_PORT		80
#endif
#ifndef HTTP_PORT_OTHER
#define HTTP_PORT_OTHER	8080
#endif
#ifndef HTTP_PORT_ANTHER
#define HTTP_PORT_ANTHER	443
#endif



#endif /* __PARSE_TCP_H__ */

