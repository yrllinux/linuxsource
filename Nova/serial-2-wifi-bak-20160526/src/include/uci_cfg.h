#ifndef _UCI_INTERFACE_H__
#define _UCI_INTERFACE_H__

#include <uci.h>

#include "common_types.h"

#define FC_SUCCESS 0
#define FC_ERROR 1

static struct uci_package *config_init_package ( const char *config );
INT8 uci_update_cfg ( char *option, char *value );
INT8 uci_del_cfg ( char *option );
INT8 get_uci_config ( char *option, char *value, int len );

#endif
