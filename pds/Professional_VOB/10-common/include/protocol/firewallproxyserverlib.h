/****************************************************************************
  模块名   ：穿越防火墙&NAT代理Server端                        
  文件名   ：FirewallProxyServerLib.h
  相关文件 ：FirewallProxyServerLib.cpp
  文件功能 ：定义穿越防火墙&NAT
             代理服务端Lib的使用头文件
  作者	   ：Tony
  版本	   ：1.0
----------------------------------------------------------------------------
  修改记录:
  日  期		版本		修改人		走读人    修改内容
  2004/12/09    1.0         
******************************************************************************/
#ifndef FIREWALLPROXYSERVERLIB_H_
#define FIREWALLPROXYSERVERLIB_H_

#include "osp.h"

//#define UNIT_TEST
extern u16     UNIT_TEST_TYPE;

//add by gzj.070815 
#define VER_PXYS (const char *)"PxyServer40.00.01.05.130124(CBB)"

// add by gxb 2011-11-24
#define MAX_VIRTUAL_IP_COUNT  (u16)10

//Proxy Server配置信息
struct TProxyServerCfg
{
	BOOL32 m_bStartPxy;
    BOOL32 m_bStartMp;
    u32 m_dwGKIpAddr;        //GK的IP地址
	u16 m_wGKRasPort;        //GK的Ras端口
	u32 m_dwSrvIpAddr;       //PROXY SERVER使用的IP地址
	u16 m_wSrvRasPort;       //PROXY SERVER的信令端口
	u16 m_wSrvH245Port;
	u16 m_wOspTcpPort;
    u16 m_wRtpPort;          //RTP端口
    u16 m_wRtcpPort;         //RTCP端口
    u16 m_wMediaBasePort;    //码流起始端口地址
	u32 m_dwSrvDmzIpAddr;	 //代理服务器DMZ映射地址
	BOOL m_bUseDmzAddr;		 //是否启用代理服务器DMZ映射
	BOOL32	m_b323UseSpecialIP; //323指定使用m_dwSrvIpAddr的地址
	u16 m_wVirtualIPNum;                         //虚拟IP的数量
	u32 m_dwVirtualIPAddr[MAX_VIRTUAL_IP_COUNT]; //虚拟IP的地址
	BOOL32 m_bIsForceGKRoute;                    //是否使用GK路由 2013/05/09
};

extern u16 InitPxy();
extern u16 InitPxyServer(TProxyServerCfg &tCfg);
extern void UnInitPxyServer();


#endif

