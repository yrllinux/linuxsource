/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/

#ifndef _RV_RTP_BUFFER_H
#define _RV_RTP_BUFFER_H

#include "rvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    RvUint32 length;
    RvUint8*  buffer;
} RvRtpBuffer;


RvBool buffAddToBuffer(RvRtpBuffer* to, RvRtpBuffer* from, RvUint32 offset);
RvBool buffValid(RvRtpBuffer* buff,RvUint32 size);
RvRtpBuffer buffCreate(void* data,int size);


#ifdef __cplusplus
}
#endif

#endif /* _RV_RTP_BUFFER_H */




