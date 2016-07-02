#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>     
#include <errno.h> 
#include <semaphore.h>
#include <limits.h>
#include <fcntl.h>			 /* For O_* constants */
#include <sys/stat.h>		 /* For mode constants */
#include <mqueue.h>

#include "osal_api.h"
#include "util.h"


typedef struct
{
    int   free;
    mqd_t id;
    char  name [OSAL_MAX_API_NAME];
    int   creator;
}OSAL_queue_record_t;


pthread_mutex_t OSAL_queue_table_mut;
OSAL_queue_record_t   OSAL_queue_table[OSAL_MAX_QUEUES];

#define DEFAULT_QUEUE_DEPTH					20

static UINT32  OSAL_QueueDelayTime(UINT32 msecs, struct timespec * tm)
{
    clock_gettime(CLOCK_REALTIME,  tm); 

    /* add the delay to the current time */
    tm->tv_sec  += (time_t) (msecs / 1000) ;
    /* convert residue ( msecs )  to nanoseconds */
    tm->tv_nsec +=  (msecs % 1000) * 1000000L ;
    
    if(tm->tv_nsec  >= 1000000000L )
    {
        tm->tv_nsec -= 1000000000L ;
        tm->tv_sec ++ ;
    }
    
    return(OSAL_SUCCESS) ;    
}

INT32 OSAL_QueueCreate (UINT32 *queue_id, const char *queue_name, 
						UINT32 queue_depth,UINT32 data_size, UINT32 flags)
{
	int 					i;
	pid_t					process_id;
	mqd_t					queueDesc;
	struct mq_attr			queueAttr;	 
	UINT32					possible_qid;
	char					name[OSAL_MAX_API_NAME * 2];
	char					process_id_string[OSAL_MAX_API_NAME+1];
	
	
	if ( queue_id == NULL || queue_name == NULL)
	{
		return OSAL_INVALID_POINTER;
	}
	
	/* we don't want to allow names too long*/
	/* if truncated, two names might be the same */
	
	if (strlen(queue_name) >= OSAL_MAX_API_NAME)
	{
		return OSAL_ERR_NAME_TOO_LONG;
	}

	if(data_size < sizeof(MSG_HEAD))
		return OSAL_INVALID_SIZE;
	
	/* Check Parameters */
	
	pthread_mutex_lock(&OSAL_queue_table_mut);	
	
	for(possible_qid = 0; possible_qid < OSAL_MAX_QUEUES; possible_qid++)
	{
		if (OSAL_queue_table[possible_qid].free == TRUE)
			break;
	}
	
	if( possible_qid >= OSAL_MAX_QUEUES || OSAL_queue_table[possible_qid].free != TRUE)
	{
		pthread_mutex_unlock(&OSAL_queue_table_mut);
		return OSAL_ERR_NO_FREE_IDS;
	}
	
	/* Check to see if the name is already taken */

	for (i = 0; i < OSAL_MAX_QUEUES; i++)
	{
		if ((OSAL_queue_table[i].free == FALSE) &&
			strcmp ((char*) queue_name, OSAL_queue_table[i].name) == 0)
		{
			pthread_mutex_unlock(&OSAL_queue_table_mut);
			return OSAL_ERR_NAME_TAKEN;
		}
	} 
	
	/* Set the possible task Id to not free so that
	 * no other task can try to use it */
	OSAL_queue_table[possible_qid].free = FALSE;
	
	pthread_mutex_unlock(&OSAL_queue_table_mut);
	
	/* set queue attributes */
	if(queue_depth == 0)
		queueAttr.mq_maxmsg = DEFAULT_QUEUE_DEPTH;
	else
		queueAttr.mq_maxmsg = queue_depth;

	queueAttr.mq_msgsize = data_size;
   
	/*
	** Construct the queue name:
	** The name will consist of "/<process_id>.queue_name"
	*/
 
	/* pre-pend / to queue name */
	strcpy(name, "/");

	/*
	** Get the process ID 
	*/
	process_id = getpid();
	sprintf(process_id_string, "%d", process_id);
	strcat(name, process_id_string);
	strcat(name,"."); 
	
	/*
	** Add the name that was passed in
	*/
	strcat(name, queue_name);

	printf("name: %s\n", name);
	/* 
	** create message queue 
	*/
	queueDesc = mq_open(name, O_CREAT | O_RDWR, 0666, &queueAttr);
	
	if ( queueDesc == -1 )
	{
		pthread_mutex_lock(&OSAL_queue_table_mut);
		OSAL_queue_table[possible_qid].free = TRUE;
		pthread_mutex_unlock(&OSAL_queue_table_mut);
		
		printf("OSAL_QueueCreate Error. errno = %d\n",errno);
		if( errno ==EINVAL)
		{
			printf("Your queue depth may be too large for the\n");
			printf("OS to handle. Please check the msg_max\n");
			printf("parameter located in /proc/sys/fs/mqueue/msg_max\n");
			printf("on your Linux file system and raise it if you\n");
			printf(" need to or run as root\n");
		}
		return OSAL_ERROR;
	}

	/*
	** store queue_descriptor
	*/
	*queue_id = possible_qid;
	
	pthread_mutex_lock(&OSAL_queue_table_mut);	
	
	OSAL_queue_table[*queue_id].id = queueDesc;
	OSAL_queue_table[*queue_id].free = FALSE;
	strcpy( OSAL_queue_table[*queue_id].name, (char*) queue_name);
	OSAL_queue_table[*queue_id].creator = OSAL_FindCreator();
	
	pthread_mutex_unlock(&OSAL_queue_table_mut);
	
	return OSAL_SUCCESS;
	
}

INT32 OSAL_QueueDelete (UINT32 queue_id)
{
	pid_t	process_id;
	char	name[OSAL_MAX_API_NAME+1];
	char	process_id_string[OSAL_MAX_API_NAME+1];

	/* Check to see if the queue_id given is valid */
	
	if (queue_id >= OSAL_MAX_QUEUES || OSAL_queue_table[queue_id].free == TRUE)
	{
	   return OSAL_ERR_INVALID_ID;
	}
	
	/*
	** Construct the queue name:
	** The name will consist of "/<process_id>.queue_name"
	*/

	/* pre-pend / to queue name */
	strcpy(name, "/");

	/*
	** Get the process ID
	*/
	process_id = getpid();
	sprintf(process_id_string, "%d", process_id);
	strcat(name, process_id_string);
	strcat(name,".");
	
	strcat(name, OSAL_queue_table[queue_id].name);
	
	/* Try to delete and unlink the queue */
	if((mq_close(OSAL_queue_table[queue_id].id) == -1) || (mq_unlink(name) == -1))
	{
		return OSAL_ERROR;
	}
	

	pthread_mutex_lock(&OSAL_queue_table_mut);	
	
	OSAL_queue_table[queue_id].free = TRUE;
	strcpy(OSAL_queue_table[queue_id].name, "");
	OSAL_queue_table[queue_id].creator = UNINITIALIZED;
	OSAL_queue_table[queue_id].id = UNINITIALIZED;
	
	pthread_mutex_unlock(&OSAL_queue_table_mut);
	
	return OSAL_SUCCESS;
	
}

/*
#define NO_WAIT           0
#define WAIT_FOREVER     -1
*/

INT32 OSAL_msgSend(MSG_HEAD * msg, int timeout)
{
	struct mq_attr	queueAttr;
	UINT32 queue_id;
	int ret=0;
	
	if(NULL == msg)
		return OSAL_INVALID_POINTER;

	queue_id = msg->destQID;
	
	/*
	** Check Parameters 
	*/
	if(queue_id >= OSAL_MAX_QUEUES || OSAL_queue_table[queue_id].free == TRUE)
	{
	   return OSAL_ERR_INVALID_ID;
	}
	
	/* get queue attributes */
	if(mq_getattr(OSAL_queue_table[queue_id].id, &queueAttr))
	{
	   return (OSAL_ERROR);
	}
	
	/* check if queue is full */
	if(timeout == NO_WAIT)
	{
		/*check mq_nums*/
		if(queueAttr.mq_curmsgs >= queueAttr.mq_maxmsg) 
		{
		   return(OSAL_QUEUE_FULL);
		}
		
		/* send message */
		if(mq_send(OSAL_queue_table[queue_id].id, (char *)msg, sizeof(MSG_HEAD), msg->option & 0x1) == -1) 
		{
			return(OSAL_ERROR);
		}		
	}
	else if(timeout == WAIT_FOREVER)
	{
        do 
        {
            /*
	        ** A signal can interrupt the mq_send call, so the call has to be done with 
	        ** a loop
	        */
           ret = mq_send(OSAL_queue_table[queue_id].id, (char *)msg, sizeof(MSG_HEAD), msg->option & 0x1);
		   
        } while ( ret == -1 && errno == EINTR );	

		 /*send failure.*/
		if(ret == -1)
			return OSAL_ERROR;

	}
	else
	{
		/*timeout.*/
	}
	
	return OSAL_SUCCESS;

}

/*
#define NO_WAIT           0
#define WAIT_FOREVER     -1
timeout = 毫秒
*/

INT32 OSAL_msgReceive(UINT32 queue_id, char *buf, UINT32 buflen, int timeout)
{
	struct mq_attr	queueAttr;
	INT32 			sizeCopied = -1;
	int 			ret_val;
	struct timespec ts;
	
	/*
	** Check Parameters 
	*/
	if(queue_id >= OSAL_MAX_QUEUES || OSAL_queue_table[queue_id].free == TRUE)
	{
		return OSAL_ERR_INVALID_ID;
	}
	else if( (buf == NULL) || (buflen < sizeof(MSG_HEAD)) )
	{
		return OSAL_INVALID_POINTER;
	}
	
	/*
	** Read the message queue for data
	*/
	if (timeout == WAIT_FOREVER) 
	{	   
		/*
		** A signal can interrupt the mq_receive call, so the call has to be done with 
		** a loop
		*/
		do 
		{
		   sizeCopied = mq_receive(OSAL_queue_table[queue_id].id, buf, buflen, NULL);
		   if(sizeCopied == -1)
		   	printf("mq_receive interrupt.%d\n", errno);
		} while ( sizeCopied == -1 && errno == EINTR );

		if(sizeCopied != buflen )
		{
			return(OSAL_QUEUE_INVALID_SIZE);
		}
	}
	else if (timeout == NO_WAIT)
	{	   
		/* get queue attributes */
		if(mq_getattr(OSAL_queue_table[queue_id].id, &queueAttr))
		{
			return (OSAL_ERROR);
		}
		
		/* check how many messages in queue */
		if(queueAttr.mq_curmsgs) 
		{
			sizeCopied	= mq_receive(OSAL_queue_table[queue_id].id, buf, buflen, NULL);
		} 
		else 
		{
			sizeCopied = -1;
		}
		
		if (sizeCopied == -1)
		{
			return OSAL_QUEUE_EMPTY;
		}
		else if(sizeCopied != buflen )
		{
			return(OSAL_QUEUE_INVALID_SIZE);
		}
	}
	else /* timeout */ 
	{
		ret_val = OSAL_QueueDelayTime(timeout , &ts) ;

		/*
		** If the mq_timedreceive call is interrupted by a system call or signal,
		** call it again.
		*/
		do
		{
		   sizeCopied = mq_timedreceive(OSAL_queue_table[queue_id].id, buf, buflen, NULL, &ts);
		} while ( sizeCopied == -1 && errno == EINTR );
		
		if((sizeCopied == -1) && (errno == ETIMEDOUT))
		{
			return(OSAL_QUEUE_TIMEOUT);
		}
		else if( sizeCopied == buflen )
		{
			return buflen;
		}
		else
		{
			return OSAL_QUEUE_INVALID_SIZE;
		}
		
	} /* END timeout */
	
	return (INT32)buflen;
	
}


INT32 OSAL_QueueGetInfo (UINT32 queue_id, OSAL_queue_prop_t *queue_prop)
{
	/* Check to see that the id given is valid */
	
	if (queue_prop == NULL)
	{
		return OSAL_INVALID_POINTER;
	}
	
	if (queue_id >= OSAL_MAX_QUEUES || OSAL_queue_table[queue_id].free == TRUE)
	{
		return OSAL_ERR_INVALID_ID;
	}

	/* put the info into the stucture */
	pthread_mutex_lock(&OSAL_queue_table_mut);	

	queue_prop ->creator =   OSAL_queue_table[queue_id].creator;
	strcpy(queue_prop ->name, OSAL_queue_table[queue_id].name);

	pthread_mutex_unlock(&OSAL_queue_table_mut);

	return OSAL_SUCCESS;
	
}


INT32 OSAL_QueueGetIdByName (UINT32 *queue_id, const char *queue_name)
{
	UINT32 i;

	if(queue_id == NULL || queue_name == NULL)
	{
	   return OSAL_INVALID_POINTER;
	}
	
	if (strlen(queue_name) >= OSAL_MAX_API_NAME)
	{
	   return OSAL_ERR_NAME_TOO_LONG;
	}

	for (i = 0; i < OSAL_MAX_QUEUES; i++)
	{
		if (OSAL_queue_table[i].free != TRUE &&
		   (strcmp(OSAL_queue_table[i].name, (char*) queue_name) == 0 ))
		{
			*queue_id = i;
			return OSAL_SUCCESS;
		}
	}

	/*no found.*/
	return OSAL_ERR_NAME_NOT_FOUND;

}


INT32 OSAL_QueueApi_init(void)
{
    int i;
    int                 ret;
    pthread_mutexattr_t mutex_attr ;    
    INT32               return_code = OSAL_SUCCESS;    
    
   
	 /* Initialize Message Queue Table */
	 
	 for(i = 0; i < OSAL_MAX_QUEUES; i++)
	 {
		 OSAL_queue_table[i].free 	   = TRUE;
		 OSAL_queue_table[i].id		   = UNINITIALIZED;
		 OSAL_queue_table[i].creator	   = UNINITIALIZED;
		 strcpy(OSAL_queue_table[i].name,""); 
	 }


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

    /*
 	INIT OSAL_queue_table_mut
	*/
    ret = pthread_mutex_init((pthread_mutex_t *) &OSAL_queue_table_mut,&mutex_attr);
    if ( ret != 0 )
    {
       return_code = OSAL_ERROR;
       return(return_code);
    }

    return return_code;
    
}



INT32 OSAL_QueueApi_uninit(UINT32 qid)
{
	int qid0;
	if(qid < OSAL_MAX_QUEUES)
	{
		OSAL_QueueDelete(qid);
		
		return OSAL_SUCCESS;
	}

	
	for(qid0 = 0 ; qid0 < OSAL_MAX_QUEUES; qid0++)
		OSAL_QueueDelete(qid0);

	
	return OSAL_SUCCESS;
}

