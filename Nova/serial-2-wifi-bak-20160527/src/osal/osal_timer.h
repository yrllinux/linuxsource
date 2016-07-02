#ifndef __OSAL_TIMER_H_
#define __OSAL_TIMER_H_

#include <sys/time.h>
#include "common_types.h"
#include "linklist.h"

#define DEFAULT_MIN_TIMER_TICKS 	100		/*MS*/

#define TIMER_ONETIME		0x1
#define TIMER_REPEATER		0x2

typedef struct 
{
	LIST_ENTRY node;
	UINT16              flags;
	UINT16 				used;
	void(*timer_cb)(INT32 args);
	UINT32 args;
	INT32 timerID;
	struct	timeval expires;
	
}TIMERBLK;



#endif
