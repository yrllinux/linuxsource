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

#ifndef CATDB_H
#define CATDB_H

#ifdef __cplusplus
extern "C" {
#endif


#include "rvlock.h"
#include "cat.h"



/* Number of supported key types in CAT */
#define CAT_KEY_TYPES (5)




/************************************************************************
 * catKeyStruct
 * This struct holds the keys that we use for the hash table itself
 * The information stored here is minimal - we want to make sure that
 * the hash table is kept small.
 ************************************************************************/
typedef struct
{
    RvUint32    keyValue; /* The key enumeration we're dealing with
                             This is the enumeration value from keyTypes[] array of the
                             catModule struct. */
    catStruct*  key; /* The actual key information */
} catKeyStruct;




/************************************************************************
 * catDataStruct
 * This struct holds the information associated with a call inside CAT.
 * It holds the handle of the call and the IRR transactions for that
 * call.
 ************************************************************************/
typedef struct
{
    catStruct       key; /* Keys information used to find the call */
    void*           hashValues[5]; /* Hash locations of all keys handled by this call */

    /* User related information */
    HCALL           hsCall; /* Call handle for the call */
    HRAS            unsolicitedIRR; /* Handle to an unsolicited IRR message for this call.
                                       This handle is created by cmRASDummyRequest()
                                       for backward compatibility. It is recommended
                                       not to use such calls. */
} catDataStruct;




/************************************************************************
 * catModule
 * This struct holds the CAT instance information.
 ************************************************************************/
typedef struct
{
    RvLogMgr*       logMgr; /* Log manager to use */
    RvLogSource     log; /* Log instance used for logging messages */
    RvUint32        keyTypes[CAT_KEY_TYPES]; /* Key types supported by this CAT instance (we've got
                                                different types for CID association or srcAddress
                                                association on Version 1 incoming calls. */
    RvLock          lock; /* Mutex to use */
    HHASH           hash; /* Hash table used by CAT */
    HRA             calls; /* RA holding call information and the call's keys */
    int             numSimultKeys; /* Number of simultaneous keys */
    RvBool          compare15bitCrv; /* Use 15bit comparison on CRV instead of 16bit */
} catModule;



#ifdef __cplusplus
}
#endif

#endif  /* CATDB_H */

