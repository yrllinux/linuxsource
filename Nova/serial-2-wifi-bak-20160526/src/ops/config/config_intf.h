#ifndef __CONFIG_INTF_H_
#define __CONFIG_INTF_H_

#include "common_types.h"

//#include "wifiprobe.h"
//#include "uart_msg.h"

//#define IMAGE_FILE 		"/tmp/upgrade_image.tar.gz"
//#define UPGRADE_FILE	"/usr/upgrade/upgrade_wifi_probe"

//typedef enum{
//	CONFIG_IDLE = 0,
//	CONFIG_RUN,
//	CONFIG_IMAGE,
//	CONFIG_END
//}CONFIG_STATE;

#define NET_STRING_MAX 16

typedef struct config_db{
	UINT8 netip_mode;
	UINT8 netip_ip[NET_STRING_MAX];
	UINT8 netip_mask[NET_STRING_MAX];
	UINT8 netip_dgw[NET_STRING_MAX];
	UINT8 netip_dns[NET_STRING_MAX];
	UINT8 netport_type;
	
	UINT32 serial_rate;
	UINT8 serial_db;
	UINT8 serial_sb;
	UINT8 serial_parity;
	
	UINT8 socket_mode;
	UINT8 socket_proto;
	UINT8 socket_cs;
	UINT8 socket_host[NET_STRING_MAX];
	UINT16 socket_cport;
	UINT16 socket_sport;
	UINT8 socket_timeout;	
}CONFIG_DB_T;

extern CONFIG_DB_T configDB;

#endif
