/***********************************************************************
Filename   : rvsymservint.h
Description: host related functions support for Symbian
************************************************************************
        Copyright (c) 2001 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/

#ifndef __RVSYMSERVERINT_H__
#define __RVSYMSERVERINT_H__

#if ((RV_NET_TYPE != RV_NET_NONE) && (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN))
#include <e32base.h>
#include "rvaddress.h"
#include "rvsocket.h"

class RvSymStatic {
public:
	RvStatus Construct(RvUint32 maxReadBuff,RvUint32 maxWriteBuff);
	void Destruct();
	inline RvUint32 GetMaxReadBufferLen() { return maxReadBufLen; };
	inline RvUint32 GetMaxWriteBufferLen() { return maxWriteBufLen; };
	inline void SetMaxReadBufferLen(RvUint32 bufLen) { maxReadBufLen = bufLen; }
	inline void SetMaxWriteBufferLen(RvUint32 bufLen) { maxWriteBufLen = bufLen; }
	inline RSocketServ *GetSocketServ() { return &iSocketServ; }
    void AddSock(RvSymSock* sock);
    RvSymSock *GetSock();
private:
	RSocketServ			iSocketServ;
	RvUint32			maxReadBufLen;
	RvUint32			maxWriteBufLen;
	CActiveScheduler	*scheduler;
    RvSymSock           *sockDeleteList;
    RvLock              sockDeleteListLock;
};


_LIT(KRvServerName,"RvServer");

typedef struct {
	RvSelectEngine	*selectEngine;
	RvSelectEvents 	events;
	RvSelectFd		*fd;
} rvSymSelectRqstParams;

typedef struct {
	RvSelectEngine*     selectEngine;
	RvUint64			nsecTimeout;
} rvSymSelectSetTimeOutRqstParams;

typedef struct {
 	RvSocket*        sock;
	IN RvAddress*    address;
	IN RvPortRange*  portRange;
	IN RvLogMgr*     logMgr;
} rvSymSocketBindRqstParams;

typedef struct {
	RvSocket*    sock;
	RvLogMgr*    logMgr;
	RvSocket*    newSocket;
	RvAddress*   remoteAddress;
} rvSymSocketAcceptRqstParams;

typedef struct {
    RvSocket*    sock;
    RvLogMgr*    logMgr;
} rvSymSocketCleanRqstParams;

typedef struct {
	RvSocket*    sock;
    RvAddress*   address;
    RvLogMgr*    logMgr;
} rvSymSocketConnectRqstParams;

typedef struct {
    RvInt             addressType;
    RvSocketProtocol  protocolType;
    RvLogMgr*         logMgr;
	RvSocket**        sock;
} rvSymSocketConstructRqstParams;

typedef struct {
    RvSocket*    sock;
    RvBool       cleanSocket;
    RvPortRange* portRange;
    RvLogMgr*    logMgr;
} rvSymSocketDestructRqstParams;

typedef struct {
    RvSocket*    sock;
    RvSize_t* 	bytesAvailable;
} rvSymSocketBytesRqstParams;

typedef struct {
    RvSocket 	*sock;
    RvAddress	*address;
} rvSymSocketLocalAddrRqstParams;

typedef struct {
    RvSocket 	*sock;
    RvAddress	*address;
} rvSymSocketRemoteAddrRqstParams;

typedef struct {
	RvSocket    *sock;
    RvUint32    queuelen;
    RvLogMgr	*logMgr;
} rvSymSocketListenRqstParams;

typedef struct {
    RvSocket*   sock;
    RvUint8*    buffer;
    RvSize_t    bytesToReceive;
    RvLogMgr*   logMgr;
    RvSize_t*   bytesReceived;
    RvAddress*  remoteAddress;
} rvSymSocketReceiveRqstParams;

typedef struct {
    RvSocket*   sock;
    RvUint8*    buffer;
    RvSize_t    bytesToSend;
    RvAddress*  remoteAddress;
    RvLogMgr*   logMgr;
    RvSize_t*   bytesSent;
} rvSymSocketSendRqstParams;

typedef struct {
	RvSocket*    sock;
	RvInt32      sendSize;
	RvInt32      recvSize;
	RvLogMgr*    logMgr;
} rvSymSocketSetBufRqstParams;


typedef struct {
	RvSocket*    sock;
	RvBool       cleanSocket;
} rvSymSocketShutdownRqstParams;

typedef struct {
	RvSelectEngine	*selectEngine;
	RvUint8			message;
} rvSymSelectPreemptRqstParams;

typedef struct 
{
	RvUint32	    memsize;
	void          **pMem;
} rvSymAlloc;

typedef struct 
{
	void          *memptr;
} rvSymMemFree;

typedef struct 
{
	RvUint32	    count;
	void          **sem;
} rvSymSemaphore;

typedef struct 
{
	void          **lock;
} rvSymLock;


typedef struct 
{
	void          *pMem;
} rvSymFreeSockPool;


enum RvSymServerCmd {
	RvSymSelectAddRqst = 1,
	RvSymSelectUpdateRqst,
	// Remove request is implemented by the 'update'
	// request with events parameter = 0
	RvSymSelectSetTimeOutRqst,
	RvSymSocketBindRqst,
	RvSymSocketAcceptRqst,
	RvSymSocketCleanRqst,
	RvSymSocketConnectRqst,
	RvSymSocketConstructRqst,
	RvSymSocketDestructRqst,
	RvSymSocketBytesRqst,
	RvSymSocketLocalAddrRqst,
	RvSymSocketRemoteAddrRqst,
	RvSymSocketListenRqst,
	RvSymSocketReceiveRqst,
	RvSymSocketSendRqst,
	RvSymSocketSetBufRqst,
	RvSymSocketShutdownRqst,
	RvSymPreemptionRqst,
	RvSymAllocRqst,
	RvSymMemFreeRqst,
	RvSymSemaphoreRqst,
	RvSymLockRqst,
	RvSymLockColseRqst,
	RvSymSemaphoreColseRqst,
	RvSymFreeSockPool
};


/********************************************************************************************
 *
 *  RvSymSelectClientSideSession class implements client side of the Symbian session for
 *  the Symbian server running in separate thread where select loop is applied
 *
 ********************************************************************************************/
class RvSymClientSideSession : public RSessionBase {
public:
	RvSymClientSideSession();
	TInt Connect();
	void Close();
	RvStatus SendSelectAddRequest(
		IN RvSelectEngine	*selectEngine,
		IN RvSelectEvents 	events,
		IN RvSelectFd		*fd);
	RvStatus SendSelectUpdateRequest(
		IN RvSelectEngine	*selectEngine,
		IN RvSelectEvents 	events,
		IN RvSelectFd		*fd);
	RvStatus SendSelectTimeOutRequest(
	   	IN RvSelectEngine*      selectEngine,
		IN RvUint64				nsecTimeout);
	RvStatus SendSocketBindRequest(
		IN RvSocket*         sock,
		IN RvAddress*        address,
		IN RvPortRange*      portRange,
		IN RvLogMgr*         logMgr);
	RvStatus SendSocketAcceptRequest(
		IN  RvSocket*    sock,
		IN  RvLogMgr*    logMgr,
		OUT RvSocket*    newSocket,
		OUT RvAddress*   remoteAddress);
	RvStatus SendSocketCleanRequest(
	    IN RvSocket*    sock,
	    IN RvLogMgr*    logMgr);
	RvStatus SendSocketConnectRequest(
	    IN RvSocket*    sock,
	    IN RvAddress*   address,
	    IN RvLogMgr*    logMgr);
	RvStatus SendSocketConstructRequest(
	    IN  RvInt             addressType,
	    IN  RvSocketProtocol  protocolType,
	    IN  RvLogMgr*         logMgr,
		OUT RvSocket**        sock);
	RvStatus SendSocketDestructRequest(
	    IN RvSocket*    sock,
	    IN RvBool       cleanSocket,
	    IN RvPortRange* portRange,
	    IN RvLogMgr*    logMgr);
	RvStatus SendSocketBytesRequest(
	    IN RvSocket* sock,
	    OUT RvSize_t* bytesAvailable);
	RvStatus SendSocketLocalAddrRequest(
	    IN RvSocket *sock,
	    OUT RvAddress* address);
	RvStatus SendSocketRemoteAddrRequest(
	    IN RvSocket *sock,
	    OUT RvAddress* address);
	RvStatus SendSocketListenRequest(
		IN  RvSocket    *sock,
	    IN  RvUint32    queuelen,
	    IN  RvLogMgr	*logMgr);
	RvStatus SendSocketReceiveRequest(
	    IN  RvSocket*   sock,
	    IN  RvUint8*    buffer,
	    IN  RvSize_t    bytesToReceive,
	    IN  RvLogMgr*   logMgr,
	    OUT RvSize_t*   bytesReceived,
	    OUT RvAddress*  remoteAddress);
	RvStatus SendSocketSendRequest(
	    IN  RvSocket*   sock,
	    IN  RvUint8*    buffer,
	    IN  RvSize_t    bytesToSend,
	    IN  RvAddress*  remoteAddress,
	    IN  RvLogMgr*   logMgr,
	    OUT RvSize_t*   bytesSent);
	RvStatus SendSocketSetBufRequest(
		IN RvSocket*    sock,
		IN RvInt32      sendSize,
		IN RvInt32      recvSize,
		IN RvLogMgr*    logMgr);
	RvStatus SendSocketShutdownRequest(
		IN RvSocket*    sock,
		IN RvBool       cleanSocket);
	RvStatus SendPreemptionRequest(
		IN RvSelectEngine	*selectEngine,
		IN RvUint8			message);
    RvStatus ServerAlloc(int memsize,void **pMem);
    RvStatus ServerFree(void *memptr);
    RvStatus ServerSemaphoreCreate(int count,void **sem);
    RvStatus ServerLockCreate(void **lock);
    RvStatus ServerLockClose(void **lock);
    RvStatus ServerSemahporeClose(void **sem);
    RvStatus SendSymFreeSockPool(void);            
private:
	RvStatus SendRequest(
		IN RvSymServerCmd	rqst,
		IN TAny				*arg);
	static RvThread *serverThread;
};

/********************************************************************************************
 *
 *  RvSymServerSideSession class implements server side of the Symbian session for
 *  the Symbian server running in separate thread where select loop is applied
 *
 ********************************************************************************************/
class RvSymServerSideSession : public CSharableSession {
public:
	RvSymServerSideSession();
	virtual void ServiceL(
		IN const RMessage &aMessage);
};

/********************************************************************************************
 *
 *  RvSymSelectServer class implements Symbian server for thread that executes
 *	select loop.
 *
 ********************************************************************************************/
class RvSymServer: public CServer {
public:
	RvSymServer();
	CSharableSession *NewSessionL(const TVersion &aVersion) const;
};


RvStatus RvSymServerGetClient(
	OUT RvSymClientSideSession 	**client);

RvSymStatic *RvSymServerGetStaticParams();

#endif /* (RV_NET_TYPE != RV_NET_NONE) */
#endif /* __RVSYMSERVERINT_H__ */
