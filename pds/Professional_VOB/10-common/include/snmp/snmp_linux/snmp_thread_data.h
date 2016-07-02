
#ifndef __SNMP_THREAD_DATA__
#define __SNMP_THREAD_DATA__

#include <pthread.h>
#include "snmp_linux/snmp_module.h"
#ifdef MOD_SNMP_VER_0_1
extern pthread_key_t snmp_key;

extern int snmp_pthread_key_create();
extern int snmp_pthread_setspecific(const void *value);
extern void *snmp_pthread_getspecific();
extern  int snmp_pthread_key_delete();
#endif
#endif

