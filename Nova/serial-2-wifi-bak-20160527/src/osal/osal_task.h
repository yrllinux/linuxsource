#ifndef _osal_task_
#define _osal_task_

#include <pthread.h>

/*tasks */
typedef struct
{
    int       free;
    pthread_t id;
    char      name [OSAL_MAX_API_NAME];
    int       creator;
    UINT32    stack_size;
    UINT32    priority;
    void     *delete_hook_pointer;
	void     *stack_point;
	int      dym;
    
}OSAL_task_record_t;


#endif
