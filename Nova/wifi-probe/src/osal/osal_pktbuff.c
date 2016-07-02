#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>     
#include <errno.h> 
#include <semaphore.h>
#include <limits.h>

#include "osal_api.h"
#include "linklist.h"
#include "pktbuff.h"
#include "util.h"


enum{
	PKT_BUFF_ID		=0,	/*FOR 报文队列*/
	INFO_BUFF_ID	=1, /*报文解析后的信息队列*/
	MAX_BUFF_QUEUE
};

#define OSAL_BUFF_MAGIC			0xdeaddeaf


#define PKT_BUFF_MAX_NUMS		2048

#define INFO_BUFF_SIZE			512
#define INFO_BUFF_MAX_NUMS		(2 * PKT_BUFF_MAX_NUMS)

typedef struct
{
	LIST_ENTRY node;
    char * buffer;	
    int callid;
	int magic;
	int line;
	char file[OSAL_MAX_API_NAME];
}BUFFER_INFO;

typedef struct buff_queue{
    LIST_ENTRY  Qfree;/*buffer 链表*/
	LIST_ENTRY Qlist;/*队列链表*/
	
	UINT16 count;/*总的buffer 数*/
	UINT16 size;/*每个buffer 大小*/
	
	UINT16 freecount;/*还有多少buffer可以分配*/
	UINT16 currentQ;/*队列中有多少*/
	
	pthread_mutex_t lock;	/*buffer mutex*/
	pthread_mutex_t qlock;	/*queue mutex*/
	
	void *pBuffInfo;
	void *pBuffHead;
}BUFF_QUEUE;

BUFF_QUEUE bQueue[MAX_BUFF_QUEUE];

static INT32 init_buffers(int qid, int size, int count)
{
    INT32               return_code = OSAL_SUCCESS;    
	int ret;
    pthread_mutexattr_t mutex_attr ; 
	int allsize, ii;
	BUFFER_INFO *pBuffInfo;
	
	if(qid >= MAX_BUFF_QUEUE)
		return OSAL_ERR_INVALID_ID;

	
    /*初始化mutex参数*/
    return_code = pthread_mutexattr_init(&mutex_attr); 
    if ( return_code != 0 )
    {
        printf("Error: pthread_mutexattr_init failed\n");
        return_code = OSAL_ERROR;
        return (return_code);
    }

    /*
    ** 允许优先级反转  
    */  
    return_code = pthread_mutexattr_setprotocol(&mutex_attr,PTHREAD_PRIO_INHERIT);
    if ( return_code != 0 )
    {
        printf("Error: pthread_mutexattr_setprotocol failed\n");
        return_code = OSAL_ERROR;
        return (return_code);
    }    
    
    /*
    **  设置锁的嵌套.同一线程可以多次锁
    */
    return_code = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    if ( return_code != 0 )
    {
        printf("Error: pthread_mutexattr_settype failed\n");
        return_code = OSAL_ERROR;
        return (return_code);
    }

	/*buffer mutex init*/
    ret = pthread_mutex_init((pthread_mutex_t *)&bQueue[qid].lock,&mutex_attr);
    if ( ret != 0 )
    {
       return_code = OSAL_ERROR;
       return(return_code);
    }

	/*queue mutex init*/
    ret = pthread_mutex_init((pthread_mutex_t *)&bQueue[qid].qlock,&mutex_attr);
    if ( ret != 0 )
    {
       return_code = OSAL_ERROR;
       return(return_code);
    }
	
	/*初始化链表头*/
	InitializeListHead(&bQueue[qid].Qfree);
	bQueue[qid].size = size;
	bQueue[qid].count = count;
	bQueue[qid].freecount = 0;

	/*分配buffer内存*/
	allsize = bQueue[qid].size * bQueue[qid].count;
	bQueue[qid].pBuffHead = aligned_malloc(allsize, 0x1000);
	if(NULL == bQueue[qid].pBuffHead)
	{
		return OSAL_ERR_NO_MEMORY;
	}

	/*分配buffer info 节点*/
	allsize = bQueue[qid].count * sizeof(BUFFER_INFO);
	
	bQueue[qid].pBuffInfo = osal_malloc(allsize);
	if(NULL == bQueue[qid].pBuffInfo)
	{
		if(bQueue[qid].pBuffHead)
			aligned_free(bQueue[qid].pBuffHead);
		return OSAL_ERR_NO_MEMORY;
	}
	
	/*初始化队列链表*/
	pBuffInfo = (BUFFER_INFO *)bQueue[qid].pBuffInfo;
	for(ii = 0; ii< bQueue[qid].count; ii++)
	{
        pBuffInfo->magic = OSAL_BUFF_MAGIC;
		pBuffInfo->buffer = (char *)((UINT32)bQueue[qid].pBuffHead + ii * bQueue[qid].size);
		pBuffInfo->line = 0;
		pBuffInfo->file[0]=0;
		pBuffInfo->callid = -1;
		lstInsertTail(&bQueue[qid].Qfree, &pBuffInfo->node);
		bQueue[qid].freecount++;
		pBuffInfo++;
	}


	/*init 队列链表*/
	
	InitializeListHead(&bQueue[qid].Qlist);
	bQueue[qid].currentQ =0;
	
	return OSAL_SUCCESS;
	
}

INT32 unInit_bufferApi(void)
{
	int ii;
	for(ii =0; ii < MAX_BUFF_QUEUE; ii++)
	{
		aligned_free(bQueue[ii].pBuffHead);
		bQueue[ii].pBuffHead = NULL;
		osal_free(bQueue[ii].pBuffInfo);
		bQueue[ii].pBuffInfo = NULL;
	}
}

INT32 init_bufferApi(void)
{
	int ii;
	
	INT32 ret_code = OSAL_SUCCESS;

	for(ii =0; ii < MAX_BUFF_QUEUE; ii++)
	{
		memset((void*)&bQueue[ii], 0, sizeof(BUFF_QUEUE));
	}

	
	/*init PKT BUFFER.*/
	ret_code = init_buffers(PKT_BUFF_ID, PKT_BUFF_SIZE, PKT_BUFF_MAX_NUMS);
	if(ret_code != OSAL_SUCCESS)
	{
		unInit_bufferApi();
		return ret_code;
	}
	
	/*init info buffer.*/
	ret_code = init_buffers(INFO_BUFF_ID, INFO_BUFF_SIZE, INFO_BUFF_MAX_NUMS);
	if(ret_code != OSAL_SUCCESS)
	{
		unInit_bufferApi();
		return ret_code;
	}

	return ret_code;
	
}


char *alloc_buffer(int qid, char*file, int line)
{
    int *pLine=NULL;
	char *pName=NULL;
	PLIST_ENTRY	pEntry;
	BUFFER_INFO *pPktBuffInfo;
	BUFF_QUEUE *pBuffQueue;
	

	pBuffQueue = &bQueue[qid];
	
	pthread_mutex_lock(&pBuffQueue->lock);	
	
	if(pBuffQueue->freecount == 0)
	{
		pthread_mutex_unlock(&pBuffQueue->lock);	
		return NULL;		
	}
	
	pBuffQueue->freecount--;
	
	pEntry = lstRemoveHead(&pBuffQueue->Qfree);
	
	pthread_mutex_unlock(&pBuffQueue->lock);	
	
	pPktBuffInfo = (BUFFER_INFO *)pEntry;
	pPktBuffInfo->callid = OSAL_FindCreator();

	pPktBuffInfo->magic = 0;
	pPktBuffInfo->line = 0;
	pPktBuffInfo->file[0] = '\0';

	/*记录分配的函数和行数*/
	if(file != NULL)
	{
	    pName = strrchr(file, '/');
	    if(NULL==pName)
	        pName = file;
		if(strlen(pName) > (OSAL_MAX_API_NAME-1))
		{
			bcopy(pName,pPktBuffInfo->file,OSAL_MAX_API_NAME-1);
			pPktBuffInfo->file[OSAL_MAX_API_NAME-1]='\0';
		}
		else
			strcpy(pPktBuffInfo->file, pName);
		
		pPktBuffInfo->line = line;
	}
	
	return pPktBuffInfo->buffer;
	
}


INT32 free_buffer(int qid, void *ptr, char*file, int line)
{
	UINT32 addrs;
	int index;
	char *pName;
	BUFFER_INFO *pPktBuffInfo;
	BUFF_QUEUE *pBuffQueue;
	

	pBuffQueue = &bQueue[qid];
	
	/*
		check free ADDRS.
	*/
	addrs = (UINT32)ptr;
	if((addrs < (UINT32)pBuffQueue->pBuffHead) || \
		(addrs > (pBuffQueue->count * pBuffQueue->size + (UINT32)pBuffQueue->pBuffHead)))
	{
		return OSAL_INVALID_POINTER;		
	}
	
	addrs-= (UINT32)pBuffQueue->pBuffHead;
	if((addrs % pBuffQueue->size) != 0)
	{		
		return OSAL_INVALID_POINTER;
	}


	index = addrs / pBuffQueue->size;

	/*GET PKT BUFFER INFO POINT.*/
	
	pthread_mutex_lock(&pBuffQueue->lock);	
	
	pPktBuffInfo = (BUFFER_INFO *)((UINT32)pBuffQueue->pBuffInfo + index * sizeof(BUFFER_INFO));
	if(pPktBuffInfo->magic == OSAL_BUFF_MAGIC)
	{	
		pthread_mutex_unlock(&pBuffQueue->lock);	
		return OSAL_ERR_BUFFER_REFREE;		
	}
	pPktBuffInfo->magic = OSAL_BUFF_MAGIC;
	pPktBuffInfo->line = line;
	
	pName = strrchr(file, '/');
	if(NULL==pName)
		pName = file;
	if(strlen(pName) > (OSAL_MAX_API_NAME-1))
	{
		bcopy(pName,pPktBuffInfo->file,OSAL_MAX_API_NAME-1);
		pPktBuffInfo->file[OSAL_MAX_API_NAME-1]='\0';
	}
	else
		strcpy(pPktBuffInfo->file, pName);
	
	lstInsertTail(&pBuffQueue->Qfree, &pPktBuffInfo->node);
	pBuffQueue->freecount++;
	pthread_mutex_unlock(&pBuffQueue->lock);	

	return OSAL_SUCCESS;
	
}


PPKT_HDR _alloc_pktbuff(char*file, int line)
{

	PPKT_HDR pHdr;

	pHdr = (PPKT_HDR)alloc_buffer(PKT_BUFF_ID, file, line);
	if(NULL == pHdr)
		return NULL;
	
	pHdr->pcapOffset = REF_PCAP_OFFSET;
	pHdr->magic= OSAL_BUFF_MAGIC;
	
	memset((void*)&pHdr->tuple, 0, sizeof(PKT_TUPLE));
	
	return pHdr;
}

INT32 _free_pktbuff(void *ptr, char*file, int line)
{
	return free_buffer(PKT_BUFF_ID, ptr, file, line);
}

PPROBE_NODE_INFO _alloc_probebuff(char*file, int line)
{

	PPROBE_NODE_INFO pHdr;

	pHdr = (PPROBE_NODE_INFO)alloc_buffer(INFO_BUFF_ID, file, line);
	if(NULL == pHdr)
		return NULL;
	
	memset((void*)pHdr, 0, sizeof(PROBE_NODE_INFO));
	
	return pHdr;
}

INT32 _free_probebuff(void *ptr, char*file, int line)
{
	return free_buffer(INFO_BUFF_ID, ptr, file, line);
}


/*packet buffer 入队列*/
INT32 enqueue_pktbuff(PPKT_HDR pHdr)
{
	pthread_mutex_lock(&bQueue[PKT_BUFF_ID].qlock);	
	InsertTailList(&bQueue[PKT_BUFF_ID].Qlist, &pHdr->node);
	bQueue[PKT_BUFF_ID].currentQ++;
	pthread_mutex_unlock(&bQueue[PKT_BUFF_ID].qlock);
	return OSAL_SUCCESS;
}

/*packet buffer 出队列*/
PPKT_HDR dequeue_pktbuff(void)
{
    PLIST_ENTRY pListEntry;

	pthread_mutex_lock(&bQueue[PKT_BUFF_ID].qlock);	

	if(bQueue[PKT_BUFF_ID].currentQ ==0)
	{
		pthread_mutex_unlock(&bQueue[PKT_BUFF_ID].qlock);
		return NULL;
	}

	pListEntry = RemoveHeadList(&bQueue[PKT_BUFF_ID].Qlist)
	bQueue[PKT_BUFF_ID].currentQ--;
	pthread_mutex_unlock(&bQueue[PKT_BUFF_ID].qlock);

	return (PPKT_HDR)pListEntry;

}

/*probe info 入队列*/
INT32 enqueue_probebuff(PPROBE_NODE_INFO pHdr)
{
	pthread_mutex_lock(&bQueue[INFO_BUFF_ID].qlock);	
	InsertTailList(&bQueue[INFO_BUFF_ID].Qlist, &pHdr->node);
	bQueue[INFO_BUFF_ID].currentQ++;
	pthread_mutex_unlock(&bQueue[INFO_BUFF_ID].qlock);
	
	return OSAL_SUCCESS;
}

/*probe info 入队列*/
PPROBE_NODE_INFO dequeue_probebuff(void)
{
    PLIST_ENTRY pListEntry;

	pthread_mutex_lock(&bQueue[INFO_BUFF_ID].qlock);	

	if(bQueue[INFO_BUFF_ID].currentQ ==0)
	{
		pthread_mutex_unlock(&bQueue[INFO_BUFF_ID].qlock);
		return NULL;
	}

	pListEntry = RemoveHeadList(&bQueue[INFO_BUFF_ID].Qlist)
	bQueue[INFO_BUFF_ID].currentQ--;
	pthread_mutex_unlock(&bQueue[INFO_BUFF_ID].qlock);

	return (PPROBE_NODE_INFO)pListEntry;
}

void bufferQueueShow(void)
{
	int ii;
	UINT32 free;
	UINT32 qcurr;
	printf("QID    bfree    Qused\n");
	for(ii=0;ii<MAX_BUFF_QUEUE; ii++)
	{
		pthread_mutex_lock(&bQueue[ii].lock);
		free = bQueue[ii].freecount;
		pthread_mutex_unlock(&bQueue[ii].lock);

		pthread_mutex_lock(&bQueue[ii].qlock);
		qcurr = bQueue[ii].currentQ;
		pthread_mutex_unlock(&bQueue[ii].qlock);

		printf("%02d    %d    %d\n", ii, free, qcurr);
	}
		
}
