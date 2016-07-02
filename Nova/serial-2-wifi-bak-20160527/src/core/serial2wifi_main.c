#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netdb.h>
#include<stdarg.h>
#include<string.h>
#include<signal.h>

#include "ap_log.h"
#include "osal_api.h"
#include "pktbuff.h"
#include "util.h"

#include "network_com.h"
#include "serial2wifi_main.h"

TASK_LIST_T tasklist[]={
	//{-1, TASK_UART_TX_NAME, UART_tx_pthread, NULL, 0, TASK_UART_TX_PRIO, 0},
	//{-1, TASK_UART_RX_NAME, UART_rx_pthread, NULL, 0, TASK_UART_RX_PRIO, 0},
	{-1, TASK_NETWORK_TX_NAME, NETWORK_tx_pthread, NULL, 0, TASK_NETWORK_TX_PRIO, 0},
	{-1, TASK_NETWORK_CLIENT_RX_NAME, NETWORK_rx_Client_pthread, NULL, 0, TASK_NETWORK_CLIENT_RX_PRIO, 0},
	{-1, TASK_NETWORK_SERVER_RX_NAME, NETWORK_rx_Server_pthread, NULL, 0, TASK_NETWORK_SERVER_RX_PRIO, 0},
};

QUEUE_LIST_T queuelist[]={
	/*config thread receive.*/
	{-1, QUEUE_CONFIG_NAME, QUEUE_CONFIG_NUMS, sizeof(MSG_HEAD), 0},
	/*for UART TX thread to sending data to host.*/
	{-1, QUEUE_UART_TX_NAME, QUEUE_CONFIG_NUMS, sizeof(MSG_HEAD), 0},		
};

SEM_LIST_T semlist[]={
	/*pcap -> parse sync.*/
	{-1, SEM_PARSE_QUEUE_NAME, SEM_EMPTY},
	/*all thread sync.*/
	{-1, SEM_DEVICE_SYNC_NAME, SEM_EMPTY}
};

MUTEX_LIST_T mutexlist[]={
	{-1, MUTEX_CONFIG_NAME, SEM_EMPTY},	
	{-1, NULL, 0}
};

static INT32 OSAL_init(void)
{
	INT32 ret_code = OSAL_SUCCESS;
	
	ret_code = OSAL_taskApi_init();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"OSAL_taskApi_init Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
	
	ret_code = OSAL_SemaphoreApi_init();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"OSAL_SemaphoreApi_init Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
		
	ret_code = OSAL_QueueApi_init();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"OSAL_QueueApi_init Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}

	ret_code = OSAL_TimerAPIInit();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"OSAL_TimerAPIInit Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
	
	ret_code = init_bufferApi();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"init_bufferApi Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
	
	return STATUS_SUCCESS;
}

static INT32 tasks_start(void)
{
	int arr = 0;
	INT32 retcode = OSAL_SUCCESS;

	for(arr = 0; arr< ARRAY_SIZE(tasklist); arr++)
	{
		if(tasklist[arr].name != NULL)
		{
			retcode = OSAL_TaskCreate(&tasklist[arr].tid, tasklist[arr].name, 	\
						tasklist[arr].function_pointer, tasklist[arr].stack_pointer, \
						tasklist[arr].stack_size, tasklist[arr].priority, tasklist[arr].flags);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CORE, "task create %s return =%d\n",tasklist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_CORE, "task :%s,  tid =%d\n", tasklist[arr].name,tasklist[arr].tid);

		}
	}

	return STATUS_SUCCESS;

}

static INT32 resource_alloc(void)
{
	int arr = 0;
	INT32 retcode = OSAL_SUCCESS;

	/*CREATE QUEUE;*/
	for(arr = 0; arr< ARRAY_SIZE(queuelist); arr++)
	{
		if(queuelist[arr].name != NULL)
		{
			retcode = OSAL_QueueCreate(&queuelist[arr].qid, queuelist[arr].name, 	\
						queuelist[arr].queue_nums, queuelist[arr].queue_size, queuelist[arr].flags);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CORE, "QUEUE create %s return =%d\n",queuelist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_CORE, "QUEUE :%s,  QID =%d\n", queuelist[arr].name,queuelist[arr].qid);

		}
	}

	/*CREATE Semaphore;*/
	for(arr = 0; arr< ARRAY_SIZE(semlist); arr++)
	{
		if(semlist[arr].name != NULL)
		{
			retcode = OSAL_SemaphoreCreate(&semlist[arr].semID, semlist[arr].name, semlist[arr].options);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CORE, "Semaphore name: %s, create return =%d\n",semlist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_CORE, "Semaphore :%s,  semID =%d\n", semlist[arr].name,semlist[arr].semID);

		}
	}


	/*CREATE Semaphore;*/
	for(arr = 0; arr< ARRAY_SIZE(mutexlist); arr++)
	{
		if(mutexlist[arr].name != NULL)
		{
			retcode = OSAL_MutSemCreate(&mutexlist[arr].mutexID, mutexlist[arr].name, mutexlist[arr].options);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CORE, "Mutex name: %s, create return =%d\n",mutexlist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_CORE, "Mutex :%s,  muxID =%d\n", mutexlist[arr].name,mutexlist[arr].mutexID);

		}
	}	
	
	return STATUS_SUCCESS;
}

static INT32 resource_release(void)
{
	int arr = 0;
	INT32 retcode = OSAL_SUCCESS;

	/*release task;*/
	for(arr = 0; arr< ARRAY_SIZE(tasklist); arr++)
	{
		if((tasklist[arr].name != NULL) && (tasklist[arr].tid != -1))
		{
			retcode = OSAL_TaskDelete(tasklist[arr].tid);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CORE, "TASK delete %s, tid = %d, return =%d\n",
						tasklist[arr].name, tasklist[arr].tid, retcode);
			}			
		}
	}

	/*release QUEUE. file name;*/
	for(arr = 0; arr< ARRAY_SIZE(queuelist); arr++)
	{
		if((queuelist[arr].name != NULL) && (queuelist[arr].qid != -1))
		{
			retcode = OSAL_QueueDelete(queuelist[arr].qid);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CORE, "QUEUE %s Qid =%d, delete return =%d\n",	\
						queuelist[arr].name, queuelist[arr].qid, retcode);
			}			
		}
	}

	/*free queue buffers.*/
	unInit_bufferApi();
	return STATUS_SUCCESS;

}

static void serial2wifi_cleanup(int signo)
{
	AP_LOG(AP_LOG_NOTICE, LOG_MOUDLES_CORE, "catch signal =%d\n", signo);
	NETWORK_uninit();
	resource_release();
	exit(0);
}

static void *setsignal (int sig, void (*func)(int))
{
	struct sigaction old, new;
	memset(&new, 0, sizeof(new));
	new.sa_handler = func;
	if (sigaction(sig, &new, &old) < 0)
		return (SIG_ERR);
	return (old.sa_handler);
}

static void usage(void)
{
	fprintf(stderr, "Usage: network [<options> ...]\n\n"
        "Following options are available:\n"
        "        -t <type>        eg: tcp-0 udp-1\n"
        "        -m <mode>        eg: CLIENT-0 SERVER-1 CLIENT_SERVER-2\n"
        "        -d <dest ip>     eg: 192.168.1.10\n"
        "        -p <dest port>   eg: 5678\n"
        "        -f <config file> eg: /var/run/serial2wifi.ini\n");
 
	exit(1);
}

int main(int argc, char *argv[])
{
	int ch = 0;
	INT32 retcode= OSAL_SUCCESS;
	char *pcEnd = NULL;
	char *configFile = NULL;
	struct timeval tv1;
	
	while ((ch = getopt(argc, argv,	"t:m:d:p:f:")) != -1)
	{
		switch (ch)
		{
			case 't':
				g_NetWork.type = atoi(optarg);
				break;
			case 'm':
				g_NetWork.mode = atoi(optarg);
				break;
			case 'd':
				g_NetWork.server_ip = optarg;
				break;
			case 'p':
				g_NetWork.server_port = atoi(optarg);
				g_NetWork.listen_port = atoi(optarg) + 1;
				break;
			case 'f':
				configFile = optarg;
				break;
			default:
				usage();
		}
	}
	
	printf("type:\t%s\nmode:\t%s\nip:\t%s\nport:\t%d\nlport:\t%d\n\n",
		g_NetWork.type == 0 ? "TCP" : "UDP",
		g_NetWork.mode == 0 ? "Client" : g_NetWork.mode == 1 ? "Server" : "Client && Server",
		g_NetWork.server_ip, g_NetWork.server_port, g_NetWork.listen_port);

	(void)setsignal(SIGPIPE, serial2wifi_cleanup);
	(void)setsignal(SIGTERM, serial2wifi_cleanup);
	(void)setsignal(SIGINT, serial2wifi_cleanup);
	(void)setsignal(SIGHUP, serial2wifi_cleanup);

	/*
		Init sequence.
	*/
	
	if(OSAL_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"OSAL_init Failure.\n");
		exit(1);
	}

	if(resource_alloc() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"resource_alloc Failure.\n");
		exit(1);
	}
#if 0	
	if(CM_ctrl_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"CM_ctrl_init Failure.\n");
		exit(1);	
	}
#endif
	if(CONFIG_init(configFile) != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"CONFIG_init Failure.\n");
		exit(1);	
	}
#if 0
	if(SNIFF_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"SNIFF_init Failure.\n");
		exit(1);	
	}
	
	if(PARSE_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"PARSE_init Failure.\n");
		exit(1);	
	}
	
	if(UART_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"UART_init Failure.\n");
		exit(1);	
	}
#endif	
	if(NETWORK_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"ETHERNET_init Failure.\n");
		exit(1);
	}

	/*CREATE TASKS.*/
	retcode = tasks_start();
	if(retcode != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_CORE,"tasks_start Failure.\n");
		exit(1);
	}

	while(1)
	{
		OSAL_TaskDelay(0xffffffff);
		continue;
	}

	return 0;
}

