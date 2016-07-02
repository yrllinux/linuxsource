#ifndef __PKTBUFF_H_
#define __PKTBUFF_H_

#include "common_types.h"
#include "linklist.h"

#define PKT_BUFF_SIZE			2048

/*64-bytes aligned.*/

#define REF_PCAP_OFFSET		64

typedef struct _PKT_TUPLE{
	UINT32 srcIp;
	UINT32 dstIp;
	UINT16 srcPort;
	UINT16 dstPort;
	UINT16 proto;
	UINT16 direction;	/*STA->AP, AP->STA*/
	UINT32 sequence;
}PKT_TUPLE;

typedef struct _PKT_HDR{
	LIST_ENTRY node;
	UINT16 pcapOffset;
	UINT16 pcapLength;
	
	UINT16 wlanOffset;
	UINT16 wlanLen;
	
	UINT16 ipOffset;
	UINT16 ipLen;

	
	PKT_TUPLE tuple;
	UINT32 magic;
	
	UINT32 resv[5];
	
}PKT_HDR, *PPKT_HDR;


#define MSG_DATA_SIZE	1460

typedef struct _SERIAL2WIFI_MSG_DATA{
	LIST_ENTRY node;
	UINT8 type;	/*0£ºclient 1£ºserver 2£ºall*/
	UINT32 datalen;
	UINT8 data[MSG_DATA_SIZE];	
}SERIAL2WIFI_MSG_DATA, *PSERIAL2WIFI_MSG_DATA;


#define pktBuffAlloc()			_alloc_pktbuff(__FILE__,__LINE__)
#define pktBuffFree(ptr) 		_free_pktbuff(ptr,__FILE__,__LINE__)

#define serial2wifimsgBuffAlloc()		_alloc_serial2wifimsgbuff(__FILE__,__LINE__)
#define serial2wifimsgBuffFree(ptr)		_free_serial2wifimsgbuff(ptr,__FILE__,__LINE__)

/*EXTERN FUNCTION*/
extern INT32 init_bufferApi(void);

extern INT32 enqueue_pktbuff(PPKT_HDR pHdr);
extern PPKT_HDR dequeue_pktbuff(void);
extern INT32 enqueue_serial2wifi_msgbuff(PSERIAL2WIFI_MSG_DATA pMsgData);
extern PSERIAL2WIFI_MSG_DATA dequeue_serial2wifi_msgbuff(void);

#endif

