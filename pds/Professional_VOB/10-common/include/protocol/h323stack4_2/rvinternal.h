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


/************************************************************************
 *
 * rvinternal.h
 *
 *
 * Common definitions for RADVISION project
 * This file should be included by RADVISION code, and should not be used
 * by customer's applications.
 *
 ************************************************************************/

#ifndef __RVINTERNAL_H
#define __RVINTERNAL_H

#include "rvcommon.h"
#include "rv_defs.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Protocol Identifiers Used */
#define H225_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 2250 0 4"
/*#define H245_PROTOCOL_IDENTIFIER - defined in H.245 */


/************************************************************************
 * nprn  - Makes sure a string is printed and not core dumps on NULL strings
 * nprnd - Returns the value inside a pointer or an error if pointer is NULL
 ************************************************************************/
#ifndef nprn
#define nprn(s) ((s)?(s):"(null)")
#define nprnd(d) ((d)?(*d):-1)
#endif



/* Make sure internal files of the stack are always compiled with the latest
   definitions */
#define RV_H323_VERSION_DEFINITIONS 420

  


#ifdef __cplusplus
}
#endif

#endif  /* __RVINTERNAL_H */

