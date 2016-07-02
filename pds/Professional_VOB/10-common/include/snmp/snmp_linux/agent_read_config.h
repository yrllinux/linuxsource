/*
 * agent_read_config.h: reads configuration files for extensible sections.
 *
 */
#ifndef _AGENT_READ_CONFIG_H
#define _AGENT_READ_CONFIG_H

#ifdef __cplusplus
extern          "C" {
#endif

    void            init_agent_read_config(const char *);
    void            update_config(void);
    void            snmpd_register_config_handler(const char *,
                                                  void (*parser) (const
                                                                  char *,
                                                                  char *),
                                                  void (*releaser) (const char *,char *),
                                                  const char *);
    void            snmpd_unregister_config_handler(const char *);
    void            snmpd_store_config(const char *);
void 	snmpd_set_agent_packetsize(const char *token, char *param);
void 	snmpd_free_agent_packetsize(const char * token, char * param);
	

#ifdef __cplusplus
}
#endif
#endif                          /* _AGENT_READ_CONFIG_H */
