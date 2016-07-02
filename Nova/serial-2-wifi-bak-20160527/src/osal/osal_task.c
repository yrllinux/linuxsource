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
#include "osal_task.h"
#include "util.h"

OSAL_task_record_t    OSAL_task_tbl[OSAL_MAX_TASKS];
pthread_mutex_t       OSAL_task_tbl_mutex;
pthread_key_t         thread_key;

#define STACK_FILL_PATTEN   0xEE
#define PAGE_SIZE_ALIGNED   0x1000

static void OSAL_stackpointFree(INT32 taskid)
{
    if((OSAL_task_tbl[taskid].dym == 1) && \
            (OSAL_task_tbl[taskid].stack_point != NULL))
    {
        aligned_free(OSAL_task_tbl[taskid].stack_point);
    }
}

static INT32 OSAL_PriorityRemap(UINT32 InputPri)
{
    return (INT32)InputPri;
}

/*
* 获取当前任务ID.
*/
UINT32 OSAL_FindCreator(void)
{
    pthread_t    pthread_id;
    UINT32 i;  
   
    pthread_id = pthread_self();
    /* 
    ** Get PTHREAD Id
    */
    for (i = 0; i < OSAL_MAX_TASKS; i++)
    {
        if (pthread_equal(pthread_id, OSAL_task_tbl[i].id) != 0 )
        {
            break;
        }
    }

    return i;
}

/*获取任务ID号。根据 OS threadiD 对应task号*/
UINT32 OSAL_TaskGetId (void)
{ 
   void*   task_id;
   int     task_id_int;
   UINT32   task_key;
   task_key = 0;
   
   task_id = (void *)pthread_getspecific(thread_key);

   memcpy(&task_id_int,&task_id, sizeof(UINT32));
   
   task_key = task_id_int & 0xFFFF;
   
   return(task_key);
}

INT32 OSAL_TaskCreate (UINT32 *task_id, const char *task_name, void* (*function_pointer)(void*),
                      const char *stack_pointer, UINT32 stack_size, UINT32 priority,
                      UINT32 flags)
{
	int 			   return_code = 0;
	pthread_attr_t	   custom_attr ;
	struct sched_param priority_holder ;
	int 			   possible_taskid;
	int 			   i;
	UINT32			   local_stack_size;
	int 			   ret;  
	int 			   os_priority;
	
	/* Check for NULL pointers */	 
	if( (task_name == NULL) || (function_pointer == NULL) || (task_id == NULL) )
	{
		return OSAL_INVALID_POINTER;
	}

    if((stack_size < OSAL_TASK_STACK_MIN) && (NULL != stack_pointer))
    {
        return OSAL_ERR_STACKSIZE_TOO_LOW;
    }
    
	if (strlen(task_name) >= OSAL_MAX_API_NAME)
	{
		return OSAL_ERR_NAME_TOO_LONG;
	}

	/* Check for bad priority */
	if (priority > OSAL_TASK_MAX_PRIORITY)
	{
		return OSAL_ERR_INVALID_PRIORITY;
	}

	/* Change OSAL priority into a priority that will work for this OS */
	os_priority = OSAL_PriorityRemap(priority);
	
	/* Check Parameters */
	pthread_mutex_lock(&OSAL_task_tbl_mutex); 

	for(possible_taskid = 0; possible_taskid < OSAL_MAX_TASKS; possible_taskid++)
	{
		if (OSAL_task_tbl[possible_taskid].free == TRUE)
		{
			break;
		}
	}

	/* Check to see if the id is out of bounds */
	if( possible_taskid >= OSAL_MAX_TASKS || OSAL_task_tbl[possible_taskid].free != TRUE)
	{
		pthread_mutex_unlock(&OSAL_task_tbl_mutex);
		return OSAL_ERR_NO_FREE_IDS;
	}

	/* Check to see if the name is already taken */ 
	for (i = 0; i < OSAL_MAX_TASKS; i++)
	{
		if ((OSAL_task_tbl[i].free == FALSE) &&
		   ( strcmp((char*)task_name, OSAL_task_tbl[i].name) == 0)) 
		{		
			pthread_mutex_unlock(&OSAL_task_tbl_mutex);
			return OSAL_ERR_NAME_TAKEN;
		}
	}
	
	/* 
	** Set the possible task Id to not free so that
	** no other task can try to use it 
	*/
	OSAL_task_tbl[possible_taskid].free = FALSE;
	
	pthread_mutex_unlock(&OSAL_task_tbl_mutex);

    if ( stack_size < OSAL_TASK_STACK_MIN )
    {
       local_stack_size = OSAL_TASK_STACK_MIN;
    }
    else
    {
       local_stack_size = stack_size;
    }

    if(NULL == stack_pointer)
    {
        //OSAL_task_tbl[possible_taskid].stack_point = (void *)malloc(local_stack_size);
        OSAL_task_tbl[possible_taskid].stack_point = aligned_malloc(local_stack_size, PAGE_SIZE_ALIGNED);
        if(NULL == OSAL_task_tbl[possible_taskid].stack_point)
        {
			pthread_mutex_unlock(&OSAL_task_tbl_mutex);
			return OSAL_ERR_NO_MEMORY;
        }
        
        OSAL_task_tbl[possible_taskid].dym = 1;
    }
    else
    {
        OSAL_task_tbl[possible_taskid].stack_point = (void *)stack_pointer;
        OSAL_task_tbl[possible_taskid].dym = 0;
    }
    
    memset(OSAL_task_tbl[possible_taskid].stack_point, STACK_FILL_PATTEN, local_stack_size);
    
   /*
   ** Initialize the pthread_attr structure. 
   ** The structure is used to set the stack and priority
   */
	memset(&custom_attr, 0, sizeof(custom_attr));
	if(pthread_attr_init(&custom_attr))
	{  
	
        OSAL_stackpointFree(possible_taskid);
        
		pthread_mutex_lock(&OSAL_task_tbl_mutex); 
		OSAL_task_tbl[possible_taskid].free = TRUE;
		pthread_mutex_unlock(&OSAL_task_tbl_mutex); 
		printf("pthread_attr_init error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
		return(OSAL_ERROR); 
	}

	/*
	** Set the Stack Size
	*/
	if (pthread_attr_setstack(&custom_attr, OSAL_task_tbl[possible_taskid].stack_point, (size_t)local_stack_size ))
	{
        OSAL_stackpointFree(possible_taskid);
        
		printf("pthread_attr_setstack error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
		return(OSAL_ERROR); 
	}

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) 

	/*
	** Set the scheduling policy 
	** On Linux, the schedpolity must be SCHED_FIFO or SCHED_RR to set the priorty
	*/
	if (pthread_attr_setschedpolicy(&custom_attr, SCHED_FIFO))
	{
        OSAL_stackpointFree(possible_taskid);
        
		printf("pthread_attr_setschedpolity error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
		return(OSAL_ERROR);
	}

	/* 
	** Set priority 
	*/
	memset(&priority_holder, 0, sizeof(priority_holder));
	priority_holder.sched_priority = os_priority;
	ret = pthread_attr_setschedparam(&custom_attr,&priority_holder);
	if(ret !=0)
	{
        OSAL_stackpointFree(possible_taskid);
        printf("pthread_attr_setschedparam error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
        return(OSAL_ERROR);
	}

    /*active set priority&schedule*/
    ret = pthread_attr_setinheritsched(&custom_attr, PTHREAD_EXPLICIT_SCHED);
    if(ret !=0)
    {
       OSAL_stackpointFree(possible_taskid);
       printf("pthread_attr_setinheritsched error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
       return(OSAL_ERROR);
    }
#else
    printf("Priority scheduling not supported\n");
#endif

	/*
	** Create thread
	*/
	return_code = pthread_create(&(OSAL_task_tbl[possible_taskid].id),
								 &custom_attr,
								 function_pointer,
								 (void *)0);
	if (return_code != 0)
	{
	
        OSAL_stackpointFree(possible_taskid);
		pthread_mutex_lock(&OSAL_task_tbl_mutex); 
		OSAL_task_tbl[possible_taskid].free = TRUE;
		pthread_mutex_unlock(&OSAL_task_tbl_mutex); 
		printf("pthread_create error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
		return(OSAL_ERROR);
	}
    
	/*
	** Free the resources that are no longer needed
	*/
	return_code = pthread_detach(OSAL_task_tbl[possible_taskid].id);
	if (return_code !=0)
    {   
        OSAL_stackpointFree(possible_taskid);
        pthread_mutex_lock(&OSAL_task_tbl_mutex);
        OSAL_task_tbl[possible_taskid].free = TRUE;
        pthread_mutex_unlock(&OSAL_task_tbl_mutex);
        printf("pthread_detach error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
        return(OSAL_ERROR);
	}

	return_code = pthread_attr_destroy(&custom_attr);
	if (return_code !=0)
    {   
        OSAL_stackpointFree(possible_taskid);
        pthread_mutex_lock(&OSAL_task_tbl_mutex);
        OSAL_task_tbl[possible_taskid].free = TRUE;
        pthread_mutex_unlock(&OSAL_task_tbl_mutex);
        printf("pthread_attr_destroy error in OSAL_TaskCreate, Task ID = %d\n",possible_taskid);
        return(OSAL_ERROR);
	}

	/*下列的信息，和线程起来有一个时间差，有可能获取不到本任务信息。*/
	/*
	** Assign the task ID
	*/
	*task_id = possible_taskid;

	/* 
	** Initialize the table entries 
	*/
	pthread_mutex_lock(&OSAL_task_tbl_mutex); 

	OSAL_task_tbl[possible_taskid].free = FALSE;
	strcpy(OSAL_task_tbl[*task_id].name, (char*) task_name);
	OSAL_task_tbl[possible_taskid].creator = OSAL_FindCreator();
	OSAL_task_tbl[possible_taskid].stack_size = stack_size;
	/* Use the abstracted priority, not the OS one */
	OSAL_task_tbl[possible_taskid].priority = priority;

	pthread_mutex_unlock(&OSAL_task_tbl_mutex);

	return OSAL_SUCCESS;
	
}

/*
 一个任务删除另外一个任务
*/
INT32 OSAL_TaskDelete (UINT32 task_id)
{    
    int       ret;
    FuncPtr_t    FunctionPointer;
    void* stack_piont = NULL;
    
    /* 
    ** Check to see if the task_id given is valid 
    */
    if (task_id >= OSAL_MAX_TASKS || OSAL_task_tbl[task_id].free == TRUE)
    {
        return OSAL_ERR_INVALID_ID;
    }

    /*
    ** Call the thread Delete hook if there is one.
    */
    if ( OSAL_task_tbl[task_id].delete_hook_pointer != NULL)
    {
       FunctionPointer = (FuncPtr_t)(OSAL_task_tbl[task_id].delete_hook_pointer);
       (*FunctionPointer)();
    }

    /* 
    ** Try to delete the task 
    */
    ret = pthread_cancel(OSAL_task_tbl[task_id].id);
    if (ret != 0)
    {
        /*debugging statement only*/
        printf("FAILED PTHREAD CANCEL %d, %d \n",ret, ESRCH);
        return OSAL_ERROR;
    }    
    
    /*
    ** Now that the task is deleted, remove its 
    ** "presence" in OSAL_task_table
    */
    pthread_mutex_lock(&OSAL_task_tbl_mutex); 

    OSAL_task_tbl[task_id].free = TRUE;
    strcpy(OSAL_task_tbl[task_id].name, "");
    OSAL_task_tbl[task_id].creator = UNINITIALIZED;
    OSAL_task_tbl[task_id].stack_size = UNINITIALIZED;
    OSAL_task_tbl[task_id].priority = UNINITIALIZED;    
    OSAL_task_tbl[task_id].id = UNINITIALIZED;
    OSAL_task_tbl[task_id].delete_hook_pointer = NULL;
    if(OSAL_task_tbl[task_id].dym)
        stack_piont = OSAL_task_tbl[task_id].stack_point;
    OSAL_task_tbl[task_id].dym = UNINITIALIZED;
    OSAL_task_tbl[task_id].stack_point = NULL;
    
    pthread_mutex_unlock(&OSAL_task_tbl_mutex);

    if(stack_piont)
        aligned_free(stack_piont);

    return OSAL_SUCCESS;
    
}/* end OSAL_TaskDelete */

/*
 任务自己可以终止自己
*/
void OSAL_TaskExit(void)
{
    UINT32 task_id;
    void* stack_piont = NULL;

    task_id = OSAL_TaskGetId();

	if(task_id >= OSAL_MAX_TASKS)
		return ;
	
    pthread_mutex_lock(&OSAL_task_tbl_mutex); 

    OSAL_task_tbl[task_id].free = TRUE;
    strcpy(OSAL_task_tbl[task_id].name, "");
    OSAL_task_tbl[task_id].creator = UNINITIALIZED;
    OSAL_task_tbl[task_id].stack_size = UNINITIALIZED;
    OSAL_task_tbl[task_id].priority = UNINITIALIZED;
    OSAL_task_tbl[task_id].id = UNINITIALIZED;
    OSAL_task_tbl[task_id].delete_hook_pointer = NULL;
    if(OSAL_task_tbl[task_id].dym)
        stack_piont = OSAL_task_tbl[task_id].stack_point;
    OSAL_task_tbl[task_id].dym = UNINITIALIZED;    
    OSAL_task_tbl[task_id].stack_point = NULL;
    
    pthread_mutex_unlock(&OSAL_task_tbl_mutex);
    
    if(stack_piont)
        aligned_free(stack_piont);

    //pthread_exit(NULL);

}

/*
 任务挂起等待指定时间。
 单位=毫秒
*/
INT32 OSAL_TaskDelay(UINT32 millisecond)
{
    if (usleep(millisecond * 1000 ) != 0)
    {
        return OSAL_ERROR;
    }
    else
    {
        return OSAL_SUCCESS;
    }
}

/*
* 绑定OS threadid和TASK ID的对应关系。
* 在每个任务开始运行的时候，必须调用此函数，
* 不然OSAL_TaskGetId，OSAL_TaskExit函数将运行不正确。
*/

INT32 OSAL_TaskRegister (void)
{
    int          i;
    int          ret;
    UINT32       task_id;
    pthread_t    pthread_id;

    /* 
    ** Get PTHREAD Id
    */
    pthread_id = pthread_self();

    /*
    ** Look our task ID in table 
    */
    for(i = 0; i < OSAL_MAX_TASKS; i++)
    {
       if(OSAL_task_tbl[i].id == pthread_id)
       {
          break;
       }
    }
    task_id = i;

    if(task_id == OSAL_MAX_TASKS)
    {
        return OSAL_ERR_INVALID_ID;
    }

    /*
    ** Add pthread variable
    */
    ret = pthread_setspecific(thread_key, (void *)task_id);
    if ( ret != 0 )
    {
       printf("OSAL_TaskRegister Failed during pthread_setspecific function\n");
       return(OSAL_ERROR);
    }

    return OSAL_SUCCESS;
}/* end OSAL_TaskRegister */

/*
* 获取任务创建时的信息,供调试用
*/
INT32 OSAL_TaskGetInfo (UINT32 task_id, OSAL_task_info_t *task_prop)  
{
    /* 
    ** Check to see that the id given is valid 
    */
    if (task_id >= OSAL_MAX_TASKS || OSAL_task_tbl[task_id].free == TRUE)
    {
       return OSAL_ERR_INVALID_ID;
    }

    if( task_prop == NULL)
    {
       return OSAL_INVALID_POINTER;
    }

    /* put the info into the stucture */
    pthread_mutex_lock(&OSAL_task_tbl_mutex); 

    task_prop ->creator =    OSAL_task_tbl[task_id].creator;
    task_prop ->stack_size = OSAL_task_tbl[task_id].stack_size;
    task_prop ->stack_point = OSAL_task_tbl[task_id].stack_point;
    task_prop ->priority =   OSAL_task_tbl[task_id].priority;
    task_prop ->pthreadID =  (UINT32) OSAL_task_tbl[task_id].id;
    
    strcpy(task_prop->name, OSAL_task_tbl[task_id].name);

    pthread_mutex_unlock(&OSAL_task_tbl_mutex);
    
    return OSAL_SUCCESS;
    
} /* end OSAL_TaskGetInfo */

INT32 OSAL_TaskGetIdByName (UINT32 *task_id, const char *task_name)
{
    UINT32 i;

    if (task_id == NULL || task_name == NULL)
    {
       return OSAL_INVALID_POINTER;
    }
    
    /* 
    ** we don't want to allow names too long because they won't be found at all 
    */
    if (strlen(task_name) >= OSAL_MAX_API_NAME)
    {
       return OSAL_ERR_NAME_TOO_LONG;
    }

    for (i = 0; i < OSAL_MAX_TASKS; i++)
    {	
        if((OSAL_task_tbl[i].free != TRUE) &&
                (strcmp(OSAL_task_tbl[i].name,(char*) task_name) == 0 ))
        {
            *task_id = i;
            return OSAL_SUCCESS;
        }
    }
    /* The name was not found in the table,
    **  or it was, and the task_id isn't valid anymore 
    */
    return OSAL_ERR_NAME_NOT_FOUND;

}/* end OSAL_TaskGetIdByName */            

/*
* 注册任务删除的回调函数
*/
INT32 OSAL_TaskDeleteHook(void *function_pointer)
{
    UINT32 task_id;

    task_id = OSAL_TaskGetId();

    if ( task_id >= OSAL_MAX_TASKS )
    {
       return(OSAL_ERR_INVALID_ID);
    }

    pthread_mutex_lock(&OSAL_task_tbl_mutex); 

    if ( OSAL_task_tbl[task_id].free != FALSE )
    {
       /* 
       ** Somehow the calling task is not registered 
       */
       pthread_mutex_unlock(&OSAL_task_tbl_mutex);
       return(OSAL_ERR_INVALID_ID);
    }

    /*
    ** Install the pointer
    */
    OSAL_task_tbl[task_id].delete_hook_pointer = function_pointer;    
    
    pthread_mutex_unlock(&OSAL_task_tbl_mutex);

    return(OSAL_SUCCESS);
    
}/*end OSAL_TaskInstallDeleteHandler */

/*

*/

INT32 OSAL_taskApi_init(void)
{
    int i;
    int                 ret;
    pthread_mutexattr_t mutex_attr ;    
    INT32               return_code = OSAL_SUCCESS;    
    
     /* Initialize Task Table */
    
    for(i = 0; i < OSAL_MAX_TASKS; i++)
    {
         OSAL_task_tbl[i].free                = TRUE;
         OSAL_task_tbl[i].creator             = UNINITIALIZED;
         OSAL_task_tbl[i].delete_hook_pointer = NULL;
         OSAL_task_tbl[i].stack_point         = NULL;
         strcpy(OSAL_task_tbl[i].name,"");    
     }

    ret = pthread_key_create(&thread_key, NULL);
    if ( ret != 0 )
    {
       printf("Error creating thread key\n");
       return_code = OSAL_ERROR;
       return(return_code);
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
    

    ret = pthread_mutex_init((pthread_mutex_t *) &OSAL_task_tbl_mutex,&mutex_attr);
    if ( ret != 0 )
    {
       return_code = OSAL_ERROR;
       return(return_code);
    }

    return return_code;
    
}


