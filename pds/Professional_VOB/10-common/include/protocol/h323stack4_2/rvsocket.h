/***********************************************************************
Filename   : rvsocket.h
Description: enables OS-independent BSD4.4 sockets operations.
************************************************************************
      Copyright (c) 2001,2002 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is proprietary to RADVISION LTD.
No part of this publication may be reproduced in any form whatsoever
without written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
************************************************************************/

#ifndef RV_SOCKET_H
#define RV_SOCKET_H

#include "rvlog.h"
#include "rvaddress.h"
#include "rvportrange.h"


#if defined(__cplusplus)
extern "C" {
#endif


/* Error checks to make sure configuration has been done properly */
#if !defined(RV_SOCKET_TYPE) || ((RV_SOCKET_TYPE != RV_SOCKET_BSD) && \
    (RV_SOCKET_TYPE != RV_SOCKET_WIN32_WSA) && (RV_SOCKET_TYPE != RV_SOCKET_PSOS) && \
    (RV_SOCKET_TYPE != RV_SOCKET_NUCLEUS) && (RV_SOCKET_TYPE != RV_SOCKET_SYMBIAN))
#error RV_SOCKET_TYPE not set properly
#endif

/* Lets make error codes a little easier to type */
#define RvSocketErrorCode(_e) RvErrorCode(RV_ERROR_LIBCODE_CCORE, RV_CCORE_MODULE_SOCKET, (_e))
    
/* Socket specific error codes */
#define RV_SOCKET_ERROR_EOF RvSocketErrorCode(-513)


#if (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)
#include <inc/tcp.h>
#include <target.h>
#include <externs.h>

#endif


/* Type declaration of sockets */
#if (RV_SOCKET_TYPE == RV_SOCKET_WIN32_WSA)

#pragma warning(push, 3)
#include <winsock2.h>
#pragma warning(pop)

typedef SOCKET RvSocket;

#define RV_SOCKET_QOS_MAX_SOCKS 100

#elif (RV_OS_TYPE == RV_OS_TYPE_WINCE)

#pragma warning(push, 3)
#include <winsock.h>
#pragma warning(pop)

typedef SOCKET RvSocket;

#elif (RV_OS_TYPE == RV_OS_TYPE_MOPI)
#include <enipsock.h>
#include <esockth.h>

typedef SOCKET RvSocket;


/*#define inet_ntoa         sock_inet_ntoa*/
#define inet_ntoa           bsd_inet_ntoa
/*  use this format for MOPI:
char* bsd_inet_ntoa (struct bsd_in_addr in, char* aBuffer, int aLen)*/

#define in_addr             sock_in_addr
#define inet_addr           sock_inet_addr

#if defined(_RV_SIP_USER_CONFIG)
#define Rv2SocketConstruct(a, b, c, d) Rv2SocketMopiConstruct((a), (b), (c), MOPI_EMB_PROC_SIP, (d))

#elif defined(RVRTPCONFIG_H)
#define Rv2SocketConstruct(a, b, c, d) Rv2SocketMopiConstruct((a), (b), (c), MOPI_EMB_PROC_RTP, (d))

#else
#define Rv2SocketConstruct(a, b, c, d) Rv2SocketMopiConstruct((a), (b), (c), 0, (d))

#endif

/*#elif (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN)

typedef void * RvSocket;
*/
#else
/* Other operating systems */
typedef int RvSocket;

#endif


typedef enum
{
    RvSocketProtocolUdp = 0,
    RvSocketProtocolTcp,
    RvSocketProtocolIcmp,
    RvSocketProtocolSctp
} RvSocketProtocol;




/********************************************************************************************
 * RvSocketInit
 * Open the Socket Module.
 * INPUT   : None
 * RETURN  : RV_OK on success, other on failure
 */
RvStatus Rv2SocketInit(void);


/********************************************************************************************
 * RvSocketEnd
 * Close the Socket module.
 * INPUT   : None
 * RETURN  : RV_OK on success, other on failure
 */
RvStatus Rv2SocketEnd(void);


/********************************************************************************************
 * RvSocketSourceConstruct
 * Create a log source for the socket module.
 * INPUT   : logMgr - log manager associated with the log source.
 * RETURN  : RV_OK on success, other on failure
 */
RvStatus Rv2SocketSourceConstruct(
    IN RvLogMgr* logMgr);


#if (RV_NET_TYPE != RV_NET_NONE)

/* =========================== */
/* ==== General functions ==== */
/* =========================== */

/********************************************************************************************
 * RvSocketConstruct
 * Construct a socket object. During construction time, the socket's type of address
 * and protocol must be supplied.
 * A call to RvSocketSetBlocking() must follow the call to this function to set
 * this socket as a blocking or a non-blocking socket.
 * This function is thread-safe.
 * INPUT   : addressType    - Address type of the created socket.
 *           protocolType   - The type of protocol to use (TCP, UDP, etc).
 *           logMgr         - log manager instance
 * OUTPUT:   sock           - Socket to construct
 * RETURN  : RV_OK on success, other on failure
 */
#if (RV_OS_TYPE != RV_OS_TYPE_MOPI)
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketConstruct(
    IN  RvInt             addressType,
    IN  RvSocketProtocol  protocolType,
    IN  RvLogMgr*         logMgr,
    OUT RvSocket*         sock);
#else
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketMopiConstruct(
    IN  RvInt             addressType,
    IN  RvSocketProtocol  protocolType,
    IN  RvLogMgr*         logMgr,
    IN  MOPI_EMB_PROC     stackType,
    OUT RvSocket*         sock);
#endif

/********************************************************************************************
 * RvSocketBind
 * Bind a socket to a local address
 * This function is NOT thread-safe.
 * INPUT   : sock       - Socket to bind
 *           address    - Address to bind socket to
 *           portRange  - Port range to use if address states an ANY port.
 *                        NULL if this parameter should be ignored.
 *           logMgr         - log manager instance
 * RETURN  : RV_OK on success, other on failure.
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketBind(
    IN RvSocket*         sock,
    IN RvAddress*        address,
    IN RvPortRange*      portRange,
    IN RvLogMgr*         logMgr);


/********************************************************************************************
 * RvSocketSetBuffers
 * Sets the send and receive buffer sizes for the specified socket. The default sizes
 * of send and receive buffers are platform dependent. This function may be used
 * to increase the default sizes if larger packets are expected, such as video packets,
 * or to decrease the default size if smaller packets are expected.
 * This function is thread-safe.
 * INPUT   : socket     - Socket whose buffer sizes are to be changed.
 *           sendSize   - The new size of the send buffer. If the size is negative, the existing
 *                        value remains.
 *           recvSize   - The new size of the receive buffer. If the size is negative, the existing
 *                        value remains.
 *           logMgr         - log manager instance
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSetBuffers(
        IN RvSocket*    sock,
        IN RvInt32      sendSize,
        IN RvInt32      recvSize,
        IN RvLogMgr*    logMgr);

/********************************************************************************************
 * RvSocketSetLinger
 * Set the linger time after socket is closed.
 * This function can only be called only on UDP sockets.
 * This function is thread-safe.
 * INPUT   : sock       - Socket to modify
 *           lingerTime - Time to linger in seconds
 *                        Setting this parameter to -1 sets linger off for this socket
 *           logMgr         - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSetLinger(
        IN  RvSocket*   sock,
        IN  RvInt32     lingerTime,
        IN  RvLogMgr*   logMgr);


/********************************************************************************************
 * RvSocketReuseAddr
 * Set the socket as a reusable one (in terms of its address)
 * This allows a TCP server socket and UDP multicast addresses to be used by
 * other processes on the same machine as well.
 * This function has to be called before RvSocketBind().
 * This function is thread-safe.
 * INPUT   : sock     - Socket to modify
 *           logMgr   - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketReuseAddr(
        IN  RvSocket*   sock,
        IN  RvLogMgr*   logMgr);


/********************************************************************************************
 * RvSocketSetBlocking
 * Set blocking/non-blocking mode on a socket.
 * This function must be called after calling RvSocketConstruct(), otherwise, the
 * socket's mode of operation will be determined by each operating system separately,
 * causing applications and stacks to be non-portable.
 * This function is thread-safe.
 * INPUT   : sock       - Socket to modify
 *           isBlocking - RV_TRUE for a blocking socket
 *                        RV_FALSE for a non-blocking socket
 *           logMgr         - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSetBlocking(
        IN RvSocket*    sock,
        IN RvBool       isBlocking,
        IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketSetTypeOfService
 * Set the type of service (DiffServ Code Point) of the socket (IP_TOS)
 * This function is supported by few operating systems.
 * IPV6 does not support type of service.
 * This function is thread-safe.
 * INPUT   : sock           - Socket to modify
 *           typeOfService  - type of service to set
 *           logMgr         - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
*/
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSetTypeOfService(
        IN RvSocket*    sock,
        IN RvInt        typeOfService,
        IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketGetTypeOfService
 * Get the type of service (DiffServ Code Point) of the socket (IP_TOS)
 * This function is supported by few operating systems.
 * IPV6 does not support type of service.
 * This function is thread-safe.
 * INPUT   : sock           - socket to modify
 *           logMgr         - log manager instance
 * OUTPUT  : typeOfService  - type of service to set
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketGetTypeOfService(
        IN  RvSocket*    sock,
        IN  RvLogMgr*    logMgr,
        OUT RvInt32*     typeOfService);


/********************************************************************************************
 * RvSocketSetBroadcast
 * Set permission for sending broadcast datagrams on a socket
 * This function is only needed if the stack being ported supports multicast sending.
 * This function is thread-safe.
 * INPUT   : sock           - Socket to modify
 *           canBroadcast   - RV_TRUE for permitting broadcast
 *                            RV_FALSE for not permitting broadcast
 *           logMgr         - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSetBroadcast(
        IN RvSocket*    sock,
        IN RvBool       canBroadcast,
        IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketSetMulticastTtl
 * Set the TTL to use for multicast sockets (UDP)
 * This function is only needed if the stack being ported supports multicast sending.
 * This function is thread-safe.
 * INPUT   : sock       - Socket to modify
 *           ttl        - TTL to set
 *           logMgr     - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSetMulticastTtl(
        IN RvSocket*    sock,
        IN RvInt32      ttl,
        IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketSetMulticastInterface
 * Set the interface to use for multicast packets (UDP)
 * This function is only needed if the stack being ported supports multicast receiving.
 * This function is thread-safe.
 * INPUT   : sock       - Socket to modify
 *           address    - Local address to use for the interface
 *           logMgr     - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSetMulticastInterface(
        IN RvSocket*    sock,
        IN RvAddress*   address,
        IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketJoinMulticastGroup
 * Join a multicast group
 * This function is only needed if the stack being ported supports multicast receiving.
 * This function is thread-safe.
 * INPUT   : sock             - Socket to modify
 *           multicastAddress - Multicast address to join
 *           interfaceAddress - Interface address to use on the local host.
 *                              Setting this to NULL chooses an arbitrary interface
 *           logMgr           - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketJoinMulticastGroup(
    IN RvSocket*    sock,
    IN RvAddress*   multicastAddress,
    IN RvAddress*   interfaceAddress,
    IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketLeaveMulticastGroup
 * Leave a multicast group
 * This function is only needed if the stack being ported supports multicast receiving.
 * This function is thread-safe.
 * INPUT   : sock             - Socket to modify
 *           multicastAddress - Multicast address to leave
 *           interfaceAddress - Interface address to use on the local host.
 *                              Setting this to NULL chooses an arbitrary interface
 *           logMgr           - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketLeaveMulticastGroup(
    IN RvSocket*    sock,
    IN RvAddress*   multicastAddress,
    IN RvAddress*   interfaceAddress,
    IN RvLogMgr*    logMgr);


/* =========================== */
/* ==== functions for TCP ==== */
/* =========================== */


/********************************************************************************************
 * RvSocketConnect
 * Starts a connection between the specified socket and the specified destination.
 * The destination must be running the RvSocketListen() function to receive the
 * incoming connection.
 * In blocking mode, this function returns only when the socket got connected, or
 * on a timeout with a failure return value.
 * In non-blocking mode, this function returns immediately, and when the remote side
 * accepts this connection, this socket will receive the RvSelectConnect event in the
 * select module.
 * This function needs to be ported only if TCP is used by the stack.
 * This function is thread-safe.
 * INPUT   : socket     - Socket to connect
 *           logMgr     - log manager instance
 * OUTPUT  : address    - Remote address to connect this socket to
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketConnect(
        IN RvSocket*    sock,
        IN RvAddress*   address,
        IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketAccept
 * Accept an incoming socket connect request, creating a new socket object.
 * In blocking mode, this function blocks until an incoming connect request to this
 * socket is made.
 * In non-blocking mode, this function will exit immediately and when an incoming
 * connection request to this socket is made, this socket will receive the
 * RvSocketAccept event in the select module.
 * The newSocket object should be regarded as if it was created using
 * RvSocketConstruct().
 * This function needs to be ported only if TCP is used by the stack.
 * This function is thread-safe.
 * INPUT   : socket         - Listening socket receiving the incoming connection
 *           logMgr         - log manager instance
 * OUTPUT  : newSocket      - Accepted socket information
 *           remoteAddress  - Address of remote side of connection
 *                            Can be passed as NULL if not needed
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketAccept(
    IN  RvSocket*    sock,
    IN  RvLogMgr*    logMgr,
    OUT RvSocket*    newSocket,
    OUT RvAddress*   remoteAddress);


/********************************************************************************************
 * RvSocketDontAccept
 *
 * Rejects an incoming socket connect request.
 * This function needs to be ported only if TCP is used by the stack.
 * This function is thread-safe.
 * INPUT   : socket         - Listening socket receiving the incoming connection
 *           logMgr         - log manager
 * OUTPUT  : none
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketDontAccept(
    IN  RvSocket*    sock,
    IN  RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketListen
 * Listens on the specified socket for connections from other locations.
 * In non-blocking mode, when a connection request is received, this socket will receive
 * the RvSocketAccept event in the select module.
 * This function needs to be ported only if TCP is used by the stack.
 * This function is thread-safe.
 * INPUT   : sock           - Listening socket receiving the incoming connection
 *           queuelen       - Length of queue of pending connections
 *           logMgr         - log manager instance
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketListen(
        IN  RvSocket    *sock,
        IN  RvUint32    queuelen,
        IN  RvLogMgr*   logMgr);


/********************************************************************************************
 * RvSocketShutdown
 * Shutdown a TCP socket. This function should be called before calling
 * RvSocketDestruct() for TCP sockets.
 * In blocking mode, this function blocks until RvSocketDestruct() can be called in
 * a graceful manner to close the connection.
 * In non-blocking mode, when the remote side will close its connection, this socket
 * will receive the RvSocketClose event in the select module and RvSocketDestruct()
 * should be called at that point.
 * This function needs to be ported only if TCP is used by the stack.
 * This function is NOT thread-safe.
 * INPUT   : socket         - Socket to shutdown
 *           cleanSocket    - RV_TRUE if you want to clean the socket before shutting it down.
 *                            this will try to receive from the socket some buffers.
 *                            It is suggested to set this value to RV_TRUE for non-blocking
 *                            sockets.
 *           logMgr         - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketShutdown(
    IN RvSocket*    sock,
    IN RvBool       cleanSocket,
    IN RvLogMgr*    logMgr);


/********************************************************************************************
 * RvSocketDestruct
 * Close a socket.
 * This function is NOT thread-safe.
 * INPUT   : sock           - Socket to shutdown
 *           cleanSocket    - RV_TRUE if you want to clean the socket before shutting it down.
 *                            this will try to receive from the socket some buffers.
 *                            It is suggested to set this value to RV_TRUE for TCP sockets.
 *                            It should be set to RV_FALSE for UDP sockets.
 *           portRange      - Port range to return this socket's port to. If NULL, the
 *                            socket's port will not be added to any port range object.
 *           logMgr         - log manager instance
 * OUTPUT  : None
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketDestruct(
    IN RvSocket*    sock,
    IN RvBool       cleanSocket,
    IN RvPortRange* portRange,
    IN RvLogMgr*    logMgr);

/********************************************************************************************
 * RvSocketClean
 *
 * Read & drop all waiting on the socket data.
 *
 * INPUT   : sock           - Socket to shutdown
 *           logMgr         - log manager pointer
 * OUTPUT  : None
 * RETURN  : none
 */
RVCOREAPI
void RVCALLCONV Rv2SocketClean(
    IN RvSocket*    sock,
    IN RvLogMgr*    logMgr);

/* ================================ */
/* ==== Send/receive functions ==== */
/* ================================ */

/********************************************************************************************
 * RvSocketSendBuffer
 * Send a buffer on a socket.
 * This function actually copies the given buffer to the operating system's memory
 * for later sending.
 * In blocking mode, this function will block until the buffer is sent to the remote
 * side or upon failure.
 * In non-blocking mode, this function will return immediately, indicating the exact
 * amount of bytes the operating system has processed and sent.
 * This function handles both TCP and UDP sockets. You might need to port only parts
 * of this function if you need only one of these protocols.
 * This function is thread-safe.
 * INPUT   : socket         - Socket to send the buffer on
 *           buffer         - Buffer to send
 *           bytesToSend    - Number of bytes to send from buffer
 *           remoteAddress  - Address to send the buffer to.
 *                            Should be set to NULL on connection-oriented sockets (TCP).
 *           logMgr         - log manager instance
 * OUTPUT  : bytesSent      - Number of bytes we sent
 *                            If less than bytesToSend, then we would block if we tried on a
 *                            blocking socket. The application in this case should wait for the
 *                            RvSelectWrite event in the select module before trying to send
 *                            the buffer again.
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketSendBuffer(
    IN  RvSocket*   sock,
    IN  RvUint8*    buffer,
    IN  RvSize_t    bytesToSend,
    IN  RvAddress*  remoteAddress,
    IN  RvLogMgr*   logMgr,
    OUT RvSize_t*   bytesSent);


/********************************************************************************************
 * RvSocketReceiveBuffer
 * Receive a buffer from a socket.
 * In blocking mode, this function will block until a buffer is received on the
 * socket.
 * In non-blocking mode, this function returns immediately, even if there is nothing
 * to be received on this socket at the moment. This function is usually called after
 * RvSelectRead event is received in the select module on this socket, indicating that
 * there is information to receive on this socket.
 * This function handles both TCP and UDP sockets. You might need to port only parts
 * of this function if you need only one of these protocols.
 * This function is thread-safe.
 * INPUT   : socket         - Socket to receive the buffer from
 *           buffer         - Buffer to use for received data
 *           bytesToReceive - Number of bytes available on the given buffer
 *                            For UDP sockets, this value must be higher than the incoming datagram.
 *           logMgr         - log manager instance
 * OUTPUT  : bytesReceived  - Number of bytes that were actually received
 *           remoteAddress  - Address buffer was received from
 *                            Should be given as NULL on connection-oriented sockets (TCP).
 *                            This address is constructed by this function and should be
 *                            destructed by the caller to this function
 * RETURN  : RV_OK on success, other on failure
 *
 * Notes:
 *   * Return value of RV_SOCKET_EOF_ERROR on TCP socket indicates that peer tries
 *     to close this connection. 'bytesReceived' in this case will be 0
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketReceiveBuffer(
    IN  RvSocket*   sock,
    IN  RvUint8*    buffer,
    IN  RvSize_t    bytesToReceive,
    IN  RvLogMgr*   logMgr,
    OUT RvSize_t*   bytesReceived,
    OUT RvAddress*  remoteAddress);


/* =========================== */
/* ==== Utility functions ==== */
/* =========================== */

/********************************************************************************************
 * RvSocketGetBytesAvailable
 * Get the number of bytes that are in the receive buffer of the socket.
 * This number might be larger than the return result of a single recv() operation.
 * This is important when dealing with data-gram types of connections (such as UDP).
 * This function is not available by some of the operating systems.
 * This function is NOT thread-safe.
 * INPUT   : socket         - Socket to check
 *           logMgr         - log manager instance
 * OUTPUT  : bytesAvailable - Number of bytes in the receive buffer of the socket
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketGetBytesAvailable(
    IN RvSocket*  sock,
    IN RvLogMgr*  logMgr,
    OUT RvSize_t* bytesAvailable);


/********************************************************************************************
 * RvSocketGetLastError
 * Get the last error that occured on a socket.
 * This function works for synchronous sockets only.
 * The return value is OS-specific. A value of 0 means no error occured.
 * This function is thread-safe.
 * INPUT   : sock     - Socket to check
 *           logMgr         - log manager instance
 * OUTPUT  : lastError  - Error that occured. 0 means no error.
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketGetLastError(
    IN RvSocket* sock,
    IN RvLogMgr* logMgr,
    OUT RvInt32* lastError);


/********************************************************************************************
 * RvSocketGetLocalAddress
 * Get the local address used by this socket.
 * This function is thread-safe.
 * INPUT   : sock     - Socket to check
 *           logMgr   - log manager instance
 * OUTPUT  : address    - Local address
 *                        Must be destructed by the caller to this function
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketGetLocalAddress(
    IN RvSocket  *sock,
    IN RvLogMgr  *logMgr,
    OUT RvAddress* address);


/********************************************************************************************
 * RvSocketGetRemoteAddress
 * Get the remote address used by this socket.
 * This function can only be called only on TCP sockets.
 * This function is thread-safe.
 * INPUT   : sock     - Socket to check
 *           logMgr   - log manager instance
 * OUTPUT  : address  - Remote address
 *                      Must be destructed by the caller to this function
 * RETURN  : RV_OK on success, other on failure
 */
RVCOREAPI
RvStatus RVCALLCONV Rv2SocketGetRemoteAddress(
    IN RvSocket  *sock,
    IN RvLogMgr  *logMgr,
    OUT RvAddress* address);


#if (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS)

void Rv2SocketNucleusTaskDelete(IN RvSocket* socket);

void Rv2SocketNucleusTaskClean(IN RvSocket* socket);

/********************************************************************************************
 * RvSocketSetSelectEngine
 * Update the socket database - associate a socket with it's select-engine.
 * This function is thread-safe.
 * INPUT   : socket       - Socket to check
 *           selectEngine - select-engine that probes the socket.
 * OUTPUT  : None.
 * RETURN  : RV_OK on success, other on failure
 */
RvStatus Rv2SocketSetSelectEngine(
    IN RvSocket *socket,
    IN void* selectEngine);

#endif


#if (RV_SOCKET_USE_SHARER == RV_YES)

/**********************************************************************************
 * RvSocketSharerMultiShare
 * "share" a set of sockets, using the socket sharer module. this is called right
 * before calling 'select()' function.
 * INPUT:
 *  sock   - a pointer to a set of sockets to share.
 *  width  - the length of the set.
 * RETURN:
 *  RvStatus - RV_OK on success, other on failure.
 */
RvStatus Rv2SocketSharerMultiShare(
    IN OUT RvSocket* sock,
    IN int width);


/**********************************************************************************
 * RvSocketSharerClose
 * close a socket in the socket sharer module.
 * INPUT:
 *  _sock   - Socket to close
 * RETURN:
 *  RvStatus - RV_OK on success, other on failure.
 */
void Rv2SocketSharerClose(IN RvSocket* sock);


#endif /* (RV_SOCKET_USE_SHARER == RV_YES) */


#if (RV_OS_TYPE == RV_OS_TYPE_MOPI)

void Rv2SocketMopiSetConnectionId(MOPI_EMB_PROC proc, RvUint16 cid);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* RV_SOCKET_H */

#endif /* (RV_NET_TYPE != RV_NET_NONE) */
