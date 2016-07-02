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

#ifndef _TPKT_H
#define _TPKT_H

#include "rvinternal.h"
#include "rpool.h"
#include "cm.h"
#include "rvh323connection.h"

#ifdef __cplusplus
extern "C" {
#endif


RV_DECLARE_HANDLE(HTPKT);
RV_DECLARE_HANDLE(HTPKTCTRL);

typedef void (*LPTPKTEVENTHANDLER)(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context);

typedef enum
{
    tpktClient,
    tpktServer,
    tpktMultiServer,
    tpktAcceptedServer
} tpktTypes;


/*******************************************************************************************
 * tpktInit
 *
 * Purpose: Initialize the structures of the tpkt module
 *
 * Input:   hApp        - Stack's application handle
 *          selEngine   - Select Engine, used for the file descriptors
 *          sessions    - Maximal number of allowed connections
 *          portRange   - Range of ports to use
 *          hTimers     - timers pool handle 
 *          logMgr      - Log manager to use
 * Return Value: Handle to the TPKT instance
 *******************************************************************************************/
HTPKTCTRL tpktInit(
    IN HAPP             hApp,
    IN RvSelectEngine*  selEngine,
    IN int              sessions,
    IN RvPortRange*     portRange,
    IN RvH323TimerPoolHandle hTimers, 
    IN RvLogMgr*        logMgr);

/*******************************************************************************************
 * tpktEnd
 *
 * Purpose: Destroy the tpktInfo elements and close all connections
 *
 * Input:   hCtrl - Handle to the list of tpktInfo elements
 *
 * Return Value: 0
 *******************************************************************************************/
int tpktEnd(HTPKTCTRL hCtrl);

/*******************************************************************************************
 * tpktOpen
 *
 * Purpose: Allocate a socket and start either a connect process in case of a client which
 *      supplied a full address, or a listenning process in case of server.
 *      For a client that didn't supply an address yet, just create the socket.
 *
 * Input:   hTpktCtrl       - Handle to the tpkt instance
 *          localAddress    - Local address to use
 *          connectionType  - What type of connection we request (Tcp or TcpServer)
 *          standard        - Standard to use on connection (Q931 or H245)
 *          context         - Additional data to be supplied on the callback
 *
 * Return Value: A handle to the tpkt object created
 *******************************************************************************************/
HTPKT tpktOpen(
    IN HTPKTCTRL            hTpktCtrl,
    IN cmTransportAddress*  localAddress,
    IN RvH323ConnectionType connType,
    IN tpktTypes            tpktType,
    IN LPTPKTEVENTHANDLER   eventHandler,
    IN void*                context);

/*******************************************************************************************
 * tpktConnect
 *
 * Purpose: This routine supplements the tpktOpen routine, for clients which didn't supply
 *          an address and now wish to do the actual connect operation on the already allocated
 *          socket.
 *
 * Input:   hTpkt           - Handle to the tpktInfo element
 *          remoteAddress   - Address to connect to
 *
 * Return Value: Non-negative value on success, other on failure
 *******************************************************************************************/
RvStatus tpktConnect(IN HTPKT hTpkt, IN cmTransportAddress* remoteAddress);

/*******************************************************************************************
 * tpktClose
 *
 * Purpose: Starts the closing procedure of a tpkt object.
 *          Frees all its resources if an Idle event won't arrive from the IO layer for the
 *          TPKT object.
 *
 * Input:   hTpkt - Handle to the tpktInfo element
 *
 * Return Value: Non-negative value on success, other on failure
 *******************************************************************************************/
int tpktClose(IN HTPKT hTpkt);

/*******************************************************************************************
 * tpktDelete
 *
 * Purpose: Releases a tpkt object and frees all its resources
 *
 * Input:   hTpkt - Handle to the tpktInfo element
 *
 * Return Value: Non-negative value on success, other on failure
 *******************************************************************************************/
int tpktDelete(IN HTPKT hTpkt);

/*******************************************************************************************
 * tpktSendFromRpool
 *
 * Purpose: This routine sends a given rpool message over a connection, assuming that the message
 *      already contains a tpkt header.
 *
 * Input:   hTpkt           - Handle to the tpktInfo element
 *          hRpool          - handle to the rpool.
 *          message         - points to the message to be sent.
 *          offset          - where in the message to start the sending.
 *          rpoolLock       - The lock of the rpool
 *
 * Return Value: 0 or RVERROR
 *******************************************************************************************/
 int tpktSendFromRpool(HTPKT hTpkt,
                      HRPOOL hRpool,
                      HRPOOLELEM message,
                      int offset,
                      RvLock* rpoolLock);

/*******************************************************************************************
 * tpktRecvNotify
 *
 * Purpose: to set a CallBack routine to be called when data arrives.
 *      This routine is called on EvAccept since it's fast.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *      eventHandler - The callback to be read on receiving data
 *      context - To be returned with the callback (may be appl handle etc.)
 * Return Value: -1 : on Error;
 *        0 : OK
 *******************************************************************************************/
int tpktRecvNotify(HTPKT hTpkt,LPTPKTEVENTHANDLER eventHandler,void*context);

/*******************************************************************************************
 * tpktRecvIntoRpool
 *
 * Purpose: to initiate the reading of a new message while seting a new CallBack routine
 *          to be called when data arrives.
 *          The routine handles any problems that arise from the fact that an rpool message
 *          may be not contiguous.
 *          This routine is called on each EvRead event, but NOT on EvAccept since it's
 *          too slow.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *      buffer - The buffer into which to read the received data
 *      length - Maximal size of the buffer
 *
 * Return Value: -1 : on Error;
 *        0 : on not receiving a full buffer or a full message;
 *       >0 : Number of bytes read (may be either the length of the bufer
 *            in case of an incomplete message or the message length).
 *******************************************************************************************/
int tpktRecvIntoRpool(HTPKT hTpkt,
                      HRPOOL hRpool,
                      HRPOOLELEM message,
                      RvLock* rpoolLock);

/*******************************************************************************************
 * tpktGetAddresses
 *
 * Purpose: return the addresses associated with the given tpkt object.
 *
 * Input:   hTpkt - A handle to the TPKT module
 * Output:  localAddress    - Local address of the connection. If NULL, not updated
 *          remoteAddress   - Remote address of the connection. If NULL, not updated
 *
 * Return Value: Non-negative value on success, other on failure
 *******************************************************************************************/
RvStatus tpktGetAddresses(
    IN  HTPKT               hTpkt,
    OUT cmTransportAddress* localAddress,
    OUT cmTransportAddress* remoteAddress);

/*******************************************************************************************
 * tpktGetContext
 *
 * Purpose: return the context of the given tpkt object. The context is the host of this
 *          connection.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: Context on success, NULL on failure
 *******************************************************************************************/
void* tpktGetContext(
    IN  HTPKT   hTpkt);

/*******************************************************************************************
 * tpktGetConnection
 *
 * Purpose: return the connection of the given tpkt object.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: IO connection on success, NULL on failure
 *******************************************************************************************/
RvH323Connection* tpktGetConnection(
    IN  HTPKT   hTpkt);

/*******************************************************************************************
 * tpktGetFromConnection
 *
 * Purpose: return the TPKT object from a connection.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: TPK handle on success, NULL on failure
 *******************************************************************************************/
HTPKT tpktGetFromConnection(
    IN  RvH323Connection*   connection);

/*******************************************************************************************
 * tpktIsConnected
 *
 * Purpose: return isConnected member of the TPKT object.
 *
 * Input:   hTpkt - A handle to the TPKT module
 *
 * Return Value: isConnected
 *******************************************************************************************/
RvBool tpktIsConnected(
    IN  HTPKT   hTpkt);

int tpktSendPinhole(HTPKT hTpkt);//add for pinhole [20120830]

#ifdef __cplusplus
}
#endif

#endif  /* _TPKT_H */



