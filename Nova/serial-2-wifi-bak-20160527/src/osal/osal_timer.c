#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <errno.h>        //错误号定义

#include "osal_api.h"
#include "osal_timer.h"

#include "ap_log.h"

OSAL_timer_record_t OSAL_timer_table[OSAL_MAX_TIMERS];
TIMERBLK OSAL_timerblk[OSAL_MAX_TIMERS];

static LIST_ENTRY gTimerblkList;

pthread_mutex_t    OSAL_timer_table_mut;
pthread_mutex_t    OSAL_timer_blk_mut;

static UINT32 listcnt=0;

static BOOL timer_before(struct timeval *tv1, struct timeval *tv2)
{
	if(tv1->tv_sec < tv2->tv_sec)
		return TRUE;
	else
	{
		if(tv1->tv_sec == tv2->tv_sec)
		{
			if(tv1->tv_usec < tv2->tv_usec)
				return TRUE;
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	return TRUE;
}

static void add_timer_list(PLIST_ENTRY newTimerNode)
{
    PLIST_ENTRY pNode;
	TIMERBLK *timerb = (TIMERBLK *)newTimerNode;
	TIMERBLK *tmpTimerblk;
	INT32 nums = 0;
	
	if(NULL == timerb)
		return ;
	
	listcnt++;
	/*按从小到大排列*/
    ListFor(pNode, &gTimerblkList)
	{
		tmpTimerblk = (TIMERBLK *)pNode;
		if(timer_before(&timerb->expires, &tmpTimerblk->expires))
		{
			InsertBeforeList(pNode, newTimerNode);
			return ;
		}
	}

	/*NO ENTRY.*/
	InsertTailList(&gTimerblkList, newTimerNode);
}

static void del_timer_list(PLIST_ENTRY oldTimerNode)
{
    PLIST_ENTRY pNode;
	TIMERBLK *timerb = (TIMERBLK *)oldTimerNode;
	TIMERBLK *tmpTimerblk;
	
	if(NULL == timerb)
		return ;

    ListFor(pNode, &gTimerblkList)
    {
		tmpTimerblk = (TIMERBLK *)pNode;
		if(tmpTimerblk == timerb)
		{
			lstRemoveEntry(pNode);
			listcnt--;
			return ;
		}
	}

}

INT32 OSAL_TimerCreate(INT32 *timer_id, const char *timer_name, void (*callback)(INT32), INT32 args)
{
   UINT32             possible_tid;
   INT32              i;

   int                status;

   if ((timer_id == NULL) || (timer_name == NULL) || (NULL == callback))
   {
        return OSAL_INVALID_POINTER;
   }

   if (strlen(timer_name) > OSAL_MAX_API_NAME)
   {
      return OSAL_ERR_NAME_TOO_LONG;
   }

   /* 
   ** Check Parameters 
   */
   pthread_mutex_lock(&OSAL_timer_table_mut); 
    
   for(possible_tid = 0; possible_tid < OSAL_MAX_TIMERS; possible_tid++)
   {
      if (OSAL_timer_table[possible_tid].free == TRUE)
         break;
   }


   if( possible_tid >= OSAL_MAX_TIMERS || OSAL_timer_table[possible_tid].free != TRUE)
   {
        pthread_mutex_unlock(&OSAL_timer_table_mut);
        return OSAL_ERR_NO_FREE_IDS;
   }

   /* 
   ** Check to see if the name is already taken 
   */
   for (i = 0; i < OSAL_MAX_TIMERS; i++)
   {
       if ((OSAL_timer_table[i].free == FALSE) &&
            strcmp ((char*) timer_name, OSAL_timer_table[i].name) == 0)
       {
            pthread_mutex_unlock(&OSAL_timer_table_mut);
            return OSAL_ERR_NAME_TAKEN;
       }
   }


   /* 
   ** Set the possible timer Id to not free so that
   ** no other task can try to use it 
   */
   OSAL_timer_table[possible_tid].free = FALSE;
   OSAL_timer_table[possible_tid].timerBlk = (void *)&OSAL_timerblk[possible_tid];
   pthread_mutex_unlock(&OSAL_timer_table_mut);
   
   OSAL_timer_table[possible_tid].creator = OSAL_FindCreator();
   OSAL_timer_table[possible_tid].expireMS = 0;

   /*timer domean 使用的参数*/
   OSAL_timerblk[possible_tid].timer_cb = callback;
   OSAL_timerblk[possible_tid].args = args;
   OSAL_timerblk[possible_tid].flags = 0;
   OSAL_timerblk[possible_tid].used = 0;
   OSAL_timerblk[possible_tid].timerID = possible_tid;

   /*
   ** Return timer ID 
   */
   *timer_id = possible_tid;

   return OSAL_SUCCESS;
}

INT32 OSAL_TimerStart(INT32 timer_id,UINT32 expiryMs, UINT32 flags)
{
	/* 
	** Check to see if the timer_id given is valid 
	*/
	if (timer_id >= OSAL_MAX_TIMERS || OSAL_timer_table[timer_id].free == TRUE)
	{
		return OSAL_ERR_INVALID_ID;
	}

	if(expiryMs < DEFAULT_MIN_TIMER_TICKS)
		return OSAL_INVALID_EXPIRE_ARGS;
	
	OSAL_timerblk[timer_id].flags = flags;
	/*初始化超时时间*/
	gettimeofday(&OSAL_timerblk[timer_id].expires, NULL);
	//printf("timer%d set time: %d:%d\n", timer_id, OSAL_timerblk[timer_id].expires.tv_sec, OSAL_timerblk[timer_id].expires.tv_usec);
	
	OSAL_timerblk[timer_id].expires.tv_sec += expiryMs/1000;
	OSAL_timerblk[timer_id].expires.tv_usec += (expiryMs %1000) * 1000;
	if(OSAL_timerblk[timer_id].expires.tv_usec > 1000000)
	{
		OSAL_timerblk[timer_id].expires.tv_sec ++;
		OSAL_timerblk[timer_id].expires.tv_usec = OSAL_timerblk[timer_id].expires.tv_usec - 1000000;
	}

	pthread_mutex_lock(&OSAL_timer_blk_mut); 
	add_timer_list(&OSAL_timerblk[timer_id].node);
	OSAL_timerblk[timer_id].used = 1;	/*开始定时*/
	pthread_mutex_unlock(&OSAL_timer_blk_mut); 

	return OSAL_SUCCESS;
}



INT32 OSAL_TimerStop(INT32 timer_id)
{
   
	/* 
	** Check to see if the timer_id given is valid 
	*/
	if (timer_id >= OSAL_MAX_TIMERS || OSAL_timer_table[timer_id].free == TRUE)
	{
		return OSAL_ERR_INVALID_ID;
	}

	pthread_mutex_lock(&OSAL_timer_blk_mut); 
	if(OSAL_timerblk[timer_id].used == 0)
	{
		pthread_mutex_unlock(&OSAL_timer_blk_mut); 
		return OSAL_TIMER_STOP_NOUSED;
	}
	
	del_timer_list(&OSAL_timerblk[timer_id].node);
	OSAL_timerblk[timer_id].used = 0;	/*结束定时*/
	pthread_mutex_unlock(&OSAL_timer_blk_mut); 

	return OSAL_SUCCESS;
}

INT32 OSAL_TimerDelete(INT32 timer_id)
{

	if (timer_id >= OSAL_MAX_TIMERS || OSAL_timer_table[timer_id].free == TRUE)
	{
		return OSAL_ERR_INVALID_ID;
	}


	OSAL_timer_table[timer_id].free = TRUE;
	
	pthread_mutex_lock(&OSAL_timer_blk_mut); 	
	if(OSAL_timerblk[timer_id].used == TRUE){
		pthread_mutex_unlock(&OSAL_timer_blk_mut); 
		return OSAL_TIMER_DEL_NOSTOP;
	}
	
	pthread_mutex_unlock(&OSAL_timer_blk_mut); 
	
	return OSAL_SUCCESS;
}

/**domean pthread for timer process.*/
static void timer_wait_ticks(INT32 ms)
{
	int fd = 0;
	struct timeval timeout;
	int rc;
	
	do{
		timeout.tv_sec = ms/1000;	
		timeout.tv_usec = (ms % 1000)*1000;
		rc = select(fd, NULL,NULL, NULL, &timeout);
	}while((rc == -1) & (errno == EINTR));

}
void * timer_pthread(void *args)
{

    PLIST_ENTRY pNode;
	TIMERBLK *tmpTimerblk;
	struct	timeval base_ticks;

	while(1)
	{
		timer_wait_ticks(DEFAULT_MIN_TIMER_TICKS);
		gettimeofday(&base_ticks, NULL);

		/***/
		pthread_mutex_lock(&OSAL_timer_blk_mut); 
	    ListFor(pNode, &gTimerblkList)
		{
			tmpTimerblk = (TIMERBLK *)pNode;
			if(timer_before(&tmpTimerblk->expires, &base_ticks))
			{
				if(tmpTimerblk->used == 1)
				{
					pthread_mutex_unlock(&OSAL_timer_blk_mut); 
					tmpTimerblk->timer_cb(tmpTimerblk->timerID);
					pthread_mutex_lock(&OSAL_timer_blk_mut); 
					
					lstRemoveEntry(pNode);
					listcnt--;
					tmpTimerblk->used = 0;
					if(tmpTimerblk->flags & TIMER_REPEATER)
					{
					}
				}
			}
			else
				break;
		}	
		pthread_mutex_unlock(&OSAL_timer_blk_mut); 
		
	}

}

INT32  OSAL_TimerAPIInit (void)
{
	int    i;   
	int    status;
	INT32  return_code = OSAL_SUCCESS;

	/*
	** Mark all timers as available
	*/
	for ( i = 0; i < OSAL_MAX_TIMERS; i++ )
	{
		OSAL_timer_table[i].free      = TRUE;
		OSAL_timer_table[i].creator   = UNINITIALIZED;
		strcpy(OSAL_timer_table[i].name,"");

		OSAL_timerblk[i].used = UNINITIALIZED;
		OSAL_timerblk[i].flags = UNINITIALIZED;

	}

	/*INIT timer blk list.*/
	InitializeListHead(&gTimerblkList);

	status = pthread_mutex_init((pthread_mutex_t *) & OSAL_timer_table_mut,NULL); 
	if ( status < 0 )
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_OSAL, "OSAL_TimerAPIInit: init OSAL_timer_table_mut err\n");
		return_code = OSAL_ERROR;
	}

	status = pthread_mutex_init((pthread_mutex_t *) & OSAL_timer_blk_mut,NULL); 
	if ( status < 0 )
	{
		AP_LOG(AP_LOG_ERR, LOG_MOUDLES_OSAL, "OSAL_TimerAPIInit: init OSAL_timer_blk_mut err\n");
		return_code = OSAL_ERROR;
	}
	
	return(return_code);

}

void timer1_cb(INT32 args)
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	printf("timer%d timerout: %d: %d\n", args, tv.tv_sec, tv.tv_usec);
}


void timer1_test(void)
{
	INT32 retcode;
	INT32 timeID1 =99;
	UINT32 expireMs;
	retcode = OSAL_TimerCreate(&timeID1, "timer1", timer1_cb, 100);
	if(retcode != OSAL_SUCCESS)
	{
		printf("OSAL_TimerCreate err. retcode=%d\n", retcode);
		return ;
	}

	expireMs = DEFAULT_MIN_TIMER_TICKS * ((rand() % 20) + 1);

	retcode = OSAL_TimerStart(timeID1, expireMs, TIMER_ONETIME);
	if(retcode != OSAL_SUCCESS)
	{
		printf("OSAL_TimerStart err. retcode=%d\n", retcode);
		return ;
	}

	printf("timeID1 = %d, settime = %d(ms)\n", timeID1, expireMs);
	
}

void timerShow(void)
{
    PLIST_ENTRY pNode;
	TIMERBLK *tmpTimerblk;

	pthread_mutex_lock(&OSAL_timer_blk_mut); 
	printf("listcnt = %d\n", listcnt);
	ListFor(pNode, &gTimerblkList)
	{
		pthread_mutex_unlock(&OSAL_timer_blk_mut); 
		tmpTimerblk = (TIMERBLK *)pNode;
		printf("timerid = %d, %d:%d,used:%d\n", tmpTimerblk->timerID, 	\
					tmpTimerblk->expires.tv_sec, tmpTimerblk->expires.tv_usec, tmpTimerblk->used);
		pthread_mutex_lock(&OSAL_timer_blk_mut); 

	}	
	
	pthread_mutex_unlock(&OSAL_timer_blk_mut); 

}

void timerDelete(void)
{
	INT32 retcode;
	INT32 timer_id;
	
	for(timer_id = 0; timer_id< OSAL_MAX_TIMERS; timer_id++)
	{
		retcode = OSAL_TimerDelete(timer_id);
		if(retcode != OSAL_SUCCESS)
		{
			printf("OSAL_TimerDelete err. retcode=%d\n", retcode);
		}		
	}

}


void timerStop(void)
{
	INT32 retcode;
	INT32 timeID1 =99;
	UINT32 expireMs;
	retcode = OSAL_TimerCreate(&timeID1, "timer1", timer1_cb, 100);
	if(retcode != OSAL_SUCCESS)
	{
		printf("OSAL_TimerCreate err. retcode=%d\n", retcode);
		return ;
	}

	expireMs = 10000;

	retcode = OSAL_TimerStart(timeID1, expireMs, TIMER_ONETIME);
	if(retcode != OSAL_SUCCESS)
	{
		printf("OSAL_TimerStart err. retcode=%d\n", retcode);
		return ;
	}

	printf("timeID1 = %d, settime = %d(ms)\n", timeID1, expireMs);

	OSAL_TaskDelay(3000);
	
	timerShow();
	
	printf("begin stop timerid = %d\n", timeID1);

	retcode = OSAL_TimerStop(timeID1);
	if(retcode != OSAL_SUCCESS)
	{
		printf("OSAL_TimerStop err. retcode=%d\n", retcode);
		return ;
	}	

	
	timerShow();
}


