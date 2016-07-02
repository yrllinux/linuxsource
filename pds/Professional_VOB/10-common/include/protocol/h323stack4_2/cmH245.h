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

#ifndef CMH245_H
#define CMH245_H

/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/
#include "rvlog.h"
#include "rvtimer.h"
#include "ema.h"
#include "pvaltree.h"
#include "cmH245GeneralDefs.h"
#include "cmControl.h"

#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/
RV_DECLARE_HANDLE(HH245); /* H.245 Instance handle */


/* Ema types */
#define CHAN          (100)
#define H225CALL      (1001)
#define H223CALL      (1002)


/* Log declarations for API and callback functions */
typedef void (*h245iAPIEnterEvT)(IN HAPP hApp, IN const char*line, ...);
typedef void (*h245iAPIExitEvT)(IN HAPP hApp, IN const char*line, ...);
typedef void (*h245iCBEnterEvT)(IN HAPP hApp, IN const char*line, ...);
typedef void (*h245iCBExitEvT)(IN HAPP hApp, IN const char*line, ...);

/* Notify changes in the state of a control object of a call. This is called for connected
   states only (Connected/Conference) and when MultiplexEntrySend mechanism is over. */
typedef void (*h245iNotifyStateEvT)(IN HCALL hCall, IN H245ControlState state, IN RvBool bIsMesFinished);

/* Send a message callback. Every H.245 message is sent by calling this function
   directly. The actual sending is done by the user of the H.245 module. The messageNodeId
   should not be deleted by this callback */
typedef RvStatus (*h245iSendMessageEvT)(IN HCALL hCall, IN RvPvtNodeId messageNodeId, OUT RvBool * sentOnDummy);
typedef RvStatus (*h245iSendChanMessageEvT)(IN HCALL hCall, IN HCHAN hChan, IN RvPvtNodeId messageNodeId, OUT RvBool * sentOnDummy);

/* Start a timer */
typedef RvTimer* (*h245iTimerStartEvT)(
    IN HCONTROL     hCtrl,
    IN RvTimerFunc  eventHandler,
    IN void*        context,
    IN RvInt32      millisec);

/* Cancel a timer */
typedef RvStatus (*h245iTimerCancelEvT)(
    IN    HCONTROL  hCtrl,
    INOUT RvTimer   **timer);

/* Clear a timer that has timed-out */
typedef RvStatus (*h245iTimerClearEvT)(
    IN    HCONTROL  hCtrl,
    INOUT RvTimer   **timer);

/* Open logical channel in the H.223 module. The actual opening is done by the upper
   layer. */
typedef RvStatus (*h245iH223LogicalChannelEvT)(IN HCALL hCall, IN HCHAN hChan, IN RvBool isChannelOpen);

/* insert the demux table entries from the multiplex entry send message */
typedef RvStatus (*h245iH223MEStoDEMUXEvT)(
    IN  HCALL           h3G324mCall,
    IN  HAPP            h3G324m,
    IN  RvPvtNodeId     descriptors,
    OUT RvUint16        *acceptedEntries,
    OUT RvBool          *tooComplex);

/* Set the Mux-Level and Optional Header in the Demux module */
typedef RvStatus (*h245iH223MultiplexReconfiguration)(
    IN  HCALL           h3G324mCall,
    IN  RvPstFieldId    fieldId,
    IN  RvBool          bIsH223ModeChange);


typedef RvBool (RVCALLCONV *cmCallInAnsweringStateT)(IN HCALL hsCall);


/* Internal callbacks between the H.245 module and the stack on top of it */
typedef struct
{
    /* API logger functions */
    h245iAPIEnterEvT                  pfnAPIEnterEv;
    h245iAPIExitEvT                   pfnAPIExitEv;
    h245iCBEnterEvT                   pfnCBEnterEv;
    h245iCBExitEvT                    pfnCBExitEv;

    /* Timer functions */
    h245iTimerStartEvT                pfnTimerStartEv;
    h245iTimerCancelEvT               pfnTimerCancelEv;
    h245iTimerClearEvT                pfnTimerClearEv;

    /* Misc */
    h245iNotifyStateEvT               pfnNotifyStateEv;
    h245iSendMessageEvT               sendMessageForH245;
    h245iSendChanMessageEvT           sendMessageForH245Chan;

    h245iH223LogicalChannelEvT        pfnH223LogicalChannelEv;
    h245iH223MEStoDEMUXEvT            pfnH223MEStoDEMUXEv;
    h245iH223MultiplexReconfiguration pfnH223MultiplexReconfigurationEv;

    cmCallInAnsweringStateT           cmCallInAnsweringState;
} H245EvHandlers;



typedef struct
{
    int                     maxCalls;
    int                     maxChannels;

    /* Control Offset */
    int                     h225CtrlOffset;
    int                     h223CtrlOffset;

    H245EvHandlers          evHandlers; /* General H.245 Callbacks to the stack above */

    /* parameters that are used in H.245 and outside H.245 */
    RvLogMgr*               logMgr; /* Log manager to use */

    HPVT                    hVal;
    RvBool                  bIsPropertyModeNotUsed;  /* Mode used for the property database of
                                                        the stack */
    HPST                    hSynProtH245;
    HPST                    hAddrSynH245;
    RvPvtNodeId             h245Conf; /* H.245 configuration node id */
    HPST                    synOLC;
    HPST                    h245TransCap;
    HPST                    h245RedEnc;
    HPST                    h245DataType;
    MibEventT               mibEvent;       /* MIB Callbacks to call */
    HMibHandleT             mibHandle;      /* Handle of MIB to use */
    int                     encodeBufferSize;
    RvWatchdog*             watchdog; /* Watchdog to use for resources */
} H245Config;




/******************************************************************************
 * H245Init
 * ----------------------------------------------------------------------------
 * General: Initialize an H.245 module.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  pH245Cfg     - Configuration parameters for the H245 module.
 *         hApp         - Application handle to associate with this module.
 * Output: pH245        - H245 module handle created.
 *****************************************************************************/
RvStatus H245Init(
    IN  H245Config  *pH245Cfg,
    IN  HAPP        hApp,
    OUT HH245       *pH245);


/******************************************************************************
 * H245End
 * ----------------------------------------------------------------------------
 * General: Destruct an H.245 module.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH245        - Handle to the H.245 module.
 *****************************************************************************/
void H245End(IN HH245 hH245);


/******************************************************************************
 * H245Start
 * ----------------------------------------------------------------------------
 * General: Start the execution of the H.245 module.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH245        - Handle to the H.245 module.
 *****************************************************************************/
void H245Start(IN HH245 hH245);


/******************************************************************************
 * H245Stop
 * ----------------------------------------------------------------------------
 * General: Stop the execution of the H.245 module.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  hH245        - Handle to the H.245 module.
 *****************************************************************************/
void H245Stop(IN HH245 hH245);


/************************************************************************
 * H245GetChannelsEmaHandle
 * purpose: get the channels ema handle.
 * input  : hH245         - handle to the h245 module.
 * output : none
 * return : The ema handle.
 ************************************************************************/
HEMA H245GetChannelsEmaHandle(HH245 hH245);


HCHAN cmH245GetChannelList(
              HCONTROL hControlChan);


void cmH245SetChannelList(
              HCONTROL hControlChan,HCHAN ch);


/* Cross Reference functions */

/************************************************************************
 * cmiGetH245Handle
 * purpose: Get the H245 handle.
 *          note that we are counting on the fact that the H.245 handle is declared
 *          first in the cmElem object.
 * input  : hApp    - Stack's application handle
 * output : none
 * return : HH245 handle.
 ************************************************************************/
HH245 cmiGetH245Handle(IN HAPP hApp);

/************************************************************************
 * cmiGetByControl
 * purpose: get the call handle from the control handle.
 * input  : ctrl         - the control handle.
 * output : none
 * return : The call handle.
 ************************************************************************/
HCALL cmiGetByControl(HCONTROL ctrl);

/************************************************************************
 * cmiGetControl
 * purpose: get the control handle from the call handle.
 * input  : call         - the call handle.
 * output : none
 * return : The control handle.
 ************************************************************************/
HCONTROL cmiGetControl(HCALL call);


/************************************************************************
 * cmiGetIsControl
 * purpose: Get the indication for H.245 control connection.
 * input  : hsCall - handle to the call.
 * output : none
 * return : RV_TRUE if the connection exists, otherwise RV_FALSE.
 ************************************************************************/
RvBool cmiGetIsControl(IN HCALL hsCall);

/************************************************************************
 * cmiSetIsControl
 * purpose: Set the indication for H.245 control connection.
 * input  : hsCall     - handle to the call.
 *          bIsControl - RV_TRUE if the H.245 connection exists, otherwise RV_FALSE.
 * output : none
 * return : none
 ************************************************************************/
void cmiSetIsControl(IN HCALL hsCall, RvBool bIsControl);


/************************************************************************
 * cmiGetIsFastStartFinished
 * purpose: Get the indication that tells us whether the fast start process was finished.
 * input  : hsCall - handle to the call.
 * output : none
 * return : RV_TRUE if the fast start process was finished, otherwise RV_FALSE.
 ************************************************************************/
RvBool cmiGetIsFastStartFinished(IN HCALL hsCall);

/************************************************************************
 * cmiSetIsFastStartFinished
 * purpose: Set the indication that tells us whether the fast start process was finished.
 * input  : hsCall               - handle to the call.
 *          bIsFastStartFinished - RV_TRUE if the fast start process was finished, otherwise RV_FALSE.
 * output : none
 * return : none
 ************************************************************************/
void cmiSetIsFastStartFinished(IN HCALL hsCall, RvBool bIsFastStartFinished);


/************************************************************************
 * cmiGetFirstChannel
 * purpose: Get the first channel in the channel list.
 * input  : hsCall - handle to the call.
 * output : none
 * return : handle of the first channel.
 ************************************************************************/
HCHAN cmiGetFirstChannel(IN HCALL hsCall);

/************************************************************************
 * cmiGetNextChannel
 * purpose: Get the next channel in the channel list.
 * input  : hsChan - handle to the channel.
 * output : none
 * return : handle of the next channel.
 ************************************************************************/
HCHAN cmiGetNextChannel(IN HCHAN hsChan);

#if (RV_H245_SUPPORT_H223_PARAMS == 1)

/************************************************************************
 * cmiGetChannelDataType
 * purpose: Get the data type of the channel.
 * input  : hsChan - handle to the channel.
 * output : none
 * return : Data type.
 ************************************************************************/
DataType cmiGetChannelDataType(IN HCHAN hsChan);

#endif

#if (RV_H245_SUPPORT_H225_PARAMS == 1)

typedef int
    (RVCALLCONV * cmH245EvCallFastStartSetupT)(
        IN      HAPPCALL            haCall,
        IN      HCALL               hsCall,
        OUT IN  cmFastStartMessage  *fsMessage);

typedef int
    (RVCALLCONV*cmH245EvCallFastStartT)(
        IN  HAPPCALL                haCall,
        IN  HCALL                   hsCall);

typedef  struct
{
    cmH245EvCallFastStartSetupT     cmEvCallFastStartSetup;
    cmH245EvCallFastStartT          cmEvCallFastStart;

} H245SupportH225CallEvent;


RvStatus H245SetH225CallEventHandler(
    IN HH245                        hH245,
    IN H245SupportH225CallEvent*    callEvents,
    IN RvSize_t                     size);

/************************************************************************
 * cmiStopControlTimers
 * purpose: Stop the control timers after tunneling was rejected.
 * input  : hsCall     - handle to the call.
 * output : none
 * return : none
 ************************************************************************/
RvStatus cmiStopControlTimers(IN HCALL hsCall);

/************************************************************************
 * cmiStartControlTimers
 * purpose: Restart the control timers after a real H.245 connection was 
 *          established.
 * input  : hsCall     - handle to the call.
 * output : none
 * return : none
 ************************************************************************/
RvStatus cmiStartControlTimers(IN HCALL hsCall);

/* Util functions */
int cmVtToTA_H245(HPVT hVal,int nodeId, cmTransportAddress* ta);

int cmTAToVt_H245(HPVT hVal,int nodeId, cmTransportAddress* ta);

#endif /* #if (RV_H245_SUPPORT_H225_PARAMS == 1) */




#ifdef __cplusplus
}
#endif


#endif  /* CMH245_H */
