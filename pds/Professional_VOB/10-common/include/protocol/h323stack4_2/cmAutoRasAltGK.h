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
 * cmAutoRasAltGK
 * --------------
 * This file provides functions to handle retrival and selection of
 * Alternate Gatekeepers.
 ************************************************************************/


#ifndef _CM_AUTORAS_ALTGK_H
#define _CM_AUTORAS_ALTGK_H

#include "cmAutoRasIntr.h"

#ifdef __cplusplus
extern "C" {
#endif



/************************************************************************
 * autoRasGetNextAltGkNode
 * purpose: Advances the EP's search index and returns the next GK.
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call (may be NULL)
 * output : none
 * return : node ID of the new GK, Negative value if none.
 ************************************************************************/
int autoRasGetNextAltGkNode(autorasEndpoint * autoras, autorasCall * autorasC);

/************************************************************************
 * autoRasGetCurrentAltGkNode
 * purpose: returns the indexed GK.
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call (may be NULL)
 * output : none
 * return : node ID of the new GK, Negative value if none.
 ************************************************************************/
int autoRasGetCurrentAltGkNode(autorasEndpoint * autoras, autorasCall * autorasC);

/************************************************************************
 * autoRasRemovePermList
 * purpose: Clears the Permanent Alt GKs array and frees all the nodes.
 * input  : autoras - Automatic RAS instance
 * output : none
 * return : none
 ************************************************************************/
void autoRasRemovePermList(autorasEndpoint * autoras);

/************************************************************************
 * autoRasRemoveTempList
 * purpose: Clears the Temporary Alt GKs array and frees all the nodes.
 * input  : autoras - Automatic RAS instance
 * output : none
 * return : none
 ************************************************************************/
void autoRasRemoveTempList(autorasEndpoint * autoras);

/************************************************************************
 * autoRasIsInSearch
 * purpose: Checkes if the EP is in a state of Alt GK search
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call
 * output : none
 * remark : one of the instances may be NULL
 * return : true iff in search
 ************************************************************************/
RvBool autoRasIsInSearch(autorasEndpoint * autoras, autorasCall * autorasC);

/************************************************************************
 * autoRasResetSearch
 * purpose: Resets the status of a search to No Search
 * input  : autoras - Automatic RAS instance of an EP
 *          autorasC - Automatic RAS instance of a call
 * output : none
 * remark : one of the instances may be NULL
 * return : none
 ************************************************************************/
void autoRasResetSearch(autorasEndpoint * autoras, autorasCall * autorasC);

/************************************************************************
 * autoRasResetSearch
 * purpose: Set the array Premanent Alt GKs according to the AltGKs node,
 *          and sorts by priority.
 * input  : autoras - Automatic RAS instance of an EP
 *          nodeId - Alternate Gateeker node ID
 * output : none
 * return : none
 ************************************************************************/
void autoRasSetPermList(autorasEndpoint * autoras, int nodeId);

/************************************************************************
 * autoRasSetTempList
 * purpose: Set the array Temporary Alt GKs according to the AltGKs node,
 *          and sorts by priority.
 * input  : autoras - Automatic RAS instance of an EP
 *          nodeId - Alternate Gateeker node ID
 * output : none
 * return : none
 ************************************************************************/
void autoRasSetTempList(autorasEndpoint * autoras, int nodeId);

/************************************************************************
 * autoRasStartAltGKs
 * purpose: Set both arrays of Alt GKs to default (-1) values
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : none
 ************************************************************************/
void autoRasStartAltGKs(autorasEndpoint * autoras);

/************************************************************************
 * autoRasStopAltGKs
 * purpose: Deletes all the copied nodes in both AltGK arays
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : none
 ************************************************************************/
void autoRasStopAltGKs(autorasEndpoint * autoras);

/************************************************************************
 * autoRasGotPermList
 * purpose: Checkes if there is a list of Permanent Alt GK
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : true iff the PermAltGKs list contains GKs
 ************************************************************************/
RvBool autoRasGotPermList(autorasEndpoint * autoras);

/************************************************************************
 * autoRasGotTempList
 * purpose: Checkes if there is a list of Temporary Alt GK
 * input  : autoras - Automatic RAS instance of an EP
 * output : none
 * return : true iff the TempAltGKs list contains GKs
 ************************************************************************/
RvBool autoRasGotTempList(autorasEndpoint * autoras);





#ifdef __cplusplus
}
#endif

#endif  /* _CM_AUTORAS_ALTGK_H */
