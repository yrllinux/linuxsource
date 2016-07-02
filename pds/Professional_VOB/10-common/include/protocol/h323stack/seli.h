/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced IN  any form whatsoever without
written prior approval by RADVISION LTD.

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SELI_H
#define __SELI_H

#include "rvcommon.h"
#include "threads_api.h"


/*
#ifdef WIN32
typedef void *HANDLE;	
#define SEMHANDLE HANDLE
#elif __VXWORKS__
#define SEMHANDLE SEM_ID
#endif*/



typedef enum {
  seliEvRead=0x01,
  seliEvWrite=0x02,
  seliEvExept=0x04
} seliEvents;

/*  Desc: Init SELI module.  */
RVAPI int RVCALLCONV seliInit(void);

/*  Desc: End module operation.  */
RVAPI int RVCALLCONV seliEnd(void);

RVAPI
int RVCALLCONV seliSelect(void);

typedef void (RVCALLCONV *seliCallback)(int fd, seliEvents sEvent, BOOL error);

RVAPI
int RVCALLCONV seliCallOn(int fd, seliEvents sEvent, seliCallback _callback);

RVAPI
int RVCALLCONV seliCallOnThread(int fd, seliEvents sEvent, seliCallback _callback, RV_THREAD_Handle threadId);

/**************Add by eric 2003-06-09***************************/
RVAPI
BOOL RVCALLCONV RadInitSem();

RVAPI
BOOL RVCALLCONV RadEnterCritic();

RVAPI
BOOL RVCALLCONV RadExitCritic();

RVAPI
BOOL RVCALLCONV RadEndSem();

/*
 *	add by Tan guang
 */
RVAPI
void RVCALLCONV seliSetMultiThreads(BOOL mt);
/*
RVAPI
BOOL RVCALLCONV RadSemBCreate(SEMHANDLE *ptSemHandle);

RVAPI
BOOL RVCALLCONV RadSemTake(SEMHANDLE tSemHandle );

RVAPI
BOOL RVCALLCONV RadSemGive(SEMHANDLE tSemHandle );*/

/****************************************************************/

#endif /* __SELI_H */
#ifdef __cplusplus
}
#endif
