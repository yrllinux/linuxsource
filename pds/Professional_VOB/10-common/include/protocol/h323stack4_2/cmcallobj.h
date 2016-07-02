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

#ifndef _CM_CALLOBJ_H
#define _CM_CALLOBJ_H

#include "rvinternal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { /* call source */
  Incoming = 0x4,
  Outgoing = 0x8

} callSrcEnum;


/************************************************************************
 * callObjectCreate
 * purpose: Create a CAT struct for a given call by the call's object.
 * input  : app     - Stack's instance
 *          hsCall  - Stack's handle of the call
 *          call    - Type of call - Incoming or Outgoing
 * output : callObj - CAT struct for the call
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
int
callObjectCreate(
          IN  cmElem        *app,
          IN  HCALL         hsCall,
          IN  callSrcEnum   call,
          OUT catStruct     *callObj);


int
callObjectCreateFromMessage(
          IN  cmElem *app,
          IN  int message,
          OUT catStruct *callObj);


#ifdef __cplusplus
}
#endif

#endif  /* _CM_CALLOBJ_H */
