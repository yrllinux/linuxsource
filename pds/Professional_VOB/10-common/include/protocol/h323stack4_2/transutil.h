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

#ifndef _TRANSUTIL_H
#define _TRANSUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

    

/******************************************************************************
 * updateHostMultiplexStatus
 * ----------------------------------------------------------------------------
 * General: Update the status of a multiplexed host, moving it into or out of
 *          the garbage collection list of empty multiplexed hosts.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global data of the module.
 *         host         - Host to update.
 * Output: none
 *****************************************************************************/
RvStatus updateHostMultiplexStatus(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host);


/******************************************************************************
 * garbageCollectMultiplexHost
 * ----------------------------------------------------------------------------
 * General: Check if we need to garbage collect a multiplexed host and drop it
 *          if we have to.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global data of the module.
 * Output: none
 *****************************************************************************/
RvStatus garbageCollectMultiplexHost(
    IN cmTransGlobals   *transGlobals);


/**************************************************************************************
 * createMessage
 *
 * Purpose: creates a skeleton message of the given type and fills in the CRV and callId
 *          of the given session.
 * Input:   transGlobals    - The global data of the module.
 *          msgType         - which message to create
 *          CRV             - CRV to use.
 *          callId          - call Identifier to use.
 *
 * Output:  node - the created message.
 *
 * Returned Value:  RV_TRUE - success
 **************************************************************************************/
RvBool createMessage(
    cmTransGlobals    *transGlobals,
    cmCallQ931MsgType msgType,
    RvUint16          CRV,
    RvUint8           *callId,
    int               *node);

/**************************************************************************************
 * findHostInHash
 *
 * Purpose: To look for an existing host with the same address as the given one.
 *          If a suitable host is found, i.e. identical in address and type,
 *          we check it to be connected (the prefered state). If it's not we look for
 *          another one which is connected. If none was found we bring a suitable host
 *          that is not connected. If such a host doesn't exist either, return NULL.
 * Input:   transGlobals    - The global data of the module.
 *          remoteAddress   - The address of the looked for host.
 *          isMultiplexed   - Do we need a host which is multiplexed or not
 *          isAnnexE        - the type of the connection (TPKT or annex E).
 *
 * Output:  None.
 *
 * Returned Value:  A pointer to the found host element or NULL if not found
 **************************************************************************************/
cmTransHost *findHostInHash(IN cmTransGlobals       *transGlobals,
                            IN cmTransportAddress   *remoteAddress,
                            IN cmTransportAddress   *localAddress,
                            IN RvBool               isMultiplexed,
                            IN RvBool               isAnnexE);

/**************************************************************************************
 * transEncodeMessage
 *
 * Purpose: to get the given message encoded, either by a callback or by the cmEmEncode.
 * Input:   host            - The host on which the message is to be sent
 *          session         - The session on whose behalf the message is to be sent.
 *          transGlobals    - The global data of the transport module.
 *          pvtNode         - the message to be encoded
 *          buffer          - the buffer into which to encode
 *
 * Output:  encMsgSize - The size of the encoded message.
 *
 * Returned Value: cmTransErr - in case that the encoding failed, cmTransOK - otherwise
 **************************************************************************************/
 TRANSERR transEncodeMessage(cmTransHost    *host,
                             cmTransSession *session,
                             cmTransGlobals *transGlobals,
                             int            pvtNode,
                             RvUint8        *buffer,
                             int            *encMsgSize);

/**************************************************************************************
 * addSessionToPendingList
 *
 * Purpose: Add a session to the list of sessions that were unable to send a message
 *          due to lack of rpool resources. Such sessions should be notified when
 *          resources become available again.
 * Input:   transGlobals - The global data of the transport module.
 *          session      - The session which failed to send the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
void addSessionToPendingList(IN cmTransGlobals *transGlobals,
                             IN cmTransSession *session);

/**************************************************************************************
 * removeSessionFromPendingList
 *
 * Purpose: Remove a session from the list of sessions that were unable to send a message
 *          due to lack of rpool resources. Such sessions are removed when notified that
 *          resources become available again.
 * Input:   transGlobals - The global data of the transport module.
 *          session      - The session which failed to send the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
void removeSessionFromPendingList(IN cmTransGlobals *transGlobals,
                                  IN cmTransSession *session);


/**************************************************************************************
 * saveMessageToPool
 *
 * Purpose: This routine gets an encoded message and saves it, until it can send it.
 * Input:   transGlobals - The global data of the transport module.
 *          session      - The session which wants to send the message.
 *          host         - The host on which the message is to be sent.
 *          buffer       - The encoded message.
 *          encMsgSize   - Its size.
 *          isTunneled   -  RV_TRUE: save to the tunneled message queue,
 *                          RV_FALSE: to the sending message queue
 *          addToTop     - RV_TRUE: Add the message to the start of the queue
 *                         RV_FALSE: Add it at the end.
 *          CRV          - The CRV of the call on which behalf the message is sent
 *                         (used for annex E session ID field).
 *
 * Output:  None.
 *
 **************************************************************************************/
HRPOOLELEM saveMessageToPool(cmTransGlobals *transGlobals,
                            void            *element,
                            RvUint8         *buffer,
                            int             encMsgSize,
                            RvBool          isTunneled,
                            RvBool          addToTop,
                            RvUint16        CRV);

/**************************************************************************************
 * moveMessagesFronSessionToHost
 *
 * Purpose: This routine sets the tunneled messages on the session to be sent with the 
 *          messages on the host.
 * Input:   transGlobals - The global data of the transport module.
 *          session      - The session which wants to send the message.
 *          host         - The host on which the message is to be sent.
 * Output:  None.
 *
 **************************************************************************************/
void moveMessagesFronSessionToHost(cmTransGlobals *transGlobals,
                                   cmTransSession *session,
                                   cmTransHost    *host);

/**************************************************************************************
 * extractMessageFromPool
 *
 * Purpose: This routine removes an encoded message from the head of the host list.
 *          The messages extracted here are tunneled messages.
 * Input:   transGlobals - The global data of the transport module.
 *          element      - The host or session which wants to send the message.
 *          isTunneled   -  TRUE: remove from the tunneled message queue,
 *                          FALSE: remove from the sending message queue
 *
 * Output:  None.
 *
 * Return Value: next message.
 *
 **************************************************************************************/
HRPOOLELEM extractMessageFromPool(
                             cmTransGlobals *transGlobals,
                             void           *element,
                             RvBool         isTunneled);

/**************************************************************************************
 * processH245OutgoingMessage
 *
 * Purpose: This routine gets a decoded outgoing H.245 message and modifies it according to
 *          the different tasks at hand:
 *          - If tunneling: encodes it and puts it into the H.245 tunneled messages sylo
 *                          and inhibits its sending.
 *          - If not tunneling: do nothing.
 *
 * Input:   session     - The session which wants to send the message.
 *          host        - The host on which the message is to be sent.
 *          pvtNode     - The message.
 *
 * Output:  None.
 *
 * Returned Value: RV_TRUE - send the message, RV_FALSE - don't send it.
 *
 **************************************************************************************/
RvBool processH245OutgoingMessage(IN    cmTransSession    *session,
                                IN    cmTransHost       *host,
                                INOUT int               pvtNode);

/**************************************************************************************
 * processOutgoingMessage
 *
 * Purpose: This routine gets a decoded outgoing message and modifies it according to
 *          the different tasks at hand:
 *          - Add fields to Q.931 messages, such as multiplexing flags.
 *          - Save H.245 tunneled messages and eliminate their actual sending
 *          - Insert H.245 addresses, if necessary to the messages.
 *          - Insert H.245 tunneled messages.
 * Input:   session     - The session which wants to send the message.
 *          host        - The host on which the message is to be sent.
 *          pvtNode     - The message.
 *          type        - The type of the message (Q.931/H.245)
 *
 * Output:  None.
 *
 * Returned Value: RV_TRUE - send the message, RV_FALSE - don't send it.
 *
 **************************************************************************************/
RvBool processOutgoingMessage(IN    cmTransSession    *session,
                            IN    cmTransHost       *host,
                            INOUT int               pvtNode,
                            IN    TRANSTYPE         type);

/**************************************************************************************
 * transSessionClose
 *
 * Purpose: This routine closes a session and breaks all its associations.
 *
 * Input/Output:   hsTransSession - The session to be closed.
 *
 * Output:  None.
 *
 **************************************************************************************/
void transSessionClose(cmTransSession *session);

/**************************************************************************************
 * transHostClose
 *
 * Purpose: This routine reports a host close and tells the sessions connected to
 *          it that the connection has ended and releasing all the messages kept for it
 *          in the pool.
 *
 * Input/Output:   hsTransHost      - The host to be deleted.
 *                 killHost         - Should the host be removed from the hash and close
 *                                    its TPKT element or just disconnect from session?
 *
 * Output:  None.
 *
 **************************************************************************************/
void transHostClose(HSTRANSHOST hsTransHost, RvBool killHost);

/**************************************************************************************
 * findSessionByMessage
 *
 * Purpose: looks according to the CRV and callID of the message for an entry of the session
 *          in the host's hash table. If none was found, creates an entry.
 *          In case of non-session related messages (CRV = 0) the routine treats it
 *          as an error and expect the upper layer to handle the case.
 *
 * Input:   transGlobals    - The global data of the transport module.
 *          host            - The host on which the message arrived.
 *          pvtNode         - The decoded message.
 *          isAnnexE        - is the host on which the message came is an annex E host?
 *
 * Output:  session         - The session found or created.
 *          releaseReason   - Release complete reason to set if an error occured.
 *
 * returned value: RV_TRUE - all is OK (if session==NULL in this case, then the message
 *                           is a general one to the host and not session-specific).
 *                 RV_FALSE - No session was found/created due to a possible error.
 *                            If this is an incoming setup message, then releaseReason
 *                            holds the reason for releasing this call.
 *
 **************************************************************************************/
RvBool findSessionByMessage(
    IN  cmTransGlobals *transGlobals,
    IN  cmTransHost    *host,
    IN  RvPvtNodeId    pvtNode,
    IN  RvBool         isAnnexE,
    OUT cmTransSession **session,
    OUT RvPstFieldId   *releaseReason);

/**************************************************************************************
 * setRemoteAddress
 *
 * Purpose: sets the remote address to the host and adds an entry to the host hash table
 *
 * Input:   host            - The host to which the address is to be set.
 *          remoteAddress   - The address to be set.
 *
 * Return Value: cmTransOK - if all is well; cmTranErr - otherwise
 *
 **************************************************************************************/
TRANSERR setRemoteAddress(cmTransHost *host, cmTransportAddress *remoteAddress);

/************************************************************************************
 * canWeAcceptTheCall
 *
 * Purpose: to check if this is the first call on this host, or if this host supports
 *          multiplexing. If this host doesn't support multiplexing and already has a
 *          session attached to it, refuse the message and send releaseComplete on
 *          the call.
 *
 * Input: transGlobals - The global variables of the module
 *        host         - the host through which the message was received.
 *        pvtNode      - the decoded message
 *
 * Output: None
 *
 * returned value: RV_TRUE  - accept the message
 *                 RV_FALSE - refuse it and disconnect the call.
 *
 **************************************************************************************/
RvBool canWeAcceptTheCall(cmTransGlobals *transGlobals, cmTransHost *host, int pvtNode);

/******************************************************************************
 * encodeAndSend
 * ----------------------------------------------------------------------------
 * General: Encodes and sends a given decoded message on a given host.
 *          No checks and no processing is done on the message.
 *          Note: The node of the decoded message remains untouched.
 *
 * Return Value: RV_TRUE: message was sent, RV_FALSE: sending failed.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global variables of the module.
 *         host         - The host through which the message is to be sent.
 *         newMessage   - The decoded message to send.
 * Output: none
 *****************************************************************************/
RvBool encodeAndSend(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host,
    IN RvPvtNodeId      newMessage);


/******************************************************************************
 * sendRawMessage
 * ----------------------------------------------------------------------------
 * General: Sends a given raw buffer as a message on a given host.
 *          No checks and no processing is done on the message.
 *
 * Return Value: RV_TRUE: message was sent, RV_FALSE: sending failed.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  transGlobals - The global variables of the module.
 *         host         - The host through which the message is to be sent.
 *         buffer       - Buffer to send as the message.
 *         bufferSize   - Size of buffer to send.
 * Output: none
 *****************************************************************************/
RvBool sendRawMessage(
    IN cmTransGlobals   *transGlobals,
    IN cmTransHost      *host,
    IN RvUint8          *buffer,
    IN RvSize_t         bufferSize);


/************************************************************************************
 * sendGeneralFacility
 *
 * Purpose: creates a general purpose faclity message with CRV and callId set to zero.
 *          sets in itthe host's current multiplex parameters, encodes it and send it
 *          on the host.
 *
 * Input: transGlobals - The global variables of the module
 *        host         - the host through which the message is to be sent.
 *
 * Output: None
 *
 **************************************************************************************/
void sendGeneralFacility(cmTransGlobals *transGlobals, cmTransHost *host);

/************************************************************************************
 * initiateTunnelingFacility
 *
 * Purpose: if we are in a 'stable' state we need to initiate a facility send
 *          to get rid of the tunneled messages, and initiate it.
 *              The condition for that are:
 *              a. We have tunneled messages to send
 *              b. At least one message was already sent from this end
 *              c. Tunneling was approved
 *
 * Input: transGlobals - The global variables of the module
 *        session      - the session whose state is tested.
 *        host         - the related locked host, if at all
 *
 * Output: None
 *
 **************************************************************************************/
void initiateTunnelingFacility(cmTransGlobals *transGlobals,
                               cmTransSession *session,
                               cmTransHost    *host);

/************************************************************************************
 * sendReleaseCompleteMessage
 *
 * Purpose: initiate a send of releaseComplete in case that:
 *              a.  A call was initiated on a non multiplexed connection that
 *                  has already other call(s).
 *              b.  We ran out of resources (mainly sessions)
 *
 * Input: transGlobals - The global variables of the module
 *        host         - the host through which the message is to be sent.
 *        CRV          - the CRV to attach to the message
 *        callId       - the call identifier to attach to th emessage
 *        cause        - The call release cause
 *        reasonNameId - field Id of the disconnect reason.
 *
 * Output: None
 *
 **************************************************************************************/
void sendReleaseCompleteMessage(cmTransGlobals  *transGlobals,
                                cmTransHost     *host,
                                RvUint16        CRV,
                                RvUint8         *callId,
                                int             cause,
                                RvPstFieldId    reasonNameId);

/************************************************************************************
 * getGoodAddress
 *
 * Purpose: Calculate the ip address as follows:
 *
 *          a. Try and get the allocated ip address from the TCP/IP stack.
 *          b. if 0, try and get the Q.931 connection ip address.
 *          c. if 0, use the localIPAddress as was given in cmTransStart.
 *          d. if 0, just leave it as 0 since we might want to use different
 *             lan interface than the one of the first IP address.
 *
 *          Note: for Q.931 connections, step a is being skipped.
 *
 * Input: transGlobals - The global variables of the module.
 *        hTpkt        - The tpkt element of the socket whose address we want to find.
 *        q931         - The Q.931 connection of a given H.245 socket (relevent only when
 *                       type is cmTransH245Conn).
 *        type         - The type of the connection: Q.931, H.245 or annexE.
 *
 * Output: addr - The calculated good ip address.
 *
 **************************************************************************************/
void getGoodAddress(cmTransGlobals     *transGlobals,
                    HTPKT              hTpkt,
                    cmTransHost        *q931Host,
                    TRANSCONNTYPE      type,
                    cmTransportAddress *addr);

/************************************************************************************
 * sendStartH245Facility
 *
 * Purpose: to create and send a facility message with reason startH245.
 *
 *
 * Input: transGlobals - The global variables of the module.
 *        session      - The session on which the facility is to be sent.
 *
 * Output: None.
 *
 **************************************************************************************/
void sendStartH245Facility(cmTransGlobals *transGlobals, cmTransSession *session);

/************************************************************************************
 * transCompareAddresses
 *
 * Purpose: compare to addresses according to their type.
 *
 *
 * Input: addr1, addr2 - teh cmTransportAddress to compare.
 *
 * Output: None.
 *
 * Returned Value: RV_TRUE - the addresses are identical; RV_FALSE - Otherwise
 **************************************************************************************/
RvBool transCompareAddresses(cmTransportAddress *addr1, cmTransportAddress *addr2);

/**************************************************************************************
 * closeHostInternal
 *
 * Purpose: Deletes a host element. Will notify all its associates sessions.
 *
 * Input:   hsTranHost  - An handle to the host element
 *
 * Output:  None.
 *
 **************************************************************************************/
TRANSERR closeHostInternal(IN HSTRANSHOST hsTransHost, IN RvBool reportToUser);


#ifdef __cplusplus
}
#endif


#endif  /* _TRANSUTIL_H */
