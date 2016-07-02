#ifndef __SNMP_AGENT_LIB__
#define __SNMP_AGENT_LIB__
#include <pthread.h>

#ifdef __cplusplus
extern          "C" {
#endif

#define ZMSG_SNMP_BASE	 0x00012
#define ZMSG_SNMP_CONFIG_TYPE ZMSG_SNMP_BASE + 1
#define ZMSG_SNMP_SHUTDOWN_TYPE ZMSG_SNMP_BASE + 2
#define ZMSG_SNMP_CONFIG_INTERPROC_TYPE ZMSG_SNMP_BASE + 3
#define SNMP_ZEBRA_SOCKET     "zebra"


#ifndef MAX_SUBID               /* temporary - duplicate definition protection */
#ifndef EIGHTBIT_SUBIDS
typedef u_long  oid;
#define MAX_SUBID   0xFFFFFFFF
#else
typedef u_char  oid;
#define MAX_SUBID   0xFF
#endif
#endif

#define MAX_OID_LEN	    128 /* max subid's in an oid */

struct variable_list;
typedef struct variable_list netsnmp_variable_list;

typedef union {
   long           *integer;
   u_char         *string;
   oid            *objid;
   u_char         *bitstring;
   struct counter64 *counter64;
#ifdef OPAQUE_SPECIAL_TYPES
   float          *floatVal;
   double         *doubleVal;
   /*
    * t_union *unionVal; 
    */
#endif                          /* OPAQUE_SPECIAL_TYPES */
} netsnmp_vardata;

struct variable_list {
   /** NULL for last variable */
   struct variable_list *next_variable;    
   /** Object identifier of variable */
   oid            *name;   
   /** number of subid's in name */
   size_t          name_length;    
   /** ASN type of variable */
   u_char          type;   
   /** value of variable */
    netsnmp_vardata val;
   /** the length of the value to be copied into buf */
   size_t          val_len;
   /** 90 percentile < 24. */
   oid             name_loc[MAX_OID_LEN];  
   /** 90 percentile < 40. */
   u_char          buf[40];
   /** (Opaque) hook for additional data */
   void           *data;
   /** callback to free above */
   void            (*dataFreeHook)(void *);    
   int             index;
};

typedef struct snmp_info{
	int fd0;
	int fd1;
	int port;
	char syn_strflag[16];
}snmp_info;

typedef struct snmp_msg
{
	char token[32];
	char config_cptr[256];
	int option;/*0---add,1---del*/
	int rtn;
}snmp_msg;

#ifdef MOD_SNMP_VER_0_1
extern int snmp_main (void * para);
#else
extern int snmp_main (int, int, int,char *);
#endif
extern char * sagent_init(char * );

extern void sagent_handle_recv_ex();

extern void sagent_handle_recv(fd_set * fdset);

extern int sagent_snmp_select_info(int * numfds, fd_set * fdset);

extern void sagent_stop( char * );

extern void sagent_shutdown();

extern netsnmp_variable_list * sagent_varlist_add_variable(netsnmp_variable_list
                                                     ** varlist,
                                                     const oid * name,
                                                     size_t name_length,
                                                     u_char type,
                                                     const u_char * value,
                                                     size_t len);

extern   void   sagent_free_varbind(netsnmp_variable_list * var);     /* frees all in list */

extern void sagent_send_trap_vars(int trap, int specific, netsnmp_variable_list * vars);

extern void 	   sagent_log_enable();

extern void           sagent_log_disable();

extern int           sagent_log_get_enable();

#ifdef __cplusplus
}
#endif

#endif

