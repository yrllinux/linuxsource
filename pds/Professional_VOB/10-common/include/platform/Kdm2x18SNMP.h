/******************************************************************************
模块名  ：Kdm2x18Os
文件名	：Kdm2x18SNMP.h
相关文件：无
文件实现功能：KDM2x18 SNMP服务功能的编程接口
作者	：钮月忠
版本	：1.0
-------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人       走读人		修改内容
10/18/2005	1.0         钮月忠       向  飞     创建
******************************************************************************/

#ifndef _KDM2X18SNMP_H_
#define _KDM2X18SNMP_H_ 

#ifdef __cplusplus 
extern "C" { 
#endif

typedef int (* init_mibs_callback_t)(void);
typedef int (* snmp_config_callback_t)(void);

int snmp_start
    (
        init_mibs_callback_t init_mibs_callback,    /*加载用户实现的MIB模块的回调函数*/
        snmp_config_callback_t snmp_config_callback /*配置snmp的回调函数*/
    );

int snmp_config_comm_port_add
    (
        unsigned short port         /*AGENT 与NMS通信的端口号，1-65535*/
    );
    
int snmp_config_community_info_add
    (
        char *community_name,       /*共同体名，长度范围为1-32字节*/
        char *ipaddress ,           /*NMS的IP地址，应该是A.B.C.D/M格式*/
        char *view_name,            /*与共同体名相关联的视图名*/
        unsigned int access_type,   /*所关联视图的中管理变量的权限，0-----read 1-----write*/
        unsigned int acl_num        /*acl 规则号 <1-99>*/
    );
    
int snmp_config_view_info_add
    (
        char *view_name,            /*视图名，长度范围为1-32字节*/
        u_char included,            /*这个视图是否包含mib_name的子树 1----包含，0---不包含*/
        char *mib_name              /*视图所要包含的mib子树，长度范围为1-50字节*/
    );
    
int snmp_config_group_info_add
    (
        char *group_name,           /*组名，长度范围为1-32字节*/
        u_char ver_type,            /*snmp的版本号，0----v1,1-----v2c,2------v3 */
        u_char auth_type,           /*是否需要验证和加密0-验证，1--不需要验证，2-需要验证和加密*/
        char *read_view,            /*读权限的视图名，长度范围为1-32字节*/
        char *write_view,           /*写权限的视图名，长度范围为1-32字节*/
        char *notify_view,          /*通告的视图名，长度范围为1-32字节*/
        u_short acl_num             /*acl的规则号，1-99*/
    );
    
int snmp_config_user_info_add
    (
        char *user_name,            /*用户名或共同体名，长度范围为1-32字节*/
        u_char auth_type,           /*是否需要验证和加密0-验证，1--不需要验证，2-需要验证和加密*/
        u_char ver_type,            /*snmp的版本号，0----v1,1-----v2c,2------v3 */
        char *group_name,           /*与此用户名相关联的组，长度范围为1-32字节*/
        char *auth_paras,           /*验证参数，长度范围为1-32字节*/
        char *pri_paras,            /*加密参数，最大长度为32字节*/
        u_short acl_num             /*acl规则号 1-99*/
    );
    
int snmp_config_traphost_info_add
    (
        char *traphost_name,        /*trap报文目的地的IP地址应该为1.1.1.1格式的有效的IP地址*/
        char *trap_security_name,   /* trap报文的用户名/共同体,* 长度范围为1-32字节*/
        u_short  trap_port,         /* trap报文目的地的端口号1-65535*/
        u_char  trap_ver,           /* trap报文的版本0-v1,1-v2,3-v3*/
        u_char  trap_security_model /*是否需要验证和加密0-验证，1--不需要验证，2-需要验证和加密*/
    );
    
int snmp_config_community_info_del
    (
        char *community_name        /*共同体名，长度范围为1-32字节*/
    );
    
int snmp_config_view_info_del
    (
        char *view_name             /*视图名，长度范围为1-32字节*/
    );
    
int snmp_config_group_info_del
    (
        char *group_name,           /*组名，长度范围为1-32字节*/
        u_char ver_type             /*snmp的版本号，0----v1,1-----v2c,2------v3 */
    );
    
int snmp_config_user_info_del
    (
        char *user_name             /*用户名或共同体名，长度范围为1-32字节*/
    );
    
int snmp_config_traphost_info_del
    (
        char *traphost_name,        /*trap报文目的地的IP地址为1.1.1.1格式的有效的IP地址*/
        int trap_port               /* trap报文目的地的端口号<1-65535>*/
    );

#ifdef __cplusplus 
} 
#endif

#endif /*_KDM2X18SNMP_H_*/
