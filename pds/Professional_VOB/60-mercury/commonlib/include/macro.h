#ifndef _COMMONLIB_MACRO_H_
#define _COMMONLIB_MACRO_H_

#ifdef _LINUX_
#define ROUTE_CFGFILE       "/proc/net/route"
#define NETWORK_CFGFILE     "/etc/sysconfig/network"
#define NETIF_CFGFILE_DIR   "/etc/sysconfig/network-scripts/"  
#define RC_LOCAL            "/etc/rc.d/rc.local"

#define GW_KEY      (LPCSTR)"GATEWAY"
#define IP_KEY      (LPCSTR)"IPADDR"
#define HWADDR_KEY  (LPCSTR)"HWADDR"
#define DEVICE_KEY  (LPCSTR)"DEVICE"
#define NETMAS_KEY  (LPCSTR)"NETMASK"
#define TYPE_KEY    (LPCSTR)"Ethernet"
#define ONBOOT_KEY  (LPCSTR)"ONBOOT"
#define BOOTPR_KEY  (LPCSTR)"BOOTPROTO"
#endif // _LINUX_

#endif
