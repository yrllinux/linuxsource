/***********************************************************************************************
  Notice:
  This document contains information that is proprietary to RADVISION LTD..
  No part of this publication may be reproduced in any form whatsoever without
  written prior approval by RADVISION LTD..

    RADVISION LTD. reserves the right to revise this publication and make changes
    without obligation to notify any person of such revisions or changes.

*************************************************************************************************/
/********************************************************************************************
*                                h235_api.h
*
* This file contains all the API of the h235 add-on
*
*
*      Written by                        Version & Date                        Change
*     ------------                       ---------------                      --------
*      Dani Stettiner                     10-Jan-2001
*
********************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __H235API_H
#define __H235API_H

#include "cm.h"

/*================================================
 * GENERAL DEFINITIONS
 ===============================================*/
DECLARE_OPAQUE(H235HAPP);       /* h235 application handle                                          */
DECLARE_OPAQUE(H235HKEY);       /* key handle                                                       */
DECLARE_OPAQUE(H235HMSG);       /* handle of the results of a received message security check*/

/*********************************************************************
 * The following string is the string of object id U.
 * It identifies the algorithm HMAC-SHA1-96 algorithm.
 * To be used as parameter for the h235SetEncodingCallback() function
 * (in this version, this is the only possible parameter)
 *********************************************************************/
#define OID_U_STRING "itu-t (0) recommendation (0) h (8) 235 version (0) 2 6"


/*=============================================
 * TYPEDEFS
 =============================================*/

typedef enum {
    h235ModeNone,
    h235ModeProcedure1
} h235Mode_t;

typedef enum {
    h235StateUndefined,
    h235StateSecurityOK,
	h235StateSecurityFailed
} h235State_t;


/*********************************************************************
 * reasons for success or fail of security check for incoming messages.
 *********************************************************************/

typedef enum {
    h235ReasonUndefined = 0,    /* no reason needed, message did not fail on sec check  */
    h235ReasonNoSecureRequired ,/* relevant when CheckIncomingMessages is FALSE         */
    h235ReasonNoSecurityInfo,   /*the message does not contain any security information */
    h235ReasonMessageError,     /* some info required for security check is missing     */
    h235ReasonGeneralIDIncorrect,/*the generalID is missing or not equal our ID         */
    h235ReasonSendersIDMissing, /*the sendersID is missing                              */
    h235ReasonUnknownSender,    /*When this application is a gatekeeper:                */
                                /*    the sender entry was not returned by application  */
                                /*else: sender id is not the expected gkId              */
    h235ReasonProcNotSupported, /*incoming msg security mode is not supported           */
    h235ReasonTimeStampMissing, /*the timestamp in the message was invalid              */
    h235ReasonTimeStampFail,    /*the timestamp compare not close enougth               */
    h235ReasonTimeRandomFail,   /*random value is incorrect                             */
    h235ReasonIncorrectAuth,    /*the authenticator in the message was incorrect        */
    h235ReasonEncodeCBMissing,  /*the relevant encode callback not supplied by          */
                                /*the application (by h235SetEncodingCallback())        */
    h235ReasonAppForceFail,     /*the application forced failure                        */
    h235ReasonGeneralError      /*general error                                         */
} h235Reason_t;

/****************************************************************************
 * Possible application replies when incoming message passed security check
 ***************************************************************************/
typedef enum {
    h235SuccessReplyForceFail = 100,     /*even though passed security, make it fail    */
    h235SuccessReplyDeleteMessage,       /*delete message and ignore it                 */
	h235SuccessReplyContinue  		     /*continue	(message flow continues, the 		*/
                                         /*application can check results later)         */
} h235AppSuccessReply_t;

/****************************************************************************
 * Possible application replies when incoming message failed security check
 ***************************************************************************/
typedef enum {
    h235FailReplyForceOk = 200,          /*ignore the failure, continue with security checks*/

    h235FailReplyDeleteMessage,          /*delete message and ignore it                     */

	h235FailReplyContinue	  		     /*continue, the message will continue its flow     */
                                         /*as usual, but the application can later check    */
                                         /*if failed (use h235GetSecurityCheckResults..)    */
} h235AppFailReply_t;


/****************************************
 * sendersId or GeneralId in BMP format
 ***************************************/
typedef struct {
  UINT8 data[256];
  INT32 length; 
} entityId_t;


/*================================================
 * EVENT HANDLERS PROTOTYPES
 ===============================================*/
/********************************************************************************
 * Type Name:  h235EvGetIDByRAS_t   (callback prototype)
 * Description : a callback to accept the ID of the target entity by RAS handle
 *               This callback is needed only when the application is a gatekeeper
 *               and for outgoing messages only.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          hsRas       - rasHandle
  *         haRas       - application RAS handle
* Output:  generalID    - the id of the other side, BMP format
 * Return: TRUE if found, FALSE if failed
 ********************************************************************************/
typedef BOOL (RVCALLCONV *h235EvGetIDByRAS_t)(
                            IN  HAPP hApp,
                            IN  H235HAPP h235HApp,
                            IN  HRAS        hsRas,
                            IN  HAPPRAS     haRas,
                            OUT entityId_t  *generalID);



/********************************************************************************
 * Type Name:  h235EvGetIDByCall_t   (callback prototype)
 * Description : a callback to accept the ID of the target entity by Call handle
 *               This callback is needed only when the application is a gatekeeper
 *                and for outgoing messages only.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          hsCall      - callHandle    -
 *          haCall      - application call handle
 * Output:  generalID   - the id of the other side, BMP format
 * Return: TRUE if found, FALSE if failed
 ********************************************************************************/
typedef BOOL (RVCALLCONV *h235EvGetIDByCall_t)(
                            IN  HAPP hApp,
                            IN  H235HAPP h235HApp,
                            IN  HCALL       hsCall,
                            IN  HAPPCALL    haCall,
                            OUT entityId_t  *generalID);



/********************************************************************************
 * Type Name:  h235EvGetEntryByEndpointId_t   (callback prototype)
 * Description : This callback is needed only when the application is a gatekeeper.
 *               The gatekeeper application should store entries for h235, one entry
 *               per endpoint.  The structure of the entry is unknown to the gatekeeper.
 *               This callback retrieves the entry by id.
 *               If no such entry exist, the application should create a buffer
 *               (of size h235GetSizeofEntityEntry()) and return its pointer.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          endpointID  - The id by which the application serach the entry (it is the id
 *                        which is sendersID in received messages, or generalID in sent messages)
 *
 * Output:  isNewEntry  - a flag says if the returned buffer this is a new allocation.
 *          ptr         - the buffer pointer
 * Return:
 ********************************************************************************/
typedef void (RVCALLCONV *h235EvGetEntryByEndpointId_t)(
                            IN  HAPP hApp,
                            IN  H235HAPP h235HApp,
                            IN  entityId_t  *endpointID,
                            OUT BOOL *isNewEntry,
                            OUT void **ptr);


/********************************************************************************
 * Type Name:  h235EvGetKeyHandleById_t   (callback prototype)
 * Description : This callback is needed only when the application is a gatekeeper.
 *               This callback retreives application data about a new endpoint, it is
 *               called when the callback h235EvGetEntryByEndpointId_t creates a new entry.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          endpointID  - The id by which the application serach the entry (it is the id
 *                        which is sendersID in received messages, or generalID in sent messages)
 * Output:  keyHandle   - a handle to the key(password) defined for the endpoint
 * Return: TRUE if the keyHandle was found, FALSE if not
 ********************************************************************************/
typedef BOOL (RVCALLCONV *h235EvGetKeyHandleById_t)(
                            IN  HAPP hApp,
                            IN  H235HAPP h235HApp,
                            IN  entityId_t  *endpointID,
                            OUT H235HKEY *keyHandle);


/********************************************************************************
 * Type Name:  h235EvFailure_t   (callback prototype)
 * Description : a callback to notify the application about security failure of
 *               incoming message.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          h235HMsg    - handle of the message check details, can be used
 *                      - to get more information about the message security check
 *          nodeId      - root node of incoming message
 *          senderId    - the sender id (if exists) from the message
 *          reason      - the security failure reason
 * Return: h235AppFailReply_t   (see comments in h235AppFailReply_t decleration)
 ********************************************************************************/
typedef h235AppFailReply_t (RVCALLCONV *h235EvFailure_t)(
                            IN  HAPP hApp,
                            IN  H235HAPP h235HApp,
                            IN  H235HMSG h235HMsg,
                            IN  int  nodeId,
                            IN  entityId_t *senderId,
                            IN  h235Reason_t reason);

/********************************************************************************
 * Type Name:  h235EvSuccess_t   (callback prototype)
 * Description : a callback to notify the application about security success of
 *               incoming message.
 * Input:   hApp        - stack application handle.
 *          h235HApp    - h235 application handle
 *          h235HMsg    - handle of the message check details, can be used
 *                      - to get more information about the message security check
 *          nodeId      - root node of incoming message
 *          senderId    - the sender id (if exists) from the message
  * Return: h235AppSuccessReply_t   (see comments in h235AppSuccessReply_t decleration)
 ********************************************************************************/
typedef h235AppSuccessReply_t (RVCALLCONV *h235EvSuccess_t)(
                            IN  HAPP hApp,
                            IN  H235HAPP h235HApp,
                            IN  H235HMSG h235HMsg,
                            IN  int  nodeId,
                            IN  entityId_t *senderId);


/********************************************************************************
 * Type Name:  h235EvEncode_t    (callback prototype)
 * Description : a callback to accept encode results from the Application
 *               for a specific message.
 *               for example: the results of the HMAC-SHA1-96 algorithm.
 * Input:
 *        keyHandle - that was accepted from the Application by h235EvGetKeyHandleById_t callback
 *        buf       - The message buffer
 *        bufLen    -
 * Output:digest    - the value calculated by the application encode function
 *        digestLen - length(in bytes) of digest
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
typedef int (RVCALLCONV *h235EvEncode_t)(
                        IN H235HKEY keyHandle ,
                        IN void *Buf,
                        IN int bufLen,
                        OUT unsigned char* digest,
                        OUT int *digestLen);


typedef struct
{
    /******************************************************/
    /* these callbacks are relevant for all applications  */
    /* (endpoints and gatekeepers)                        */
    /* If the callback pointer is NULL, or not set, h235  */
    /* will behave as if  replyContinue  was returned.    */
    /******************************************************/
    h235EvFailure_t         evFailure;      /* OPTIONAL, can set NULL */
    h235EvSuccess_t         evSuccess;      /* OPTIONAL, can set NULL */

    /********************************************************************/
    /* these callbacks are required from gatekeeper                     */
    /* applications only, not relevant for endpoint applications        */
    /* All of them are MANDATORY for gatekeeper apps, NULL for others   */
    /********************************************************************/
    h235EvGetIDByRAS_t              evGetIDByRAS;
    h235EvGetIDByCall_t             evGetIDByCall;
    h235EvGetEntryByEndpointId_t    evGetEntryByEndpointId;
    h235EvGetKeyHandleById_t        evGetKeyHandleById;

} h235EventHandlers_T;


/*=============================================
 * Function prototypes
 ==============================================*/
/********************************************************************************
 * Function Name:  h235SetEventHandlers
 * Description :
 *               Sets the event handlers (callbacks) the application should supply.
 *               For more information see h235EventHandlers_T definition.
 * Input: h235HApp          - h235 instance handle.
 *        eventHandlers     - the callbacks structure
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetEventHandlers(
    IN  H235HAPP h235HApp,
    IN  h235EventHandlers_T     *eventHandlers);


/********************************************************************************
 * Function Name:  h235Init
 * Description : Initiates h235 module instance
 *
 * Input: hApp      - stack application handle.
 * Output:h235HApp  - The new h235 handle
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235Init(IN  HAPP hApp,OUT H235HAPP *h235HApp);

/********************************************************************************
 * Function Name:  h235End
 * Description : close and delete h235 module instance
 *
 * input:       h235 application handle
 * Return:      0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235End(IN   H235HAPP h235HApp);


/************************************************************************
 * h235GetHAPP
 * purpose: Get the stack handle of a stack instance for a
 *          h235 application handle
 * input:       h235 application handle
 * output : none
 * return : The stack instance handle of the h235 instance handle on success
 *          NULL on failure
 ************************************************************************/
RVAPI
HAPP RVCALLCONV h235GetHAPP(
    IN  H235HAPP h235HApp);


/********************************************************************************
 * Function Name:  h235SetSecureMode
 * Description : Sets the secure mode for sent messages.
 *               This function can be called any time while exceuting
 *               to change the current mode.
 *               Until this function is called, the default mode is h235ModeNone.
 * Input: h235HApp          - h235 instance handle.
 *        secureMode        - The required secure mode
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetSecureMode(
    IN  H235HAPP        h235HApp,
    IN  h235Mode_t    secureMode);

/********************************************************************************
 * Function Name:  h235GetSecureMode
 * Description : Gets the secure mode for sent messages.
 * Input: h235HApp          - h235 instance handle.
 * Return: secureMode       - The  secure mode
 ********************************************************************************/
RVAPI
h235Mode_t RVCALLCONV h235GetSecureMode(
    IN  H235HAPP        h235HApp);

/********************************************************************************
 * Function Name:  h235SetCheckIncomingMessages
 * Description : Sets whether incoming messages should pass security check.
 *               This function can be called any time while exceuting.
 *               Until this function is called, the default value is FALSE.
 * Input: h235HApp          - h235 instance handle.
 *        isCheck           - TRUE: check, FALSE: dont check
  * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetCheckIncomingMessages(
    IN  H235HAPP        h235HApp,
    IN  BOOL            isCheck);

/********************************************************************************
 * Function Name:  h235GetCheckIncomingMessages
 * Description : Gets whether incoming messages should pass security check.
 * Input: h235HApp          - h235 instance handle.
 *
 * Return: isCheck          - TRUE: check, FALSE: dont check
 ********************************************************************************/
RVAPI
BOOL RVCALLCONV h235GetCheckIncomingMessages(
    IN  H235HAPP        h235HApp);

/********************************************************************************
 * Function Name:  h235SetSenderId
 * Description : Sets the sendersId.
 *               This value is used for purposes:
 *                  1. Set as the sendersId in sent messages.
 *                  2. Compared with generalId from received messages.
 * Input: h235HApp          - h235 instance handle.
 *        senderId          - the id of this application.
  * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetSenderId(
    IN  H235HAPP        h235HApp,
    IN  entityId_t      *senderId);

/********************************************************************************
 * Function Name:  h235GetSenderId
 * Description : Gets the sendersId.
 * Input: h235HApp          - h235 instance handle.
 * Output:senderId          - the id of this application.
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSenderId(
    IN  H235HAPP        h235HApp,
    OUT entityId_t      *senderId);

/********************************************************************************
 * Function Name:  h235SetGatekeeperId
 * Description :
 *               This function should be called only by non-gatekeeper applications.
 *               By calling this function, you declare your application to be an
 *               endpoint application, if not called, h235 assumes it is a gatekeeper
 *               application.
 *               The gatkeeperId is used for purposes:
 *                  1. Set as the generalId in sent messages.
 *                  2. Compared with sendersId from received messages.
 * Input: h235HApp          - h235 instance handle.
 *        gatkeeperId       - the id
 *        keyHandle         - key handle to be used with messages sent to
 *                            the gatekeeper or received from it.
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetGatekeeperId(
    IN  H235HAPP        h235HApp,
    IN  entityId_t      *gatkeeperId,
    IN H235HKEY         keyHandle);

/********************************************************************************
 * Function Name:  h235GetGatekeeperId
 * Description :
 *               Gets the GatekeeperId.
 * Input: h235HApp          - h235 instance handle.
 * Output:gatkeeperId       - the id
 *        keyHandle         - the key handle
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetGatekeeperId(
    IN  H235HAPP        h235HApp,
    OUT entityId_t      *gatkeeperId,
    OUT H235HKEY        *keyHandle);

/********************************************************************************
 * Function Name:  h235SetTimeSync
 * Description :
 *               Sets your time synchronization check policy.
 *               This is about the compare of time when the message was sent,
 *               and the time it was received by this application.
 * Input: h235HApp          - h235 instance handle.
 *        performTimeSync   - TRUE: compare times, FALSE: dont compare
 *        timeDiff          - The difference between time of creation and time
 *                            of receiving a message (in seconds), that you still
 *                            consider as close enough to pass the security check.
 *                            (this value is not defined in the standard, and is
 *                            for application decision.
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetTimeSync(
    IN  H235HAPP        h235HApp,
    IN  BOOL            performTimeSync,
    IN  UINT32          timeDiff);

/********************************************************************************
 * Function Name:  h235GetTimeSync
 * Description :
 *               Gets the timediff settings
 * Input: h235HApp          - h235 instance handle.
 * Output:performTimeSync   - TRUE: compare times, FALSE: dont compare
 *        timeDiff          - The time difference
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetTimeSync(
    IN  H235HAPP        h235HApp,
    OUT  BOOL           *performTimeSync,
    OUT  UINT32         *timeDiff);

/********************************************************************************
 * Function Name:  h235GetSizeofEntityEntry
 * Description :
 *               Returns the buffer size that the gatekeeper application should allocate
 *               when it returns a pointer via the h235EvGetEntryByEndpointId_t callback
 * Input: none
 * Return: the size
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSizeofEntityEntry(void);

/********************************************************************************
 * Function Name:  h235SetEncodingCallback
 * Description : sets the encoding callback functions
 *
 * Input: h235HApp          - h235 instance handle.
 *        callback          - a pointer to the callback function
 *        algorithmOIDstr   - the relevant Object ID string that identifies
 *                            the algorithm applied by the callback.
 *                            You may use OID_U_STRING to identify the  HMAC-SHA1-96 algorithm
 *                            (this algorithm is required by procedure 1), other algorithms
 *                            are for future use.
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235SetEncodingCallback(
    IN  H235HAPP        h235HApp,
    IN  h235EvEncode_t  callback,
    IN  const char      *algorithmOIDstr);

/********************************************************************************
 * Function Name:  h235GetSecurityCheckResultsByRAS
 * Description : Gets the results of security check.
 *               This function allows you to check the results of the security check.
 * Input: h235HApp          - h235 instance handle.
 *        hsRas             - a RAS handle, this function will retreive the security check results
 *                            of the last received message related to this RAS handle.
 * Output:
 *        state             - security check results, can pass NULL value
 *        reason            - security fail or success reason, can pass NULL value
 *        h235HMsg          - handle of the security check results.
 *                            This handle can be used to get
 *                            more details about the results (e.g. h235GetModeByHMSG()).
 *                            Can pass NULL value
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSecurityCheckResultsByRAS(
    IN  H235HAPP        h235HApp,
    IN  HRAS            hsRas,
    OUT h235State_t     *state,
    OUT h235Reason_t    *reason,
    OUT H235HMSG        *h235HMsg);

/********************************************************************************
 * Function Name:   h235GetSecurityCheckResultsByCall
 * Description :    Gets the results of security check.
 *                  This function allows you to check the results of the security check.
 * Input: h235HApp          - h235 instance handle.
 *        hsCall            - a call handle, this function will retreive the security check results
 *                            of the last received message related to this RAS handle.
 * Output:
 *        state             - security check results, can pass NULL value
 *        reason            - security fail or success reason, can pass NULL value
 *        h235HMsg          - handle of the security check results, can be used to get
 *                            more details about the results (e.g. h235GetModeByHMSG()).
 *                            Can pass NULL value
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetSecurityCheckResultsByCall(
    IN  H235HAPP        h235HApp,
    IN  HCALL           hsCall,
    OUT h235State_t     *state,
    OUT h235Reason_t    *reason,
    OUT H235HMSG        *h235HMsg);


/********************************************************************************
 * Function Name:   h235GetStateByHMSG
 * Description :    Gets the security state of an incoming message.
 * Input: h235HMsg          - handle of message security results
 * Output:state             - the state of security check (pass/fail)
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetStateByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235State_t     *state);

/********************************************************************************
 * Function Name:   h235GetReasonByHMSG
 * Description :    Gets the security state reason of an incoming message.
 * Input: h235HMsg          - handle of message security results
 * Output:reason            -
 * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetReasonByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235Reason_t    *reason);

/********************************************************************************
 * Function Name:   h235GetModeByHMSG
 * Description :    Gets the security mode of an incoming message.
 * Input: h235HMsg          - handle of message security results
 * Output:secureMode        - the mode of security which the message was secured with.
 *                            If the message has no security info, or the mode is unfamiliar
 *                            to RV h235, h235ModeNone will be returned.
  * Return: 0 - 0k , -1 - failed
 ********************************************************************************/
RVAPI
int RVCALLCONV h235GetModeByHMSG(
    IN  H235HMSG        h235HMsg,
    OUT h235Mode_t      *secureMode);


/********************************************************************************
 * Function Name:   h235ReasonName
 * Description :    translates reason code to a name
 * Input:   reason              - reason code
 * Return:  the name string
 ********************************************************************************/
RVAPI
const char* RVCALLCONV h235ReasonName(
    IN   h235Reason_t reason);

/********************************************************************************
 * Function Name:   h235ModeName
 * Description :    translates mode code to a name
 * Input:   mode            - mode code
 * Return:  the name string
 ********************************************************************************/
RVAPI
const char* RVCALLCONV h235ModeName(
    IN   h235Mode_t mode);

/********************************************************************************
 * Function Name:   h235StateName
 * Description :    translates state code to a name
 * Input:   state           - state code
 * Return:  the name string
 ********************************************************************************/
RVAPI
const char* RVCALLCONV h235StateName(
    IN   h235State_t    state);


#endif

#ifdef __cplusplus
}
#endif

