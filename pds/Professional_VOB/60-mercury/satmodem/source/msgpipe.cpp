
#include "msgpipe.h"
#include "osp.h"
#include "modemcmdproxy.h"


#ifdef _VXWORKS_
#include "taskLib.h"
#include "sysLib.h"
#endif


#ifdef _LINUX_
SEMHANDLE		m_hRWPipe;
#define			WAIT_SEM_TIMEOUT  (u8)2000
#endif

extern MdcomEnv g_env;

int CreateMsgPipe(MsgPipe *pMsgPipe)
{
    BOOL32 bRet = FALSE;

#ifdef WIN32
	bRet = CreatePipe(&pMsgPipe->read, &pMsgPipe->write, NULL, MDCOM_PIPEBUF_SIZE * PROTLPACK_MAXSIZE );
	if(!bRet) return -1;

    InitializeCriticalSection(&pMsgPipe->readCS);
#endif

#ifdef _VXWORKS_
	pMsgPipe->pipe = msgQCreate(MDCOM_PIPEBUF_SIZE, sizeof(MdcomCmd), MSG_Q_FIFO);
	if (pMsgPipe->pipe == NULL)
		return -1;
#endif 

#ifdef _LINUX_

	pMsgPipe->m_nHead = 0;
	pMsgPipe->m_nTail = 0;
	memset(pMsgPipe->cmdQueue, 0, sizeof(MdcomCmd) * MAXNUM_CMD);

	// 创建信号量
	if( !OspSemBCreate(&m_hRWPipe) )
	{
		OspSemDelete( m_hRWPipe );
		m_hRWPipe = NULL;
        printf("[CreateMsgPipe] create handle<0x%x> failed!\n", m_hRWPipe );
		bRet = FALSE;
	}
    return bRet;
#endif

    return 0;
}

void CloseMsgPipe(MsgPipe *pipe)
{
#ifdef WIN32
    CloseHandle(pipe->read);
    CloseHandle(pipe->write);
    DeleteCriticalSection(&pipe->readCS);
#endif

#ifdef _VXWORKS_
	msgQDelete(pipe->pipe);
#endif

#ifdef _LINUX_

	pipe->m_nHead = 0;
	pipe->m_nTail = 0;
	memset(pipe->cmdQueue, 0, sizeof(MdcomCmd) * MAXNUM_CMD);

	// 删除信号量
	if( NULL != m_hRWPipe )
	{
		OspSemDelete( m_hRWPipe );
		m_hRWPipe = NULL;
	}
#endif
}

int WriteMsgPipe(MsgPipe *pipe, const MdcomCmd *msg)
{
    int msglen;    
    msglen = sizeof(MdcomCmd);

#ifdef WIN32
    BOOL ret;
    unsigned long writelen;
    
	/* 保证串行地写入命令 */
    EnterCriticalSection(&pipe->readCS);
    ret = WriteFile(pipe->write, msg, msglen, &writelen, NULL);
    LeaveCriticalSection(&pipe->readCS);
    if (!ret || (msglen != (int)writelen))
    {
        debug("Write file failed.\n");
        return -1;
    }
#endif

#ifdef _VXWORKS_
    STATUS ret;
    ret = msgQSend(pipe->pipe, (char *)msg, msglen, WAIT_FOREVER, 0);
    if (ret == ERROR) return -1;    
#endif

#ifdef _LINUX_

	
	OspSemTakeByTime( m_hRWPipe, WAIT_SEM_TIMEOUT );

	//队列满，丢弃当前消息
	if (pipe->cmdQueue[pipe->m_nTail].port != 0 &&
		pipe->cmdQueue[pipe->m_nTail].ip != 0)
	{
		MdcomOutput(LVL_ERR, "[WriteMsgPipe] cmdQueue fulled, ignore the new cmd!\n");
		OspSemGive( m_hRWPipe );
		return -1;
	}

	//写入当前消息
	pipe->cmdQueue[pipe->m_nTail] = *msg;

	MdcomOutput(LVL_VERBOSE, "[WriteMsgPipe] cmd< %s, ip.%s, port.%d, val.%d> has been set in!\n",
							strOfCmd(pipe->cmdQueue[pipe->m_nTail].cmd),
							strOfIp(pipe->cmdQueue[pipe->m_nTail].ip),
							pipe->cmdQueue[pipe->m_nTail].port,
							pipe->cmdQueue[pipe->m_nTail].val);

    pipe->m_nTail = (pipe->m_nTail + 1) % MAXNUM_CMD;

	OspSemGive( m_hRWPipe );
	
#endif

    return 0;
}

int ReadMsgPipe(MsgPipe *pipe, MdcomCmd *msg)
{
    int ret;
    unsigned long msglen, readlen;    
    msglen = sizeof(MdcomCmd);    
    memset(msg, 0, msglen);
#ifdef WIN32
    ret = ReadFile(pipe->read, msg, msglen, &readlen, NULL);
    if (!ret || (msglen != readlen))
    {
        debug("Read file failed or nothing exists.\n");
        return -1;
    }
#endif 

#ifdef _VXWORKS_
    ret = msgQReceive(pipe->pipe, (char *)msg, msglen, WAIT_FOREVER);
    if (ret == ERROR) return -1;
#endif

#ifdef _LINUX_

	OspSemTakeByTime( m_hRWPipe, WAIT_SEM_TIMEOUT );

	//队列空，忽略
	if (pipe->cmdQueue[pipe->m_nHead].ip == 0 &&
		pipe->cmdQueue[pipe->m_nHead].port == 0)
	{
		MdcomOutput(LVL_VERBOSE, "[ReadMsgPipe] cmdQueue null, ignore the new read!\n");
		OspSemGive(m_hRWPipe);
		return -1;
	}


	memcpy(msg, &(pipe->cmdQueue[pipe->m_nHead]), sizeof(MdcomCmd));


	MdcomOutput(LVL_VERBOSE, "[ReadMsgPipe] cmd< %s, ip.%s, port.%d, val.%d> has been get out!\n",
							strOfCmd(pipe->cmdQueue[pipe->m_nHead].cmd),
							strOfIp(pipe->cmdQueue[pipe->m_nHead].ip),
							pipe->cmdQueue[pipe->m_nHead].port,
							pipe->cmdQueue[pipe->m_nHead].val);


	memset(&(pipe->cmdQueue[pipe->m_nHead]), 0, sizeof(MdcomCmd));
    pipe->m_nHead = (pipe->m_nHead + 1) % MAXNUM_CMD;
	
	
	OspSemGive( m_hRWPipe );
	
#endif

    return 0;
}

int InitSocket(SOCKET *sock)
{
    int err;    

#ifdef WIN32
    WSADATA wsaData;
   	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(err != 0)
	{
		debug("WSAStartup Error.\n");
		return -1;
	}
#endif

  	*sock = socket(AF_INET, SOCK_DGRAM , 0);
	if( *sock == INVALID_SOCKET )
	{
		debug("Create UDP Socket Error.\n");
		return -1;
	}

#if 0
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("172.16.10.8");
    addr.sin_port = 10001;
    memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));
    bind(*sock, (struct sockaddr*)&addr, sizeof(addr));
#endif
    
    return 0;
}

void CloseSocket(SOCKET *sock)
{
#ifdef WIN32
    closesocket(*sock);
    WSACleanup();
#else
    close(*sock);
#endif
}

u32 GetTick()
{
    return OspTickGet();
}

void Waitms(u32 millisecs)
{
    OspTaskDelay(millisecs);
    return;
}

