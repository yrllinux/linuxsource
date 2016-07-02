/******************************************************************************
模块名	: OSPSMALL
文件名	: osp_small.h
功能    : OSPSMALL库的头文件
版本	：4.0
******************************************************************************/
/***************************
模块版本：OSP.Small.KDV.4.0.1.1.0.20080628
增加功能：协议组OSP版本管理,修正OSAPITickGet函数，避免返回-1，始终返回正确的tick
修改缺陷：无
提交人：徐一平
*****************************/

#ifndef _OSP_SMALL_H
#define _OSP_SMALL_H


#define OSAPI_TIMER_MAGIC                 0xFCA512B5 

#define OSAPI_TIMER_NUMBER				  100  

#define OSAPI_THREAD_NUMBER               100     

#define OSAPI_ERROR_CODE_BASE             25000


#define OSAPI_TIMER_NOT_INIT              (OSAPI_ERROR_CODE_BASE+1)
#define OSAPI_TIMER_PARAM_ERROR           (OSAPI_ERROR_CODE_BASE+2)
#define OSAPI_TIMER_HANDLE_ERROR          (OSAPI_ERROR_CODE_BASE+3)
#define OSAPI_TIMER_MEMALLOC_FAIL         (OSAPI_ERROR_CODE_BASE+4)
#define OSAPI_TIMER_SEM_CREATE_FAIL       (OSAPI_ERROR_CODE_BASE+4)
#define OSAPI_TIMER_TASK_CREATE_FAIL      (OSAPI_ERROR_CODE_BASE+4)
#define OSAPI_TIMER_NO_FREE               (OSAPI_ERROR_CODE_BASE+5)
#define OSAPI_TIMER_IS_FREE               (OSAPI_ERROR_CODE_BASE+6)
#define OSAPI_TIMER_DELETED               (OSAPI_ERROR_CODE_BASE+7)
#define OSAPI_TIMER_NO_USED               (OSAPI_ERROR_CODE_BASE+8)

#define OSAPI_TELNET_BIND_FAIL            (OSAPI_ERROR_CODE_BASE+9)
#define OSAPI_TELNET_LISTEN_FAIL          (OSAPI_ERROR_CODE_BASE+10)
#define OSAPI_TELNET_SEM_FAIL             (OSAPI_ERROR_CODE_BASE+11)
#define OSAPI_TELNET_TASK_FAIL            (OSAPI_ERROR_CODE_BASE+12)

#define OSAPI_TELNET_NOT_CONNECT          (OSAPI_ERROR_CODE_BASE+13)
#define OSAPI_TELNET_SERVER_FAIL          (OSAPI_ERROR_CODE_BASE+14)
#define OSAPI_TELNET_SEND_OVERFLOW        (OSAPI_ERROR_CODE_BASE+15)
#define OSAPI_TELNET_SOCK_ERROR           (OSAPI_ERROR_CODE_BASE+16)
#define OSAPI_TELNET_PARAM_ERROR          (OSAPI_ERROR_CODE_BASE+17)

/********************************************************************
    OSAPI module
********************************************************************/
//#ifdef _VXWORKS_
///*#include "brddrvlib.h"*/
//#endif
//
//#ifdef _ARM_
//#include "brdwrapper.h"
//#endif


/*****************************************************************
	Common header files
******************************************************************/

#include "kdvtype.h"



/******************************************************************************
	Linux header files
******************************************************************************/

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
#include <sys/ioctl.h>
#include <sys/types.h>
typedef struct sockaddr * SOCKADDRPTR;  /* AL - 09/17/98 */
#define ioctlOnPtrTypeCast s32         /* AL - 10/28/98 */
 
typedef socklen_t sockaddr_namelen;

#define ERRNOGET errno
#define ERRNOSET(a) errno = a

#endif 

/*****************************************************************
	Win32 header files
******************************************************************/
#ifdef _MSC_VER

#include <malloc.h>
#include <stdio.h>
/*#include <windows.h>*/
#include <winsock2.h>

#include <assert.h>
#pragma comment(lib,"Ws2_32.lib")

typedef struct sockaddr * SOCKADDRPTR;  /* AL - 09/17/98 */
typedef unsigned long * ioctlOnPtrTypeCast;         /* AL - 11/25/98 */
typedef int sockaddr_namelen;
#define ioctl ioctlsocket
#define close closesocket

#define ERRNOGET GetLastError()
#define ERRNOSET(a) SetLastError(a)

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE


#endif


/*****************************************************************
		OS dependent data type definitions
******************************************************************/


#ifdef _LINUX_
#ifndef BOOL_DEFINE
#define BOOL_DEFINE
typedef int 						 BOOL;
#endif
#ifndef TRUE
#define TRUE						 1
#endif
#ifndef FALSE
#define FALSE						 0
#endif

typedef void *(*OSAPILINUXFUNC)(void *);

/*#define OSAPISEMHANDLE					 sem_t *  */
#define OSAPISEMHANDLE					 pthread_cond_t *
#define OSAPITASKHANDLE					 pthread_t
#endif

#ifndef _MSC_VER
#ifndef u64
#define u64							 unsigned long long
#endif
#define OSAPISOCKHANDLE				 int
#define OSAPISERIALHANDLE			 int

#ifndef SOCKADDR
#define SOCKADDR					 struct sockaddr
#endif
#ifndef SOCKADDR_IN
#define SOCKADDR_IN 				 struct sockaddr_in
#endif
#endif

#ifdef _MSC_VER
#define OSAPISEMHANDLE					 HANDLE
#define OSAPITASKHANDLE					 HANDLE

#define OSAPISOCKHANDLE		    	 SOCKET
#define OSAPISERIALHANDLE			 HANDLE
#endif

/*****************************************************************
		OS dependent macro definitions
******************************************************************/

#ifdef _LINUX_
#ifndef SOCKET_ERROR
#define SOCKET_ERROR		  (-1)
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET		  (-1)
#endif
#ifndef INVALID_SERIALHANDLE
#define INVALID_SERIALHANDLE  (-1)
#endif

#define OSAPI_NO_WAIT 		  0
#define OSAPI_WAIT_FOREVER	  (-1)

/* 在函数SockShutdown中使用的宏 */
#ifndef STOP_RECV
#define STOP_RECV			  0
#endif
#ifndef STOP_SEND
#define STOP_SEND			  1
#endif
#ifndef STOP_BOTH
#define STOP_BOTH			  2
#endif

#endif /* _LINUX_  */


#ifdef _MSC_VER
#ifndef INVALID_SERIALHANDLE
#define INVALID_SERIALHANDLE  INVALID_HANDLE_VALUE
#endif
#define TOsApiCommTimeouts	  COMMTIMEOUTS
#define TOsApiDCB 			  DCB
#define TOsApiComStat 		  COMSTAT

#define OSAPI_NO_WAIT 		  ((int)0)
#define OSAPI_WAIT_FOREVER	  ((int)-1)

#endif


/*****************************************************************
		OS independent section
******************************************************************/

#define OSAPI						0x20

#ifdef _MSC_VER			/* for Microsoft c++ */

#ifndef _EQUATOR_
#pragma pack(push)
#pragma pack(1)
#endif	/* _EQUATOR__ */

#ifdef __cplusplus			/* for C++ */
#ifndef API
#define API  extern "C"			__declspec(dllexport)
#endif
#else						/* for C */
#ifndef API
#define API						__declspec(dllexport)
#endif
#endif  /*__cplusplus*/

#else							/* for gcc */

#ifdef __cplusplus			/* for C++ */
#ifndef API
#define API  extern "C"	
#endif
#else						/* for C */
#ifndef API
#define API	
#endif
#endif

#endif

#define OSAPIVERSION			 "OSP.Small.KDV.4.0.1.1.0.20080628"	/* OSAPI Version */
#define OSAPI_VERSION_ID			 0x40					 /* Version 3.0 */

#define  OSAPI_MAX_TIMER_NUM 		     5000;   /* only support for 1~5000, 5001~9999 reserved */
#define  OSAPI_MAX_SERIALPORT_NUM	     10      /* maxium serial port supported */
#define  OSAPI_MAX_LOG_MSG_LEN          6000    //每条日志的最大长度


#define TCPDISCONNECTTIME		 10 /* seconds now just for debug */
#define UNDISCONNDETECT 		 0	/* unable disconnect detect */
#define TCPHBNUM				 3	/* disconnect heartbeat times */
#define STATUSMSGOUTTIME		 3600 /* the status message output timer seconds */


#ifndef OSAPIMINCHAR
#define OSAPIMINCHAR 				0x80	  
#endif

#ifndef OSAPIMAXCHAR
#define OSAPIMAXCHAR 				0x7f		
#endif

#ifndef OSAPIMINSHORT
#define OSAPIMINSHORT				0x8000		
#endif

#ifndef OSAPIMAXSHORT
#define OSAPIMAXSHORT				0x7fff		
#endif

#ifndef OSAPIMINLONG
#define OSAPIMINLONG 				0x80000000	
#endif

#ifndef OSAPIMAXLONG
#define OSAPIMAXLONG 				0x7fffffff	
#endif

#ifndef OSAPIMAXBYTE
#define OSAPIMAXBYTE 				0xff		
#endif

#ifndef OSAPIMAXWORD
#define OSAPIMAXWORD 				0xffff		
#endif

#ifndef OSAPIMAXDWORD
#define OSAPIMAXDWORD				0xffffffff	
#endif

#ifndef _MSC_VER


/* Provider capabilities flags. */


#define OSAPI_PCF_DTRDSR		  ((u32)0x0001)
#define OSAPI_PCF_RTSCTS		  ((u32)0x0002)
#define OSAPI_PCF_RLSD		      ((u32)0x0004)
#define OSAPI_PCF_PARITY_CHECK    ((u32)0x0008)
#define OSAPI_PCF_XONXOFF   	  ((u32)0x0010)
#define OSAPI_PCF_SETXCHAR  	  ((u32)0x0020)
#define OSAPI_PCF_TOTALTIMEOUTS   ((u32)0x0040)
#define OSAPI_PCF_INTTIMEOUTS     ((u32)0x0080)
#define OSAPI_PCF_SPECIALCHARS    ((u32)0x0100)
#define OSAPI_PCF_16BITMODE  	  ((u32)0x0200)


/* Comm provider settable parameters. */

#define OSAPI_SP_PARITY		  ((u32)0x0001)
#define OSAPI_SP_BAUD 		  ((u32)0x0002)
#define OSAPI_SP_DATABITS 	  ((u32)0x0004)
#define OSAPI_SP_STOPBITS 	  ((u32)0x0008)
#define OSAPI_SP_HANDSHAKING  ((u32)0x0010)
#define OSAPI_SP_PARITY_CHECK ((u32)0x0020)
#define OSAPI_SP_RLSD 		  ((u32)0x0040)


/* Settable baud rates in the provider. */


#if defined(_LINUX_) && !defined(_EQUATOR_)
#define OSAPI_BAUD_075		  ((u32)0x00000001)
#define OSAPI_BAUD_110		  ((u32)0x00000002)
#define OSAPI_BAUD_134_5	  ((u32)0x00000004)
#define OSAPI_BAUD_150		  ((u32)0x00000008)
#define OSAPI_BAUD_300		  ((u32)0x00000010)
#define OSAPI_BAUD_600		  ((u32)0x00000020)
#define OSAPI_OSAPI_BAUD_1200 ((u32)0x00000040)
#define OSAPI_BAUD_1800		  ((u32)0x00000080)
#define OSAPI_BAUD_2400		  ((u32)0x00000100)
#define OSAPI_BAUD_4800		  ((u32)0x00000200)
#define OSAPI_BAUD_7200		  ((u32)0x00000400)
#define OSAPI_BAUD_9600		  ((u32)0x00000800)
#define OSAPI_BAUD_14400	  ((u32)0x00001000)
#define OSAPI_BAUD_19200	  ((u32)0x00002000)
#define OSAPI_BAUD_38400	  ((u32)0x00004000)
#define OSAPI_BAUD_56K		  ((u32)0x00008000)
#define OSAPI_BAUD_128K		  ((u32)0x00010000)
#define OSAPI_BAUD_115200 	  ((u32)0x00020000)
#define OSAPI_BAUD_57600	  ((u32)0x00040000)
#define OSAPI_BAUD_USER		  ((u32)0x10000000)
#endif

/* Equator上只能设置波特率，其他的都不能设置 */
#ifdef _EQUATOR_
/*  注意：最高速率为9600 */
#define OSAPI_BAUD_110		  (u16)110
#define OSAPI_BAUD_300		  (u16)300
#define OSAPI_BAUD_600		  (u16)600
#define OSAPI_BAUD_1200		  (u16)1200
#define OSAPI_BAUD_2400		  (u16)2400
#define OSAPI_BAUD_4800		  (u16)4800
#define OSAPI_BAUD_9600		  (u16)9600
#endif

/* DTR Control Flow Values. */

#define OSAPI_DTR_CONTROL_DISABLE		0x00
#define OSAPI_DTR_CONTROL_ENABLE		0x01
#define OSAPI_DTR_CONTROL_HANDSHAKE 	0x02


/* RTS Control Flow Values */

#define OSAPI_RTS_CONTROL_DISABLE		0x00
#define OSAPI_RTS_CONTROL_ENABLE		0x01
#define OSAPI_RTS_CONTROL_HANDSHAKE	    0x02
#define OSAPI_RTS_CONTROL_TOGGLE		0x03

#define OSAPI_NOPARITY				0
#define OSAPI_ODDPARITY				1
#define OSAPI_EVENPARITY			2
#define OSAPI_MARKPARITY			3
#define OSAPI_SPACEPARITY 			4

#define OSAPI_ONESTOPBIT			0
#define OSAPI_ONE5STOPBITS			1
#define OSAPI_TWOSTOPBITS 			2


/* Escape Functions */


#define OSAPI_SETXOFF 				1		// Simulate XOFF received
#define OSAPI_SETXON				2		// Simulate XON received
#define OSAPI_SETRTS				3		// Set RTS high
#define OSAPI_CLRRTS				4		// Set RTS low
#define OSAPI_SETDTR				5		// Set DTR high
#define OSAPI_CLRDTR				6		// Set DTR low
#define OSAPI_RESETDEV				7		// Reset device if possible
#define OSAPI_SETBREAK				8		// Set the device break line.
#define OSAPI_CLRBREAK				9		// Clear the device break line.


/* PURGE function flags. */

#define OSAPI_PURGE_TXABORT			0x0001	/*  Kill the pending/current writes to the comm port. */
#define OSAPI_PURGE_RXABORT			0x0002	/* Kill the pending/current reads to the comm port. */
#define OSAPI_PURGE_TXCLEAR			0x0004	/* Kill the transmit queue if there. */
#define OSAPI_PURGE_RXCLEAR			0x0008	/* Kill the typeahead buffer if there. */

typedef struct 
{  
	u32 ReadIntervalTimeout; 
	u32 ReadTotalTimeoutMultiplier; 
	u32 ReadTotalTimeoutConstant; 
	u32 WriteTotalTimeoutMultiplier; 
	u32 WriteTotalTimeoutConstant; 
} TOsApiCommTimeouts; 

typedef struct
{ 
	u32 DCBlength;			 /*  sizeof(DCB) */
	u32 BaudRate;			 /*  current baud rate */
	u32 fBinary: 1; 		 /*  binary mode, no EOF check */
	u32 fParity: 1; 		 /*  enable parity checking */
	u32 fOutxCtsFlow:1; 	 /*  CTS output flow control */
	u32 fOutxDsrFlow:1; 	 /*  DSR output flow control */
	u32 fDtrControl:2;		 /*  DTR flow control type */
	u32 fDsrSensitivity:1;	 /*  DSR sensitivity */
	u32 fTXContinueOnXoff:1; /* XOFF continues Tx */
	u32 fOutX: 1;			 /*  XON/XOFF out flow control */
	u32 fInX: 1;			 /*  XON/XOFF in flow control */
	u32 fErrorChar: 1;		 /*  enable error replacement */
	u32 fNull: 1;			 /*  enable null stripping */
	u32 fRtsControl:2;		 /*  RTS flow control */
	u32 fAbortOnError:1;	 /*  abort on error */
	u32 fDummy2:17; 		 /*  reserved */
	u32 wReserved;			 /*  not currently used */
	u16 XonLim; 			 /*  transmit XON threshold */
	u16 XoffLim;			 /*  transmit XOFF threshold */
	u8	ByteSize;			 /*  number of bits/Byte, 4-8 */
	u8	Parity; 			 /*  0-4=no,odd,even,mark,space */
	u8	StopBits;			 /*  0,1,2 = 1, 1.5, 2 */
	char XonChar;			 /*  Tx and Rx XON character */
	char XoffChar;			 /*  Tx and Rx XOFF character */
	char ErrorChar; 		 /*  error replacement character */
	char EofChar;			 /*  end of input character */
	char EvtChar;			 /*  received event character */
	u16  wReserved1;		 /* reserved; do not use */
}TOsApiDCB; 

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
} TOsApiComStat; 

#endif /*#ifndef _MSC_VER */

/*=============================================================================
函 数 名：OsApi_help
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
API void OsApi_help(void);

/*====================================================================
函数名：OsApi_SemBCreate
功能：创建一个二元信号量
算法实现：（可选项）
引用全局变量：
输入参数说明：(out)ptSemHandle 返回的信号量句柄

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 OsApi_SemBCreate(OSAPISEMHANDLE *ptSemHandle);

/*====================================================================
函数名：OsApi_SemCCreate
功能：创建计数信号量
算法实现：（可选项）
引用全局变量：
输入参数说明：(out)ptSemHandle 信号量句柄返回参数， 
(in)initcount 初始计数，
(in)MaxSemCount 最大计数

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API BOOL32 OsApi_SemCCreate(OSAPISEMHANDLE *ptSemHandle,u32 initcount,u32 MaxSemCount);

/*====================================================================
函数名：OsApi_SemDelete
功能：删除信号量
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 待删除信号量的句柄

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API  BOOL32 OsApi_SemDelete(OSAPISEMHANDLE tSemHandle );

/*====================================================================
函数名：OsApi_SemTake
功能：信号量p操作，如该信号量不可用，则阻塞调用实例所在任务(APP)
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 信号量句柄

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API BOOL32	OsApi_SemTake(OSAPISEMHANDLE tSemHandle );

/*====================================================================
函数名：OsApi_SemTakeByTime
功能：带超时的信号量p操作，如信号量不可用，则时间消逝dwtime毫秒后，出错返回
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 信号量句柄, (in)dwtime 超时间隔(以ms为单位)

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API BOOL32	OsApi_SemTakeByTime(OSAPISEMHANDLE tSemHandle, u32 dwtime);

/*====================================================================
函数名：OsApi_SemGive
功能：信号量v操作
算法实现：（可选项）
引用全局变量：
输入参数说明：(in)tSemHandle 信号量句柄

 返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
API  BOOL32 OsApi_SemGive(OSAPISEMHANDLE tSemHandle );

/*====================================================================
函数名：OsApi_TaskCreate
功能：创建并执行一个任务
算法实现：（可选项）
引用全局变量：
输入参数说明：
     上层在调用该函数时，统一先将要传入的函数指针转换为u32，再转换为(void *)，例如：
     
     API void MyTask(void)
     {...}

     ......
  
     OsApi_TaskCreate((void *)(u32)MyTask, ...);

 返回值说明：
====================================================================*/
API OSAPITASKHANDLE OsApi_TaskCreate(void* pvTaskEntry, char* szName, u8 byPriority, u32 dwStacksize, u32 dwParam, u16 wFlag, u32 *puTaskID);

/*====================================================================
函数名：OsApi_TaskExit
功能：退出调用任务，任务退出之前用户要注意释放本任务申请的内存、信号量等资源。
封装Windows的ExitThread(0)和vxWorks的taskDelete(0)。
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskExit(void);

/*====================================================================
函数名：OsApi_TaskWaitEnd
功能：等待指定任务结束。
算法实现：（可选项）
引用全局变量：
输入参数说明：handle--待结束任务的handle

 返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskWaitEnd(OSAPITASKHANDLE handle);

/*====================================================================
函数名：OsApi_TaskTerminate
功能：强行终止指定任务。
封装Windows的ExitThread()和vxWorks的taskDelete()。
算法实现：（可选项）
引用全局变量：
输入参数说明：handle--待杀任务的handle

 返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskTerminate(OSAPITASKHANDLE handle);

/*====================================================================
函数名：OsApi_TaskSuspend
功能：挂起当前任务
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskSuspend(OSAPITASKHANDLE handle);

/*====================================================================
函数名：OsApi_TaskResume
功能：使得先前挂起的任务继续执行
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskResume(OSAPITASKHANDLE handle);

/*====================================================================
函数名：OsApi_TaskSetPriority
功能：改变任务的优先级。

 算法实现：（可选项）
 引用全局变量：
 输入参数说明：
 
  返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskSetPriority(OSAPITASKHANDLE taskHandle, u8 newPriority);

/*====================================================================
函数名：OsApi_TaskGetPriority
功能：获得任务的优先级。

 算法实现：（可选项）
 引用全局变量：
 输入参数说明：
 
  返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskGetPriority(OSAPITASKHANDLE taskHandle, u8 * puPri);

/*====================================================================
函数名：OsApi_TaskSelfHandle
功能：获得调用任务的句柄
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API OSAPITASKHANDLE OsApi_TaskSelfHandle(void);

/*====================================================================
函数名：OsApi_TaskSelfID
功能：获得调用任务的ID
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API u32 OsApi_TaskSelfID(void);

/*====================================================================
函数名：OsApi_TaskHandleVerify
功能：判断指定任务是否存在
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_TaskHandleVerify(OSAPITASKHANDLE handle);

/*====================================================================
函数名：OsApi_TaskSafe
功能：保护调用任务不被非法删除
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OsApi_TaskSafe(void);

/*====================================================================
函数名：OsApi_TaskUnsafe
功能：释放调用任务的删除保护，使得调用任务可以被安全删除
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OsApi_TaskUnsafe(void);

/*====================================================================
函数名：OsApi_TaskDelay
功能：任务延时
算法实现：（可选项）
引用全局变量：
输入参数说明：uMseconds--延时间隔(ms)

 返回值说明：
====================================================================*/
#define OsApi_Delay					  OsApi_TaskDelay

API void OsApi_TaskDelay(u32 dwMseconds);

/*====================================================================
函数名：OsApi_CreateMailbox
功能：创建消息队列
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_CreateMailbox(char* szName,u32 dwMsgNumber,u32 dwMsgLength ,u32 *puQueueIDRead,u32 *puQueueIDWrite);

/*====================================================================
函数名：OsApi_CloseMailbox
功能：关闭邮箱
算法实现：（可选项）
引用全局变量：
输入参数说明：uQueueIDRead, uQueueIDWrite--邮箱的读、写句柄

 返回值说明：
====================================================================*/
API void OsApi_CloseMailbox(u32 dwQueueIDRead, u32 dwQueueIDWrite);

/*====================================================================
函数名		：OsApi_SndMsg
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

 返回值说明  ：成功返回TRUE, 失败或超时返回FALSE. timeout=2000
====================================================================*/					
API BOOL32 OsApi_SndMsg(u32 tQueueID, const char *lpMsgBuf, u32 len, int timeout);

/*====================================================================
函数名		：OsApi_RcvMsg
功能		：从信箱收消息
算法实现	：
引用全局变量：无
输入参数说明：
OSTOKEN tQueueID	信箱ID
u32  dwTimeout	 超时(毫秒)
LPVOID *lpMsgBuf	返回消息内容指针
返回值说明	：
====================================================================*/
API BOOL32 OsApi_RcvMsg(u32 tQueueID,u32  dwTimeout,char * lpMsgBuf, u32 len, u32 * pLenRcved);

/*====================================================================
函数名：OsApi_ClkRateGet
功能：得到tick精度：n ticks/sec
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API u32 OsApi_ClkRateGet();

/*====================================================================
函数名：OsApi_TickGet
功能：取得当前的tick数
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API u32 OsApi_TickGet();

/*====================================================================
	套接字封装：用户使用套接字前必须先调用OsApi_SockInit()对套接字库进行初始化，
	使用完之后则需要调用OsApi_SockCleanup()释放对套接字库的使用。由于vxWorks和
	Windows的关闭套接字的函数不同，而且在调用socket、connect等函数时，返回
	值类型也不一样，OSP也进行了必要的封装，原则是保持与Windows下基本一致。
====================================================================*/

/*====================================================================
函数名：OsApi_SockInit
功能：套接字初始化，封装Windows下的WSAStartup，vxWorks下返回TRUE
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 OsApi_SockInit(void);


/*====================================================================
函数名：OsApi_SockClose
功能：关闭套接字，封装windows下的closesocket和vxworks下的close.
算法实现：（可选项）
引用全局变量：
输入参数说明：tSock--待关闭套接字的句柄（调用socket时返回）。

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 OsApi_SockClose(OSAPISOCKHANDLE tSock);

/*====================================================================
函数名：OsApi_SockCleanup
功能：套接字销毁，封装windows的WSACleanup，vxWorks下返回TRUE
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：成功返回TRUE，失败返回FALSE
====================================================================*/
API BOOL32 OsApi_SockCleanup(void);

/*====================================================================
函数名：IsOsApi_Initd
功能：判断OSP是否已初始化过了
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：OSP已初始化，返回TRUE；否则返回FALSE.
====================================================================*/
API BOOL32 IsOsApi_Initd(void);

/*=============================================================================
函 数 名：OsApi_Init
功	  能：initialize OSP module
注	  意：
算法实现：OsApi_Init内部自动初始化了定时器线程
全局变量：
参	  数：TelNetEnable,telnet是否有效，TelNetPort，telnet端口，如果为设为0表示由OSAPI自动选择可用端口（2500开始）
返 回 值：成功返回TRUE，否则返回FALSE
=============================================================================*/
API BOOL32 OsApi_Init(BOOL32 TelNetEnable, u16 TelNetPort);

/*====================================================================
函数名：OsApi_Quit
功能：退出OSP，调用本地所有APP的所有实例的InstanceExit方法，退出实例，
断开所有OSP结点，挂起所有OSP任务包括APP，然后返回。OSP退出之后，
所有OSP服务将不能使用。
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API void OsApi_Quit(void);

/*=============================================================================
函 数 名：OsApi_AddrListGet
功	  能：获取本机地址表内容
注	  意：如果是在Windows下使用本函数，必须先调用OsApi_Init，或者WSAStartup。
本函数效率较低，不要重复调用。
算法实现：
全局变量：
参	  数： u32	 adwIP[] : [in/out]用户申请的用于地址表内容的数组的首地址
u16   dNum : [in]用户申请的数组的大小
返 回 值： 返回本机地址表中地址个数，错误时返回0
=============================================================================*/
API u16  OsApi_AddrListGet(u32 adwIPList[], u16 dwNum);


/*=============================================================================
函 数 名：OsApi_VerPrintf, OsApi_ver
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
#define  OsApi_VerPrintf() \
OsApi_Printf(TRUE, TRUE, "OsApi version: %s  compile time: %s %s\n", OSAPIVERSION, __DATE__, __TIME__)
API  void OsApi_ver();

/*====================================================================
  函数名：OsApi_RegCommand
  功能：注册可以在Telnet下执行的函数
  算法实现：
  引用全局变量：
  输入参数说明：
  	szName	- Telnet下输入的命令
	pfFunc	- 对应的函数指针
	szUsage	- 命令的帮助信息
  返回值说明：无
  ====================================================================*/
API void OsApi_RegCommand(const char* szName, void* pfFunc, const char* szUsage);

/* 串口封装：以Windows串口操作函数为准，在VxWorks下提供相应函数 */
/*====================================================================
函数名：OsApi_SerialOpen
功能：打开指定的串口
算法实现：（可选项）
引用全局变量：
输入参数说明：byPort: 待打开的串口号
约定：console口为ttyS0，com1口为ttyS1，com2口为ttyS2，...，以下类推
返回值说明：成功返回打开串口的句柄，失败返回INVALID_SERIALHANDLE
====================================================================*/
API OSAPISERIALHANDLE OsApi_SerialOpen(u8 byport);

/*====================================================================
函数名：OsApi_SerialClose
功能：关闭指定的串口
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_SerialClose(OSAPISERIALHANDLE hCom);

/*====================================================================
函数名：OsApi_SerialRead
功能：从指定的串口读数据
算法实现：（可选项）
引用全局变量：
输入参数说明：hCom: 串口句柄,
              pchBuf: 存放读出数据的buffer,
              dwBytesToRead: 要读出的u8数,
              pdwBytesRead: 返回实际读出的u8数.

  返回值说明：成功返回TRUE, 失败返回FALSE.
====================================================================*/
API BOOL32 OsApi_SerialRead(OSAPISERIALHANDLE hCom, char *pchBuf, u32 dwBytesToRead, u32 *pdwBytesRead);

/*====================================================================
函数名：OsApi_SerialWrite
功能：向指定的串口写数据
算法实现：（可选项）
引用全局变量：
输入参数说明：hCom: 串口句柄,
              pchBuf: 待写数据的buffer,
              uBytesToWrite: 要写的u8数,
              puBytesWrite: 返回实际写入的u8数.

  返回值说明：成功返回TRUE, 失败返回FALSE.
====================================================================*/
API BOOL32 OsApi_SerialWrite(OSAPISERIALHANDLE hCom, char *pchBuf, u32 dwBytesToWrite, u32 *pdwBytesWrite);

/*====================================================================
函数名：OsApi_SetCommTimeouts
功能：设置串口超时
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_SetCommTimeouts(OSAPISERIALHANDLE hCom, TOsApiCommTimeouts *ptCommTimeouts);

/*====================================================================
函数名：OsApi_GetCommState
功能：获得串口状态
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_GetCommState(OSAPISERIALHANDLE hCom, TOsApiDCB *ptDCB);

/*====================================================================
函数名：OsApi_SetCommState
功能：设置串口状态（linux下目前仅设置波特率部分有效。）
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_SetCommState(OSAPISERIALHANDLE hCom, TOsApiDCB *ptDCB);

/*====================================================================
函数名：OsApi_EscapeCommFunction
功能：使用串口扩展功能
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_EscapeCommFunction(OSAPISERIALHANDLE hCom, u32 dwFunc);

/*====================================================================
函数名：OsApi_PurgeComm
功能：丢弃所有未决数据，终止串口读写操作
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_PurgeComm(OSAPISERIALHANDLE hCom, u32 dwFlags);

/*====================================================================
函数名：OsApi_ClearCommError
功能：与Windows下ClearCommError相同
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_ClearCommError(OSAPISERIALHANDLE hCom, u32 *pdwErrors, TOsApiComStat *ptStat);

/*====================================================================
函数名：OsApi_GetCommTimeouts
功能：取得打开串口的超时设置
算法实现：（可选项）
引用全局变量：
输入参数说明：

 返回值说明：
====================================================================*/
API BOOL32 OsApi_GetCommTimeouts(OSAPISERIALHANDLE hCom, TOsApiCommTimeouts *ptCommTimeouts);

/********************************************************************
    Telnet module
********************************************************************/

/*====================================================================
函数名：OsApi_TcpOpen
功能：在指定地址和端口建立监听sock
算法实现：（可选项）
引用全局变量：
输入参数说明：无

返回值说明：返回监听的socket
====================================================================*/
API OSAPISOCKHANDLE OsApi_TcpOpen(u32 dwIpAddr, u16 wPort);

/*====================================================================
函数名：OsApi_TcpListen
功能：在指定sock上进行监听
算法实现：（可选项）
引用全局变量：
输入参数说明：无

返回值说明：0表示成功,OSAPI_TELNET_LISTEN_FAIL表示失败
====================================================================*/
API s32 OsApi_TcpListen(OSAPISOCKHANDLE hSock, s32 nQueue);

/*====================================================================
函数名：OsApi_sockBytesAvailable
功能：返回指定sock的每次recv时能够读取的输入缓存的大小
算法实现：（可选项）
引用全局变量：无
输入参数说明：

返回值说明：大于等于0为实际能读取的大小，小于0为相应的失败错误码
====================================================================*/
API s32 OsApi_sockBytesAvailable(OSAPISOCKHANDLE hSocket);

/*====================================================================
函数名：OsApi_Printf
功能：把相应的内容显示到屏幕,存储到文件,不能屏蔽
算法实现：（可选项）
引用全局变量：
输入参数说明：
bScreen: 是否输出到屏幕,
bFile: 是否输出到文件,
szFormat: 格式,
返回值说明：
====================================================================*/
API void OsApi_Printf(BOOL32 bScreen, BOOL32 bFile, char *szFormat, ...);

/*====================================================================
函数名：OsApi_SetPrompt
功能：设置Telnet提示符, Windows下适用; Osp取模块名为缺省提示符.
算法实现：（可选项）
引用全局变量：g_Osp
输入参数说明：prompt: 新的提示符

  返回值说明：成功返回TRUE, 失败返回FALSE.
====================================================================*/
API BOOL32 OsApi_SetPrompt(const s8 *prompt);



/********************************************************************
    Timer Module
********************************************************************/
typedef void* HTIMERHANDLE;

typedef s32 (*OsApi_TimerHandler)(HTIMERHANDLE dwTimerId, void* param);

/*====================================================================
函数名：OsApi_TimerInit
功能：定时器初始化。
算法实现：（可选项）
引用全局变量：g_tTimerMgr
输入参数说明：hHandle: 定时器句柄

返回值说明：0表示初始化成功,其他值表示错误码
====================================================================*/
API u16 OsApi_TimerInit(s32 nTotalNum);

/*====================================================================
函数名：OsApi_TimerEnd
功能：终止定时器线程。
算法实现：（可选项）
引用全局变量：无
输入参数说明：

返回值说明：0 表示终止成功,其他值表示错误码
====================================================================*/
API u16 OsApi_TimerEnd();

/*====================================================================
函数名：OsApi_TimerNew
功能：申请新的OSP定时器。
算法实现：（可选项）
引用全局变量：g_tTimerMgr
输入参数说明：

返回值说明：0 表示申请成功,其他值表示错误码
====================================================================*/
API u16 OsApi_TimerNew(HTIMERHANDLE* pHandle);

/*====================================================================
函数名：OsApi_TimerDelete
功能：删除OSP定时器。
算法实现：（可选项）
引用全局变量：无
输入参数说明：

返回值说明：0 表示删除成功,其他值表示错误码
====================================================================*/
API u16 OsApi_TimerDelete(HTIMERHANDLE hHandle);

/*====================================================================
函数名：OsApi_TimerSet
功能：设置OSP定时器参数。
算法实现：（可选项）
引用全局变量：无
输入参数说明：

返回值说明：0 表示设置成功,其他值表示错误码
====================================================================*/
API u16 OsApi_TimerSet(HTIMERHANDLE hHandle, u32 dwInterval, OsApi_TimerHandler pCallBack, void* context);


/*====================================================================
函数名：OsApi_TimerInfo
功能：获得OSP定时器信息。
算法实现：（可选项）
引用全局变量：无
输入参数说明：

返回值说明：0 表示获取成功,其他值表示错误码
====================================================================*/
API u16 OsApi_TimerInfo(HTIMERHANDLE hHandle, u32* pdwInterval, OsApi_TimerHandler* pCallBack,
                     void** pcontext, u32* pState);

/*====================================================================
函数名：OsApi_TimerStop
功能：改变指定句柄的OSP定时器状态为停止。
算法实现：（可选项）
引用全局变量：无
输入参数说明：

返回值说明：0 表示改变状态成功,其他值表示错误码
====================================================================*/
API u16 OsApi_TimerStop(HTIMERHANDLE hHandle);

/*====================================================================
函数名：OsApi_TimerRunCount
功能：定时器有效运行计数。
算法实现：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明：返回定时器线程有效运行状态时内部计数值
====================================================================*/
API u32 OsApi_TimerRunCount();

/********************************************************************
    OSAPI module
********************************************************************/

/* CPU信息 */
typedef struct
{
	/* 空闲百分比，0－100 */
	u8 m_byIdlePercent;
}TOsApiCpuInfo;
/* 内存信息 */
typedef struct
{
	/* 物理内存大小(KByte) */
	u32 m_dwPhysicsSize;
	/* 已使用内存(KBytes) */
	u32 m_dwAllocSize;
	/* 可用内存(KBytes) */
	u32 m_dwFreeSize;
}TOsApiMemInfo;

/* 时间信息 */
typedef struct
{
	/* 年(2006) */
	u16 m_wYear;
	/* 月(1--12) */
	u16 m_wMonth;
	/* (1--31) */
	u16 m_wDay;
	/* 时(0--23) */
	u16 m_wHour;
	/* 分(0--59) */
	u16 m_wMinute;
	/* 秒(0--59) */
	u16 m_wSecond;
}TOsApiTimeInfo;

typedef struct tagTThreadInfoRD
{
	char m_strThreadName[100];				/* 线程名称 */
	u32  m_dwThreadId;						/* 线程ID */
}TThreadInfoRecord;

/* getpid() --获得当前线程ID函数*/
#ifdef WIN32
	#define  getpid() GetCurrentThreadId()
#else
#endif


/*====================================================================
函数名：OsApi_GetMemInfo
功能：取得当前的Mem信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptMemInfo，用户MEM信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OsApi_GetMemInfo( TOsApiMemInfo* ptMemInfo );

/*====================================================================
函数名：OsApi_GetCpuInfo
功能：取得当前的CPU信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptCpuInfo，用户CPU信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OsApi_GetCpuInfo( TOsApiCpuInfo* ptCpuInfo );

/*====================================================================
函数名：OsApi_GetTimeInfo
功能：取得当前的Time信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptTimeInfo，系统time信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
====================================================================*/
API BOOL32 OsApi_GetTimeInfo( TOsApiTimeInfo* ptTimeInfo );

/*====================================================================
函数名：OsApi_SetTimeInfo
功能：设置当前的Time信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptTimeInfo，系统time信息结构
返回值说明：为TRUE时表示设置信息成功，否则表示失败
====================================================================*/
API BOOL32 OsApi_SetTimeInfo( TOsApiTimeInfo* ptTimeInfo );

/*====================================================================
函数名：OsApi_AddThreadInfo
功能：设置线程信息到OSAPI中
算法实现：（可选项）
引用全局变量：
输入参数说明：ptThreadInfo，线程信息结构指针
返回值说明：为TRUE时表示设置信息成功，否则表示失败
====================================================================*/
API BOOL32 OsApi_AddThreadInfo( TThreadInfoRecord* ptThreadInfo );

/*====================================================================
函数名：OsApi_DelThreadInfo
功能：从OSAPI中删除指定线程ID的信息
算法实现：（可选项）
引用全局变量：
输入参数说明：dwThreadId，线程ID
返回值说明：为TRUE时表示删除信息成功，否则表示失败
====================================================================*/
API BOOL32 OsApi_DelThreadInfo( u32  dwThreadId );

/*====================================================================
函数名：OsApi_PrintThreadInfo
功能：  将当前已设置到OSP中的线程信息打印显示
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明：无
====================================================================*/
API void OsApi_PrintThreadInfo();


#endif
