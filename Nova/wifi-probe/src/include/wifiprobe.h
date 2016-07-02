#ifndef _WIFI_PROBE_H
#define _WIFI_PROBE_H

#include "common_types.h"

/*TASK list.*/

typedef enum{


	TASK_UART_TX_PRIO = 10,
	TASK_PARSE_PRIO,
	TASK_PCAP_PRIO,
	TASK_CONFIG_PRIO,
	TASK_UART_RX_PRIO ,
	TASK_CHANNAL_PRIO ,
	TASK_TIMER_PRIO ,
	
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

#define DEFAULT_CTRL_INTF_NAME  "/var/run/ctrl_intf"
#define MAX_CTRL_INTF_LEN   32
#define MAX_QQ_PORTS 		10

#define DEFAULT_TTY_NAME		"/dev/ttyS0"
#define MAX_TTY_NAME_LEN		16
#define MAX_FIXED_MAC_NUMS		5
/*配置结构*/
#define DEFAULT_SCAN_CH		0x1FFF	/*1-13*/
#define DEFAULT_CHANNEL_COUNT	13
#define DEFAULT_CH_SW_TIME	10		/*SECONDS*/
#define DEFAULT_REPORT_TIME	10		/*SECONDS*/
#define CONFIG_AUTO_CH_MASK	0xFFFFFFFF


typedef struct _mac_probe{
	unsigned char mac[6];
}MAC_PROBE_INFO;

typedef struct _probe_config{
    char ctrl_intf[MAX_CTRL_INTF_LEN];
	char tty_name[MAX_TTY_NAME_LEN];
	MAC_PROBE_INFO probe_fixed_mac[MAX_FIXED_MAC_NUMS];	/*测试用，获取指定MAC的信息，其他不上报*/
	UINT32 probe_fixed_mac_nums;
	UINT32  chn_scan_time;
	UINT32  chn_mask;
	UINT32  report_time;	/*同一个STA多长时间报一次*/
	UINT16  qqport_nums;
	UINT16  qqport[MAX_QQ_PORTS];
	
}PROBE_CONFIG_T;

/*统计结构*/
typedef struct _probe_stat{
	UINT32 all_packets;
	UINT32 good_packets;
	UINT32 large_packets;
	UINT64 bytes;

	/*wlan*/
	UINT32 ctrl80211;
	UINT32 mng80211;
	UINT32 data80211;
	UINT32 other80211;
	
	UINT32 null80211;
	UINT32 encry80211;

	/*TCP/IP*/
	UINT32 ip4;
	UINT32 ip6;
	UINT32 proto_others;	
	UINT32 udp;	
	UINT32 tcp;
	UINT32 ipproto_others;	
	
	UINT32 http80;
	UINT32 http_data;

	/*pkt buff*/
	UINT32 no_pkt_buff;
	UINT32 pkt_enqueues;
	UINT32 pkt_dequeues;

	/*probe buff*/
	UINT32 good_probe_info;
	UINT32 no_probe_buff;
	UINT32 probe_enqueues;
	UINT32 probe_dequeues;

	/*uart*/
	UINT32 uart_tx_pkts;
	UINT32 uart_tx_err;
	UINT64 uart_tx_bytes;
	UINT32 uart_rx_pkts;
	UINT64 uart_rx_bytes;

	/*sniff module*/
	UINT32 sniff_semGive_fail;
	UINT32 parse_semTake_fail;

	/*speed .*/
	UINT32 speedCosttime;		/*多长时间来测一下速率*/
	UINT32 maxspeedCosttime;		/*多长时间来测一下速率*/
	
	UINT32 sniffer_speed;		/*抓包的速率Kbit/perseconds*/
	UINT32 max_sniffer_speed;

	UINT32 sniffer_pkts_per_sec;/*抓包每秒钟多少个报文*/
	UINT32 max_sniffer_pkts_per_sec;

	UINT32 uart_tx_pkts_per_sec; /*串口每秒钟多少个报文*/
	UINT32 max_uart_tx_pkts_per_sec;
	
}PROBE_STATISTIC;


#define MAC_FILTER_SLOT_MAX 	2000
#define MAC_NUMS_PER_SLOT		3

typedef struct _filter_mac{
	UINT32 timestamp;
	UINT8 mac[6];
}FILTER_MAC;

typedef struct _filter_mac_slot{
	UINT32 hit;
	FILTER_MAC macfilter[MAC_NUMS_PER_SLOT];
}FILTER_MAC_SlOT;

typedef struct _filter_info{
	UINT32 hit;	/*冲突的总次数*/
	UINT32 time;	/*过滤设定的时间*/
	FILTER_MAC_SlOT filter_slot[MAC_FILTER_SLOT_MAX];
	
}PROBE_FILTER_INFO;

/*全局的数据结构*/
typedef struct _wifi_probe{
	UINT32  verbose;
	SEM_ID	semDeviceSync;
	PROBE_CONFIG_T config;
	PROBE_FILTER_INFO probefilter;	
	PROBE_STATISTIC stat;
}WIFI_PROBE_T;

extern WIFI_PROBE_T gWifiProbe;

#define DEBUG_CTRL_INTF	gWifiProbe.config.ctrl_intf

#define STATISTIC_PROBE_INC(field)	\
do{	\
	gWifiProbe.stat.field+=1;	\
}while(0)

#define STATISTIC_PROBE_INC_VAL(field, val)	\
do{	\
	gWifiProbe.stat.field+=val;	\
}while(0)


#endif
