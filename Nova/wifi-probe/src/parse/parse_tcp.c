#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_public.h"
#include "parse_ip.h"
#ifdef INET6
#include "parse_ip6.h"
#endif
#include "parse_tcp.h"
#include "ap_log.h"

#include "wifiprobe.h"

#define QQ_DATA_CODE_A0_OFFSET	78
#define QQ_DATA_CODE_60_OFFSET  10
#define QQ_DATA_CODE_MIN_LEN    20

extern int http_parse(char *bp, u_int length, PACKET_STATISTICS_T *pstStaticOutPut);
extern int is_http(const char *buf, int rlen);
extern int http_data_parse(char *pData, int len, PACKET_STATISTICS_T *pstStaticOutPut);


static int isQQ_port(u_int16_t port)
{
	int iLoop = 0;
		
	if(0 == gWifiProbe.config.qqport[0])
	{
		if((port == HTTP_PORT) || 
			(port == HTTP_PORT_OTHER) || 
			(port == HTTP_PORT_ANTHER) ||
			(port == 14000))
			return 1;
	}
	else
	{
		for(iLoop = 0; iLoop < MAX_QQ_PORTS; ++iLoop)
		{
			if(port == gWifiProbe.config.qqport[iLoop])
			{
				return 1;
			}
		#if 0
			if(0 == gWifiProbe.config.qqport[iLoop])
			{
				break;
			}
		#endif
		}
	}
	return 0;
}


void tcp_parse(const u_char *bp, 
				u_int length, 
				const u_char *bp2, 
				int fragmented, 
				PACKET_STATISTICS_T *pstStaticOutPut)
{
	u_int uiDataLen = 0;
	u_int uiHdrLen = 0;
	u_int uiFlag = 0;
	u_int uiLoop = 0;
	u_int16_t usSport = 0;
	u_int16_t usDport = 0;
	
	const u_char *puiQQTmp = NULL;
	char  acQQCode[16] = {0};

	const struct tcphdr *tp;
	const struct ip *ip;
#ifdef INET6
	const struct ip6_hdr *ip6;
#endif	

	tp = (struct tcphdr *)bp;
	ip = (struct ip *)bp2;
#ifdef INET6
	if (IP_V(ip) == 6)
	{
		ip6 = (struct ip6_hdr *)bp2;
	}
	else
	{
		ip6 = NULL;
	}
#endif /*INET6*/
	/* IPV6不解析, 暂时只解析IPV4报文 */
	if(4 == IP_V(ip))
	{
		pstStaticOutPut->pktHdr->tuple.srcIp = ip->ip_src.s_addr;
		pstStaticOutPut->pktHdr->tuple.dstIp = ip->ip_dst.s_addr;
	}
	
	usSport = EXTRACT_16BITS(&tp->th_sport);
    usDport = EXTRACT_16BITS(&tp->th_dport);

	pstStaticOutPut->pktHdr->tuple.srcPort = usSport;
	pstStaticOutPut->pktHdr->tuple.dstPort = usDport;
	pstStaticOutPut->pktHdr->tuple.sequence = EXTRACT_32BITS(&tp->th_seq);
	pstStaticOutPut->pktHdr->tuple.proto = IPPROTO_TCP;

	uiHdrLen = TH_OFF(tp) * 4;
	/*
     * Decode payload if necessary.
     */
    bp += TH_OFF(tp) * 4;

	uiDataLen = length - uiHdrLen;
	uiFlag = 0;

	if(uiDataLen == 0)
	{
		return ;
	}

	if((HTTP_PORT == usSport) || (usDport == HTTP_PORT))
	{
		if(is_http(bp, uiDataLen))
		{
			/*http protocal.*/
			STATISTIC_PROBE_INC(http80);
			http_parse((char *)bp, uiDataLen, pstStaticOutPut);
			return;
		}
		else
		{
			/*http data.*/			
			STATISTIC_PROBE_INC(http_data);
			if(http_data_parse((char *)bp, uiDataLen, pstStaticOutPut) == 0)
				return ;
		}
	}

	if(isQQ_port(usSport))
	{
		if(uiDataLen < QQ_DATA_CODE_60_OFFSET + QQ_DATA_CODE_MIN_LEN)
		{
			return;
		}
		else
		{
			puiQQTmp = bp + QQ_DATA_CODE_60_OFFSET;
			if((puiQQTmp[0] == 0x00) \
				&& (puiQQTmp[1] == 0x00) \
				&& (puiQQTmp[2] == 0x00) \
				&& ((puiQQTmp[3] == 0x0d) \
				|| (puiQQTmp[3] == 0x0e)))
			{
				uiFlag = 1;
				uiLoop = 0;
				//while((puiQQTmp[uiLoop + 4] > 0x2f) \
				//  	&& (puiQQTmp[uiLoop + 4] < 0x3a))
				for(uiLoop = 0; uiLoop < puiQQTmp[3] - 4; ++uiLoop)
				{
					//acQQCode[uiLoop] = puiQQTmp[uiLoop + 8] - 0x30;
					sprintf(acQQCode, "%s%x", acQQCode, \
											  puiQQTmp[uiLoop + 4] - 0x30);
					//printf(" %c %02x ", uiLoop, acQQCode[uiLoop], puiQQTmp[uiLoop + 8]);
				}
				
				if(uiLoop > (puiQQTmp[3]-4))
					uiLoop = (puiQQTmp[3]-4);
				
				acQQCode[uiLoop] = '\0';
			}
		}
	}
	else if(isQQ_port(usDport))
	{
		if(uiDataLen < QQ_DATA_CODE_60_OFFSET + QQ_DATA_CODE_MIN_LEN)
		{
			return;
		}
		else
		{
			puiQQTmp = bp + QQ_DATA_CODE_60_OFFSET + 4;
			if((puiQQTmp[0] == 0x00) \
				&& (puiQQTmp[1] == 0x00) \
				&& (puiQQTmp[2] == 0x00) \
				&& ((puiQQTmp[3] == 0x0d) \
				|| (puiQQTmp[3] == 0x0e) \
				|| (puiQQTmp[3] == 0x0f)))
			{
				uiFlag = 1;
				uiLoop = 0;
				//for(uiQQLoop = 0; uiQQLoop < puiQQTmp[3] - 4; ++uiQQLoop)
				//while((puiQQTmp[uiLoop + 4] > 0x2f) \
				//	     && (puiQQTmp[uiLoop + 4] < 0x3a))
				for(uiLoop = 0; uiLoop < puiQQTmp[3] - 4; ++uiLoop)
				{
					//acQQCode[uiLoop] = puiQQTmp[uiLoop + 8] - 0x30;
					sprintf(acQQCode, "%s%x", acQQCode, puiQQTmp[uiLoop + 4] - 0x30);
					//printf(" %c %02x ", uiLoop, acQQCode[uiLoop], puiQQTmp[uiLoop + 8]);
				}
				
				if(uiLoop > (puiQQTmp[3]-4))
					uiLoop = (puiQQTmp[3]-4);
				
				acQQCode[uiLoop] = '\0';
			}
		}

		if(uiFlag != 1)
		{
			if(uiDataLen < QQ_DATA_CODE_60_OFFSET + QQ_DATA_CODE_MIN_LEN + 40)
			{
				return;
			}
			else
			{
				puiQQTmp = bp + QQ_DATA_CODE_60_OFFSET + 44;
				if((puiQQTmp[0] == 0x00)
					&& (puiQQTmp[1] == 0x00) \
					&& (puiQQTmp[2] == 0x00) \
					&& ((puiQQTmp[3] == 0x0d) \
					|| (puiQQTmp[3] == 0x0e) \
					|| (puiQQTmp[3] == 0x0f)))
				{
					uiFlag = 1;
					uiLoop = 0;
					//while((puiQQTmp[uiLoop + 4] > 0x2f) \
					//	  && (puiQQTmp[uiLoop + 4] < 0x3a))
					for(uiLoop = 0; uiLoop < puiQQTmp[3] - 4; ++uiLoop)
					{
						//acQQCode[uiLoop] = puiQQTmp[uiLoop + 8] - 0x30;
						sprintf(acQQCode, "%s%x", acQQCode, puiQQTmp[uiLoop + 4] - 0x30);
						//printf(" %c %02x ", uiLoop, acQQCode[uiLoop], puiQQTmp[uiLoop + 8]);
					}
					if(uiLoop > (puiQQTmp[3]-4))
						uiLoop = (puiQQTmp[3]-4);
					acQQCode[uiLoop] = '\0';
				}
			}
		}

		if(uiFlag != 1)
		{
			if(uiDataLen < QQ_DATA_CODE_A0_OFFSET + QQ_DATA_CODE_MIN_LEN)
			{
				return;
			}
			else
			{
				//printf(" A0_OFFSET ");
				puiQQTmp = bp + QQ_DATA_CODE_A0_OFFSET;
				if((puiQQTmp[0] == 0x00) \
					&& (puiQQTmp[1] == 0x00) \
					&& (puiQQTmp[2] == 0x00) \
					&& ((puiQQTmp[3] == 0x0d) \
					|| (puiQQTmp[3] == 0x0e \
					|| (puiQQTmp[3] == 0x0f))))
				{
					uiFlag = 1;
					uiLoop = 0;
					//while((puiQQTmp[uiLoop + 4] > 0x2f) \
					//   	&& (puiQQTmp[uiLoop + 4] < 0x3a))
					for(uiLoop = 0; uiLoop < puiQQTmp[3] - 4; ++uiLoop)
					{
						sprintf(acQQCode, "%s%x", acQQCode, puiQQTmp[uiLoop + 4] - 0x30);
						//printf(" %c %02x ", uiLoop, acQQCode[uiLoop], puiQQTmp[uiLoop + 8]);
					}
				}

				if(uiLoop > (puiQQTmp[3]-4))
					uiLoop = (puiQQTmp[3]-4);
				
				acQQCode[uiLoop] = '\0';
				//printf(" %02x%02x%02x%02x%02x%02x%02x%02x ", puiQQTmp[0], puiQQTmp[1], puiQQTmp[2], puiQQTmp[3], puiQQTmp[4], puiQQTmp[5], puiQQTmp[6], puiQQTmp[7]);
			}
			
		}
	}

	if(1 == uiFlag)
	{
		pstStaticOutPut->ucGot_qq = DATA_STAT_SUCCESS;
		strcpy(pstStaticOutPut->ucQq, acQQCode);
	}
	return ;
}

