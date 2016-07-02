#ifndef DHCP_EVENT_H
#define DHCP_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmDHCP
{
#endif 

  _ev_segment( dhcp  )

/***********************<< DHCP 分配失败  >>************************
 *[消息体]			 
 *  + BOOL  TRUE = 注册成功 FALSE失败
 *
 *[消息方向]
 *
 *    MtService => UI
 *
 */_event(  ev_DHCPIpDown   )
  _body(u8,1 )/*取得/失去IP地址的网口号*/
  _body(u32_ip,1 )/*取得/失去的IP地址，网络字节序*/
  _body(u32_ip,1 )/*取得/失去的IP地址子网掩码，网络字节序*/
  _body(u32_ip,1 )/*取得/失去的默认网关，网络字节序，有可能为0，因为有的DHCP-Server不一定为设备指定默认网关*/
   _ev_end


/***********************<< DHCP IP过期  >>************************
 *[消息体]			 
 *[消息方向]
 *    HDCP server        => MtService 
 *
 */_event(  ev_DHCPOverdue   )
   _ev_end


/***********************<< 获取DHCP IP  >>************************
 *[消息体]			 
 *  无
 *[消息方向]
 *    UI | MTC       => MtService 
 */_event(  ev_GetDHCPIPCmd   )
   _ev_end
   
/***********************<< 关闭DHCP 服务命令  >>************************
 *[消息体]			 
 *  无
 *[消息方向]
 *    UI | MTC       => MtService 
 */_event(  ev_ShutdownDHCPIPCmd   )
   _ev_end

/***********************<< 关闭DHCP 服务指示  >>************************
 *[消息体]			 
 *  BOOL  是否关闭成功
 *[消息方向]
 *    UI | MTC       => MtService 
 */_event(  ev_ShutdownDHCPIPInd   )
   _body(BOOL,1)
   _ev_end

/***********************<< DHCP超时  >>************************
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *    MtService         => MtService 
 *
 */_event(  ev_DHCPTimesout   )
   _ev_end
   
  /***********************<< 配置DHCP  >>************************
 *[消息体]			 
 * BOOL 是否启用DHCP
 *[消息方向]
 *
 *    UI | MTC       => MtService 
 *
 */_event(  ev_CfgDHCPIPCmd   )
	_body(BOOL , 1)
   _ev_end

  /***********************<< 配置DHCP指示  >>************************
 *[消息体]			 
 *  无	
 *[消息方向]
 *
 *    MtService => UI | MTC
 *
 */_event(  ev_CfgDHCPIPInd   )
   _body(BOOL , 1)
   _ev_end
   
  /***********************<< 通知用户DHCP IP信息  >>************************
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *    MtService => UI | MTC
 *
 */_event(  ev_NotifyDHCPInfoInd   ) 
  _body(u32_ip,1 )/*取得/失去的IP地址，网络字节序*/
  _body(u32_ip,1 )/*取得/失去的IP地址子网掩码，网络字节序*/
  _body(u32_ip,1 )/*取得/失去的默认网关，网络字节序，有可能为0，因为有的DHCP-Server不一定为设备指定默认网关*/
  _ev_end   
  /***********************<< 通知Serveci DHCP IP申请成功  >>************************
 *[消息体]			 
 * u32_ip  DHCP IP 地址
 * u32_ip  DHCP 网关地址
 *[消息方向]
 *
 *    HDCP server        => MtService 
 *
 */_event(  ev_NewDHCPIpInd   )
  _body(u8,1 )/*取得/失去IP地址的网口号*/
  _body(u32_ip,1 )/*取得/失去的IP地址，网络字节序*/
  _body(u32_ip,1 )/*取得/失去的IP地址子网掩码，网络字节序*/
  _body(u32_ip,1 )/*取得/失去的默认网关，网络字节序，有可能为0，因为有的DHCP-Server不一定为设备指定默认网关*/
  _ev_end     
   
 /***********************<< 通知Service DHCP IP申请失败  >>************************
 *[消息体]			 
 * u32_ip  DHCP IP 地址
 * u32_ip  DHCP 网关地址
 *[消息方向]
 *
 *    HDCP server        => MtService 
 *
 */_event(  ev_RenewDHCPIpFailInd   )
  _body(u8,1 )/*失去IP地址的网口号*/
  _body(u32_ip,1 )/*失去的IP地址，网络字节序*/
  _body(u32_ip,1 )/*失去的IP地址子网掩码，网络字节序*/
  _body(u32_ip,1 )/*失去的默认网关，网络字节序，有可能为0，因为有的DHCP-Server不一定为设备指定默认网关*/
  _ev_end     

#ifndef _MakeMtEventDescription_
   _ev_segment_end(dhcp)
};
#endif
#endif

