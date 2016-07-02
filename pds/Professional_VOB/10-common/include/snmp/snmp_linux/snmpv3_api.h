#ifndef NET_SNMP_SNMPV3_H
#define NET_SNMP_SNMPV3_H

    /**
     *  Library API routines concerned with SNMPv3 handling.
     *
     *  Most of these would typically not be used directly,
     *     but be invoked via version-independent API routines.
     */

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

#include "snmp_linux/callback.h"
#include "snmp_linux/snmpv3.h"
#include "snmp_linux/transform_oids.h"
#include "snmp_linux/keytools.h"
#include "snmp_linux/scapi.h"
#include "snmp_linux/lcd_time.h"
#ifdef USE_INTERNAL_MD5
#include "snmp_linux/md5.h"
#endif

#include "snmp_linux/snmp_secmod.h"
#include "snmp_linux/snmpv3-security-includes.h"

#endif                          /* NET_SNMP_SNMPV3_H */
