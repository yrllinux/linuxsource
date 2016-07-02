/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/


#ifndef SS_H
#define SS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <rvcommon.h>
#include <psyntree.h>
#include <pvaltree.h>
#include <cm.h>

DECLARE_OPAQUE(HSSAPP);
DECLARE_OPAQUE(HSSAPPAPP);
DECLARE_OPAQUE(HSSSERVICE);
DECLARE_OPAQUE(HSSAPPSERVICE);
DECLARE_OPAQUE(HSSCALL);
DECLARE_OPAQUE(HSSAPPCALL);
DECLARE_OPAQUE(HSSOPER);
DECLARE_OPAQUE(HSSAPPOPER);


typedef enum
{
    csMessageSetup,
    csMessageCallProceeding,
    csMessageAlerting,
    csMessageConnect,
    csMessageReleaseComplete,
    csMessageFacility,
    csMessageProgress,
    csMessageAny
} csStateEnum;

typedef enum
{
    ssDefaultAck,
    ssDefaultNack,
    ssWait
} ssAction;


typedef enum
{
    ssReasonTypeDestinationRejection
} ssReasonType;

typedef int (RVCALLCONV *ssEvServicePrimitiveT)(
                            IN      HSSSERVICE      hSSServ,
                            IN      HSSAPPSERVICE   hSSaServ,
                            IN      int             code,
                            IN      BOOL            ack,
                            IN      BOOL            ok,
                            IN      char*           role,
                            IN      int             parameters);


typedef int (RVCALLCONV *ssEvServicePrimitiveExtendedT)(
                            IN      HSSSERVICE      hSSServ,
                            IN      HSSAPPSERVICE   hSSaServ,
                            IN      int             code,
                            IN      BOOL            ack,
                            IN      BOOL            ok,
                            IN      int             errCode,
                            IN      char*           role,
                            IN      int             parameters);



typedef int (RVCALLCONV *ssEvServiceExitedT)(
                            IN      HSSSERVICE      hSSServ,
                            IN      HSSAPPSERVICE   hSSaServ);



typedef  struct
{
    ssEvServicePrimitiveT         ssEvServicePrimitive;
    ssEvServiceExitedT            ssEvServiceExited;
    ssEvServicePrimitiveExtendedT ssEvServicePrimitiveExtended;
} SSSSERVICEEVENT,*SSSERVICEEVENT;


typedef int (RVCALLCONV *ssEvCallNewMessageT)(
                            IN      HSSCALL     hSSCall,
                            IN      HSSAPPCALL  hSSaCall,
                            IN      csStateEnum csState,
                            IN      char*       buff,
                            IN      int         size);

typedef int (RVCALLCONV *ssEvCallReleaseT)(
                            IN      HSSCALL         hSSCall,
                            IN      HSSAPPCALL      hSSaCall,
                            IN      HSSSERVICE      hSSServ,
                            IN      HSSAPPSERVICE   hSSaServ);


typedef int (RVCALLCONV *ssEvCallReleaseWithReasonT)(
                            IN      HSSCALL         hSSCall,
                            IN      HSSAPPCALL      hSSaCall,
                            IN      HSSSERVICE      hSSServ,
                            IN      HSSAPPSERVICE   hSSaServ,
                            IN      ssReasonType    ssReason);


typedef int (RVCALLCONV *ssEvCallBeforeEncodingT)(
                            IN      HSSCALL     hSSCall,
                            IN      HSSAPPCALL  hSSaCall,
                            IN      csStateEnum csState,
                            IN      int         message);

typedef  struct
{
    ssEvCallNewMessageT         ssEvCallNewMessage;
    ssEvCallReleaseT            ssEvCallRelease;
    ssEvCallReleaseWithReasonT  ssEvCallReleaseWithReason;
    ssEvCallBeforeEncodingT     ssEvCallBeforeEncoding;
} SSSCALLEVENT, *SSCALLEVENT;



/* Initialization/ Shutdown */

/*---------------------------------------------------------------------------------------
 * ssInit :
 *
 * Initialization of the ss instance.
 * The following operations are done:
 * 1. Exchange handles with the application.
 * 2. Allocate RA elements for the services, calls and the association between them.
 * 3. Create global Val Tree and Syntax Tree.
 *
 *
 * Input:  name - name of the configuration file.
 *         hSSaApp - handle of application ss instance.
 *
 * Output: hSSApp  - handle of ss instance.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to initialize the ss instance.
 *---------------------------------------------------------------------------------------*/
RVAPI int RVCALLCONV ssInit(IN      char*       name,
                            OUT     HSSAPP*     hSSApp,
                            IN      HSSAPPAPP   hSSaApp);


/*---------------------------------------------------------------------------------------
 * ssEnd :
 *
 * Shutdown the ss instance.
 * All the memory is deallocated before closing the instance.
 *
 *
 * Input:  hSSApp  - handle of ss instance.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to initialize the ss instance.
 *---------------------------------------------------------------------------------------*/
RVAPI int RVCALLCONV ssEnd(   IN      HSSAPP      hSSApp);


/*---------------------------------------------------------------------------------------
 * ssSetHandle :
 *
 * Set the application handle of the instance.
 *
 * Input:  hSSApp  - handle of ss instance.
 *         hSSaApp - application handle to set.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to set the handle.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssSetHandle(IN      HSSAPP      hSSApp,
                                  IN      HSSAPPAPP   hSSaApp);

/*---------------------------------------------------------------------------------------
 * ssGetHandle :
 *
 * Get the application handle of the instance.
 *
 * Input:  hSSApp  - handle of ss instance.
 *
 * Output: hSSaApp - application handle.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to get the handle.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssGetHandle(IN      HSSAPP      hSSApp,
                                  OUT     HSSAPPAPP*  hSSaApp);


 /*---------------------------------------------------------------------------------------
 * ssSetCallEventHandler :
 *
 * Set the application implementations of the call callbacks functions.
 *
 * Input:  hSSApp      - handle of ss instance.
 *         ssCallEvent - structs contains applications callbacks functions.
 *         size        - maximum size to set.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to set EventHandler struct.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssSetCallEventHandler(IN      HSSAPP      hSSApp,
                                            IN      SSCALLEVENT ssCallEvent,
                                            IN      int         size);

/*---------------------------------------------------------------------------------------
 * ssGetCallEventHandler :
 *
 * Get the application implementations of the call callbacks functions.
 *
 * Input:  hSSApp      - handle of ss instance.
 *         size        - maximum size to copy.
 *
 * Output: ssCallEvent - structs contains applications callbacks functions.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to get EventHandler struct.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssGetCallEventHandler(IN      HSSAPP      hSSApp,
                                            OUT     SSCALLEVENT ssCallEvent,
                                            IN      int         size);

/*---------------------------------------------------------------------------------------
 * ssGetVersion :
 *
 * Get current version.
 *
 * Input:  size - length of string to copy.
 *
 * Output: buffer - the version. the buffer is allocated by the applcation.
 *
 * Return: non negative value.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssGetVersion(INOUT     char*       buffer,
                                   IN        int         size);

/*---------------------------------------------------------------------------------------
 * ssSetDefaultAddress :
 *
 * Set local address (for use in the call transfer supplementry service).
 *
 * Input:  hSSApp      - handle of ss instance.
 *         addrNodeId  - pvt with the local address.
 *
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to set the address.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssSetDefaultAddress(IN      HSSAPP     hSSApp,
                                          IN      int        addrNodeId);


/*Services*/

/*---------------------------------------------------------------------------------------
 * ssCreateService :
 *
 * Create a new service object and initialize it.
 *
 * Input:  hSSApp      - handle of ss instance.
 *         hSSaServ    - application handle of the service.
 *         serviceName - unique string with the service name.
 *
 * Output: hSSServ     - ss handle of the new service.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to create new service.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCreateService(IN      HSSAPP        hSSApp,
                                      OUT     HSSSERVICE*   hSSServ,
                                      IN      HSSAPPSERVICE hSSaServ,
                                      IN      char*         serviceName);

 /*---------------------------------------------------------------------------------------
 * ssServiceSetHandle :
 *
 * Set application handle of the service.
 *
 * Input:  hSSServ     - handle of ss instance.
 *         hSSaServ    - application service handle to set.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to set application handle of the service.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssServiceSetHandle(IN      HSSSERVICE  hSSServ,
                                         IN      HSSAPPSERVICE hSSaServ);

/*---------------------------------------------------------------------------------------
 * ssServiceGetHandle :
 *
 * Get the application handle of the service.
 *
 * Input:  hSSServ     - handle of ss instance.
 *
 * Output: hSSaServ    - application service handle to set.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to get application handle of the service.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssServiceGetHandle(IN      HSSSERVICE      hSSServ,
                                         IN      HSSAPPSERVICE*  hSSaServ);


/*---------------------------------------------------------------------------------------
 * ssSetServiceEventHandler :
 *
 * Set the application implementations of the service callbacks functions.
 *
 * Input:  hSSApp         - handle of ss instance.
 *         ssServiceEvent - struct contains applications callbacks functions.
 *         size           - maximum size to set.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to set ssServiceEvent struct.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssSetServiceEventHandler(IN      HSSAPP         hSSApp,
                                               IN      SSSERVICEEVENT ssServiceEvent,
                                               IN      int            size);

/*---------------------------------------------------------------------------------------
 * ssGetServiceEventHandler :
 *
 * Get the application implementations of the service callbacks functions.
 *
 * Input:  hSSApp         - handle of ss instance.
 *         size           - maximum size to set.
 *
 * Output: ssServiceEvent - struct contains applications callbacks functions.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to get ssServiceEvent struct.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssGetServiceEventHandler(IN      HSSAPP         hSSApp,
                                               OUT     SSSERVICEEVENT ssServiceEvent,
                                               IN      int            size);

/*---------------------------------------------------------------------------------------
 * ssServiceClose :
 *
 * This function close the service by doing the following:
 * 1. For every call in the call list, remove the service from its services list.
 * 2. Release the list of calls of the service
 * 3. Release the service object from the services RA.
 *
 *
 * Input:  hSSServ        - handle of the service to close.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to close the service.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssServiceClose(IN      HSSSERVICE  hSSServ);



/*---------------------------------------------------------------------------------------
 * ssServiceAddCall :
 *
 * Associate call and service.
 * The function insert the service to the list of services of the call and insert the
 * call (with its role) to the list of calls of the service.
 *
 *
 * Input:  hSSServ        - ss handle of the service.
 *         hSSCall        - ss handle of the call.
 *
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to associate service and call.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssServiceAddCall(IN      HSSSERVICE  hSSServ,
                                       IN      HSSCALL     hSSCall,
                                       IN      char*       role);


/*---------------------------------------------------------------------------------------
 * ssServiceGetCall :
 *
 * Get the ss call handle, of the call associated to the service with the given role.
 *
 * Input:  hSSServ        - ss handle of the service.
 *         role           - role of the call at the service.
 *
 * Output: hSSCall        - ss handle of the call.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to close the service.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssServiceGetCall(IN      HSSSERVICE  hSSServ,
                                       IN      char*       role,
                                       OUT     HSSCALL*    hSSCall);


/*Calls*/

/*---------------------------------------------------------------------------------------
 * ssCreateCall :
 *
 * Create a new ss Call object and initialize it.
 *
 * Input:  hSSApp      - handle of ss instance.
 *         hSSaCall    - application handle of the call.
 *
 * Output: hSSCall     - ss andle of the new call.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to create new call.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCreateCall(IN      HSSAPP       hSSApp,
                                   OUT     HSSCALL     *hSSCall,
                                   IN      HSSAPPCALL   hSSaCall);


/*---------------------------------------------------------------------------------------
 * ssCallSetHandle :
 *
 * Set the application handle of the call.
 *
 * Input:  hSSCall     - ss handle of the call..
 *         hSSaCall    - application handle of the call to set.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to set application handle of the call.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCallSetHandle(IN      HSSCALL     hSSCall,
                                      IN      HSSAPPCALL  hSSaCall);


/*---------------------------------------------------------------------------------------
 * ssCallGetHandle :
 *
 * Get the application handle of the call.
 *
 * Input:  hSSCall     - ss handle of the call.
 *
 * Output: hSSaCall    - application handle of the call.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to get application handle of the call.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCallGetHandle(IN      HSSCALL     hSSCall,
                                      OUT     HSSAPPCALL* hSSaCall);


/*---------------------------------------------------------------------------------------
 * ssCallClose :
 *
 * This function close the call by doing the following:
 * 1. For every service in the services list, remove the call from its calls list.
 *    if after removing the call from the list, the service list of associated calls
 *    is empty, then the callback ssEvServiceExited is called.
 * 2. Release the list of services of the call.
 * 3. Release the call object from the call RA.
 *
 *
 * Input:  hSSServ        - handle of the service to close.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to close the call.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCallClose(IN      HSSCALL     hSSCall);





/*---------------------------------------------------------------------------------------
 * ssCallEncodeMessage :
 *
 * Encode a message from pvt structure.
 *
 * Input:  hSSCall - ss handle of the call.
 *         message - message in pvt.
 *         size - maximum size of buffer.
 *
 * Output: buffer - includes the message encoded.
 *
 * Return: non negative value - if success.
 *         negative value     - if failed to encode the message.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCallEncodeMessage(IN      HSSCALL   hSSCall,
                                          IN      int       message,
                                          OUT     char*     buffer,
                                          IN      int       size);



/*---------------------------------------------------------------------------------------
 * ssCallDecodeMessage :
 *
 * Decodes a buffer with encoded message to a pvt structure.
 *
 * Input:  hSSCall - ss handle of the call.
 *         csState - call state.
 *         buffer  - includes the message encoded.
 *         size    - size of buffer.
 *
 * Output:
 *
 * Return: non negative value - nodeId of the pvt decode message, if success.
 *         negative value     - if failed to decode the message.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCallDecodeMessage(IN        HSSCALL     hSSCall,
                                          IN        csStateEnum csState,
                                          IN        char*       buffer,
                                          IN        int         size);


/*---------------------------------------------------------------------------------------
 * ssCallProcessMessage :
 *
 * Check if the message belongs to one of the services releated to the call.
 * For each service activate the 'select' function that returns TRUE is case
 * the message belongs to the service.
 * The first service that the message belong to him, is processing the message,
 * and we don't keep looking for other service to process the message (the messages
 * should be unique per service).
 *
 * Input:  hSSCall - ss handle of the call.
 *         message - pvt with the decoded message.
 *
 * Return: non negative value -  if success.
 *         negative value     -  if error occured.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCallProcessMessage(IN       HSSCALL hSSCall,
                                           IN       int     message);




/*---------------------------------------------------------------------------------------
 * ssCallNewMessage :
 *
 * If buffer is not empty, the message is decoded and then processed by a service
 * asscoaited to the call, which identifys that the message belongs to him.
 * If buffer is empty, only the call state is updated.
 * In any case (buffer empty or not) we notify each of the service associated to the
 * call about the new Q931 message recieved.
 *
 * Input:  hSSCall - ss handle of the call.
 *         csState - the q931 call state in which the apdu was recieved.
 *         buffer  - encoded message recieved from network.
 *                   if buffer is NULL only call state is updated.
 *         size    - the size of the buffer.
 *
 * Return: non negative value -  if success.
 *         negative value     -  if error occured.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssCallNewMessage(IN       HSSCALL     hSSCall,
                                       IN       csStateEnum csState,
                                       IN       char*       buffer,
                                       IN       int         size);


/*---------------------------------------------------------------------------------------
 * ssGetStateName :
 *
 * Returns the name of the current state in the state machine hSSServ
 *
 * Input:  hSSServ - ss handle of the service.
 *
 * Output: state_name - string with the name of the state, allocated by the application
 *                      (maximum size is STATE_NAME_LENGTH).
 *
 * Return: non negative value -  if success.
 *         negative value     -  if error occured.
 *
 *---------------------------------------------------------------------------------------*/
RVAPI int RVCALLCONV ssGetStateName(IN      HSSSERVICE  hSSServ,
                                    OUT     char *      state_name);




/*Primitives*/


RVAPI int RVCALLCONV ssServicePrimitive(
    IN      HSSSERVICE  hSSServ,
    IN      int         code,
    IN      BOOL        ack,
    IN      BOOL        ok,
    IN      char*       role,
    IN      int         parameters);


/*---------------------------------------------------------------------------------------
 * ssGetValTree :
 *
 * Gets a handle to PVT (Program Value Tree) for performing operations with the PVT API.
 *
 * Input:  hSSApp - handle of the ss instnace.
 *
 * Return: The PVT handle, if function completed successfully.
 *         If an error occurs, the function returns a negative value.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI HPVT RVCALLCONV ssGetValTree(IN   HSSAPP          hSSApp);


 RVAPI HPST RVCALLCONV ssGetSynTreeByRootName(
                            IN      HSSAPP  hSSApp,
                            IN      char*       name);

/*---------------------------------------------------------------------------------------
 * ssMeiEnter :
 *
 * Enters a critical section. The function waits for ownership of the critical section
 * object associated with the specified application handle. The function returns when the
 * calling thread is granted ownership.
 * This function is used to protect the H.450 function calls that are not
 * protected by themselves.
 *
 * Input:  hSSApp - handle of the ss instnace.
 *
 * Return: non negative value -  if success.
 *         negative value     -  if hSSApp is NULL.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssMeiEnter(IN HSSAPP  hSSApp);


/*---------------------------------------------------------------------------------------
 * ssMeiEnter :
 *
 * Exits critical section. The function releases ownership of the critical section
 * object associated with the specified application handle.
 *
 * Input:  hSSApp - handle of the ss instnace.
 *
 * Return: non negative value -  if success.
 *         negative value     -  if hSSApp is NULL.
 *
 *---------------------------------------------------------------------------------------*/
 RVAPI int RVCALLCONV ssMeiExit(IN  HSSAPP  hSSApp);


/*---------------------------------------------------------------------------------------
 * rejectProblem -
 *
 * enumeration of all possible reject problems.
 * problems starting with '-1XX' are general problem.
 * problems starting with '-2XX' are invoke problem.
 * problems starting with '-3XX' are returnResult problem.
 * problems starting with '-4XX' are returnError problem.
 *
 *---------------------------------------------------------------------------------------*/
 typedef enum
 {
   /* General Problem */
   general_unrecognizedComponent     = -100,
   general_mistypedComponent         = -101,
   general_badlyStructuredComponent  = -102,

   /* InvokeProblem */
   invoke_duplicateInvocation       = -200,
   invoke_unrecognizedOperation     = -201,
   invoke_mistypedArgument          = -202,
   invoke_resourceLimitation        = -203,
   invoke_releaseInProgress         = -204,
   invoke_unrecognizedLinkedId      = -205,
   invoke_linkedResponseUnexpected  = -206,
   invoke_unexpectedLinkedOperation = -207,

   /*ReturnResultProblem */
   returnResult_unrecognizedInvocation    = -300,
   returnResult_resultResponseUnexpected  = -301,
   returnResult_mistypedResult            = -302,


   /*ReturnErrorProblem */
   returnError_unrecognizedInvocation    = -400,
   returnError_errorResponseUnexpected   = -401,
   returnError_unrecognizedError         = -402,
   returnError_unexpectedError           = -403,
   returnError_mistypedParameter         = -404

} rejectProblem;



/*
   Primitive enumerations for the various H.450 Supplamentary Services.
   Values of 60000 and higher are not declared in the ASN.1 standards at all, but
   rather used by SUPS as additional notifications that can be sent to the
   layer on top of SUPS (SSE or others).
*/

/* H450.2 Call Transfer */
typedef enum
{
    ssCTIdentify           =  7,
    ssCTAbandon            =  8,
    ssCTInitiate           =  9,
    ssCTSetup              = 10,
    ssCTActive             = 11,
    ssCTComplete           = 12,
    ssCTUpdate             = 13,
    ssCTSubaddressTransfer = 14
} ctPrimitivesEnum;

/* H450.3 Call Diversion */
typedef  enum
{
    ssCDActivateDiversionQ       = 15,
    ssCDDeactivateDiversionQ     = 16,
    ssCDInterrogateDiversionQ    = 17,
    ssCDCheckRestriction         = 18,
    ssCDCallRerouting            = 19,
    ssCDDivertingLegInformation1 = 20,
    ssCDDivertingLegInformation2 = 21,
    ssCDDivertingLegInformation3 = 22,
    ssCDDivertingLegInformation4 = 100,
    ssCDDivertedLegFailed        = 23
} cdPrimitivesEnum;

/* H450.4 Call Hold */
typedef enum
{
    ssCHholdNotific     = 101,
    ssCHretrieveNotific = 102,
    ssCHremoteHold      = 103,
    ssCHremoteRetrieve  = 104
} chPrimitivesEnum;

/* H450.5 Call Park and Pickup */
typedef enum
{
    ssCPRequest            = 106,
    ssCPSetup              = 107,
    ssCPGroupIndicationOn  = 108,
    ssCPGroupIndicationOff = 109,
    ssCPPickrequ           = 110,
    ssCPPickup             = 111,
    ssCPPickExe            = 112,
    ssCPNotify             = 113,
    ssCPickupNotify        = 114,
    ssCPAlertPark          = 60020 /*the application indicates to prkt that an alerting 
                                     all is waiting to be picked-up, the service should 
                                     changed its state in the state machine*/
} cpPrimitivesEnum;

/* H450.6 Call Waiting */
typedef enum
{
    ssCWcallWaiting = 105
} cwPrimitivesEnum;

/* H450.7 Message Waiting Indication */
typedef enum
{
    ssMWIActivate     = 80,
    ssMWIDeactivate   = 81,
    ssMWIInterrogate  = 82
} mwiPrimitivesEnum;

/* H450.8 Name Identification */
typedef enum
{
    ssNIcallingName   = 0,
    ssNIalertingName  = 1,
    ssNIconnectedName = 2,
    ssNIbusyName      = 3
} niPrimitivesEnum;


/* H450.9 Call Completion primitives */
typedef enum
{
    /* Primitives declared in the standard */
    ssCCBSRequest           = 40,
    ssCCNRRequest           = 27,
    ssCCCancel              = 28,
    ssCCExecPossible        = 29,
    ssCCRingout             = 31,
    ssCCSuspend             = 32,
    ssCCResume              = 33,
    ssShortTermRejection    = 1010,
    ssRemoteUserBusyAgain   = 1012,
    ssFailureToMatch        = 1013,

	/* "Virtual" primitives */
    ssCCCloseService        = 60010,    /* ssCCCloseService:
                                           If sent as a request through SUPS, it indicates
                                           that  the message is supposed to belong to some
                                           other service already running, and this service
                                           sould be closed.
                                           If received by the App/SSE, it indicates that
                                           SUPS wants this service to be canceled, and that
                                           the application should generate a call and send
                                           a ssCCCancel primitive on that call. */
    ssCCCallDropped         = 60011,    /* Call was dropped by one of the sides, and the
                                           service is still running. This means that we
                                           don't retain the connection anymore. */
    ssCCServiceIdle         = 60012,    /* Indication from SUPS that the service reached the
                                           IDLE state, meaning it has finished. */
    ssCCTimedOut            = 60013     /* Indication from SUPS that the service timed-out and
                                           the application has to cancel it. It is called
                                           when SUPS hasn't got a call for this service to use
                                           for canceling it automatically. */
} ccPrimitivesEnum;
/* Call Completion calls:
    primary - The calls that are used to maintain the service
    secondary - The call with the Ringout.inv primitive
*/


/* H450.10 Call Offer */
typedef enum
{
    ssCORequest				= 34,
    ssCWremoteUserAlerting  = 115, /*this message was intredused here but it 
                                     belongs to the wait service*/
    ssCOcfbOverride         = 49
} coPrimitivesEnum;

/* H450.11 Call Intrusion */
typedef enum
{
    ssCIRequest				    = 43,
    ssCIGetCIPL                 = 44,
    ssCIIsolated                = 45,
    ssCIForcedRelease           = 46,
    ssCIWOBRequest              = 47,
    ssCISilentMoitor            = 116,
    ssCINotification            = 117,
    ssCInotBusy                 = 1009,
    ssCItemporarilyUnavailable  = 1000,
    ssCInotAuthorized           = 1007,
    ssCIMakeSilent				= 60000,         /*opcode i added*/
    ssCIBcloseService           = 60001,         /*opcode i added*/
    ssCIconfInProgress          = 60002          /*opcode i added*/

} ciPrimitivesEnum;



#ifdef __cplusplus
}
#endif

#endif  /* SS_H */



