/*****************************************************************************
模块名      : dataswitch_kdm
文件名      : dataswitch_kdm.h
相关文件    : dataswitch_kdm.cpp
文件实现功能: 监控系统全球眼项目媒体流交换
作者		: gie
版本        : V4.0
-----------------------------------------------------------------------------
修改记录:
日  期      版本     修改人      修改内容
2005/11/02  4.0      gie         创建
******************************************************************************/

#ifndef __DATA_SWITCH_KDM_H__
#define __DATA_SWITCH_KDM_H__

#include "osp.h"
#include "kdmmedianet.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma pack( push )
#pragma pack( 1 )
#endif

//DataSwitch_kdm版本号
#define DSVERSION       "Dataswitch_kdm 40.10.06.06.051208"

//DataSwitch 本机IP最大个数
#define MAX_DS_IP_NUM						(u8)2

//DataSwitch组播组最大个数
#define MAX_DS_MCGRP_NUM					(u16)256

//数据包最大长度（包括头）
#define MAX_RTP_PACKET_LENGTH				(u32)1600

//超时等待时间
#define MAX_COMMAND_TIMEOUT					(u32)8

//操作成功
#define DS_OK								(u32)0
//操作错误码
#define DS_ERROR							(u32)10000

//DataSwitch已经成功初始化
#define DS_ALREADY_INITIAL					( DS_ERROR + 1 )
//DataSwitch初始化IP参数错误
#define DS_INTERFACE_ERROR					( DS_ERROR + 2 )
//创建内部通信Socket失败
#define DS_CREATE_INTERIOR_SOCKET_FAILED	( DS_ERROR + 3 )
//DataSwitch未成功初始化
#define DS_NOT_INITIAL						( DS_ERROR + 4 )
//接口参数中IP无效
#define DS_NOT_VALID_IP						( DS_ERROR + 5 )
//接口参数中Port无效
#define DS_NOT_VALID_PORT					( DS_ERROR + 6 )
//接口参数中IP并非DataSwitch本机IP
#define DS_NOT_DATASWITCH_IP				( DS_ERROR + 7 )
//接口参数中SessionID错误
#define DS_INVALID_SESSION_ID				( DS_ERROR + 8 )

//创建规则维护和数据交换任务失败
#define DS_CREATE_DATASWITCHTASK_FAILED		( DS_ERROR + 10 )
//创建内部通信信道失败
#define DS_START_INTERIOR_CONNECT_FAILED	( DS_ERROR + 11 )
//创建建链任务失败
#define DS_CREATE_TCPCONNECTTASK_FAILED		( DS_ERROR + 12 )
//指针参数为空
#define DS_INVALID_NULL_POINTER				( DS_ERROR + 13 )
//有效的Socket集已满
#define DS_NO_MORE_VALID_SOCKET				( DS_ERROR + 14 )

//主任务发送消息给规则维护和数据交换任务失败
#define DS_SENDCOMMAND_SEND_FAILED			( DS_ERROR + 20 )
//主任务等待规则维护和数据交换任务回复消息超时
#define DS_SENDCOMMAND_SELECT_TIMEOUT		( DS_ERROR + 21 )
//主任务等待规则维护和数据交换任务回复消息失败
#define DS_SENDCOMMAND_SELECT_FAILED		( DS_ERROR + 22 )
//主任务接收规则维护和数据交换任务回复消息失败
#define DS_SENDCOMMAND_RECV_FAILED			( DS_ERROR + 23 )

//创建接收结点失败(暂时TCP接收结点不能复用TCP发送结点的Socket，UDP可以)
#define DS_CREATE_RECVNODE_FAILED_REUSE		( DS_ERROR + 30 )
//创建接收结点失败(创建Socket失败)
#define DS_CREATE_RECVNODE_FAILED_SOCKET	( DS_ERROR + 31 )
//创建接收结点失败(作为TCP服务端监听失败)
#define DS_CREATE_RECVNODE_FAILED_LISTEN	( DS_ERROR + 32 )
//创建接收结点失败(创建结点失败)
#define DS_CREATE_RECVNODE_FAILED_NODE		( DS_ERROR + 33 )
//创建接收结点失败(加入组播组失败)
#define DS_CREATE_RECVNODE_FAILED_JOINMCGRP	( DS_ERROR + 34 )

//复用结点失败(TCP地址不能同时作为服务端和客户端)
#define DS_TCP_CANT_AS_SERVER_AND_CLIENT_SIMULTANEOUS			( DS_ERROR + 40 )
//复用结点失败(TCP客户端不可能同时连接多个服务端)
#define DS_TCP_CLIENT_CANT_CONNECT_MULTI_SERVER_SIMULTANEOUS	( DS_ERROR + 41 )
//复用结点失败(加入组播组失败)
#define DS_REUSE_RECVNODE_FAILED_JOINMCGRP						( DS_ERROR + 41 )

//创建源结点失败(创建结点失败)
#define DS_CREATE_SRCNODE_FAILED_NODE		( DS_ERROR + 50 )

//创建发送结点失败(暂时TCP发送结点不能复用TCP接收结点的Socket，UDP可以)
#define DS_CREATE_SENDNODE_FAILED_REUSE		( DS_ERROR + 60 )
//创建发送结点失败(创建Socket失败)
#define DS_CREATE_SENDNODE_FAILED_SOCKET	( DS_ERROR + 61 )
//创建发送结点失败(作为TCP服务端监听失败)
#define DS_CREATE_SENDNODE_FAILED_LISTEN	( DS_ERROR + 62 )
//创建发送结点失败(创建结点失败)
#define DS_CREATE_SENDNODE_FAILED_NODE		( DS_ERROR + 63 )

//创建发送目标结点失败(创建结点失败)
#define DS_CREATE_SENDTOITEM_FAILED_ITEM	( DS_ERROR + 70 )
//创建发送目标结点失败(禁止相同接收组，相同发送目的的规则)
#define DS_CREATE_SENDTOITEM_FORBID_SAMERECV_SAMESENDTO	( DS_ERROR + 71 )
//创建发送目标结点失败(禁止相同发送组的规则)
#define DS_CREATE_SENDTOITEM_FORBID_SAMESEND	( DS_ERROR + 72 )

//接收结点不存在
#define DS_NO_EXIST_RECVNODE				( DS_ERROR + 80 )
//源结点不存在
#define DS_NO_EXIST_SRCNODE					( DS_ERROR + 81 )
//发送结点不存在
#define DS_NO_EXIST_SENDNODE				( DS_ERROR + 82 )
//发送目标结点不存在
#define DS_NO_EXIST_SENDTOITEM				( DS_ERROR + 83 )

//设置发送UDP接收回复
#define DS_UDP_RECV_ACK_FAILED_RECVNODE		( DS_ERROR + 90 )

//发送时允许的最大修改长度
#define SENDMEM_MAX_MODLEN					(u8)32

//交换规则接收结点结构,表示接收端的套接字布局
//一。当m_dwProtocol(=SOCK_DGRAM)时,表示接收采用UDP协议,m_bTCPServer无意义:
//	1。要求m_dwRecvIP必须在dsCreate参数本机接口IP集合中,并且要求m_wRecvPort(!=0);
//	2。m_bUseSessionID(=FALSE),表示用IP Port对表示源:
//		a。m_dwSrcIP(=0) && m_wSrcPort(=0) 按端口转发的规则;
//		b。m_dwSrcIP(!=0) && m_wSrcPort(!=0) 按源转发的规则;
//	3。m_bUseSessionID(=TRUE),表示采用SessionID表示源,并且要求m_dwSrcSessionID(!=0);
//	4。m_dwMulticastGrp为组播地址时,要求必须加入组播组接收数据;
//	5。其他不符合规范;
//二。当m_dwProtocol(=SOCK_STREAM)时,表示发送采用TCP协议,m_dwMulticastGrp无意义:
//	1。要求m_dwRecvIP必须在dsCreate参数本机接口IP集合中,并且要求m_wRecvPort(!=0);
//	2。m_bTCPServer(=FALSE),表示作为TCP客户端,m_bUseSessionID无意义:
//		要求 m_dwSrcIP(!=0) && m_wSrcPort(!=0);
//	3。m_bTCPServer(=TRUE),表示作为TCP服务端:
//		a。m_bUseSessionID(=FALSE),表示用IP Port对表示源,要求 m_dwSrcIP(!=0) && m_wSrcPort(!=0);
//		b。m_bUseSessionID(=TRUE),表示采用SessionID表示源,要求 m_dwSrcSessionID(!=0);
//	4。其他不符合规范;
struct TSwitchRecvPoint
{
	u32 m_dwProtocol;			//接收采用协议(SOCK_STREAM表示采用TCP,SOCK_DGRAM表示采用UDP)
	BOOL32 m_bTCPServer;		//接收作为TCP服务端还是TCP客户端
	BOOL32 m_bUseSessionID;		//接收是否采用SessionID表示源
	u32 m_dwRecvIP;				//本地接收IP
	u16 m_wRecvPort;			//本地接收端口
	u32 m_dwMulticastGrp;		//接收需要加入的组播组
	u32 m_dwSrcIP;				//源IP
	u16 m_wSrcPort;				//源端口
	u32 m_dwSrcSessionID;		//源SessionID
}
#ifndef _MSC_VER
__attribute__ ( (packed) )
#endif
;

//交换规则发送结点结构,表示发送端的套接字布局
//一。当m_dwProtocol(=SOCK_DGRAM)时,表示发送采用UDP协议,m_bTCPServer无意义:
//	1。要求m_dwSendIP必须在dsCreate参数本机接口IP集合中,并且m_wSendPort(!=0);
//	2。采用UDP协议发送,同时要求 m_dwSendtoIP(!=0) && m_wSendtoPort(!=0);
//	3。其他不符合规范;
//二。当m_dwProtocol(=SOCK_STREAM)时,表示发送采用TCP协议:
//	1。要求m_dwSendIP必须在dsCreate参数本机接口IP集合中,并且m_wSendPort(!=0);
//	2。m_bTCPServer(=FALSE),表示作为TCP客户端,同时要求 m_dwSrcIP(!=0) && m_wSrcPort(!=0);
//	3。m_bTCPServer(=TRUE),表示作为TCP服务端,同时要求 m_dwSrcIP(!=0) && m_wSrcPort(!=0);
//	4。其他不符合规范;
struct TSwitchSendPoint
{
	u32	m_dwProtocol;			//发送采用协议(SOCK_STREAM表示采用TCP,SOCK_DGRAM表示采用UDP)
	BOOL32 m_bTCPServer;		//发送作为TCP服务端还是TCP客户端
	u32 m_dwSendIP;				//本地发送IP
	u16 m_wSendPort;			//本地发送端口
	u32 m_dwSendtoIP;			//发送目标IP
	u16 m_wSendtoPort;			//发送目标端口
}
#ifndef _MSC_VER
__attribute__ ( (packed) )
#endif
;

//交换规则结构,作为创建规则参数
//	a。UDP接收、TCP作为服务端接收 可以采用SessionID表示源
//	b。TCP作为客户端接收 不能采用SessionID表示源
//	c。发送 不能采用SessionID表示发送目标
struct TSwitchRule
{
	TSwitchRecvPoint m_tSwitchRecvPoint;	//接收结点
	TSwitchSendPoint m_tSwitchSendPoint;	//发送结点
}
#ifndef _MSC_VER
__attribute__ ( (packed) )
#endif
;

/*===================================================================================================
函数名      :	dsCreate
功能        :	初始化DataSwitch功能库,使用DataSwitch库必须先调用该接口;
算法实现    :
引用全局变量:
输入参数说明:	byIpNum	本机接口IP数目(最大个数为MAX_DS_IP_NUM);
				pdwIp	本机接口IP;
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsCreate( u8 byIpNum , u32* pdwIp );

/*===================================================================================================
函数名      :	dsDestroy
功能        :	卸载DataSwitch功能库,结束使用时需调用本接口;
算法实现    :
引用全局变量:
输入参数说明:	无
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsDestroy( );

/*===================================================================================================
函数名      :	dsAddSwitchRule
功能        :	创建交换规则;
算法实现    :
引用全局变量:
输入参数说明:	ptSwitchRule	交换规则
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsAddSwitchRule( const TSwitchRule* ptSwitchRule );

/*===================================================================================================
函数名      :	dsRemoveSwitchRule
功能        :	删除交换规则;
算法实现    :
引用全局变量:
输入参数说明:	ptSwitchRule	交换规则
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsRemoveSwitchRule( const TSwitchRule* ptSwitchRule );

/*===================================================================================================
函数名      :	dsSetSendUDPRecvACK
功能        :	设置发送UDP接收回复;
算法实现    :
引用全局变量:
输入参数说明:	dwRecvIP	接收IP
				wRecvPort	接收Port
				bSendACK	是否发送回复
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsSetSendUDPRecvACK( u32 dwRecvIP , u16 wRecvPort , BOOL32 bSendACK = TRUE );

//规则TCP链路状态
enum TLinkStatus
{
	RECV_CONNECTING = 1,	//接收链路建立中
	RECV_CONNECTED,			//接收链路已经建立

	SEND_CONNECTING = 10,	//发送链路建立中
	SEND_CONNECTED,			//发送链路已经建立
};

/*===================================================================================================
函数名      :	LinkStatusCallBack
功能        :	TCP链路的创建和维护回调函数(尽量不要在该函数中做可能延时的操作)
算法实现    :
引用全局变量:
输入参数说明:	ptSwitchRule	交换规则
				tRuleStatus		链路状态;
返回值说明  :	无
=====================================================================================================*/
typedef void (*LinkStatusCallBack)( const TSwitchRule* ptSwitchRule, TLinkStatus tLinkStatus );

/*===================================================================================================
函数名      :	dsSetRuleStatusCallback
功能        :	设置链路函数
算法实现    :
引用全局变量:
输入参数说明:	pfRuleStatusCallback	链路回调函数;
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsSetRuleStatusCallback( LinkStatusCallBack pfLinkStatusCallback );

/*===================================================================================================
函数名      :	dsGetRecvPktCount
功能        :	查询接收包数;
算法实现    :
引用全局变量:
输入参数说明:	ptSwitchRule	交换规则
				dwRecvPktCount	接收包数
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsGetRecvPktCount( const TSwitchRule* ptSwitchRule , u32& dwRecvPktCount );

/*===================================================================================================
函数名      :	dsGetSendPktCount
功能        :	查询发送包数;
算法实现    :
引用全局变量:
输入参数说明:	ptSwitchRule	交换规则
				dwSendPktCount	发送包数
返回值说明  :	DSOK	操作成功
				其他	操作失败的错误码
=====================================================================================================*/
u32 dsGetSendPktCount( const TSwitchRule* ptSwitchRule , u32& dwSendPktCount );

/*===================================================================================================
函数名      :	dsGetRecvBytesCount
功能        :	查询接收总字节数
算法实现    :
引用全局变量:
输入参数说明:	
返回值说明  :	返回即时接收字节数
=====================================================================================================*/
u32 dsGetRecvBytesCount( );

/*===================================================================================================
函数名      :	dsGetSendBytesCount
功能        :	查询发送总字节数
算法实现    :
引用全局变量:
输入参数说明:	
返回值说明  :	返回即时字节数
=====================================================================================================*/
u32 dsGetSendBytesCount( );

/*===================================================================================================
函数名      :	dsinfo
功能        :	显示所有转发规则和可读Socket的状态;
算法实现    :
引用全局变量:
输入参数说明:	无
返回值说明  :	无
=====================================================================================================*/
API void dsinfo();

/*===================================================================================================
函数名      :	dsver
功能        :	打印版本号;
算法实现    :
引用全局变量:
输入参数说明:	无
返回值说明  :	无
=====================================================================================================*/
API void dsver();

/*===================================================================================================
函数名      :	dsdebug
功能        :	切换打印开关,初始打印关闭;
算法实现    :
引用全局变量:
输入参数说明:	无
返回值说明  :	无
=====================================================================================================*/
API void dsdebug();

/*===================================================================================================
函数名      :	dshelp
功能        :	打印DataSwitch支持的命令和使用方法;
算法实现    :
引用全局变量:
输入参数说明:	无
返回值说明  :	无
=====================================================================================================*/
API void dshelp();

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif/*!__DATA_SWITCH_KDM_H__*/
