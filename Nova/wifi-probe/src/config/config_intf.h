#ifndef __CONFIG_INTF_H_
#define __CONFIG_INTF_H_

#include "common_types.h"

#include "wifiprobe.h"
#include "uart_msg.h"


typedef enum{
	CONFIG_IDLE = 0,
	CONFIG_RUN,
	CONFIG_IMAGE,
	CONFIG_END
}CONFIG_STATE;

typedef struct config_db{
	MUX_ID dblock;
	char hwversion[8];
	char swversion[8];
	UINT16 scan_is_auto;
	UINT16 current_ch;
	UINT32 scan_ch_mask;
	UINT32 auto_scan_ch_mask;	
	UINT32 channel_caps_mask;
	UINT16 ch_switch_time;
	UINT16 report_time_per_mac;
	UINT32 settingchange;
	CONFIG_STATE state;
	MSG_FUNCS *msg_funcs;
	
}CONFIG_DB_T;

extern CONFIG_DB_T configDB;

#endif
