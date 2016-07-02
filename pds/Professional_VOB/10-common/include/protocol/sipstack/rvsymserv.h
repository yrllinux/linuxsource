/***********************************************************************
Filename   : rvsymserv.h
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

#ifndef __RVSYMSERVER_H__
#define __RVSYMSERVER_H__

#if (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN)

#ifdef __cplusplus
extern "C" {
#endif

#include "rvselect.h"


RvStatus RvSymServerInit(void);
RvStatus RvSymServerEnd(void);

RvSelectEngine* RvSymbianGetTheSelectEngine(void);

#ifdef __cplusplus
}
#endif

#endif /* (RV_NET_TYPE != RV_NET_NONE) */
#endif /* __RVSYMSERVER_H__ */
