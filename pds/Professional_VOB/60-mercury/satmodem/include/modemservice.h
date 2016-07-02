
#ifndef MODEM_SERVICE_H
#define MODEM_SERVICE_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


#ifdef _VXWORKS_
    #include "tickLib.h"
    #include "iolib.h"
    #include "types.h"
    //#include <mbuf.h>
    #include "socket.h"
    #include "in.h"
    #include "socklib.h"
    #include "msgQLib.h"
    #include "netinet\tcp.h"
    #include "ctype.h"
    #include "inetlib.h"
    //#include <socketvar.h>
#endif

#ifdef WIN32
    #include "winsock2.h"
    #include "Windows.h"
#endif

#ifdef _LINUX_

#endif

#define MAXNUM_CMD		(u8)100

#include "kdvtype.h"
#include "modeminterface.h"



#define THREAD_STACK_SIZE   400 << 10       /*< 线程堆栈的大小 */
#define CMDTABLE_ITEM_NUM   1 * 256         /*< 定义为256的整数倍，映射一个网段的IP */
#define PROTLPACK_MAXSIZE   150             /*< 协议包的最大长度 */

#define MAX_LOG_MSG_LEN		1000			/*  调试消息输出缓冲的最大长度 */


#define MONITOR_INTERVAL    2               /* Monitor线程的扫描时间间隔，100ms的倍数 */
#define COMMAND_FAILNUM     2               /* 出现错误的重试次数；不包括第一次发送 */


#define debug               //printf


typedef struct MsgPipe 
{
#ifdef WIN32
    HANDLE read;                            /*< 管道的读句柄 */
    HANDLE write;                           /*< 管道的写句柄 */
    CRITICAL_SECTION readCS;                /*< 保护管道的写 */
#endif

#ifdef _VXWORKS_
    MSG_Q_ID pipe;
#endif

#ifdef _LINUX_
    //int anPipe[2];
	MdcomCmd		cmdQueue[MAXNUM_CMD];
	s32             m_nHead;
    s32             m_nTail;
#endif

}MsgPipe;


/* 命令表及命令项的定义 */
typedef enum InCmdStatus
{
    ics_invalid = 0,                        /*< 此项无效 */
    ics_ready,                              /*< 命令等待发送 */
    ics_send,                               /*< 命令已经发出 */
    ics_receive,                            /*< 命令已接收 */
    ics_equal,                              /*< 结果一致 */
    ics_unequal                             /*< 结果不一致 */
} InCmdStatus;

typedef struct InCmd
{
    u32 set;                                /*< 要设置的命令的内容 */
    u32 ret;                                /*< 回应命令的内容 */
    u32 tick;                               /*< 发出命令的时间 */
    InCmdStatus  status;                    /*< 命令执行的状态 */
    u8  failnum;                            /*< 执行命令失败次数 */
    u8  trynum;                             /*< 连发次数 */
    u8  triednum;                           /*< 已经连发次数 */
	u8  query;								/*< 0: set 1: query */
} InCmd;

typedef struct CmdTableItem
{
    u32 ip;                                 /*< 对应MODEM的IP */
    u16 port;                               /*< 端口暂时不用 */
    InCmd cmds[MDCOM_SET_MAX];              /*< 每个MODEM上的7个命令；位置决定命令号 */
} CmdTableItem;


#ifndef WIN32
    typedef int                   HANDLE;
    typedef int                   SOCKET;
    #define INVALID_SOCKET        ERROR
    #define SOCKET_ERROR          ERROR
#endif

typedef struct ThreadData
{
    HANDLE handle;                                  /* 线程的句柄 */
    int terminate;                                  /* 通知线程退出 */
#ifdef _VXWORKS_
    char name[20];
#endif
#ifdef _LINUX_
	char name[20];
#endif
} ThreadData;

/* 所有TICK相关的常量均使用此结构，因为VXWORKS上的毫秒数是会变化的，
 * 没办法使用常量定义时间 
 */
typedef struct MdcomTime
{
    u16     interval;                               /* 连发的时间间隔*/
    u16     timeout;                                /* 单次发送的超时时限 */
} MdcomTime;

typedef struct MdcomEnv
{
    SOCKET  sock;                                   /* 发送UDP的SOCKET */
    MsgPipe pipe;                                   /* 命令传递管道 */
    ThreadData thread[3];                           /* 共有三个线程 */
    CmdTableItem table[CMDTABLE_ITEM_NUM];          /* 命令表 */
    MdcomTime time;                                 /* 时间相关的常量 */
	int     log;								    /* 是否需要运行信息：0－不需要 */
} MdcomEnv;

typedef struct MdcomStat
{
	int send;
	int recv;
	int fail;
	int succ;	
}MdcomStat;


u32		MdcomSendTask(void *data);
u32		MdcomRevTask(void *data);
u32		MdcomMonitorTask(void *data);
int		DumpMsg(const u8* msg, int msglen, char* buf);

const char* ip2String(struct in_addr addr);
const char* ip2String_u32(u32 dwIP );

API void pmdmpxymsg( void );
API void npmdmpxymsg( void );
API void setmdmpxylog( u8 byLevel );

#ifdef __cplusplus
}
#endif


#endif



