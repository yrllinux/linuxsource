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

#ifndef _CMIRAS_H
#define _CMIRAS_H

#include "rvinternal.h"
#include "cat.h"
#include "cmCrossReference.h"
#include "cmras.h"

#ifdef __cplusplus
extern "C" {
#endif



/* Application's handle of a RAS message */
RV_DECLARE_HANDLE(HAPPRASMSG);






/************************************************************************
 * cmiRASInitialize
 * purpose: Initialize a RAS instance for use by the CM
 * input  : hApp            - Stack's instance handle
 *          hCat            - CAT instance handle
 *          hVal            - Value "forest" to use for ASN.1 messages
 *          rasConfNode     - RAS configuration node id
 *          maxIncomingTx   - Maximum number of incoming RAS transactions
 *          maxOutgoingTx   - Maximum number of outgoing RAS transactions
 *          maxBufSize      - Maximum buffer size for encoded messages
 *          isGatekeeper    - Indication if we should act as a GK
 *          logMgr          - Log manager to use
 * output : none
 * return : RAS module handle on success
 *          NULL on failure
 ************************************************************************/
HRASMGR cmiRASInitialize(
    IN  HAPP        hApp,
    IN  RVHCAT      hCat,
    IN  HPVT        hVal,
    IN  RvPvtNodeId rasConfNode,
    IN  RvUint32    maxIncomingTx,
    IN  RvUint32    maxOutgoingTx,
    IN  RvUint32    maxBufSize,
    IN  RvBool      isGatekeeper,
    IN  RvLogMgr*   logMgr);


/************************************************************************
 * cmiRASStart
 * purpose: Start a RAS instance for use by the CM
 *          Should be called after cmiRASInitialize() and before any
 *          other function.
 * input  : hRasMgr             - RAS instance handle
 *          rasAddressesNode    - RAS listening addresses node id
 *          startingSeqNum      - Starting sequence number to use
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASStart(
    IN  HRASMGR     hRasMgr,
    IN  RvPvtNodeId rasAddressesNode,
    IN  RvUint32    startingSeqNum);


/************************************************************************
 * cmiRASStop
 * purpose: Stop a RAS instance from running
 * input  : hRasMgr - RAS instance handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASStop(
    IN  HRASMGR hRasMgr);


/************************************************************************
 * cmiRASEnd
 * purpose: Free any resources taken by a RAS instance and stop its
 *          activities.
 * input  : hRasMgr - RAS instance handle
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASEnd(
    IN  HRASMGR hRasMgr);


/************************************************************************
 * cmiRASGetEndpointID
 * purpose: Retrieves the EndpointID stored in the ras
 *
 * input  : hRasMgr             - RAS instance handle
 *          eId                 - pointer to the buffer for endpoint ID
 *                                buffer should be big enough for longest EID
 *                                possible (256 byte)
 * output : none
 * return : The length of EID in bytes on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASGetEndpointID(
    IN  HRASMGR hRasMgr,
    IN  void*   eId);


/************************************************************************
 * cmiRASUpdateRegInfo
 * purpose: Update the registration information inside the default RAS
 *          messages. This function should be called whenever the
 *          registration status changes.
 *          It takes most of the information from the RAS configuration
 *          message in the PVT database.
 * input  : hRasMgr             - RAS instance handle
 *          beforeRegistration  - RV_TRUE if called before cmRegister()
 *                                RV_FALSE if called after RCF received
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASUpdateRegInfo(
    IN  HRASMGR hRasMgr,
    IN  RvBool  beforeRegistration);


/************************************************************************
 * cmiRASUpdateCallSignalingAddress
 * purpose: Update the CallSignalling address inside the default RAS
 *          messages. This function should be called whenever this
 *          adress changes.
 *          It takes most of the information from the given nodeId
 * input  : hRasMgr     - RAS instance handle
 *          addressId   - nodeId of the addresses to set
 *          annexEId    - nodeId of the annexE addresses to set
 * output : none
 * return : Non negative value on success
 *          Negative value on failure
 ************************************************************************/
int cmiRASUpdateCallSignalingAddress(
    IN  HRASMGR     hRasMgr,
    IN  RvPvtNodeId addressId,
    IN  RvPvtNodeId annexEId);


/************************************************************************
 * cmiRASGetHAPP
 * purpose: Get the application's handle of a RAS instance for a
 *          specific stack's RAS transaction handle
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : The application's handle of the RAS instance on success
 *          NULL on failure
 ************************************************************************/
HAPP cmiRASGetHAPP(
    IN  HRAS hsRas);




/************************************************************************
 * cmiRASGetRequest
 * purpose: Gets the pvt node of the current RAS request
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : pvt node of the current RAS request
 *          or negative value on error
 ************************************************************************/
RvPvtNodeId cmiRASGetRequest(
    IN  HRAS         hsRas);


/************************************************************************
 * cmiRASGetResponse
 * purpose: Gets the pvt node of the current RAS response
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : pvt node of the current RAS response
 *          or negative value on error
 ************************************************************************/
RvPvtNodeId cmiRASGetResponse(
    IN  HRAS         hsRas);


/************************************************************************
 * cmiRASReceiveMessage
 * purpose: Notify the RAS instance that a message was received from the
 *          network and should be handled.
 * input  : hRasMgr         - RAS instance that needs to handle the message
 *          index           - Index of the connection the message was
 *                            received to
 *          isMulticast     - RV_TRUE if message was received on a multicast
 *                            connection
 *          srcAddress      - Address of the sender
 *          message         - The message received
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns RV_OK.
 ************************************************************************/
RvStatus cmiRASReceiveMessage(
    IN HRASMGR              hRasMgr,
    IN RvInt                index,
    IN RvBool               isMulticast,
    IN cmTransportAddress   *srcAddress,
    IN RvUint8*             messageBuf,
    IN RvSize_t             messageLength);







/************************************************************************
 *
 *                           CALLBACK functions
 *
 ************************************************************************/


/************************************************************************
 * cmiEvRasMessageToSend
 * purpose: Callback function that the RAS calls when it wants to send
 *          out a RAS message through the network.
 * input  : hApp            - Stack's instance handle
 *          index           - Index to send the message on
 *          destAddress     - Address to send the message to
 *          message         - The message to send
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
typedef RvStatus (RVCALLCONV *cmiEvRasMessageToSend)(
    IN HAPP                 hApp,
    IN RvInt                index,
    IN cmTransportAddress   *destAddress,
    IN RvUint8*             messageBuf,
    IN RvSize_t             messageLength);


/************************************************************************
 * cmiEvRASEPRequestT
 * purpose: Callback function that the RAS calls when a new incoming
 *          RAS request that is not related to any particular call
 *          is being handled
 * input  : hApp        - Stack instance handle
 *          hsRas       - Stack's handle for the RAS transaction
 *          transaction - The type of transaction that arrived
 *          srcAddress  - Address of the source
 *          notifyCb    - Callback for application about the request
 * output : none
 * return : RV_TRUE if message was processed by this callback and shouldn't
 *          be processed by the manual RAS callbacks
 *          RV_FALSE if message wasn't processed by this callback
 ************************************************************************/
typedef RvBool (RVCALLCONV *cmiEvRASEPRequestT)(
    IN  HAPP                hApp,
    IN  HRAS                hsRas,
    IN  cmRASTransaction    transaction,
    IN  cmTransportAddress  *srcAddress,
    IN  cmEvAutoRASRequestT notifyCb);


/************************************************************************
 * cmiEvRASCallRequestT
 * purpose: Callback function that the RAS calls when a new incoming
 *          RAS request that is related to a specific call that
 *          is being handled
 * input  : hApp        - Stack instance handle
 *          hsRas       - Stack's handle for the RAS transaction
 *          hsCall      - Stack's call handle
 *          transaction - The type of transaction that arrived
 *          srcAddress  - Address of the source
 *          haCall      - Application's call handle
 *          notifyCb    - Callback for application about the request
 * output : none
 * return : RV_TRUE if message was processed by this callback and shouldn't
 *          be processed by the manual RAS callbacks
 *          RV_FALSE if message wasn't processed by this callback
 ************************************************************************/
typedef RvBool (RVCALLCONV *cmiEvRASCallRequestT)(
    IN  HAPP                hApp,
    IN  HRAS                hsRas,
    IN  HCALL               hsCall,
    IN  cmRASTransaction    transaction,
    IN  cmTransportAddress  *srcAddress,
    IN  HAPPCALL            haCall,
    IN  cmEvAutoRASRequestT notifyCb);


/************************************************************************
 * cmiEvRASResponseT
 * purpose: Callback function that the RAS calls when a new incoming
 *          RAS response has to be handled
 * input  : haRas       - The application's RAS transaction handle
 *          hsRas       - Stack's handle for the RAS transaction
 *          trState     - Transaction stage we're in
 * output : none
 * return : non-negative value on success
 *          negative value on failure
 ************************************************************************/
typedef int (RVCALLCONV *cmiEvRASResponseT)(
    IN  HAPPRAS          haRas,
    IN  HRAS             hsRas,
    IN  cmRASTrStage     trStage);




/************************************************************************
 * cmiRASSetSendEventHandler
 * purpose: Set the callback function to use for sending messages through
 *          the network.
 * input  : hRasMgr         - RAS module to set
 *          evMessageToSend - Callback function to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetSendEventHandler(
    IN  HRASMGR                 hRasMgr,
    IN  cmiEvRasMessageToSend   evMessageToSend);


/************************************************************************
 * cmiRASSetTrEventHandler
 * purpose: Set the callback function to use for responses to RAS
 *          transactions.
 *          It is called per transaction if the stack wishes to be
 *          notified of the response.
 * input  : hRas            - The application's RAS transaction handle
 *                            to deal with
 *          cmEvRASResponse - The callback to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetTrEventHandler(
    IN  HRAS                hRas,
    IN  cmiEvRASResponseT   cmEvRASResponse);


/************************************************************************
 * cmiRASSetEPTrEventHandler
 * purpose: Set the callback function to use for new RAS requests that
 *          are not related to specific calls.
 * input  : hApp            - Application's handle for a stack instance
 *          cmEvRASRequest  - The callback to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetEPTrEventHandler(
    IN  HAPP                hApp,
    IN  cmiEvRASEPRequestT  cmEvRASEPRequest);


/************************************************************************
 * cmiRASSetCallTrEventHandler
 * purpose: Set the callback function to use for new RAS requests that
 *          are related to specific calls.
 * input  : hApp            - Application's handle for a stack instance
 *          cmEvRASRequest  - The callback to use
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetCallTrEventHandler(
    IN  HAPP                    hApp,
    IN  cmiEvRASCallRequestT    cmEvRASCallRequest);



/************************************************************************
 * cmiRASGetProperty
 * purpose: Gets the pvt node of the RAS property DB
 * input  : hsRas       - Stack's handle for the RAS transaction
 * output : none
 * return : pvt node of the RAS property DB
 *          or negative value on error
 ************************************************************************/
RvPvtNodeId cmiRASGetProperty(
    IN  HRAS         hsRas);


/************************************************************************
 * cmiEvRASNewCallT
 * purpose: Callback function that the RAS calls when a new call is detected
 * input  : hApp        - Application's handle for a stack instance
 *        : lphCatCall  - the pointer where to put the CAT handle
 *        : callObj     - call parameters
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
typedef int (*cmiEvRASNewCallT)(
    IN HAPP         hApp,
    IN RVHCATCALL*  lphCatCall,
    IN catStruct*   callObj);



/************************************************************************
 * cmiRASSetNewCallEventHandler
 * purpose: Set the callback function to use when RAS detects new call
 *
 * input  : hApp            - Application's handle for a stack instance
 *          cmiEvRASNewCall - The callback for new call
 * output : none
 * return : If an error occurs, the function returns a negative value.
 *          If no error occurs, the function returns a non-negative value.
 ************************************************************************/
int cmiRASSetNewCallEventHandler(
    IN  HAPP                hApp,
    IN  cmiEvRASNewCallT    cmiEvRASNewCall);





#ifdef __cplusplus
}
#endif

#endif  /* _CMIRAS_H */

