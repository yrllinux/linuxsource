#include <stdio.h>
#include <stdlib.h>

/*
* 对齐分配内存
*/
void *aligned_malloc(int size, int alignment)
{
    void *ptr = (void*)malloc(size + alignment);
    if (ptr)
    {
        void * aligned =(void *)(((unsigned long)ptr + alignment) & (~(alignment-1)));
        *(unsigned long *)((unsigned long)aligned - sizeof(long)) = ptr;
        return aligned;
    }

    return NULL;
}

/*
* 释放内存
*/

void *aligned_free(void *paligned)
{
    void *real;
    if(NULL == paligned)
		return NULL;
	
    if((unsigned long)paligned < sizeof(long))
        return NULL;
    
    real = (void *)(*(long*)((unsigned long)paligned-sizeof(long)));
    free(real);
}

void *osal_malloc(int size)
{
	return malloc((size_t)size);
}

void osal_free(void *ptr)
{
	if(ptr)
		free(ptr);
}

