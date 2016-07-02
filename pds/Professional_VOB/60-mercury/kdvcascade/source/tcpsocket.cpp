
/*lint -save -e749*/
/*lint -save -e751*/
/*lint -save -e737*/
/*lint -save -e703*/
/*lint -save -e573*/
#include "osp.h"
#include "tcpsocket.h"

#ifdef _VXWORKS_
#include <sockLib.h>
#include <netinet/tcp.h>
#ifdef NULL
#undef NULL
#endif
#define NULL 0

/*
#include <mbuf.h>
#include <socketvar.h>
*/
#endif

#include "h323mcuconn.h"
#include "kdvlog.h"
#include "loguserdef.h"

void mmculog(s32 nLevel, s8 * format,...);

#define MAX_WATCH_LEN  (s32)32 //最大的watch数据包长度


typedef enum 
{
	SOCK_ADD    = 0,   //MSGID+HNODE+HAPPNODE
	SOCK_RCV    = 1, 
	SOCK_STOP   = 2, 
	SOCK_DEL    = 3, 
	SOCK_EXIT   = 4,
	SOCK_EVENT  = 5    //MSGID+TTCPEvent;
}TSockState;


//全局接收socket列表结构
typedef struct tagSOCKLIST
{
	HNODE    m_ahNode[FD_SETSIZE];
	HAPPNODE m_ahAppNode[FD_SETSIZE];
	BOOL32   m_abListen[FD_SETSIZE];  
    u32      m_adwLocalIP[FD_SETSIZE];
    u16      m_awLocalPort[FD_SETSIZE];
    u32      m_adwPeerIP[FD_SETSIZE];
    u16      m_awPeerPort[FD_SETSIZE];
}TSockList;

static BOOL32       g_bInit			= FALSE;
static TASKHANDLE   g_hRcvTask		= FALSE;
static SOCKHANDLE   g_hWatchSocket  = INVALID_SOCKET;  //控制线程的socket
static SEMHANDLE    g_hExitTaskSem  = NULL;
static TSockList    g_tSockList;
//static BOOL32       g_bTaskRet      = FALSE;  
static SEMHANDLE    g_hSyncSem      = NULL;
static TTCPEvent    g_tEvent;       

#define  MAXTCPWATCHSOCKPORT  (u16)20599//max watchsock port
#define  MINTCPWATCHSOCKPORT  (u16)20500//min watchsock port
static u16          g_wWatchPort    = MINTCPWATCHSOCKPORT;
static u16			g_wLastBindLocalPort = 0;

BOOL32 SocketBindToCascIns(HNODE hSocket, HAPPNODE hCascIns)
{
    s32 nMaxSocketNum = sizeof(g_tSockList.m_ahNode)/sizeof(g_tSockList.m_ahNode[0]);
    s32 nIndex=0;
	for(; nIndex<nMaxSocketNum; nIndex++)
	{
		if(g_tSockList.m_ahNode[nIndex] == hSocket )
		{
            if (NULL != hCascIns)
            {
                CMCUCascade *pcSession = (CMCUCascade *)hCascIns;
                pcSession->m_dwLocalIP = g_tSockList.m_adwLocalIP[nIndex];
                pcSession->m_wLocalPort = g_tSockList.m_awLocalPort[nIndex];
                pcSession->m_dwPeerIP = g_tSockList.m_adwPeerIP[nIndex];
                pcSession->m_wPeerPort = g_tSockList.m_awPeerPort[nIndex];
            }
            
    		g_tSockList.m_ahAppNode[nIndex] = hCascIns;

			return TRUE;
		}
	}
	return FALSE;
}


// API void printnode()
// {
//     s32 nMaxSocketNum = sizeof(g_tSockList.m_ahNode)/sizeof(g_tSockList.m_ahNode[0]);
//     s32 nIndex;
//     for(nIndex = 0; nIndex<nMaxSocketNum; nIndex++)
//     {
//         if (g_tSockList.m_ahNode[nIndex] != 0)
//         {
//             u8* pchLocalIP = (u8*)&g_tSockList.m_adwLocalIP[nIndex];
//             u8* pchPeerIP = (u8*)&g_tSockList.m_adwPeerIP[nIndex];
//             OspPrintf(TRUE, FALSE, "[%d] Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u, socket:%u session:0x%x\n",
//                 nIndex,
//                 pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
//                 g_tSockList.m_awLocalPort[nIndex],
//                 pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
//                 g_tSockList.m_awPeerPort[nIndex],
//                 g_tSockList.m_ahNode[nIndex],
//                 g_tSockList.m_ahAppNode);
//         }
//     }
// }


static void* TcpSocketRcvTaskProc(void * pParam);
static BOOL32 TcpCreateWatchSocket()
{
	/*创建控制套接字*/
	if(g_hWatchSocket != INVALID_SOCKET)
	{
		SockClose(g_hWatchSocket);
		g_hWatchSocket = INVALID_SOCKET;  
	}
	g_hWatchSocket = socket(AF_INET, (s32)SOCK_DGRAM, 0);
	if( INVALID_SOCKET == g_hWatchSocket )
	{
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]CreateTcpWatchSocket sock Error\n");
		return FALSE;
	}
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family      = AF_INET; 
	addr.sin_addr.s_addr = 0;
	addr.sin_port        = htons(g_wWatchPort);
	
	while( (g_wWatchPort <= MAXTCPWATCHSOCKPORT)&&
		   (SOCKET_ERROR == bind(g_hWatchSocket, (sockaddr *)(void*)&addr, sizeof(SOCKADDR_IN))))
	{
		g_wWatchPort++;
		addr.sin_port    = htons(g_wWatchPort);
	}
	
	if(g_wWatchPort>MAXTCPWATCHSOCKPORT)
	{
		g_hWatchSocket = INVALID_SOCKET;  
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]CreateTcpWatchSocket bind Error\n");
		return FALSE;
	}

	return TRUE;
}

//发送控制消息												
static BOOL32 TcpSendCtlMsg(s32 nMsgId, u8 *pBuf=NULL, s32 nLen=0)
{
	BOOL32 bRet = FALSE;
	
	if(g_hWatchSocket  == INVALID_SOCKET)
	{
		return bRet;
	}
	if((u32)nLen + sizeof(s32) > MAX_WATCH_LEN)
	{
		return bRet;
	}

	s32 nSndNum  = 0;
	
	SOCKADDR_IN  AddrIn;
	memset( &AddrIn, 0, sizeof(AddrIn));
    AddrIn.sin_family	   = AF_INET; 
    AddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");
    AddrIn.sin_port		   = htons(g_wWatchPort);
	
	u8  abyBuf[MAX_WATCH_LEN];
	*((u32 *)abyBuf) = nMsgId;
	if(pBuf != NULL)
	{
		memcpy(abyBuf+sizeof(s32), pBuf, nLen);
	}
    
	
    nSndNum = sendto( g_hWatchSocket, (s8*)abyBuf, (u32)nLen+sizeof(s32), 0, 
        (sockaddr *)(void*)&AddrIn, sizeof(SOCKADDR_IN) );
	
	if(nSndNum == (s32)((u32)nLen + sizeof(s32))) bRet = TRUE;

	return bRet;
}

static BOOL32 IsSocketFull()
{
    s32 nMaxSocketNum = sizeof(g_tSockList.m_ahNode)/sizeof(g_tSockList.m_ahNode[0]);
    s32 nIndex=0;
	for(; nIndex<nMaxSocketNum; nIndex++)
	{
		if(g_tSockList.m_ahNode[nIndex]==0)
		{
		   break;
		}
	}
	
	return (nIndex >= nMaxSocketNum);
}

static void DelNodeFromList(HNODE hNode)
{
	s32 nMaxSocketNum = sizeof(g_tSockList.m_ahNode)/sizeof(g_tSockList.m_ahNode[0]);
	for(s32 i=0; i<nMaxSocketNum; i++)
	{
		if(g_tSockList.m_ahNode[i]==hNode)
		{
            u8* pchLocalIP = (u8*)&g_tSockList.m_adwLocalIP[i];
            u8* pchPeerIP = (u8*)&g_tSockList.m_adwPeerIP[i];

//             mmculog(1, "[MMCU] Tcp close, Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u socket:%u session:0x%x\n",
//                     pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
//                     g_tSockList.m_awLocalPort[i],
//                     pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
//                     g_tSockList.m_awPeerPort[i],
//                     g_tSockList.m_ahNode[i],
//                     g_tSockList.m_ahAppNode);
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MMCULIB,"[MMCU] Tcp close, Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u socket:%u session:0x%x\n",
				pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
				g_tSockList.m_awLocalPort[i],
				pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
				g_tSockList.m_awPeerPort[i],
				g_tSockList.m_ahNode[i],
                g_tSockList.m_ahAppNode);
            
            SockClose(g_tSockList.m_ahNode[i]);
			g_tSockList.m_ahNode[i] = 0;
			break;
		}
	}	
}

static void TcpSetOpt(HNODE hSock)
{
	s32 optVal = 1;
    setsockopt(hSock, SOL_SOCKET, TCP_NODELAY, (char*)&optVal, sizeof(optVal));
  
	optVal = MAX_SOCKETBUFF_LEN;
    setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char *)&optVal, sizeof(optVal));
	
	optVal = 1;// 开启keepalive属性
    setsockopt(hSock, SOL_SOCKET, SO_KEEPALIVE , (char *)&optVal, sizeof(optVal));
		
#ifndef WIN32
	int keepidle     = 30;     // 如该连接在30秒内没有任何数据往来,则进行探测
	int keepinterval = 5;      // 探测时发包的时间间隔为5 秒
	int keepcount    = 3;      // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
	setsockopt(hSock, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle ));
	setsockopt(hSock, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval ));
	setsockopt(hSock, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount ));
#endif	

}

BOOL32 TcpSocketInit( BOOL32 bSockInit)
{
	if(g_bInit)
	{
		return TRUE;
	}
	if(bSockInit)
	{
		SockInit();		
	}

	if(!TcpCreateWatchSocket())
	{
		TcpSocketClean(bSockInit);
		return FALSE;
	}

	if(g_hExitTaskSem != NULL)
	{
		OspSemDelete(g_hExitTaskSem);
		g_hExitTaskSem = NULL;
	}
	if(!OspSemBCreate( &g_hExitTaskSem))
	{
		TcpSocketClean(bSockInit);
		OspPrintf(TRUE, TRUE, "\n TcpSocketInit OspSemBCreate g_hExitTaskSem Error  \n");
		return FALSE;
	}

	if(g_hSyncSem != NULL)
	{
		OspSemDelete(g_hSyncSem);
		g_hSyncSem = NULL;
	}
	if(!OspSemBCreate( &g_hSyncSem))
	{
		TcpSocketClean(bSockInit);
		OspPrintf(TRUE, TRUE, "\n TcpSocketInit OspSemBCreate g_hSyncSem Error  \n");
		return FALSE;
	}

	g_hRcvTask = OspTaskCreate( TcpSocketRcvTaskProc, 
								"tRcvTcpDataTask", 
								60, 512*1024, 0, 0 );		
	
    if(g_hRcvTask == FALSE)
	{
		TcpSocketClean(bSockInit);
		return FALSE;
	}
	memset(&g_tSockList, 0, sizeof(g_tSockList));
	memset(&g_tEvent, 0, sizeof(g_tEvent));
	
	g_bInit = TRUE;
	return TRUE;
}

void TcpDestroyNode(HNODE hListenNode)
{
	if(hListenNode != INVALID_NODE)
	{
		SockClose(hListenNode);
		//hListenNode = INVALID_NODE;
	}
}

BOOL32 TcpSocketClean(BOOL32 bSockClean)
{
	if(!g_bInit)
	{
		return TRUE;
	}
	
	if(bSockClean)
	{
		SockCleanup();
	}

	if(g_hRcvTask != FALSE)
	{						
		if(g_hExitTaskSem != NULL)
		{
			TcpSendCtlMsg((s32)SOCK_EXIT); //向线程发消息退出
			if( FALSE == OspSemTakeByTime( g_hExitTaskSem, 5000 ) )//等待线程退出
			{				
				OspTaskTerminate(g_hRcvTask);
				g_hRcvTask = FALSE;			 
			}
		}
		else
		{
			OspTaskTerminate(g_hRcvTask);
			g_hRcvTask = FALSE;	
		}
	}
	
	if(g_hExitTaskSem != NULL)
	{
		OspSemDelete(g_hExitTaskSem);
		g_hExitTaskSem = NULL;
	}

	if(g_hSyncSem != NULL)
	{
		OspSemDelete(g_hSyncSem);
		g_hSyncSem = NULL;
	}

	if(g_hWatchSocket != INVALID_SOCKET)
	{
		SockClose(g_hWatchSocket);
		g_hWatchSocket = INVALID_SOCKET;
	}
	
	memset(&g_tSockList, 0, sizeof(g_tSockList));
	memset(&g_tEvent, 0, sizeof(g_tEvent));
	
	g_bInit = FALSE;
	return TRUE;
}

BOOL32 TcpSetCallBack(TTCPEvent &tTCPEvent)
{
	return TcpSendCtlMsg((s32)SOCK_EVENT, (u8 *)&tTCPEvent, sizeof(TTCPEvent));
}

HNODE  TcpCreateNode( u32 dwIp, u16 wPort )
{
	if(IsSocketFull()) return 0; //不一定安全

    SOCKHANDLE hSock = 0;
	hSock = socket(AF_INET, (s32)SOCK_STREAM, IPPROTO_TCP);
	if(hSock == 0|| hSock == INVALID_SOCKET)
	{
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]TcpCreateNode sock Error\n");
		return 0;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family      = AF_INET; 
	addr.sin_addr.s_addr = dwIp;
	addr.sin_port        = htons(wPort);

#if defined(_LINUX_)
	// [1/5/2011 xliang] 解决mcu异常后自动起来，级联库的port(3337)还能绑定上的问题
	/*
	* When using this example for testing, it's common to kill and restart
	* regularly.  On some systems, this causes bind to fail with "address
	* in use" errors unless this option is set.
	*/
	BOOL32 bReuse = 1;
	//const s8 reuse = 1;
	if ( SOCKET_ERROR == setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, &bReuse, sizeof (bReuse)) )
	{
#ifdef _LINUX_
		printf("[TCPSOCKET]setsockopt reuseAddr Error %s\n", strerror(errno)); 

#elif WIN32

		s32 nCode = 0;
        nCode = WSAGetLastError();
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]setsockopt reuseAddr Error(code:%d)\n", nCode);
#endif
	}
	
	//bool bReusePort = 1;
	//setsockopt(hSock, SOL_SOCKET, SO_REUSEPORT, &bReusePort, sizeof (bReusePort)); // SO_REUSEPORT not support
	
	//struct linger Linger;
	//Linger.l_onoff = 1; /*FALSE;*/
	//Linger.l_linger = 0; /* Ido -270999-1*/
	
	//int yes = TRUE;
	
	//setsockopt(hSock, SOL_SOCKET, SO_LINGER, (char *)&Linger, sizeof(Linger)); //强制关闭，不经历TIME_WAIT的过程//有风险
	//setsockopt(hSock, IPPROTO_TCP, TCP_NODELAY, (char *)&yes, sizeof(yes));	//禁用Nagle算法
	//setsockopt(hSock, SOL_SOCKET, SO_KEEPALIVE, (char *)&yes, sizeof(yes));
#endif

	if(SOCKET_ERROR == bind(hSock, (sockaddr *)(void*)&addr, sizeof(SOCKADDR_IN)))
	{
		s32 nCode = 0;
#ifdef WIN32 
        nCode = WSAGetLastError();
#endif		
		SockClose(hSock);
		//hSock = INVALID_NODE;
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]TcpCreateNode bind Error(code:%d, ip:%x,port:%d)\n", nCode, dwIp, wPort);
	    return 0; 
	}

	if(SOCKET_ERROR == listen(hSock, 5))
	{
		SockClose(hSock);
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]TcpCreateNode listen Error\n");
		return 0; 
	}

	u32 adwBuf[3];
	adwBuf[0] = (u32)hSock;
	adwBuf[1] = 0;//hAppNode;
    adwBuf[2] = TRUE;
	TcpSendCtlMsg((s32)SOCK_ADD, (u8 *)adwBuf, sizeof(adwBuf));

	//同步等待完成
	if(OspSemTakeByTime(g_hSyncSem, 5000 ) == FALSE)
	{
        SockClose(hSock);
		return 0;		
	}
	return (HNODE)hSock;
}

HNODE  TcpConnect( u32 dwIp, u16 wPort, HAPPNODE hAppNode )
{
	if(IsSocketFull()) return 0; //不一定安全
	
    SOCKHANDLE hSock = 0;
	hSock = socket(AF_INET, (s32)SOCK_STREAM, 0);
	if(hSock == 0|| hSock == INVALID_SOCKET)
	{
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]TcpConnect sock Error\n");
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"\n [TCPSOCKET]TcpConnect sock Error\n");

		return 0;
	}
	
	struct linger Linger;
	Linger.l_onoff = 1;
	Linger.l_linger = 0; 
	if ( SOCKET_ERROR == setsockopt(hSock,SOL_SOCKET,SO_LINGER,(const char*)&Linger,sizeof(Linger))) //强制关闭，不经历TIME_WAIT的过程//有风险
	{
#ifdef _LINUX_
		OspPrintf(TRUE, FALSE, "[TCPSOCKET]****setsockopt SO_DONTLINGER Error! error:%d, %s\n",errno, strerror(errno) );
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[TCPSOCKET]****setsockopt SO_DONTLINGER Error! error:%d, %s\n",errno, strerror(errno) );
#else
		OspPrintf(TRUE, FALSE, "[TCPSOCKET]***setsockopt SO_DONTLINGER Error!error:%d\n", GetLastError() );
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[TCPSOCKET]***setsockopt SO_DONTLINGER Error!error:%d\n", GetLastError() );
#endif
	}
	 
	//尝试bind端口LOCAL_DEFAULT_PORT 10次，均失败，结束尝试 [3/22/2012 chendaiwei]
	SOCKADDR_IN addr;
	u16 wLocalBindPort = LOCAL_DEFAULT_PORT;

	if( 0 == g_wLastBindLocalPort )
	{
		g_wLastBindLocalPort = LOCAL_DEFAULT_PORT;
	}
	else
	{
		++g_wLastBindLocalPort;
	}
	
	if( g_wLastBindLocalPort < LOCAL_DEFAULT_PORT )
	{
		g_wLastBindLocalPort = LOCAL_DEFAULT_PORT;
	}

	for(u8 byIdx = 0; byIdx < MAX_PORT_SPAN; byIdx ++)
	{
		wLocalBindPort = g_wLastBindLocalPort + byIdx;

		if( wLocalBindPort >  ( LOCAL_DEFAULT_PORT + MAX_PORT_SPAN ) )
		{
			wLocalBindPort = LOCAL_DEFAULT_PORT + (wLocalBindPort - LOCAL_DEFAULT_PORT - MAX_PORT_SPAN - 1 );
		}

		memset(&addr, 0, sizeof(SOCKADDR_IN));
		addr.sin_family      = AF_INET; 
		addr.sin_addr.s_addr = htonl (INADDR_ANY);    
		addr.sin_port        = htons(wLocalBindPort);

		if( bind(hSock, (SOCKADDR *)(void*)&addr, sizeof(addr)) == SOCKET_ERROR )
		{

#ifdef _LINUX_
			OspPrintf(TRUE, FALSE, "[TCPSOCKET]****bind port[%d] error! error:%d, %s\n",wLocalBindPort, errno, strerror(errno) );
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[TCPSOCKET]****bind port[%d] error! error:%d, %s\n",wLocalBindPort, errno, strerror(errno) );
#else
			OspPrintf(TRUE, FALSE, "[TCPSOCKET]***bind port[%d]error! error:%d\n", wLocalBindPort,GetLastError() );
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[TCPSOCKET]***bind port[%d]error! error:%d\n", wLocalBindPort,GetLastError() );
#endif
		}
		else
		{
			break;
		}
		
		//尝试10次失败
		if( byIdx == MAX_PORT_SPAN-1 )
		{
			SockClose(hSock);
			return 0;
		}
	}

	g_wLastBindLocalPort = wLocalBindPort;

	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family      = AF_INET; 
	addr.sin_addr.s_addr = dwIp;
	addr.sin_port        = htons(wPort);
	if (SOCKET_ERROR == connect(hSock,(sockaddr *)(void*)&addr, sizeof(SOCKADDR_IN)))
	{
		SockClose(hSock);
		OspPrintf(TRUE, FALSE,"\n [TCPSOCKET]TcpConnect connect Error(%x:%d)\n", dwIp, wPort);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"\n [TCPSOCKET]TcpConnect connect Error(%x:%d)\n", dwIp, wPort);
#ifdef _LINUX_
		OspPrintf(TRUE, FALSE, "[TCPSOCKET]****TcpConnect error! error:%d, %s\n", errno, strerror(errno) );
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[TCPSOCKET]****TcpConnect error! error:%d, %s\n", errno, strerror(errno) );
#else
		OspPrintf(TRUE, FALSE, "[TCPSOCKET]***TcpConnect error! error:%d\n", GetLastError() );
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[TCPSOCKET]***TcpConnect error! error:%d\n", GetLastError() );
#endif
		return 0;
	}

#ifdef _LINUX_
    socklen_t nAddrLen;
#else
    s32 nAddrLen;
#endif
    
    memset(&addr, 0, sizeof(addr));
    nAddrLen = sizeof(addr);
    getsockname(hSock, (sockaddr *)(void*)&addr, &nAddrLen);
    u32 dwLocalIP = addr.sin_addr.s_addr;
    u16 wLocalPort = ntohs(addr.sin_port);
    
    u8* pchLocalIP = (u8*)&dwLocalIP;
    u8* pchPeerIP = (u8*)&dwIp;
//     mmculog(1, "[MMCU] Tcp connect, Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u\n",
//                 pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
//                 wLocalPort,
//                 pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
//                 wPort);
    LogPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MMCULIB,"[MMCU] Tcp connect, Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u\n",
		pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
		wLocalPort,
		pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
                wPort);
	
    if (NULL != hAppNode)
    {
        CMCUCascade *pcSession = (CMCUCascade *)hAppNode;
        pcSession->m_dwPeerIP = dwIp;
        pcSession->m_wPeerPort = wPort;
        pcSession->m_dwLocalIP = dwLocalIP;
        pcSession->m_wLocalPort = wLocalPort;
    }

	u32 adwBuf[3];
	adwBuf[0] = (u32)hSock;
	adwBuf[1] = (u32)hAppNode;
	adwBuf[2] = FALSE;
	TcpSendCtlMsg((s32)SOCK_ADD, (u8 *)adwBuf, sizeof(adwBuf));

	TcpSetOpt(hSock);	
		
	//同步等待完成
	if(OspSemTakeByTime(g_hSyncSem, 5000 ) == FALSE)
	{
        SockClose(hSock);
		return 0;		
	}

	return (HNODE)hSock;
}

s32 TcpSend( HNODE hNode, u8 *pbyBuf, s32 nLen)
{
	if(nLen > MAX_TCPMSG_LEN|| nLen<=0 ) return 0;
	return send((SOCKHANDLE)hNode, (char *)pbyBuf, nLen, 0);	
}

BOOL32 TcpDisconnect( HNODE hNode)
{
	u32 adwBuf[1];
	adwBuf[0] = (u32)hNode;
	BOOL32 bRet = TcpSendCtlMsg((s32)SOCK_DEL, (u8 *)adwBuf, sizeof(adwBuf));
	if(!bRet)
	{
		return FALSE;
	}

	//同步等待完成
	if(OspSemTakeByTime(g_hSyncSem, 5000 ) == FALSE)
	{
		return FALSE;		
	}

	return TRUE;
}


static void OnWatchSocket(u8 *pBuf, s32 nLen)
{
	u32 dwMsgId = *((u32 *)pBuf);
	switch(dwMsgId)
	{
	case SOCK_EVENT:
		{
			TTCPEvent *ptEvent = (TTCPEvent *)(pBuf+sizeof(u32));
			memcpy(&g_tEvent, ptEvent, sizeof(g_tEvent));
			break;
		}
	case SOCK_ADD:
		{
			HNODE *phNode = (HNODE *)(pBuf+sizeof(u32));
			HNODE *phAppNode = phNode+1;
			BOOL32 *pbListen   = (BOOL32 *)(phAppNode+1);
			s32 nMaxSocketNum = sizeof(g_tSockList.m_ahNode)/sizeof(g_tSockList.m_ahNode[0]);
		    s32 nIndex = 0;
			for(; nIndex<nMaxSocketNum; nIndex++)
			{
				if(g_tSockList.m_ahNode[nIndex]==0)
				{				
					break;
				}
			}
			if(nIndex < nMaxSocketNum)
			{
				g_tSockList.m_ahNode[nIndex] = *phNode;
				g_tSockList.m_ahAppNode[nIndex] = *phAppNode;
				g_tSockList.m_abListen[nIndex] = *pbListen;
                
                 CMCUCascade *pcSession = (CMCUCascade *)*phAppNode;
                 if (NULL != pcSession)
                 {
                    g_tSockList.m_adwLocalIP[nIndex] = pcSession->m_dwLocalIP;
                    g_tSockList.m_awLocalPort[nIndex] = pcSession->m_wLocalPort;
                    g_tSockList.m_adwPeerIP[nIndex] = pcSession->m_dwPeerIP;
                    g_tSockList.m_awPeerPort[nIndex] = pcSession->m_wPeerPort;
                 }
			}
			else
			{
				OspPrintf(TRUE, FALSE, "\n[TCPSOCKET]SOCK_ADD FULL! nLen.%d\n",nLen);
			}
			OspSemGive(g_hSyncSem);
			break;
		}
	case SOCK_DEL:
		{
			HNODE *phNode = (HNODE *)(pBuf+sizeof(u32));
		    DelNodeFromList(*phNode);
			OspSemGive(g_hSyncSem);
			break;
		}
	default:break;
	}
}


static void OnDispatchMsg(fd_set *pfdset, u8 *pBuf)
{	
	s32 nMaxSocketNum = sizeof(g_tSockList.m_ahNode)/sizeof(g_tSockList.m_ahNode[0]);
    
	s32 nIndex = 0;
	for(; nIndex<nMaxSocketNum; nIndex++)
	{
		if(g_tSockList.m_ahNode[nIndex]==0)
		{
			continue;
		}
		if(!(FD_ISSET(g_tSockList.m_ahNode[nIndex], pfdset)))
		{
		   continue;
		}
        
		HNODE hNode = g_tSockList.m_ahNode[nIndex];
		if(g_tSockList.m_abListen[nIndex])
		{//listen soket
			struct sockaddr_in fsin;  
#ifdef _LINUX_

            socklen_t nLen = sizeof(fsin);
#else
            s32 nLen = sizeof(fsin);
#endif

            HNODE hChildNode = accept(hNode, (struct sockaddr *)(void*)&fsin, &nLen); 
			if(hChildNode == (u32)INVALID_SOCKET ||hChildNode == 0)
			{
#ifdef _LINUX_
				OspPrintf(TRUE, FALSE, "[TCPSOCKET]accept error! error:%d, %s\n", errno, strerror(errno) );
#else
				OspPrintf(TRUE, FALSE, "[TCPSOCKET]accept error! error:%d\n", GetLastError() );
#endif
				
				continue;
			}
            
            u32 dwPeerIP = fsin.sin_addr.s_addr;
            u16 wPeerPort = ntohs(fsin.sin_port);

#ifdef _LINUX_
            socklen_t nAddrLen;
#else
            s32 nAddrLen;
#endif
            
            memset(&fsin, 0, sizeof(fsin));
            nAddrLen = sizeof(fsin);
            getsockname(hChildNode, (sockaddr *)(void*)&fsin, &nAddrLen);
            u32 dwLocalIP = fsin.sin_addr.s_addr;
            u16 wLocalPort = ntohs(fsin.sin_port);
            
            u8* pchLocalIP = (u8*)&dwLocalIP;
            u8* pchPeerIP = (u8*)&dwPeerIP;
//             mmculog(1, "[MMCU] Tcp accept, Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u\n",
//                     pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
//                     wLocalPort,
//                     pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
//                     wPeerPort);
            LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"[MMCU] Tcp accept, Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u\n",
				pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
				wLocalPort,
				pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
                    wPeerPort);

            
            TcpSetOpt(hChildNode);
			
            
			//add to list
			for(s32 i=0; i<nMaxSocketNum; i++)
			{
				if(g_tSockList.m_ahNode[i]==0)
				{
					g_tSockList.m_ahNode[i] = hChildNode;
					g_tSockList.m_abListen[i] = FALSE;
					HAPPNODE hAppNode = 0;
					if(g_tEvent.pNewConnEvent != NULL)
					{
						g_tEvent.pNewConnEvent(g_tSockList.m_ahNode[nIndex], hChildNode, &hAppNode);						
					}
					g_tSockList.m_ahAppNode[i] = hAppNode;
                    
                    g_tSockList.m_adwPeerIP[i] = dwPeerIP;
                    g_tSockList.m_awPeerPort[i] = wPeerPort;

                    g_tSockList.m_adwLocalIP[i] = dwLocalIP;
                    g_tSockList.m_awLocalPort[i] = wLocalPort;

					break;
				}
			}
		}
		else
		{ //client socket
			HAPPNODE hAppNode = g_tSockList.m_ahAppNode[nIndex];
			s32 nReal = recv((SOCKHANDLE)hNode, (char *)pBuf, MAX_TCPMSG_LEN, 0); 
			if(nReal <= 0)
			{
#ifdef _LINUX_
				if(nReal == -1)
				{
					LogPrint(LOG_LVL_WARNING,MID_MCULIB_MMCULIB,"[MMCU]Local<0x%x,%d> peer<0x%x,%d>recv error! error:%d, %s\n",
						g_tSockList.m_adwLocalIP[nIndex],g_tSockList.m_awLocalPort[nIndex],g_tSockList.m_adwPeerIP[nIndex],g_tSockList.m_awPeerPort[nIndex],errno, strerror(errno) );
				}
#endif							
				//socket shutdown
				DelNodeFromList(hNode);
				return;
			}
			if(g_tEvent.pDataCallBack != NULL)
			{
				g_tEvent.pDataCallBack(hNode, hAppNode, pBuf, nReal);
			}
		}
	}
}

static void* TcpSocketRcvTaskProc(void * pParam)
{
	if(g_hWatchSocket == INVALID_SOCKET)
	{
		return 0;
	}

#ifdef _LINUX_
    OspRegTaskInfo(getpid(), "MMcuTcpRecvTask");
#endif
    
	u8  *pBuf = new u8[MAX_TCPMSG_LEN];  

	//g_hExitTaskSem 开始有信号，进入线程后置为无信号
	OspSemTake(g_hExitTaskSem); // be used to wait for  Exiting thread;
	BOOL32 bExit = FALSE;
	s32 nMaxSocketNum = sizeof(g_tSockList.m_ahNode)/sizeof(g_tSockList.m_ahNode[0]);
	fd_set  rcvSocketfd;    
	FD_ZERO(&rcvSocketfd);
	FD_SET( g_hWatchSocket, &rcvSocketfd);

  	while(select(FD_SETSIZE, &rcvSocketfd, NULL, NULL, NULL ) != SOCKET_ERROR)
	{
		if( FD_ISSET( g_hWatchSocket, &rcvSocketfd ) )
		{
			memset(pBuf, 0, MAX_WATCH_LEN);
			//解析控制包
			s32 nLen = recvfrom(g_hWatchSocket, (s8 *)pBuf, MAX_WATCH_LEN, 0, NULL, NULL);
			u32 *pMode = (u32 *)pBuf;
			//控制命令
			//解析控制命令
			switch((s32)(*pMode))
			{	
			case SOCK_EXIT:
				{
					bExit = TRUE;
					break;
				}
			default:
				{
					OnWatchSocket(pBuf, nLen);
					break;
				}
			}
		}
		if(bExit)
		{
			for(s32 i=0; i<nMaxSocketNum; i++)
			{
				if(g_tSockList.m_ahNode[i] != 0)
				{
					SockClose(g_tSockList.m_ahNode[i]);
					g_tSockList.m_ahNode[i] = 0;
				}
			}			
			break;
		}

        OnDispatchMsg(&rcvSocketfd, pBuf);

		// set socket to select
		FD_ZERO(&rcvSocketfd);
		FD_SET( g_hWatchSocket, &rcvSocketfd);
		for(s32 k=0; k<nMaxSocketNum; k++)
		{
			if(g_tSockList.m_ahNode[k] != 0)
			{
				FD_SET((g_tSockList.m_ahNode[k]), &rcvSocketfd);
			}
		}
	}
    OspSemGive(g_hExitTaskSem); // be used to wait for  Exiting thread;
	delete []pBuf;

	printf("pParam.%p\n",pParam);

	return 0;
}
/*lint -restore*/

