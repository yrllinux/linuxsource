/***********************************************************************
        Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#ifndef RASUTILS_H
#define RASUTILS_H


#include "rvlog.h"
#include "cmiras.h"
#include "rasdef.h"

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * rasGetOutgoing
 * purpose: Get the outgoing RAS transaction from its handle
 * input  : hsRas   - Application handle
 * output : none
 * return : RAS outgoing transaction on success
 *          NULL on failure
 ************************************************************************/
#ifdef RV_RAS_DEBUG
rasOutTx* rasGetOutgoing(IN HRAS hsRas);
#else
#define rasGetOutgoing(hsRas)   ((rasOutTx*)hsRas)
#endif


/************************************************************************
 * rasGetIncoming
 * purpose: Get the incoming RAS transaction from its handle
 * input  : hsRas   - Application handle
 * output : none
 * return : RAS incoming transaction on success
 *          NULL on failure
 ************************************************************************/
#ifdef RV_RAS_DEBUG
rasInTx* rasGetIncoming(IN HRAS hsRas);
#else
#define rasGetIncoming(hsRas)   ((rasInTx*)hsRas)
#endif


/************************************************************************
 * rasGetParamName
 * purpose: Get the parameter's name
 * input  : param   - Parameter enumeration value
 * output : none
 * return : Parameter's name on success
 *          Empty string on failure
 ************************************************************************/
#if (RV_LOGMASK & RV_LOGLEVEL_ENTER)
const char *rasGetParamName(IN cmRASParam param);
#else
#define rasGetParamName(_param) ""
#endif


/************************************************************************
 * rasCfgGetTimeout
 * purpose: Get the timeout of a transaction before a retry is sent from
 *          the configuration
 * input  : ras     - RAS instance handle
 * output : none
 * return : Timeout in seconds
 ************************************************************************/
RvInt16 rasCfgGetTimeout(IN rasModule* ras);


/************************************************************************
 * rasCfgGetRetries
 * purpose: Get the number of retries for a transaction before a timeout
 *          from the configuration
 * input  : ras     - RAS instance handle
 * output : none
 * return : Number of retries
 ************************************************************************/
RvInt16 rasCfgGetRetries(IN rasModule* ras);


/************************************************************************
 * rasRetransmit
 * purpose: Retransmit a message to the other side
 *          This function is used for both requests and responses.
 * input  : ras         - RAS instance handle
 *          rasTx       - RAS transaction of the message
 *          rpootMsg    - RPOOL message handle to retransmit
 *          index       - Index of connection to use
 *          destAddr    - Destination address
 *          typeStr     - Type of retransmission
 * output : none
 * return : RV_OK value on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasRetransmit(
    IN  rasModule*          ras,
    IN  HRAS                rasTx,
    IN  HRPOOLELEM          rpoolMsg,
    IN  RvInt               index,
    IN  cmTransportAddress* destAddr,
    IN  const RvChar*       typeStr);


/************************************************************************
 * rasEncodeAndSend
 * purpose: Encode and send a message on the net
 * input  : ras         - RAS instance handle
 *          rasTx       - RAS transaction of the message
 *          stage       - Stage of the RAS transaction
 *          nodeId      - nodeId of the message to send out
 *          index       - Index of connection to use for sending the message
 *          destAddr    - Destination address
 *          storeInRPOOL- Indicate if we want the message to be stored in RPOOL
 * output : rpoolHandle - RPOOL handle storing the message
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasEncodeAndSend(
    IN  rasModule*          ras,
    IN  HRAS                rasTx,
    IN  cmRASTrStage        stage,
    IN  RvPvtNodeId         nodeId,
    IN  RvInt               index,
    IN  cmTransportAddress* destAddr,
    IN  RvBool              storeInRPOOL,
    OUT HRPOOLELEM*         rpoolHandle);


/************************************************************************
 * rasDecodeAndRecv
 * purpose: Decode and receive a message from the net
 *          This function is called after we already know if its an incoming
 *          or outgoing message.
 *          It returns as parameters the sequence number and message type,
 *          allowing the caller to know if these parameters were changed in
 *          the hook function to the application
 * input  : ras             - RAS instance handle
 *          index           - Index of the local RAS address message was received from
 *          messageBuf      - Message buffer
 *          messageLength   - Length of received message
 *          isMulticast     - Are we sending it to a multicast address
 *          srcAddr         - Source address
 * output : srcAddr         - Reply address if found inside the message
 *          nodeId          - Root where we placed the message
 *          messageType     - Message type after hook
 *          requestSeqNum   - Sequence number in decoded message after hook
 * return : RV_TRUE if message should be processed
 *          RV_FALSE if message shouldn't be processed
 *          Negative value on failure
 ************************************************************************/
int rasDecodeAndRecv(
    IN     rasModule*          ras,
    IN     RvInt               index,
    IN     RvUint8*            messageBuf,
    IN     RvUint32            messageLength,
    IN     RvBool              isMulticast,
    INOUT  cmTransportAddress* srcAddr,
    OUT    RvPvtNodeId*        nodeId,
    OUT    rasMessages*        messageType,
    OUT    RvUint32*           requestSeqNum,
    OUT    void**              hMsgContext);


/************************************************************************
 * rasRouteMessage
 * purpose: Route the message to the right transaction, making sure if
 *          it's incoming, outgoing or IRR.
 * input  : ras             - RAS instance handle
 *          srcAddr         - Source address
 *          index           - Index of the connection the message was
 *                            received to
 *          isMulticast     - RV_TRUE if message was received on a multicast
 *                            connection
 *          messageBuf      - Message buffer
 *          messageLength   - Length of received message
 *          messageNodeId   - Node ID of the message.
 *                            If this value is negative, the message is
 *                            decoded and checked, otherwise, the decoded
 *                            message will be processed without calling the
 *                            hook functions.
 *          messageType     - Message type after hook
 *          requestSeqNum   - Sequence number in decoded message after hook
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasRouteMessage(
    IN  rasModule*          ras,
    IN  RvInt               index,
    IN  RvBool              isMulticast,
    IN  cmTransportAddress* srcAddr,
    IN  RvUint8*            messageBuf,
    IN  RvUint32            messageLength,
    IN  int                 messageNodeId,
    IN  rasMessages         messageType,
    IN  RvUint32            requestSeqNum,
    OUT void**              hMsgContext);


/************************************************************************
 * rasCreateCatKey
 * purpose: Create the key struct for CAT from an incoming message
 *          transaction.
 * input  : ras     - RAS instance handle
 *          tx      - Incoming transaction to use
 * output : catKey  - Filled CAT key struct for this transaction
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasCreateCatKey(
    IN  rasModule*  ras,
    IN  rasInTx*    tx,
    OUT catStruct*  catKey);


/************************************************************************
 * rasUpdateRegInfo
 * purpose: Update the registration information of our RAS configuration
 *          from an incoming RCF message
 * input  : ras             - RAS instance handle
 *          messageNodeId   - Incoming message that caused this call
 *                            For unregistration, this value will be negative
 * output : none
 * return : none
 ************************************************************************/
void rasUpdateRegInfo(
    IN  rasModule*  ras,
    IN  int         messageNodeId);

/************************************************************************
 * rasUpdatePartUnreg
 * purpose: Update the registration information of our RAS configuration
 *          from an outgoing partial URQ message.
 * input  : ras             - RAS instance handle
 *          messageNodeId   - Outgoing message whose approval caused this
 *                            function to be called.
 * output : none
 * return : none
 ************************************************************************/
void rasUpdatePartUnreg(
    IN  rasModule*  ras,
    IN  int         messageNodeId);


/************************************************************************
 * rasUpdateGkRasAddress
 * purpose: Update the gatekeeperRASAddress (needed when using AltGK).
 * input  : ras             - RAS instance handle
 *          addrNodeId      - Node with the address.
 * output : none
 * return : none
 ************************************************************************/
void rasUpdateGkRasAddress(
    IN  rasModule*      ras,
    IN  RvPvtNodeId     addrNodeId,
    IN  char            gkId[256],
    IN  int             gkIdLen);



#ifdef __cplusplus
}
#endif

#endif  /* RASUTILS_H */

