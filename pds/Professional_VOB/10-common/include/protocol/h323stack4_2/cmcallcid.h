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

#ifndef _CM_CALLCID_H
#define _CM_CALLCID_H

#include "rvlog.h"
#include "rvrandomgenerator.h"
#include "rvinternal.h"

#ifdef __cplusplus
extern "C" {
#endif


RV_DECLARE_HANDLE(RvH323UniqueIdGeneratorHandle);


/************************************************************************
 * RvH323UniqueIdGeneratorConstruct
 * purpose: Create a globally unique identifier generator. This is needed
 *          to create call identifiers and conference identifiers in H.323
 *          calls.
 * input  : randomGenerator - Random numbers generator
 *          logMgr          - Log manager to use
 * output : none
 * return : pointer to GUID generator on success, NULL on failure
 ************************************************************************/
RvH323UniqueIdGeneratorHandle RvH323UniqueIdGeneratorConstruct(
    IN RvRandomGenerator    *randomGenerator,
    IN RvLogMgr             *logMgr);


/************************************************************************
 * RvH323UniqueIdGeneratorDestruct
 * purpose: Delete a globally unique identifier generator.
 * input  : uidGenerator    - GUID generator
 *          logMgr          - Log manager to use
 * output : none
 * return : RV_OK on success, other on failure
 ************************************************************************/
RvStatus RvH323UniqueIdGeneratorDestruct(
    IN RvH323UniqueIdGeneratorHandle uidGenerator,
    IN RvLogMgr                     *logMgr);


/************************************************************************
 * RvH323UniqueIdGeneratorGet
 * purpose: Delete a globally unique identifier generator.
 * input  : uidGenerator    - GUID generator
 * output : guid            - The generated globally unique ID.
 *                            This string must be with at least 16 characters
 * return : RV_OK on success, other on failure
 ************************************************************************/
RvStatus RvH323UniqueIdGeneratorGet(
    IN  RvH323UniqueIdGeneratorHandle   uidGenerator,
    OUT RvUint8*                        guid);


#ifdef __cplusplus
}
#endif

#endif  /* _CM_CALLCID_H */

