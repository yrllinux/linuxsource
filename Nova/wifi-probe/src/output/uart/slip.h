#ifndef __SLIP_H_
#define __SLIP_H_

#include "common_types.h"

#define MAX_SLIP_MTU	 1024
#define MIN_SLIP_MTU	 5
#define SLIP_END         0xC0
#define SLIP_ESC         0xDB
#define SLIP_ESC_END   	 0xDC
#define SLIP_ESC_ESC   	 0xDD

/*SLIP FLAGS DEFINE.*/
#define SLIP_ESCAPE	0               /* ESC received                 */
#define SLIP_ERROR	1               /* Parity, etc. error           */	

typedef struct _slip{

	MUX_ID	lock;
	UINT32  flags;
	char 	recvbuff[MAX_SLIP_MTU];
	INT32 	rcount;
	INT32 	mtu;
	char   xmitbuff[MAX_SLIP_MTU];
	INT32   tcount;

	INT32 (*SLIP_reset)(struct _slip *slip);
	INT32 (*SLIP_decap)(struct _slip *slip, UINT8 ch);
	INT32 (*SLIP_encap)(struct _slip *slip, char *buff, int len);
	
}SLIP_T;

#endif
