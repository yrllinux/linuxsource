#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ap_log.h"

char *log_moudle_name(unsigned int iLogChanMask)
{
	switch(iLogChanMask)
	{	
		case LOG_MOUDLES_MAIN:
			return LOG_MOUDLES_MAIN_NAME;
		case LOG_MOUDLES_SNIFF:
			return LOG_MOUDLES_SNIFF_NAME;
		case LOG_MOUDLES_PARSE:
			return LOG_MOUDLES_PARSE_NAME;
		case LOG_MOUDLES_CONFIG:
			return LOG_MOUDLES_CONFIG_NAME;
		case LOG_MOUDLES_OSAL:
			return LOG_MOUDLES_OSAL_NAME;
		case LOG_MOUDLES_UART:
			return LOG_MOUDLES_UART_NAME;
		default:
			return LOG_MOUDLES_UNKNOW_NAME;
	}
	return LOG_MOUDLES_UNKNOW_NAME;
}

