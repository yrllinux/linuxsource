#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ap_log.h"

char *log_moudle_name(unsigned int iLogChanMask)
{
	switch(iLogChanMask)
	{	
		case LOG_MOUDLES_CORE:
			return LOG_MOUDLES_CORE_NAME;
		case LOG_MOUDLES_NETWORK:
			return LOG_MOUDLES_NETWORK_NAME;
		case LOG_MOUDLES_SERIAL:
			return LOG_MOUDLES_SERIAL_NAME;
		case LOG_MOUDLES_OPS:
			return LOG_MOUDLES_OPS_NAME;
		case LOG_MOUDLES_OSAL:
			return LOG_MOUDLES_OSAL_NAME;
		default:
			return LOG_MOUDLES_UNKNOW_NAME;
	}
	return LOG_MOUDLES_UNKNOW_NAME;
}

