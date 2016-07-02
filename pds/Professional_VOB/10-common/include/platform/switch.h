/////////////////////////////////////////////////////////////////////////////////
//  模块名	：DataSwitch                        
//  文件名	：switch.h
//  功能        ：NIP模块提供的一些函数的声明
//  版本	：
//-----------------------------------------------------------------------------
//  修改记录:
//  日  期		版本		修改人		走读人    修改内容
/////////////////////////////////////////////////////////////////////////////////

#ifndef _SWITCH_H_
#define _SWITCH_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*VOIDFUNCPTR)();

#if defined(_VXWORKS_)

void vc_mod_switch_reg(const VOIDFUNCPTR func);
void vc_mod_switch_unreg();
void vc_switch_set_minpt();
int  vc_switch_get_minpt();
long if2_ip2index( unsigned long ip );
void vc_init();

extern int _vc_enable;
extern int _vc_switch;
extern int __vc_debug;
extern int __vc_error;

#elif defined(_LINUX_)

#define IP_DSW_ADD_PORT 0
#define IP_DSW_DEL_PORT 1
#define IP_DSW_CLR_PORT 2

#define IP_DSW_REG   0
#define IP_DSW_UNREG 1

typedef char* (*DSW_BCALLFUNC)( char* pchBuffer, unsigned short wRecvDataLen , unsigned short* pwSendNum );

int ip_dsw_enable(unsigned short wDataLen, DSW_BCALLFUNC func);
int ip_dsw_disable();
int ip_dsw_set_port(int cmd, unsigned short port);
int ip_dsw_core();
int ip_dsw_cleanup();
int ip_dsw_init();

int ip_dsw_get_index(unsigned long ip);

#endif

#ifdef __cplusplus
}
#endif

#endif/* _SWITCH_H_ */


