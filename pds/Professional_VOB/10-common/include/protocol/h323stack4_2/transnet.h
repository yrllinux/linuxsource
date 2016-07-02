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

#ifndef _TRANSNET_H
#define _TRANSNET_H


#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************
 * createHostByType
 *
 * Purpose: To allocate a new host element and fill it with data according to its
 *          type, i.e. Q.931-TPKT, Q.931-Annex E, H.245
 *
 * Input:  transGlobals     - The global variables of the module
 *         hsTranSession    - The session for which the host is created (may be NULL)
 *         type             - The type of the connection (TPKT, H.245, Annex E)
 *         hTpkt            - The connection handle in TPKT for a new incoming connection
 *         annexEUsageMode  - is this an annex E host or other, i.e. TPKT for Q.931 or H.245
 *         annexELocalIndex - the local address index for annex E. 0 o.w.
 *
 * Output: hsTransHost -  A handle for the newly created host element.
 *
 **************************************************************************************/
 TRANSERR createHostByType(cmTransGlobals      *transGlobals,
                           HSTRANSSESSION      hsTransSession,
                           HSTRANSHOST         *hsTransHost,
                           TRANSCONNTYPE       type,
                           HTPKT               hTpkt,
                           cmAnnexEUsageMode   annexEUsageMode,
                           int                 annexELocalIndex);

/**************************************************************************************
 * determineWinningHost
 *
 * Purpose: check if there was a competition between TPKT and annex E, and if so
 *          close the Annex E.
 *
 * Input:   transGlobals  - The global variables of the module
 *          session       - The session for which the conection was made
 *          annexEWon     - which of the two hosts won..
 *
 * Return Value: RV_TRUE - The decision was not overturned, RV_FALSE - Otherwise.
 **************************************************************************************/
RvBool determineWinningHost(cmTransGlobals *transGlobals, cmTransSession *session, RvBool annexEWon);

/**************************************************************************************
 * transReportConnect
 *
 * Purpose: To invoke the callbacks for the connected host and all its associated sessions
 *
 * Input: host         - The host which got connected.
 *        session      - The session on which to report the host connection (NULL for all).
 *        isConnected  - RV_TRUE: A connect happened, RV_FALSE: an accept happened.
 *
 **************************************************************************************/
void transReportConnect(cmTransHost *host, cmTransSession *session, RvBool isConnected);

/**************************************************************************************
 * solveH245Conflict
 *
 * Purpose: To decide which one of two coliding H.245 connections will survive.
 *
 * Input: host         - The h.245 host which has a listen and connecting processes.
 *
 **************************************************************************************/
void solveH245Conflict(cmTransHost *host);

 /**************************************************************************************
 * transQ931AcceptHandler
 *
 * Purpose: The callback routine for the socket that listens for incoming Q.931 requests
 *
 * Input: standard input of TPKT callback
 *
 **************************************************************************************/
 void transQ931AcceptHandler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context);

 /**************************************************************************************
 * transH245Handler
 *
 * Purpose: The callback routine for a H245 connection
 *
 * Input: standard input of TPKT callback
 *
 **************************************************************************************/
 void transH245Handler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context);

 /**************************************************************************************
 * transH245AcceptHandler
 *
 * Purpose: The callback routine for the socket that listens for incoming H.245 request
 *
 * Input: standard input of TPKT callback
 *
 **************************************************************************************/
 void transH245AcceptHandler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context);

 /**************************************************************************************
 * transQ931Handler
 *
 * Purpose: The callback routine for a Q.931 connection
 *
 * Input: standard input of TPKT callback
 *
 *
 **************************************************************************************/
 void transQ931Handler(HTPKT tpkt,RvSelectEvents event,int length,int error,void*context);

/**************************************************************************************
 * transHostConnect
 *
 * Purpose: Starts the process of connecting on the given host connection.
 *
 * Input:   hsTransSession  - the handle to the session.
 *          type            - The type of the connection (annex E or TPKT)
 *
 * Output:  None.
 *
 * Returned Value:  cmTransWouldBlock - In case that a connect procedure was instigated but
 *                                      not yet completed.
 *                  cmTransOK         - In case that the connection is already opened.
 *                  cmTransErr        - couldn't create the hosts
 **************************************************************************************/
TRANSERR transHostConnect(IN HSTRANSSESSION hsTransSession,
                          IN TRANSINTCONNTYPE  type);

/**************************************************************************************
 * sendMessageOnHost
 *
 * Purpose: send a message on the given host according to its communication protocol.
 *          this will also send any messages that are linked after this one.
 *
 * Input:   transGlobal - The global variables of the module.
 *          host        - The host on which to send the messages that await to be sent.
 *          rpoolMessage- RPOOL message to send on the host
 *
 * Output:  None.
 *
 * Returned Value:  cmTransOK             - All is ok.
 *                  cmTransConnectionBusy - The connection is still sending previous messages.
 **************************************************************************************/
TRANSERR sendMessageOnHost(
    IN cmTransGlobals*  transGlobals,
    IN cmTransHost*     host);

/**************************************************************************************
 * decodeIncomingMessage
 *
 * Purpose: tries to receive and decode an incoming message
 *
 * Input:   tpkt    - a handle to the TPKT connection
 *          length  - the length of the received message (0 if no buffers for reading the message)
 *          offset  - Where to start the decoding process in the encoded buffer
 *          context - The context associated with the connection , i.e. the host element
 *          node    - The pvt node into which to decode the message
 *          type    - The type of the message.
 *
 *
 * Output:  hMsgContext - An external context associated with the message.
 *
 * Returned Value:  number of bytes decoded, or <=0 in case of trouble.
 **************************************************************************************/
int  decodeIncomingMessage( HTPKT       tpkt,
                            int         *length,
                            int         *offset,
                            void        *context,
                            int         *node,
                            TRANSTYPE   type,
                            void        **hMsgContext);

/**************************************************************************************
 * processQ931IncomingMessage
 *
 * Purpose: handles incoming messages:
 *          extract the states of the faststart and tunneling (including parallel one).
 *          updates the multiplexing parameters of the host
 *          decides whether to report the openning of a H.245 connection
 *          decides whether to initiate a connect for H.245
 *          extract all kinds of tunneled messages from the message
 *
 * Input:   host    - The host on which the message arrived
 *          session - The session on which the message arrived (i.e. the call)
 *          pvtNode - The decoded message itself
 *          manualRAS - The RAS mode of the application
 *
 * Output:  None.
 *
 * Return value: msgType - the message type.
 *
**************************************************************************************/
int processQ931IncomingMessage(cmTransHost *host, cmTransSession *session, int pvtNode);

/**************************************************************************************
 * connectH245
 *
 * Purpose: connects if the host state permits, a H.245 connection.
 *
 * Input:   transGlobal - The global variables of the module.
 *          session     - The session for which the connection is done
 *          msgType     - The type of the message that is being checked
 *
 * Output:  None.
 *
 **************************************************************************************/
void connectH245(cmTransGlobals *transGlobals,
                 cmTransSession *session,
                 int            msgType);

/**************************************************************************************
 * reportH245Establishment
 *
 * Purpose: if not done so yet, report to the user about an active H.245 connection.
 *
 * Input:   transGlobal - The global variables of the module.
 *          session     - The session for which the connection is done
 *          host        - the host on which message arrived
 *
 * Output:  None.
 *
 **************************************************************************************/
void reportH245Establishment(cmTransGlobals *transGlobals, cmTransSession *session, cmTransHost  *host);

/**************************************************************************************
 * annexEEvNewMessage
 *
 * Purpose: Callback for Annex E upon receiving new incoming message.
 *          The call back looks for the host (or create one if it's new) and
 *          then calls the regular event handler (of TPKT) with event liEvRead.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actually
 *                          our very own transGlobals structure).
 *          localAddrIndex- Index of the local address that received this message.
 *          remoteAddress - The remote address whence the message arrived.
 *          pMessage      - The encoded message itself.
 *          nSize         - The size of the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvNewMessage(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  RvInt               localAddressIndex,
    IN  cmTransportAddress* remoteAddress,
    IN  void*               pMessage,
    IN  int                 nSize);

/**************************************************************************************
 * annexEEvConnectionBroken
 *
 * Purpose: Callback for Annex E upon expiration of I-AM-ALIVE timer & retries.
 *          The call back looks for the host and then calls the regular event handler
 *          (of TPKT) with event liEvClose.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actualy
 *                          our very own transGlobals structure).
 *          remoteAddress - The remote address whence the message arrived.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvConnectionBroken(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  cmTransportAddress* remoteAddress,
    IN  int                 localAddressIndex
);

/**************************************************************************************
 * annexEEvUseOtherAddress
 *
 * Purpose: Callback for Annex E upon receiving a request to change the remote address.
 *          The call back looks for the host and then deletes it from the host hash only
 *          to put it there agian with the new address.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actualy
 *                          our very own transGlobals structure).
 *          remoteAddress - The remote address whence the message arrived.
 *          pNewAddress   - the new address to set the host to.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvUseOtherAddress(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  cmTransportAddress* remoteAddress,
    IN  cmTransportAddress* pNewAddress,
    IN  int                 localAddressIndex
);

/**************************************************************************************
 * annexEEvWriteable
 *
 * Purpose: Callback for Annex E when resources are available again for sending, after
 *          receiving would block error on send.
 *          The call back looks for the host and then calls the regular event handler
 *          (of TPKT) with event liEvWrite.
 *
 * Input:   hAnnexE       - The annex E module handle
 *          hAppAnnexE    - The application handle of the annex E module (actualy
 *                          our very own transGlobals structure).
 *          remoteAddress - The remote address whence the message arrived.
 *
 * Output:  None.
 *
 **************************************************************************************/
annexEStatus RVCALLCONV annexEEvWriteable(
    IN  HANNEXE             hAnnexE,
    IN  HAPPANNEXE          hAppAnnexE,
    IN  cmTransportAddress* remoteAddress,
    IN  int                 localAddressIndex
);

/**************************************************************************************
 * zeroTimerEvent
 *
 * Purpose: call back rotine that simulates a connect from the network on one of the
 *          connections (H245, Q931 or Annex E). It is used when a host is already
 *          connected and we just want to simulate its connection event as if we got
 *          it from the network
 *
 * input   : userData   - The handle to the session on which we need to report the connect.
 *
 * Output:  Always RV_FALSE.
 *
 **************************************************************************************/
RvBool zeroTimerEvent(IN void*  userData);


#ifdef __cplusplus
}
#endif


#endif  /* _TRANSNET_H */
