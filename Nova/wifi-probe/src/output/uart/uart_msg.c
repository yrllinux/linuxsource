#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ap_log.h"
#include "osal_api.h"
#include "uart_msg.h"
#include "pktbuff.h"
#include "uartcom.h"
#include "crc16.h"

#define add_crc16(pCrc, crc16)	\
do{	\
	pCrc[0] = (UINT8)((crc16 & 0xFF00) >> 8);\
	pCrc[1] = (UINT8)(crc16 & 0x00FF);	\
}while(0)

#define get_crc16(pCrc, crc16)	\
do{	\
	crc16  = pCrc[0] << 8;	\
	crc16 |= pCrc[1];	\
}while(0)


static PELEMENT_OPT AppendOption(PELEMENT_OPT pOpt, unsigned char Type, unsigned char Length, void* Value)
{
    pOpt->type = Type;
    pOpt->Length = Length;
    memcpy(pOpt->Value, Value, Length);
    return (PELEMENT_OPT)ELEMENT_OPT_NEXT(pOpt);
}

INT32 valid_messages(char *inbuff, int len)
{
	struct MSG_BODY *msgBody;
	UINT16 current_crc, calc_crc;
	UINT8 *pCrc;
	UINT16 frmLen = 0;

	if(NULL == inbuff)
		return MSG_INVALID_POINTER;

	if(len > MAX_MSG_MTU)
		return MSG_INVALID_LEN;

	msgBody = (struct MSG_BODY *)inbuff;
	
	/*CHECK crc16.*/
	pCrc = inbuff + len - MAX_MSG_CRC16;
	get_crc16(pCrc, current_crc);
	calc_crc = crc16(0, (UINT8 *)msgBody, len - MAX_MSG_CRC16);
	if(current_crc != calc_crc)
		return MSG_INVALID_CRC;

	return MSG_SUCCESS;
	
}

/*

*/
INT32 build_msg_probe_info(PPROBE_NODE_INFO probeHdr, char *outbuff, int *outlen)
{
	UINT16 frmLen = 0;
	UINT16 msgLen = 0;
	INT16 rssi;
	UINT16 crc;
	UINT8 *pCrc;
	
	struct MSG_BODY *msgBody = (struct MSG_BODY *)outbuff;
	struct msg_probe_report * probeReport = NULL;
	PELEMENT_OPT element_option,tmp_option;
	
	if((NULL == msgBody) || (NULL == probeHdr))
		return MSG_INVALID_POINTER;

	msgBody->msgID = UART_MSG_PROBE_REPORT;
	frmLen++;	/*MSGID*/

	probeReport = (struct msg_probe_report *)msgBody->body;
	probeReport->timestamp = htonl(probeHdr->timestamp);
	
	probeReport->devtype = probeHdr->type;
	
	rssi = probeHdr->rssi;
	if(rssi != 0)
		rssi-=DEFAULT_WIFI_NOISE;
	probeReport->rssi = htons(rssi);
	
	memcpy((void *)&probeReport->mac[0], (void *)&probeHdr->mac[0], 6);
	
	probeReport->chn = probeHdr->channel;
	
	element_option = (PELEMENT_OPT)((UINT32)probeReport + PROBE_REPORT_MSG_COMM_LEN);
	if(probeReport->devtype == DEV_STA)
	{
		if(probeHdr->got_qq)
			element_option = AppendOption(element_option, QQ_ELEMENT_ID, strlen(probeHdr->qq), probeHdr->qq);
		
		if(probeHdr->got_tb)
			element_option = AppendOption(element_option, TB_ELEMENT_ID, strlen(probeHdr->tb), probeHdr->tb);
		
		if(probeHdr->got_wb)
			element_option = AppendOption(element_option, WB_ELEMENT_ID, strlen(probeHdr->wb), probeHdr->wb);
		
	}else if(probeReport->devtype == DEV_AP)
	{
		element_option = AppendOption(element_option, AP_SSID_ELEMENT_ID, strlen(probeHdr->ssid), probeHdr->ssid);
		element_option = AppendOption(element_option, AP_AUTH_ELEMENT_ID, strlen(probeHdr->auth), probeHdr->auth);
		element_option = AppendOption(element_option, AP_ENCRY_ELEMENT_ID, strlen(probeHdr->encry), probeHdr->encry);
	}

	msgLen = (UINT32)element_option - (UINT32)probeReport;
	if(msgLen > MAX_BODY_LEN)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"build_msg_probe_info body len > %d =%d\n", msgLen, MAX_BODY_LEN);
		return MSG_INVALID_LEN;
	}

	probeReport->msgLen = htons(msgLen);
	frmLen+=msgLen;	/*MSG BODY.*/
	frmLen+=MAX_MSG_CRC16; /*MSG crc16.*/
	msgBody->frmLen = htons(frmLen);

	/*calc CRC16*/
	crc = crc16(0, (UINT8 *)msgBody, frmLen);
	pCrc = (char *)element_option;
	add_crc16(pCrc, crc);

	/*total: + frmLen */
	*outlen = frmLen + sizeof(msgBody->frmLen);

	return MSG_SUCCESS;
}

/******************************ONLINE MSG***************************************/
INT32 build_msg_online_request(struct msg_online_request *in_online_req, char *outbuff, int *outlen)
{
	struct MSG_BODY *msgBody;
	struct msg_online_request *req;
	UINT16 crc;
	UINT8 *pCrc;
	
	UINT16 frmLen = 0;
	
	if((NULL == in_online_req) || (NULL == outbuff))
		return MSG_INVALID_POINTER;

	msgBody = (struct MSG_BODY *)outbuff;
	
	msgBody->msgID = UART_MSG_ONLINE_REQUEST;
	frmLen = 1;
	req = (struct msg_online_request *)msgBody->body;
	pCrc = (char *)req + sizeof(struct msg_online_request);
	
	bcopy(in_online_req->hwversion, req->hwversion,sizeof(req->hwversion));
	bcopy(in_online_req->swversion, req->swversion,sizeof(req->swversion));
	in_online_req->cur_scan_ch_mask = 0xC0DBC0DB;
	req->cur_scan_ch_mask = htonl(in_online_req->cur_scan_ch_mask);
	req->ch_switch_time = htons(in_online_req->ch_switch_time);

	//printf("hw:%s\n", req->hwversion);
	
	//printf("sw:%s\n", req->swversion);
	
	frmLen +=sizeof(struct msg_online_request);
	frmLen+=MAX_MSG_CRC16; /*+MSG crc16.*/
	msgBody->frmLen = htons(frmLen);
	
	/*calc CRC16*/
	crc = crc16(0, (UINT8 *)msgBody, frmLen);
	add_crc16(pCrc, crc);

	/*total: + frmLen */
	*outlen = frmLen + sizeof(msgBody->frmLen);	
	
	return MSG_SUCCESS;
}


INT32 parse_msg_online_response(struct msg_online_response *out_online_resp, char *inbuff, int len)
{
	struct MSG_BODY *msgBody;
	struct msg_online_response *resp;
	
	UINT16 current_crc, calc_crc;
	UINT8 *pCrc;
	UINT16 frmLen = 0;
	
	if((NULL == out_online_resp) || (NULL == inbuff))
		return MSG_INVALID_POINTER;

	if(len > MAX_MSG_MTU)
		return MSG_INVALID_LEN;
	
	msgBody = (struct MSG_BODY*)inbuff;
	
	/*check frm len.*/
	frmLen = ntohs(msgBody->frmLen);
	if(frmLen != (sizeof(struct msg_online_response) + MAX_MSG_CRC16 + 1))
		return MSG_INVALID_LEN;

	/*check msgID.*/
	if(msgBody->msgID != UART_MSG_ONLINE_RESPONSE)
		return MSG_INVALID_MSGID;
	
	/*everything is ok.*/
	resp = (struct msg_online_response *)msgBody->body;
	out_online_resp->set_scan_ch_mask = ntohl(resp->set_scan_ch_mask);
	out_online_resp->set_ch_swith_time = ntohl(resp->set_ch_swith_time);
	out_online_resp->set_report_time_per_mac = ntohl(resp->set_report_time_per_mac);
	
	return MSG_SUCCESS;
}

MSG_FUNCS gUartMsgFuncs;

INT32 UART_msg_init(void)
{
	gUartMsgFuncs.valid_messages = valid_messages;
	gUartMsgFuncs.build_msg_online_request = build_msg_online_request;
	gUartMsgFuncs.parse_msg_online_response = parse_msg_online_response;

	CONFIG_register_msgfuncs(&gUartMsgFuncs);
	
	return MSG_SUCCESS;
}
