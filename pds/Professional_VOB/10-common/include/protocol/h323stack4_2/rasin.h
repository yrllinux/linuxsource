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

#ifndef RASIN_H
#define RASIN_H


#include "cmiras.h"
#include "rasdef.h"

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * rasGarbageCollection
 * purpose: Clean the incoming transaction list from old transactions that
 *          are no more relevant to us.
 * input  : ras             - RAS module to use
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasGarbageCollection(
    IN rasModule*   ras);


/************************************************************************
 * rasIncomingHashFunc
 * purpose: Incoming transactions hash function
 *          This function returns as the hash key the actual sequence
 *          number of the transaction without fooling around with it
 * Input:   key     - Pointer the key.
 *          length  - The size of the key, in bytes.
 *          initVal - An initial 4-byte value. Can be the previous hash,
 *                    or an arbitrary value. Passed as 0 by the hash
 *                    module.
 * Return : 32bit hash result
 ************************************************************************/
RvUint32 RVCALLCONV rasIncomingHashFunc(
    IN void     *key,
    IN RvUint32 length,
    IN RvUint32 initVal);


/************************************************************************
 * rasIncomingHashCompare
 * purpose: Compare keys in the outgoing hash table
 * input  : key1, key2  - Keys to compare
 *          keySize     - Size of each key
 * return : RV_TRUE if elements are the same
 *          RV_FALSE otherwise
 ************************************************************************/
RvBool rasIncomingHashCompare(IN void *key1, IN void* key2, IN RvUint32 keySize);


/************************************************************************
 * rasHandleRequest
 * purpose: Handle an incoming request message
 * input  : ras             - RAS module to use
 *          index           - Index of connection this message arrived on
 *          isMulticast     - RV_TRUE if message was received no a multicast socket
 *          srcAddress      - Address of the sender
 *          messageBuf      - The message buffer to send
 *          messageLength   - The length of the message in bytes
 *          messageNodeId   - Node ID of message root. If negative, then
 *                            message is decoded from given buffer and hook
 *                            is called
 *          messageType     - Message type of the reply
 *          seqNum          - requestSeqNum field value of the message
 *          hsCall          - Call related to this transaction if it's an unsolicited IRR
 *                            NULL on any other case
 * output : hMsgContext     - Incoming message context. Used mostly by security
 *                            If the returned value is different than NULL,
 *                            then the message context is not used by the
 *                            transaction and should be released
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasHandleRequest(
    IN  rasModule*          ras,
    IN  RvInt               index,
    IN  RvBool              isMulticast,
    IN  cmTransportAddress  *srcAddress,
    IN  RvUint8*            messageBuf,
    IN  RvUint32            messageLength,
    IN  RvPvtNodeId         messageNodeId,
    IN  rasMessages         messageType,
    IN  RvUint32            seqNum,
    IN  HCALL               hsCall,
    OUT void**              hMsgContext);


/************************************************************************
 * rasSendConfirmMessage
 * purpose: Sends a confirm response on an incoming RAS request
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int rasSendConfirmMessage(
    IN rasModule*   ras,
    IN rasInTx*     tx);


/************************************************************************
 * rasSendRejectMessage
 * purpose: Sends a reject response on an incoming RAS request
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction
 *          reason          - The reject reason to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int rasSendRejectMessage(
    IN rasModule*   ras,
    IN rasInTx*     tx,
    IN cmRASReason  reason);


/************************************************************************
 * rasSendRIP
 * purpose: Sends a RIP response on an incoming RAS request
 * input  : ras     - RAS module to use
 *          tx      - Incoming transaction
 *          delay   - Delay for RIP message in milliseconds
 *          updateStopTime  - Indicate if we're updating the stop time
 *                            and increasing the transaction's time or not
 * output : none
 * return : If an error occurs, the function returns RV_OK.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
RvStatus rasSendRIP(
    IN rasModule*   ras,
    IN rasInTx*     tx,
    IN int          delay,
    IN RvBool       updateStopTime);


/************************************************************************
 * rasCloseInTx
 * purpose: Close an incoming transaction
 *          This won't actually free resources, only mark the transaction
 *          as a possibility for removal in the garbage collection.
 * input  : ras             - RAS module to use
 *          tx              - Incoming transaction to close
 * output : none
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasCloseInTx(
    IN rasModule*       ras,
    IN rasInTx*         tx);

//////////////////////////////////////////////////////////////////////////
//jacky Bug00151888
/************************************************************************
 * rasCloseAllIncTxs
 * purpose: Clean the incoming transaction list from old transactions that
 *          are no more relevant to us.
 * input  : ras             - RAS module to use
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasCloseAllIncTxs(IN rasModule* ras);
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif  /* RASIN_H */

