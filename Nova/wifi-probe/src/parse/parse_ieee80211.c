/******************************************************************************

                  版权所有 (C), 2012-2016, 纳瓦电子（上海）有限公司

 ******************************************************************************
  文 件 名   : parse_ieee80211.c
  版 本 号   : 初稿
  作    者   : kevin
  生成日期   : 2016年4月14日
  最近修改   :
  功能描述   : 解析无线报文头
  函数列表   :
  修改历史   :
  1.日    期   : 2016年4月14日
    作    者   : kevin
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pcap.h>

#include "parse_wifi.h"
#include "parse_public.h"
#include "parse_api_private.h"
#include "ap_log.h"
#include "wifiprobe.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

extern unsigned int g_uiChanMask;


static int extract_header_length(u_int16_t fc)
{
	int len;

	switch (FC_TYPE(fc)) {
	case WLAN_FC_TYPE_MGMT:
		return MGMT_HDRLEN;
	case WLAN_FC_TYPE_CTRL:
		switch (FC_SUBTYPE(fc)) {
		case WLAN_FC_STYPE_BAR:
			return CTRL_BAR_HDRLEN;
		case WLAN_FC_STYPE_PSPOLL:
			return CTRL_PS_POLL_HDRLEN;
		case WLAN_FC_STYPE_RTS:
			return CTRL_RTS_HDRLEN;
		case WLAN_FC_STYPE_CTS:
			return CTRL_CTS_HDRLEN;
		case WLAN_FC_STYPE_ACK:
			return CTRL_ACK_HDRLEN;
		case WLAN_FC_STYPE_CFEND:
			return CTRL_END_HDRLEN;
		case WLAN_FC_STYPE_CFENDACK:
			return CTRL_END_ACK_HDRLEN;
		default:
			return 0;
		}
	case WLAN_FC_TYPE_DATA:
		len = (FC_TO_DS(fc) && FC_FROM_DS(fc)) ? 30 : 24;
		if (DATA_FRAME_IS_QOS(FC_SUBTYPE(fc)))
			len += 2;
		return len;
	default:
		//printf("unknown IEEE802.11 frame type (%d)", FC_TYPE(fc));
		return 0;
	}
}

static int extract_mesh_header_length(const u_char *p)
{
	return (p[0] &~ 3) ? 0 : 6*(1 + (p[0] & 3));
}

static int remova_valid_mac(const u8 *pucSrcMac)
{
	if(1 == pucSrcMac[0] & 0x01)
	{
		return 1;
	}
	
	if((pucSrcMac[0] == 0x0) && (pucSrcMac[1] == 0x0) \
		&& (pucSrcMac[2] == 0x0) && (pucSrcMac[3] == 0x0) \
		&& (pucSrcMac[4] == 0x0) && (pucSrcMac[5] == 0x0))
	{
		return 1;
	}
	
	if((pucSrcMac[0] == 0xFF) && (pucSrcMac[1] == 0xFF) \
		&& (pucSrcMac[2] == 0xFF) && (pucSrcMac[3] == 0xFF) \
		&& (pucSrcMac[4] == 0xFF) && (pucSrcMac[5] == 0xFF))
	{
		return 1;
	}
	
	return 0;
}
static void mac_cp(u8 *puDstMac, const u8 *pucSrcMac)
{
	int iLoop;
	
	if(NULL == puDstMac || NULL == pucSrcMac)
	{
		return ;
	}
	
	for(iLoop = 0; iLoop < 6; ++iLoop)
	{
		puDstMac[iLoop] = pucSrcMac[iLoop];
	}
	return ;
}
static void print_mac(u8 *pucMac)
{
	printf("%02x:%02x:%02x:%02x:%02x:%02x ",   \
									pucMac[0], \
									pucMac[1], \
									pucMac[2], \
									pucMac[3], \
									pucMac[4], \
									pucMac[5]);
	return ;
}
static int rsn_selector_to_bitfield(const u_int8_t *s)
{
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_NONE)
		return WPA_CIPHER_NONE;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_WEP40)
		return WPA_CIPHER_WEP40;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_TKIP)
		return WPA_CIPHER_TKIP;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_CCMP)
		return WPA_CIPHER_CCMP;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_WEP104)
		return WPA_CIPHER_WEP104;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_AES_128_CMAC)
		return WPA_CIPHER_AES_128_CMAC;
	return 0;
}

static int wpa_selector_to_bitfield(const u_int8_t *s)
{
#if 0
	printf("######%d#####", RSN_SELECTOR_GET(s));
	printf("%d %d %d %d %d\n", WPA_CIPHER_SUITE_NONE, \
							WPA_CIPHER_SUITE_WEP40, \
							WPA_CIPHER_SUITE_TKIP, \
							WPA_CIPHER_SUITE_CCMP, \
							WPA_CIPHER_SUITE_WEP104);
#endif
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_NONE)
		return WPA_CIPHER_NONE;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_WEP40)
		return WPA_CIPHER_WEP40;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_TKIP)
		return WPA_CIPHER_TKIP;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_CCMP)
		return WPA_CIPHER_CCMP;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_WEP104)
		return WPA_CIPHER_WEP104;
	return 0;
}


static int mgmt_elements_parse(const u_char *p, 
							   int offset,
    						   u_int length,
    						   PACKET_STATISTICS_T *pstStaticOutPut)
{
	u_int uiElementLen = 0;
	u_char aucSSid[33] = {0}; /* SSID最长32个字节 */
	u_int uiElementId = 0;
	u_int uiRsnCount = 0;
	u_int uiLoop = 0;
	
	u_int uiPairWiseCipher = 0;
	u_int uiProto = 0;
	u_int uiChannel = 0;
	
	const u_char *pucPtmp = NULL;
		
	if(NULL == pstStaticOutPut)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "input null pointer!\n");
		return -1;
	}
	
	/* start parse */
	while(length >= 2)
	{
		uiElementId = *(p + offset);
		uiElementLen = *(p + offset + 1);
		offset += 2;
		length -= 2;
		#if 0
		printf("EleId:%d len:%d \n", uiElementId, uiElementLen);
		#endif	
		if(length < uiElementLen)
		{
			AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "element parse failed(id=%d elen=%d "
											 "length=%d)!\n", 
											 uiElementId, uiElementLen, length);
			return -2;
		}
		
		switch(uiElementId)
		{
			case WLAN_EID_SSID:
				
				if(uiElementLen != 0)
				{
					if(uiElementLen > sizeof(aucSSid) - 1)
					{
						AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE,"SSID Len[%d] > 32!\n",
														uiElementLen);
						return -1;
					}
					memcpy(aucSSid, p + offset, uiElementLen);

					aucSSid[uiElementLen] = '\0';
					strcpy(pstStaticOutPut->ucSsid, aucSSid);
					
				}
				memcpy(aucSSid, p + offset, 2);
				break;
			case WLAN_EID_DS_PARAMS:
				if(uiElementLen != 0)
				{
					uiChannel = *(p + offset);
					pstStaticOutPut->ucRealChannel = uiChannel;
				}
				break;
			case WLAN_EID_RSN:
				if(uiElementLen != 0)
				{
					pucPtmp = p + offset + 2 + 4;
					uiRsnCount = EXTRACT_LE_16BITS(pucPtmp);
					if(uiRsnCount == 0 || length < uiRsnCount * 4)
					{
						AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "Rsn Count[%d] error, len[%d]!\n", uiRsnCount, length);
						return -2;
					}
					pucPtmp += 2;
					for(uiLoop = 0; uiLoop < uiRsnCount; ++uiLoop)
					{
						/* 获取wpa或rsn的加密算法，使用bit map方式 */
						uiPairWiseCipher |= rsn_selector_to_bitfield(pucPtmp);
						pucPtmp += WPA_SELECTOR_LEN;
					}
					uiProto |= WPA_PROTO_RSN;
				}
				break;
			case WLAN_EID_VENDOR_SPECIFIC:
				if(uiElementLen != 0)
				{
					pucPtmp = p + offset; /* ELEMENTID + LENGTH */
					if(RSN_SELECTOR_GET(pucPtmp) == WPA_OUI_TYPE)
					{
						pucPtmp = p + offset + 4 + 2 + 4;
						uiRsnCount = EXTRACT_LE_16BITS(pucPtmp);
						if(uiRsnCount == 0 || length < uiRsnCount * 4)
						{
							AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "Rsn Count error!\n");
							return -2;
						}
						pucPtmp += 2;
						for(uiLoop = 0; uiLoop < uiRsnCount; ++uiLoop)
						{
							/* 获取wpa或rsn的加密算法，使用bit map方式 */
							uiPairWiseCipher |= wpa_selector_to_bitfield(pucPtmp);
							pucPtmp += WPA_SELECTOR_LEN;
						}
						uiProto |= WPA_PROTO_WPA;
					}
				}
				break;
			default :
				break;
		}

		offset += uiElementLen;
		length -= uiElementLen;
	}
	pstStaticOutPut->uiAuth = uiProto;
	pstStaticOutPut->uiEncry = uiPairWiseCipher;
	//printf("SSid:%s, proto:%d, pairwise:%d \n", aucSSid, uiProto, uiPairWiseCipher);
	return 0;
}
static int beacon_parse(const u_char *p, 
						u_int length, 
						PACKET_STATISTICS_T *pstStaticOutPut)
{
	int iOffset = 0;
	int iRet = 0;
	u_int uiLength = 0;

	if(NULL == pstStaticOutPut)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "input null pointer!\n");
		return -1;
	}
	
	if(length < IEEE802_11_TSTAMP_LEN + IEEE802_11_BCNINT_LEN \
	    + IEEE802_11_CAPINFO_LEN)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "frm len too short!\n");
		return -1;
	}
	
	iOffset = IEEE802_11_TSTAMP_LEN + IEEE802_11_BCNINT_LEN + IEEE802_11_CAPINFO_LEN;
	uiLength = length - iOffset;

	iRet = mgmt_elements_parse(p, iOffset, uiLength, pstStaticOutPut);
	if(iRet < 0)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "mgmt_elements_parse return error!\n");
	}
	
	return iRet;

}
static int probe_response_parse(const u_char *p, 
								u_int length, 
								PACKET_STATISTICS_T *pstStaticOutPut)
{
	int iOffset = 0;
	int iRet = 0;
	u_int uiLength = 0;

	if(NULL == pstStaticOutPut)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "input null pointer!\n");
		return -1;
	}
	
	if(length < IEEE802_11_TSTAMP_LEN + IEEE802_11_BCNINT_LEN \
	    + IEEE802_11_CAPINFO_LEN)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "frm len too short!\n");
		return -1;
	}
	
	iOffset = IEEE802_11_TSTAMP_LEN + IEEE802_11_BCNINT_LEN + IEEE802_11_CAPINFO_LEN;
	uiLength = length - iOffset;

	iRet = mgmt_elements_parse(p, iOffset, uiLength, pstStaticOutPut);
	if(iRet < 0)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "mgmt_elements_parse return error!\n");
	}

	return iRet;
}

static int wifi_mgmt_parse(const u_char *p, 
						   u_int16_t fc,
						   u_int hdrlen,
 						   u_int length, 
 						   u_int caplen, 
 						   PACKET_STATISTICS_T *pstStaticOutPut)
{

	const struct mgmt_header  *pstMgmtheader = NULL;

	if(NULL == pstStaticOutPut)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "input null pointer!\n");
		return -1;
	}
	
	pstMgmtheader = (const struct mgmt_header  *)(p - hdrlen);
	
	switch(FC_SUBTYPE(fc))
	{
		case WLAN_FC_STYPE_PROBE_REQ:
			pstStaticOutPut->ucType = DEV_TYPE_STA;
			mac_cp(pstStaticOutPut->ucMac, pstMgmtheader->sa);
			#if 0
			printf("probe request: ");
			print_mac(pstStaticOutPut->ucMac);
			printf("\n");
			#endif
			pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_ENABLE;
			break;
		case WLAN_FC_STYPE_PROBE_RESP:
			pstStaticOutPut->ucType = DEV_TYPE_AP;
			mac_cp(pstStaticOutPut->ucMac, pstMgmtheader->sa);
			#if 0
			print_mac(pstStaticOutPut->ucMac);
			#endif
			probe_response_parse(p, length, pstStaticOutPut);
			
			if(pstStaticOutPut->ucChannel != pstStaticOutPut->ucRealChannel)
			{
				pstStaticOutPut->ucType = DEV_TYPE_STA;
				pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_DISABLE;
			}
			else
			{
				pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_ENABLE;
			}
			break;
		case WLAN_FC_STYPE_BEACON:
			pstStaticOutPut->ucType = DEV_TYPE_AP;
			mac_cp(pstStaticOutPut->ucMac, pstMgmtheader->sa);
			#if 0
			print_mac(pstStaticOutPut->ucMac);
			#endif
			beacon_parse(p, length, pstStaticOutPut);
			
			if(pstStaticOutPut->ucChannel != pstStaticOutPut->ucRealChannel)
			{
				pstStaticOutPut->ucType = DEV_TYPE_STA;
				pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_DISABLE;
			}
			else
			{
				pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_ENABLE;
			}
			break;
		default :
			break;
	}
	return 0;
}
static int wifi_ctrl_parse(const u_char *p, 
						   u_int16_t fc,
						   u_int hdrlen,
 						   u_int length, 
 						   u_int caplen, 
 						   PACKET_STATISTICS_T *pstStaticOutPut)
{
	return 0;
}
/*****************************************************************************
 函 数 名  : wifi_data_parse
 功能描述  : 解析无线data帧，
 输入参数  : const u_char *p                       
             u_int16_t fc                          
             u_int hdrlen 无线数据帧头长度              
             u_int length 无线数据帧payload长度         
             u_int caplen 无线数据帧payload长度    
             PACKET_STATISTICS_T *pstStaticOutPut  
 输出参数  : 无
 返 回 值  : static
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年4月20日
    作    者   : kevin
    修改内容   : 新生成函数

*****************************************************************************/
static int wifi_data_parse(const u_char *p, 
						   u_int16_t fc,
						   u_int hdrlen,
 						   u_int length, 
 						   u_int caplen, 
 						   PACKET_STATISTICS_T *pstStaticOutPut)
{
	const u_int8_t *src;
	const u_int8_t *dst;
	u_int	uiStreamDir = 0;
	
	const u_char *fch;
	
	if(NULL == pstStaticOutPut)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "input null pointer!\n");
		return -1;
	}
	/* 用于获取MAC地址 */
	fch = p - hdrlen;

	pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_DISABLE;
	
	if(FC_TO_DS(fc))
	{
		if(!FC_FROM_DS(fc))
		{
			src = fch + 10;	/* sta mac */
			dst = fch + 16;
		}
		else if(FC_FROM_DS(fc))
		{
			return 0;
		}
		else 
		{
			return -3;
		}
		uiStreamDir = DATA_STREAM_DIR_UP;
	}
	else if(!FC_TO_DS(fc))
	{
		if(FC_FROM_DS(fc))
		{
			src = fch + 4; /* sta MAC */
			dst = fch + 16; /* src MAC */
		}
		else if(!FC_FROM_DS(fc))
		{
			src = fch + 10;
			dst = fch + 4;
		}
		else 
		{
			return -3;
		}
		pstStaticOutPut->ucRssi = 0;
		uiStreamDir = DATA_STREAM_DIR_DOWN;
	}
	pstStaticOutPut->pktHdr->tuple.direction = uiStreamDir;
	/* 去除组播MAC，全F MAC， 全0 MAC */
	if(1 == remova_valid_mac(src))
	{
		pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_DISABLE;
		return 0;
	}
	/* cp dev mac */
	mac_cp(pstStaticOutPut->ucMac, src);

	/* handle null data or encry frame */
	if(DATA_FRAME_IS_NULL(FC_SUBTYPE(fc)) || FC_WEP(fc))
	{
		if(DATA_FRAME_IS_NULL(FC_SUBTYPE(fc)))
			STATISTIC_PROBE_INC(null80211);
		if(FC_WEP(fc))
			STATISTIC_PROBE_INC(encry80211);
		
		if(DATA_STREAM_DIR_UP == uiStreamDir)
		{
			pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_ENABLE;
		}
		return 0;
	}
	else  /* 解析有效数据报文 */
	{
		llc_parse(p, length, caplen, pstStaticOutPut);

		/* 下行流，并且没有获取QQ号，taobao号，weibo号，该数据放弃，不入队 */
		if((DATA_STREAM_DIR_DOWN != uiStreamDir) \
			|| (DATA_STAT_SUCCESS == pstStaticOutPut->ucGot_qq) \
			|| (DATA_STAT_SUCCESS == pstStaticOutPut->ucGot_tb) \
			|| (DATA_STAT_SUCCESS == pstStaticOutPut->ucGot_wb))
		{
			pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_ENABLE;
		}
		else
		{
			pstStaticOutPut->ucEnqueueFlag = ENQUEUE_FLAG_DISABLE;
		}
		return 0;
	}

	

	return 0;
}

static	int parse_wifi_frame(const u_char *p, 
							 u_int length, 
							 u_int orig_caplen, 
							 int pad,
							 u_int fcslen,
							 PACKET_STATISTICS_T *pstStaticOutPut)
{
	u_int uiCaplen;
	u_int uiLength;
	u_int uiHdrlen;
	u_int uiMesHdrlen;
	u_int16_t usFc;
	u_char *ucFch;
	int	  iRet;
	const u_int8_t *src, *dst;
	

	/* 根据帧长度判断无线帧的合法性 */
	uiCaplen = orig_caplen;
	uiLength = length;

	if(uiLength < fcslen)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "frame len[%d] < FCS len[%d]!\n", \
															uiLength, fcslen);
		return -1;
	}
	uiLength -= fcslen;

	/* delete fcs len */
	if(uiCaplen > fcslen)
	{
		fcslen = uiCaplen - uiLength;
		uiCaplen -= fcslen;
	}
	if(uiCaplen < IEEE802_11_FC_LEN)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "frame len[%d] < FC len[%d]!\n", \
													uiCaplen, IEEE802_11_FC_LEN);
		return -1;
	}
	
	/* 提取无线帧帧头长度 */
	usFc = EXTRACT_LE_16BITS(p);
	uiHdrlen = extract_header_length(usFc);
	if(pad)
	{
		uiHdrlen = roundup2(uiHdrlen, 4);
	}
	if(FC_TYPE(usFc) == WLAN_FC_TYPE_DATA && \
	   DATA_FRAME_IS_QOS(FC_SUBTYPE(usFc)))
	{
		uiMesHdrlen = extract_mesh_header_length(p + uiHdrlen);
	}
	else
	{
		uiMesHdrlen = 0;
	}

	if(uiCaplen < uiHdrlen)
	{	
		#if 0
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "frame len[%d] < FC header len[%d], type %d stype %x!\n", \
																uiCaplen, uiHdrlen, FC_TYPE(usFc), FC_SUBTYPE(usFc));
		#endif
		return -1;
	}

	uiLength -= uiHdrlen;
	uiCaplen -= uiHdrlen;
	#if 0
	AP_LOG(AP_LOG_DEBUG, LOG_MOUDLES_PARSE, "caplen[%d] Length[%d] hdrlen[%d],type %d stype %x!!\n", \
													uiCaplen, uiLength, uiHdrlen, FC_TYPE(usFc), FC_SUBTYPE(usFc));
	#endif
	p += uiHdrlen;
	switch(FC_TYPE(usFc))
	{
		case WLAN_FC_TYPE_MGMT:
			STATISTIC_PROBE_INC(mng80211);
			iRet = wifi_mgmt_parse(p, usFc, uiHdrlen, uiLength, uiCaplen, pstStaticOutPut);
			break;
		case WLAN_FC_TYPE_CTRL:
			STATISTIC_PROBE_INC(ctrl80211);
			iRet = wifi_ctrl_parse(p, usFc, uiHdrlen, uiLength, uiCaplen, pstStaticOutPut);
			break;
		case WLAN_FC_TYPE_DATA:
			STATISTIC_PROBE_INC(data80211);
			iRet = wifi_data_parse(p, usFc, uiHdrlen, uiLength, uiCaplen, pstStaticOutPut);
			break;
		default :
			STATISTIC_PROBE_INC(other80211);
			AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "frm type not exit, FC=%x!\n", \
																FC_TYPE(usFc));
			break;
	}
	
	return iRet;
}

static inline int parse_prism_header(const u_char *p, 
										 u_int length, 
										 u_int orig_caplen, 
										 int pad,
										 u_int fcslen,
										 PACKET_STATISTICS_T *pstStaticOutPut)
{
	u_int uiLength; 
	u_int uiCaplen;

	wlan_ng_prism2_header *pstWlanPrismT = NULL;

	pstWlanPrismT = (wlan_ng_prism2_header *)p;
	
	if(NULL == pstWlanPrismT)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "parse_prism_header input null point!\n");
		return -1;
	}
	/* 判断接收的报文是否正确，不正确返回-1 */
	if(pstWlanPrismT->frmlen.status != 0)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "parse_prism_header frm error!\n");
		return -2; 
	}
	
	pstStaticOutPut->ucRssi = pstWlanPrismT->rssi.data;
	pstStaticOutPut->ucChannel = pstWlanPrismT->channel.data;

	return sizeof(wlan_ng_prism2_header);
}

u_int prism_if_handle(const struct pcap_pkthdr *h, 
						const u_char *p, 
						void *pvOutPut)
{
	u_int   uiCapLen = h->caplen;
	u_int	uiLength = h->len;
	u_int   uiRet = 0;
	u_int32_t uiMsgCode;
	
	u_char  *pucPktTmp = NULL;

	PACKET_STATISTICS_T  stPacketStatistics = {0};
	
	PACKET_STATISTICS_T  *pstPktStatTmp = NULL;

	if(NULL == pvOutPut)
	{
		pstPktStatTmp = &stPacketStatistics;
	}
	else
	{
		pstPktStatTmp = (PACKET_STATISTICS_T  *)pvOutPut;
	}
	
	//printf(" %d %d\n", uiCapLen, uiLength);

	#if 0 /* 当前不考虑AVS报文(包含无线卡信息的报文) */
	uiMsgCode = EXTRACT_32BITS(p);
	if( uiMsgCode == WLANCAP_MAGIC_COOKIE_V1 || \
	    uiMsgCode == WLANCAP_MAGIC_COOKIE_V2)
	{
		/* AVS报文，使用报文的开始两个字节区分prism header和AVS header */
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "This is AVS packet!");
		return 0;
	}
	#endif
	
	if (uiCapLen < PRISM_HDR_LEN) 
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "[|802.11] caplen:%d < %d\n", \
												uiCapLen, PRISM_HDR_LEN);
		return uiCapLen;
	}
	pstPktStatTmp->uiTimeStamp= h->ts.tv_sec;

	/* 解析无线帧prism头，144个字节 */
	uiRet = parse_prism_header(p, uiLength, uiCapLen, 0, 0, pstPktStatTmp);
	if(uiRet < 0)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_PARSE, "parse_prism_header error, %d\n", uiRet);
		return 0;
	}
	/* 解析无线帧, 包含header以及payload */
	uiLength = uiLength - PRISM_HDR_LEN;
	uiCapLen = uiCapLen - PRISM_HDR_LEN;

	parse_wifi_frame(p + PRISM_HDR_LEN, uiLength, uiCapLen, 0, 0, pstPktStatTmp);

	if(DEV_TYPE_AP == pstPktStatTmp->ucType)
	{
		if(0 != pstPktStatTmp->ucChannel)
		{
			set_auto_scan_ch_mask(pstPktStatTmp->ucChannel - 1);
			//g_uiChanMask |= 1 << (pstPktStatTmp->ucChannel - 1);
		}
	}
	#if 0
	/* 将统计信息入队 */
	if(ENQUEUE_FLAG_ENABLE == pstPktStatTmp->ucEnqueueFlag)
	{
		printf("TimeStamp:%d", pstPktStatTmp->uiTimeStamp);
		printf(" Rssi:%d Channel:%d", pstPktStatTmp->ucRssi, pstPktStatTmp->ucChannel);
		print_mac(pstPktStatTmp->ucMac);
		if(DEV_TYPE_AP == pstPktStatTmp->ucType)
		{
			printf(" SSID:%s Auth:%d Encry:%d", \
								pstPktStatTmp->ucSsid, \
								pstPktStatTmp->uiAuth, \
								pstPktStatTmp->uiEncry);
		}
		else if(DEV_TYPE_STA == pstPktStatTmp->ucType)
		{
			if(DATA_STAT_SUCCESS == pstPktStatTmp->ucGot_qq)
			{
				printf(" QQ:%s", pstPktStatTmp->ucQq);
			}
			if(DATA_STAT_SUCCESS == pstPktStatTmp->ucGot_tb)
			{
				printf(" taobao:%s", pstPktStatTmp->ucTb);
			}
			if(DATA_STAT_SUCCESS == pstPktStatTmp->ucGot_wb)
			{
				printf(" weibo:%s", pstPktStatTmp->ucWb);
			}
		}
		printf("\n");
	}
	#endif
	return 0;
}

