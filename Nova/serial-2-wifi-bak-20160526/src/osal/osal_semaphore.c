#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>     
#include <errno.h> 
#include <semaphore.h>
#include <limits.h>

/*
** The __USE_UNIX98 is for advanced pthread features on linux
*/

#include "osal_api.h"

#include "osal_semaphore.h"
#include "util.h"

/*
** Global data for the API
*/

pthread_mutex_t OSAL_mut_sem_tbl_mut;
pthread_mutex_t OSAL_semaphore_tbl_mut;

OSAL_mut_sem_record_t OSAL_mut_sem_table       [OSAL_MAX_MUTEXES];
OSAL_bin_sem_record_t OSAL_bin_sem_table       [OSAL_MAX_BIN_SEMAPHORES];

static UINT32  OSAL_CalcDelayTime(UINT32 msecs, struct timespec * tm)
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

/****************************************************************************************
                                  MUTEX API
****************************************************************************************/

/*
* 互斥信号量创建。目前options没有用，未来扩展
*/
INT32 OSAL_MutSemCreate (UINT32 *sem_id, const char *sem_name, UINT32 options)
{
    int                 return_code;
    pthread_mutexattr_t mutex_attr ;    
    UINT32              possible_semid;
    UINT32              i;      

    /* Check Parameters */
    if (sem_id == NULL || sem_name == NULL)
    {
        return OSAL_INVALID_POINTER;
    }
    
    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */
    if (strlen(sem_name) >= OSAL_MAX_API_NAME)
    {
        return OSAL_ERR_NAME_TOO_LONG;
    }

    pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);  

    for (possible_semid = 0; possible_semid < OSAL_MAX_MUTEXES; possible_semid++)
    {
        if (OSAL_mut_sem_table[possible_semid].free == TRUE)    
            break;
    }
    
    if( (possible_semid == OSAL_MAX_MUTEXES) ||
        (OSAL_mut_sem_table[possible_semid].free != TRUE) )
    {
        pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);
        return OSAL_ERR_NO_FREE_IDS;
    }

    /* Check to see if the name is already taken */

    for (i = 0; i < OSAL_MAX_MUTEXES; i++)
    {
        if ((OSAL_mut_sem_table[i].free == FALSE) &&
                strcmp ((char*) sem_name, OSAL_mut_sem_table[i].name) == 0)
        {
            pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);
            return OSAL_ERR_NAME_TAKEN;
        }
    }

    /* 置位不允许其他任务使用同样的sem table */
    
    OSAL_mut_sem_table[possible_semid].free = FALSE;
    pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);

    /* 
    ** initialize the attribute with default values 
    */
    return_code = pthread_mutexattr_init(&mutex_attr); 
    if (return_code != 0)
    {
        /* 初始化失败，重新释放资源 */
        pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);
        OSAL_mut_sem_table[possible_semid].free = TRUE;
        pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);

       printf("Error: Mutex could not be created. pthread_mutexattr_init failed ID = %lu\n",possible_semid);
       return OSAL_SEM_FAILURE;
    }

    /*
    ** 允许信号优先级反转。priority inheritance  
    */  
    return_code = pthread_mutexattr_setprotocol(&mutex_attr,PTHREAD_PRIO_INHERIT) ;
    if ( return_code != 0 )
    {
        /* Since the call failed, set free back to true */
        pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);
        OSAL_mut_sem_table[possible_semid].free = TRUE;
        pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);

       printf("Error: Mutex could not be created. pthread_mutexattr_setprotocol failed ID = %lu\n",possible_semid);
       return OSAL_SEM_FAILURE;    
    }	
    
    /*
    **  允许同一个线程互斥嵌套，但必须lock和unlock对应。Set the mutex type to RECURSIVE 
    */
    return_code = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    if ( return_code != 0 )
    {
        /* Since the call failed, set free back to true */
        pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);
        OSAL_mut_sem_table[possible_semid].free = TRUE;
        pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);

       printf("Error: Mutex could not be created. pthread_mutexattr_settype failed ID = %lu\n",possible_semid);
       return OSAL_SEM_FAILURE;   
    }

    /* 
    ** create the mutex 
    ** 初始化并置unlock.
    */
    return_code = pthread_mutex_init((pthread_mutex_t *) &OSAL_mut_sem_table[possible_semid].id,&mutex_attr); 
    if ( return_code != 0 )
    {
        /* Since the call failed, set free back to true */
        pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);
        OSAL_mut_sem_table[possible_semid].free = TRUE;
        pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);

       printf("Error: Mutex could not be created. ID = %lu\n",possible_semid);
       return OSAL_SEM_FAILURE;
    }
    else
    {
       /*
       ** Mark mutex as initialized
       */
       *sem_id = possible_semid;    /*返回逻辑semid*/
    
       pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);  

       strcpy(OSAL_mut_sem_table[*sem_id].name, (char*)sem_name);
       OSAL_mut_sem_table[*sem_id].free = FALSE;
       OSAL_mut_sem_table[*sem_id].creator = OSAL_FindCreator();
       OSAL_mut_sem_table[*sem_id].lock_count = 0;
       pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);

       return OSAL_SUCCESS;
    }

}/*end of OSAL_MutSemCreate*/

/*
* 互斥信号量删除。
*/
INT32 OSAL_MutSemDelete (UINT32 sem_id)
{
    int status=-1;

    /* Check to see if this sem_id is valid   */
    if (sem_id >= OSAL_MAX_MUTEXES || OSAL_mut_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }

    status = pthread_mutex_destroy( &(OSAL_mut_sem_table[sem_id].id)); /* 0 = success */   
    
    if( status != 0)
    {
        /*EBUSY  the mutex is currently locked*/
        return OSAL_SEM_FAILURE;
    }
    
    /*释放资源 */
   
    pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);  

    OSAL_mut_sem_table[sem_id].free = TRUE;
    strcpy(OSAL_mut_sem_table[sem_id].name , "");
    OSAL_mut_sem_table[sem_id].creator = UNINITIALIZED;
    OSAL_mut_sem_table[sem_id].lock_count = UNINITIALIZED;
    
    pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);
    
    return OSAL_SUCCESS;

}

/*
* 释放信号量
*/
INT32 OSAL_MutSemGive (UINT32 sem_id)
{
    UINT32 ret_val ;

    /* Check Parameters */

    if(sem_id >= OSAL_MAX_MUTEXES || OSAL_mut_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }

    /*
    ** Unlock the mutex
    */
    if(pthread_mutex_unlock(&(OSAL_mut_sem_table[sem_id].id)))
    {
        /*
        *    EINVAL: the mutex has not been properly initialized
        *    EPERM:  the calling thread does not own the mutex
        */
        ret_val = OSAL_SEM_FAILURE ;
    }
    else
    {
        ret_val = OSAL_SUCCESS ;
    }
    
    return ret_val;
} /* end of OSAL_MutSemGive */

/*
* 获取信号量.
*/
INT32 OSAL_MutSemTake (UINT32 sem_id)
{
    int status;

    /* 
    ** Check Parameters
    */  
    if(sem_id >= OSAL_MAX_MUTEXES || OSAL_mut_sem_table[sem_id].free == TRUE)
    {
       return OSAL_ERR_INVALID_ID;
    }
 
    /*
    ** Lock the mutex
    **如果一个线程在等待一个mutex锁得时候收到了一个signal,那么在从signal handler返回的时候，
    **该线程继续等待该mutex锁，就像这个线程没有被中断一样
    */
    
    status = pthread_mutex_lock(&(OSAL_mut_sem_table[sem_id].id));
    if( status == EINVAL )
    {
      return OSAL_SEM_FAILURE ;
    }
    else if ( status == EDEADLK )
    {
       printf("Task would deadlock--nested mutex call!\n");
       return OSAL_SUCCESS ;
    }
    else
    {
      return OSAL_SUCCESS;
    }

}/*end of OSAL_MutSemTake*/

INT32 OSAL_MutSemGetInfo (UINT32 sem_id, OSAL_sem_info_t *mut_prop)  
{
    /* Check parameters */
    if (sem_id >= OSAL_MAX_BIN_SEMAPHORES || OSAL_mut_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }
    
    if (mut_prop == NULL)
    {
        return OSAL_INVALID_POINTER;
    }

    /* put the info into the stucture */
    pthread_mutex_lock(&OSAL_mut_sem_tbl_mut);  

    mut_prop ->creator =    OSAL_mut_sem_table[sem_id].creator;
    mut_prop ->lock_count = OSAL_mut_sem_table[sem_id].lock_count;
    strcpy(mut_prop->name, OSAL_mut_sem_table[sem_id].name);
    
    pthread_mutex_unlock(&OSAL_mut_sem_tbl_mut);

    return OSAL_SUCCESS;
    
} /* end OSAL_BinSemGetInfo */

INT32 OSAL_MutSemGetIdByName (UINT32 *sem_id, const char *sem_name)
{
    UINT32 i;

    if(sem_id == NULL || sem_name == NULL)
    {
        return OSAL_INVALID_POINTER;
    }

    /* 
    ** Check parameters 
    */ 
    if (strlen(sem_name) >= OSAL_MAX_API_NAME)
    {
        return OSAL_ERR_NAME_TOO_LONG;
    }

    for (i = 0; i < OSAL_MAX_MUTEXES; i++)
    {
        if ((OSAL_mut_sem_table[i].free != TRUE) &&
           (strcmp (OSAL_mut_sem_table[i].name, (char*) sem_name) == 0) )
        {
            *sem_id = i;
            return OSAL_SUCCESS;
        }
    }
    
    return OSAL_ERR_NAME_NOT_FOUND;

}/* end OSAL_MutSemGetIdByName */


void muxShow(UINT32 semID)
{
	int status;
	int failTaskGetcount = 0;
	int totalfail = -999;
	int i;
	OSAL_sem_info_t sem_prop;


	printf("\n%6s%6s%11s%8s%12s\n", "SEMID", "NAME", "CREATOR", "value", "lock_count");
	
	if(semID >= OSAL_MAX_BIN_SEMAPHORES)	/*show all*/
	{
		for(i=0; i< OSAL_MAX_BIN_SEMAPHORES; i++)
		{
			memset((void*)&sem_prop, 0, sizeof(OSAL_sem_info_t));
			if(OSAL_ERR_INVALID_ID == OSAL_MutSemGetInfo(i, &sem_prop))
				continue;
			
			printf("%6d%6s%11x%8d%12d\n", i, sem_prop.name, sem_prop.creator, \
					sem_prop.value, sem_prop.lock_count);	
		}
	}
	else
	{

		memset((void*)&sem_prop, 0, sizeof(OSAL_sem_info_t));
		status = OSAL_MutSemGetInfo(semID, &sem_prop);
		if(status != OSAL_SUCCESS)
			failTaskGetcount++;
		else
			printf("%6d%6s%11x%8d%12d\n", semID, sem_prop.name, sem_prop.creator, \
					sem_prop.value, sem_prop.lock_count);	

	}
	


	if (failTaskGetcount != 0)
		printf("SEM Get Failed Count: %d\n", failTaskGetcount);
	
	return totalfail;

}


/****************************************************************************************
                                  SEMAPHORE API
****************************************************************************************/

/*
* 同步信号量创建
* options : SEM_FULL, SEM_EMPTY
*/
INT32 OSAL_SemaphoreCreate (UINT32 *sem_id, const char *sem_name, UINT32 options)
{
    UINT32              possible_semid;
    UINT32              i;
    int                 Status;
    pthread_mutexattr_t mutex_attr;    

    /* 
    ** Check Parameters 
    */
    if (sem_id == NULL || sem_name == NULL)
    {
        return OSAL_INVALID_POINTER;
    }

    if((options != SEM_FULL) && (options != SEM_EMPTY))
    {
        return OSAL_INVALID_POINTER;
    }
    
    /* 
    ** Check for maximum name length 
    */
    if (strlen(sem_name) >= OSAL_MAX_API_NAME)
    {
        return OSAL_ERR_NAME_TOO_LONG;
    }

    /* Lock table */
    pthread_mutex_lock(&OSAL_semaphore_tbl_mut);  

    for (possible_semid = 0; possible_semid < OSAL_MAX_BIN_SEMAPHORES; possible_semid++)
    {
        if (OSAL_bin_sem_table[possible_semid].free == TRUE)    
            break;
    }

    if((possible_semid >= OSAL_MAX_BIN_SEMAPHORES) ||  
       (OSAL_bin_sem_table[possible_semid].free != TRUE))
    {
        pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);
        return OSAL_ERR_NO_FREE_IDS;
    }
    
    /* Check to see if the name is already taken */
    for (i = 0; i < OSAL_MAX_BIN_SEMAPHORES; i++)
    {
        if ((OSAL_bin_sem_table[i].free == FALSE) &&
                strcmp ((char*) sem_name, OSAL_bin_sem_table[i].name) == 0)
        {
            pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);
            return OSAL_ERR_NAME_TAKEN;
        }
    }  


    /*
        1. 初始化mutex
    */    
    Status = pthread_mutexattr_init(&mutex_attr);
    if ( Status == 0 )
    {
       /*
       ** 设置优先级反转.Use priority inheritance  
       */
       Status = pthread_mutexattr_setprotocol(&mutex_attr,PTHREAD_PRIO_INHERIT);
       if ( Status == 0 )
       {
          /*
          ** 为某个mutex 设置条件变量。
          */
          Status = pthread_mutex_init(&(OSAL_bin_sem_table[possible_semid].id), &mutex_attr);
          if( Status == 0 )
          {
             /*
             ** Initialize the condition variable
             */
             Status = pthread_cond_init(&(OSAL_bin_sem_table[possible_semid].cv), NULL);
             if ( Status == 0 )
             {
                /*
                ** fill out the proper OSAL table fields
                */
                *sem_id = possible_semid;

                strcpy(OSAL_bin_sem_table[*sem_id].name , (char*) sem_name);
                OSAL_bin_sem_table[*sem_id].creator = OSAL_FindCreator();
    
                OSAL_bin_sem_table[*sem_id].max_value = SEM_FULL;
                OSAL_bin_sem_table[*sem_id].current_value = options;
                OSAL_bin_sem_table[*sem_id].free = FALSE;
   
                /* Unlock table */ 
                pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);

                return OSAL_SUCCESS;
             } 
             else
             {
                pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);
                printf("Error: pthread_cond_init failed\n");
                return (OSAL_SEM_FAILURE);
             }  /*end of pthread_cond_init*/
          }
          else
          {
             pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);
             printf("Error: pthread_mutex_init failed\n");
             return (OSAL_SEM_FAILURE);
          } /*end of pthread_mutex_init*/
      }
      else
      {
          pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);
          printf("Error: pthread_mutexattr_setprotocol failed\n");
          return (OSAL_SEM_FAILURE);
      } /*end of pthread_mutexattr_setprotocol*/
   }
   else
   {
      pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);
      printf("Error: pthread_mutexattr_init failed\n");
      return (OSAL_SEM_FAILURE);
   } /*end of pthread_mutexattr_init*/
            
}/* end of OSAL_SemaphoreCreate */


/*
* 同步信号量的删除.
*/
INT32 OSAL_SemaphoreDelete (UINT32 sem_id)
{
    int status=-1;
    
    /* Check to see if this sem_id is valid */
    if (sem_id >= OSAL_MAX_BIN_SEMAPHORES || OSAL_bin_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }

    /* Lock table */
   
    /* Remove the Id from the table, and its name, so that it cannot be found again */
    status = pthread_mutex_destroy(&(OSAL_bin_sem_table[sem_id].id));
    if( status != 0)
    {
        /*EBUSY  the mutex is currently locked*/
        return OSAL_SEM_FAILURE;
    }
    
    status = pthread_cond_destroy(&(OSAL_bin_sem_table[sem_id].cv));
    if( status != 0)
    {
        
       /* EBUSY  some threads are currently waiting on cond.*/
        return OSAL_SEM_FAILURE;
    }
    
    pthread_mutex_lock(&OSAL_semaphore_tbl_mut);  
    
    OSAL_bin_sem_table[sem_id].free = TRUE;
    strcpy(OSAL_bin_sem_table[sem_id].name , "");
    OSAL_bin_sem_table[sem_id].creator = UNINITIALIZED;
    OSAL_bin_sem_table[sem_id].max_value = 0;
    OSAL_bin_sem_table[sem_id].current_value = 0;

    /* Unlock table */
    pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);
   
    return OSAL_SUCCESS;

}/* end of OSAL_SemaphoreDelete */

/*
* 同步信号触发
*/
INT32 OSAL_SemaphoreGive ( UINT32 sem_id )
{
    int    ret;
   
    /* Check Parameters */
    if(sem_id >= OSAL_MAX_BIN_SEMAPHORES || OSAL_bin_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }

    /* 锁住同步信号量.Lock the mutex ( not the table! ) */    
    ret = pthread_mutex_lock(&(OSAL_bin_sem_table[sem_id].id));
    if ( ret != 0 )
    { 
    	/*互斥失败*/
       return(OSAL_SEM_FAILURE);
    }
    
    /* 
    ** If the sem value is not full ( 1 ) then increment it.
    */
    if ( OSAL_bin_sem_table[sem_id].current_value  < OSAL_bin_sem_table[sem_id].max_value )
    {
         OSAL_bin_sem_table[sem_id].current_value ++;
         pthread_cond_signal(&(OSAL_bin_sem_table[sem_id].cv));
    }

    pthread_mutex_unlock(&(OSAL_bin_sem_table[sem_id].id));
    return (OSAL_SUCCESS);

}/* end of OSAL_BinSemGive */

/*
* 释放所有等待的同步信号。
*/
INT32 OSAL_SemaphoreFlush (UINT32 sem_id)
{
    UINT32 ret_val;
    INT32  ret = 0;

    /* Check Parameters */
    if(sem_id >= OSAL_MAX_BIN_SEMAPHORES || OSAL_bin_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }

    /* 锁住同步信号量. Lock the mutex ( not the table! ) */    
    ret = pthread_mutex_lock(&(OSAL_bin_sem_table[sem_id].id));
    if ( ret != 0 )
    {
       return(OSAL_SEM_FAILURE);
    }

    /* 
    ** 释放所有等待在此信号的线程 
    */
    ret = pthread_cond_broadcast(&(OSAL_bin_sem_table[sem_id].cv));
    if ( ret == 0 )
    {
       ret_val = OSAL_SUCCESS ;
       /*设置信号 SEM_FULL.*/
       OSAL_bin_sem_table[sem_id].current_value = OSAL_bin_sem_table[sem_id].max_value;
    }
    else
    {
       ret_val = OSAL_SEM_FAILURE;
    }
    ret = pthread_mutex_unlock(&(OSAL_bin_sem_table[sem_id].id));

    return(ret_val);

}/* end of OSAL_SemaphoreFlush */

/*
* 等待同步信号量的获取
* timeout: 
* #define NO_WAIT           0
* #define WAIT_FOREVER     -1
* timeout的单位 ticks ; 1ticks = 10ms.
*/

INT32 OSAL_SemaphoreTake ( UINT32 sem_id, int timeout)
{
    UINT32 ret_val;
    int    ret;
    struct timespec  ts;
    UINT32 delay_ms =0;
    
    /* Check parameters */ 
    if(sem_id >= OSAL_MAX_BIN_SEMAPHORES  || OSAL_bin_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }
        
    /* Lock the mutex */    
    ret = pthread_mutex_lock(&(OSAL_bin_sem_table[sem_id].id));
    if ( ret != 0 )
    {
       return(OSAL_SEM_FAILURE);
    }

    if(timeout == NO_WAIT)
    {
        if(OSAL_bin_sem_table[sem_id].current_value == SEM_FULL){
            ret_val = OSAL_SUCCESS;
            OSAL_bin_sem_table[sem_id].current_value --;
        }
        else
            ret_val = OSAL_SEM_FAILURE;    

    }
    else if(timeout == WAIT_FOREVER)
    {
    
        /*
        ** If the value is less than the max (1) 
        ** wait until it is available
        ** If the value is max (1), then grab the resource without waiting
        */
    
        if (OSAL_bin_sem_table[sem_id].current_value < OSAL_bin_sem_table[sem_id].max_value )
        {
           /*
           ** Wait on the condition variable. Calling this function unlocks the mutex and 
           ** re-aquires the mutex when the function returns. This allows the function that
           ** calls the pthread_cond_signal or pthread_cond_broadcast to aquire the mutex
           */
           /*
            * 当前信号没有资源，等待pthread_cond_signal
           */
           ret = pthread_cond_wait(&(OSAL_bin_sem_table[sem_id].cv),&(OSAL_bin_sem_table[sem_id].id));
           if ( ret == 0 )
           {
              ret_val = OSAL_SUCCESS;
              /*
              ** Decrement the counter
              */
              OSAL_bin_sem_table[sem_id].current_value --;
           }
           else
           {
               ret_val = OSAL_SEM_FAILURE;
           }
        
        }
        else /*信号资源已经就绪*/
        {
           OSAL_bin_sem_table[sem_id].current_value --;
           ret_val = OSAL_SUCCESS;
        }

    }
    else 
    {
        //delay_ms = TICKS_TO_MSEC(timeout);
        OSAL_CalcDelayTime(timeout, &ts);
        
        /*
        * 需要注意下，如果timedwait还没有timeout，这个时候系统的时间改变，
        * 是否会导致timeout提前，或延迟，或者就死等了。?
        */
        if ( OSAL_bin_sem_table[sem_id].current_value < OSAL_bin_sem_table[sem_id].max_value )
        {
        
          do{
             ret = pthread_cond_timedwait(&(OSAL_bin_sem_table[sem_id].cv), &(OSAL_bin_sem_table[sem_id].id), &ts);
			 
		   } while(ret == -1 && errno == EINTR);
		   
           if ( ret == 0 )
           {
              ret_val = OSAL_SUCCESS;
              /* Decrement the counter */
              OSAL_bin_sem_table[sem_id].current_value --;
           }
           else if ( ret == ETIMEDOUT )
           {
              /*pthread_cond_timedwait returns the error ETIMEDOUT*/
              ret_val = OSAL_SEM_TIMEOUT;
           }
           else
           {
               ret_val = OSAL_SEM_FAILURE;
           }

        }
        else
        {
           OSAL_bin_sem_table[sem_id].current_value --;
           ret_val = OSAL_SUCCESS;
        }        
    }


    /* Unlock the mutex */
    pthread_mutex_unlock(&(OSAL_bin_sem_table[sem_id].id));
    
    return (ret_val);

}/* end of OSAL_SemaphoreTake */


INT32 OSAL_SemaphoreGetInfo (UINT32 sem_id, OSAL_sem_info_t *bin_prop)  
{
    /* Check parameters */
    if (sem_id >= OSAL_MAX_BIN_SEMAPHORES || OSAL_bin_sem_table[sem_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }
    if (bin_prop == NULL)
    {
        return OSAL_INVALID_POINTER;
    }

    /* put the info into the stucture */
    pthread_mutex_lock(&OSAL_semaphore_tbl_mut);  

    bin_prop ->creator =    OSAL_bin_sem_table[sem_id].creator;
    bin_prop ->value = OSAL_bin_sem_table[sem_id].current_value ;
    strcpy(bin_prop->name, OSAL_bin_sem_table[sem_id].name);
    
    pthread_mutex_unlock(&OSAL_semaphore_tbl_mut);

    return OSAL_SUCCESS;
    
} /* end OSAL_BinSemGetInfo */

INT32 OSAL_SemaphoreGetIdByName (UINT32 *sem_id, const char *sem_name)
{
    UINT32 i;

    /* Check parameters */
    if (sem_id == NULL || sem_name == NULL)
    {
        return OSAL_INVALID_POINTER;
    }

    /* 
    ** a name too long wouldn't have been allowed in the first place
    ** so we definitely won't find a name too long
    */
    if (strlen(sem_name) >= OSAL_MAX_API_NAME)
    {
       return OSAL_ERR_NAME_TOO_LONG;
    }

    for (i = 0; i < OSAL_MAX_BIN_SEMAPHORES; i++)
    {
        if (OSAL_bin_sem_table[i].free != TRUE &&
                (strcmp (OSAL_bin_sem_table[i].name , (char*) sem_name) == 0))
        {
            *sem_id = i;
            return OSAL_SUCCESS;
        }
    }
    /* 
    ** The name was not found in the table,
    ** or it was, and the sem_id isn't valid anymore 
    */
    return OSAL_ERR_NAME_NOT_FOUND;
    
}/* end OSAL_SemaphoreGetIdByName */

INT32 OSAL_SemaphoreApi_init(void)
{
    int i;
    int                 ret;
    pthread_mutexattr_t mutex_attr ;    
    INT32               return_code = OSAL_SUCCESS;    
    
     /* Initialize MUTEX SEMAPHORE Table */
    
    for(i = 0; i < OSAL_MAX_MUTEXES; i++)
    { 
        OSAL_mut_sem_table[i].free        = TRUE;
        OSAL_mut_sem_table[i].creator     = UNINITIALIZED;
        OSAL_mut_sem_table[i].lock_count  = UNINITIALIZED;
        strcpy(OSAL_mut_sem_table[i].name,"");         
     }


     /* Initialize BIN SEMAPHORE Table */
    
    for(i = 0; i < OSAL_MAX_BIN_SEMAPHORES; i++)
    { 
        OSAL_bin_sem_table[i].free        = TRUE;
        OSAL_bin_sem_table[i].creator     = UNINITIALIZED;
        strcpy(OSAL_bin_sem_table[i].name,"");      
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

    /**/
    ret = pthread_mutex_init((pthread_mutex_t *) &OSAL_mut_sem_tbl_mut,&mutex_attr);
    if ( ret != 0 )
    {
       return_code = OSAL_ERROR;
       return(return_code);
    }


    ret = pthread_mutex_init((pthread_mutex_t *) &OSAL_semaphore_tbl_mut,&mutex_attr);
    if ( ret != 0 )
    {
       return_code = OSAL_ERROR;
       return(return_code);
    }

    return return_code;
    
}


/* *************************************************************************** */

void semShow(UINT32 semID)
{
	int status;
	int failTaskGetcount = 0;
	int totalfail = -999;
	int i;
	OSAL_sem_info_t sem_prop;


	printf("\n%6s%6s%11s%8s%12s\n", "SEMID", "NAME", "CREATOR", "value", "lock_count");
	
	if(semID >= OSAL_MAX_BIN_SEMAPHORES)	/*show all*/
	{
		for(i=0; i< OSAL_MAX_BIN_SEMAPHORES; i++)
		{
			memset((void*)&sem_prop, 0, sizeof(OSAL_sem_info_t));
			if(OSAL_ERR_INVALID_ID == OSAL_SemaphoreGetInfo(i, &sem_prop))
				continue;
			
			printf("%6d%6s%11x%8d%12d\n", i, sem_prop.name, sem_prop.creator, \
					sem_prop.value, sem_prop.lock_count);	
		}
	}
	else
	{

		memset((void*)&sem_prop, 0, sizeof(OSAL_sem_info_t));
		status = OSAL_SemaphoreGetInfo(semID, &sem_prop);
		if(status != OSAL_SUCCESS)
			failTaskGetcount++;
		else
			printf("%6d%6s%11x%8d%12d\n", semID, sem_prop.name, sem_prop.creator, \
					sem_prop.value, sem_prop.lock_count);	

	}
	


	if (failTaskGetcount != 0)
		printf("SEM Get Failed Count: %d\n", failTaskGetcount);
	
	return totalfail;

}


