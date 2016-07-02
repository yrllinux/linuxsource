/***********************************************************************
Filename   : rvsymsock.h
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

#include "rvaddress.h"

#if ((RV_NET_TYPE != RV_NET_NONE) && (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN))

#if defined(__cplusplus)
extern "C" {
#endif

void RvSymSockInit(void);


void RvSymGetHostByName(
	IN		RvChar*     hostName,
    INOUT   RvUint32*   numOfIps,
    OUT     RvAddress*  ipList);

RvStatus RvBuildWritePool(void);
RvStatus RvBuildOpenSockets(void);

RvStatus RvSymSockAccept(
	IN  RvSocket*    sock,
	IN  RvLogMgr*    logMgr,
	OUT RvSocket*    newSocket,
	OUT RvAddress*   remoteAddress);

RvStatus RvSymSockBind(
	IN RvSocket*         sock,
	IN RvAddress*        address,
	IN RvPortRange*      portRange,
	IN RvLogMgr*         logMgr);

void RvSymSockClean(
    IN RvSocket*    sock,
    IN RvLogMgr*    logMgr);


RvStatus RvSymSockConnect(
        IN RvSocket*    sock,
        IN RvAddress*   address,
        IN RvLogMgr*    logMgr);

RvStatus RvSymSockConstruct(
    IN  RvInt             addressType,
    IN  RvSocketProtocol  protocolType,
    IN  RvLogMgr*         logMgr,
	OUT RvSocket*         sock);

RvStatus RvSymSockDestruct(
    IN RvSocket*    sock,
    IN RvBool       cleanSocket,
    IN RvPortRange* portRange,
    IN RvLogMgr*    logMgr);


RvStatus RvSymSockGetBytesAvailable(
    IN RvSocket* sock,
    OUT RvSize_t* bytesAvailable);


RvStatus RvSymSockGetLastError(
    IN RvSocket* sock,
    OUT RvInt32* lastError);

RvStatus RvSymSockGetLocalAddress(
    IN RvSocket *sock,
    OUT RvAddress* address);

RvStatus RvSymSockGetRemoteAddress(
    IN RvSocket *sock,
    OUT RvAddress* address);

RvStatus RvSymSockGetTypeOfService(
	IN RvSocket*    sock,
    IN RvInt32*     typeOfService,
    IN RvLogMgr*    logMgr);

RvStatus RvSymSockJoinMulticastGroup(
    IN RvSocket*    sock,
    IN RvAddress*   multicastAddress,
    IN RvAddress*   interfaceAddress,
    IN RvLogMgr*    logMgr);

RvStatus RvSymSockLeaveMulticastGroup(
    IN RvSocket*    sock,
    IN RvAddress*   multicastAddress,
    IN RvAddress*   interfaceAddress,
    IN RvLogMgr*    logMgr);

RvStatus RvSymSockListen(
	IN  RvSocket    *sock,
    IN  RvUint32    queuelen,
    IN  RvLogMgr*   logMgr);

RvStatus RvSymSockReceiveBuffer(
    IN  RvSocket*   sock,
    IN  RvUint8*    buffer,
    IN  RvSize_t    bytesToReceive,
    IN  RvLogMgr*   logMgr,
    OUT RvSize_t*   bytesReceived,
    OUT RvAddress*  remoteAddress);


RvStatus RvSymSockReuseAddr(
	IN  RvSocket*   sock,
    IN  RvLogMgr*   logMgr);

RvStatus RvSymSockSendBuffer(
    IN  RvSocket*   sock,
    IN  RvUint8*    buffer,
    IN  RvSize_t    bytesToSend,
    IN  RvAddress*  remoteAddress,
    IN  RvLogMgr*   logMgr,
    OUT RvSize_t*   bytesSent);


RvStatus RvSymSockSetBroadcast(
	IN RvSocket*    sock,
	IN RvBool       canBroadcast,
	IN RvLogMgr*    logMgr);

RvStatus RvSymSockSetBuffers(
	IN RvSocket*    sock,
	IN RvInt32      sendSize,
	IN RvInt32      recvSize,
	IN RvLogMgr*    logMgr);

RvStatus RvSymSockSetLinger(
	IN  RvSocket*   sock,
	IN  RvInt32     lingerTime,
	IN  RvLogMgr*   logMgr);


RvStatus RvSymSockSetMulticastInterface(
	IN RvSocket*    sock,
	IN RvAddress*   address,
	IN RvLogMgr*    logMgr);

RvStatus RvSymSockSetMulticastTtl(
	IN RvSocket*    sock,
	IN RvInt32      ttl,
	IN RvLogMgr*    logMgr);

RvStatus RvSymSockSetTypeOfService(
	IN RvSocket*    sock,
	IN RvInt        typeOfService,
	IN RvLogMgr*    logMgr);

RvStatus RvSymSockShutdown(
	IN RvSocket*    sock,
	IN RvBool       cleanSocket);

void RvSymDestructPoolAndSock();
RvBool RvBuildPoolSocket(void);
RvStatus RvSymbianStartCallBackThread(void);
void RvSymbianCallBackThreadEnd(void);


#if defined(__cplusplus)
}
#endif


#endif /* (RV_NET_TYPE != RV_NET_NONE) */
