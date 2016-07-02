#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "osal_api.h"

#include "ap_log.h"

#include "slip.h"

#include "wifiprobe.h"
#include "uartcom.h"

SLIP_T gSlip;
QUEUE_ID qSlip2ConfigID = -1;

static BOOL test_and_clear_bit(UINT32 bit, UINT32 *flags)
{
	if((bit > 31) || (NULL == flags))
		return FALSE;
	
	if(*flags & (1 << bit))
	{
		*flags &= ~(1 << bit);
		return TRUE;
	}
	else
		return FALSE;
}

static BOOL test_bit(UINT32 bit, UINT32 *flags)
{
	if((bit > 31) || (NULL == flags))
		return FALSE;
	
	if(*flags & (1 << bit))
		return TRUE;
	else
		return FALSE;
}

static void clear_bit(UINT32 bit, UINT32 *flags)
{
	if((bit > 31) || (NULL == flags))
		return ;
	
	*flags &= ~(1 << bit);
	
	return ;
}

static void set_bit(UINT32 bit, UINT32 *flags)
{
	if((bit > 31) || (NULL == flags))
		return ;
	
	*flags |= (1 << bit);
	
	return ;
}
static void SLIP_dump(SLIP_T *slip, int txrx)
{
	int ii;
	int count;
	char *buf;

	if(gWifiProbe.verbose < 1)
		return ;
	
	if(txrx == 0)	/*rx*/
	{
		count = slip->rcount;
		buf = slip->recvbuff;
	}
	else
	{
		count = slip->tcount;
		buf = slip->xmitbuff;
	}

	
	printf("SLIP %s %d ==>\n", txrx?"TX":"RX", count);
	for(ii =0; ii < count; ii++)
	{
		if((ii != 0) && ((ii % 16) == 0))
			printf("\n");
		printf("%02x ", (UINT8)buf[ii]);
	}
	
	printf("\n");
		
}

static INT32 SLIP_reset(SLIP_T *slip)
{
	slip->rcount = 0;
	clear_bit(SLIP_ESCAPE, &slip->flags);
	return 0;
}

static INT32 SLIP_rx(SLIP_T *slip)
{
	char *pBuff;
	MSG_HEAD msgHdr;
	INT32 retcode;
	
	if(NULL == slip)
		return -1;

	STATISTIC_PROBE_INC(uart_rx_pkts);
	STATISTIC_PROBE_INC_VAL(uart_rx_bytes,slip->rcount);
	SLIP_dump(slip, 0);

	/*alloc memory.*/
	pBuff = (char *)osal_malloc(slip->mtu);
	if(NULL == pBuff)
		return -1;

	memcpy(pBuff, slip->recvbuff, COMMON_MIN(slip->mtu, slip->rcount));

	/*sendMSG to config pthread.*/
	msgHdr.destQID = qSlip2ConfigID;
	msgHdr.flags = MSG_FLAGS_MALLOC;
	msgHdr.option = MSG_NORMAL;
	msgHdr.msgLen = COMMON_MIN(slip->mtu, slip->rcount);
	msgHdr.msgType = MSG_SLIP_TO_CONFIG;
	msgHdr.msgPara1 = 0;
	msgHdr.msgPara2.pAddedMsg = pBuff;

	retcode = OSAL_msgSend(&msgHdr,NO_WAIT);

	if(OSAL_SUCCESS != retcode)
	{
		osal_free(pBuff);
		return -1;
	}

	return 0;
	
}

static INT32 SLIP_decap(SLIP_T *slip, UINT8 ch)
{
	
	switch (ch) 
	{
		
		case SLIP_END:

			if (!test_and_clear_bit(SLIP_ERROR, &slip->flags) && (slip->rcount > MIN_SLIP_MTU))
			{
				/*send msg to config queue.*/
				SLIP_rx(slip);
			}
			
			SLIP_reset(slip);
			
			return 0;

		case SLIP_ESC:
			set_bit(SLIP_ESCAPE, &slip->flags);
			return 0;
			
		case SLIP_ESC_ESC:
			if (test_and_clear_bit(SLIP_ESCAPE, &slip->flags))
				ch = SLIP_ESC;
			break;
			
		case SLIP_ESC_END:
			if (test_and_clear_bit(SLIP_ESCAPE, &slip->flags))
				ch = SLIP_END;
			break;
		
	}
	
	if (!test_bit(SLIP_ERROR, &slip->flags))  
	{
		if (slip->rcount < slip->mtu)	
		{
			slip->recvbuff[slip->rcount++] = ch;
			return 0;
		}
		
		set_bit(SLIP_ERROR, &slip->flags);
	}

	return 0;
	
}

static INT32 SLIP_encap(SLIP_T *slip, char* inbuff, int len)
{
	UINT8 *ptr = (UINT8*)slip->xmitbuff;
	UINT8 ch;
	char *pSrc = inbuff;
	
	if((NULL == slip) || (inbuff == NULL))
		return -1;
	
	if(len > slip->mtu)
		return -1;

	*ptr++ = SLIP_END;

	/*
	 * For each byte in the packet, send the appropriate
	 * character sequence, according to the SLIP protocol.
	 */

	while (len-- > 0) {
		switch (ch = *pSrc++) 
		{
	        /* if it's the same code as an END character, we send a
	        * special two character code so as not to make the
	        * receiver think we sent an END
	        */
			case SLIP_END:
				*ptr++ = SLIP_ESC;
				*ptr++ = SLIP_ESC_END;
				break;
				
            /* if it's the same code as an ESC character,
            * we send a special two character code so as not
            * to make the receiver think we sent an ESC
            */				
			case SLIP_ESC:
				*ptr++ = SLIP_ESC;
				*ptr++ = SLIP_ESC_ESC;
				break;
			default:
				*ptr++ = ch;
				break;
		}
	}
	
	*ptr++ = SLIP_END;
	slip->tcount = (char *)ptr-slip->xmitbuff;

	SLIP_dump(slip, 1);
	
	return 0;
}

INT32 SLIP_init(void)
{
	INT32 retcode;
	
	memset(&gSlip, 0, sizeof(gSlip));

	retcode = OSAL_QueueGetIdByName(&qSlip2ConfigID,QUEUE_CONFIG_NAME);
	if(retcode != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"Slip init Get queueid =%s failure.\n", QUEUE_CONFIG_NAME);
		return -1;
	}

	gSlip.flags = 0;
	gSlip.rcount = 0;
	gSlip.tcount = 0;
	gSlip.mtu = MAX_SLIP_MTU;

	#if 0
	gSlip.recvbuff = (char *)osal_malloc(gSlip.mtu); 	/*alloc recv first buffer.*/
	if(NULL == gSlip.recvbuff)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"Slip init malloc recv failure.\n");
		return -1;
	}

	gSlip.xmitbuff = (char *)osal_malloc(gSlip.mtu);	/*alloc temp xmit buffer.*/
	if(NULL == gSlip.xmitbuff)
	{
		osal_free(gSlip.recvbuff);
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"Slip init malloc xmit failure.\n");
		return -1;
	}	
	#endif
	
	/*register callback.*/
	gSlip.SLIP_reset = SLIP_reset;
	gSlip.SLIP_decap = SLIP_decap;
	gSlip.SLIP_encap = SLIP_encap;
	
	UART_register_slip(&gSlip);

	return 0;
	
}
