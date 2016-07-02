/******************************************************************************
模块名  ： DHCP-CLIENT
文件名  ： boarddhcp.h
相关文件：
文件实现功能：DHCP-CLIENT模块对外提供的宏定义头文件
作者    ：王俊华
版本    ：1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
07/14/2006  1.0         王俊华      创建
******************************************************************************/
#ifndef _BOARDDHCP_H
#define _BOARDDHCP_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */


/*宏定义*/

/*DHCP回调函数的消息码基址*/
#define DHCP_MSG_BASE			                    (u32)30000
/*通知用户DHCP获得的IP地址的回调消息，消息体：TBrdDhcpNotify */
#define DHCP_GET_IP_MSG							    (u32)( DHCP_MSG_BASE+1)
/*通知用户设备失去DHCP获得的IP地址的回调消息，消息体：TBrdDhcpNotify */
#define DHCP_LOSS_IP_MSG							(u32)( DHCP_MSG_BASE+2)
/*通知用户因租约到期但又得不到地址原因导致设备失去DHCP获得的IP地址的回调消息，消息体：TBrdDhcpNotify */
#define DHCP_RENEW_FAIL_MSG							(u32)( DHCP_MSG_BASE+3)

#define DHCP_IFDOWN_LOSS_IP_MSG						 (u32)(DHCP_MSG_BASE+4)
#define DHCP_IFERROR_LOSS_IP_MSG					 (u32)(DHCP_MSG_BASE+5)
#define DHCP_DNS_MAX_DNS_SERVER 64

/*结构体定义*/

/*通知用户DHCP获得或者失去的IP地址的结构体*/
typedef struct{
u8 byEthId;/*取得/失去IP地址的网口号*/
u32 dwIpAddrs;/*取得/失去的IP地址，网络字节序*/
u32 dwMask;/*取得/失去的IP地址子网掩码，网络字节序*/
u32 dwGateWay;/*取得/失去的默认网关，网络字节序，有可能为0，因为有的DHCP-Server不一定为设备指定默认网关*/
u32 dwDnsNum;/*DNS Server 数量最多为64个 */
u32 dwDnsServer[DHCP_DNS_MAX_DNS_SERVER];/*取得/失去的DNS Server ，网络字节序*/
}TBrdDhcpNotify;

/*开启了DHCP-Client端功能的网口上获取的IP地址、租约等相关信息结构体*/
typedef struct{
u8 byEthId;/*取得IP地址的网口号*/
u32 dwIpAddrs;/*取得的IP地址，网络字节序*/
u32 dwMask;/*取得的IP地址子网掩码，网络字节序*/
u32 dwGateWay;/*取得的默认网关，网络字节序，有可能为0，因为有的DHCP-Server不一定为设备指定默认网关*/
u32 dwLeaseInfo;/*总的租约时间，以秒为单位*/
u32 dwDnsNum;/*DNS Server 数量最多为64个 */
u32 dwDnsServer[DHCP_DNS_MAX_DNS_SERVER];/*取得/失去的DNS Server ，网络字节序*/
}TBrdDhcpLeaseInfo;

typedef struct
{
	u32 dwDhcpcIsRun;
}TBrdDhcpcState;

/*回调函数类型定义*/

/*================================
函数名：BrdDhcpNotifyCallBack
功能：当DHCP获得IP地址或者因某种原因失去已获得的IP地址时，通知用户的回调函数
算法实现：（可选项）
引用全局变量：
输入参数说明：dwMsgID：事件类型，见上面的消息类型定义
		      pMsg：与该事件类型相匹配的消息内容，见上面消息类型说明
返回值说明： 暂时都定为0，以后可以扩展
==================================*/
typedef int(*BrdDhcpNotifyCallBack) (u32  dwMsgID ,void*  pMsg);


/*函数接口定义*/


/*================================
函数名：BrdDhcpStart
功能：在指定的网口上开启DHCP-Client端功能
算法实现：（可选项）
引用全局变量：
输入参数说明：byEthId：要开启DHCP-Client端功能的网口号(从0开始)
		      ptDhcpCallBack：上层注册的回调函数指针
              bSetDefautGate:  回调网关是否设为默认网关
返回值说明： OK/ERROR
==================================*/
STATUS BrdDhcpStart (u8  byEthId,  BrdDhcpNotifyCallBack  ptDhcpCallBack, BOOL32 bSetDefautGate);

/*================================
函数名：BrdDhcpRenew
功能：在开启了DHCP-Client端功能的网口上主动请求更新租约
算法实现：（可选项）
引用全局变量：
输入参数说明：byEthId：要更新租约的网口号(从0开始) 
返回值说明： OK/ERROR
==================================*/
STATUS BrdDhcpRenew (u8  byEthId);

/*================================
函数名：BrdDhcpGetLeaseInfo
功能：在开启了DHCP-Client端功能的网口上获取租约等相关信息
算法实现：（可选项）
引用全局变量：
输入参数说明：byEthId：开启了DHCP-Client端功能的网口号(从0开始) 
			  ptBrdDhcpLeaseInfo:返回的包含租约等信息的结构体指针
返回值说明： OK/ERROR
==================================*/
STATUS BrdDhcpGetLeaseInfo (u8  byEthId, TBrdDhcpLeaseInfo*  ptBrdDhcpLeaseInfo);

/*================================
函数名：BrdDhcpStop
功能：在指定的网口上关闭DHCP-Client端功能
算法实现：（可选项）
引用全局变量：
输入参数说明：byEthId：要关闭DHCP-Client端功能的网口号(从0开始) 
返回值说明： OK/ERROR
==================================*/
STATUS BrdDhcpStop (u8  byEthId);

/*================================
函数名：BrdDhcpGetIfstate
功能：在指定的网口上的dhcp信息
算法实现：（可选项）
引用全局变量：
输入参数说明：byEth：网口号(从0开始) 
返回值说明： OK/ERROR
==================================*/
STATUS BrdDhcpGetIfstate(u8 byEth, TBrdDhcpcState *tBrddhcpcIfState);



#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /*_BOARDDHCP_H*/
