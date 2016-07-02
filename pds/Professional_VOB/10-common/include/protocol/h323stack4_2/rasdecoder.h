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

#ifndef RASDECODER_H
#define RASDECODER_H


#include "cmiras.h"
#include "rasdef.h"

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * rasDecoderInit
 * purpose: Initialize the decoder. Checks out the information inside
 *          the PST tree to allow faster runtime decoding of the first
 *          2 fields.
 * input  : ras             - RAS instance to use
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasDecoderInit(IN  rasModule*  ras);


/************************************************************************
 * rasDecoderEnd
 * purpose: Deinitialize the decoder.
 * input  : ras             - RAS instance to use
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int rasDecoderEnd(IN  rasModule*  ras);






/************************************************************************
 * rasDecodePart
 * purpose: Decode the first 2 fields of an incoming message. This way
 *          We know the message's type and its sequence number.
 * input  : ras             - RAS instance to use
 *          messageBuffer   - The encoded message to check
 *          messageSize     - The size of the message in bytes
 * output : index           - Index of the RAS message
 *          requestSeqNum   - Request sequence number of the message
 * return : RV_OK on success
 *          Negative value on failure
 ************************************************************************/
RvStatus rasDecodePart(
    IN  rasModule*  ras,
    IN  RvUint8*    messageBuffer,
    IN  RvUint32    messageSize,
    OUT RvUint32*   index,
    OUT RvUint32*   requestSeqNum);





#ifdef __cplusplus
}
#endif

#endif  /* RASDECODER_H */

