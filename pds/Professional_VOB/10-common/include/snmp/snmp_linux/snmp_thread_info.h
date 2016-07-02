#ifndef __SNMP_THREAD_INFO__
#define __SNMP_THREAD_INFO__
#include "snmp_linux/net-snmp-config.h"
#include "snmp_linux/net-snmp-includes.h"
#include "snmp_linux/net-snmp-agent-includes.h"
#include "protocol.h"
#include "snmp_linux/snmp_api.h"
#include "snmp_linux/callback.h"
#include "snmp_linux/snmp_agent.h"
#include "snmp_linux/snmpudpdomain.h"
#include "snmpudpipv6domain.h"
#include "snmp_linux/snmpunixdomain.h"
#include "snmp_linux/read_config.h"
#include "snmp_linux/var_struct.h"
#include "snmp_linux/agent_registry.h"
#include "snmp_linux/snmp_debug.h"
#include "snmp_linux/parse.h"
#include "mib_modules.h"
#include "snmp_linux/snmp_logging.h"      /* For this file's "internal" definitions */
#include "agent_index.h"
#include "snmp_linux/mib.h"
#include "snmp_linux/snmp_transport.h"
#include "snmp_linux/snmpudpdomain.h"
#include "snmp_linux/snmptcpdomain.h"
#include "snmp_linux/vacm.h"
#include "snmp_linux/lcd_time.h"
#include "snmp_linux/callback.h"
#include "snmp_linux/default_store.h"    /* for "internal" definitions */
#include "snmp_linux/snmp_secmod.h"
#include "des_locl.h"

//#include "snmp/snmp_enum.h"
//#include "snmp/snmpcallbackdomain.h"
//#include "autonlist.h"
  
typedef struct snmp_thread_infos{
	/*thread info data*/
	char * pmib_defdir;
	snmp_info *psnmp_info;
	netsnmp_agent_session  *agent_delegated_list;
	netsnmp_session *agentx_callback_sess;
	char * app_name;
	int callback_master_num;
	netsnmp_session * callback_master_sess;
	com2Sec6Entry  *com2Sec6List;
	com2Sec6Entry *com2Sec6ListLast;
	com2SecEntry   *com2SecList;
	com2SecUnixEntry   *com2SecUnixList;
	com2SecUnixEntry *com2SecUnixListLast;
	struct config_files *config_files;
	subtree_context_cache *context_subtrees;
	const char     *curfilename;
	unsigned int    current_maj_num;
	unsigned int    current_min_num;	
	int      dodebug ;
	int             debug_num_tokens;
	int      debug_print_everything;
	netsnmp_token_descr dbg_tokens[MAX_DEBUG_TOKENS];
	/*
	 * indent debugging:  provide a space padded section to return an indent for 
	 */
	int      debugindent;

	char     * debugindentchars;
	int des_check_key;
	int des_rw_mode;
	int             opterr ;     /* if error message should be printed */
	int             optind;     /* index into parent argv vector */
	int             optopt;         /* character checked for validity */
	int             optreset;       /* reset getopt */
	char           *optarg;         /* argument associated with option */
	char           EMSG[256];
	int	 external_readfd[NUM_EXTERNAL_FDS];
	int    external_readfdlen;
	int	external_writefd[NUM_EXTERNAL_FDS];
	int    external_writefdlen;
	int	external_exceptfd[NUM_EXTERNAL_FDS];
	int    external_exceptfdlen;
	void  (*external_readfdfunc[NUM_EXTERNAL_FDS]) (int, void *);
	void  (*external_writefdfunc[NUM_EXTERNAL_FDS]) (int, void *);
	void  (*external_exceptfdfunc[NUM_EXTERNAL_FDS]) (int, void *);
	void   *external_readfd_data[NUM_EXTERNAL_FDS];
	void   *external_writefd_data[NUM_EXTERNAL_FDS];
	void   *external_exceptfd_data[NUM_EXTERNAL_FDS];
	int             Facility;

	/*parase.c global var*/
	struct tc  tclist[MAXTC];
	int             mibLine;
	const char     *File;
	int      anonymous;
	struct objgroup *objgroups;
	struct objgroup *objects;
	struct objgroup *notifs;
	struct tok tokens[TOK_NUM];
	struct module_compatability *module_map_head;
	struct module_compatability module_map[MODULE_MAP_NUM];
	struct tok *buckets[HASHSIZE];

	struct node *nbuckets[NHASHSIZE];
	struct tree *tbuckets[NHASHSIZE];
	struct module *module_head;

	struct node    *orphan_nodes;
	struct tree    *tree_head;
	struct module_import root_imports[NUMBER_OF_ROOT_NODES];

	int      current_module;
	int      max_module;
	char    *last_err_module;    /* no repeats on "Cannot find module..." */
	int      erroneousMibs;
 	int      translation_table[256];
	char     leave_indent[256];
	int      leave_was_simple;
	struct module_init_list *initlist;
	struct module_init_list *noinitlist;
	int linecount;
	int             log_addresses;
	struct addrCache addrCache[SNMP_ADDRCACHE_SIZE];
	/*snmp log global*/
#ifndef MOD_SNMP_ZEBRA_0_2
	netsnmp_log_handler *logh_head;
	netsnmp_log_handler *logh_priorities[LOG_DEBUG+1];

	int      newline;	 /* MTCRITICAL_RESOURCE */
#else
	int     snmp_log_enable; /*1----enable, 0----disable*/
#endif
	char syslogname[64];
	int lookup_cache_size;
       lookup_cache_context *thecontextcache;	
	netsnmp_session *main_session;	   
	struct snmp_index *snmp_index_head;
	unsigned int max_packet_len;
	int mib_file_total_len;
       mib_node * p_mibroot;
	PrefixList      mib_prefixes[MIB_PREFIXES_NUM];

	char  *   Standard_Prefix;

	/*
	 * Set default here as some uses of read_objid require valid pointer. 
	 */
	char    *Prefix; 
	netsnmp_tdomain tcpDomain;

       netsnmp_tdomain tcp6Domain;


	netsnmp_tdomain udp6Domain;
	

	netsnmp_tdomain unixDomain;
	
	struct autonlist *nlists;


	struct session_list *Sessions;   /* MT_LIB_SESSION */
	long     Reqid;      /* MT_LIB_REQUESTID */
	long     Msgid;      /* MT_LIB_MESSAGEID */
	long     Sessid;     /* MT_LIB_SESSIONID */
	long     Transid;    /* MT_LIB_TRANSID */
	int             snmp_errno;
	/*
	 * END MTCRITICAL_RESOURCE
	 */

	/*
	 * global error detail storage
	 */
	char     snmp_detail[192];
	int      snmp_detail_f;
       u_int    statistics[MAX_STATS];

	int sagent_shutdown_fd;
	char  shutdown_id[32];
	
	struct trap_sink *sinks;

	struct snmp_enum_list_str *sliststorage;
	int             snmp_enableauthentraps;
	int             snmp_enableauthentrapsset;
	char * snmp_trapcommunity;

	callback_queue *thequeue;


	oid             usmAESPrivProtocol[10];
	/* backwards compat */
	oid             *usmAES128PrivProtocol;	

	oid             usmNoAuthProtocol[10];
#ifndef DISABLE_MD5
	oid             usmHMACMD5AuthProtocol[10];
#endif
	oid             usmHMACSHA1AuthProtocol[10];
	oid             usmNoPrivProtocol[10];
#ifndef DISABLE_DES
	oid             usmDESPrivProtocol[10];
#endif
	struct timeval  starttime;
	struct tree    *Mib;            /* Backwards compatibility */
       struct tree *tree_top;
	char    *confmibdir;
	char    *confmibs;
	agent_nsap *agent_nsap_list;
	netsnmp_agent_session *agent_session_list;
	netsnmp_agent_session *netsnmp_processing_set;
	netsnmp_agent_session *netsnmp_agent_queued_list;
	oid             nullOid[10];
	int             nullOidLen;
       struct snmp_enum_list ***snmp_enum_lists;
	netsnmp_tdomain ipxDomain;
	netsnmp_tdomain udpDomain;

	u_long   engineBoots;
	unsigned int engineIDType;
	unsigned char *engineID;
	size_t   engineIDLength;
	unsigned char *engineIDNic;
	unsigned int engineIDIsSet;  /* flag if ID set by config */
	unsigned char *oldEngineID;
	size_t   oldEngineIDLength;
	struct timeval snmpv3starttime;

	/*
	 * Set up default snmpv3 parameter value storage.
	 */
	const oid *defaultAuthType;
	size_t   defaultAuthTypeLen;
	const oid *defaultPrivType;
	size_t   defaultPrivTypeLen;

	clock_t snmpv3startClock;
	long clockticks;
	unsigned int lastcalltime;
	unsigned int wrapcounter;

	struct vacm_viewEntry *viewList ;
	struct vacm_viewEntry *viewScanPtr;
	struct vacm_accessEntry *accessList ;
	struct vacm_accessEntry*accessScanPtr;
	struct vacm_groupEntry *groupList;
	struct vacm_groupEntry *groupScanPtr;
	struct agent_netsnmp_set_info *Sets;

	netsnmp_transport_list *trlist;
	int      callback_count;

	u_int    salt_integer;
	        /*
	         * 1/2 of seed for the salt.   Cf. RFC2274, Sect 8.1.1.1.
	         */

	struct usmUser *noNameUser;
	/*
	 * Local storage (LCD) of the default user list.
	 */
	struct usmUser *userList;
       Enginetime etimelist[ETIMELIST_SIZE];	
 	char     done_init_agent;

	int      traptype;

       struct snmp_gen_callback
               *thecallbacks[MAX_CALLBACK_IDS][MAX_CALLBACK_SUBIDS];
	int _lock;

	struct snmp_alarm *thealarms;
	int      start_alarms;
	unsigned int regnum;

	int             swap;
	int mem; 
	int kmem;

	netsnmp_ds_read_config *netsnmp_ds_configs;
	char * stores [NETSNMP_DS_MAX_IDS];
	int   netsnmp_ds_integers[NETSNMP_DS_MAX_IDS][NETSNMP_DS_MAX_SUBIDS];
	char  netsnmp_ds_booleans[NETSNMP_DS_MAX_IDS][NETSNMP_DS_MAX_SUBIDS/8];
	char *netsnmp_ds_strings[NETSNMP_DS_MAX_IDS][NETSNMP_DS_MAX_SUBIDS];
	void *netsnmp_ds_voids[NETSNMP_DS_MAX_IDS][NETSNMP_DS_MAX_SUBIDS];

       struct snmp_secmod_list *registered_services;
	netsnmp_data_list *auto_tables;	   

	 netsnmp_cache  *cache_head;
	 int             cache_outstanding_valid;
        int      config_errors;

        netsnmp_container *containers;
	 netsnmp_agent_session *current_agent_session;

	int current_globalid;

      netsnmp_tdomain *domain_list;

	int      doneit;
	netsnmp_data_list *handler_reg;
       struct read_config_memory *memorylist;
       netsnmp_data_list *saveHead;
       agent_set_cache * cache_Sets;
	char     have_done_init;
	int      done_init;      /* To prevent double init's. */
       int seed;
       des_cblock init;
}snmp_thread_infos;

#endif

	
