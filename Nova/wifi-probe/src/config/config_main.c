#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ap_log.h"

#include "osal_api.h"
#include "pktbuff.h"

#include "config_intf.h"


static QUEUE_ID qConfigRecvID;
static QUEUE_ID qConfig2UartTxID;

CONFIG_DB_T configDB;

static void calc_time_abs(struct	timeval *end, struct	timeval *start, struct	timeval *out)
{
	out->tv_sec = end->tv_sec - start->tv_sec;
	if(end->tv_usec < start->tv_usec)
	{
		out->tv_sec--;
		out->tv_usec = 1000000 - (start->tv_usec - end->tv_usec);
	}
	else
		out->tv_usec = end->tv_usec - start->tv_usec;
	
}

void change_state(CONFIG_STATE state)
{
	configDB.state = state;
}

BOOL config_state_is_running(void)
{
	if(configDB.state == CONFIG_RUN)
		return TRUE;
	else
		return FALSE;
}

char *printf_state(CONFIG_STATE state)
{
	switch(state)
	{
		case CONFIG_IDLE:
			return "IDLE";
		case CONFIG_RUN:
			return "RUN";
		case CONFIG_IMAGE:
			return "IMAGE";
		default:
			return "UNKONW";
	}
}

static INT32 CONFIG_sendMsg_to_uart(int msgType, char *Buffer, int msgLen)
{
	MSG_HEAD msgHdr;
	/*sendMSG to UART TX pthread.*/
	msgHdr.destQID = qConfig2UartTxID;
	msgHdr.flags = MSG_FLAGS_MALLOC;
	msgHdr.option = MSG_NORMAL;
	msgHdr.msgLen = msgLen;
	msgHdr.msgType = MSG_CONFIG_TO_UART;
	msgHdr.msgPara1 = 0;
	msgHdr.msgPara2.pAddedMsg = Buffer;

	return OSAL_msgSend(&msgHdr,NO_WAIT);
	
}

static INT32 CONFIG_handle_online_response(MSG_FUNCS *funcs, char *inBuf, int inLen)
{
	INT32 retcode;
	
	struct MSG_BODY *msgBody = inBuf;
	struct msg_online_response resp;

	if((NULL == funcs) || (NULL == inBuf))
	{
		return MSG_INVALID_POINTER;
	}
	
	retcode = funcs->parse_msg_online_response(&resp, inBuf, inLen);
	if(MSG_SUCCESS != retcode)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "parse_msg_online_response err=%d\n", retcode);
		return MSG_ERROR;
	}

	/*MUT LOCK.*/
	OSAL_MutSemTake(configDB.dblock);
	
	configDB.scan_ch_mask = resp.set_scan_ch_mask;
	configDB.ch_switch_time = resp.set_ch_swith_time;
	configDB.report_time_per_mac = resp.set_report_time_per_mac;
	if(configDB.scan_ch_mask  == CONFIG_AUTO_CH_MASK)
		configDB.auto_scan_ch_mask = 0;	/*重新开始*/

	/*MUT UNLOCK.*/
	OSAL_MutSemGive(configDB.dblock);

	/*save to config file.*/
	
	return MSG_SUCCESS;
	
}
static INT32 CONFIG_handle_msg_cont(MSG_FUNCS *funcs, char *inBuf, int inLen, int *timeout)
{
	INT32 retcode = MSG_SUCCESS;
	
	struct MSG_BODY *msgBody = inBuf;

	if((NULL == funcs) || (NULL == inBuf))
	{
		return MSG_INVALID_POINTER;
	}

	switch(msgBody->msgID)
	{
		case UART_MSG_ONLINE_RESPONSE:
			retcode = CONFIG_handle_online_response(funcs, inBuf, inLen);
			if(retcode == MSG_SUCCESS)
			{
				*timeout = WAIT_FOREVER;
				change_state(CONFIG_RUN);
				printf("config state => %x\n", CONFIG_RUN);
				//AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "config state %x=> %s\n", CONFIG_RUN, printf_state(CONFIG_RUN));
			}
			break;
		case UART_MSG_CONFIG_REQUEST:
			break;
		default:
			break;
	}

	return retcode;
	
}

static INT32 CONFIG_handle_msg(CONFIG_DB_T *DB, MSG_HEAD *msgHdr, int *timeout)
{
	INT32 retcode;
	char *pBuf=NULL;
	int recvLen = 0;
	
	if((NULL == msgHdr) || (NULL == timeout) || (NULL == DB))
		return -1;

	if(NULL == DB->msg_funcs)
		return -1;
	
	if(NULL == DB->msg_funcs->valid_messages)
		return -1;

	recvLen = msgHdr->msgLen;
	pBuf = msgHdr->msgPara2.pAddedMsg;

	/*verify msg.*/
	retcode = DB->msg_funcs->valid_messages(pBuf, recvLen);
	if(retcode != MSG_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "valid_messages failure. errcode =%d\n", retcode);
		if(msgHdr->flags & MSG_FLAGS_MALLOC)
			osal_free(pBuf);
		
		return -1;
	}
	
	switch(msgHdr->msgType)
	{
		case MSG_SLIP_TO_CONFIG:
			CONFIG_handle_msg_cont(DB->msg_funcs, pBuf, recvLen, timeout);
			if(msgHdr->flags & MSG_FLAGS_MALLOC)
				osal_free(pBuf);
			break;
		default:
			break;
	}


	
	return 0;
}

static INT32 CONFIG_send_online_req(CONFIG_DB_T *DB)
{
	struct msg_online_request online_req;
	char *reqBuffer;
	int sendlen = 0;
	INT32 retcode;

	if(NULL == DB)
		return -1;
	
	if(NULL == DB->msg_funcs)
		return -1;
	
	if(NULL == DB->msg_funcs->build_msg_online_request)
		return -1;	
	
	reqBuffer = (char *)osal_malloc(MAX_MSG_MTU);
	if(NULL == reqBuffer)
		return -1;
	
	/*fill content.*/
	bcopy(DB->hwversion, online_req.hwversion,sizeof(online_req.hwversion));
	bcopy(DB->swversion, online_req.swversion,sizeof(online_req.swversion));
	online_req.cur_scan_ch_mask = DB->scan_ch_mask;
	online_req.ch_switch_time = DB->ch_switch_time;

	
	/*build online message.*/
	retcode = DB->msg_funcs->build_msg_online_request(&online_req, reqBuffer, &sendlen);
	if(retcode != MSG_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "build_msg_online_request failure. errcode =%d\n", retcode);
		osal_free((void*)reqBuffer);
		return -1;
	}

	/*send msg 2 uart tx.*/
	retcode = CONFIG_sendMsg_to_uart(MSG_CONFIG_TO_UART, reqBuffer, sendlen);
	if(OSAL_SUCCESS != retcode)
		osal_free((void*)reqBuffer);
	
	return 0;
}

void *CONFIG_pthread(void *args)
{
	MSG_HEAD msgHdr;
	int timeout;
	
	INT32 retcode = OSAL_SUCCESS;
	
	
	OSAL_TaskRegister();

	timeout = MSG_ONLINE_RETRASMIT_TIME *1000;
	//CONFIG_send_online_req(&configDB);
	timeout = WAIT_FOREVER;
	while(1)
	{
		retcode = OSAL_msgReceive(qConfigRecvID,(char *)&msgHdr,sizeof(MSG_HEAD),timeout);
		if(OSAL_QUEUE_TIMEOUT == retcode)
		{
			/*retransmit online request msg.*/
			CONFIG_send_online_req(&configDB);
		}
		else
		{
			if(retcode < 0 )
				continue;
			
			/*recv msg from host.*/
			CONFIG_handle_msg(&configDB, &msgHdr, &timeout);			
		}


	}

}

INT32 CONFIG_register_msgfuncs(MSG_FUNCS * funcs)
{
	configDB.msg_funcs = funcs;
}

INT32 CONFIG_init(char *configfile)
{
	INT32 retcode;
	UINT32 tid;

	memset((void *)&configDB, 0, sizeof(configDB));
	configDB.dblock = 99;	/*0 为有效值*/

	/*parse config file */
	read_cfg_file(configfile);
	
	/*INIT some local database.for config request.*/
	get_hwversion(configDB.hwversion, sizeof(configDB.hwversion));
	get_swversion(configDB.swversion, sizeof(configDB.swversion));
	configDB.scan_ch_mask = gWifiProbe.config.chn_mask;
	configDB.ch_switch_time = gWifiProbe.config.chn_scan_time;
	configDB.report_time_per_mac = gWifiProbe.config.report_time;
	configDB.channel_caps_mask = get_channel_caps();
	configDB.settingchange = 0;

	retcode = OSAL_MutSemGetIdByName(&configDB.dblock, MUTEX_CONFIG_NAME);
	if(retcode != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "OSAL_MutSemGetIdByName return =%d\n", retcode);
		return STATUS_ERROR;
	}
	
	retcode = OSAL_QueueGetIdByName(&qConfigRecvID, QUEUE_CONFIG_NAME);
	if(retcode != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "OSAL_QueueGetIdByName return =%d\n", retcode);
		return STATUS_ERROR;
	}

	retcode = OSAL_QueueGetIdByName(&qConfig2UartTxID, QUEUE_UART_TX_NAME);
	if(retcode != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "OSAL_QueueGetIdByName return =%d\n", retcode);
		return STATUS_ERROR;
	}

	change_state(CONFIG_IDLE);
	
	return STATUS_SUCCESS;
}

