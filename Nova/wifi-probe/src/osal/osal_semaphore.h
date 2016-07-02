#ifndef _osal_semaphore_
#define _osal_semaphore_

#include <pthread.h>

/* Binary Semaphores */
typedef struct
{
    int             free;
    pthread_mutex_t id;     /*互斥信号量*/
    pthread_cond_t  cv;     /*条件变量*/
    char            name [OSAL_MAX_API_NAME];
    int             creator;
    int             max_value;  /*SEM_FULL.*/
    int             current_value; /*当前值，SEM_EMPTY, SEM_FULL*/
}OSAL_bin_sem_record_t;


/* Mutexes */
typedef struct
{
    int             free;
    pthread_mutex_t id;     /*pthread mutex id.*/
    char            name [OSAL_MAX_API_NAME];
    int             creator;    /*哪个任务创建*/
    int             lock_count; /*目前有多少个lock Take.*/
    
}OSAL_mut_sem_record_t;

#endif

