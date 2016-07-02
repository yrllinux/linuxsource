#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>

#include "common.h"
#include "sniff_public.h"
#include "ap_log.h"

#include "osal_api.h"
#include "pktbuff.h"

#include "wifiprobe.h"
#include "util.h"

AP_LOG_LEVEL_E g_enDebugLevel = AP_LOG_DEBUG;


/*****extern funcs*****************************/
extern void *UART_tx_pthread(void* args);
extern void *CONFIG_pthread(void* args);
extern void *UART_rx_pthread(void* args);
extern void *PARSE_pthread(void* args);
extern void *CM_ctrl_handle(void* args);
extern void *sniff_handle(void *args);
extern void * timer_pthread(void *args);
extern void * channel_change_handle(void *args);

extern void pcap_cleanup(void);


typedef struct task_list_t{
	TASK_ID tid;
	char *name;
	void *(*function_pointer)(void *);
	char *stack_pointer;
	UINT32 stack_size;
	UINT32 priority;
	UINT32 flags;
}TASK_LIST_T;

TASK_LIST_T tasklist[]={
	
	{
		.tid = -1, 
		.name = TASK_UART_TX_NAME, 
		.function_pointer = UART_tx_pthread, 
		.stack_pointer = NULL, 
		.stack_size = 0, 
		.priority = TASK_UART_TX_PRIO, 
		.flags = 0
	},
	{-1, TASK_CONFIG_NAME, CONFIG_pthread, NULL, 0, TASK_CONFIG_PRIO, 0},
	{-1, TASK_UART_RX_NAME, UART_rx_pthread, NULL, 0, TASK_UART_RX_PRIO, 0},
	{-1, TASK_PARSE_NAME, PARSE_pthread, NULL, 0, TASK_PARSE_PRIO, 0},
	{-1, TASK_PCAP_NAME, sniff_handle, NULL, 0, TASK_PCAP_PRIO, 0},	
	//{-1, TASK_TIMER_NAME, timer_pthread, NULL, 0, TASK_TIMER_PRIO, 0}, 
	{-1, TASK_CHANNAL_NAME, channel_change_handle, NULL, 0, TASK_CHANNAL_PRIO, 0},
	
	{-1, TASK_DEBUG_CTRL_NAME, CM_ctrl_handle, NULL, 0, TASK_DEBUG_CTRL_PRIO, 0}
	
};


typedef struct queue_list_t{
	QUEUE_ID qid;
	char *name;
	UINT32 queue_nums;
	UINT32 queue_size;
	UINT32 flags;
}QUEUE_LIST_T;

QUEUE_LIST_T queuelist[]={
	/*config thread receive.*/
	{-1, QUEUE_CONFIG_NAME, QUEUE_CONFIG_NUMS, sizeof(MSG_HEAD), 0},
	/*for UART TX thread to sending data to host.*/
	{-1, QUEUE_UART_TX_NAME, QUEUE_CONFIG_NUMS, sizeof(MSG_HEAD), 0},		
};

typedef struct sem_list{
	SEM_ID semID;
	char *name;
	UINT32 options;
}SEM_LIST_T;

SEM_LIST_T semlist[]={
	/*pcap -> parse sync.*/
	{-1, SEM_PARSE_QUEUE_NAME, SEM_EMPTY},
	/*all thread sync.*/
	{-1, SEM_DEVICE_SYNC_NAME, SEM_EMPTY}
};

typedef struct mux_list{
	MUX_ID mutexID;
	char *name;
	UINT32 options;
}MUTEX_LIST_T;

MUTEX_LIST_T mutexlist[]={
	{-1, MUTEX_CONFIG_NAME, SEM_EMPTY},	
	{-1, NULL, 0}
};

WIFI_PROBE_T gWifiProbe;

char g_acSniffDevName[16] = {0};	/* 设备名称最大长度为16 */
int	g_iSnapSize = 0;
int	g_iPrintFlag = 0;

static INT16 default_qq_ports[]={80, 8080, 433, 14000};

static void usage(void)
{
	fprintf(stderr, "Usage: wifiprobe [<options> ...]\n\n"		
        "Following options are available:\n"
        "        -d                      verbose, more d can output more debug info.eg: -dddd\n"
        "        -f                      config file.\n"
        "        -p <ctrl_interface>     debug ctrl interface\n"
        "        -i <dev_interface>      sniff frame interface\n"
        "        -s <snapsize>           sniff frame size\n"
        "        -P                      handle frame by print\n");
    
	exit(1);
}

INT32 OSAL_init(void)
{
	INT32 ret_code = OSAL_SUCCESS;
	
	ret_code = OSAL_taskApi_init();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"OSAL_taskApi_init Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
	
	ret_code = OSAL_SemaphoreApi_init();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"OSAL_SemaphoreApi_init Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
		
	ret_code = OSAL_QueueApi_init();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"OSAL_QueueApi_init Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}

	ret_code = OSAL_TimerAPIInit();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"OSAL_TimerAPIInit Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
	
	ret_code = init_bufferApi();
	if(ret_code != OSAL_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"init_bufferApi Failure.ret=%d\n", ret_code);
		return STATUS_ERROR;
	}
	
	return STATUS_SUCCESS;
}

void wifiprobe_init(void)
{
	int ii;
    memset((void *)&gWifiProbe, 0, sizeof(WIFI_PROBE_T));
    strcpy(gWifiProbe.config.ctrl_intf, DEFAULT_CTRL_INTF_NAME);
    strcpy(gWifiProbe.config.tty_name, DEFAULT_TTY_NAME);
	gWifiProbe.config.probe_fixed_mac_nums = 0;
	gWifiProbe.config.chn_mask = DEFAULT_SCAN_CH;
	gWifiProbe.config.chn_scan_time = DEFAULT_CH_SW_TIME;
	gWifiProbe.config.report_time = DEFAULT_REPORT_TIME;
	gWifiProbe.config.qqport_nums = COMMON_MIN(ARRAY_SIZE(default_qq_ports), MAX_QQ_PORTS);
	for(ii =0; ii<gWifiProbe.config.qqport_nums; ii++)
		gWifiProbe.config.qqport[ii] = default_qq_ports[ii];
}

INT32 probe_tasks_start(void)
{
	int arr;
	INT32 retcode;

	for(arr = 0; arr< ARRAY_SIZE(tasklist); arr++)
	{
		if(tasklist[arr].name != NULL)
		{
			retcode = OSAL_TaskCreate(&tasklist[arr].tid, tasklist[arr].name, 	\
						tasklist[arr].function_pointer, tasklist[arr].stack_pointer, \
							tasklist[arr].stack_size, tasklist[arr].priority, tasklist[arr].flags);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "task create %s return =%d\n",tasklist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_MAIN, "task :%s,  tid =%d\n", tasklist[arr].name,tasklist[arr].tid);

		}
	}

	return STATUS_SUCCESS;

}

INT32 probe_resource_alloc(void)
{
	int arr;
	INT32 retcode;

	/*CREATE QUEUE;*/
	for(arr = 0; arr< ARRAY_SIZE(queuelist); arr++)
	{
		if(queuelist[arr].name != NULL)
		{
			retcode = OSAL_QueueCreate(&queuelist[arr].qid, queuelist[arr].name, 	\
						queuelist[arr].queue_nums, queuelist[arr].queue_size, queuelist[arr].flags);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "QUEUE create %s return =%d\n",queuelist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_MAIN, "QUEUE :%s,  QID =%d\n", queuelist[arr].name,queuelist[arr].qid);

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
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "Semaphore name: %s, create return =%d\n",semlist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_MAIN, "Semaphore :%s,  semID =%d\n", semlist[arr].name,semlist[arr].semID);

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
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "Mutex name: %s, create return =%d\n",mutexlist[arr].name, retcode);
				return STATUS_ERROR;
			}			

			AP_LOG(AP_LOG_INFO, LOG_MOUDLES_MAIN, "Mutex :%s,  muxID =%d\n", mutexlist[arr].name,mutexlist[arr].mutexID);

		}
	}	
	
	return STATUS_SUCCESS;
}

INT32 probe_resource_release(void)
{

	int arr;
	INT32 retcode;

	/*release task;*/
	for(arr = 0; arr< ARRAY_SIZE(tasklist); arr++)
	{
		if((tasklist[arr].name != NULL) && (tasklist[arr].tid != -1))
		{
			retcode = OSAL_TaskDelete(tasklist[arr].tid);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "TASK delete %s, tid = %d, return =%d\n",
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
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "QUEUE %s Qid =%d, delete return =%d\n",	\
						queuelist[arr].name, queuelist[arr].qid, retcode);
			}			
		}
	}

#if 0
	/*release Semaphore;*/
	for(arr = 0; arr< ARRAY_SIZE(semlist); arr++)
	{
		if((semlist[arr].name != NULL) && (semlist[arr].semID != -1))
		{
			retcode = OSAL_SemaphoreDelete(semlist[arr].semID);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "Semaphore name: %s, semID = %d, delete return =%d\n",	\
						semlist[arr].name, semlist[arr].semID, retcode);
			}			

		}
	}

	/*release Semaphore;*/
	for(arr = 0; arr< ARRAY_SIZE(mutexlist); arr++)
	{
		if((mutexlist[arr].name != NULL) && (mutexlist[arr].mutexID != -1))
		{
			retcode = OSAL_MutSemDelete(mutexlist[arr].mutexID);
			
			if(retcode != OSAL_SUCCESS)
			{
				AP_LOG(AP_LOG_ERR, LOG_MOUDLES_MAIN, "Mutex name: %s, Mutid = %d ,delete return =%d\n",
						mutexlist[arr].name, mutexlist[arr].mutexID, retcode);
			}			
		}
	}	
	
#endif

	/*free queue buffers.*/
	unInit_bufferApi();
	UART_uninit();
	return STATUS_SUCCESS;

}



void dump_probe_stat(void)
{
	PROBE_STATISTIC stat;
	memcpy((void*)&stat, (void*)&gWifiProbe.stat, sizeof(PROBE_STATISTIC));

	printf("total  packet: =%d\n", stat.all_packets);
	printf("good   packet: =%d\n", stat.good_packets);
	printf("large  packet: =%d\n", stat.large_packets);
	printf("total  bytes : =%llu\n", stat.bytes);

	printf("80211    ctrl: =%d\n", stat.ctrl80211);
	printf("80211    mng : =%d\n", stat.mng80211);
	printf("80211    data: =%d\n", stat.data80211);
	printf("80211    null: =%d\n", stat.null80211);	
	printf("80211   encry: =%d\n", stat.encry80211);	
	printf("80211   other: =%d\n", stat.other80211);

	printf("ip4          : =%d\n", stat.ip4);
	printf("ip6          : =%d\n", stat.ip6);
	printf("other   proto: =%d\n", stat.proto_others);
	printf("udp          : =%d\n", stat.udp);	
	printf("tcp          : =%d\n", stat.tcp);	
	printf("other ipproto: =%d\n", stat.ipproto_others);
	printf("http         : =%d\n", stat.http80);	
	printf("http data    : =%d\n", stat.http_data);	

	printf("no pkt buffer: =%d\n", stat.no_pkt_buff);
	printf("pkt   enqueue: =%d\n", stat.pkt_enqueues);
	printf("pkt   dequeue: =%d\n", stat.pkt_dequeues);
	printf("no probe buff: =%d\n", stat.no_probe_buff);	
	
	printf("good   probes: =%d\n", stat.good_probe_info);
	printf("probe enqueue: =%d\n", stat.probe_enqueues);	
	printf("probe dequeue: =%d\n", stat.probe_dequeues);


	printf("uart tx  pkts: =%d\n", stat.uart_tx_pkts);
	printf("uart tx   err: =%d\n", stat.uart_tx_err);
	printf("uart tx bytes: =%llu\n", stat.uart_tx_bytes);
	printf("uart rx  pkts: =%d\n", stat.uart_rx_pkts);	
	printf("uart rx bytes: =%llu\n", stat.uart_rx_bytes);	

	printf("sniff give err: =%d\n", stat.sniff_semGive_fail);	
	printf("sniff take err: =%d\n", stat.parse_semTake_fail);	
	
}

void dump_speed(void)
{
	PROBE_STATISTIC stat;
	memcpy((void*)&stat, (void*)&gWifiProbe.stat, sizeof(PROBE_STATISTIC));
	
	printf("cost        time: =%d(ms)\n", stat.speedCosttime);
	printf("cost peak   time: =%d(ms)\n", stat.maxspeedCosttime);

	printf("sniff      speed: =%d(kbps)\n", stat.sniffer_speed);
	printf("sniff peak speed: =%d(Kbps)\n", stat.max_sniffer_speed);
	
	printf("sniff      speed: =%d(pps)\n", stat.sniffer_pkts_per_sec);
	printf("sniff peak speed: =%d(pps)\n", stat.max_sniffer_pkts_per_sec);	
	
	printf("uart       speed: =%d(pps)\n", stat.uart_tx_pkts_per_sec);	
	printf("uart  peak speed: =%d(pps)\n", stat.max_uart_tx_pkts_per_sec);	

}

void probe_cleanup(int signo)
{
	AP_LOG(AP_LOG_NOTICE, LOG_MOUDLES_MAIN, "catch signal =%d\n", signo);
	pcap_cleanup();
	probe_resource_release();
	dump_probe_stat();
	exit(0);
}

void probe_calc_speed(struct	timeval *tv1, PROBE_STATISTIC * last_stat)
{
	struct	timeval    tv2;
	UINT32 sec, usec, msec;
	UINT32 delta_pkts;
	UINT64 delta_bytes;
	UINT32 uart_delta_pkts;
	
	gettimeofday(&tv2,NULL);
	sec = tv2.tv_sec - tv1->tv_sec;
	if(tv2.tv_usec >= tv1->tv_usec)
		usec = tv2.tv_usec - tv1->tv_usec;
	else
	{
		usec = tv2.tv_usec + 1000000 - tv1->tv_usec;
		sec -=1;
	}

	msec = sec * 1000 + usec / 1000;	/*base time.*/
	if(msec == 0)
		return;
	gWifiProbe.stat.maxspeedCosttime =COMMON_MAX(gWifiProbe.stat.maxspeedCosttime, msec);
	gWifiProbe.stat.speedCosttime = msec;

	delta_pkts = gWifiProbe.stat.all_packets -last_stat->all_packets;
	last_stat->all_packets = gWifiProbe.stat.all_packets;
	
	delta_bytes = gWifiProbe.stat.bytes -last_stat->bytes;
	last_stat->bytes = gWifiProbe.stat.bytes;
	
	uart_delta_pkts = gWifiProbe.stat.uart_tx_pkts-last_stat->uart_tx_pkts;
	last_stat->uart_tx_pkts = gWifiProbe.stat.uart_tx_pkts;

	gWifiProbe.stat.sniffer_speed = (delta_bytes * 8)/msec; // Kbps
	gWifiProbe.stat.max_sniffer_speed = COMMON_MAX(gWifiProbe.stat.max_sniffer_speed, gWifiProbe.stat.sniffer_speed);
	
	gWifiProbe.stat.sniffer_pkts_per_sec = (delta_pkts * 1000)/msec; // packets/per seconds.
	gWifiProbe.stat.max_sniffer_pkts_per_sec = COMMON_MAX(gWifiProbe.stat.max_sniffer_pkts_per_sec, \
							gWifiProbe.stat.sniffer_pkts_per_sec);
	
	gWifiProbe.stat.uart_tx_pkts_per_sec = (uart_delta_pkts * 1000)/msec; // packets/per seconds.
	gWifiProbe.stat.max_uart_tx_pkts_per_sec = COMMON_MAX(gWifiProbe.stat.max_uart_tx_pkts_per_sec, 	\
							gWifiProbe.stat.uart_tx_pkts_per_sec);
	
	
}

void *setsignal (int sig, void (*func)(int))
{
	struct sigaction old, new;
	memset(&new, 0, sizeof(new));
	new.sa_handler = func;
	if (sigaction(sig, &new, &old) < 0)
		return (SIG_ERR);
	return (old.sa_handler);
}

int main(int argc, char *argv[])
{

	int ch;
 	int ret;
    PPKT_HDR pktHrd;
	INT32 retcode= OSAL_SUCCESS;
	char *pcEnd = NULL;
	char *configFile = NULL;
	struct	timeval    tv1;
	PROBE_STATISTIC last_probe;
	
	wifiprobe_init();

	while ((ch = getopt(argc, argv,	"f:p:m:Pi:s:")) != -1)
	{
		switch (ch) 
		{
			case 'd':
				gWifiProbe.verbose ++;
				break;
				
			case 'f':
				configFile = optarg;
				break;
			case 'p':
                ret = snprintf(gWifiProbe.config.ctrl_intf, MAX_CTRL_INTF_LEN-1, "%s", optarg);               
				gWifiProbe.config.ctrl_intf[ret]='\0';
				break;
			case 'i':
				/* 确定抓包端口, 物理端口名称最长为16个字节 */
				strncpy(g_acSniffDevName, optarg, 15);
				g_acSniffDevName[15] = '\0';
				
				break;
			case 's':
				g_iSnapSize = strtol(optarg, &pcEnd, 0);
				
				if((optarg == pcEnd) || (*pcEnd != '\0'))
				{
					g_iSnapSize = 0;
				}
				
				break;
			case 'P':
				g_iPrintFlag++;
				break;
			default:
				usage();
		}

	}

	(void)setsignal(SIGPIPE, probe_cleanup);
	(void)setsignal(SIGTERM, probe_cleanup);
	(void)setsignal(SIGINT, probe_cleanup);
	(void)setsignal(SIGHUP, probe_cleanup);

	/*
		Init sequence.
	*/
	
	if(OSAL_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"OSAL_init Failure.\n");
		exit(1);
	}

	if(probe_resource_alloc() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"probe_resource_alloc Failure.\n");
		exit(1);
	}
	
	if(CM_ctrl_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"CM_ctrl_init Failure.\n");
		exit(1);	
	}

	if(CONFIG_init(configFile) != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"CONFIG_init Failure.\n");
		exit(1);	
	}

	if(SNIFF_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"SNIFF_init Failure.\n");
		exit(1);	
	}
	
	if(PARSE_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"PARSE_init Failure.\n");
		exit(1);	
	}
	
	if(UART_init() != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"UART_init Failure.\n");
		exit(1);	
	}
	

	/*CREATE TASKS.*/
	retcode = probe_tasks_start();
	if(retcode != STATUS_SUCCESS)
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_MAIN,"probe_tasks_start Failure.\n");
		exit(1);
	}

	
	memcpy(&last_probe, &gWifiProbe.stat, sizeof(PROBE_STATISTIC));
	while(1)
	{
		/**/
		gettimeofday(&tv1,NULL);
		OSAL_TaskDelay(1000);
		probe_calc_speed(&tv1, &last_probe);
	}

	return 0;
}
