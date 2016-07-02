#ifndef ALL_HANDLERS_H
#define ALL_HANDLERS_H

#ifdef __cplusplus
extern          "C" {
#endif

#include "snmp_linux/instance.h"
#include "snmp_linux/baby_steps.h"
#include "snmp_linux/scalar.h"
#include "snmp_linux/scalar_group.h"
#include "snmp_linux/watcher.h"
#include "snmp_linux/multiplexer.h"
#include "snmp_linux/null.h"
#include "snmp_linux/debug_handler.h"
#include "snmp_linux/cache_handler.h"
#include "snmp_linux/old_api.h"
#include "snmp_linux/read_only.h"
#include "snmp_linux/row_merge.h"
#include "snmp_linux/serialize.h"
#include "snmp_linux/bulk_to_next.h"
#include "snmp_linux/mode_end_call.h"
/*
 * #include <net-snmp/agent/set_helper.h> 
 */
#include "snmp_linux/snmp_table.h"
#include "snmp_linux/table_data.h"
#include "snmp_linux/table_dataset.h"
#include "snmp_linux/table_iterator.h"
#include "snmp_linux/table_container.h"
/*
 * #include <net-snmp/agent/table_array.h> 
 */

#include "snmp_linux/mfd.h"


void            netsnmp_init_helpers(void);

#ifdef __cplusplus
}
#endif
#endif                          /* ALL_HANDLERS_H */
