
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_public.h"
#include "parse_api_private.h"
#include "parse_ip6.h"
#include "ap_log.h"
#ifdef INET6

/* ip6报文解析有问题 */
void ip6_parse(const u_char *bp, 
				u_int length, 
				u_int caplen, 
				PACKET_STATISTICS_T *pstStaticOutPut)
{
	const struct ip6_hdr *pstIp6;

	u_int	uiPayLoadLen;
	u_int	uiLen;

	int iNextHdr;
	int iAdvance;
	int fragmented = 0;
	const u_char *pucBpCp;
	const u_char *pucIPend;

	pstIp6 = (const struct ip6_hdr *)bp;

	if(length < sizeof(struct ip6_hdr))
	{
		return;
	}

	uiPayLoadLen = EXTRACT_16BITS(&pstIp6->ip6_plen);
	uiLen = uiPayLoadLen + sizeof(struct ip6_hdr);

	pucIPend = bp + uiLen;

	pucBpCp = (const u_char *)pstIp6;
	iAdvance = sizeof(struct ip6_hdr);
	iNextHdr = pstIp6->ip6_nxt;

	while(pucBpCp < pucIPend && iAdvance > 0)
	{
		pucBpCp += iAdvance;
		uiLen -= iAdvance;
		switch(iNextHdr)
		{
			case IPPROTO_TCP:
				tcp_parse(pucBpCp, uiLen, (const u_char *)pstIp6, fragmented, pstStaticOutPut);
				return;
			default:
				return;
		}
	}
	return ;
}

#endif

