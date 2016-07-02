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

/*
  pvaltreeStackApi

  This file contains functions which are available for the use of the stack modules,
  but are not part of the API provided to the users

  */

#ifndef _PVALTREE_STACKAPI_H
#define _PVALTREE_STACKAPI_H

#include "pvaltree.h"
#include "pvaltreeDef.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * pvtCompareTree
 * purpose: Compare between two trees
 *          The trees must be structure identical, the compare function only
 *          checks that the values are identical.
 * input  : val1H       - PVT handle of the 1st tree
 *          val1RootId  - Root ID of the 1st tree
 *          val2H       - PVT handle of the 2nd tree
 *          val2RootId  - Root ID of the 2nd tree
 * output : none
 * return : Non-negative value if trees are identical
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV pvtCompareTree(
    IN  HPVT            val1H,
    IN  RvPvtNodeId     val1RootId,
    IN  HPVT            val2H,
    IN  RvPvtNodeId     val2RootId);



RVAPI int RVCALLCONV
pvtAddChildsIfDiffer(
        IN  HPVT        destH,
        IN  RvPvtNodeId destParentId,
        IN  HPVT        srcH,
        IN  RvPvtNodeId srcParentId,
        IN  RvBool      move);

RVAPI int RVCALLCONV
pvtFindObject(
         IN  HPVT           hVal,
         IN  RvPvtNodeId    nodeId,
         IN  HPST           hSyn,
         IN  int            stNodeIdOfAT,
         OUT int*           objectId);


/************************************************************************
 * pvtPrintInternal
 * purpose: Print a tree from a given node.
 *          This function should be used internally instead of using
 *          pvtPrintStd().
 * input  : valH        - Value Tree handle
 *          parentId    - node ID to start printing from
 *          logHandle   - Log source to print to
 *          level       - Debug level to use for printing
 *                        0,1 - None
 *                        2 - Print the tree
 *                        3 - Print all values in the tree (even the irrelevant onse)
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV pvtPrintInternal(
    IN  HPVT            valH,
    IN  RvPvtNodeId     parentId,
    IN  RvLogSource*    logHandle,
    IN  int             level);


#if defined(RV_DEBUG)
/************************************************************************
 * pvtAddRootShielded
 * purpose: Adds a new shielded node that constitutes the root of a new tree.
 * input  : valH    - PVT handle to use
 *          synH    - Syntax tree to use
 *                    Can be set to -1 if it will be supplied later (by pvtSetTree)
 *          value   - Value of the root node, or length of the value if
 *                    value is a string
 *          string  - String value of the node, or NULL if not a string
 * output : none
 * return : Node id of the added root on success
 *          Negative value on failure
 ************************************************************************/
RVAPI RvPvtNodeId RVCALLCONV
pvtAddRootShielded(
    IN  HPVT        valH,
    IN  HPST        synH,
    IN  RvInt32     value,
    IN  const char* string);
#else
#define pvtAddRootShielded(_valH, _synH, _value, _string) pvtAddRoot(_valH, _synH, _value, _string)
#endif


#if defined(RV_DEBUG)
/************************************************************************
 * pvtDeleteShielded
 * purpose: Deletes a shielded sub-tree
 * input  : valH            - PVT handle to use
 *          subTreeRootId   - The ID of the root node to delete
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI int RVCALLCONV
pvtDeleteShielded(
    IN  HPVT          hVal,
    IN  RvPvtNodeId   subTreeRootId);
#else
#define pvtDeleteShielded(_hVal, _subTreeRootId) pvtDelete(_hVal, _subTreeRootId)
#endif



#ifdef __cplusplus
}
#endif

#endif  /* _PVALTREE_STACKAPI_H */


