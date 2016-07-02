#ifndef __NETWORKCOM_H_
#define __NETWORKCOM_H_

#include "common_types.h"

#define RECV_ONE_CHAR_TIMEOUT	1000	/*UNIT = MS*/
#define DEFAULT_WIFI_NOISE		95

#define BUFFER_SIZE	1460 
#define MAX_LINK_COUNT	32 

typedef struct _NETWORK
{
	UINT8	type;			/*tcp or udp*/
	UINT8	mode;			/*client or server*/
	INT32	client_fd;		/*client fd*/
	INT32	server_fd[MAX_LINK_COUNT];		/*server fd*/
	UINT8	*server_ip;		/*remote ip*/
	UINT16	server_port;	/*remote port*/
	UINT16	listen_port;	/*server listen port*/
}NETWORK_T;

enum{
	TCP = 0,
	UDP
};

enum{
	CLIENT = 0,
	SERVER,
	CLIENT_SERVER
};

extern NETWORK_T g_NetWork;

void *NETWORK_tx_pthread(void* args);
void *NETWORK_rx_Client_pthread(void* args);
void *NETWORK_rx_Server_pthread(void* args);
#if 1
void *UART_tx_pthread(void* args);
#endif

#endif
