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
	UINT16 proto;		/* TCP, 6; UDP, 17 */
	UINT16 direction;	/*STA->AP, AP->STA 0:上行；1:下行*/ 
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


#define OSAL_MAX_COUNTER_NAME	64

typedef struct _PROBE_INFO{
	LIST_ENTRY node;
	UINT32 timestamp;
	UINT8 type;	/*AP or STA*/
	UINT8 mac[6];
	UINT8 rssi;
	UINT8 channel;
	UINT8 got_qq;
	UINT8 qq[OSAL_MAX_COUNTER_NAME];
	UINT8 got_tb;
	UINT8 tb[OSAL_MAX_COUNTER_NAME];
	UINT8 got_wb;
	UINT8 wb[OSAL_MAX_COUNTER_NAME];
	/*如果是AP，下列信息必须有*/
	UINT8 ssid[OSAL_MAX_COUNTER_NAME];
	UINT8 encry[OSAL_MAX_COUNTER_NAME];
	UINT8 auth[OSAL_MAX_COUNTER_NAME];
	
}PROBE_NODE_INFO, *PPROBE_NODE_INFO;


#define pktBuffAlloc()			_alloc_pktbuff(__FILE__,__LINE__)
#define pktBuffFree(ptr) 		_free_pktbuff(ptr,__FILE__,__LINE__)

#define probeBuffAlloc()		_alloc_probebuff(__FILE__,__LINE__)
#define probeBuffFree(ptr) 		_free_probebuff(ptr,__FILE__,__LINE__)

/*EXTERN FUNCTION*/
extern INT32 init_bufferApi(void);

extern INT32 enqueue_pktbuff(PPKT_HDR pHdr);
extern PPKT_HDR dequeue_pktbuff(void);
extern INT32 enqueue_probebuff(PPROBE_NODE_INFO pHdr);
extern PPROBE_NODE_INFO dequeue_probebuff(void);

#endif

