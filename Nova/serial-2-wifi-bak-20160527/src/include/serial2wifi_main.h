#ifndef _SERIAL2WIFI_MAIN_H
#define _SERIAL2WIFI_MAIN_H

#include "common_types.h"

/*TASK list.*/

typedef enum{

	TASK_UART_TX_PRIO = 10,
	TASK_NETWORK_TX_PRIO,
	TASK_PARSE_PRIO,
	TASK_PCAP_PRIO,
	TASK_CONFIG_PRIO,
	TASK_UART_RX_PRIO ,
	TASK_CHANNAL_PRIO ,
	TASK_TIMER_PRIO ,
	TASK_NETWORK_CLIENT_RX_PRIO,
	TASK_NETWORK_SERVER_RX_PRIO,

	TASK_DEBUG_CTRL_PRIO ,

	TASK_PRIO_END =99,
	
}TASK_PRIO;

/*TASK name.*/
#define TASK_PCAP_NAME			"tPCAP"
#define TASK_PARSE_NAME			"tPARSE"
#define TASK_CONFIG_NAME		"tCONFIG"
#define TASK_UART_TX_NAME		"tUART TX"
#define TASK_UART_RX_NAME		"tUART RX"
#define TASK_DEBUG_CTRL_NAME	"tDEBUG"
#define TASK_TIMER_NAME			"tTIMER"
#define TASK_CHANNAL_NAME		"tChannel"
#define TASK_NETWORK_TX_NAME	"tNETWORK TX"
#define TASK_NETWORK_CLIENT_RX_NAME	"tNETWORK CLIENT RX"
#define TASK_NETWORK_SERVER_RX_NAME	"tNETWORK SERVER RX"

/*QUEUE name.*/
#define QUEUE_CONFIG_NAME		"qCONFIG"
#define QUEUE_CONFIG_NUMS		10

#define QUEUE_UART_TX_NAME		"qUartTx"
#define QUEUE_UART_TX_NUMS		10


/*SYNC SEM name*/
#define SEM_PARSE_QUEUE_NAME	"sPARSE SYNC"
#define SEM_DEVICE_SYNC_NAME	"DEVICE SYNC"

/*MUT NAME*/
#define MUTEX_CONFIG_NAME		"lockDB"

/*MSG TYPE*/
typedef enum{

	MSG_TIMEOUT = 0,
		
	MSG_CONIFG = 0x1000,
	MSG_SLIP_TO_CONFIG,
	MSG_SUBTYPE0_0,
	MSG_SUBTYPE0_1,
	MSG_SUBTYPE0_2,
	MSG_SUBTYPE0_3,
	
	MSG_UART_TX = 0x2000,
	MSG_CONFIG_TO_UART,
	MSG_SUBTYPE1_0,
	MSG_SUBTYPE2_1,
	MSG_SUBTYPE3_2,
	MSG_SUBTYPE4_3,
	
	MSG_END
}MSG_TYPE;

typedef struct task_list_t{
	TASK_ID tid;
	char *name;
	void *(*function_pointer)(void *);
	char *stack_pointer;
	UINT32 stack_size;
	UINT32 priority;
	UINT32 flags;
}TASK_LIST_T;

typedef struct queue_list_t{
	QUEUE_ID qid;
	char *name;
	UINT32 queue_nums;
	UINT32 queue_size;
	UINT32 flags;
}QUEUE_LIST_T;

typedef struct sem_list{
	SEM_ID semID;
	char *name;
	UINT32 options;
}SEM_LIST_T;

typedef struct mux_list{
	MUX_ID mutexID;
	char *name;
	UINT32 options;
}MUTEX_LIST_T;

#endif

