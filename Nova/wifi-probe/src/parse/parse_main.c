#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ap_log.h"

#include "osal_api.h"
#include "pktbuff.h"

#include "parse_api.h"

#include "wifiprobe.h"
#include "uart_msg.h"

#include "parse_public.h"



PROBE_FILTER_INFO stProbeFlt = {0};

static SEM_ID semParseFromSniffSync = 99;

extern WIFI_PROBE_T gWifiProbe;
void *PARSE_pthread(void *args)
{

	INT32 retcode = OSAL_SUCCESS;
	INT32 iMacSum = 0;
	INT32 iLoop = 0;
	INT32 iEnqFlag = ENQUEUE_FLAG_ENABLE;
	INT32 iHitFlag = 0;
	INT32 iMacFlag = 0;
	PPKT_HDR pktHdr;
	SEM_ID semID = semParseFromSniffSync;
	
	TASK_ID tid;

	u_char *pcBufTmp = NULL;
	PACKET_STATISTICS_T  stPktStat = {0};
	PPROBE_NODE_INFO probeHdr = NULL;
	
	OSAL_TaskRegister();
	
	tid = OSAL_TaskGetId();
	
	//stProbeFlt.time = gWifiProbe.config.report_time; /* 同一个设备，多长时间报一次 */
	
	while(1){
			
			retcode = OSAL_SemaphoreTake(semID, WAIT_FOREVER);
			if(retcode != OSAL_SUCCESS)
			{
				STATISTIC_PROBE_INC(parse_semTake_fail);
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "semTake err, ret = %d\n", retcode);
				OSAL_TaskDelay(10);
				continue;
			}
	
			/**/
			do{
				pktHdr = dequeue_pktbuff();
				if(NULL == pktHdr)
				{
					//OSAL_TaskDelay(100);
					/*QUEUE EMPTY.*/
					break;
				}

				stProbeFlt.time = get_mac_report_time();/* 同一个设备，多长时间报一次 */
				STATISTIC_PROBE_INC(pkt_dequeues);
				/* 记录tcp头 五元组 */
				stPktStat.pktHdr = pktHdr;

				/* handle parse */
				//printf("tid =%d, dequeue pkt=%p\n",tid, pktHdr);
				pcBufTmp = ((u_char *)pktHdr) + REF_PCAP_OFFSET;
				prism_if_handle((const struct pcap_pkthdr *)pcBufTmp, pcBufTmp + sizeof(struct pcap_pkthdr), (void *)(&stPktStat));

				if(ENQUEUE_FLAG_ENABLE != stPktStat.ucEnqueueFlag)
				{
					/*free*/
					pktBuffFree(pktHdr);
					continue;
				}
				/* 将获取到的数据封装到uart队列中 */
				STATISTIC_PROBE_INC(good_probe_info);
				
				/* 只上报指定MAC设备 */
				if(gWifiProbe.config.probe_fixed_mac_nums > 0)
				{
					for(iLoop = 0; iLoop < gWifiProbe.config.probe_fixed_mac_nums; ++iLoop)
					{
						if((gWifiProbe.config.probe_fixed_mac[iLoop].mac[0] == stPktStat.ucMac[0]) \
							&& (gWifiProbe.config.probe_fixed_mac[iLoop].mac[1] == stPktStat.ucMac[1]) \
							&& (gWifiProbe.config.probe_fixed_mac[iLoop].mac[2] == stPktStat.ucMac[2]) \
							&& (gWifiProbe.config.probe_fixed_mac[iLoop].mac[3] == stPktStat.ucMac[3]) \
							&& (gWifiProbe.config.probe_fixed_mac[iLoop].mac[4] == stPktStat.ucMac[4]) \
							&& (gWifiProbe.config.probe_fixed_mac[iLoop].mac[5] == stPktStat.ucMac[5]))
						{
							iMacFlag = 1;
							break;
						}
					}
					/* 没有找到指定MAC，continue */
					if(iMacFlag != 1)
					{
						/*free*/
						pktBuffFree(pktHdr);
						continue;
					}
				}
				/* 将指定MAC地址过滤 */
				iMacFlag = 0;
				
				/* 通过MAC过滤, 去掉最近几秒重复出现的终端信息 */
				if(0 != stProbeFlt.time)
				{
					iMacSum = stPktStat.ucMac[0] + stPktStat.ucMac[1] +stPktStat.ucMac[2] + stPktStat.ucMac[3] + stPktStat.ucMac[4] + stPktStat.ucMac[5];
					for(iLoop = 0; iLoop < MAC_NUMS_PER_SLOT; ++iLoop)
					{
						if((stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[0] == stPktStat.ucMac[0]) \
						   && (stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[1] == stPktStat.ucMac[1]) \
						   && (stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[2] == stPktStat.ucMac[2]) \
						   && (stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[3] == stPktStat.ucMac[3]) \
						   && (stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[4] == stPktStat.ucMac[4]) \
						   && (stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[5] == stPktStat.ucMac[5]))
						{
							if(stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].timestamp <  stPktStat.uiTimeStamp - stProbeFlt.time)
							{
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].timestamp = stPktStat.uiTimeStamp;
								iEnqFlag = ENQUEUE_FLAG_ENABLE;
							}
							else
							{
								if((DATA_STAT_SUCCESS == stPktStat.ucGot_qq) \
									|| (DATA_STAT_SUCCESS == stPktStat.ucGot_tb) \
									|| (DATA_STAT_SUCCESS == stPktStat.ucGot_wb))
								{
									iEnqFlag = ENQUEUE_FLAG_ENABLE;
								}
								else
								{
									iEnqFlag = ENQUEUE_FLAG_DISABLE;
								}
							}
							iHitFlag = 1;
							stProbeFlt.hit++;
							stProbeFlt.filter_slot[iMacSum].hit++;
						}
					}
					if(1 != iHitFlag)
					{
						iEnqFlag = ENQUEUE_FLAG_ENABLE;
						for(iLoop = 0; iLoop < MAC_NUMS_PER_SLOT; ++iLoop)
						{
							if(stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].timestamp < stPktStat.uiTimeStamp - stProbeFlt.time)
							{
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].timestamp = stPktStat.uiTimeStamp;
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[0] = stPktStat.ucMac[0];
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[1] = stPktStat.ucMac[1];
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[2] = stPktStat.ucMac[2];
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[3] = stPktStat.ucMac[3];
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[4] = stPktStat.ucMac[4];
								stProbeFlt.filter_slot[iMacSum].macfilter[iLoop].mac[5] = stPktStat.ucMac[5];							
							}
						}
					}
				}
				else
				{
					iEnqFlag = ENQUEUE_FLAG_ENABLE;
				}
				/* 封装待发送消息 */
				if(ENQUEUE_FLAG_ENABLE == iEnqFlag)
				{
					probeHdr = probeBuffAlloc();

					if(NULL != probeHdr)
					{
						//printf("tid = %d, alloc probeHdr = %p\n",tid,probeHdr);
						probeHdr->timestamp = stPktStat.uiTimeStamp;
						probeHdr->type = stPktStat.ucType;
						probeHdr->rssi = stPktStat.ucRssi;
						
						probeHdr->mac[0] = stPktStat.ucMac[0];
						probeHdr->mac[1] = stPktStat.ucMac[1];
						probeHdr->mac[2] = stPktStat.ucMac[2];
						probeHdr->mac[3] = stPktStat.ucMac[3];
						probeHdr->mac[4] = stPktStat.ucMac[4];
						probeHdr->mac[5] = stPktStat.ucMac[5];
						probeHdr->channel = stPktStat.ucChannel;

						if(DEV_TYPE_AP == stPktStat.ucType)
						{
							sprintf(probeHdr->ssid, "%s", stPktStat.ucSsid);
							switch(stPktStat.uiAuth)
							{
								case 1:
									strcpy(probeHdr->auth, "WPA");
									break;
								case 2:
									strcpy(probeHdr->auth, "WPA2");
									break;
								case 3:
									strcpy(probeHdr->auth, "WPA/WPA2");
									break;
								default :
									strcpy(probeHdr->auth, "OPEN");
									break;
							}
							switch(stPktStat.uiEncry)
							{
								case 8:
									strcpy(probeHdr->encry, "TKIP");
									break;
								case 16:
									strcpy(probeHdr->encry, "CCMP");
									break;
								case 24:
									strcpy(probeHdr->encry, "TKIP/CCMP");
									break;
								default :
									strcpy(probeHdr->encry, "NONE");
									break;
							}
							
						}
						else if(DEV_TYPE_STA == stPktStat.ucType)
						{
							if(DATA_STAT_SUCCESS == stPktStat.ucGot_qq)
							{
								sprintf(probeHdr->qq, "%s", stPktStat.ucQq);
								probeHdr->got_qq = 1;
							}
							if(DATA_STAT_SUCCESS == stPktStat.ucGot_tb)
							{
								sprintf(probeHdr->tb, "%s", stPktStat.ucTb);
								probeHdr->got_tb = 1;
							}
							if(DATA_STAT_SUCCESS == stPktStat.ucGot_wb)
							{
								sprintf(probeHdr->wb, "%s", stPktStat.ucWb);
								probeHdr->got_wb = 1;
							}
						}
						enqueue_probebuff(probeHdr);
						STATISTIC_PROBE_INC(probe_enqueues);
					}
					else
					{
						/*NO PROBE BUFF.*/
						STATISTIC_PROBE_INC(no_probe_buff);
					}
				}
				memset(&stPktStat, 0, sizeof(PACKET_STATISTICS_T));
				iEnqFlag = ENQUEUE_FLAG_ENABLE;
				iHitFlag = 0;
				/*free*/
				pktBuffFree(pktHdr);
			}while(1);
			
		}

}

INT32 PARSE_init(void)
{
	INT32 retcode = STATUS_SUCCESS;
	
	retcode = OSAL_SemaphoreGetIdByName(&semParseFromSniffSync,SEM_PARSE_QUEUE_NAME);
	if(retcode != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_PARSE,"OSAL_QueueGetIdByName return =%d\n", retcode);
		return STATUS_ERROR;
	}	
	
	return STATUS_SUCCESS;
}

