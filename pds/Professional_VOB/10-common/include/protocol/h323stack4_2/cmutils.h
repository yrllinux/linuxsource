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

#ifndef _RV_CMUTILS_H
#define _RV_CMUTILS_H

#include "rvlog.h"
#include "cmintr.h"
#include "cm.h"

#ifdef __cplusplus
extern "C" {
#endif

int setNonStandard(
        IN  HPVT  hVal,
        IN  int   nodeId,
        IN  cmNonStandard*nonStandard
);
int setNonStandardParam(
        IN  HPVT  hVal,
        IN  int   vNodeId,
        IN  cmNonStandardParam*nonStandard
);

int getNonStandard(
        IN  HPVT  hVal,
        IN  int   nodeId,
        OUT cmNonStandard*nonStandard
);

int getNonStandardParam(
        IN  HPVT  hVal,
        IN  int   vNodeId,
        OUT cmNonStandardParam*nonStandard
);

RvInt32 getEnumNameId(
          IN    fieldNames* enumFieldNames,
          IN    int     enumFieldNamesSize,
          IN    unsigned    enumValue
);

int getEnumValueByNameId(
                    IN  fieldNames* enumFieldNames,
            IN  RvInt32     nameId
);

int aliasToVt(
          IN  HPVT    hVal,
          IN  cmAlias*alias,
          IN  int     nodeId);

RvStatus vtToAlias(
    IN  HPVT        hVal,
    IN  cmAlias     *alias,
    IN  RvPvtNodeId nodeId,
    IN  RvBool      bCheckLength);


RvStatus cmVtToTA(HPVT hVal,int nodeId, cmTransportAddress* ta);
RvStatus cmVtToLTA(HPVT hVal,int nodeId, cmTransportAddress* ta);

int cmTAToVt(HPVT hVal,int nodeId, cmTransportAddress* ta);
int cmLTAToVt(HPVT hVal,int nodeId, cmTransportAddress* ta);


/**************************************************************************************
 * RvH323CoreToCmAddress
 *
 * Purpose: Convert a core address into a CM address.
 *
 * Input:   coreAddress - Core address to convert
 * Output:  cmAddress   - CM address to create
 * Returns: RV_OK on success, other on failure
 **************************************************************************************/
RvStatus RvH323CoreToCmAddress(
    IN  RvAddress*          coreAddress,
    OUT cmTransportAddress* cmAddress);


/**************************************************************************************
 * RvH323CmToCoreAddress
 *
 * Purpose: Convert a CM address into a core address.
 *
 * Input:   cmAddress   - CM address to create
 * Output:  coreAddress - Core adress to convert. This address must be destructed
 *                        by the caller to this function
 * Returns: RV_OK on success, other on failure
 **************************************************************************************/
RvStatus RvH323CmToCoreAddress(
    IN  cmTransportAddress* cmAddress,
    OUT RvAddress*          coreAddress);


/**************************************************************************************
 * getIP
 *
 * Purpose: Extract an IP string from a CM address.
 *
 * Input:   cmAddress   - CM address to create
 *          buff        - buffer for the address
 * Output:  none
 * Returns: String of the IP on success, empty string on failure. string is static and
 *          is not to be used accept for printing.
 **************************************************************************************/
char * getIP(IN cmTransportAddress* cmAddress, OUT RvChar buff[RV_ADDRESS_MAXSTRINGSIZE]);

/**************************************************************************************
 * getIPLength
 *
 * Purpose: GetThe length needed for the IP address.
 *
 * Input:   cmAddress   - CM address to create
 * Output:  none
 * Returns: Length needed for the IP address.
 **************************************************************************************/
int getIPLength(IN cmTransportAddress* cmAddress);

/**************************************************************************************
 * condenseIpTo32bit
 *
 * Purpose: Turn an IP to a 32bit key for use in hashing.
 *
 * Input:   cmAddress   - CM address containing the IP
 * Output:  none
 * Returns: 32bits to be used for hashing
 **************************************************************************************/
RvUint32 condenseIpTo32bit(IN cmTransportAddress* cmAddress);

/************************************************************************
 * addrIntoHashKey
 * purpose: Calculate a hash value for an address. This function can
 *          be used as part of a hash function, but not as the hash
 *          function itself.
 * input  : addr    - Address to hash
 *          hashKey - Starting hash value
 * output : none
 * return : Calculated hash value
 ************************************************************************/
int addrIntoHashKey(
    IN cmTransportAddress*  addr,
    IN RvUint32             hashKey);

/**************************************************************************************
 * isSameAddress
 *
 * Purpose: returnes true if the addresses are the same.
 *
 * Input:   addr1   - CM address containing the first address
 *          addr2   - CM address containing the second address
 * Output:  none
 * Returns: true if equal, false otherwise.
 **************************************************************************************/
RvBool isSameAddress(IN cmTransportAddress* addr1, IN cmTransportAddress* addr2);

/**************************************************************************************
 * cmTaHasIp
 *
 * Purpose: Verify that a transport address has a non-zero IP.
 *
 * Input:   cmAddress   - CM address to create
 * Output:  none
 * Returns: RV_TRUE if address has nonzero IP, RV_FALSE if it does not.
 **************************************************************************************/
RvBool cmTaHasIp(IN cmTransportAddress* ta);


/**************************************************************************************
 * RvH323CmPrintMessage
 *
 * Purpose: Print an incoming or an outgoing message to the logfile.
 *
 * Input:   logSource           - Log source to print into
 *          messageStr          - String to use before printing the message
 *          hVal                - Value tree to use
 *          messageNodeId       - Message root node to print. Shout be set to -1 if decoding
 *                                of the buffer failed for incoming messages.
 *          messageBuffer       - Message buffer to print
 *          messageBufferSize   - Size of the message buffer. Should be set to -1 if encoding
 *                                of the nodeId failed for outgoing messages.
 *          isIncoming          - RV_TRUE if the message is incoming, RV_FALSE for outgoing
 * Output:  None
 * Returns: None
 **************************************************************************************/
#if (RV_LOGMASK & (RV_LOGLEVEL_DEBUG | RV_LOGLEVEL_ERROR))
void RvH323CmPrintMessage(
    IN RvLogSource*     logSource,
    IN const RvChar*    messageStr,
    IN HPVT             hVal,
    IN RvPvtNodeId      messageNodeId,
    IN RvUint8*         messageBuffer,
    IN int              messageBufferSize,
    IN RvBool           isIncoming);
#else
#define RvH323CmPrintMessage(_log, _msgStr, _hVal, _msgNodeId, _msgBuf, _msgBufSize, _isIncoming)
#endif



/**************************************************************************************
 * encodeDecodeBufferInit
 *
 * Purpose: To initialize the buffer for coding purposes.
 *
 * Input:   none
 *
 * Output:  none
 *
 **************************************************************************************/
void encodeDecodeBufferInit(void);

/**************************************************************************************
 * getEncodeDecodeBuffer
 *
 * Purpose: To get the buffer for coding purposes.
 *
 * Input:   bufferSize   - The size of buffer required.
 *          buffer       - A pointer to the buffer.
 *
 *
 * Output:  buffer       - A pointer to the buffer .
 *
 **************************************************************************************/
void getEncodeDecodeBuffer(IN int bufferSize, OUT RvUint8 **buffer);

/**************************************************************************************
 * encodeDecodeBufferEnd
 *
 * Purpose: close down the local storage.
 *
 * Input:   none.
 * Output:  none.
 *
 **************************************************************************************/
void encodeDecodeBufferEnd(void);

/**************************************************************************************
 * cleanMessage
 *
 * Purpose: Deletes all tunneling elements (H.245, H.450, annex M, annex L) from the
 *          given message. This is to avoid resending tunneled messages when reusing
 *          messages from the cm Property tree.
 *
 * Input:   hPvt - handle to the PVT of the message
 *          message  - A PVT node to the message.
 *
 * Output:  None.
 *
 **************************************************************************************/
void cleanMessage(IN HPVT hPvt, IN int message);


/******************************************************************************
 * RvH245TimerStartEv
 * ----------------------------------------------------------------------------
 * General: Start a timer for the use of H.245.
 *
 * Return Value: Timer started on success
 *               NULL on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCtrl        - Control object to start a timer for.
 *         eventHandler - Callback function on timeout.
 *         context      - Context for the callback function.
 *         millisec     - Length of the timer in milliseconds.
 * Output: none.
 *****************************************************************************/
RvTimer* RvH245TimerStartEv(
    IN HCONTROL     hCtrl,
    IN RvTimerFunc  eventHandler,
    IN void*        context,
    IN RvInt32      millisec);


/******************************************************************************
 * RvH245TimerCancelEv
 * ----------------------------------------------------------------------------
 * General: Cancel a timer that wasn't timed-out yet.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCtrl        - Control object that is associated with the timer.
 *         timer        - Pointer to the timer to cancel.
 * Output: timer        - NULL timer pointer for easy handling.
 *****************************************************************************/
RvStatus RvH245TimerCancelEv(
    IN    HCONTROL  hCtrl,
    INOUT RvTimer   **timer);


/******************************************************************************
 * RvH245TimerClearEv
 * ----------------------------------------------------------------------------
 * General: Clear a timer that has timed-out.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hCtrl        - Control object that is associated with the timer.
 *         timer        - Pointer to the timer to clear.
 * Output: timer        - NULL timer pointer for easy handling.
 *****************************************************************************/
RvStatus RvH245TimerClearEv(
    IN    HCONTROL  hCtrl,
    INOUT RvTimer   **timer);

#ifdef __cplusplus
}
#endif

#endif  /* _RV_CMUTILS_H */


