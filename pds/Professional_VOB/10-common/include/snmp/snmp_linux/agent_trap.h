#ifndef AGENT_TRAP_H
#define AGENT_TRAP_H
#include "snmp_linux/snmp_module.h"

#ifdef __cplusplus
extern          "C" {
#endif

struct agent_add_trap_args {
    netsnmp_session *ss;
    int             confirm;
};

#ifdef MOD_SNMP_VER_0_1
#define SNMP_AUTHENTICATED_TRAPS_ENABLED	1
#define SNMP_AUTHENTICATED_TRAPS_DISABLED	2
#define SNMPV2_TRAP_OBJS_PREFIX	SNMP_OID_SNMPMODULES,1,1,4
#define SNMPV2_TRAPS_PREFIX	SNMP_OID_SNMPMODULES,1,1,5

struct trap_sink {
    netsnmp_session *sesp;
    struct trap_sink *next;
    int             pdutype;
    int             version;
};
#endif

void            init_traps(void);
void            send_easy_trap(int, int);
void            send_trap_pdu(netsnmp_pdu *);
void            send_v2trap(netsnmp_variable_list *);
void            send_trap_vars(int, int, netsnmp_variable_list *);
void            send_enterprise_trap_vars(int trap, int specific,
                                          oid * enterprise,
                                          int enterprise_length,
                                          netsnmp_variable_list * vars);
int             netsnmp_send_traps(int trap, int specific,
                          oid * enterprise, int enterprise_length,
                          netsnmp_variable_list * vars,
                          /* These next two are currently unused */
                          char * context, int flags);
void            snmpd_parse_config_authtrap(const char *, char *);
void            snmpd_parse_config_trapsink(const char *, char *);
void            snmpd_parse_config_trap2sink(const char *, char *);
void            snmpd_parse_config_informsink(const char *, char *);
void            snmpd_parse_config_trapsess(const char *, char *);
void            snmpd_free_trapsinks(void);
void 	snmpd_free_trapsink_node(const char *token, char *param);

void            snmpd_parse_config_trapcommunity(const char *, char *);
void            snmpd_free_trapcommunity(const char *, char *);
void            send_trap_to_sess(netsnmp_session * sess,
                                  netsnmp_pdu *template_pdu);

int             create_trap_session(char *, u_short, char *, int, int);
int             add_trap_session(netsnmp_session *, int, int, int);
int             remove_trap_session(netsnmp_session *);

void                   convert_v2_to_v1(netsnmp_variable_list *, netsnmp_pdu *);
netsnmp_variable_list *convert_v1_to_v2(netsnmp_pdu *);
netsnmp_pdu    *convert_v2pdu_to_v1(netsnmp_pdu *);
netsnmp_pdu    *convert_v1pdu_to_v2(netsnmp_pdu *);

#ifdef __cplusplus
}
#endif
#endif                          /* AGENT_TRAP_H */
