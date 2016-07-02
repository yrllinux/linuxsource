#ifdef __cplusplus
extern "C" {
#endif



#ifndef SSE_H
#define SSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rvcommon.h>
#include <h450.h>

/* SSE Instance handle and application's handle */
DECLARE_OPAQUE(HSSEAPP);
DECLARE_OPAQUE(HSSEAPPAPP);

/* SSE Call handle and application's call handle */
DECLARE_OPAQUE(HSSECALL);
DECLARE_OPAQUE(HSSEAPPCALL);


/********************************************************************************************
 *              Service association
 * Some of the services need association after their creation/invokation, and sometimes this
 * association can't be done using specific call handles.
 * This happens mainly when the service can exist at times when it has no call associated
 * to it.
 * For these kinds of services, we use the specified handle and enumerations
 ********************************************************************************************/

/* SSE Service handle and application's handle */
DECLARE_OPAQUE(HSSESERVICE);
DECLARE_OPAQUE(HSSEAPPSERVICE);

/* Type of service. This list includes only the types that support and require the use
   of HSSESERVICE. */
typedef enum
{
    sseServiceCallCompletion
} sseServiceType;




typedef enum
{
    cfu_p   = 0,
    cfb_p   = 1,
    cfnr_p  = 2
}   proc_e;

typedef enum
{
    unknown_r = 0,
    cfu_r     = 1,
    cfb_r     = 2,
    cfnr_r    = 3
}   reason_e;


typedef enum
{
    unspecified =0,
    parkedToUserIdle , 
    parkedToUserBusy , 
    parkedToGroup 
} cpCallCondition;

typedef enum
{
  parkedCall = 0, 
  alertingCall  
}cpCallType;




/********************************************************************************************
 *
 *                  Callbacks supplied by the application
 *
 ********************************************************************************************/


/********************************************************************************************
 * sseEvServiceHandleCreatedT
 * purpose: Creation of a service handle indication for the application
 *          Called only for the services in sseServiceType, which were created by network events.
 * input  : servType    - Service type that was created
 *          hSSEaCall   - The application's handle of the SSE call that caused the creation
 *                        of this service
 *          hSSECall    - The SSE handle of the SSE call that caused the creation
 *                        of this service
 *          hSSEService - HSSE Service created
 * output : hSSEaService- Application's handle for the service
 ********************************************************************************************/
typedef int
    (RVCALLCONV *sseEvServiceHandleCreatedT)(
        IN      sseServiceType  servType,
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSESERVICE     hSSEService,
        OUT     HSSEAPPSERVICE* hSSEaService);

/********************************************************************************************
 * sseEvServiceHandleIdleT
 * purpose: Notification for the application to cancel the service handle
 *          Called only for the services in sseServiceType.
 * input  : hSSEaService- Application's handle of the SSE service
 *          hSSEService - HSSE Service to cancel
 *          servType    - Service type that was created
 * output : none
 ********************************************************************************************/
typedef int
    (RVCALLCONV *sseEvServiceHandleIdleT)(
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        IN      sseServiceType  servType);

typedef int
    (RVCALLCONV *sseEvForwardActivatedT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall);


typedef int
    (RVCALLCONV *sseEvForwardDeactivatedT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall);

typedef int
    (RVCALLCONV *sseEvCallTransferT)(
        IN      HSSEAPPCALL     hSSEaCallPri,
        IN      HSSECALL        hSSECallPri,
        OUT     HSSECALL*       hSSECallSec);

typedef int
    (RVCALLCONV *sseEvCallTransferSetupT)(
        IN      HSSEAPPCALL     hSSEaCallPri,
        IN      HSSECALL        hSSECallPri);

typedef int
    (RVCALLCONV *sseEvCallRerouteT)(
        IN      HSSEAPPCALL     hSSEaCallPri,
        IN      HSSECALL        hSSECallPri,
        OUT     HSSECALL*       hSSECallSec);

typedef int
   (RVCALLCONV *sseEvRemoteHoldIndT)(
        IN      HSSEAPPCALL     hSSEaCallPri,
        IN      HSSECALL        hSSECallPri
   );

typedef int
   (RVCALLCONV *sseEvRemoteHoldRetrieveIndT)(
        IN      HSSEAPPCALL     hSSEaCallPri,
        IN      HSSECALL        hSSECallPri
    );

typedef void
   (RVCALLCONV *sseEvRemoteHoldConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  UINT32          ok
    );

typedef void
   (RVCALLCONV *sseEvRemoteRetrieveConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  UINT32          ok
    );

typedef int
    (RVCALLCONV *sseEvNearHoldIndT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri
    );

typedef int
    (RVCALLCONV *sseEvNearHoldRetrieveIndT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri
    );


typedef void
    (RVCALLCONV *sseEvWaitIndT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri
    );


typedef void
    (RVCALLCONV *sseEvWaitTimerExpiredT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri
    );



typedef struct
{
    int num;
    char partyNum[2048];
    char numString[2048];
    char choice[20]; /* NUMBER / PN / NUMSTR */
} MsgCtrIdT;



typedef struct
{
    char service[100];
    MsgCtrIdT msgCtr;
    int       numOfmsgs;
    char      orig[2048];
    char      time[200];
    int       priority;
} interResT;



typedef enum
{
    calling    = 0,
    alerting   = 1,
    connected  = 2,
    busy       = 3

}   niOperation;


typedef enum
{
    allowed      = 0,
    restricted   = 1,
    notAvailable = 2

} niNameChoice;


typedef void
    (RVCALLCONV *sseEvMWIActivateConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  UINT32          ok
    );

typedef int
    (RVCALLCONV *sseEvMWIActivateIndT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  char *          servedUserNr,
        IN  int             numOfMsgs,
        IN  char *          service,
        IN  MsgCtrIdT*      msgCtrId,
        IN  char *          origNum,
        IN  char *          time,
        IN  int             priority
    );


typedef int
    (RVCALLCONV *sseEvMWIDeactivateIndT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  char *          servedUserNr,
        IN  char *          service,
        IN  MsgCtrIdT*      msgCtrId,
        IN  int             isCallBackReq
    );


typedef void
    (RVCALLCONV *sseEvMWIDeactivateConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  UINT32          ok
    );



typedef int
    (RVCALLCONV *sseEvMWIInterrogateIndT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  char *          servedUserNr,
        IN  char *          service,
        IN  MsgCtrIdT*      msgCtrId,
        IN  int             isCallBackReq

    );


typedef void
    (RVCALLCONV *sseEvMWIInterrogateConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             resultElements,
        IN  int             errCode,
        IN  int             ok
    );


typedef void
    (RVCALLCONV *sseEvNICallingNameIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      char *          simple_name,
        IN      char *          character_set,
        IN      niNameChoice    choice
    );

typedef void
    (RVCALLCONV *sseEvNIAlertingingNameIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      char *          simple_name,
        IN      char *          character_set,
        IN      niNameChoice    choice
    );

typedef void
    (RVCALLCONV *sseEvNIConnectedNameIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      char *          simple_name,
        IN      char *          character_set,
        IN      niNameChoice    choice
    );


typedef void
    (RVCALLCONV *sseEvNIBusyNameIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      char *          simple_name,
        IN      char *          character_set,
        IN      niNameChoice    choice
    );


typedef int 
    (RVCALLCONV *sseEvCfbOverrideIndT)(
		IN      HSSEAPPCALL	    hSSEaCall,
		IN      HSSECALL	    hSSECall
	);

typedef int 
    (RVCALLCONV *sseEvRemoteUseAlertingIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall);

typedef int 
    (RVCALLCONV *sseEvCOCompleteT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall);

typedef int 
    (RVCALLCONV *sseEvCIIsolatedIndT)(IN HSSEAPPCALL  hSSEaCall,
                                      IN HSSECALL      call,
                                      OUT int*         parameter);
typedef int 
(RVCALLCONV *sseEvCIForcedReleaseIndT)(IN HSSEAPPCALL  hSSEaCall,
                                       IN HSSECALL     call,
                                       OUT BOOL *      possible,
                                       OUT int*         parameter);
typedef int 
(RVCALLCONV *sseEvCIWOBIndT)(IN HSSEAPPCALL  hSSEaCall,
                             IN HSSECALL     call,
                             OUT BOOL *      possible,
                             OUT int *        parameter);
typedef int 
    (RVCALLCONV *sseEvCINotificationIndT)(IN HSSEAPPCALL  hSSEaCall,
	    						 IN HSSECALL     call);
typedef int 
    (RVCALLCONV *sseEvCIInitialRequestIndT)(IN   HSSEAPPCALL              hSSEaCall,
                                            IN   HSSECALL                 call, 
										    IN   ciPrimitivesEnum         opcode,
                                            IN   char*                    callIdentifer,
                                            OUT  int*                     reason,
                                            OUT  BOOL*                    possible,
                                            OUT  BOOL*                    IsIsolated,
                                            OUT  HSSECALL*                estCall);

typedef int 
    (RVCALLCONV *sseEvCIMakeSilentIndT)(  IN HSSEAPPCALL              hSSEaCallEst,
                                          IN   HSSECALL               callEst); 
typedef int 
    (RVCALLCONV *sseEvCIFindServiceT)(
        IN  HSSEAPPCALL     hSSEaCall,
        IN  HSSECALL        call,
        OUT HSSECALL*       oldHSSServ);


typedef void
    (RVCALLCONV *sseEvCIRequestConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  int             ok
    );
typedef void
    (RVCALLCONV *sseEvCISilentMonitorConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  int             ok
    );
typedef void
    (RVCALLCONV *sseEvCIWOBRequestConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  int             ok
    );
typedef void
    (RVCALLCONV *sseEvCIForcedReleaseConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  int             ok
    );
typedef void
    (RVCALLCONV *sseEvCIIsolatedConfirmT)(
        IN  HSSEAPPCALL     hSSEaCallPri,
        IN  HSSECALL        hSSECallPri,
        IN  int             errCode,
        IN  int             ok
    );
typedef int
    (RVCALLCONV *sseEvCParkRequestIndT)(
        IN HSSEAPPCALL      hSSEaCallPri,
        IN HSSECALL         hSSECallPri,
        OUT HSSECALL*       callSec
    );
typedef int
    (RVCALLCONV *sseEvCPickupRequestIndT)(
		IN  HSSEAPPCALL     hSSEaCall,
		IN  HSSECALL        call,
		IN  char*           callIdentifier
    );
typedef int
    (RVCALLCONV *sseEvCPickupFindCallT)(
		IN  HSSEAPPCALL     hSSEaCall,
		IN  HSSECALL        call,
		IN  char*           callIdentifier,
        IN  BOOL            IsIncoming,
        OUT HSSECALL*       oldHSSCall
    );
typedef int
    (RVCALLCONV *sseEvCPickupIndT)(
        IN    HSSEAPPCALL     hSSEaCall,
        IN    HSSECALL        call,
        OUT   char*           locAddr,
        OUT   HSSECALL*       pickupCall);
typedef int
    (RVCALLCONV *sseEvCPSetupIndT)(
        IN  HSSEAPPCALL     hSSEaCall,
        IN  HSSECALL        call,
        OUT int*             parkPosition,
        OUT cpCallCondition* parkCondition);

typedef int 
    (RVCALLCONV *sseEvCPickExeIndT)(
        IN  HSSEAPPCALL hSSEaCall,
        IN  HSSECALL    call,
        IN  char*       callIdentifier,
        OUT int*        reason);typedef int
    (RVCALLCONV *sseEvCPGroupIndicationT)(
        IN   HSSEAPPCALL  hSSEaCall,
        IN   HSSECALL     call,
        IN   int          parameters,
        IN   BOOL         IsIndOn
    );
typedef int
    (RVCALLCONV *sseEvCPNotifyIndT)(
        IN HSSEAPPCALL  hSSEaCall,
        IN HSSECALL     call,
        IN int          parameters);
typedef int
    (RVCALLCONV *sseEvCPickNotifyIndT)(
        IN HSSEAPPCALL  hSSEaCall,
        IN HSSECALL     call,
        IN int          parameters);

typedef int                                       
    (RVCALLCONV *sseEvCPRequestConfirmT)(
        IN HSSEAPPCALL       hSSEaCallPri,
        IN HSSECALL          hSSECallPri,
        IN int               errCode,
        IN BOOL              ok
    ) ;
typedef int                                       
    (RVCALLCONV *sseEvCPPickRequConfirmT)(
        IN HSSEAPPCALL    hSSEaCall,
        IN HSSECALL       call, 
        IN int            errCode, 
        IN int            ok,
        IN char*          callIdentifier
    );
typedef int                                       
    (RVCALLCONV *sseEvCPPickupConfirmT)(
        IN HSSEAPPCALL    hSSEaCall,
        IN HSSECALL       call, 
        IN int            errCode, 
        IN int            ok,
        IN char*          callIdentifier
    );
typedef int                                       
    (RVCALLCONV *sseEvCPSetupConfirmT)(
        IN HSSEAPPCALL       hSSEaCall,
        IN HSSECALL          call, 
        IN int               errCode, 
        IN int               ok,
        IN int*              reason
    );
typedef int                                       
    (RVCALLCONV *sseEvCPGroupIndicationConfirmT)(
        IN HSSEAPPCALL    hSSEaCall,
        IN HSSECALL          call, 
        IN int               errCode, 
        IN int               ok, 
        IN BOOL              indOn
    );

typedef void
    (RVCALLCONV *sseEvRemoteUserAlertingIndT)(
        IN HSSEAPPCALL      hSSEaCallPri,
        IN HSSECALL         hSSECallPri
    );




/*-----------------------------------------------------------------------------------------
 * sseCallImplementForward
 *
 * Passes the appropriate call handle when preparing the calls that will participate in
 * the Forward service, such as the originating call, the served call, or the rerouted call.
 *
 * Input:   hSSECall -  The call object handle of SSE.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
 *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallImplementForward(IN    HSSECALL   hSSECall);


/*-----------------------------------------------------------------------------------------
 * sseCallImplementForwardServer
 *
 * Passes a call handle when preparing the calls that will participate in the Transfer service.
 * A call can be an originating call, a served call or a rerouted call.
 *
 * Input:   hSSECall -  The call object handle of SSE.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallImplementForwardServer(IN HSSECALL    hSSECall);


RVAPI
int RVCALLCONV sseCallTransfer(HSSECALL hPri,HSSECALL hSec, char* destAddress);

/*-------------------------------------------------------------------------
* sseGetConsultationCall
*
* Retrieve the handle of the consultation call from the handle of the
* transferred call.
* Sholud be called at the callback sseEvCallTransferSetup.
*
* Input:  hSSECallPri - handle of the transferred call (between TRDSE and TRTSE).
*
* Output: hSSEConsCall  - sse handle of consultation call (between TRGSE and TRTSE).
*                         NULL if call doesn't exists.
*         hSSEaConsCall - application handle of consultation call.
*                         NULL if call doesn't exists.
*
* Return: non negative value if success
*         negative value if an error occured.
*-------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseGetConsultationCall(IN  HSSECALL     hSSECallPri,
                                      OUT HSSECALL*    hSSEConsCall,
                                      OUT HSSEAPPCALL* hSSEaConsCall);


RVAPI
int RVCALLCONV sseCallAssociateForTransfer(HSSECALL hPri,HSSECALL hSec);


/*-----------------------------------------------------------------------------------------
 * sseForwardActivate
 *
 * This function is used for an existing call object to request a server to forward calls to a
 * specified forwarding destination.
 *
 * Input:   hSSECall -  The call object handle of SSE.
 *          destAddr -  Forwarding address.
 *          proc     -  Forward Procedure, possible values are:
 *                        cfu_p, cfb_p and cfnr_p.
 *          servAddr - The entity whose calls need to be forwarded.
 *          actAddr  - The entity that activates the call forward.
 *          serverAddr - The entity that causes the call forward, for example, a Gatekeeper.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
 *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseForwardActivate(IN    HSSECALL    hSSECall,
                IN  char*       destAddr,
                IN  proc_e      proc,
                IN  char*       servAddr,
                IN  char*       actAddr,
                IN  char*       serverAddr);
/*-----------------------------------------------------------------------------------------
 * sseForwardDeactivate
 *
 * This function is used for an existing call object to stop a server from forwarding calls to a
 * specified forwarding destination.
 *
 * Input:   hSSECall -  The call object handle of SSE.
 *          proc     -  Forward Procedure, possible values are:
 *                      cfu_p, cfb_p and cfnr_p.
 *          servAddr - The entity whose calls need to be forwarded.
 *          deactAddr  - The entity that deactivates the call forward.
 *          serverAddr - The entity that causes the call forward, for example, a Gatekeeper.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
 *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseForwardDeactivate(IN  HSSECALL    hSSECall,
                  IN    proc_e      proc,
                  IN    char*       servAddr,
                  IN    char*       deactAddr,
                  IN    char*       serverAddr);

/*-----------------------------------------------------------------------------------------
 * sseForwardInterogate:
 *
 * This function is used  to request an interogating end point to interogate
 * a destination end point
 *
 * Input:   hSSECall -  The call object handle of SSE.
 *          InterAddr -  the address of the interogating endpoint.
 *          proc     -  Forward Procedure, possible values are:
 *                        cfu_p, cfb_p and cfnr_p.
 *          servAddr - The entity whose calls need to be forwarded.
 *          serverAddr - The entity that causes the call forward, for example, a Gatekeeper.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
 *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseForwardInterogate(IN  HSSECALL    hSSECall,
                                    IN  char*       InterAddr,
                                    IN  proc_e      proc,
                                    IN  char*       servAddr,
                                    IN  char*       serverAddr);                                    

RVAPI
int RVCALLCONV sseCallReroute(IN    HSSECALL    hSSECall,
                IN  reason_e    reason,
                IN  char*       calledAddr,
                IN  char*       lastReroutingNr,
                IN  char*       callingNumber);

/*-----------------------------------------------------------------------------------------
 * sseCallImplementTransfer
 *
 * Passes a call handle when preparing the calls that will participate in the Transfer service.
 * A call can be an originating call, a served call or a rerouted call. 
 *
 * Input:   hSSECall -  The call object handle of SSE.
 *          primary  -  This parameter is obsolete and kept only for backwards compatability.
 *
 * Return: non negative value if success. 
 *         negative value if error occured.
 *-----------------------------------------------------------------------------------------*/
 RVAPI
 int RVCALLCONV sseCallImplementTransfer(IN HSSECALL    hSSECall,
                                         IN BOOL        primary);

RVAPI int RVCALLCONV sseSetDefaultAddress(
                            IN      HSSEAPP     hSSEApp,
                IN      char*   addr);

RVAPI HSSAPP RVCALLCONV sseGetSSHandle(
                            IN      HSSEAPP     hSSEApp);

/*--------------------------------------------------------------------------- */
/* sseCallImplementHold is called when a new call is created, to create the   */
/* HDSE service and relate it to the new call.                                */
/* Parameters: hSSECall - the sse handle to the sse call                      */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallImplementHold(IN    HSSECALL  hSSECall);


/*--------------------------------------------------------------------------- */
/* sseCallHoldInit is called when the user want to initiate the Hold service. */
/* parameters: hPri - handle to the sse call.                                 */
/*             near_hold - true if the user want to initiate Near Hold        */
/*                         false if the user want to initiate Remote Hold     */
/* After checking that the call is connected and that it is not already held, */
/* the ssServicePrimitive function is used to send the appropriate invoke     */
/* message, holdNotific or remoteHold according to the near_hold value.       */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallHoldInit(IN HSSECALL hPri,IN BOOL near_hold);


/*--------------------------------------------------------------------------- */
/* sseCallOfferInit is called when the user want to initiate the call Offer   */
/* service.                                                                   */
/* parameters: hPri            - handle to the sse call.                      */
/*             cfbOverride     - tells is the option is on.                   */
/* After checking that the call is connected the ssServicePrimitive function  */
/* is used to send the callOfferRequesr invoke message.                       */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallOfferInit(IN HSSECALL hPri, IN BOOL cfbOverride);

/*-----------------------------------------------------------------------------*/
/* sseRemoteUserAlerting is called when user B made free resources avialible   */
/* for a call between him and usr A. Now a remoteUserAlerting invoke message   */
/* should be sent.                                                             */
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */
/*-----------------------------------------------------------------------------*/

RVAPI
int RVCALLCONV sseRemoteUserAlerting(IN HSSECALL hPri);

/*------------------------------------------------------------------------------*/
/* sseCallOfferRetrieve - user B accepts waiting call by sending C connect,     */
/* after he freed resources.                                                    */
/* user B close the CO service by calling ssServiceClose.                       */
/* parameters: HSSECALL hPri - the sse handle to the sse call                   */
/*------------------------------------------------------------------------------*/

RVAPI
int RVCALLCONV sseCallOfferRetrieve(IN HSSECALL hPri);
/*--------------------------------------------------------------------------- */
/* sseCallHoldRetrieve is called when the holding user wants to terminate the */
/* hold service.                                                              */
/* parameters: hPri - handle to the sse call.                                 */
/*             near_hold_ret - true if the service is Near Hold               */
/*                             false if the service is Remote Hold            */
/* After checking that the call is in connect state and that the user willing */
/* to terminate the call isn't the held user, ssServicePrimitive is called to */
/* send holdRetrieve invoke or remoteRetrieve invoke according to             */
/* near_hold_ret value.                                                       */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallHoldRetrieve(IN HSSECALL hPri,IN BOOL near_hold_ret);


/*--------------------------------------------------------------------------- */
/* sseHoldSendNonDefaultResponse is called after the application's response   */
/* to remoteHold or remoteRetrieve was a none default response, inorder to    */
/* send the response Apdu to the holding side, that sent the invoke.          */
/* parametrs: hPri - handle to the sse call.                                  */
/*            ok - true if the response is ack, false if it is nack           */
/*            errCode - the value of error Code if it is a returnError message*/
/*                      if it is a returnResult the errCode has to be -1.     */
/* The function just calls the ssServicePrimitive to send the appropriate Apdu*/
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseHoldSendNonDefaultResponse(IN HSSECALL hPri,
                                           IN BOOL ok,
                                           IN int errCode);

/*--------------------------------------------------------------------------- */
/* sseCallImplementParkPickup is called when a new call is created, to create */
/* the prkd,prkt and gpmem services and relate it to the new call.            */
/* Parameters: hSSECall - the sse handle to the sse call                      */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallImplementParkPickup(IN    HSSECALL  hSSECall);

/*-----------------------------------------------------------------------------*/
/* sseCallParkInit is called when user A want to park it's call with B at      */
/* endpoint C.                                                                 */
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */
/* char*    parkingNumber                                                      */
/* char*    parkedNumber                                                       */
/* char*    parkedToNumber                                                     */
/* int      parkedToPosition  could be one of :parkedToUserIdle (1),           */
/*                                             parkedToUserBusy (2),           */
/*                                             parkedToGroup    (3),              */
/*-----------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallParkInit(IN HSSECALL hPri,
                               IN char*    parkingNumber,
                               IN char*    parkedNumber,
                               IN char*    parkedToNumber,
                               IN int      parkedToPosition);
/*-----------------------------------------------------------------------------*/
/* sseCallAlertParkInt is called when user replay a setup message with alert   */
/* and is willing to let the call be picked-up. this function changr the the   */
/* state in the state machine o fprkt service so it will be ready for pickup.  */                                                                
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */                               
/*-----------------------------------------------------------------------------*/

RVAPI
int RVCALLCONV sseCallAlertParkInt(IN HSSECALL hPri);
/*-----------------------------------------------------------------------------*/
/* sseCallGroupIndOn is called when a call is parked , the park-tp endpoind    */
/* send indicatin of the parking call to all group members (one member at a    */
/* time)                                                                       */                                                                      
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */
/*                             char callPickupId[16]                           */
/*                             char *       groupMemberUserNr                  */
/*                             char *       picking_upNumber                   */
/*                             char *       retrieveAddress                    */
/*                             char *       partyToRetrieve                    */
/*                             cpCallType   retrieveCallType                   */
/*                             int          parkPosition                       */
/*-----------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallGroupIndOn(IN HSSECALL   hPri,
                                  IN char        callPickupId[16],
                                  IN char *      groupMemberUserNr,
                                  IN char *      partyToRetrieve,
                                  IN char *      retrieveAddress,
                                  IN cpCallType  retrieveCallType,
                                  IN int         parkPosition);
/*-----------------------------------------------------------------------------*/
/* sseCallGroupIndOff is called when a call is picked-up , the park-tp endpoind*/
/* send indicatin that the parked call was picked-up to all group members (one */
/* member at a time)                                                           */                                                                      
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */
/*                             char callPickupId[16]                           */
/*                             char *       groupMemberUserNr                  */
/*-----------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallGroupIndOff(IN HSSECALL   hPri,
                                  IN char        callPickupId[16],
                                  IN char *      groupMemberUserNr);
/*-----------------------------------------------------------------------------*/
/* sseCallPickupInit is called when a user wants to pick up a parked call (or  */
/* alerted).                                                                   */
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */
/* char callPickupId[16]                                                       */
/* char * picking_upNumber                                                     */
/* char * partyToRetrieve                                                      */
/* char * retrieveAddress                                                      */
/* int    parkPosition                                                         */
/*-----------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallPickupInit(IN HSSECALL hPri,
                                 IN char callPickupId[16],
                                 IN char * picking_upNumber,
                                 IN char * partyToRetrieve,
                                 IN char * retrieveAddress,
                                 IN int    parkPosition);
                                 
/*-----------------------------------------------------------------------------*/
/* sseCallLocalPickupInit is called when a user wants to pick up a parked call */
/* (or alerted).                                                               */
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */
/* char callPickupId[16]                                                       */
/* char * picking_upNumber                                                     */
/*-----------------------------------------------------------------------------*/

RVAPI
int RVCALLCONV sseCallLocalPickupInit(IN HSSECALL hPri,
                                 IN char callPickupId[16],
                                 IN char * picking_upNumber);
/*--------------------------------------------------------------------------- */
/* sseCallImplementWait is called when a new call is created, to create the   */
/* WDSE service and relate it to the new call.                                */
/* Parameters: hSSECall - the sse handle to the sse call                      */
/*--------------------------------------------------------------------------- */

RVAPI
int RVCALLCONV sseCallImplementWait(IN    HSSECALL  hSSECall);

/*-----------------------------------------------------------------------------*/
/* sseCallWaitInit is called when user B, whom is busy, recieved setup message */
/* from user C and wants to initiate SS-Wait, which means put user C on wait.  */
/* parameters: HSSECALL hPri - the sse handle to the sse call                  */
/*-----------------------------------------------------------------------------*/

RVAPI
int RVCALLCONV sseCallWaitInit(IN HSSECALL hPri);

/*------------------------------------------------------------------------------*/
/* sseCallWaitRetrieve - user B accepts waiting call by sending C connect,      */
/* after he freed resources.                                                    */
/* user B close the WGSE service by calling ssServiceClose.                     */
/* parameters: HSSECALL hPri - the sse handle to the sse call                   */
/*------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallWaitRetrieve(IN HSSECALL hPri);

/*------------------------------------------------------------------------------*/
/* sseCallImplementMessageWaitIndServed - is called when a new call is created, */
/* to create the MWISER service and relate it to the new call.                  */
/* Parameters: hSSECall - the sse handle to the sse call                        */
/*------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallImplementMWIServed(IN    HSSECALL hSSECall);

/*------------------------------------------------------------------------------*/
/* sseCallImplementMessageWaitIndMsg - is called when a new call is created,    */
/* to create the MWIMC service and relate it to the new call.                   */
/* Parameters: hSSECall - the sse handle to the sse call                        */
/*------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallImplementMWIMsgCtr(IN    HSSECALL hSSECall);


/*-----------------------------------------------------------------------------------*/
/* sseMWIActivate - the function is called when the message centre wants to indicate */
/* the served user that messages intended for that user are available.               */
/*                                                                                   */
/* parameters: hPri - the sse handle to the sse call                                 */
/*             serverUserAddr - the user address to whom message will be sent        */
/*             service - basic service of the messages for the served user           */
/*                       (fax, email,voicemail, etc)                                 */
/*             MsgCtrId - uniqe identification of the message centre that            */
/*                        initiate the service.                                      */
/*                        the struct MsgCtrIdT contains 4 fields: NUMBER,PN,NUMSTR , */
/*                        one of first 3 has to be assigned, and the                 */
/*                        fourth field, choice, implys which of them was assigned.   */
/*                        This is an OPTIONAL field,if the choice fiels is NULL the  */
/*                        parameter is ignored                                       */
/*             nbOfMsgs - how many messages are available for the served user.       */
/*                        This is an OPTIONAL field,if <0 the parameter is ignored   */
/*             origAddr - The address of the user that left the message at the       */
/*                        message centre for the served user.                        */
/*                        This is an OPTIONAL field,if NULL the parameter is ignored */
/*             time     - time stamp of the message.                                 */
/*                        This is an OPTIONAL field,if NULL the parameter is ignored */
/*             priority_val - the priority of the highest priority message waiting.  */
/*                        0 means the highest priority and 9 the lowest.             */
/*                        This is an OPTIONAL field,if <0 the parameter is ignored   */
/*-----------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseMWIActivate(IN HSSECALL     hPri,
                            IN char*        serverUserAddr,
                            IN char *       service,
                            IN MsgCtrIdT*   MsgCtrId, /* int / partyNumber / numericString */
                            IN int          nbOfMsgs,
                            IN char *       origAddr,
                            IN char *       time,  /* format of ISO 8601 ???? */
                            IN int          priority_val);

/*-----------------------------------------------------------------------------------*/
/* sseMWIActivateCallBack - the function is called by message centre to use the      */
/* mwiActivate invoke apdu to signal a callback request to the served user.          */
/* in this case:  nbOfMessages = 0                                                   */
/*                msgCentreId = endpoint address destination for the callback.       */
/* parameters: hPri - the sse handle to the sse call                                 */
/*             serverUserAddr - the user address to whom message will be sent        */
/*             service - basic service of the messages for the served user           */
/*                       (fax, email,voicemail, etc)                                 */
/*             destAddr - endpoint address destination for the callback.             */
/*                        This is an OPTIONAL field,if NULL the parameter is ignored */
/*             origAddr - The address of the user that left the message at the       */
/*                        message centre for the served user.                        */
/*                        This is an OPTIONAL field,if NULL the parameter is ignored */
/*             time     - time stamp of the message.                                 */
/*                        This is an OPTIONAL field,if NULL the parameter is ignored */
/*             priority - the priority of the highest priority message waiting.      */
/*                        0 means the highest priority and 9 the lowest              */
/*                        This is an OPTIONAL field,if <0 the parameter is ignored   */
/*-----------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseMWIActivateCallBack(IN HSSECALL     hPri,
                                    IN char*        serverUserAddr,
                                    IN char *       service,
                                    IN char *       destAddr, /* endpoint address destination for call back */
                                    IN char *       origAddr,
                                    IN char *       time,    /* format of ISO 8601 ???? */
                                    IN int          priority_val);


/*-----------------------------------------------------------------------------------*/
/* sseMWIDeactivate - this function is called by the message centre inorder to cancel*/
/* the MWI at the served user, concerned the basic service supplyed by the message   */
/* centre in the Deactivation message.                                               */
/* parameters: hPri - the sse handle to the sse call                                 */
/*             serverUserAddr - the user address to whom message will be sent        */
/*             service - basic service of the messages for the served user           */
/*                       (fax, email,voicemail, etc)                                 */
/*             MsgCtrId - uniqe identification of the message centre that            */
/*                        initiate the service.                                      */
/*                        the struct MsgCtrIdT contains 4 fields: MUNBER,PN,NUMSTR , */
/*                        one of first 3 has to be assigned, and the                 */
/*                        fourth field, choice, implys which of them was assigned.   */
/*                        This is an OPTIONAL field,if the choice fiels is NULL the  */
/*                        parameter is ignored                                       */
/*          callBackReq - true: only matching MWI activations for callbacks shall be */
/*                              deactivated.                                         */
/*                        false: MWI activations which are not for callbacks shall be*/
/*                              deactivated.                                         */
/*          is_callback - whether to fill the callBackReq at mwiDeactivation or not. */
/*-----------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseMWIDeactivate(IN HSSECALL   hPri,
                              IN char *     serverUserAddr,
                              IN char *     service,
                              IN MsgCtrIdT* MsgCtrId, /* int / partyNumber / numericString */
                              IN BOOL       callBackReq,
                              IN BOOL       is_callback
                              );

RVAPI
/*---------------------------------------------------------------------------------- */
/* sseMWIInterrogate - this function is called when the served user request to query */
/* message centre known to him for the MWI activatinos currently apllied to it.      */
/* A typical usage of this mechanism is by a served user to recreate its MWI status  */
/* when the endpoint is returned to service.                                         */
/* parameters: hPri - the sse handle to the sse call                                 */
/*             serverUserAddr - the user address to whom message will be sent        */
/*             service - basic service of the messages for the served user           */
/*                       (fax, email,voicemail, etc)                                 */
/*             MsgCtrId - uniqe identification of the message centre that            */
/*                        initiate the service.                                      */
/*                        the struct MsgCtrIdT contains 4 fields: MUNBER,PN,NUMSTR , */
/*                        one of first 3 has to be assigned, and the                 */
/*                        fourth field, choice, implys which of them was assigned.   */
/*                        This is an OPTIONAL field,if the choice fiels is NULL the  */
/*                        parameter is ignored                                       */
/*          callBackReq - true: : limit results for activations which are callback   */
/*                                 requests.                                         */
/*                        false: limit results for activations which are not         */
/*                               callback requests.                                  */
/*          is_callback - whether to fill the callBackReq at mwiDeactivation or not  */
/*------------------------------------------------------------------------------*/
int RVCALLCONV sseMWIInterrogate(IN HSSECALL   hPri,
                              IN char *     serverUserAddr,
                              IN char *     service,
                              IN MsgCtrIdT* MsgCtrId, /* int / partyNumber / numericString */
                              IN BOOL       callBackReq,
                              IN BOOL       is_callback
                              );

/* -----------------------------------------------------------------------------------------*/
/* sseMWIGetNumOfResultElements - this function recieves the nodeId of MWIInterrogateResElt */
/* of the mwiInterrogate.returnResult message, and returns the number of result elements    */
/* that this returnResult message includes.                                                 */
/* parameters: hPri - the sse handle to the sse call                                        */
/*             resultEltsNodeId - the nodeId of MWIInterrogateResElt from the               */
/*                                mwiInterrogate.returnResult message.                      */
/*             numOfElements - the number of result elements.                               */
/* -----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseMWIGetNumOfResultElements(IN HSSECALL   hPri,
                                          IN int resultEltsNodeId,
                                          OUT int *numOfElements);


/* -------------------------------------------------------------------------------------*/
/* sseMWIGetResultElements - this function get the first numOfElements result elements  */
/* that appears in the nodeId resultEltsNodeId and fill them in the array elements, in  */
/* entries 0 - numOfElements-1.                                                         */
/* parameters: hPri - the sse handle to the sse call                                    */
/*             resultEltsNodeId - the nodeId of MWIInterrogateResElt from the           */
/*                                mwiInterrogate.returnResult message.                  */
/*             numOfElements - the number of result elements to get.                    */
/*             elements - this array is allocated by the user, of size > = numOfElements*/
/*                        the result elements are filled in the array in places         */
/*                        0 - numOfElements-1.                                          */
/* -------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseMWIGetResultElements(IN HSSECALL   hPri,
                                     IN UINT32 resultEltsNodeId,
                                     IN int numOfElements,
                                     IN OUT interResT elements[]);

/* -------------------------------------------------------------------------------------*/
/* sseMWIGetResultElt - get one result element from the nodeId elementNodeId to the     */
/*                      element of type interResT.                                      */
/* parameters: hPri - the sse handle to the sse call                                    */
/*             elementNodeId - the nodeId of the result element to convert to struct    */
/*                             interResT.                                               */
/*             element - allocate by the user. The function fill it according the values*/
/*                       valTree of elementNodeId                                       */
/* -------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseMWIGetResultElt(IN HSSECALL   hPri,
                                IN int elementNodeId,
                                IN OUT interResT* element);



/*--------------------------------------------------------------------------- */
/* sseMWISendNonDefaultResponse is called after the application's response    */
/* to mwiActivate, mwiDeactivate or mwiInterrogate was a non default response,*/
/* inorder to send the response Apdu to the other side, that sent the invoke. */
/* parametrs: hPri - handle to the sse call.                                  */
/*            ok - true if the response is ack, false if it is nack           */
/*            errCode - the value of error Code if it is a returnError message*/
/*                            numOfElements must be >0.                       */
/*            interResult - an array with elements  of type interResT to send */
/*                          in the returnResult.                              */
/*                          the service field must be one of the BasicService */
/*                          defined in h450.7                                 */
/*            errCode - the value of error Code if it is a returnError message*/
/*                      if it is a returnResult the errCode has to be -1.     */
/* The function just calls the ssServicePrimitive to send the appropriate Apdu*/
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseMWISendNonDefaultResponse(IN HSSECALL  hPri,
                                            IN BOOL      ok,
                                            IN int       numOfElements,
                                            IN interResT interResult[],
                                            IN int       errCode);




/********************************************************************************************
 *
 *                  H450.9: Call Completion
 *
 ********************************************************************************************/

/* H450.9 CallCompletion: Type of connection retention supported by UserA */
typedef enum
{
    ccRetentionDontCare,
    ccRetainConnection,
    ccDontRetainConnection
} ccRetentionType;

/* H450.9 CallCompletion: Indication about the state of a service */
typedef enum
{
    ccActivated,        /* User A has received ccXXRequest.res, and the service
                           is now up and running */
    ccSuspended,        /* User A has suspended the service - it's probably busy */
    ccResume,           /* User A isn't busy anymore - service can be resumed by an
                           additional call to sseCallCompletionExecPossible() */
    ccRingout,          /* User B has received ccRingout.inv, and should no connect
                           this call to finish the service */
    ccRejected,         /* Cannot execute the CallCompletion service */
    ccTimeOut,          /* Service timed-out */
    ccCallDropped       /* Call was dropped by one of the sides, and the application
                           is responsible for creating and managing calls for new
                           messages it wants to send on this CallCompletion service. */
} ccIndication;


/************************************************************************
 * sseEvCCFindServiceT
 * purpose: Callback function that is called when the stack receives a
 *          CallCompletion (H450.9)	message that it can't find a relevant
 *          service for.
 *			This is the case whenever the call signalling connection is not retained
 *          or when a ccRingout.inv message is received.
 * input  : hSSEaCall   - The application's handle of the SSE call
 *          hSSECall    - The SSE handle of the SSE call
 *          hSSEaService- SSE Application's handle of the service, that the stack thinks
 *					      is the right one.
 *						  NULL if stack couldn't find any service that matches
 *          hSSEService - SSE Service of CallCompletion, that the stack thinks is the right
 *						  one (can be overridden by the application)
 *						  NULL if stack couldn't find any service that matches
 *			argNodeId	- Argument nodeId, holding the information received in the
 *					      H450.9 message.
 * output : hSSEService - SSE Service this message belongs to
 *						  If set to NULL by the application, then this message won't
 *						  be processed any further and the service will be canceled.
 * return : none
 ************************************************************************/
typedef void
    (RVCALLCONV *sseEvCCFindServiceT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        INOUT   HSSESERVICE*    hSSEService,
		IN		int				argNodeId);


/************************************************************************
 * sseEvCCRequestIndT
 * purpose: Callback function that is called when the stack receives a
 *          CallCompletion (H450.9) request.
 * input  : hSSEaCall       - The application's handle of the SSE call
 *          hSSECall        - The SSE handle of the SSE call
 *          hSSEaService    - SSE Application's handle of the service
 *          hSSEService     - SSE Service of CallCompletion
 *          isBusy          - TRUE if ccbsRequest was received
 *                            FALSE if ccnrRequest was received
 *          retainService   - TRUE if user A can retain the service, FALSE if it can't
 *          retentionType   - Retention type chosen by user A
 * output : retainService   - Application can set this value to TRUE or FALSE if it was
 *                            TRUE, meaning that user B chooses to retain the service or
 *                            not. If *retainService was given by SSE as FALSE, this
 *                            parameter is discarded and service retention is not supported.
 *          retainConnection- TRUE if application wants to retain the signaling connection
 *                            FALSE if it doesn't
 * return : ssDefaultAck    - To activate the service
 *          ssDefaultNack   - To reject the service
 *          ssWait          - If the user wants to answer later on
 *                            In this case, it should call sseCancelCallCompletion()
 *                            if it decided not to handle this service at all
 ************************************************************************/
typedef int
    (RVCALLCONV *sseEvCCRequestIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        IN      BOOL            isBusy,
        IN      ccRetentionType retentionType,
        INOUT   BOOL*           retainService,
        OUT     BOOL*           retainConnection);


/************************************************************************
 * sseEvCCExecPossibleIndT
 * purpose: Callback function that is called when User A receives an
 *          ExecPossible.inv message from User B on a CallCompletion service.
 *          This means that User A can now complete the call.
 * input  : hSSEaCall   - The application's handle of the SSE call
 *          hSSECall    - The SSE handle of the SSE call
 *          hSSEaService- SSE Application's handle of the service
 *          hSSEService - SSE Service of CallCompletion
 * output : isBusy      - TRUE if User A is currently busy
 *                        FALSE if service can be completed since User A isn't busy
 * return : Non-negative value on success. In this case, the user has to
 *          call sseCallCompletionRingout() if it indicated that the user isn't busy. If
 *          the user isn't busy, this will cause the stack to automatically send out
 *          ccSuspend.inv message for this service, and the user will have to call
 *          sseCallCompletionResume() when he's no longer busy.
 *          Negative value on failure
 ************************************************************************/
typedef int
    (RVCALLCONV *sseEvCCExecPossibleIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        OUT     BOOL*           isBusy);


/************************************************************************
 * sseEvCCStageIndT
 * purpose: Callback function that is called for different stages of the
 *          call completion service. This function should be implemented by
 *          both User A and User B of this service, since some of the stages
 *          can occur in both sides.
 * input  : hSSEaCall   - The application's handle of the SSE call
 *                        Can be NULL if timeout
 *          hSSECall    - The SSE handle of the SSE call
 *          hSSEaService- SSE Application's handle of the service
 *          hSSEService - SSE Service of CallCompletion
 *          indication  - The indication fo the stage that we're in
 *                        If this is ccTimeOut, then hSSEaCall and hSSECall
 *                        may be NULL, if no signaling connection is retained
 *                        for this service
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
typedef int
    (RVCALLCONV *sseEvCCStageIndT)(
        IN      HSSEAPPCALL     hSSEaCall,
        IN      HSSECALL        hSSECall,
        IN      HSSEAPPSERVICE  hSSEaService,
        IN      HSSESERVICE     hSSEService,
        IN      ccIndication    indication);


/************************************************************************
 * sseCallImplementCompletion
 * purpose: Should be called when a new call is created, in order to create
 *          the CC service and relate it to the new call. This will allow
 *          this call to handle Call-Completion related messages when this
 *          call didn't invoke them.
 * input  : hSSECall    - The SSE handle of the SSE call
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV sseCallImplementCompletion(IN HSSECALL hSSECall);


/************************************************************************
 * sseCallCompletion
 * purpose: Implement a Call Completion procedure
 * input  : hSSECall            - The SSE handle of the SSE call
 *          hSSEaService        - The application's handle for the SSE service. This
 *                                will be returned on every callback regarding this service
 *          srcAddress          - Source address of the call (User A)
 *          destAddress         - Destination address for call completion procedure (User B)
 *          hOriginalCall       - If not NULL, it holds the original call which was
 *                                released due to busy indication or no response
 *          service             - basic service of the messages for the served user
 *                                (fax, email,voicemail, etc)
 *          isBusy              - TRUE: implements CCBS (Call Completion on Busy)
 *                                FALSE: implements CCNR (Call Completion on no response)
 *          canRetainService    - TRUE if user A supports service retention, FALSE if
 *                                it doesn't
 *          ratainSigConnection - ccRetainConnection if the application wants to retain the connection
 *                                ccDontRetainConnection if it doesn't want to retain the connection
 *                                ccRetentionDontCare if the application doesn't care
 * output : none
 * return : Service handle on success - can be used to cancel this
 *          procedure later on or to distinguish the incoming events on
 *          several such procedures if necessary
 *          NULL on failure
 ************************************************************************/
RVAPI
HSSESERVICE RVCALLCONV sseCallCompletion(
    IN HSSECALL         hSSECall,
    IN HSSEAPPSERVICE   hSSEaService,
    IN const char*      srcAddress,
    IN const char*      destAddress,
    IN HCALL            hOriginalCall,
    IN const char*      service,
    IN BOOL             isBusy,
    IN BOOL             canRetainService,
    IN ccRetentionType  retainSigConnection);


/************************************************************************
 * sseCallCompletionExecPossible
 * purpose: Should be called by User B when he is no longer considered
 *          as busy.
 * input  : hSSEService - SSE service to use
 *          hSSECall    - SSE call to use
 *                        For a service that retained the signalling connection,
 *                        this parameter should be set to NULL. Otherwise, it
 *                        should be linked to a new call that already has
 *                        the destination addresses for the call.
 *          srcAddress  - Source address of this call (User B)
 *                        Should be set to NULL if hSSECall!=NULL (i.e connection is retained)
 *          destAddress - Destination address for call completion procedure (User A)
 *                        Should be set to NULL if hSSECall!=NULL (i.e connection is retained)
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV sseCallCompletionExecPossible(
    IN HSSESERVICE  hSSEService,
    IN HSSECALL     hSSECall,
    IN const char*  srcAddress,
    IN const char*  destAddress);


/************************************************************************
 * sseCallCompletionRingout
 * purpose: Should be called by User A, after receiving
 *          sseEvCCExecPossibleInd(). This causes the call to complete
 *          by calling cmCallMake() and sending a ccRingout.inv primitive
 *          to User B.
 * input  : hSSEService - SSE service to use
 *          hSSECall    - SSE call to use
 *                        This should be a newly created call that is the
 *                        call the application is trying to complete.
 *          srcAddress  - Source address of this call (User A)
 *          destAddress - Destination address for call completion procedure (User B)
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV sseCallCompletionRingout(
    IN HSSESERVICE  hSSEService,
    IN HSSECALL     hSSECall,
    IN const char*  srcAddress,
    IN const char*  destAddress);


/************************************************************************
 * sseCallCompletionResume
 * purpose: Should be called by User A, if he notified that he was busy
 *          when he received sseEvCCExecPossibleInd().
 *          This function tells User B that User A is no longer busy and
 *          that he can try to send ccExecPossible.inv again.
 * input  : hSSEService - SSE service to use
 *          hSSECall    - SSE call to use
 *                        For a service that retained the signalling connection,
 *                        this parameter should be set to NULL. Otherwise, it
 *                        should be linked to a new call that already has
 *                        the destination addresses for the call.
 *          srcAddress  - Source address of this call (User A)
 *                        Should be set to NULL if hSSECall!=NULL (i.e connection is retained)
 *          destAddress - Destination address for call completion procedure (User B)
 *                        Should be set to NULL if hSSECall!=NULL (i.e connection is retained)
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV sseCallCompletionResume(
    IN HSSESERVICE  hSSEService,
    IN HSSECALL     hSSECall,
    IN const char*  srcAddress,
    IN const char*  destAddress);


/************************************************************************
 * sseCallCompletionCancel
 * purpose: Can be called by any of the sides of the service to cancel
 *          the service. The application receives sseEvServiceHandleIdle()
 *          when the service is no longer valid.
 * input  : hSSEService - SSE service to use
 *          hSSECall    - SSE call to use
 *                        For a service that retained the signalling connection,
 *                        this parameter should be set to NULL. Otherwise, it
 *                        should be linked to a new call that already has
 *                        the destination addresses for the call.
 *          srcAddress  - Source address of this call
 *                        Should be set to NULL if hSSECall!=NULL (i.e connection is retained)
 *          destAddress - Destination address for call completion procedure
 *                        Should be set to NULL if hSSECall!=NULL (i.e connection is retained)
 * output : none
 * return : Non-negative value on success
 *          Negative value on failure
 ************************************************************************/
RVAPI
int RVCALLCONV sseCallCompletionCancel(
    IN HSSESERVICE  hSSEService,
    IN HSSECALL     hSSECall,
    IN const char*  srcAddress,
    IN const char*  destAddress);







/*------------------------------------------------------------------------------*/
/* sseCallImplementIntrusion - is called when a new call is created, in order   */
/* create the CI service and relate it to the new call.                         */
/* Parameters: hSSECall - the sse handle to the sse call                        */
/*------------------------------------------------------------------------------*/

RVAPI
int RVCALLCONV sseCallImplementIntrusion(IN    HSSECALL	hSSECall);


/*--------------------------------------------------------------------------- */
/* sseCallIntrusionInit is called when the user want to initiate the Intrusion*/
/* service. parameters:                                                       */
/*             hPri          - handle to the sse call.                        */
/*             IntrusionType - enum for the type of intrusion. can be -       */
/*                             ssCallIntrusionRequest,ssCISilentMoitor or     */
/*                             ssCIForcedRelease                              */
/*             CallIdentifier- for silent monitor.                            */
/* a setup message should be sent with the apdu.                              */
/* the ssServicePrimitive function is used to send the appropriate invoke     */
/* message.                                                                   */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallIntrusionInit(IN HSSECALL hPri,IN ciPrimitivesEnum IntrusionType,
                                    IN char CallIdentifier[16]);


/*--------------------------------------------------------------------------- */
/* sseCallIntrusion2 is called when the user want to initiate the Intrusion   */ 
/* service. parameters:                                                       */
/*             hPri          - handle to the sse call.                        */
/*             IntrusionType - enum for the type of intrusion. can be -       */
/*                             ssCIIsolated,ssCIForcedRelease,ssCIWOBRequest  */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallIntrusion2(IN HSSECALL hPri,
                                 IN ciPrimitivesEnum IntrusionType);


/*------------------------------------------------------------------------------*/
/* sseCallImplementCallOffer - is called when a new call is created, in order   */
/* create the CO service and relate it to the new call.                         */
/* Parameters: hSSECall - the sse handle to the sse call                        */
/*------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallImplementCallOffer(IN    HSSECALL hSSECall);


/*--------------------------------------------------------------------------- */
/* sseCallImplementNI is called when a new call is created, to create the     */
/* NIE service and relate it to the new call.                                 */
/* Parameters: hSSECall - the sse handle to the sse call                      */
/*--------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseCallImplementNI(IN    HSSECALL    hSSECall);


/*----------------------------------------------------------------------------------------- */
/* sseNISend called to send Q931 message with Name identification invoke   .                */
/* parametes:                                                                               */
/* hPri - the sse handle to the sse call                                                    */
/* operation - which primitive to send: callingName, alertingName, connectedName or busyName*/
/* simple_name - octet string in size =<50. if choice is different from notAvailable then   */
/*               must be not empty.                                                         */
/* character_set - one of: unknown, iso8859-1, t-61, iso8859-2, iso8859-3, iso8859-4,       */
/*                         iso8859-5, iso8859-7.                                            */
/* choice_name - one of: allowed, restricted, notAvailable                                  */
/* restricted_null - true in case of interworking where other network provides indication   */
/*                   that the name is restricted without the name itself                    */
/*                   this parameter has meaning only when choice_name is "restricted".      */
/*----------------------------------------------------------------------------------------- */
RVAPI
int RVCALLCONV sseNISend(IN HSSECALL     hPri,
                       IN niOperation  choice,
                       IN char         simple_name[],
                       IN char         character_set[],
                       IN niNameChoice choice_name,
                       IN BOOL         restricted_null);


/*--------------------------------------------------------------------------------------*/
/* sseNIServiceClose - close the hSSNameIdentify (NIE) service of the specific sseCall. */
/* hSSECall:  the sse handle to the sse call .                                          */
/*--------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseNIServiceClose(IN HSSECALL        hSSECall);




typedef  struct
{
    sseEvForwardActivatedT          sseEvForwardActivated;
    sseEvForwardDeactivatedT        sseEvForwardDeactivated;
    sseEvCallTransferT              sseEvCallTransfer;
    sseEvCallRerouteT               sseEvCallReroute;
    sseEvRemoteHoldIndT             sseEvRemoteHoldInd;
    sseEvRemoteHoldRetrieveIndT     sseEvRemoteHoldRetrieveInd;
    sseEvRemoteHoldConfirmT         sseEvRemoteHoldConfirm;
    sseEvRemoteRetrieveConfirmT     sseEvRemoteRetrieveConfirm;
    sseEvNearHoldIndT               sseEvNearHoldInd;
    sseEvNearHoldRetrieveIndT       sseEvNearHoldRetrieveInd;
    sseEvWaitIndT                   sseEvWaitInd;
    sseEvWaitTimerExpiredT          sseEvWaitTimerExpired;
    sseEvMWIActivateIndT            sseEvMWIActivateInd;
    sseEvMWIActivateConfirmT        sseEvMWIActivateConfirm;
    sseEvMWIDeactivateIndT          sseEvMWIDeactivateInd;
    sseEvMWIDeactivateConfirmT      sseEvMWIDeactivateConfirm;
    sseEvMWIInterrogateIndT         sseEvMWIInterrogateInd;
    sseEvMWIInterrogateConfirmT     sseEvMWIInterrogateConfirm;
    sseEvNICallingNameIndT          sseEvNICallingNameInd;
    sseEvNIAlertingingNameIndT      sseEvNIAlertingNameInd;
    sseEvNIConnectedNameIndT        sseEvNIConnectedNameInd;
    sseEvNIBusyNameIndT             sseEvNIBusyNameInd;
    sseEvCallTransferSetupT         sseEvCallTransferSetup;
    sseEvCfbOverrideIndT            sseEvCfbOverrideInd;
	sseEvCCFindServiceT				sseEvCCFindService;
    sseEvCCRequestIndT              sseEvCCRequestInd;
    sseEvCCExecPossibleIndT         sseEvCCExecPossibleInd;
    sseEvCCStageIndT                sseEvCCStageInd;
    sseEvCOCompleteT                sseEvCOComplete;
    sseEvCIInitialRequestIndT       sseEvCIInitialRequestInd;
    sseEvCIIsolatedIndT             sseEvCIIsolatedInd;
    sseEvCIForcedReleaseIndT        sseEvCIForcedReleaseInd;
    sseEvCIWOBIndT                  sseEvCIWOBInd;
	sseEvCINotificationIndT         sseEvCINotificationInd;
	sseEvCIRequestConfirmT          sseEvCIRequestConfirm;
    sseEvCIIsolatedConfirmT         sseEvCIIsolatedConfirm;
    sseEvCIForcedReleaseConfirmT    sseEvCIForcedReleaseConfirm;
    sseEvCIWOBRequestConfirmT       sseEvCIWOBRequestConfirm;
    sseEvCISilentMonitorConfirmT    sseEvCISilentMonitorConfirm;
    sseEvCIMakeSilentIndT           sseEvCIMakeSilentInd;
    sseEvCIFindServiceT             sseEvCIFindService;
    sseEvCPSetupIndT                sseEvCPSetupInd;
    sseEvCPickExeIndT               sseEvCPickExeInd;
    sseEvCParkRequestIndT           sseEvCParkRequestInd;
    sseEvCPickupRequestIndT         sseEvCPickupRequestInd;
    sseEvCPickupFindCallT           sseEvCPickupFindCall;
    sseEvCPickupIndT                sseEvCPickupInd;
    sseEvCPickNotifyIndT            sseEvCPickNotifyInd;
    sseEvCPNotifyIndT               sseEvCPNotifyInd;
    sseEvCPGroupIndicationT         sseEvCPGroupIndication;
    sseEvCPRequestConfirmT          sseEvCPRequestConfirm;
    sseEvCPPickRequConfirmT         sseEvCPPickRequConfirm;
    sseEvCPPickupConfirmT           sseEvCPPickupConfirm;
    sseEvCPSetupConfirmT            sseEvCPSetupConfirm;
    sseEvCPGroupIndicationConfirmT  sseEvCPGroupIndicationConfirm;
    sseEvRemoteUserAlertingIndT     sseEvRemoteUserAlertingInd;  

    sseEvServiceHandleCreatedT      sseEvServiceHandleCreated;
    sseEvServiceHandleIdleT         sseEvServiceHandleIdle;
} SSSECALLEVENT,*SSECALLEVENT;




/********************************************************************************************
 *
 *                  General public functions
 *
 ********************************************************************************************/


/*-----------------------------------------------------------------------------------------
 * sseInit
 *
 * Initializes an SSE instance and exchanges application object handles.
 *
 * Input:  name     - Location of the Stack Configuration parameters. This can be the path of
 *                    the Stack Configuration file, or the name of a buffer where it is stored
 *                    or a place in the registry.
 *         hSSEaApp - The application object handle of the application.
 *         hApp     - The application handle that cmInitialize returns to the application.
 *         maxCalls - Maximum number of calls that this instance of SSE can support.
 *
 * Output: hSSEApp  - The pointer to the application object handle of SSE.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseInit(
              IN      char*         name,
              OUT     HSSEAPP*      hSSEApp,
              IN      HSSEAPPAPP    hSSEaApp,
              IN      HAPP          hApp,
              IN      int           maxCalls);


/*-----------------------------------------------------------------------------------------
 * sseEnd
 *
 * terminate the SSE instance.
 * Input:  hSSEApp  - The pointer to the application object handle of SSE.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseEnd( IN       HSSEAPP    hSSEApp);


/*-----------------------------------------------------------------------------------------
 * sseSetCallEventHandler
 *
 * Installs the call event handler.
 *
 * Input:   hSSEApp  - The pointer to the application object handle of SSE.
 *          sseCallEvent - Call event handler structure.
 *          size - The size of the call event handler structure.
 *
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseSetCallEventHandler(
                            IN      HSSEAPP      hSSEApp,
                            IN      SSECALLEVENT sseCallEvent,
                            IN      int          size);

/*-----------------------------------------------------------------------------------------
 * sseGetCallEventHandler
 *
 * Gets the call event handler.
 *
 * Input:   hSSEApp  - The pointer to the application object handle of SSE.
 *          size - The size of the call event handler structure.
 *
 * Output: sseCallEvent - Call event handler structure.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseGetCallEventHandler(
                            IN      HSSEAPP      hSSEApp,
                            OUT     SSECALLEVENT sseCallEvent,
                            IN      int          size);

/*-----------------------------------------------------------------------------------------
 * sseCreateCall
 *
 * Creates a new call object.
 *
 * Input:   hSSEApp  - The pointer to the application object handle of SSE.
 *          hSSEaCall - The call object handle of the application.
 *          hCall     - The H.323 Stack call object handle.
 *
 *
 * Output: hSSECall - The pointer to the call object handle of SSE.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCreateCall(
            IN  HSSEAPP         hSSEApp,
            OUT HSSECALL*       hSSECall,
            IN  HSSEAPPCALL     hSSEaCall,
            IN  HCALL           hCall);


/*-----------------------------------------------------------------------------------------
 * sseCallClose
 *
 * Closes the call and invalidates the call object handle.
 *
 * Input:   hSSECall - The pointer to the call object handle of SSE.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallClose(
              IN    HSSECALL        hSSECall);


/*-----------------------------------------------------------------------------------------
 * sseCallH450SupplServ
 *
 * The function the application need to call in cmEvCall450SupplServ.
 *
 * Input:   hSSECall - The pointer to the call object handle of SSE.
 *          msgType  - Q.931 message type.
 *          nodeId   - PVT node ID of the H.450 APDU Tree.
 *          size     - Size of the APDU in bytes.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallH450SupplServ(
            IN  HSSECALL            hSSECall,
            IN  cmCallQ931MsgType   msgType,
            IN  int                 nodeId,
            IN  int                 size);



/*-----------------------------------------------------------------------------------------
 * sseCallStateChanged
 *
 * The function the application need to call in cmEvCallStateChanged().
 *
 * Input:   hSSECall -  The pointer to the call object handle of SSE.
 *          state    -  New state of the call.
 *          stateMode - New state mode. This parameter describes why or how the call
 *                      enters a new state.
 *
 * Return: non negative value if success.
 *         negative value if error occured.
  *-----------------------------------------------------------------------------------------*/
RVAPI
int RVCALLCONV sseCallStateChanged(
        IN      HSSECALL        hSSECall,
        IN      UINT32          state,
        IN      UINT32          stateMode);


RVAPI
int RVCALLCONV sseEAtoStr(HSSAPP hSSApp,int eaNodeId, char *str, int len);

RVAPI
int RVCALLCONV sseStrtoEA(char *str,HSSAPP hSSApp,int eaNodeId);

RVAPI
int RVCALLCONV sseMeiEnter(IN  HSSEAPP hSSEApp);

RVAPI
int RVCALLCONV sseMeiExit(IN  HSSEAPP hSSEApp);



#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
}
#endif



