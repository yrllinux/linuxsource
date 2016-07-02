/*****************************************************************************
   模块名      : sitecall
   文件名      : sitecall.h
   相关文件    : sitecall.CPP
   文件实现功能: 实现主叫呼集
   作者        : ZHHE
   版本        : V4.0  Copyright(C) 2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/

#ifndef _SITECALL_H
#define _SITECALL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "transportint.h"

/*================================= 宏定义 ================================*/	
#define NUMOFSESSION	4
#define NUMOFHOST		4
#define MAXBUFFSIZE		2048
#define RPOOLCHUNKSIZE  512
#define MAXHOSTNUM		8	//client
#define MAXCONNECTION	MAXHOSTNUM	//gk
#define TRANSTIMEOUT	30000
#define SITECALLTIMEOUT	30000
	
#ifndef SITECALLPORT
#define SITECALLPORT	700
#endif

DECLARE_OPAQUE(HSITECALL);	
	
enum SiteCallState
{
	SiteCallHostClosed,
	SiteCallTimeOut,
	SiteCallHostConnected
};

/*================================= 结构定义 ================================*/
typedef struct tagSiteCallTransHost
{
	BOOL                used;
	BOOL                incoming;
	BOOL                reported;
	HOSTSTATE           state;
	cmTransportAddress  remoteAddress;
	cmTransportAddress  localAddress;
	HTPKT               hTpkt;
	HTI					hTimer;
	void                *firstMessage;
	void                *lastMessage;
	void                *incomingMessage;
}scTransHost;

typedef struct tagSiteCallNetAddr
{
	UINT32 ip;
	UINT16 port;
}SCNetAddr;

typedef struct tagSiteCallConfig
{
	BOOL bGk;
	SCNetAddr localAddr;
	SCNetAddr GkAddr;
	int		  maxClientNum;
}SiteCallConfig, *PSiteCallConfig;

typedef int (* NotifyHostStateT)(HSITECALL hSiteCall, int state);
typedef int (* NotifyReceiveMessageT)(HSITECALL hSiteCall, unsigned char* pBuf, int nBufLen);
	
typedef  struct
{
	NotifyHostStateT					fpNotifySiteCallState;
	NotifyReceiveMessageT				fpNotifyReceiveMessage;
}TRANSEVENTS, *LPTRANSEVENTS;

typedef struct tagSiteCallTransGlobals{
    HRPOOL              messagesRPool;
    TRANSEVENTS			scEvents;
    HTPKTCTRL           tpktCntrl;
    scTransHost         *hostListen;
	scTransHost			*host[MAXHOSTNUM];
	HSTIMER				hTimer;
	SiteCallConfig		config;
    int                 codingBufferSize;
	int					curClientNum;
	int		maxUsedNumOfMessagesInRpool;
	int		curUsedNumOfMessagesInRpool;
} scTransGlobals;

extern scTransGlobals* g_ptTrans;


/*====================================================================
函	 数   名: SiteCallInit
功	      能: initialize sitecall module
算 法 实  现: 
引用全局变量: 
输入参数说明: 无			  
返   回   值: 成功 - TRUE
			  失败 - FALSE		
====================================================================*/
RVAPI BOOL RVCALLCONV TransSiteCallInit(SiteCallConfig* pConfig);

/*====================================================================
函	 数   名: SetCallBack
功	      能: register callback function
算 法 实  现: 
引用全局变量: 无
输入参数说明: lpEvent - callback pointer
返   回   值: 成功 - sc_ok
			  失败 - sc_err
====================================================================*/
RVAPI int RVCALLCONV TransSetCallBack(LPTRANSEVENTS lpEvent);

/*====================================================================
函	 数   名: CreateNewHost
功	      能: create a new host for sitecall.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: 
返   回   值: host	- The host on which to send the messages that await to be sent.
              NULL  - failed.
====================================================================*/
RVAPI scTransHost* RVCALLCONV CreateNewHost();

/*====================================================================
函	 数   名: Connect2Gk
功	      能: connect to GK server by tpkt.
算 法 实  现: 
引用全局变量: 
输入参数说明: host        - The host on which to connect.
返   回   值: sc_ok       - All is ok.
              sc_err	  - The connection is busy.
====================================================================*/
RVAPI int RVCALLCONV Connect2Gk(void* element);

/*====================================================================
函	 数   名: TransSiteCallHandle
功	      能: The callback routine for a tpkt connection
算 法 实  现: 无
引用全局变量: 无
输入参数说明: standard input of TPKT callback	  
返   回   值: void	
====================================================================*/
void TransSiteCallHandle(HTPKT tpkt, liEvents event, int length, int error, void*context);

/*====================================================================
函	 数   名: SaveMsgToPool
功	      能: This routine gets an encoded message and saves it, until it can send it.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: host         - The host on which the message is to be sent.
			  buffer       - The encoded message.
			  encMsgSize   - Its size.
			  addToTop     - TRUE: Add the message to the start of the queue
							 FALSE: Add it at the end.  
返   回   值: saved message	
====================================================================*/
RVAPI void * RVCALLCONV SiteCallSaveMsgToPool(void *element, unsigned char *buffer, int encMsgSize, BOOL addToTop = FALSE);

/*====================================================================
函	 数   名: ExtractMessageFromPool
功	      能: This routine removes an encoded message from the head of the host list.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: element      - The host or session which wants to send the message.
返   回   值: next message.	
====================================================================*/
void *SiteCallExtractMessageFromPool(void *element);

/*====================================================================
函	 数   名: sendMessageOnHost
功	      能: send a message on the given host according to its communication protocol.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: host        - The host on which to send the messages that await to be sent.
返   回   值: cmTransOK             - All is ok.
              cmTransConnectionBusy - The connection is still sending previous messages.
====================================================================*/
RVAPI int RVCALLCONV SendSiteCallMessage(void* element);

/*====================================================================
函	 数   名: TransSiteCallHostClosed
功	      能: host closed.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: context     - The host or session which send the message.
返   回   值: void
====================================================================*/
void TransSiteCallHostClosed(void* context);

/*====================================================================
函	 数   名: PrintSiteCallState
功	      能: print debug infomation.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: 
返   回   值: void
====================================================================*/
RVAPI void RVCALLCONV PrintSiteCallState();

/*====================================================================
函	 数   名: TransMessageTimeOut
功	      能: host send message timeout or didn't receive response.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: context     - The host or session which send the message.
返   回   值: void
====================================================================*/
void TransMessageTimeOut(void* element);

/*====================================================================
函	 数   名: TransSiteCallTimeOut
功	      能: sitecall timeout.
算 法 实  现: 无
引用全局变量: 无
输入参数说明: context     - The host to close
返   回   值: void
====================================================================*/
void TransSiteCallTimeOut(void* element);

/*====================================================================
函	 数   名: SiteCallClose
功	      能: close sitecall host.
算 法 实  现: 
引用全局变量: 
输入参数说明: 无			  
返   回   值: void	
====================================================================*/
RVAPI void RVCALLCONV SiteCallHostClose(void* element);

/*====================================================================
函	 数   名: SiteCallHostInit
功	      能: 
算 法 实  现: 
引用全局变量: 
输入参数说明: 无			  
返   回   值: void	
====================================================================*/
RVAPI void RVCALLCONV SiteCallHostInit(scTransHost* host);

/*====================================================================
函	 数   名: SiteCallSetGkIp
功	      能: 
算 法 实  现: 
引用全局变量: 
输入参数说明: 无			  
返   回   值: void	
====================================================================*/
RVAPI void RVCALLCONV SiteCallSetGatekeeperIp(UINT32 gkIp, UINT16 gkPort);

/*====================================================================
函	 数   名: SiteCallEnd
功	      能: close sitecalll module
算 法 实  现: 
引用全局变量: 
输入参数说明: 无			  
返   回   值: void	
====================================================================*/
RVAPI void RVCALLCONV TransSiteCallEnd();


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*_SITECALL_H*/

