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

#ifndef __BITFIELD_H
#define __BITFIELD_H

#include "rvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

RvUint32 bitfieldSet(
    IN  RvUint32    value,
    IN  RvUint32    bitfield,
    IN  int         nStartBit,
    IN  int         nBits);

RvUint32 bitfieldGet(
    IN  RvUint32    value,
    IN  int         nStartBit,
    IN  int         nBits);

#ifdef __cplusplus
}
#endif

#endif /* __BITFIELD_H */

