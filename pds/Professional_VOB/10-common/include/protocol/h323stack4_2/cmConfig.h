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

#ifndef _CMCONFIG_H
#define _CMCONFIG_H

#include "cmintr.h"
#include "ci.h"
#include "pvaltree.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * cmiCreateLog
 * purpose: Create a log manager for use by a stack instance
 *          This function reads the information from a configuration
 *          manager and initializes the log manager with it.
 * input  : app     - Stack's application handle
 * output : none
 * return : none
 ************************************************************************/
void cmiCreateLog(IN cmElem* app);


/************************************************************************
 * cmiDestroyLog
 * purpose: Destroy a log manager and the CM's sources
 * input  : app     - Stack's application handle
 * output : none
 * return : none
 ************************************************************************/
void cmiDestroyLog(IN cmElem* app);


/************************************************************************
 * pvtLoadFromConfig
 * purpose: Convert information from a configuration instance (HCFG) to
 *          a value tree of a given syntax.
 * input  : hCfg        - Configuration instance handle
 *          hVal        - Value tree handle
 *          name        - Name of root from configuration to convert
 *          rootNodeId  - Syntax of configuration
 *          logErrors   - RV_TRUE if we're interested in errors while
 *                        converting the configuration
 *          log         - Log handle to use for debug messages
 * output : none
 * return : Root ID of configuration created on success
 *          Negative value on failure
 ************************************************************************/
int pvtLoadFromConfig(
    IN HCFG             hCfg,
    IN HPVT             hVal,
    IN const RvChar     *name,
    IN RvPvtNodeId      rootNodeId,
    IN RvBool           logErrors,
    IN RvLogSource      *log);


#ifdef __cplusplus
}
#endif

#endif  /* _CMCONFIG_H */

