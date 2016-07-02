#ifndef NET_SNMP_UTILITIES_H
#define NET_SNMP_UTILITIES_H

    /**
     *  Library API routines not specifically concerned with SNMP directly,
     *    but used more generally within the library, agent and other applications.
     *
     *  This also includes "standard" system routines, which are missing on
     *    particular O/S distributiones.
     */

#ifndef NET_SNMP_CONFIG_H
#error "Please include <net-snmp/net-snmp-config.h> before this file"
#endif

#include "snmp_linux/types.h"

    /*
     *  For the initial release, this will just refer to the
     *  relevant UCD header files.
     *    In due course, the routines relevant to this area of the
     *  API will be identified, and listed here directly.
     *
     *  But for the time being, this header file is a placeholder,
     *  to allow application writers to adopt the new header file names.
     */

#include "snmp_linux/snmp_api.h"
#include "snmp_linux/snmp_client.h"
#if HAVE_GETOPT_H
#include <lib/getopt.h>
#else
#include "lib/getopt.h"
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>         /* for in_addr_t */
#endif
#include "snmp_linux/system.h"
#include "snmp_linux/tools.h"
#include "snmp_linux/asn1.h"    /* for counter64 */
#include "snmp_linux/int64.h"

#include "snmp_linux/mt_support.h"
/*  #include <net-snmp/library/snmp_locking.h>  */
#include "snmp_linux/snmp_alarm.h"
#include "snmp_linux/callback.h"
#include "snmp_linux/data_list.h"
#include "snmp_linux/oid_stash.h"
#include "snmp_linux/snmp.h" 
#include "snmp_linux/snmp_impl.h"
#include "snmp_linux/snmp-tc.h"
#include "snmp_linux/check_varbind.h"
#include "snmp_linux/container.h"
#include "snmp_linux/container_binary_array.h"
#include "snmp_linux/container_list_ssll.h"
#include "snmp_linux/container_iterator.h"

#include "snmp_linux/snmp_assert.h"

#include "snmp_linux/snmp_version.h"

#endif                          /* NET_SNMP_UTILITIES_H */
