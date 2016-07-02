/***********************************************************************
Filename   : rvsymsock.h
Description: host related functions support for Symbian
************************************************************************
        Copyright (c) 2001 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/

#if ((RV_NET_TYPE != RV_NET_NONE) && (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN))

#if defined(__cplusplus)
extern "C" {
#endif

void RvSymSelectInit(void);

RvStatus RvSymSelectConstruct(
    IN RvSelectEngine*      selectEngine);

RvStatus RvSymSelectDestruct(
    IN RvSelectEngine*      selectEngine);    
RvStatus RvSymSelectAdd(
    IN RvSelectEngine*      selectEngine,
    IN RvSelectFd*          fd,
    IN RvSelectEvents       selectEvents);

RvStatus RvSymSelectUpdate(
    IN RvSelectEngine*      selectEngine,
    IN RvSelectFd*          fd,
    IN RvSelectEvents       selectEvents);

RvStatus RvSymSelectSetTimeOut(
   	IN RvSelectEngine*      selectEngine,
	IN RvUint64				nsecTimeout);

RvStatus RvSymSelectWaitAndBlock(
    IN RvSelectEngine*      selectEngine,
    IN RvUint64             nsecTimeout);

RvStatus RvSymSelectStopWaiting(
    IN RvSelectEngine*      selectEngine,
	IN RvUint8				message);

#if defined(__cplusplus)
}
#endif


#endif /* (RV_NET_TYPE != RV_NET_NONE) */
