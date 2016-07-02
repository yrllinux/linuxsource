#ifndef __PARSE_API_PRIVATE_H__
#define __PARSE_API_PRIVATE_H__

extern int llc_parse(const u_char *p, u_int length, u_int caplen, PACKET_STATISTICS_T *pstStaticOutPut);
extern void ip_parse(const u_char *bp, u_int length, u_int caplen, PACKET_STATISTICS_T *pstStaticOutPut);
extern void ip6_parse(const u_char *bp, u_int length, u_int caplen, PACKET_STATISTICS_T *pstStaticOutPut);
extern void tcp_parse(const u_char *bp, u_int length, const u_char *bp2, int fragmented, PACKET_STATISTICS_T *pstStaticOutPut);

#endif /* __PARSE_API_PRIVATE_H__ */
