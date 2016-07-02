#ifndef __TTY_H_
#define __TTY_H_
#include "common_types.h"

typedef struct
{
	INT32 speed;
	INT32 flowctrl;
	INT32 databits;
	INT32 stopbits;
	INT32 parity;
	
}UART_OPTIONS;

typedef enum {
	NO_FLOWCTRL =0,
	HW_FLOWCTRL,
	SOFT_FLOWCTRL,
	END_FLOWCTRL
}TTY_FLOWCTRL;

typedef enum{
	DATABITS_5 =0,
	DATABITS_6,
	DATABITS_7,
	DATABITS_8,
	END_DATABITS
	
}TTY_DATABITS;

typedef enum{
	PARITY_NONE =0,
	PARITY_ODD,
	PARITY_EVEN,
	END_PARITY
	
}TTY_PARITY;

typedef enum{
	STOPBITS_1 = 0,
	STOPBITS_2,
	STOPBITS_END
}TTY_STOPBITS;

#endif
