/******************************************************************************
模块名	: OSP
文件名	: osp.h
功能    : OSP库的头文件
版本	：KDV 4.0
******************************************************************************/
/*************************************************
模块版本：OSP.1.5.3.200803011
增加功能：VX下mstotick修改，使用u64存在问题
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.5.2.200803011
增加功能：OspTaskSetPriority在SCHED_OTHER情况下优先级只有0才有效
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.5.0.20080304
增加功能：增加OspTaskSetPriorityByName等接口
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.4.4.20080303
增加功能：解决telnet的异常问题，NVR, BUG1048, 803
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.4.3.20080116
增加功能：Ospconnecttcpnode的错误修改，连接到不存在节点时的问题
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.4.2.20080109
增加功能：SemTakeByTime临界区问题解决,nsec=1000000的情况导致pthread_wait_cond错误
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.4.1.20080102
增加功能：增加取消绝对定时器接口，修正LInux下绝对定时器的BUG
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.3.0.20071214
增加功能：KDV的OSP同KDM同步
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.2.5.20071211
增加功能：osp注册断链检测，增加锁保护
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.2.4.20071211
增加功能：osp post加保护，确保APPID对应的APP有效
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.2.3.20070917
增加功能：协议组OSP,修正OspTaskShow，%d 为 %u
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.2.2.20070828
增加功能：协议组OSP,修正OspGetTick接口，避免在溢出点前返回-1，始终返回正确的tick数，确保计算差值的准确性
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.2.1.20070823
增加功能：协议组OSP,增加OspGetTick64接口，返回64位tick数，解决32位tick的溢出问题
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.1.4.20070727
增加功能：协议组OSP,获取CPU利用率的函数有一些静态变量，增加锁保护，保证并发执行下的安全
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.1.3.20070725
增加功能：协议组OSP,去除OSP的信号量统计，信号量数量多的时候会应用程序导致效率极其低下，因此去除该功能
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.1.1.2.20070721
增加功能：协议组OSP,增加OspTaskShow中的全局任务记录数据的互斥保护，以及其数组越界检查
修改缺陷：无
提交人：徐一平
**************************************************/
/*************************************************
模块版本：OSP.KDV4.0.1.1.1.20070627
增加功能：协议组OSP初始版本
修改缺陷：无
提交人：徐一平
**************************************************/


#ifndef _OSP_H
#define _OSP_H

///////////////////////////////////////////////////////////////////
//	Linux 操作系统头文件
///////////////////////////////////////////////////////////////////
#ifdef _LINUX_

	#ifdef PWLIB_SUPPORT
		#include <ptlib.h>
	#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <stdarg.h>
	#include <malloc.h>
	#include <errno.h>
	#include <unistd.h>
	#include <pthread.h>
	#include <semaphore.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <sys/times.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <termios.h>
	#include <signal.h>
	
	#ifndef _EQUATOR_
		#include <mqueue.h>
		#include <sys/epoll.h>
		#include <sys/resource.h>
	#endif

#endif //_LINUX_

///////////////////////////////////////////////////////////////////
//	Vxworks 操作系统头文件
///////////////////////////////////////////////////////////////////
#ifdef _VXWORKS_

	#include <vxworks.h>
	#include <tasklib.h>
	#include <msgqlib.h>
	#include <syslib.h>
	#include <string.h>
	#include <memLib.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sockLib.h>
	#include <inetLib.h> 
	#include <stdioLib.h> 
	#include <strLib.h>
	#include <hostLib.h> 
	#include <ioLib.h>
	#include <assert.h>
	#include <netdb.h>
	#include <errno.h>
	#include <sys/stat.h>
	#include <taskHookLib.h>
	#include <logLib.h>
	//串口支持
	#include "brddrvlib.h"

#endif // _VXWORKS_

///////////////////////////////////////////////////////////////////
//	Win32 操作系统头文件
///////////////////////////////////////////////////////////////////
#ifdef _MSC_VER

	#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
	#endif
	#if _MSC_VER <= 1200
		#define _WIN32_WINNT 0x0400
	#else
		#define _WIN32_WINNT 0x0501
	#endif

	#ifdef WINVER
		#undef WINVER
	#endif
	#if _MSC_VER <= 1200
		#define WINVER 0x0400
	#else
		#define WINVER 0x0501
	#endif

#include <malloc.h>
#include <time.h>

	#ifdef _AFXDLL
		#include <afx.h>
		#include <afxwin.h>
		#include <winsock2.h>
	#else
		#ifndef WIN32_LEAN_AND_MEAN
			#define WIN32_LEAN_AND_MEAN
		#endif
        #include <stdio.h>
        #include <stdlib.h>
		#include <windows.h>
        #include <winsock2.h>
	#endif

	#include <assert.h>
	#pragma comment(lib,"Ws2_32.lib")

#endif	// _MSC_VER

///////////////////////////////////////////////////////////////////
//	公共系统头文件（类型定义）
///////////////////////////////////////////////////////////////////
#include "kdvtype.h"

///////////////////////////////////////////////////////////////////
//	操作系统相关数据类型定义
///////////////////////////////////////////////////////////////////
#ifdef _LINUX_

	typedef int 				 BOOL;
	#define TRUE				 1
	#define FALSE				 0

	typedef void *(*LINUXFUNC)(void *);

	#define SEMHANDLE			 pthread_cond_t*

	#define TASKHANDLE			 pthread_t

#endif	// _LINUX_

#ifdef _VXWORKS_

	#define SEMHANDLE			 SEM_ID
	#define TASKHANDLE			 u32

#endif	// _VXWORKS_

#ifndef _MSC_VER

	typedef unsigned long long	 u64;
	#define SOCKHANDLE			 int
	#define SERIALHANDLE		 int

	#ifndef SOCKADDR
		#define SOCKADDR		 sockaddr
	#endif
	#define SOCKADDR_IN 		 sockaddr_in

#endif	// _MSC_VER

#ifdef _MSC_VER

	#define SEMHANDLE			 HANDLE
	#define TASKHANDLE			 HANDLE

	#define SOCKHANDLE			 SOCKET
	#define SERIALHANDLE		 HANDLE

#endif	// _MSC_VER

///////////////////////////////////////////////////////////////////
//	操作系统相关宏定义
///////////////////////////////////////////////////////////////////
#ifdef _LINUX_

	#define SOCKET_ERROR		  (-1)
	#define INVALID_SOCKET		  (-1)
	#define INVALID_SERIALHANDLE  (-1)

	#define OSP_NO_WAIT 		  0
	#define OSP_WAIT_FOREVER	  (-1)

	//SockShutdown参数
	#define STOP_RECV			  0
	#define STOP_SEND			  1
	#define STOP_BOTH			  2

#endif // _LINUX_

#ifdef _VXWORKS_

	#define SOCKET_ERROR		  ERROR
	#define INVALID_SOCKET		  ERROR
	#define INVALID_SERIALHANDLE  ERROR

	#define OSP_NO_WAIT 		  NO_WAIT
	#define OSP_WAIT_FOREVER	  WAIT_FOREVER

#endif	// _VXWORKS_

#ifdef _MSC_VER

	#define INVALID_SERIALHANDLE  INVALID_HANDLE_VALUE
	#define TOspCommTimeouts	  COMMTIMEOUTS
	#define TOspDCB 			  DCB
	#define TOspComStat 		  COMSTAT

	#define OSP_NO_WAIT 		  ((int)0)
	#define OSP_WAIT_FOREVER	  ((int)-1)

#endif	// _MSC_VER

///////////////////////////////////////////////////////////////////
//	操作系统无关宏定义
///////////////////////////////////////////////////////////////////
// for Microsoft c++
#ifdef _MSC_VER

	//紧排列
	#ifndef _EQUATOR_
		#pragma pack(push)
		#pragma pack(1)
	#endif

	#define API extern "C"			__declspec(dllexport)
	#define OSP_API 				__declspec(dllexport)

// for gcc
#else

	#define API extern "C"
	#define OSP_API

#endif	// _MSC_VER

//OSP版本信息
#define OSPVERSION				"OSP.KDV.Platform.CBB.1.6.2.20111018"





//OSP版本号
#define OSP_VERSION_ID			0x40

//未使用
const u8  OSPLOCAL				= 0;
//未使用
const u8  TCP_GATEWAY			= 255;
//未使用
const u16 SECONDS				= 1000;

#ifdef _LINUX_
    #define MULTI_LISTEN_SOCKET_SUPPORT 1
#endif

//本地最大监听套接字数
#ifdef MULTI_LISTEN_SOCKET_SUPPORT
    const u32 MAX_LISTEN_SOCK_NUM       = 16;
#endif //MULTI_LISTEN_SOCKET_SUPPORT


//本地最大外部结点数
#ifdef OSP_MAX_NODE_NUM
	const u32 MAX_NODE_NUM		= OSP_MAX_NODE_NUM;
#else
	const u32 MAX_NODE_NUM		= 512;
#endif

//同一结点最大应用数
const u16 MAX_APP_NUM			 = 512;
//同一应用最大实例数
const u16 MAX_INST_NUM			 = 65000;
//最大定时器数
const u16 MAX_TIMER_NUM 		 = 5000;
//最大串口数
const u8  MAX_SERIALPORT_NUM	 = 10;

//无效的外部结点号
#define INVALID_NODE			 0
//本地结点号
#define LOCAL_NODE				 0

//无效的应用号
#define INVALID_APP 			 0
//本地全局应用号
#define LOCAL_APP				 0

//无效的实例号
#define INVALID_INS 			 0xfffb
//本地实例号
#define LOCAL_INS				 0

//默认TCP断链检测时间间隔(秒)
#define DEFAULT_TCP_HEARTBEAT_TIME		10
//默认TCP断链检测次数(多次检测均失败认为已经断链)
#define DEFAULT_TCP_HEARTBEAT_NUM		3
//默认OSP系统状态打印时间间隔(秒)
#define DEFAULT_STATUS_MSGOUT_TIME		3600
//不进行TCP断链检测
#define UNDETECT_TCP_HEARTBEAT	 		0

#ifndef MINCHAR
	#define MINCHAR 				0x80
#endif

#ifndef MAXCHAR
	#define MAXCHAR 				0x7f
#endif

#ifndef MINSHORT
	#define MINSHORT				0x8000
#endif

#ifndef MAXSHORT
	#define MAXSHORT				0x7fff
#endif

#ifndef MINLONG
	#define MINLONG 				0x80000000
#endif

#ifndef MAXLONG
	#define MAXLONG 				0x7fffffff
#endif

#ifndef MAXBYTE
	#define MAXBYTE 				0xff
#endif

#ifndef MAXWORD
	#define MAXWORD 				0xffff
#endif

#ifndef MAXDWORD
	#define MAXDWORD				0xffffffff
#endif

#ifndef _MSC_VER

//
// Provider capabilities flags.
//

#define PCF_DTRDSR		  ((u32)0x0001)
#define PCF_RTSCTS		  ((u32)0x0002)
#define PCF_RLSD		  ((u32)0x0004)
#define PCF_PARITY_CHECK  ((u32)0x0008)
#define PCF_XONXOFF 	  ((u32)0x0010)
#define PCF_SETXCHAR	  ((u32)0x0020)
#define PCF_TOTALTIMEOUTS ((u32)0x0040)
#define PCF_INTTIMEOUTS   ((u32)0x0080)
#define PCF_SPECIALCHARS  ((u32)0x0100)
#define PCF_16BITMODE	  ((u32)0x0200)

//
// Comm provider settable parameters.
//

#define SP_PARITY		  ((u32)0x0001)
#define SP_BAUD 		  ((u32)0x0002)
#define SP_DATABITS 	  ((u32)0x0004)
#define SP_STOPBITS 	  ((u32)0x0008)
#define SP_HANDSHAKING	  ((u32)0x0010)
#define SP_PARITY_CHECK   ((u32)0x0020)
#define SP_RLSD 		  ((u32)0x0040)

//
// Settable baud rates in the provider.
//
#ifdef _VXWORKS_
#define BAUD_110		  (u32)110
#define BAUD_300		  (u32)300
#define BAUD_1200		  (u32)1200
#define BAUD_2400		  (u32)2400
#define BAUD_4800		  (u32)4800
#define BAUD_9600		  (u32)9600
#define BAUD_19200		  (u32)19200
#define BAUD_38400		  (u32)38400
#endif

#if defined(_LINUX_) && !defined(_EQUATOR_)
#define BAUD_300		  (u32)300
#define BAUD_1200		  (u32)1200
#define BAUD_2400		  (u32)2400
#define BAUD_4800		  (u32)4800
#define BAUD_9600		  (u32)9600
#define BAUD_19200		  (u32)19200
#define BAUD_38400		  (u32)38400
#define BAUD_57600		  (u32)57600
#define BAUD_115200 	  (u32)115200
#endif

// Equator上只能设置波特率，其他的都不能设置
#ifdef _EQUATOR_
// 注意：最高速率为9600
#define BAUD_110		  (u16)110
#define BAUD_300		  (u16)300
#define BAUD_600		  (u16)600
#define BAUD_1200		  (u16)1200
#define BAUD_2400		  (u16)2400
#define BAUD_4800		  (u16)4800
#define BAUD_9600		  (u16)9600
#endif

//
// Settable Data Bits
//
#if defined(_VXWORKS_) || (defined(_LINUX_) && !defined(_EQUATOR_))
#define DATABITS_5		  (u8)5
#define DATABITS_6		  (u8)6
#define DATABITS_7		  (u8)7
#define DATABITS_8		  (u8)8
#else
#define DATABITS_5		  ((u16)0x0001)
#define DATABITS_6		  ((u16)0x0002)
#define DATABITS_7		  ((u16)0x0004)
#define DATABITS_8		  ((u16)0x0008)
#define DATABITS_16 	  ((u16)0x0010)
#define DATABITS_16X	  ((u16)0x0020)
#endif

//
// Settable Stop and Parity bits.
//
#if defined(_VXWORKS_) || (defined(_LINUX_) && !defined(_EQUATOR_))
#define STOPBITS_10 	  (u8)1
#define STOPBITS_20 	  (u8)2
#else
#define STOPBITS_10 	  ((u16)0x0001)
#define STOPBITS_15 	  ((u16)0x0002)
#define STOPBITS_20 	  ((u16)0x0004)
#endif

#if defined(_VXWORKS_) || (defined(_LINUX_) && !defined(_EQUATOR_))
#define PARITY_NONE 	  (u8)0
#define PARITY_ODD	      (u8)1  
#define PARITY_EVEN 	  (u8)2
#define PARITY_MARK 	  (u8)3
#define PARITY_SPACE	  (u8)4
#else
#define PARITY_NONE 	  ((u16)0x0100)
#define PARITY_ODD		  ((u16)0x0200)
#define PARITY_EVEN 	  ((u16)0x0400)
#define PARITY_MARK 	  ((u16)0x0800)
#define PARITY_SPACE	  ((u16)0x1000)
#endif
//
// DTR Control Flow Values.
//
#define DTR_CONTROL_DISABLE		0x00
#define DTR_CONTROL_ENABLE		0x01
#define DTR_CONTROL_HANDSHAKE	0x02

//
// RTS Control Flow Values
//
#define RTS_CONTROL_DISABLE		0x00
#define RTS_CONTROL_ENABLE		0x01
#define RTS_CONTROL_HANDSHAKE	0x02
#define RTS_CONTROL_TOGGLE		0x03

#define NOPARITY				0
#define ODDPARITY				1
#define EVENPARITY				2
#define MARKPARITY				3
#define SPACEPARITY 			4

#define ONESTOPBIT				0
#define ONE5STOPBITS			1
#define TWOSTOPBITS 			2

//
// Escape Functions
//

#define SETXOFF 				1		// Simulate XOFF received
#define SETXON					2		// Simulate XON received
#define SETRTS					3		// Set RTS high
#define CLRRTS					4		// Set RTS low
#define SETDTR					5		// Set DTR high
#define CLRDTR					6		// Set DTR low
#define RESETDEV				7		// Reset device if possible
#define SETBREAK				8		// Set the device break line.
#define CLRBREAK				9		// Clear the device break line.

//
// PURGE function flags.
//
#define PURGE_TXABORT			0x0001	// Kill the pending/current writes to the comm port.
#define PURGE_RXABORT			0x0002	// Kill the pending/current reads to the comm port.
#define PURGE_TXCLEAR			0x0004	// Kill the transmit queue if there.
#define PURGE_RXCLEAR			0x0008	// Kill the typeahead buffer if there.

typedef struct 
{  
	u32 ReadIntervalTimeout; 
	u32 ReadTotalTimeoutMultiplier; 
	u32 ReadTotalTimeoutConstant; 
	u32 WriteTotalTimeoutMultiplier; 
	u32 WriteTotalTimeoutConstant; 
} TOspCommTimeouts; 

typedef struct
{ 
	u32 DCBlength;			 // sizeof(DCB) 
	u32 BaudRate;			 // current baud rate 
	u32 fBinary: 1; 		 // binary mode, no EOF check 
	u32 fParity: 1; 		 // enable parity checking 
	u32 fOutxCtsFlow:1; 	 // CTS output flow control 
	u32 fOutxDsrFlow:1; 	 // DSR output flow control 
	u32 fDtrControl:2;		 // DTR flow control type 
	u32 fDsrSensitivity:1;	 // DSR sensitivity 
	u32 fTXContinueOnXoff:1; // XOFF continues Tx 
	u32 fOutX: 1;			 // XON/XOFF out flow control 
	u32 fInX: 1;			 // XON/XOFF in flow control 
	u32 fErrorChar: 1;		 // enable error replacement 
	u32 fNull: 1;			 // enable null stripping 
	u32 fRtsControl:2;		 // RTS flow control 
	u32 fAbortOnError:1;	 // abort on error 
	u32 fDummy2:17; 		 // reserved 
	u32 wReserved;			 // not currently used 
	u16 XonLim; 			 // transmit XON threshold 
	u16 XoffLim;			 // transmit XOFF threshold 
	u8	ByteSize;			 // number of bits/Byte, 4-8 
	u8	Parity; 			 // 0-4=no,odd,even,mark,space 
	u8	StopBits;			 // 0,1,2 = 1, 1.5, 2 
	char XonChar;			 // Tx and Rx XON character 
	char XoffChar;			 // Tx and Rx XOFF character 
	char ErrorChar; 		 // error replacement character 
	char EofChar;			 // end of input character 
	char EvtChar;			 // received event character 
	u16  wReserved1;		 // reserved; do not use 
}TOspDCB; 

typedef struct
{ 
	u32 fCtsHold : 1; 
	u32 fDsrHold : 1; 
	u32 fRlsdHold : 1; 
	u32 fXoffHold : 1; 
	u32 fXoffSent : 1; 
	u32 fEof : 1; 
	u32 fTxim : 1; 
	u32 fReserved : 25; 
	u32 cbInQue; 
	u32 cbOutQue; 
} TOspComStat; 

#endif //#ifndef _MSC_VER


// 根据结点号，APP号和本地实例号构造全局实例ID
inline u32	MAKEIID( u16 app, u16 ins = 0 , u8 = 0 )
{
	return (app << 16)+ins;
}

// 从ID中获得应用号
inline u16 GETAPP( u32 GlobIID )
{
	return (u16)(GlobIID >> 16);
}

// 从ID中获得网关号
inline u8 GETGATE( u32 = 0 )
{
	return 0;
}

// 从ID中获得实例号
inline u16 GETINS( u32 GlobIID )
{
	return (u16)(GlobIID);
}

// 从ID中获得实例号的低字节
inline u8 GETINSL( u32 GlobIID )
{
	return (u8)(GlobIID);
}

// 从ID中获得实例号的高字节
inline u8 GETINSH(u32 GlobIID)
{
	return (u8)(GlobIID>>8);
}

//telnet终端提示符最大长度
#define MAX_PROMPT_LEN				21

// OSP错误码 
#define OSP_ERROR_BASE				0
//操作成功
#define OSP_OK						(OSP_ERROR_BASE + 0)
//操作失败
#define OSP_ERROR					(OSP_ERROR_BASE + 1)
//未使用
#define OSPERR_SEND_FAIL			(OSP_ERROR_BASE + 2)
//发送同步消息超时
#define OSPERR_SEND_TIMEOUT 		(OSP_ERROR_BASE + 3)
//同步消息应答超长
#define OSPERR_SYNCACK_EXCEED		(OSP_ERROR_BASE + 4)
//同一应用下实例别名已经存在
#define OSPERR_ALIAS_REPEAT 		(OSP_ERROR_BASE + 5)
//未使用
#define OSPERR_APP_REPEAT			(OSP_ERROR_BASE + 6)
//外部结点号无效
#define OSPERR_NODE_INVALID 		(OSP_ERROR_BASE + 7)
//参数无效
#define OSPERR_ILLEGAL_PARAM		(OSP_ERROR_BASE + 8)
//同步消息对端无应答
#define OSPERR_PEER_NOT_RESPOND 	(OSP_ERROR_BASE + 9)

//最大消息长度
#define MAX_MSG_LEN 				(u16)0xffff
//实例别名最大长度
#define MAX_ALIAS_LEN				(u8)255
//最大同步消息应答长度
#define MAX_SYNCACK_LEN 			MAX_MSG_LEN
//最大数据长度(未使用)
#define MAX_DATA_LEN				(MAX_MSG_LEN+MAX_ALIAS_LEN+MAX_SYNCACK_LEN)

//OSP消息类型
// 异步消息
#define MSG_TYPE_ASYNC				(u16)0
// 同步消息
#define MSG_TYPE_SYNC				(u16)1
// 同步应答消息
#define MSG_TYPE_SYNCACK			(u16)2
// 全局同步消息
#define MSG_TYPE_GSYNC				(u16)3
// 全局同步应答消息
#define MSG_TYPE_GSYNCACK			(u16)4
// 超时消息
#define MSG_TYPE_TIMEOUT			(u16)5

//切换实例状态机
#define NEXTSTATE(x)				NextState(x,#x)

//OSP 用户自定义消息事件号基准(小于该值的消息为OSP保留消息，上层禁止发送该消息)
#define   OSP_USEREVENT_BASE		(u16)0x0400
//定义消息事件号
#define OSPEVENT(x,y)		const u16 x = y

//未使用
OSPEVENT(OSP_CONFIRM,		0xff);
//启动断链检测
OSPEVENT(OSP_POWERON,		0x100);
//未使用
OSPEVENT(OSP_SWITCHMASTER,	0x101);
//未使用
OSPEVENT(OSP_SWITCHSLAVE,	0x102);
//服务端无空闲实例
OSPEVENT(OSP_OVERFLOW,		0x103);
//未使用
OSPEVENT(OSP_EXCEPTION_IND, 0x104);
//未使用
OSPEVENT(OSP_INSNOTEXIST,	0x105);
//与远端结点的TCP链路已断
OSPEVENT(OSP_DISCONNECT,	0x106);
//广播消息确认应答
OSPEVENT(OSP_BROADCASTACK,	0x107);
//未使用
OSPEVENT(OSP_NODECLOSE, 	0x108);
//断链检测探测消息
OSPEVENT(OSP_NETBRAECHO,	0x109);
//断链检测探测消息确认应答
OSPEVENT(OSP_NETBRAECHOACK, 0x110);
//OSP卸载消息
OSPEVENT(OSP_QUIT,			0x120);
//结点间通信测试消息
OSPEVENT(OSP_NETSTATEST,	0x121);
//结点间通信测试消息确认应答
OSPEVENT(OSP_NETSTATESTACK, 0x122);
//未使用
OSPEVENT(OSP_APPCONN_ACK,	0x123);


// Trc跟踪标志
typedef enum TRCOPTION
{
	// 跟踪状态迁移
	TRCSTATE	 = 1,
	// 跟踪事件
	TRCEVENT	 = 2,
	// 跟踪定时器
	TRCTIMER	 = 4,
}TRCOPTION;

//状态跃迁、事件、定时器均跟踪
#define TRCALL	   (u16)(TRCSTATE|TRCEVENT|TRCTIMER)

// 1.1版以后支持同步消息
#if (defined OSP_VERSION_ID) && (OSP_VERSION_ID >= 0x11)
	#define SYNCMSG
#endif

#ifdef _LINUX_
	/**
	 * 用于网络传输数据流的类
	 * 任何一个结构要在网络上传输，都写入一个数据流对象中，
	 * 然后把此数据流对象OSP，让OSP来传输
	 * 不要再直接使用传输一个结构
	 */
	class COspStream
	{
	public:
		//当前数据长度
		u32 m_dwCurrSize;
		//当前数据
		u8 m_achData[MAX_MSG_LEN];
		
		COspStream()
		{
			m_dwCurrSize = 0;
			memset(m_achData, 0, sizeof(m_achData));
		}

		u32  PackElement(const u8* pData, u32 dwLen);
		u32  ExtractElement(u8* pData, u32 dwLen);
	};

	#define PackElement2(data)	PackElement((u8*)&data, sizeof(data))
	#define ExtractElement2(data)	ExtractElement((u8*)&data, sizeof(data))

	// OSP消息头部结构
	class CMessage 
	{
	public:
		//源结点
		u32 srcnode;
		//目的结点
		u32 dstnode;
		//目的应用实例
		u32 dstid;
		//源目的实例
		u32 srcid;
		//消息类型
		u16 type;
		//消息号
		u16 event;
		//消息体长度
		u16 length;
		//消息体
		u8 *content;
		
		//如果支持同步消息
		#ifdef SYNCMSG
			//同步消息应答
			u8 *output;
			//同步消息应答长度
			u16 outlen;
			//未使用
			u16 expire;
		#endif
		
		//实例别名
		char *dstAlias;
		//实例别名长度
		u8 dstAliasLen;

		//得到OSP消息头部长度
		static u32 GetPackedSize()
		{
			return sizeof(u32) + sizeof(u32) + sizeof(u32) + sizeof(u32) + sizeof(u16) + 
					sizeof(u16) + sizeof(u16) + sizeof(u8 *) +
					#ifdef SYNCMSG
						sizeof(u8 *) + sizeof(u16) + sizeof(u16) +
					#endif
					sizeof(char *) + sizeof(u8);
		};

		//OSP消息头部打包
		void Pack(COspStream& tStream);
		//OSP消息头部解包
		void Extract(COspStream& tStream);
	};

#else
	// OSP消息头部结构
	typedef struct CMessage
	{
		//源结点
		u32 srcnode;
		//目的结点
		u32 dstnode;
		//目的应用实例
		u32 dstid;
		//源目的实例
		u32 srcid;
		//消息类型
		u16 type;
		//消息号
		u16 event;
		//消息体长度
		u16 length;
		//消息体
		u8 *content;
		
		//如果支持同步消息
		#ifdef SYNCMSG
			//同步消息应答
			u8 *output;
			//同步消息应答长度
			u16 outlen;
			//未使用
			u16 expire;
		#endif
		
		//实例别名
		char *dstAlias;
		//实例别名长度
		u8 dstAliasLen;
	}
	//紧排列
	#if defined(_VXWORKS_) || (defined(_LINUX_) && !defined(_EQUATOR_))
		__attribute__ ((packed)) 
	#endif
	CMessage;

#endif

//实例简要信息
typedef struct
{
    u32 state;
	u16 insid;	
	u16 evtrcv;
	u16 evtsnd;
}TInstInfo;
//单个应用中可保存的最大实例简要信息个数
#define MAX_INSTINFO_NUM		   100

//应用抽象基类，主要用于定义用户App模板zTemplate
#if defined(PWLIB_SUPPORT) && defined(_LINUX_)
	class CApp : public PThread
#else
	struct OSP_API CApp
#endif
{
#if defined(PWLIB_SUPPORT) && defined(_LINUX_)	
	PCLASSINFO(CApp, PThread);
#endif
	
public:
	//应用接收消息总数
	u32 msgIncome;
	//应用已处理消息总数
	u32 msgProcessed;
	//应用邮箱中待处理的消息总数(未使用)
	u32 msgWaiting;
	//应用邮箱中待处理的消息总数峰值
	u32 msgWaitingTop;
	//已激发的定时消息总数
	u32 timerProcessed;
	//应用邮箱最大消息容量
	u32 maxMsgWaiting;
	//因邮箱满丢弃的消息总数
	u32 msgdropped;
	//应用别名
	char *pAppName;

	//应用主邮箱写句柄
	u32 queSendId;
	//应用主邮箱读句柄
	u32 queRcvId;
	
	/* 为支持同步消息而增加的消息备份队列，存放同步消息发送后，应答超时消息
	   或应答消息到达之前的所有消息 */
	//应用备邮箱中的消息总数
	u32 bakQueMsgNum;
	//应用备邮箱写句柄
	u32 bakQueSendId;
	//应用备邮箱读句柄
	u32 bakQueRcvId;
	
	//应用任务ID
	u32 taskID;
	
	// 当前收到的消息
	CMessage *curMsgRcv;
	//屏幕trc标志
	u16 scrnTraceFlag;
	//文件trc标志
	u16 fileTraceFlag;
	//应用ID
	u16 appId;
	//应用前一个空闲实例号
	u16 wLastIdleInstID;
	// 当前发送的消息号
	u16 curEvtSnd;
    //屏幕log级别
	u8 scrnLogFlag;
	//文件log级别
	u8 fileLogFlag;	

	//应用任务句柄
	TASKHANDLE hAppTask;
	//应用任务信号量句柄
	SEMHANDLE tSemMutex;    

	//应用任务优先级
	u8 byAppPri;

	// 用于记录实例最近状态和收到数据的环行缓冲
	u8 byInstInfoHead;
	u8 byInstInfoTail;
	TInstInfo tInstInfo[MAX_INSTINFO_NUM]; 
	
public:
#if defined(_LINUX_) && defined(PWLIB_SUPPORT)
	CApp() : PThread(200<<10)
#else
	CApp()
#endif
	{
		msgIncome = 0;
		msgProcessed = 0;
		timerProcessed = 0; 	
		maxMsgWaiting = 0;
		msgdropped = 0;
		msgWaitingTop = 0;
		pAppName = NULL;
		queSendId = 0;
		queRcvId = 0;
		bakQueMsgNum = 0;
		bakQueSendId = 0;
		bakQueRcvId = 0;
		taskID = 0;
		scrnTraceFlag = 0;
		fileTraceFlag = 0;
		scrnLogFlag = 0;
		fileLogFlag = 0;
		appId = INVALID_APP;
		byAppPri = 100;
		wLastIdleInstID = 0;
		hAppTask = 0;
#ifndef _LINUX_
		tSemMutex = NULL;
#endif
		byInstInfoHead = 0;
		byInstInfoTail = 0; 	
		
		curMsgRcv = NULL;
		curEvtSnd = 0;
	}
	
	virtual ~CApp()
	{
		scrnTraceFlag = 0;
		fileTraceFlag = 0;
		scrnLogFlag = 0;
		fileLogFlag = 0;
	}
	
	//根据实例号查询指定实例
	virtual class CInstance* GetInstance(u16 insid) = 0;
	//设定实例别名
	virtual BOOL32 SetInstAlias(u16 insid, const char * pAlias, u8 len) = 0;
	//根据别名查询指定实例
	virtual class CInstance* FindInstByAlias(const char * pAlias, u8 len) = 0;
	//得到应用最大实例数
	virtual int GetInstanceNumber(void) = 0;
	//得到实例别名最大长度
	virtual u8 GetMaxAliasLen(void) = 0;
	//初始化实例别名记录信息
	virtual BOOL32	InitAliasArray(void) = 0;	
	//清除实例别名纪录信息
	virtual BOOL32	ClearInstAlias(u16 insid, const char * pAlias, u8 len) = 0;
	
	//启动应用
	// APP最高优先级为80，如果小于80，OSP将强制为80。此后, 用户可以调用OspSetAppPriority()或CApp::SetPriority()更改优先级.
	//成功返回OSP_OK      //操作成功    失败返回OSP_ERROR  //操作失败
	int CreateApp( char *name, u16 aid, u8 pri, u16 queuesize = 80, u32 uStackSize = 200<<10 );
	//设定应用任务优先级
	BOOL32 SetPriority(u8 byPrior);
	//查询任务优先级
	u8 GetPriority(void)
	{
		return byAppPri;
	}
	//应用退出
	void QuitApp(void);

	//应用备邮箱消息总数增加减少查询
	BOOL32 BakMsgNumInc(void);
	BOOL32 BakMsgNumDec(void);
	u32 GetBakMsgNum(void);

	//应用主邮箱消息总数查询增加减少
	u32 GetMsgIncomeNum(void);
	void MsgIncomeNumInc(void);
	void MsgIncomeNumDec(void);

	//应用处理消息总数增加
	void MsgProcessedNumInc(void);

	//查询待处理的消息总数
	u32 GetMsgWaitingNum(void);

	//应用别名设定查询
	void SetName(const char * pName);
	char *NameGet(void);

	//定时激发次数增加
	void TimerProcessedIncrease(void);

	//设定文件屏幕log级别
	void LogLevelSet(u8 file_level, u8 screen_level);
	//设定文件屏幕trc标志
	void TrcFlagSet(u16 file_flag, u16 screen_flag);

	//实例简要信息打印
	void InstInfoShow(void);
	//实例简要信息增加
	void InstInfoAdd(u16 ins, u32 state, u16 evtrcv, u16 evtsnd);

#if defined(_LINUX_) && defined(PWLIB_SUPPORT)
	void Main(void);  // pwlib pure virtual function
#endif
};


//实例定时器信息
typedef  struct TInstTimerInfo
{
	TInstTimerInfo *next ;
	u32 timerId ;
	void *timerBlkAdrr ;
}TInstTimerInfo;


struct TmBlk // 定时控制块
{
    TmBlk * suc; // 下一个时间控制块。
    TmBlk * pre; // 上一个时间控制块。
    u64 tick;
    u16 appId;
    u16 instId;
    u16 timerId;
    u32 param;
    u32 settedTick;
    u16 timeToLeft;
    time_t absTime;      //绝对定时到时时间
	BOOL bExtendMode;    //是否使用扩展模式(默认不使用)
	u8  *pExtContent;    //扩展内容
	u16  dwExtLength;    //扩展内容的长度
};


#define  TIMERHANDLE  TmBlk*


//CInstance抽象基类规范。
//基于Osp的应用必须从该CInstance基类派生一个具体的Instance类，
//并给出一个InstanceEntry()成员函数的具体实现
class OSP_API CInstance
{
public:
	enum
	{
		//空闲实例
		PENDING,
		//广播
		EACH = 0xffff,
		//广播且要求确认应答
		EACH_ACK = 0xfffe,
		//未使用
		CONN_ACK = 0xfffd,
		//守护实例
		DAEMON = 0xfffc,
		//目的实例实例号无效，别名有效
		INVALID = 0xfffb
	};
	
public:
	//应用号
	u16 m_appId;
	//实例号
	u16 m_instId;
	//实例别名
	char m_alias[MAX_ALIAS_LEN];
	//实例别名长度
	u8 m_aliasLen;
	//实例别名最大长度
	u8 m_maxAliasLen;
	//当前状态机
	u32 m_curState;
	//上一个状态机
	u32 m_lastState;
	
public:
	//守护实例入口函数
	virtual void DaemonInstanceEntry( CMessage *const, CApp* )
	{
		log( 0, "Osp: message received in default daemon instance of app %d.\nCheck your daemon implementation, please!\n", GetAppID() );
	}
	//实例入口函数
	virtual void InstanceEntry( CMessage *const pMsg) = 0;
	//实例打印函数
	virtual void InstanceDump(u32=0)
	{
	}
	//守护实例打印函数
	virtual void DaemonInstanceDump(u32=0)
	{
	}
	//实例退出
	virtual void InstanceExit(void)
	{
	}
	
	//设定实例别名
	int SetAlias(const char * pchAlias, u8 len);
	//删除实例别名
	void DeleteAlias(void);
	//查询实例别名
	BOOL32 GetAlias(char *aliasBuf, u8 bufLen, u8 *aliasLen);	
	//查询实例别名长度
	u8 GetAliasLen(void);
	
	//查询上一个实例状态机
	u32 		LastState();
	//查询当前实例状态机
	u32 		CurState();
	
	//查询应用号
	u16 		GetAppID();
	//查询实例号
	u16 		GetInsID();

	//定时器信号量
	SEMHANDLE	   m_tSemTimerList;
	//定时器信息链表
	TInstTimerInfo	m_timerInfoListHead;

	//加入定时器信息
	void AddInstTimerInfo(u32 nTimerNo , void * pTimerAddr);
	//删除定时器信息
	void DelInstTimerInfo(u32 nTimerNo);
	//查询定时器信息
	void *FindInstTimerInfo(u32 nTimerNo);
	//删除所有定时器信息
	void DelAllInstTimerInfo();
	//设置相对定时
	int SetTimer(TIMERHANDLE *pTimerHandle, u32 dwTimer, long uMilliSeconds, const void* content, u16 length );
    //取消定时
    int KillTimer(TIMERHANDLE hTimerHandle);
protected:
	//设置相对定时
	int SetTimer( u32 nTimer, long nMilliSeconds, u32 nPara = 0 );
	//设置绝对定时
	int SetAbsTimer( u32 nTimer, u16 nYear, u8 nMon, u8 nDay, u8 nHour, u8 nMin, u8 nSec, u32 nPara = 0 );
	//取消定时
	int KillTimer(u32 nTimerNo);
	// 取消绝对定时器
	int KillAbsTimer( u32 nTimerNo);
	//实例打印,可调用OspSetLogLevel修改打印级别
	void log(u8 level, char * format,...);
	//切换状态机
	void NextState(u32 state, char *s = 0 );
	
	//发送异步消息(目的实例采用实例号标识)
	int post(u32 dstiid, u16 event, const void *content=0, u16 length=0, u32 dstnode=0);
	//发送异步消息(目的实例采用实例别名标识)
	int post(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event, 
		const void* content=0, u16 length=0, u32 dstnode=0);

	//发送同步消息(目的实例采用实例号标识)
	int send(u32 dstiid, u16 event, const void *content=0, u16 length=0, u32 dstnode=0, 
		void* ackbuf=NULL, u16 ackbuflen=0, u16 *realacklen=NULL, 
		u16 timeout=1000);
	//发送同步消息(目的实例采用实例别名标识)
	int send(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event, 
		const void* content=0, u16 length=0, u32 dstnode=0,
		void* ackbuf=NULL, u16 ackbuflen=0, u16 *realacklen=NULL, 
		u16 timeout=1000);

	//发送同步消息应答
	int SetSyncAck(const void* ack=NULL, u16 ackLen=0);
	//发送同步消息应答(建议使用SetSyncAck)
	int reply(const void* ack=NULL, u16 ackLen=0); 
};

//用于派生用户App类的模板
class CAppNoData
{
};

//实例别名快速查找记录数据
typedef struct TInstAliasInfo
{
	//实例别名的设置值(也是存放的位置下标)
	u32  aliasRet;
	//相同设置值的下一个实例存放位置
	u32  nextLoc;
	//实例号
	u16  instId;
}TInstAliasInfo;

//应用模板(模板参数为实例的继承类以及最大实例实例数)
template <class I, int maxins=1, class A=CAppNoData, u8 maxAliasLen=0>
class zTemplate:public CApp, public A
{
public:
	//设置实例别名
	BOOL32 SetInstAlias(u16 insid, const char * pchAlias, u8 len);
	//清除实例别名纪录信息
	BOOL32	ClearInstAlias(u16 insid, const char * pAlias, u8 len);
	
private:
	 //由局部实例号查找实例
	CInstance* GetInstance(u16 insid);
	//由别名查找实例
	CInstance* FindInstByAlias(const char * pchAlias, u8 len);
	//获得普通实例个数
	int GetInstanceNumber(void);
	//获得最大别名长度
	u8 GetMaxAliasLen(void);
	
private:
	//daemon实例
	I daemIns;
	//普通实例表
	I insarray[maxins];
	
	//实例别名快速查找
	//实例别名记录信息链头表
	TInstAliasInfo instAliasArray[maxins];
	//实例别名记录信息备用表，记录设置相同的实例
	TInstAliasInfo bakInstAliasArray[maxins];
	//当前备用表中记录的实例个数
	u32 curBakArrayNum;
	
	//初始化实例别名记录信息
	BOOL32 InitAliasArray();
	//实例别名转换成WORD32型整数
	u32 AliasToWord(char * pchAlias, u8 len);
	//确定实例别名信息存放在主用表还是在备用表中
	BOOL32 IsInBakArray(u32 setRet);
};

/*=============================================================================
函 数 名：OspLog
功	  能：全局log函数。这个函数的开关是应用号0。调用OspSetLogLevel(0,xx)就可以
控制该函数的打开或关闭
注	  意：
算法实现：
全局变量：
参	  数：u8 level : [in] log level
char *format : [in] message format
返 回 值：void
-------------------------------------------------------------------------------
修改记录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspLog(u8 level, char *format, ...);

/*=============================================================================
函 数 名：OspMsgDumpSet
功	  能：set long msg dump detail can/not be whole print, default is printf
10lines
注	  意：
算法实现：
全局变量：
参	  数：BOOL32 LgMsgDumpEnbl : [in] enable or not
返 回 值：void
-------------------------------------------------------------------------------
修改记录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspMsgDumpSet(BOOL32 LgMsgDumpEnbl);

/*=============================================================================
函 数 名：OspStatusMsgOutSet
功	  能：set status message can or not be out put every timer
注	  意：
算法实现：
全局变量：
参	  数：BOOL32 OutMsgEnable : [in] enable or not
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspStatusMsgOutSet(BOOL32 OutMsgEnable);

/*====================================================================
函数名：OspPrintf
功能：把相应的内容显示到屏幕,存储到文件,不能屏蔽
算法实现：（可选项）
引用全局变量：
输入参数说明：
bScreen: 是否输出到屏幕,
bFile: 是否输出到文件,
szFormat: 格式,
返回值说明：
====================================================================*/
API void OspPrintf(BOOL32 bScreen,BOOL32 bFile, char * szFormat,...);


/*====================================================================
函数名：OspTelAuthor
功能：设置Telnet的授权用户名和密码(vxworks下用户名和密码为空会设置失败)
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明：
====================================================================*/
API void OspTelAuthor(const char * szUsername = NULL ,const char * szPassword = NULL );

/*=============================================================================
函 数 名：OspVerPrintf, OspVer
功	  能：output compile version info to screen
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
#define  OspVerPrintf() \
OspPrintf(TRUE, TRUE, "Osp version: %s  compile time: %s %s\n", OSPVERSION, __DATE__, __TIME__)
API void ospver();

/*=============================================================================
函 数 名：OspCat
功	  能：output file content to screen
注	  意：
算法实现：
全局变量：
参	  数：const char*  fname : [in] file to be opened
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspCat(const char*  fname);

#ifdef _LINUX_
typedef void (*POspQuitFunc)(int);
/*====================================================================
  函数名：OspRegQuitFunc
  功能：注册任务退出函数，仅linux下有效
  算法实现：
  引用全局变量：
  输入参数说明：
  	pQuitFunc	- 任务退出函数
  返回值说明：TRUE - 注册成功
  ====================================================================*/
API BOOL32 OspRegQuitFunc( POspQuitFunc pQuitFunc );

/*====================================================================
  函数名：OspRegTaskInfo
  功能：注册任务信息，仅linux下有效
  算法实现：
  引用全局变量：
  输入参数说明：
  	dwTaskId	- 任务号
		szName	- 任务别名
  返回值说明：无
  ====================================================================*/
API void OspRegTaskInfo(u32 dwTaskId, const char* szName);

/*====================================================================
  函数名：OspUnRegTaskInfo
  功能：反注册任务信息
  算法实现：
  引用全局变量：
  输入参数说明：
  	dwTaskId	- 任务号
  返回值说明：无
  ====================================================================*/
API BOOL32 OspUnRegTaskInfo(u32 dwTaskId);

/*====================================================================
  函数名：OspTaskShow
  功能：打印任务信息，仅linux下有效
  算法实现：
  引用全局变量：
  输入参数说明：
  返回值说明：无
  ====================================================================*/
API void OspTaskShow();

/*====================================================================
  函数名：OspRegCommand
  功能：注册可以在Telnet下执行的函数(该函数仅在linux下有效，windows下不需注册)
  算法实现：
  引用全局变量：
  输入参数说明：
  	szName	- Telnet下输入的命令
	pfFunc	- 对应的函数指针
	szUsage	- 命令的帮助信息
  返回值说明：无
  ====================================================================*/
API void OspRegCommand(const char* szName, void* pfFunc, const char* szUsage);
#endif

/*=============================================================================
函 数 名：OspNodeConnTest
功	  能：test a node's network connection
注	  意：
算法实现：
全局变量：
参	  数：u32 NodeId : [in] id of the node
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspNodeConnTest(u32 NodeId);

/*=============================================================================
函 数 名：OspNodeIpGet
功	  能：获得指定结点的Ip地址
注	  意：
算法实现：
全局变量：
参	  数：dwNodeId : [in] node id

 返 回 值：成功返回结点IP, 失败返回0.
=============================================================================*/
API u32 OspNodeIpGet(u32 dwNodeId);

/*=============================================================================
  函 数 名：OspNodeLocalIpGet
  功    能：获取节点的本地地址
  注    意：对于已经连接的tcp节点，在本地存在多个地址的情况下，需要知道对方实际连接的本地ip地址。
  算法实现：
  全局变量：
  参    数：dwNodeId : [in] node id

  返 回 值：成功返回结点IP, 失败返回0.
  =============================================================================*/
API u32 OspNodeLocalIpGet(u32 dwNodeId);

/*====================================================================
函数名： OspInstShow
功能：显示某个App中的Instance的状态信息
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspInstShow(u16 aid);

/*=============================================================================
函 数 名：OspInstShowAll
功	  能：显示所有Instance的状态信息
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspInstShowAll(void);

/*=============================================================================
函 数 名：OspInstDump
功	  能：显示某一instance的自定义信息，调用该instance的dump函数。
注	  意：
算法实现：
全局变量：
参	  数：aid : [in] app id
InstId : [in] instance id, 0:显示所有的，-1:显示非IDLE的
param : [in] reserved parameter
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspInstDump(u16 aid, u16 InstId, u32 param);

/*====================================================================
函数名：OspNodeShow
功能：显示所有Node的状态信息
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspNodeShow(void);

/*=============================================================================
函 数 名：OspAppShow
功	  能：显示App的状态信息
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspAppShow();

/*=============================================================================
函 数 名：OspDispatchShow
功	  能：显示Dispatch task的信息
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspDispatchShow();

/*====================================================================
函数名：OspTimerShow
功能：显示timer状态信息
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspTimerShow(void);

/*====================================================================
函数名：OspLogShow
功能：显示日志模块状态
算法实现：（可选项）
引用全局变量:
输入参数说明：
返回值说明: 无
====================================================================*/
API void OspLogShow(void);

/*====================================================================
函数名：OspMemShow
功能：	显示内存管理堆栈中消息块数目
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspMemShow(void);

/*=============================================================================
函 数 名：OspAppEchoTest
功	  能：Application echo testing
注	  意：
算法实现：
全局变量：
参	  数：u16 AppId : [in] Application id
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspAppEchoTest(u16 AppId);

/*=============================================================================
函 数 名：OspAddEventDesc
功	  能：用于将事件的描述登记到消息跟踪系统
注	  意：
算法实现：
全局变量：
参	  数：const char *	desc : [in]
u16 event : [in]
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspAddEventDesc (const char *  desc, u16 event);

/*=============================================================================
函 数 名：OspSendTrcOn
功	  能：打开OSP结点发送跟踪标识
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspSendTrcOn();

/*=============================================================================
函 数 名：OspSendTrcOff
功	  能：关闭OSP结点发送跟踪标识
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspSendTrcOff();

/*=============================================================================
函 数 名：OspRcvTrcOn
功	  能：打开OSP结点接收跟踪标识
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspRcvTrcOn();

/*=============================================================================
函 数 名：OspRcvTrcOff
功	  能：关闭OSP结点接收跟踪标识
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspRcvTrcOff();

/*====================================================================
函数名：OspTrcAllOn
功能：打开OSP所有应用的跟踪标设
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspTrcAllOn(void);

/*====================================================================
函数名：OspTrcAllOn
功能：打开OSP所有应用的跟踪标设
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspTrcAllOff(void);

/*=============================================================================
函 数 名：OspOpenLogFile
功	  能：打开日志文件
注	  意：
算法实现：
全局变量：
参	  数：const char *	name : [in] file name
u32 maxsize_KB : [in] 文件大小限制
u32 maxfiles : [in]文件数目限制
返 回 值：BOOL32 - true 成功, false 失败
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspOpenLogFile(const char *  name, u32 maxsize_KB, u32 maxfiles);

/*=============================================================================
函 数 名：OspCloseLogFile
功	  能：关闭日志文件。
注	  意：如果调用OspQuit，则不需要调用该函数。
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspCloseLogFile(void);

/*=============================================================================
函 数 名：OspSetLogLevel
功	  能：控制一个APP的CInstance::log函数的级别
注	  意：必须在OspInit之后调用该函数才能生效。OspInit将所有应用的该标志置0
算法实现：
全局变量：
参	  数：aid : [in] app id
file_level : [in] file level
screen_level : [in] screen level
if level=5, 那么0~5 打印,  6~255 不打印,设置为3,则只有log(0|1|2,"")
的调用能够输出。
返 回 值：
=============================================================================*/
API void OspSetLogLevel(u16 aid, u8 file_level, u8 screen_level);

/*====================================================================
函数名：OspSetFileLogLevel	
功能：设置App的文件日志等级
算法实现：（可选项）
引用全局变量：
输入参数说明：wAid App的等级
byFileLevel: 文件Log的等级
返回值说明：
====================================================================*/
API void OspSetFileLogLevel(u16 wAid, u8 byFileLevel);

/*====================================================================
函数名：OspSetScrnLogLevel	
功能：设置App的屏幕日志等级
算法实现：（可选项）
引用全局变量：
输入参数说明：
uAid App的等级
uScrnLevel: 屏幕Log的等级
返回值说明：
====================================================================*/
API void OspSetScrnLogLevel(u16 wAid, u8 byScrnLevel);

/*=============================================================================
函 数 名：OspSetTrcFlag
功	  能：设置每个应用的跟踪范围。Trace 分为 Trace 状态机 1: 发消息, 2: 收消息
4: 设定时 8:收定时 16:状态跃迁
注	  意：
算法实现：
全局变量：
参	  数：aid : [in] app id
file_flag : [in] trace file flag
screen_flag : [in] screen output flag
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspSetTrcFlag(u16 wAid, u16 wFileFlag, u16 wScrnFlag);

/*====================================================================
函数名：OspSetFileTrcFlag
功能：设置某个应用的跟踪标志
算法实现：（可选项）
引用全局变量：
输入参数说明：uAid appl 标设
uFileFlag: 文件跟踪标设
返回值说明:
====================================================================*/
API void OspSetFileTrcFlag(u16 wAid, u16 wFileFlag);

/*====================================================================
函数名：OspSetScrnTrcFlag
功能：设置某个应用的跟踪标志
算法实现：（可选项）
引用全局变量：
输入参数说明：uAid application 标设
uScreenFlag: 屏幕跟踪标设
返回值说明:
====================================================================*/
API void OspSetScrnTrcFlag(u16 wAid, u16 wScreenFlag);

/*=============================================================================
函 数 名：OspRegistModule
功	  能：向OSP登记一个模块
注	  意：
算法实现：
全局变量：
参	  数：(in)pchModuleName - 模块（.exe文件）名，以NULL结尾的字符串，
如"kdvmt.dll"等
返 回 值：成功返回TRUE，失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspRegistModule(const char * szModuleName);

/*=============================================================================
函 数 名：OspSetLogEventDataLength
功	  能：设置全局的消息跟踪显示的长度。
注	  意：
算法实现：
全局变量：
参	  数：u16 wLength : [in] length of log event data
返 回 值：u16 previous length
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API u16 OspSetLogEventDataLength(u16);

/*=============================================================================
函 数 名：OspEnableBroadcastAck
功	  能：大多数情况下，用户发送广播消息并不需要对方返回确认消息。缺省情况下，
PIS不向消息的发送者发送OSP_BROADCASTACK消息，但是可以通过该函数修改这
个选项。是否发送OSP_BROADCASTACK是由接受节点决定的。
注	  意：
算法实现：
全局变量：
参	  数：u16 aid : [in] 表示广播消息的接受者AID。
BOOL32 bEnable : [in] enable or not
返 回 值：BOOL32 - true Succeeded, false Failed
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspEnableBroadcastAck(u16, BOOL32);

/*=============================================================================
函 数 名：OspGetNodeAddr
功	  能：查找Osp结点地址（本端与远端IP+Port）。
注	  意：
算法实现：
全局变量：
参	  数：u32 dwNodeId : [in] 结点ID
					TOspNodeAddr* ptOspNodeAddr : [out] 结点地址
返 回 值：FALSE - 查询成功
					TRUE - 查询失败
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2006/08/10	4.0
=============================================================================*/
typedef struct
{
	u32 m_dwLocalIP;//网络序
	u32 m_dwPeerIP;//网络序
    u16 m_wLocalPort;//主机序
	u16 m_wPeerPort;//主机序
}TOspNodeAddr;
API BOOL32 OspGetNodeAddr( u32 dwNodeId , TOspNodeAddr* ptOspNodeAddr );

#if defined(_MSC_VER) || defined(_LINUX_)

//sock5代理版本号
#define SOCK5_PROXY_VERSION			(u8)0x05
//客户端支持鉴权方法
#define SOCK5_PROXY_AUTHEN_NO_REQUIERD			(u8)0x00
#define SOCK5_PROXY_AUTHEN_USERNAME_PASSWORD	(u8)0x02
//操作结果
#define SOCK5_PROXY_SUCCESS						(u8)0x00
#define SOCK5_PROXY_AUTHEN_NO_ACCEPTABLE		(u8)0xff
//请求命令
#define SOCK5_PROXY_CMD_TCP_CONNECT				(u8)0x01
#define SOCK5_PROXY_CMD_TCP_BIND				(u8)0x02
#define SOCK5_PROXY_CMD_UDP_ASSOCIATE			(u8)0x03
//保留字段
#define SOCK5_PROXY_RESERVED_DATA				(u8)0x00
//地址类型
#define SOCK5_PROXY_IPV4_ADDR					(u8)0x01
#define SOCK5_PROXY_DOMAIN_NAME					(u8)0x03
#define SOCK5_PROXY_IPV6_ADDR					(u8)0x04


#define MAX_SOCK5PROXY_AUTHEN_NUM	(u8)0xff
#define MAX_SOCK5PROXY_NAME_PASS_LENGTH	(u8)64
typedef struct 
{
	u32 m_dwProxyIP;
	u16 m_wProxyPort;
	u8 m_byAuthenNum;
	u8 m_abyAuthenMethod[MAX_SOCK5PROXY_AUTHEN_NUM];
	s8 m_achUseName[MAX_SOCK5PROXY_NAME_PASS_LENGTH+1];
	s8 m_achPassword[MAX_SOCK5PROXY_NAME_PASS_LENGTH+1];
}TOspSock5Proxy;

/*=============================================================================
函 数 名：OspConnectTcpNodeThroughSock5Proxy
功	  能：TCP穿越sock5代理连接服务端（与OspConnectTcpNode功能相同，上层不需要调用OspConnectToSock5Proxy与代理建链）
注	  意：
算法实现：
全局变量：
参	  数：ptOspSock5Proxy : [in] 代理服务器信息结构;
		  dwServerIP : [in] 服务器IP
		  wServerPort : [in] 服务器端口
		  wHb: [in] 断链检测周期(秒)
		  byHbNum: [in] byHbNum次未到连接检测应答后认为链路已断开
		  dwTimeoutMs : [in] 操作超时时间
		  pdwLocalIP: [in,out] 本TCP链接使用的本地IP
返 回 值：失败返回INVALID_NODE；
		  成功返回一个动态分配的结点号, 此后用户可借此与服务结点通信
		  上层主动断链需调用OspDisconnectTcpNode，所有与无代理时均无区别
-------------------------------------------------------------------------------
 修改纪录：
 日 	 期  版本  修改人  修改内容
 2006/08/21  4.0   王小月
=============================================================================*/
API int OspConnectTcpNodeThroughSock5Proxy( TOspSock5Proxy* ptOspSock5Proxy , u32 dwServerIP, u16 wServerPort,
									u16 wHb = 10, u8 byHbNum = 3 , u32 dwTimeoutMs = 0 , u32 *pdwLocalIP = NULL, u8 bySocksAuthVerion = SOCK5_PROXY_VERSION );

/*=============================================================================
函 数 名：OspConnectToSock5Proxy
功	  能：与sock5代理服务器建立TCP链路（UDP穿越sock5代理必须先建立一条TCP链路）
注	  意：
算法实现：
全局变量：
参	  数：ptOspSock5Proxy : [in] 代理服务器信息结构
		  dwTimeoutMs : [in] 操作超时时间
返 回 值：失败返回INVALID_SOCKET；
		  成功返回与代理通信的TCP Socket，可进一步调用OspUdpAssociateThroughSock5Proxy建立UDP Associate；
		  可复用，需要上层维护本TCP连接，链路段无效时，Udp Associate也应视为无效，
		  主动断链请调用OspDisconnectFromSock5Proxy；
-------------------------------------------------------------------------------
 修改纪录：
 日 	 期  版本  修改人  修改内容
 2006/08/21  4.0   王小月
=============================================================================*/
API SOCKHANDLE OspConnectToSock5Proxy( TOspSock5Proxy* ptOspSock5Proxy , u32 dwTimeoutMs = 0 , u8 bySocksAuthVerion = SOCK5_PROXY_VERSION);

/*=============================================================================
函 数 名：OspUdpAssociateThroughSock5Proxy
功	  能：UDP穿越sock5代理
注	  意：
算法实现：
全局变量：
参	  数：hSocket : [in] OspConnectToSock5Proxy返回的socket(可复用);
		  dwLocalIP : [in] 本地收发socket IP，以便代理服务器限制数据穿越（网络序）
		  wLocaPort : [in] 本地收发socket 端口，以便代理服务器限制数据穿越（主机序）
		  pdwProxyMapIP : [out] 代理服务器映射IP（网络序）
		  pwProxyMapPort : [out] 代理服务器映射端口（主机序）
		  dwTimeoutMs : [in] 操作超时时间
返 回 值：失败返回FALSE；
		  成功返回TRUE
-------------------------------------------------------------------------------
 修改纪录：
 日 	 期  版本  修改人  修改内容
 2006/08/21  4.0   王小月
=============================================================================*/
API BOOL32 OspUdpAssociateThroughSock5Proxy( SOCKHANDLE hSocket , u32 dwLocalIP, u16 wLocalPort ,
									u32* pdwProxyMapIP, u16* pwProxyMapPort , u32 dwTimeoutMs = 0 );

/*=============================================================================
函 数 名：OspDisConnectFromSock5Proxy
功	  能：断开与sock5代理建立的用于UDP Associate的TCP连接
注	  意：
算法实现：
全局变量：
参	  数：hSocket : [in] OspConnectToSock5Proxy返回的socket;
返 回 值：失败返回FALSE；
		  成功返回TRUE
-------------------------------------------------------------------------------
 修改纪录：
 日 	 期  版本  修改人  修改内容
 2006/08/21  4.0   王小月
=============================================================================*/
API BOOL32 OspDisConnectFromSock5Proxy( SOCKHANDLE hSocket );

#endif

/*=============================================================================
函 数 名：OspCreateTcpNode
功	  能：创建一个TCP节点。这以后，这个节点可以被连接。请使用6682作为port参数。
注	  意：
算法实现：
全局变量：
参	  数：u32 dwAddr : [in] reserved
u16 wPort : [in] which port to use
bTcpNodeReuse:[in]创建的node退出后端口是否能立即重用或被其他程序马上占用 
返 回 值：int - If no error occurs, CreateTcpNode returns a descriptor
referencing the new socket. Otherwise, a value of
INVALID_SOCKET is returned.
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API int OspCreateTcpNode(u32 dwAddr, u16 wPort, BOOL32 bTcpNodeReuse=FALSE);

/*=============================================================================
函 数 名：OspConnectTcpNode
功	  能：在地址uIpv4Addr和端口uTcpPort上连接服务结点, 并设置断链检测参数.
注	  意：
算法实现：
全局变量：g_Osp
参	  数：uIpv4Addr : [in] 服务结点的IP地址,
wTcpPort : [in] 服务结点的侦听端口号,
wHb: [in] 断链检测周期(秒),
byHbNum: [in] byHbNum次未到连接检测应答后认为链路已断开,
dwTimeOutMs: [in] 如果在dwTimeOutMs毫秒内连接不成功, 返回出错. 缺省值0, 
表示取connect超时的缺省时间(75s).  该参数目前仅在VxWorks下有效.
pdwLocalIP: [out] 当前连接所用的IP地址

 返 回 值：成功返回一个动态分配的结点号, 此后用户可借此与服务结点通信. 
 失败返回INVALID_NODE.
 -------------------------------------------------------------------------------
 修改纪录：
 日 	 期  版本  修改人  修改内容
 2003/03/12  2.0
=============================================================================*/
API int OspConnectTcpNode(u32 dwIpv4Addr, u16 wTcpPort, u16 wHb=10, u8 byHbNum=3, u32 dwTimeoutMs=0, u32* pdwLocalIP=NULL);

/*=============================================================================
函 数 名：OspSetHBParam
功	  能：设置结点的断链检测参数
注	  意：
算法实现：
全局变量：
参	  数：dwNodeID: 结点号,
wHb: 链路检测周期(以秒为单位),
byHbNum: 如果连续ping了uHbNum次没有应答, 主动断开.

 返 回 值：成功返回TRUE, 失败返回FALSE.
 -------------------------------------------------------------------------------
 修改纪录：
 日 	 期  版本  修改人  修改内容
 2003/06/5	 2.1   
=============================================================================*/
API BOOL32 OspSetHBParam(u32 dwNodeID, u16 wHb, u8 byHbNum);

/*=============================================================================
函 数 名：OspNodeDiscCBReg
功	  能：设置在node连接中断时需通知的appid和InstId
注	  意：目前支持通知多个实例，每次调用该函数，则只通知指定实例，其他已注册待
　　　　　通知实例将被清除。
算法实现：
全局变量：
参	  数：dwNodeId : [in] node id
wAppId : [in] application id
wInsId : [in] instance id
返 回 值：成功返回OSP_OK，失败参见错误码
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API int OspNodeDiscCBReg(u32 dwNodeId, u16 wAppId, u16 wInsId);

/*=============================================================================
函 数 名：OspNodeDiscCBRegQ
功	  能：增加在node连接中断时需通知的appid和InstId
注	  意：每次调用此函数，则增加一个要通知的实例。除非达到最大个数
算法实现：
全局变量：
参	  数：dwNodeId : [in] node id
wAppId : [in] application id
wInsId : [in] instance id
返 回 值：成功返回OSP_OK，失败参见错误码
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API int OspNodeDiscCBRegQ(u32 dwNodeId, u16 wAppId, u16 wInsId);

/*=============================================================================
函 数 名：OspDisconnectTcpNode
功	  能：断开在一个node上的连接。
注	  意：
算法实现：
全局变量：
参	  数：u8 node_id : [in] node id
返 回 值：BOOL32 - true Succeeded, false Failed
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspDisconnectTcpNode(u32 nodeId);


#ifdef MULTI_LISTEN_SOCKET_SUPPORT
/*=============================================================================
函 数 名：OspCloseTcpNode
功	  能：关闭一个套接字的监听，对应于OspCreateTcpNode创建监听(目前仅linux下有效)。
注	  意：
算法实现：
全局变量：
参	  数：int nSocketFd : [in] 套接字描述符，一般为OspCreateTcpNode的返回值
返 回 值：BOOL32 - true Succeeded, false Failed
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspCloseTcpNode(int nSocketFd);

#endif //MULTI_LISTEN_SOCKET_SUPPORT


#ifdef _LINUX_
/*=============================================================================
函 数 名：OspDisconnectAllTcpNode
功	  能：停止当前的监听,并断开其上创建全部连接(目前仅linux下有效)。
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2006/12/06	1.0   罗健锋
=============================================================================*/
API BOOL32 OspDisconnectAllTcpNode(void);
#endif

/*=============================================================================
函 数 名：OspIsValidTcpNode
功	  能：判断一个TCP节点是否有效。
注	  意：
算法实现：
全局变量：
参	  数：u32 node_id : [in] node id
返 回 值：BOOL32 - Valid or not
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspIsValidTcpNode (u32 node_id);

/*====================================================================
函数名：OspIsNodeCheckEnable
功能：判断指定结点的链路检测功能是否起用。
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspIsNodeCheckEnable(u32 dwNodeId);

/*====================================================================
函数名：OspEnableNodeCheck
功能：起用指定结点的链路检测功能。
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspEnableNodeCheck(u32 dwNodeId);

/*====================================================================
函数名：OspDisableNodeCheck
功能：禁用指定结点的链路检测功能。
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspDisableNodeCheck(u32 dwNodeId);

/*====================================================================
函数名：OspSetAppPriority
功能：设置App的调度优先级。
算法实现：（可选项）
引用全局变量：
输入参数说明：wAppId -- 目标App号,
byPrior -- 优先级.

 返回值说明：成功, 返回TRUE; 失败, 返回FALSE.
====================================================================*/
API BOOL32 OspSetAppPriority(u16 wAppId, u8 byPrior);

/*====================================================================
函数名：OspGetAppPriority
功能：获得指定App的调度优先级。
算法实现：（可选项）
引用全局变量：
输入参数说明：wAppId -- (in)目标App号,
pbyPrior -- (out)优先级.

 返回值说明：成功, 返回TRUE; 失败, 返回FALSE.
====================================================================*/
API BOOL32 OspGetAppPriority(u16 wAppId, u8* pbyPrior);

/*=============================================================================
函 数 名：OspIsLocalHost
功	  能：判断是否是本机IP，如OspIsLocalHost( inet_addr( "127.0.0.1" ) );
注	  意：如果是在Windows下使用本函数，必须先调用OspInit，或者WSAStartup。
本函数效率较低，不要重复调用。
算法实现：
全局变量：
参	  数：u32 dwIP : [in] 需要判断的IP
返 回 值：BOOL32	是否为本机IP
-------------------------------------------------------------------------------
修改记录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspIsLocalHost( u32 dwIP );

/*=============================================================================
函 数 名：OspPost
功	  能：发送异步消息
注	  意：
算法实现：
全局变量：
参	  数：u32 dstid : [in] destination info
u16 event : [in] OSP_ event
const void* content : [in] message data
u16 length : [in] sizeof content in u8s
u32 dstnode : [in] destionation socket node
u32 srcGlobIID : [in] source info
u32 srcnode : [in] source node
BOOL32 bDroppable : [in] drop the message when message queue full
返 回 值：int - 0 Succeeded, others Failed
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API int OspPost(u32 dstid, u16 event, const void* content=0, u16 length=0,
				u32 dstnode=0, u32 srciid=MAKEIID(INVALID_APP, INVALID_INS), u32 srcnode=INVALID_NODE, int timeout=2000); 

/* used only in C++ environment */
int OspPost(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event, 
			const void* content=0, u16 length=0, u32 dstnode=0, u32 srciid=MAKEIID(INVALID_APP, INVALID_INS), u32 srcnode=INVALID_NODE);

			/*=============================================================================
			函 数 名：OspSend
			功	  能：全局同步消息发送
			注	  意：
			算法实现：
			全局变量：
			参	  数：u32 dstid : [in] destination info
			u16 event : [in] OSP_ event
			const void* content : [in] message data
			u16 length : [in] sizeof content in u8s
			u32 dstnode : [in] destionation socket node
			u32 srcGlobIID : [in] source info
			u32 srcnode : [in] source node
			BOOL32 bDroppable : [in] drop the message when message queue full
			返 回 值：int - 0 Succeeded, others Failed
			-------------------------------------------------------------------------------
			修改纪录：
			日		期	版本  修改人  修改内容
			2002/05/23	1.0
=============================================================================*/
API int OspSend(u32 dstiid, u16 event, const void *content=0, u16 length=0, u32 dstnode=0, 
				u32 srciid=MAKEIID(INVALID_APP, INVALID_INS), u32 srcnode=INVALID_NODE,
				void* ackbuf=NULL, u16 ackbuflen=0, u16 *realacklen=NULL, 
				u16 timeout=1000);

/* used only in C++ environment */
int OspSend(const char* dstalias, u8 aliaslen, u16 dstapp, u16 event,		
			const void* content=0, u16 length=0, u32 dstnode=0, 
			u32 srciid=MAKEIID(INVALID_APP, INVALID_INS), u32 srcnode=INVALID_NODE,
			void* ackbuf=NULL, u16 ackbuflen=0, u16 *realacklen=NULL, 
			u16 timeout=1000);

/*====================================================================
函数名：IsOspInitd
功能：判断OSP是否已初始化过了
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：OSP已初始化，返回TRUE；否则返回FALSE.
====================================================================*/
API BOOL32 IsOspInitd(void);

/*=============================================================================
函 数 名：OspInit
功	  能：initialize OSP module
注	  意：
算法实现：
全局变量：
参	  数：TelNetEnable : [in] start telnet.exe
          TelNetPort : [in] osp telnet server port
          pchModuleName : [in] process module name，maxlength = 63
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
#define MAX_MODULE_NAME_LENGTH 63
API BOOL32 OspInit(BOOL32 TelNetEnable=FALSE, u16 TelNetPort=0, char* pchModuleName = NULL );

/*====================================================================
函数名：OspQuit
功能：退出OSP，调用本地所有APP的所有实例的InstanceExit方法，退出实例，
断开所有OSP结点，挂起所有OSP任务包括APP，然后返回。OSP退出之后，
所有OSP服务将不能使用。
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspQuit(void);

/*=============================================================================
函 数 名：OspSetPrompt
功	  能：设置终端提示符
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API BOOL32 OspSetPrompt(const char * prompt);

/*====================================================================
函数名：OspStopScrnLog
功能：暂停屏幕日志输出.
算法实现：（可选项）
引用全局变量：
输入参数说明：无

 返回值说明：无.
====================================================================*/
API void OspStopScrnLog(void);

/*====================================================================
函数名：OspResumeScrnLog
功能：恢复屏幕日志输出.
算法实现：（可选项）
引用全局变量：
输入参数说明：无

 返回值说明：无.
====================================================================*/
API void OspResumeScrnLog(void);

/*=============================================================================
函 数 名：OspGetLocalID
功	  能：本地全局编号
注	  意：
算法实现：
全局变量：
参	  数：
返 回 值：int - 本地全局编号
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API int OspGetLocalID(); // 本地全局编号

/*=============================================================================
函 数 名：OspEventDesc
功    能：返回消息的描述
注    意：
算法实现：
全局变量：
参    数：u16 wEventID : [in]消息的ID
返 回 值：const char *  消息的描述，如果消息ID无效或者该消息没有描述字符串，返回
值将为NULL。
-------------------------------------------------------------------------------
修改纪录：
日 	 期  版本  修改人  修改内容
2002/05/23  1.0
=============================================================================*/
API const char * OspEventDesc( u16 wEventID );

/*=============================================================================
函 数 名：OspEventDescShow
功	  能：显示消息的描述
注	  意：
算法实现：
全局变量：
参	  数：u16 wEventID : [in]消息的ID
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void OspEventDescShow( u16 wEventID );

/*=============================================================================
函 数 名：osphelp
功	  能：inline help function of OSP module
注	  意：
算法实现：
全局变量：
参	  数：const char * pszTopic : [in] which topic's help infomation to be shown
返 回 值：void
-------------------------------------------------------------------------------
修改纪录：
日		期	版本  修改人  修改内容
2002/05/23	1.0
=============================================================================*/
API void osphelp(void);

/*====================================================================
函数名：OspSemBCreate
功能：创建一个二元信号量
算法实现：（可选项）
引用全局变量：
输入参数说明：(out)ptSemHandle 返回的信号量句柄

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 OspSemBCreate(SEMHANDLE *ptSemHandle);

/*====================================================================
函数名：OspSemCCreate
功能：创建计数信号量
算法实现：（可选项）
引用全局变量：
输入参数说明：(out)ptSemHandle 信号量句柄返回参数， 
(in)initcount 初始计数，
(in)MaxSemCount 最大计数

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API BOOL32 OspSemCCreate(SEMHANDLE *ptSemHandle,u32 initcount,u32 MaxSemCount);

/*====================================================================
函数名：OspSemDelete
功能：删除信号量
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 待删除信号量的句柄

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API  BOOL32 OspSemDelete(SEMHANDLE tSemHandle );

/*====================================================================
函数名：OspSemTake
功能：信号量p操作，如该信号量不可用，则阻塞调用实例所在任务(APP)
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 信号量句柄

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API BOOL32	OspSemTake(SEMHANDLE tSemHandle );

/*====================================================================
函数名：OspSemTakeByTime
功能：带超时的信号量p操作，如信号量不可用，则时间消逝dwtime毫秒后，出错返回
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 信号量句柄, (in)dwtime 超时间隔(以ms为单位)

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API BOOL32 OspSemTakeByTime(SEMHANDLE tSemHandle, u32 dwtime);

/*====================================================================
函数名：OspSemGive
功能：信号量v操作
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 信号量句柄

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API BOOL32 OspSemGive(SEMHANDLE tSemHandle );

/*====================================================================
函数名：OspTaskCreate
功能：创建并执行一个任务
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
#ifdef _LINUX_
API TASKHANDLE OspTaskCreate(LINUXFUNC pvTaskEntry, char* szName, u8 byPriority, u32 dwStacksize, u32 dwParam, u16 wFlag, u32 *puTaskID=NULL);
#else
API TASKHANDLE OspTaskCreate(void* pvTaskEntry, char* szName, u8 byPriority, u32 dwStacksize, u32 dwParam, u16 wFlag, u32 *puTaskID=NULL);
#endif


/*====================================================================
  函数名：OspTaskSetAffinity
  功能：
  封装
  算法实现：（可选项）
  引用全局变量：
  输入参数说明：hTaskHandle:windows下的线程/进程的句柄；hTaskId:绑定的线程/进程/任务的ID；
                dwCpuId:需绑定的CPU(从0开始计数,逐次递加)
                byTaskType：绑定的线程/进程/任务的类型（0--线程，1-进程，其他-任务），linux下默认0即可
                pdwPreviousBind：如果之前该进程/线程/任务被绑定到某CPU，则会返回该CPU号（仅Solaris下有效）
  返回值说明：成功返回TRUE, 失败返回FALSE.
  ====================================================================*/
#ifdef _MSC_VER 
API BOOL32 OspTaskSetAffinity( TASKHANDLE hTaskHandle, u32 dwCpuId, u8 byTaskType = 0, u32* pdwPreviousBind = NULL);
#else
API BOOL32 OspTaskSetAffinity( u64 hTaskId, u32 dwCpuId, u8 byTaskType = 0, u32* pdwPreviousBind = NULL);   
#endif

/*====================================================================
函数名：OspTaskExit
功能：退出调用任务，任务退出之前用户要注意释放本任务申请的内存、信号量等资源。
封装Windows的ExitThread(0)和vxWorks的taskDelete(0)。
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspTaskExit(void);

/*====================================================================
函数名：OspTaskTerminate
功能：强行终止指定任务。
封装Windows的ExitThread()和vxWorks的taskDelete()。
算法实现：（可选项）
引用全局变量：
输入参数说明：handle--待杀任务的handle

 返回值说明：
====================================================================*/
API BOOL32 OspTaskTerminate(TASKHANDLE handle);

/*====================================================================
函数名：OspTaskSuspend
功能：挂起当前任务
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspTaskSuspend(TASKHANDLE handle=0);

/*====================================================================
函数名：OspTaskResume
功能：挂起当前任务
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspTaskResume(TASKHANDLE handle);

/*====================================================================
函数名：OspTaskSetPriority
功能：改变任务的优先级。

 算法实现：（可选项）
 引用全局变量：
 输入参数说明：
 
  返回值说明：
====================================================================*/
API BOOL32 OspTaskSetPriority(TASKHANDLE taskHandle, u8 newPriority, int newSchedPolicy = 0);

/*====================================================================
函数名：OspTaskGetPriority
功能：获得任务的优先级。

 算法实现：（可选项）
 引用全局变量：
 输入参数说明：
 
  返回值说明：
====================================================================*/
API BOOL32 OspTaskGetPriority(TASKHANDLE taskHandle, u8 * puPri, int *SchedPolicy =NULL);

/*====================================================================
函数名：OspSetTaskPriorityByName
功能：通过任务名设置任务的优先级

 算法实现：（可选项）
 引用全局变量：
 输入参数说明：
 
  返回值说明：
====================================================================*/
API void OspSetTaskPriorityByName(s8 *name , u8 priority, int newSchedPolicy = 0);

/*====================================================================
函数名：OspSetTaskPriorityByName
功能：通过任务名获取任务的优先级

 算法实现：（可选项）
 引用全局变量：
 输入参数说明：
 
  返回值说明：
====================================================================*/
API void OspGetTaskPriorityByName(s8 *name , u8 *priority, int *newSchedPolicy = NULL);

/*====================================================================
函数名：OspTaskSelfHandle
功能：获得调用任务的句柄
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API TASKHANDLE OspTaskSelfHandle(void);

/*====================================================================
函数名：OspTaskSelfHandle
功能：获得调用任务的ID
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API u32 OspTaskSelfID(void);

/*====================================================================
函数名：OspTaskHandleVerify
功能：判断指定任务是否存在
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspTaskHandleVerify(TASKHANDLE handle);

/*====================================================================
函数名：OspTaskSafe
功能：保护调用任务不被非法删除
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspTaskSafe(void);

/*====================================================================
函数名：OspTaskUnsafe
功能：释放调用任务的删除保护，使得调用任务可以被安全删除
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OspTaskUnsafe(void);

/*====================================================================
函数名：OspTaskDelay
功能：任务延时
算法实现：（可选项）
引用全局变量：
输入参数说明：uMseconds--延时间隔(ms)

 返回值说明：
====================================================================*/
#define OspDelay					  OspTaskDelay

API void OspTaskDelay(u32 dwMseconds);

/*====================================================================
函数名：OspCreateMailbox
功能：创建消息队列
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspCreateMailbox(char* szName,u32 dwMsgNumber,u32 dwMsgLength ,u32 *puQueueIDRead,u32 *puQueueIDWrite);

/*====================================================================
函数名：OspCloseMailbox
功能：关闭邮箱
算法实现：（可选项）
引用全局变量：
输入参数说明：uQueueIDRead, uQueueIDWrite--邮箱的读、写句柄

 返回值说明：
====================================================================*/
API void OspCloseMailbox(u32 dwQueueIDRead, u32 dwQueueIDWrite);

/*====================================================================
函数名		：OspSndMsg
功能		：向信箱发消息
算法实现	：
引用全局变量：无
输入参数说明：tQueueID: 信箱ID,
lpMsgBuf: 内容指针,
len: 长度,
timeout: 超时值(ms). Windows下该参数无效, 邮箱满将一直阻塞直到有空的buffer.
VxWorks下, 缺省为2秒; 
OSP_NO_WAIT(0): 邮箱满立即返回;
OSP_WAIT_FOREVER(-1): 邮箱满阻塞直到有空的buffer.

 返回值说明  ：成功返回TRUE, 失败或超时返回FALSE.
====================================================================*/					
API BOOL32 OspSndMsg(u32 tQueueID, const char *lpMsgBuf, u32 len, int timeout=2000);

/*====================================================================
函数名		：OspRcvMsg
功能		：从信箱收消息
算法实现	：
引用全局变量：无
输入参数说明：
OSTOKEN tQueueID	信箱ID
u32  dwTimeout	 超时(毫秒)
LPVOID *lpMsgBuf	返回消息内容指针
返回值说明	：
====================================================================*/
API BOOL32 OspRcvMsg(u32 tQueueID,u32  dwTimeout,char * lpMsgBuf, u32 len, u32 * pLenRcved);

/*====================================================================
函数名：OspClkRateGet
功能：得到tick精度：n ticks/sec
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API u32 OspClkRateGet();

/*====================================================================
函数名：OspTickGet
功能：取得当前的tick数
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API u32 OspTickGet();


/*====================================================================
函数名：OspTickGet64
功能：取得当前的tick数,64长度，解决溢出问题
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API u64 OspTickGet64();



API void OspCpuShow();

//CPU信息
typedef struct
{
	// 空闲百分比，0－100
	u8 m_byIdlePercent;
}TOspCpuInfo;

/*====================================================================
函数名：OspGetCpuInfo
功能：取得当前的CPU信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptCpuInfo，用户CPU信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OspGetCpuInfo( TOspCpuInfo* ptCpuInfo );

//内存信息
typedef struct
{
	//物理内存大小(KByte)
	u32 m_dwPhysicsSize;
	//已使用内存(KBytes)
	u32 m_dwAllocSize;
	//可用内存(KBytes)
	u32 m_dwFreeSize;
}TOspMemInfo;

/*====================================================================
函数名：OspGetMemInfo
功能：取得当前的Mem信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptMemInfo，用户MEM信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OspGetMemInfo( TOspMemInfo* ptMemInfo );

//时间信息
typedef struct
{
	//年(2006)
	u16 m_wYear;
	//月(1--12)
	u16 m_wMonth;
	//日(1--31)
	u16 m_wDay;
	//时(0--23)
	u16 m_wHour;
	//分(0--59)
	u16 m_wMinute;
	//秒(0--59)
	u16 m_wSecond;
}TOspTimeInfo;

/*====================================================================
函数名：OspGetTimeInfo
功能：取得当前的Time信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptTimeInfo，系统time信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OspGetTimeInfo( TOspTimeInfo* ptTimeInfo );

/*====================================================================
函数名：OspSetTimeInfo
功能：设置当前的Time信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptTimeInfo，系统time信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OspSetTimeInfo( TOspTimeInfo* ptTimeInfo );

//磁盘分区信息
typedef struct
{
	//磁盘分区大小(MByte)
	u32 m_dwPhysicsSize;
	//已使用磁盘分区大小(MBytes)
	u32 m_dwUsedSize;
	//可用磁盘分区大小(MBytes)
	u32 m_dwFreeSize;
}TOspDiskInfo;
//磁盘分区最大长度
#define MAX_PARTION_NAME_LENGTH		255
/*====================================================================
函数名：OspGetDiskInfo
功能：取得当前的磁盘分区信息
算法实现：（可选项）
引用全局变量：
输入参数说明：s8 *pchPartionName， 分区名
					win32 : "c:"
					Vxworks : "/ata0/"
					Linux : "/"
			  ptDiskInfo，系统磁盘分区信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OspGetDiskInfo( s8 *pchPartionName , TOspDiskInfo* ptDiskInfo );

//////////////////////////////////////////////////////////////////////
//	套接字封装：用户使用套接字前必须先调用SockInit()对套接字库进行初始化，
//	使用完之后则需要调用SockCleanup()释放对套接字库的使用。由于vxWorks和
//	Windows的关闭套接字的函数不同，而且在调用socket、connect等函数时，返回
//	值类型也不一样，OSP也进行了必要的封装，原则是保持与Windows下基本一致。
/////////////////////////////////////////////////////////////////////

/*====================================================================
函数名：SockInit
功能：套接字初始化，封装Windows下的WSAStartup，vxWorks下返回TRUE
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 SockInit(void);

/*====================================================================
函数名：SockShutdown
功能：对各平台shutdown的简单封装
算法实现：（可选项）
引用全局变量：
输入参数说明：hSock--要操作的套接字，
			  dwMode--操作模式, 可取值为STOP_SEND, STOP_RECV或STOP_BOTH

返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
#ifdef _LINUX_
API BOOL SockShutdown(SOCKHANDLE hSock, u32 uMode);
#endif

/*====================================================================
函数名：SockClose
功能：关闭套接字，封装windows下的closesocket和vxworks下的close.
算法实现：（可选项）
引用全局变量：
输入参数说明：tSock--待关闭套接字的句柄（调用socket时返回）。

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 SockClose(SOCKHANDLE tSock);

/*====================================================================
函数名：SockCleanup
功能：套接字销毁，封装windows的WSACleanup，vxWorks下返回TRUE
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 SockCleanup(void);

// 串口封装：以Windows串口操作函数为准，在VxWorks下提供相应函数
/*====================================================================
函数名：OspSerialOpen
功能：打开指定的串口
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：成功返回打开串口的句柄，失败返回INVALID_SERIALHANDLE
====================================================================*/
API SERIALHANDLE OspSerialOpen(u8 port);

/*====================================================================
函数名：OspSerialClose
功能：关闭指定的串口
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspSerialClose(SERIALHANDLE hCom);

/*====================================================================
函数名：OspSerialRead
功能：从指定的串口读数据
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspSerialRead(SERIALHANDLE hCom, char *pchBuf, u32 dwu8sToRead, u32 *puu8sRead);

/*====================================================================
函数名：OspSerialWrite
功能：向指定的串口写数据
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspSerialWrite(SERIALHANDLE hCom, char *pchBuf, u32 dwu8sToWrite, u32 *puu8sWrite);

/*====================================================================
函数名：OspSetCommTimeouts
功能：设置串口超时
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspSetCommTimeouts(SERIALHANDLE hCom, TOspCommTimeouts *ptCommTimeouts);

/*====================================================================
函数名：OspGetCommState
功能：获得串口状态
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspGetCommState(SERIALHANDLE hCom, TOspDCB *ptDCB);

/*====================================================================
函数名：OspSetCommState
功能：设置串口状态
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspSetCommState(SERIALHANDLE hCom, TOspDCB *ptDCB);

/*====================================================================
函数名：OspEscapeCommFunction
功能：使用串口扩展功能
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspEscapeCommFunction(SERIALHANDLE hCom, u32 dwFunc);

/*====================================================================
函数名：OspPurgeComm
功能：丢弃所有未决数据，终止串口读写操作
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspPurgeComm(SERIALHANDLE hCom, u32 dwFlags);

/*====================================================================
函数名：OspClearCommError
功能：与Windows下ClearCommError相同
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspClearCommError(SERIALHANDLE hCom, u32 *pdwErrors, TOspComStat *ptStat);

/*====================================================================
函数名：OspGetCommTimeouts
功能：取得打开串口的超时设置
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OspGetCommTimeouts(SERIALHANDLE hCom, TOspCommTimeouts *ptCommTimeouts);

/*====================================================================
函数名：OspAgentStart
功能：启动一个Osp测试代理, 其中包含一个测试服务器APP和一个客户端APP, 它们
主要不同是APP号, 消息处理过程都是一样的.
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)port -- 端口号, 如无其他APP创建服务节点则用该端口创建之

 返回值说明：
====================================================================*/
API int OspAgentStart(u16 port);

/*====================================================================
函数名：OspAllocMem
功能：从内存池中分配内存块
算法实现：（可选项）
引用全局变量：
输入参数说明：size: 待分配的内存块大小，

 返回值说明：成功返回指向分配到的内存块的指针，失败返回NULL.
====================================================================*/
API void *OspAllocMem(size_t size);

/*====================================================================
函数名：OspFreeMem
功能：释放事先分配的内存块
算法实现：（可选项）
引用全局变量：
输入参数说明：token: 指向待释放的内存块的指针。

 返回值说明：无.
====================================================================*/
API void OspFreeMem(void *token);

/*====================================================================
函数名：OspSetPriRealTime
功能：把定时器、消息发送等任务提高到比系统net任务还高的优先级
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：无.
====================================================================*/
API void OspSetPriRealTime();

/*=============================================================================
函 数 名：OspAddrListGet
功	  能：获取本机地址表内容
注	  意：如果是在Windows下使用本函数，必须先调用OspInit，或者WSAStartup。
本函数效率较低，不要重复调用。
算法实现：
全局变量：
参	  数： u32	 adwIP[] : [in/out]用户申请的用于地址表内容的数组的首地址
u16   wNum : [in]用户申请的数组的大小
返 回 值： 返回本机地址表中地址个数，错误时返回0
=============================================================================*/
API u16  OspAddrListGet(u32 adwIPList[], u16 wNum);




//系统限制参数
#define OSP_ETHERNET_ADAPTER_MAX_IP 16			/*每个接口上最大地址个数*/
#define OSP_ETHERNET_ADAPTER_MAX_NUM 32			/*每个设备最大接口数*/
#define OSP_ETHERNET_ADAPTER_NAME_LENGTH 64 /*每个接口名的最大长度*/
#define OSP_ETHERNET_ADAPTER_MAC_LENGTH	6		/*接口的MAC地址长度*/

//网卡状态,对应于TOSPEthernetAdapterInfo 结构的nState字段
#define OSP_ETHERNET_ADAPTER_UNKNOWN 0
#define OSP_ETHERNET_ADAPTER_UP 1
#define OSP_ETHERNET_ADAPTER_DOWN 2

/* 单个网卡信息 */
typedef struct {
    u32 nId; 										/*网卡号*/
    u32 nState; 									/*网卡状态*/
    u8  achName[OSP_ETHERNET_ADAPTER_NAME_LENGTH]; 		/*网卡名*/
    u8  achDescription[OSP_ETHERNET_ADAPTER_NAME_LENGTH];	/*网卡信息描述*/
    u8  achMacAddr[OSP_ETHERNET_ADAPTER_MAC_LENGTH]; 		/*网卡物理地址*/
    u32 nIpNum; 									/*地址数量*/
    u32 anIp[OSP_ETHERNET_ADAPTER_MAX_IP]; 				/*地址*/
}TOSPEthernetAdapterInfo;

/* 多个网卡信息 */
typedef struct {
    u32 nNum; 														/*网卡数量*/
    TOSPEthernetAdapterInfo atEthernetAdapter[OSP_ETHERNET_ADAPTER_MAX_NUM];/*网卡信息*/
}TOSPEthernetAdapterInfoAll;


/*=============================================================================
函 数 名：OspGetEthernetAdapterInfoAll
功	  能：获取本系统所有以太网网卡信息
注	  意：如果是在Windows下使用本函数，必须先调用OspInit，或者WSAStartup。
本函数效率较低，不要重复调用。
算法实现：
全局变量：
参	  数：TEthernetAdapterInfoAll 多网卡信息结构，参考数据结构的定义
返 回 值： TRUE/ FALSE 成功/失败
=============================================================================*/
API BOOL OspGetEthernetAdapterInfoAll(TOSPEthernetAdapterInfoAll * ptEthernetAdapterInfoAll);

/*=============================================================================
函 数 名：OspGetEthernetAdapterInfo
功	  能：根据网卡号获取网卡信息
注	  意：如果是在Windows下使用本函数，必须先调用OspInit，或者WSAStartup。
本函数效率较低，不要重复调用。
算法实现：
全局变量：
参	  数：nEthAdapterId 网卡编号0-ETHERNET_ADAPTER_MAX_NUM，最多支持16个网卡
            TEthernetAdapterInfo 单网卡信息结构
返 回 值：TRUE/FALSE  成功/失败
=============================================================================*/
API BOOL OspGetEthernetAdapterInfo(u32 nEthAdapterId, TOSPEthernetAdapterInfo * ptEthernetAdapterInfo);






/*=============================================================================
函 数 名：OspSetMemCheck
功	  能：获取内存地址内容
注	  意：
算法实现：
全局变量：
参	  数： u32 * pdwCheckMem :	要检查的内存地址
u32 dwCheckValue ：  期望检查的值
BOOL32	bEqual ：当本参数为真时，表示要检查的内存地址内容不等于期望值时需要告警
当本参数为假时，表示要检查的内存地址内容等于期望值时需要告警
返 回 值： 成功返回TRUE, 失败返回FALSE.
=============================================================================*/
API BOOL32	OspSetMemCheck(u32 * pdwCheckMem, u32 dwCheckValue, BOOL32	bEqual);

/* 内联成员函数或模板类成员函数的实现 */
template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zTemplate<I, maxins, A, maxAliasLen>::InitAliasArray()
{
	for(int i = 0; i < maxins; i++)
	{
		instAliasArray[i].instId = 0xFFFF;
		instAliasArray[i].aliasRet = 0; 
		instAliasArray[i].nextLoc = 0xFFFF;
		
		bakInstAliasArray[i].instId = 0xFFFF;
		bakInstAliasArray[i].aliasRet = 0;
		bakInstAliasArray[i].nextLoc = 0xFFFF;
	}
	curBakArrayNum = 0;
	
	return TRUE;
}

template <class I, int maxins, class A, u8 maxAliasLen>
u32 zTemplate<I, maxins, A, maxAliasLen>::AliasToWord(char * pchAlias, u8 len)
{
	u32  ret = 0;
	for(u8 i = 0; i < len; i++ )
	{
		ret =  (ret<<5) + ret + (*(pchAlias + i));
	}
	
	ret = ret & (maxins-1);
	return	ret;
}

template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zTemplate<I, maxins, A, maxAliasLen>::IsInBakArray(u32  setRet)
{
	/*首先检查主数组*/
	if(instAliasArray[setRet].instId == 0xFFFF)    /*主数组中这个位置还没有被占用*/
	{
		return FALSE;	  /*这个实例存放到主数组中*/
	}
	else		 /*主数组中这个位置还已经被占用*/
	{
		return TRUE;   /*这个实例存放到bak数组中的空闲位置*/
	}
}

template <class I, int maxins, class A, u8 maxAliasLen>
int zTemplate<I, maxins, A, maxAliasLen>::GetInstanceNumber(void)
{
	return maxins;
}

template <class I, int maxins, class A, u8 maxAliasLen>
u8 zTemplate<I, maxins, A, maxAliasLen>::GetMaxAliasLen(void)
{
	return maxAliasLen;
}

template <class I, int maxins, class A, u8 maxAliasLen>
CInstance* zTemplate<I, maxins, A, maxAliasLen>::GetInstance(u16 insid)
{
	if(insid == CInstance::DAEMON)
		return &daemIns;
	
	if((insid > 0) && (insid <= maxins))
		return &insarray[insid-1];
	
	return NULL;
}

template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zTemplate<I, maxins, A, maxAliasLen>::SetInstAlias(u16 insid, const char * pchAlias, u8 len)
{
	u32  dwCycTime = 0;
	
	/*先把记录信息放入快速查找数组中*/
	if(insid != CInstance::DAEMON)
	{
		u32 setRet = AliasToWord((char *)pchAlias, len);	/*通过别名计算设置值*/
		if(setRet >= maxins)
			return FALSE;
		
		BOOL32	isBak = IsInBakArray(setRet);	/*确定存放位置*/
		if( isBak == FALSE) 	/*存放在主数组中*/
		{
			instAliasArray[setRet].aliasRet = setRet;	 
			instAliasArray[setRet].instId = insid;
			instAliasArray[setRet].nextLoc = 0xFFFF;   /*初始化*/
		}
		else		 /*存放在bak数组中*/
		{
			u32 loc = curBakArrayNum; 
			/*把数据记录到bak数组相应位置中*/
			for(loc = 0; loc < maxins; loc++)
			{
				if(bakInstAliasArray[loc].instId == 0xFFFF)
					break;
			}
			if(loc >= maxins)
			{
				return FALSE;
			}
			
			bakInstAliasArray[loc].aliasRet = setRet;
			bakInstAliasArray[loc].instId = insid;
			bakInstAliasArray[loc].nextLoc = 0xFFFF;   /*初始化*/
			
			/*把相同设置值的数据块连接起来*/
			if(instAliasArray[setRet].nextLoc == 0xFFFF)   /*主数组中尚未连接*/
			{
				instAliasArray[setRet].nextLoc = loc;	/*指向bak数组中存放位置*/
			}
			else
			{ 
				TInstAliasInfo * tempAlias = &bakInstAliasArray[instAliasArray[setRet].nextLoc];
				if(tempAlias == NULL)
					return FALSE;
				
				dwCycTime = 0;
				while(tempAlias->nextLoc != 0xFFFF)
				{
					if(tempAlias->nextLoc >= maxins)
						return FALSE;
					
					tempAlias = &bakInstAliasArray[tempAlias->nextLoc];
					if(tempAlias == NULL)
						return FALSE;
					
					dwCycTime++;
					if(dwCycTime > maxins)
					{
						OspPrintf(TRUE, FALSE, "SetInstAlias() return Cycle as dwCycTime=%d\n", dwCycTime);
						return FALSE;
					}
				}
				
				tempAlias->nextLoc = loc; /*找到bak数组中最后一个相同设置值的位置*/
			}
			
			curBakArrayNum++;	  /*bak数组存放个数增加*/
		}
	}
	
	CInstance* pIns=GetInstance(insid);
	if( pIns != NULL && len <= maxAliasLen)
	{
		CInstance* pOtherIns = FindInstByAlias(pchAlias, len);
		if( (pOtherIns != NULL) && (pOtherIns->GetInsID() != pIns->GetInsID()) )
		{
			return FALSE;
		}
		memcpy(pIns->m_alias, pchAlias, len);
		pIns->m_aliasLen = len;
		//return pIns->SetAlias(pchAlias, len);
	}
	return TRUE;
}  

template <class I, int maxins, class A, u8 maxAliasLen>
CInstance* zTemplate<I, maxins, A, maxAliasLen>::FindInstByAlias(const char * pchAlias, u8 len)
{
	/* 先看是否是Daemon实例 */
	u8	 aliasLen=0;
	u32  dwCycTime = 0;
	
	if(maxAliasLen < len)
	{
		return NULL;
	}
	
	char aliasBuf[MAX_ALIAS_LEN];
	memset(aliasBuf, 0, MAX_ALIAS_LEN);
	
	CInstance *pIns = GetInstance(CInstance::DAEMON);		
	if(pIns != NULL && 
		pIns->GetAlias(aliasBuf, maxAliasLen, &aliasLen))
	{
		if( (aliasLen == len) && 
			(memcmp(aliasBuf, pchAlias, len) == 0) )
		{
			return pIns;
		}
	}
	
	/* 再看是否普通实例 */
	TInstAliasInfo * tempAlias = NULL;
	u32  setRet = AliasToWord((char *)pchAlias, len);
	/*先查找主数组*/
	if(setRet >= maxins)
		return NULL;
	
	tempAlias = &instAliasArray[setRet];
	if(tempAlias == NULL)
		return NULL;
	
	pIns = GetInstance(tempAlias->instId);
	if((pIns != NULL) && (pIns->GetAlias(aliasBuf, maxAliasLen, &aliasLen))) 
	{
		if( (aliasLen == len) && (memcmp(aliasBuf, pchAlias, len) == 0) )
		{
			return pIns;
		}
	}
	/*再查找bak数组*/
	do
	{
		if((tempAlias == NULL) || (tempAlias->nextLoc >= maxins))
			return NULL;
		
		tempAlias = &bakInstAliasArray[tempAlias->nextLoc];
		if(tempAlias == NULL)
			return NULL;
		
		CInstance * pIns = GetInstance(tempAlias->instId);
		if((pIns != NULL) && (pIns->GetAlias(aliasBuf, maxAliasLen, &aliasLen))) 
		{
			if((aliasLen == len) && (memcmp(aliasBuf, pchAlias, len) == 0))
			{
				return pIns;
			}
		}
		
		dwCycTime++;
		if(dwCycTime > maxins)
		{
			OspPrintf(TRUE, FALSE, "FindInstByAlias() return Cycle as dwCycTime=%d\n", dwCycTime);
			return NULL;
		}
	}while(tempAlias->nextLoc != 0xFFFF); 
	
	return NULL;
}

/*清除实例别名纪录信息*/
template <class I, int maxins, class A, u8 maxAliasLen>
BOOL32 zTemplate<I, maxins, A, maxAliasLen>::ClearInstAlias(u16 insid, const char * pAlias, u8 len)
{
	u32  dwCycTime = 0;
	
	if((maxAliasLen < len) || (insid == CInstance::DAEMON))
	{
		return FALSE;
	}
	
	u32 setRet = AliasToWord((char *)pAlias, len);	  /*通过别名计算设置值*/
	if(setRet >= maxins)
		return FALSE;
	
	/*先查找主数组，再查找备用数组*/
	if(instAliasArray[setRet].instId == insid)
	{
		if(instAliasArray[setRet].nextLoc == 0xFFFF)   /*主数组中尚未连接*/
		{
			instAliasArray[setRet].aliasRet = 0;	   /*初始化*/
			instAliasArray[setRet].instId = 0xFFFF;
			instAliasArray[setRet].nextLoc = 0xFFFF;   
		}
		else
		{
			TInstAliasInfo * tempAlias = &instAliasArray[setRet];
			if(tempAlias == NULL)
				return FALSE;
			
			if(tempAlias->nextLoc >= maxins)
				return FALSE;
			
			TInstAliasInfo * relocAlias = &bakInstAliasArray[tempAlias->nextLoc];
			if(relocAlias == NULL)
				return FALSE;
			
			dwCycTime = 0;
			while(relocAlias->nextLoc != 0xFFFF)
			{
				tempAlias->aliasRet = relocAlias->aliasRet;    //用下一块记录信息填充本块记录信息
				tempAlias->instId = relocAlias->instId;
				
				if(relocAlias->nextLoc >= maxins)
					return	FALSE;
				
				tempAlias = relocAlias; 		//位置移到下一块
				relocAlias = &bakInstAliasArray[relocAlias->nextLoc];
				if(relocAlias == NULL)
					return	FALSE;
				
				dwCycTime++;
				if(dwCycTime > maxins)
				{
					OspPrintf(TRUE, FALSE, "ClearInstAlias(1) return Cycle as dwCycTime=%d\n", dwCycTime);
					return FALSE;
				}
			}
			
			tempAlias->aliasRet = relocAlias->aliasRet;    //已到最后一块记录信息块
			tempAlias->instId = relocAlias->instId;
			tempAlias->nextLoc = 0xFFFF;
			
			relocAlias->aliasRet = 0;	   //清除最后一块记录块
			relocAlias->instId = 0xFFFF;
			relocAlias->nextLoc = 0xFFFF;
			
			curBakArrayNum--;	  /*bak数组存放个数减少*/
		}
	}
	else
	{
		if(instAliasArray[setRet].nextLoc == 0xFFFF)   /*主数组中尚未连接*/
		{
			return	FALSE;
		}
		else
		{ 
			TInstAliasInfo * tempAlias = &bakInstAliasArray[instAliasArray[setRet].nextLoc];
			if(tempAlias == NULL)
				return FALSE;
			
			TInstAliasInfo * relocAlias = &(instAliasArray[setRet]);
			
			dwCycTime = 0;
			while(tempAlias->instId != insid)
			{
				relocAlias = tempAlias;
				
				if(tempAlias->nextLoc != 0xFFFF)
				{
					if(tempAlias->nextLoc >= maxins)
						return	FALSE;
					
					tempAlias = &bakInstAliasArray[tempAlias->nextLoc];
					if(tempAlias == NULL)
						return FALSE;
					
					dwCycTime++;
					if(dwCycTime > maxins)
					{
						OspPrintf(TRUE, FALSE, "ClearInstAlias(2) return Cycle as dwCycTime=%d\n", dwCycTime);
						return FALSE;
					}
				}
				else
				{
					return	FALSE;
				}
			}
			
			/*找到bak数组中对应实例纪录位置，清除该位置的记录值，把记录链表重新连接*/
			if(tempAlias->nextLoc == 0xFFFF)
			{
				if(relocAlias != NULL)
					relocAlias->nextLoc = 0xFFFF;
				
				tempAlias->aliasRet = 0;	 //该位置为本设置值的最后一位置，直接清除
				tempAlias->instId = 0xFFFF;
			}
			else
			{
				relocAlias = &bakInstAliasArray[tempAlias->nextLoc];
				if(relocAlias == NULL)
					return FALSE;
				
				dwCycTime = 0;
				while(relocAlias->nextLoc != 0xFFFF)
				{
					tempAlias->aliasRet = relocAlias->aliasRet;    //用下一块记录信息填充本块记录信息
					tempAlias->instId = relocAlias->instId;
					
					if(relocAlias->nextLoc >= maxins)
						return	FALSE;
					
					tempAlias = relocAlias; 		//位置移到下一块
					relocAlias = &bakInstAliasArray[relocAlias->nextLoc];
					if(relocAlias == NULL)
						return	FALSE;
					
					dwCycTime++;
					if(dwCycTime > maxins)
					{
						OspPrintf(TRUE, FALSE, "ClearInstAlias(3) return Cycle as dwCycTime=%d\n", dwCycTime);
						return FALSE;
					}
				}
				
				tempAlias->aliasRet = relocAlias->aliasRet;    //已到最后一块记录信息块
				tempAlias->instId = relocAlias->instId;
				tempAlias->nextLoc = 0xFFFF;
				
				relocAlias->aliasRet = 0;	   //清除最后一块记录块
				relocAlias->instId = 0xFFFF;
				relocAlias->nextLoc = 0xFFFF;
			}
			
			curBakArrayNum--;	  /*bak数组存放个数减少*/
		}
	}	
	
	return	TRUE;
}

#if defined(_MSC_VER) && !defined(_EQUATOR_)
	#pragma pack(pop)
#endif

#endif // _OSP_H
