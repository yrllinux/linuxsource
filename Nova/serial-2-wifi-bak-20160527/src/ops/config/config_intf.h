#ifndef __CONFIG_INTF_H_
#define __CONFIG_INTF_H_

#include "common_types.h"

typedef struct config_cmd{
	char * name;
	void (*f)(char *);
}CONFIG_CMD;

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
