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

#ifndef _CMIQ931_
#define _CMIQ931_


#include "cmQ931.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    RvTimer *   timer;
    RvTimer *   timerSE; /* StatusEnquiry timer object of the call */
    callStateE  callState;
    int         t301;
    int         t302;
    int         t303;
    int         t304;
    int         t310;
    int         t322; /* Timeout in milliseconds on StatusEnquiry */
} q931Elem;

#ifdef __cplusplus
}
#endif


#endif /* _CMIQ931_ */
