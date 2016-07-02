#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>


#include <pcap.h>

#include "sniff_private.h"
#include "parse_api.h"

#include "osal_api.h"
#include "pktbuff.h"

#include "wifiprobe.h"
#include "pktbuff.h"

#include "ap_log.h"
#include "common.h"

#include "parse_public.h"

#define PLURAL_SUFFIX(n)   (((n) != 1) ? "s" : "")


static pcap_t *pd;

SEM_ID g_uiPktsem = 99;

extern char g_acSniffDevName[16];	/* 设备名称最大长度为16 */
extern int	g_iSnapSize;

extern int	g_iPrintFlag;

static SNIFF_INFO_T packet_handle[]={
	{DLT_PRISM_HEADER, prism_if_handle},
};

static SEM_ID semSniffToParseSync = 99;

static void info(int verbose)
{
	struct pcap_stat stat;

	stat.ps_ifdrop = 0;

	if (pcap_stats(pd, &stat) < 0) 
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "pcap_stats: %s\n", pcap_geterr(pd));
		//infoprint = 0;
		return;
	}

	AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "%u packet%s received by filter\n", \
												stat.ps_recv,	\
	    										PLURAL_SUFFIX(stat.ps_recv));
	AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "%u packet%s dropped by kernel\n", \
												stat.ps_drop,	\
	    										PLURAL_SUFFIX(stat.ps_drop));
	if (stat.ps_ifdrop != 0)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "%u packet%s dropped by interface\n", \
													stat.ps_ifdrop,	\
													PLURAL_SUFFIX(stat.ps_ifdrop));
		//putc('\n', stderr);
	}
}


void pcap_cleanup(void)
{
	if (pd != NULL && pcap_file(pd) == NULL) 
	{
		putchar('\n');
		info(1);
	}
}


packet_handle_f lookup_p_handle(int type)
{
	SNIFF_INFO_T *pstPacketHandle;

	for (pstPacketHandle = packet_handle; pstPacketHandle->packet_handle; ++pstPacketHandle)
		if (type == pstPacketHandle->iNdoType)
			return pstPacketHandle->packet_handle;

	return NULL;
	/* NOTREACHED */
}

static void handle_packet_print(u_char *user,
						 		const struct pcap_pkthdr *h,
						 		const u_char *sp)
{
	static int iPacketCount = 0;
	u_int uiHdrLen;
	PACKET_STATISTICS_T stPktStat = {0};

	SNIFF_INFO_T *pstPacketHandle;

	iPacketCount++;

	pstPacketHandle = (SNIFF_INFO_T *)user;
	
	printf("Packet Num: %d", iPacketCount);
	printf(" %s ", ts_format((h->ts.tv_sec)/86400, h->ts.tv_usec));

	uiHdrLen = (*pstPacketHandle->packet_handle)(h, sp, (void *)(&stPktStat));
	
	return ;
}

static void handle_packet_enqueue(u_char *user,
						  		  const struct pcap_pkthdr *h,
						  	  	  const u_char *sp)
{
	static int iNum;
	PPKT_HDR pstPktHdr;
	int iCpPktLen = 0;
	int iRetCode = OSAL_SUCCESS;
	
	//printf("NUM:%d\n", iNum++);

	STATISTIC_PROBE_INC(all_packets);

	pstPktHdr = (PPKT_HDR)pktBuffAlloc();
	if(NULL == pstPktHdr)
	{
		STATISTIC_PROBE_INC(no_pkt_buff);
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "pkt buff alloc failed!\n");
		return ;
	}

	pstPktHdr->pcapLength = h->len;

	STATISTIC_PROBE_INC_VAL(bytes,pstPktHdr->pcapLength);

	if(h->len > PKT_BUFF_SIZE - REF_PCAP_OFFSET - sizeof(struct pcap_pkthdr))
	{
		iCpPktLen = PKT_BUFF_SIZE - REF_PCAP_OFFSET - sizeof(struct pcap_pkthdr);
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "pkt len more than 2048 - 64!\n");
		STATISTIC_PROBE_INC(large_packets);
	}
	else
	{
		iCpPktLen = h->len;
	}
	memcpy(((u_char *)pstPktHdr) + REF_PCAP_OFFSET, (char *)h, sizeof(struct pcap_pkthdr));
	memcpy(((u_char *)pstPktHdr) + REF_PCAP_OFFSET + sizeof(struct pcap_pkthdr), sp, iCpPktLen);
	
	enqueue_pktbuff(pstPktHdr);
	STATISTIC_PROBE_INC(pkt_enqueues);

	iRetCode = OSAL_SemaphoreGive(g_uiPktsem);
	if(OSAL_SUCCESS != iRetCode)
	{
		STATISTIC_PROBE_INC(sniff_semGive_fail);

		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "semaphore failed! = %d\n", iRetCode);
		return ;
	}
	
	return ;
}
void *sniff_handle(void *args)
{
	int	iRcvCnt = -1;
	int iIfType = 0;
	int iArgOp = 0;
	int iStatus = 0;
	
	int iSnapLen = 0;
	int iSnapLenCheck = 0;

	int iPacketHandleType = 1;	/* 报文处理方法标志 */

	int iFilterFlag = 0;
	
	unsigned int uiLocalnet;
	unsigned int uiNetMask;

	char *pcDevice = NULL;
	char *pcUserName = NULL;
	char *pcEnd = NULL;
	char *pcFiterFile = NULL;
	char *filter_exp;
	u_char *ucPcapUserData;
	char acDevName[IF_MAX_LEN] = {0};
	char acErrBuf[PCAP_ERRBUF_SIZE] = {0};
	

	struct bpf_program stFcode;
	pcap_handler callback;  /* pcap_loop的回调函数，用于处理抓取的报文 */
	SNIFF_INFO_T	stSniffInfo;
	#if 0
	/* 解析函数入参，依此获取运行配置参数，如interface、snaplen */
	while((iArgOp = getopt(argc, argv, "i:s:p")) != -1)
	{
		switch(iArgOp)
		{
			case 'i':
				/* 确定抓包端口 */
				strncpy(acDevName, optarg, IF_MAX_LEN - 1);
				acDevName[IF_MAX_LEN - 1] = '\0';
				if(strlen(acDevName) < 1)
				{
					strncpy(acDevName, DEFAULT_IF_NAME, IF_MAX_LEN - 1);
				}
				iDevFlag = 1;
				break;
			case 's':
				iSnapLen = strtol(optarg, &pcEnd, 0);
				
				if((optarg == pcEnd) || (*pcEnd != '\0') \
					|| (iSnapLen < 0) || iSnapLen > MAXIMUM_SNAPLEN)
				{
					AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "invalid snaplen %s", optarg);
				}
				else if(0 == iSnapLen)
				{
					iSnapLen = MAXIMUM_SNAPLEN;
				}
				break;
			case 'p':
				g_iPrintFlag++;
				break;
			default:
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, \
								"wifi-probe [-i interface] [-s snaplen]");
				return -1;
		}

	}
	#endif

	/* 获取同步信号量id */
	OSAL_SemaphoreGetIdByName(&g_uiPktsem, SEM_PARSE_QUEUE_NAME);

	/* 确定物理接口名 */
	if(strlen(g_acSniffDevName) < 1)
	{
		strncpy(acDevName, DEFAULT_IF_NAME, IF_MAX_LEN - 1);
	}
	else
	{
		strncpy(acDevName, g_acSniffDevName, IF_MAX_LEN - 1);
	}
	pcDevice = acDevName;

	iSnapLen = g_iSnapSize;
	if((iSnapLen < 0) || (iSnapLen > MAXIMUM_SNAPLEN) || (iSnapLen == 0))
	{
		iSnapLen = MAXIMUM_SNAPLEN;
	}
	//printf("device name:%s, snaplen:%d\n", pcDevice, iSnapLen);
	AP_LOG(AP_LOG_DEBUG, LOG_MOUDLES_SNIFF, "device name:%s, snaplen:%d\n", 
														pcDevice, iSnapLen);
	/* 创建pcap device */
	pd = pcap_open_live(pcDevice, iSnapLen, 1, 1000, acErrBuf);

	if(NULL == pd)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "%s", acErrBuf);
		return -1;
	}
	else if(*acErrBuf)
	{
		AP_LOG(AP_LOG_WARNING, LOG_MOUDLES_SNIFF, "%s", acErrBuf);	
	}

	/* 检查pcap 参数是否与入参相同 */
	iIfType = pcap_datalink(pd);
	
	iSnapLenCheck = pcap_snapshot(pd);
	AP_LOG(AP_LOG_WARNING, LOG_MOUDLES_SNIFF, "ifterface type:%d, iSnapLen:%d\n",
															iIfType, 
															iSnapLenCheck);
	
	if (pcap_lookupnet(pcDevice, &uiLocalnet, &uiNetMask, acErrBuf) < 0) 
	{
		uiLocalnet = 0;
		uiNetMask = 0;
		AP_LOG(AP_LOG_WARNING, LOG_MOUDLES_SNIFF, "%s\n", acErrBuf);
	}
#if	FILTER_FUNC /* filter function close */
	/* 设置过滤规则 */
	if(NULL != pcFiterFile)
	{
		filter_exp = read_file_malloc_buf(pcFiterFile);
	}
	else
	{
		filter_exp = copy_argv_malloc_buf(&argv[optind]);
	}

	if (pcap_compile(pd, &stFcode, filter_exp, 0, uiNetMask) < 0)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "error %s\n", pcap_geterr(pd));
		iFilterFlag = 1; /* filter compile failed */
	}
	if(NULL != filter_exp)
	{
		AP_LOG(AP_LOG_INFO, LOG_MOUDLES_SNIFF, "Filter:%s\n", filter_exp);
		free(filter_exp);
	}

	if(0 == iFilterFlag)
	{
		if (pcap_setfilter(pd, &stFcode) < 0)
		{
			AP_LOG(AP_LOG_ERR, LOG_MOUDLES_SNIFF, "error %s\n", pcap_geterr(pd));
		}
	}
#endif
	/* 选定回调函数 */
	stSniffInfo.iNdoType = iIfType;
	stSniffInfo.packet_handle = lookup_p_handle(iIfType);
	ucPcapUserData = (u_char *)&stSniffInfo;

	
	//iPacketHandleType = 1;
	if(PACKET_HANDLE_METHOD_PRINT == g_iPrintFlag)
	{
		callback = handle_packet_print;
	}
	else
	{
		callback = handle_packet_enqueue;
	}

	/* pcap_loop */
	iStatus = pcap_loop(pd, iRcvCnt, callback, ucPcapUserData);

	pcap_close(pd);
	
	return 0;
}

int SNIFF_init(void)
{

	INT32 retcode = STATUS_SUCCESS;
	
	return STATUS_SUCCESS;
}

