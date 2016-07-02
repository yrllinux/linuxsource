#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_public.h"
#include "parse_api_private.h"
#include "parse_ip.h"
#include "ap_log.h"
#include "wifiprobe.h"


struct ip_demux_state {
	const struct ip *ip;
	const u_char *cp;
	u_int   len, off;
	u_char  nh;
	int     advance;
};

u_int16_t ipcheck_sum(void *buff, int count)
{
	u_int32_t sum = 0;
	u_int16_t *source = (u_int16_t *) buff;
	
	while (count > 1)  {
		sum += *source++;
		count -= 2;
	}
	
	if (count > 0) {
		u_int16_t tmp = 0;
		*(u_char*)&tmp = *(u_char*)source;
		sum += tmp;
	}
	
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (u_int16_t)(~sum);

}


void ip_parse(const u_char *bp, 
				u_int length, 
				u_int caplen, 
				PACKET_STATISTICS_T *pstStaticOutPut)
{
	struct ip_demux_state stIpd;
	struct ip_demux_state *pstIpds = &stIpd;

	u_int	uiHdrlen;	
	
	pstIpds->ip = (const struct ip *)bp;

	if(length < sizeof(struct ip))
	{
		return ;
	}

	uiHdrlen = IP_HL(pstIpds->ip) * 4;
	if(uiHdrlen < sizeof(struct ip))
	{
		return;
	}

	if(ipcheck_sum((void*)pstIpds->ip, uiHdrlen) != 0)
	{
		return ;
	}

	pstIpds->len = EXTRACT_16BITS(&pstIpds->ip->ip_len);

	if (pstIpds->len < uiHdrlen) 
	{
	#ifdef GUESS_TSO
        if (pstIpds->len) 
		{
            return;
        }
        else 
		{
            /* we guess that it is a TSO send */
            pstIpds->len = length;
        }
	#else
        return;
	#endif /* GUESS_TSO */
	}
	pstIpds->len -= uiHdrlen;

	pstIpds->off = EXTRACT_16BITS(&pstIpds->ip->ip_off);


	/*
	 * If this is fragment zero, hand it to the next higher
	 * level protocol.
	 */
	if ((pstIpds->off & 0x1fff) == 0) {
		pstIpds->cp = (const u_char *)pstIpds->ip + uiHdrlen;
		pstIpds->nh = pstIpds->ip->ip_p;

		again:
			switch (pstIpds->nh) 
			{
				case IPPROTO_TCP:
					/* pass on the MF bit plus the offset to detect fragments */
					STATISTIC_PROBE_INC(tcp);
					tcp_parse(pstIpds->cp, 
							  pstIpds->len, 
							  (const u_char *)pstIpds->ip,
						  	  pstIpds->off & (IP_MF|IP_OFFMASK), 
						  	  pstStaticOutPut);
					break;
				case IPPROTO_UDP:
					STATISTIC_PROBE_INC(udp);
					break;
				default :
					STATISTIC_PROBE_INC(ipproto_others);
					//printf("proto: %x\n", ipds->nh);
					break;
			}
	}
	else
	{
		;
	}
	return ;
}

