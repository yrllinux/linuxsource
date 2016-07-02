#ifdef __cplusplus
extern "C" {
#endif

/*
***********************************************************************************

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

***********************************************************************************
*/



#ifndef _USER_INPUT_H
#define _USER_INPUT_H

#include "rverror.h"



typedef enum
{
    cmUserInputNonStandard = 0,
    cmUserInputAlphanumeric,
    cmUserInputSupport,
    cmUserInputSignal,
    cmUserInputSignalUpdate,
    cmUserInputExtendedAlphanumeric,
    cmUserInputEncryptedAlphanumeric
} cmUserInputIndication;


typedef enum
{
    cmSupportNonStandard = 0,
    cmSupportBasicString, /* alphanumeric */
    cmSupportIA5String, /* alphanumeric */
    cmSupportGeneralString, /* alphanumeric */
    cmSupportDtmf, /* supports dtmf using signal and signalUpdate */
    cmSupportHookflash, /* supports hookflash using signal */
    cmSupportExtendedAlphanumeric,
    cmSupportEncryptedBasicString, /* encrypted Basic string in encryptedAlphanumeric */
    cmSupportEncryptedIA5String, /* encrypted IA5 string in encryptedSignalType */
    cmSupportEncryptedGeneralString, /* encrypted general string in extendedAlphanumeric.encryptedalphanumeric */
    cmSupportSecureDTMF /* secure DTMF using encryptedSignalType */
} cmUserInputSupportIndication;


typedef struct
{
    int timestamp;      /* -1 if optional; */
    int expirationTime; /* -1 if optional;*/
    int logicalChannelNumber;
} cmUserInputSignalRtpStruct;


typedef struct
{
    char                        signalType;
    int                         duration;
    cmUserInputSignalRtpStruct  cmUserInputSignalRtp;/* if logical channel == 0 -optional*/
} cmUserInputSignalStruct;


RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputSupportIndicationBuild(
    /* Build userUser message with alphanumeric data */
    IN  HAPP                            hApp,
    IN  cmUserInputSupportIndication    userInputSupport,
    OUT int*                            nodeId);  /* nodeId of nonstandard UserInputSupportIndication */


RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputSignalBuild(
    /* Build userUser message with alphanumeric data */
    IN  HAPP                        hApp,
    IN  cmUserInputSignalStruct*    userInputSignalStruct);


RVAPI int RVCALLCONV  /* userInput message node id or negative value on failure */
cmUserInputSignalUpdateBuild(
    /* Build userUser message with alphanumeric data */
    IN  HAPP                        hApp,
    IN  cmUserInputSignalStruct*    userInputSignalStruct);


RVAPI int RVCALLCONV  /* RV_TRUE or negative value on failure */
cmUserInputGetDetail(
    IN  HAPP                    hApp,
    IN  RvInt32                 userInputId,
    OUT cmUserInputIndication*  userInputIndication);

RVAPI int RVCALLCONV
cmUserInputGetSignal(
    IN  HAPP                        hApp,
    IN  RvInt32                     signalUserInputId,
    OUT cmUserInputSignalStruct*    userInputSignalStruct);

RVAPI int RVCALLCONV
cmUserInputGetSignalUpdate(
    IN  HAPP                        hApp,
    IN  RvInt32                     signalUserInputId,
    OUT cmUserInputSignalStruct*    userInputSignalStruct);

RVAPI int RVCALLCONV
cmUserInputSupportGet(
    IN  HAPP                            hApp,
    IN  RvInt32                         supportUserInputId,
    OUT cmUserInputSupportIndication*   userInputSupportIndication);




#endif
#ifdef __cplusplus
}
#endif



