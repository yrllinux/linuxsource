// kdv设备探测工具接口文件

#ifndef KDVDEVTRACK__H__
#define KDVDEVTRACK__H__

#include "kdvtype.h"

//////////////////////////////////////////////////////////////////////////
// 常量定义
//////////////////////////////////////////////////////////////////////////

// 广播消息'KDT1'-KDV Device Track 1.0 头
#define DT_MAGIC	"KDT1"

const u32 DT_LEN_MAGIC = 4;
#define DT_LEN_MACADDR   6
#define DT_LEN_NAME	64

const u32 DT_ERR_OK = 0;
const u32 DT_ERR_SET_IP_FAILED = 1;

const u16 DT_PORT = 8878;
//////////////////////////////////////////////////////////////////////////
// 消息ID定义
//////////////////////////////////////////////////////////////////////////

#define DECLARE_MESSAGE(m, id) const u16 m = id;

// 广播消息
DECLARE_MESSAGE(NC_DEV_TRACK_REQ_CAST,		0x0001)	// 设备探测广播消息
// 接收到搜索设备消息或设备启动时发送
DECLARE_MESSAGE(DEV_NC_TRACK_ACK_CAST,		0x0002)	// 设备信息通知消息：TDTHeader+TDTDevInfo

DECLARE_MESSAGE(NC_DEV_MODADDR_REQ_CAST,	0x0003)	// 修改设备地址消息: TDTHeader+TDTDevAddr
// 修改设备完成，重启前发送此消息
DECLARE_MESSAGE(DEV_NC_MODADDR_ACK_CAST,	0x0004)	// 修改设备地址消息: TDTHeader+u32(ErrorCode)

DECLARE_MESSAGE(NC_DEV_REBOOT_REQ_CAST,	0x0005)		// 重启设备消息

//////////////////////////////////////////////////////////////////////////
// 结构体定义
//////////////////////////////////////////////////////////////////////////

// Windows OS下字节紧缩对齐
#ifdef WIN32
	#pragma pack( push )
	#pragma pack( 1 )
//	#define window( x )	x
#else
	#include <netinet/in.h>
//	#define window( x )
#endif

struct TDTHeader
{
	u8  abyMacSrc[DT_LEN_MACADDR];	// 发送者MAC地址(CHAR[12])  
	u8  abyMacDst[DT_LEN_MACADDR];	// 接收者MAC地址(CHAR[12]) 	
	u16 wMsgID;		// 消息ID
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TDTDevAddr
{
	// DHCP服务器类型，第一字节: 0-没有DHCP服务 1-服务器，2-客户端 
	// 第二字节: 0-关闭 1-开启；例如：0x0000表示没有服务器，0x0201表示客户端(开启)
	u16 wDHCPOpen;	// 0-关闭 1-开启DHCP
	u32 dwIpaddr;		// IP地址，网络序
	u32 dwNetmask;		// 子网掩码，网络序
	u32 dwGateWay;		// 默认网关，网络序
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

struct TDTDevInfo
{
	struct TDTDevAddr tAddr;	// IP、MASK、GW
	u32 dwTickCount;	// 运行时间
	u32 dwBrdID;		// 设备类型ID
	s8 achDevName[DT_LEN_NAME+1];	// 设备名称
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

// Windows OS下字节紧缩对齐
#ifdef WIN32
	#pragma pack( pop )
#endif

#endif // KDVDEVTRACK__H__
