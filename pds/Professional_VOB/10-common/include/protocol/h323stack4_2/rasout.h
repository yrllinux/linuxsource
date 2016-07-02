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

#ifndef RASOUT_H
#define RASOUT_H

#include "cmiras.h"
#include "rasdef.h"

#ifdef __cplusplus
extern "C" {
#endif




/************************************************************************
 * rasOutgoingHashFunc
 * purpose: Outgoing transactions hash function
 *          This function returns as the hash key the actual sequence
 *          number of the transaction without fooling around with it
 * Input:   key     - Pointer the key.
 *          length  - The size of the key, in bytes.
 *          initVal - An initial 4-byte value. Can be the previous hash,
 *                    or an arbitrary value. Passed as 0 by the hash
 *                    module.
 * Return : 32bit hash result
 ************************************************************************/
RvUint32 RVCALLCONV rasOutgoingHashFunc(
    IN void     *key,
    IN RvUint32 length,
    IN RvUint32 initVal);


/************************************************************************
 * rasCreateOutTx
 * purpose: Create an outgoing transaction and return its pointer
 * input  : ras         - RAS module to use
 *          haRas       - Application's handle for the RAS transaction
 *          transaction - The transaction type we want to start
 *          destAddress - Address of the destination.
 *                        If set to NULL, then it's for the gatekeeper
 * output : none
 * return : Pointer to an outgoing RAS transaction on success
 *          NULL on failure
 ************************************************************************/
rasOutTx* rasCreateOutTx(
    IN rasModule            *ras,
    IN HAPPRAS              haRas,
    IN cmRASTransaction     transaction,
    IN cmTransportAddress   *destAddress);


/************************************************************************
 * rasSendRequestMessage
 * purpose: Send an outgoing request message
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasSendRequestMessage(
    IN rasModule*       ras,
    IN rasOutTx*        tx);


/************************************************************************
 * rasDummyRequest
 * purpose: Handle incoming unsolicited IRRs as responses
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction to send
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasDummyRequest(
    IN rasModule*       ras,
    IN rasOutTx*        tx);


/************************************************************************
 * rasHandleReply
 * purpose: Handle a reply of an outgoing request message
 * input  : ras             - RAS module to use
 *          index           - Index of the local RAS address message was received from
 *          srcAddress      - Address of the sender
 *          messageBuf      - The message buffer to send
 *          messageLength   - The length of the message in bytes
 *          messageNodeId   - Node ID of message root. If negative, then
 *                            message is decoded from given buffer and hook
 *                            is called
 *          messageType     - Message type of the reply
 *          seqNum          - requestSeqNum field value of the message
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasHandleReply(
    IN  rasModule           *ras,
    IN  RvInt               index,
    IN  cmTransportAddress  *srcAddress,
    IN  RvUint8             *messageBuf,
    IN  RvUint32            messageLength,
    IN  RvPvtNodeId         messageNodeId,
    IN  rasMessages         messageType,
    IN  RvUint32            seqNum,
    OUT void                **hMsgContext);


/************************************************************************
 * rasHandleTxResponse
 * purpose: Handle a reply of an outgoing request message, when we already
 *          know the exact transaction. This function is used internally
 *          by rasHandleReply() and when we have an unsolicited IRR on
 *          a dummy request.
 *          This function doesn't lock the transaction when handling it.
 *          It assumes that the transaction is already locked from somewhere
 *          else. It will unlock the transaction when finished.
 * input  : ras             - RAS module to use
 *          tx              - Transaction we're dealing with
 *          messageNodeId   - Node ID of message root. If negative, then
 *                            message is decoded from given buffer and hook
 *                            is called
 *          messageType     - Message type of the reply
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasHandleTxResponse(
    IN  rasModule*      ras,
    IN  rasOutTx*       tx,
    IN  RvPvtNodeId     messageNodeId,
    IN  rasMessages     messageType,
    OUT void**          hMsgContext);


/************************************************************************
 * rasCloseOutTx
 * purpose: Close an outgoing transaction
 * input  : ras             - RAS module to use
 *          tx              - Outgoing transaction to close
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasCloseOutTx(
    IN rasModule*       ras,
    IN rasOutTx*        tx);

/************************************************************************
 * DeleteAllOutTx
 * purpose: Definition of a general function on an EMA element
            Delete all ras out tx while rasOutTx array full.--zhhe,2006.10.30
 * input  : elem    - Element to function on
 *          param   - Context to use for it
 * output : none
 * return : Pointer for the context to use on the next call to this
 *          EMAFunc.
 ************************************************************************/
void* rasDeleteOutTx(IN EMAElement elem, IN void *param);


#ifdef __cplusplus
}
#endif

#endif  /* RASOUT_H */

