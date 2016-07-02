/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#ifndef _RV_CMHOOK_H
#define _RV_CMHOOK_H

#ifdef __cplusplus
extern "C" {
#endif


#define CM_PROTOCOL_RAW_MESSAGE_HEADER_SIZE sizeof(void*)+4


typedef RvBool
    (RVCALLCONV *cmHookListenT)(/*Before listen*/
        IN      HPROTCONN           hConn,
        IN      int                 addr
        );


typedef int
    (RVCALLCONV *cmHookListeningT)(/*After listen*/
        IN      HPROTCONN           hConn,
        IN      int                 addr,
        IN      RvBool              error
        );

typedef int
    (RVCALLCONV*cmHookConnectingT)(/*Before connect*/
        IN      HPROTCONN           hConn,
        IN      int                 addr
        );

typedef int
    (RVCALLCONV*cmHookInConnT)(/*Incomming connect*/
        IN      HPROTCONN           hConn,
        IN      int                 addrFrom,
        IN      int                 addrTo
        );
typedef int
    (RVCALLCONV*cmHookOutConnT)(/*Outgoing connect*/
        IN      HPROTCONN           hConn,
        IN      int                 addrFrom,
        IN      int                 addrTo,
        IN      RvBool              error
        );
typedef RvBool
    (RVCALLCONV*cmHookSendT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      RvBool              error
        );
typedef RvBool
    (RVCALLCONV*cmHookRecvT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      RvBool              error
        );

typedef RvBool
    (RVCALLCONV*cmHookSendToT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      int                 addrTo,
        IN      RvBool              error
        );
typedef RvBool
    (RVCALLCONV*cmHookRecvFromT)(
        IN      HPROTCONN           hConn,
        IN      int                 nodeId,
        IN      int                 addrFrom,
        IN      RvBool              multicast,
        IN      RvBool              error
        );

typedef void
    (RVCALLCONV*cmHookCloseT)(
        IN      HPROTCONN           hConn
        );


typedef  struct
{
   cmHookListenT      hookListen;
   cmHookListeningT   hookListening;
   cmHookConnectingT  hookConnecting;
   cmHookInConnT      hookInConn;
   cmHookOutConnT     hookOutConn;
   cmHookSendT        hookSend;
   cmHookRecvT        hookRecv;
   cmHookSendToT      hookSendTo;
   cmHookRecvFromT    hookRecvFrom;
   cmHookCloseT       hookClose;
} SCMPROTOCOLEVENT,*CMPROTOCOLEVENT;


/************************************************************************
 * cmSetProtocolEventHandler
 * purpose: Installs the protocol event handler.
 * input  : hApp            - Stack handle for the application
 *          cmProtocolEvent - Pointer to array of pointers to general event callback functions
 *          size            - Size of *cmProtocolEvent in bytes
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmSetProtocolEventHandler(
        IN  HAPP            hApp,
        IN  CMPROTOCOLEVENT cmProtocolEvent,
        IN  int             size);

RVAPI
int RVCALLCONV cmSetProtocolEventHandlerReplacement(
        IN  HAPP            hApp,
        IN  CMPROTOCOLEVENT cmProtocolEventReplacement,
        IN  int             size);


/******************************************************************************
 * cmProtocolSendMessage
 * ----------------------------------------------------------------------------
 * General: Send a message through a given connection
 *          This function should be used for Q931 and H.245 messages.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - Stack handle for the application.
 *         hConn    - Connection to use.
 *         msg      - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvPvtNodeId msg);


/******************************************************************************
 * cmProtocolSendMessageTo
 * ----------------------------------------------------------------------------
 * General: Send a RAS message to a given remote address.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp     - Stack handle for the application.
 *         hConn    - Connection to use.
 *         msg      - Node id of the message to send.
 *                    The message is not deleted by the call to this function.
 *         addr     - Node id of TransportAddress type, indicating the
 *                    remote address to send the message to.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendMessageTo(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvPvtNodeId msg,
        IN  RvPvtNodeId addr);


/******************************************************************************
 * cmProtocolSendRawMessage
 * ----------------------------------------------------------------------------
 * General: Send a raw message through a given connection.
 *          This function should be used for Q931 and H.245 messages. It allows
 *          sending malformed messages to remote clients and is useful for
 *          testing tools development.
 *          Raw messages are packetized using TPKT as any other Q931 and H.245
 *          message.
 *          Note: The raw message must have a clean header of
 *          CM_PROTOCOL_RAW_MESSAGE_HEADER_SIZE bytes at its beginning for the
 *          internal use of the Stack. The bufferSize value must include this
 *          additional header size in its calculation.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolSendRawMessage(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvUint8     *buffer,
        IN  RvSize_t    bufferSize);


/******************************************************************************
 * cmProtocolRecvRawMessageFrom
 * ----------------------------------------------------------------------------
 * General: Simulate receiving a raw message on a given RAS connection.
 *          This can be used for testing or for support of listening to several
 *          different RAS addresses (since the Stack does not support this
 *          feature).
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * See Also: cmGetUdpChanHandle, cmGetRASConnectionHandle.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hApp         - Stack handle for the application.
 *         hConn        - Connection to use (can be the unicast or the
 *                        multicast RAS connection handle).
 *         buffer       - Buffer of received message.
 *         bufferSize   - Size of buffer.
 *         fromAddress  - Address this message is "received" from.
 * Output: none
 *****************************************************************************/
RVAPI
RvStatus RVCALLCONV cmProtocolRecvRawMessageFrom(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        IN  RvUint8             *buffer,
        IN  RvSize_t            bufferSize,
        IN  cmTransportAddress  *fromAddress);




typedef enum
{
    cmProtocolUnknown,
    cmProtocolRAS,
    cmProtocolQ931,
    cmProtocolH245
} cmProtocol;


/* Creates new message value tree*/
RVAPI int /* New message nodeId*/
RVCALLCONV cmProtocolCreateMessage(
                IN      HAPP                    hApp,
                IN      cmProtocol              protocol);

RVAPI/* returns protocol of the message tree*/
cmProtocol RVCALLCONV cmProtocolGetProtocol(
                IN      HAPP                    hApp,
                IN      int                     msg);/* message nodeId*/


RVAPI
char* RVCALLCONV cmProtocolGetProtocolName(
                IN      cmProtocol              protocol);


/************************************************************************
 * cmProtocolGetLocalAddress
 * purpose: Get the local address of a given connection of the stack.
 *          This function can be used on Q.931 or H.245 connections.
 * input  : hApp    - Stack handle for the application
 *          hConn   - Connection to use
 * output : address - The local address of this connection
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolGetLocalAddress(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        OUT cmTransportAddress* address);


/************************************************************************
 * cmProtocolGetRemoteAddress
 * purpose: Get the remote address of a given connection of the stack.
 *          This function can be used on Q.931 or H.245 connections.
 * input  : hApp    - Stack handle for the application
 *          hConn   - Connection to use
 * output : address - The remote address of this connection
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolGetRemoteAddress(
        IN  HAPP                hApp,
        IN  HPROTCONN           hConn,
        OUT cmTransportAddress* address);


/************************************************************************
 * cmProtocolSetTypeOfService
 * purpose: Set the connection's DiffServ Code Point value. This is used
 *          for QoS.
 *          This function can be used on RAS, Q.931 or H.245 connections.
 * input  : hApp            - Stack handle for the application
 *          hConn           - Connection to use
 *          typeOfService   - Type of service value
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV cmProtocolSetTypeOfService(
        IN  HAPP        hApp,
        IN  HPROTCONN   hConn,
        IN  RvInt       typeOfService);


RVAPI
HPROTCONN RVCALLCONV cmGetTpktChanHandle(
    IN HCALL                hsCall,
    IN cmTpktChanHandleType tpktChanHandleType);


RVAPI
HPROTCONN RVCALLCONV cmGetUdpChanHandle(IN HCALL hsCall,cmUdpChanHandleType  udpChanHandleType);


RVAPI
int RVCALLCONV cmSetUdpChanApplHandle(IN HPROTCONN hCon,HAPPCONN hAppConn);


RVAPI
int RVCALLCONV cmSetTpktChanApplHandle(
    IN HPROTCONN    hConn,
    IN HAPPCONN     hAppConn);


RVAPI
int RVCALLCONV cmGetUdpChanApplHandle(IN HPROTCONN hCon,HAPPCONN * hAppConn);


RVAPI
int RVCALLCONV cmGetTpktChanApplHandle(
    IN  HPROTCONN   hConn,
    OUT HAPPCONN*   hAppConn);




#ifdef __cplusplus
}
#endif

#endif  /* _RV_CMHOOK_H */


