#ifndef _osal_api_
#define _osal_api_

#include <pthread.h>
#include <sched.h>

#include "common_types.h"

#include "err_code.h"	

/*global config define.*/
/*
** Platform Configuration Parameters for the OSAL API
*/

#define OSAL_MAX_TASKS		    20
#define OSAL_MAX_MUTEXES            20
#define OSAL_MAX_BIN_SEMAPHORES     20
#define OSAL_MAX_QUEUES			20

/* 
** The maxium length allowed for a object (task,queue....) name 
*/

#define OSAL_MAX_API_NAME	    20


/*(1-OSAL_TASK_MAX_PRIORITY); 1 = lowest, 
** OSAL_TASK_MAX_PRIORITY = highest
*/

#define OSAL_TASK_MAX_PRIORITY		99

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN           8092
#endif

#define OSAL_TASK_STACK_MIN 		PTHREAD_STACK_MIN


#define NO_WAIT           0
#define WAIT_FOREVER     -1


/*
*Semaphore define.
*/

#define SEM_EMPTY           0
#define SEM_FULL            1

#define SYSCLOCK_HZ         100
#define TICKS_TO_MSEC(x)	(((x) * 1000) / SYSCLOCK_HZ)

/*TASKS */
typedef struct
{
    char name [OSAL_MAX_API_NAME];
    UINT32 creator;
    UINT32 stack_size;
    UINT32 priority;
    UINT32 pthreadID;
    void *stack_point;
}OSAL_task_info_t;

/*SEMAPHORE*/
typedef struct
{                     
    char name [OSAL_MAX_API_NAME];
    UINT32 creator;
    INT32  value;
    INT32  lock_count;
    
}OSAL_sem_info_t;

/*
QUEUE DEFINE
*/

/*消息队列优先级*/
#define MSG_NORMAL			0
#define MSG_URGENT			1

/*在发送MSG的时候，需要指定这个flags*/
#define MSG_FLAGS_NONE		0x01		/*不需要释放内存*/
#define MSG_FLAGS_OSAL		0x02		/*需要用OSAL函数来释放内存*/
#define MSG_FLAGS_MALLOC	0x04		/*需要用FREE函数来释放内存*/

typedef union
{
    int parameter;
    char *pAddedMsg;
} MSG_PARA2;

typedef struct 
{
    unsigned char option; /*noarmal/priority*/
    unsigned char flags; /*OSAL_ALLOC, MALLOC, NO_FREE*/
    unsigned short  msgLen;  /*指定内容的长度*/  
        
    int             destQID;	/*接收者的queue ID*/
    int             msgType;	/*消息类型*/
    int             msgPara1;	/*参数1*/
    
    /*msglen !=0 msgPara2有效*/
    MSG_PARA2       msgPara2;
} MSG_HEAD;


/*QUEUE INFO*/
typedef struct
{
	char name [OSAL_MAX_API_NAME];
	UINT32 creator;
}OSAL_queue_prop_t;

/*
* extern function
*/

/*********************************TASK***************************************************/
extern INT32 OSAL_taskApi_init(void);
extern INT32 OSAL_TaskCreate (UINT32 *task_id, const char *task_name, void* (*function_pointer)(void*),
                      const char *stack_pointer, UINT32 stack_size, UINT32 priority,
                      UINT32 flags);
extern UINT32 OSAL_TaskGetId (void);
extern INT32 OSAL_TaskDelete (UINT32 task_id);
extern void OSAL_TaskExit(void);
extern INT32 OSAL_TaskDelay(UINT32 millisecond);
extern INT32 OSAL_TaskRegister (void);
extern UINT32 OSAL_FindCreator(void);
extern INT32 OSAL_TaskGetInfo (UINT32 task_id, OSAL_task_info_t *task_prop);
extern INT32 OSAL_TaskGetIdByName (UINT32 *task_id, const char *task_name);

/**********************************END*****************************************************/

/**************************************SEMAPHORE*********************************************/
extern INT32 OSAL_MutSemCreate (UINT32 *sem_id, const char *sem_name, UINT32 options);
extern INT32 OSAL_MutSemDelete (UINT32 sem_id);
extern INT32 OSAL_MutSemGive (UINT32 sem_id);
extern INT32 OSAL_MutSemTake (UINT32 sem_id);
extern INT32 OSAL_MutSemGetInfo (UINT32 sem_id, OSAL_sem_info_t *mut_prop);
extern INT32 OSAL_MutSemGetIdByName (UINT32 *sem_id, const char *sem_name);
extern INT32 OSAL_SemaphoreCreate (UINT32 *sem_id, const char *sem_name, UINT32 options);
extern INT32 OSAL_SemaphoreDelete (UINT32 sem_id);
extern INT32 OSAL_SemaphoreGive (UINT32 sem_id);
extern INT32 OSAL_SemaphoreFlush (UINT32 sem_id);
extern INT32 OSAL_SemaphoreTake ( UINT32 sem_id, int timeout);
extern INT32 OSAL_SemaphoreGetInfo (UINT32 sem_id, OSAL_sem_info_t *bin_prop);
extern INT32 OSAL_SemaphoreGetIdByName (UINT32 *sem_id, const char *sem_name);
extern INT32 OSAL_SemaphoreApi_init(void);

/***************************************END**************************************************/

/*****************************************QUEUE**********************************************/
extern INT32 OSAL_QueueCreate (UINT32 *queue_id, const char *queue_name, 
						UINT32 queue_depth,UINT32 data_size, UINT32 flags);

extern INT32 OSAL_QueueDelete (UINT32 queue_id);

extern INT32 OSAL_msgSend(MSG_HEAD * msg, int timeout);

extern INT32 OSAL_msgReceive(UINT32 queue_id, char *buf, UINT32 buflen, int timeout);

extern INT32 OSAL_QueueGetInfo (UINT32 queue_id, OSAL_queue_prop_t *queue_prop);

extern INT32 OSAL_QueueGetIdByName (UINT32 *queue_id, const char *queue_name);

/*****************************************END*************************************************/
#endif
