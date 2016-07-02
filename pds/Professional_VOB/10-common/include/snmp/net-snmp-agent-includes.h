/*
 * A simple helpful wrapper to include lots of agent specific include
 * * files for people wanting to embed and snmp agent into an external
 * * application
 */
#ifndef NET_SNMP_AGENT_INCLUDES_H
#define NET_SNMP_AGENT_INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MOD_SNMP_VER_0
#define MOD_SNMP_VER_0
#endif
#ifndef MOD_SNMP_VER_0_1
#define MOD_SNMP_VER_0_1
#endif

/* define for memory lib */
#define _XMALLOC(mtype,size)         malloc(size)
#define _XCALLOC(mtype,size)         calloc(1,size)
#define _XREALLOC(mtype,ptr,size)    realloc(ptr,size)
#define _XFREE(mtype,ptr)            free(ptr)
#define _XSTRDUP(mtype,str)          strdup(str)

#include <string.h>

#include "snmp_linux/net-snmp-config.h"
#include "snmp_linux/types.h"
#include "snmp_linux/snmp_api.h"
#include "snmp_linux/snmp_client.h"
#include "snmp_linux/snmp_logging.h"
#include "snmp_linux/snmp_agent.h"
#include "snmp_linux/snmp_agent_lib.h"
#include "snmp_linux/snmp_debug.h"
#include "snmp_linux/agent_handler.h"
#include "snmp_linux/scalar.h"
#include "snmp_linux/snmp_impl.h"
#include "snmp_linux/callback.h"
#include "snmp_linux/snmp_transport.h"
#include "snmp_linux/tools.h"
#include "snmp_linux/data_list.h"
#include "snmp_linux/snmp_module.h"
#include "snmp_linux/snmp_vars.h"
#include "snmp_linux/ds_agent.h"
#include "snmp_linux/agent_read_config.h"
#include "snmp_linux/agent_trap.h"
#include "snmp_linux/container.h"
#include "snmp_linux/oid_stash.h"
#include "snmp_linux/all_helpers.h"
#include "snmp_linux/var_struct.h"
#include "snmp_linux/snmp_thread_data.h"

#include "snmp_linux/mib_module_config.h"
#include "snmp_linux/agent_module_config.h"
#include "snmp_linux/snmp_alarm.h"
#include "snmp_linux/snmp_assert.h"
#include "snmp_linux/mib.h"
#include "snmp_linux/check_varbind.h"
#include "snmp_linux/mib_modules.h"
#include "snmp_linux/table_container.h"
#include "snmp_linux/asn1.h"

#ifdef __cplusplus
}
#endif

#endif /* NET_SNMP_AGENT_INCLUDES_H */
