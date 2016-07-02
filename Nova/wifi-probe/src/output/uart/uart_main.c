#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ap_log.h"

#include "osal_api.h"
#include "pktbuff.h"

#include "wifiprobe.h"


#include "uartcom.h"

typedef struct _uart_com{
	INT32 fd;
	QUEUE_ID qUartTx;
	
	SLIP_T * slip;
	
}UART_COM_T;

UART_COM_T gUartCom;

#define DEFAULT_UART_SPEED 		115200


void UART_delete_cb(void)
{
	printf("WARNING ============================\n");
}

void UART_sendMsg(int msgType, int options)
{
	INT32 retcode;
	MSG_HEAD msg;
	UINT32 qid;
	UINT32 tid;
	tid = OSAL_TaskGetId();

	memset((void *)&msg, 0, sizeof(MSG_HEAD));
	
	retcode = OSAL_QueueGetIdByName(&qid, QUEUE_CONFIG_NAME);
	if(retcode != OSAL_SUCCESS)
		return ;
	
	msg.destQID = qid;		
	msg.msgType = msgType;
	msg.option = options;
	msg.flags = MSG_FLAGS_NONE;
	msg.msgPara1 = 0x88889999;
	msg.msgPara2.pAddedMsg = (void *)0x88889999;
	msg.msgLen = 40;
	retcode =  OSAL_msgSend(&msg, NO_WAIT);
	if(OSAL_SUCCESS != retcode)
		printf("tid=%d sendmsg failure.ret = %d\n", tid, retcode);

}

static INT32 UART_tx_messages(SLIP_T *slip, MSG_HEAD *msgHdr)
{
	INT16 retcode;
	INT16 sendlen;
	
	slip->SLIP_encap(slip, msgHdr->msgPara2.pAddedMsg, msgHdr->msgLen);
	if((msgHdr->flags & MSG_FLAGS_MALLOC) == MSG_FLAGS_MALLOC)
		osal_free((void *)msgHdr->msgPara2.pAddedMsg);

	sendlen = UART_send(gUartCom.fd, slip->xmitbuff, slip->tcount);
	if(sendlen < 0)
		STATISTIC_PROBE_INC(uart_tx_err);
	else
	{
		STATISTIC_PROBE_INC(uart_tx_pkts);
		STATISTIC_PROBE_INC_VAL(uart_tx_bytes, slip->tcount);
	}	
	
	return 0;
}

static INT32 UART_handle_probe_info(SLIP_T *slip, PPROBE_NODE_INFO probeHdr)
{
	char tmpbuf[MAX_SLIP_MTU];
	int sendlen =0;
	INT16 retcode;
	
	/*BUILD probe info msg.*/
	retcode = build_msg_probe_info(probeHdr, tmpbuf, &sendlen);
	probeBuffFree(probeHdr);
	
	if(retcode == 0)
	{
		slip->SLIP_encap(slip, tmpbuf, sendlen);
		sendlen = UART_send(gUartCom.fd, slip->xmitbuff, slip->tcount);
		if(sendlen < 0)
			STATISTIC_PROBE_INC(uart_tx_err);
		else
		{
			STATISTIC_PROBE_INC(uart_tx_pkts);
			STATISTIC_PROBE_INC_VAL(uart_tx_bytes, slip->tcount);
		}
	}
	
	return 0;
}


INT32 UART_register_slip(SLIP_T * slip)
{
	gUartCom.slip = slip;
}


void *UART_rx_pthread(void* args)
{
	TASK_ID tid;
	INT32 retcode;
	SEM_ID semID;
	UINT8 ch =0;
	SLIP_T *slip = gUartCom.slip;
	INT32 ttyfd = gUartCom.fd;
	
    OSAL_TaskRegister();
	tid = OSAL_TaskGetId();
	
	OSAL_TaskDeleteHook(UART_delete_cb);


	/*FLUSH INPUT BUFFER.*/
	UART_Flush(ttyfd);

	while(1)
	{
		retcode = UART_RecvChar(ttyfd, (UINT8*)&ch, RECV_ONE_CHAR_TIMEOUT);
		if(retcode <= 0)
		{
			/*RESET BUFFER.*/
			slip->SLIP_reset(slip);
			continue;
		}
		slip->SLIP_decap(slip, (UINT8)ch);
	}
	
}


void *UART_tx_pthread(void* args)
{
	PPROBE_NODE_INFO probeHdr;
	INT32 retcode;
	QUEUE_ID qid;
	MSG_HEAD msgHdr;
	SLIP_T *slip = gUartCom.slip;
	
    OSAL_TaskRegister();

	qid = gUartCom.qUartTx;
	
	while(1){
		retcode = OSAL_msgReceive(qid,(char*)&msgHdr,sizeof(MSG_HEAD),NO_WAIT);
		if(retcode >= 0)
		{
			/*handle UART TX MSG.*/
			UART_tx_messages(slip, &msgHdr);
			memset(&msgHdr, 0, sizeof(MSG_HEAD));
		}

		probeHdr = dequeue_probebuff();
		if(probeHdr == NULL)
		{
			OSAL_TaskDelay(20);
			continue;
		}

		/* handle PROBE INFO */
		STATISTIC_PROBE_INC(probe_dequeues);
		UART_handle_probe_info(slip, probeHdr);
		
	}
	
}

INT32 UART_init(void)
{
	INT32 retcode;
	UINT32 tid;
	UART_OPTIONS sUartOptions;

	memset(&gUartCom,0, sizeof(gUartCom));

	/*Open TTY Device.*/
	gUartCom.fd = UART_Open(gWifiProbe.config.tty_name);
	if(gUartCom.fd < 0)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"UART open failure.\n");
		return STATUS_ERROR;
	}

	/*set UART Options*/
	sUartOptions.databits = DATABITS_8;
	sUartOptions.flowctrl = NO_FLOWCTRL;
	sUartOptions.parity = PARITY_EVEN;
	sUartOptions.stopbits = STOPBITS_1;
	sUartOptions.speed = DEFAULT_UART_SPEED;
	retcode = UART_SetOptions(gUartCom.fd, &sUartOptions);
	if(retcode < 0)
	{
		UART_Close(gUartCom.fd);
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"UART set options failure.\n");
		return STATUS_ERROR;	
	}

	SLIP_init();
	
	UART_msg_init();
	
	/*CREATE UART TX QUEUE.*/
	retcode = OSAL_QueueGetIdByName(&gUartCom.qUartTx,QUEUE_UART_TX_NAME);
	if(retcode != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"OSAL_QueueGetIdByName return =%d\n", retcode);
		UART_Close(gUartCom.fd);
		return STATUS_ERROR;
	}

	return STATUS_SUCCESS;
}

INT32 UART_uninit(void)
{
	UART_Close(gUartCom.fd);
}

