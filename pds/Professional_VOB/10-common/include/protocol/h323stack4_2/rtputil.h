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

#ifndef __RTPUTIL_H
#define __RTPUTIL_H


#include "rvselect.h"
#include "rvtimer.h"
#include "rtp.h"

#ifdef __cplusplus
extern "C" {
#endif


/* This struct is here because some applications like to include it in their own code and access
   it directly (bad, but what can we do?) */
typedef struct
{
    RvBool              isAllocated; /* RV_TRUE if the RTP package allocated this struct internally */
    RvSocket            socket; /* UDP socket used for this session */
    RvSelectEngine      *selectEngine; /* Select engine used by this fd */
    RvSelectFd          selectFd; /* fd object to use for non-blocking mode */
    RvUint32            sSrc;
    RvUint32            sSrcMask;
    RvUint32            sSrcPattern;
    LPRTPEVENTHANDLER   eventHandler;
    void *              context;
    RvUint16            sequenceNumber;
    RvAddress           remoteAddress; /* Remote address of this session */
    RvBool              remoteAddressSet; /* RV_TRUE if we already set the remote address */
    RvBool              useSequenceNumber;
    HRTCPSESSION        hRTCP; /* Associate RTCP session if available */
} rtpSession;       /* HRTPSESSION */





void ConvertToNetwork(void *data, int pos, int n);
void ConvertFromNetwork(void *data, int pos, int n);

#ifdef __cplusplus
}
#endif

#endif  /* __RTPUTIL_H */
