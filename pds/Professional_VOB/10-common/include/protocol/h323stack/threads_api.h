/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/


/********************************************************************************************
 *                                Threads_API.h
 *
 * This module provides an API for threads handling with a common interface for all operating
 * systems.
 * It includes a single function which returns the handle of the current running thread.
 *
 *
 *      Written by                        Version & Date                        Change
 *     ------------                       ---------------                      --------
 *     Tsahi Levent-Levi                  25-Sep-2000
 *
 ********************************************************************************************/

#ifndef THREADS_API_H
#define THREADS_API_H

#ifdef __cplusplus
extern "C" {
#endif


#include "rvcommon.h"



/* A thread handle - used to identify a thread */
DECLARE_OPAQUE(RV_THREAD_Handle);




/********************************************************************************************
 * RV_THREAD_GetHandle
 * purpose : Returns the handle of the running thread
 * input   : none
 * output  : none
 * return  : A thread handle
 *           NULL on failure (unrecognized thread)
 ********************************************************************************************/
RV_THREAD_Handle RVAPI RVCALLCONV RV_THREAD_GetHandle(void);

/******************add by jason 2004-6-9************************************
* RV_THREAD_ExitHandle
* purpose : free the handle of the running thread;
* input   : none
* output  : none
* return  : none
*           
*****************************************************************/
void RVAPI RVCALLCONV RV_THREAD_ExitHandle(void);

/* RV_THREAD_ExitHandle
* purpose : free the handle of the running thread;
* input   : none
* output  : none
* return  : none
*          
*****************************************************************/ 
BOOL RVAPI RVCALLCONV RV_THREAD_IsExistHandle(void);


/***********************end by jason*************************************/

/********************************************************************************************
 * RV_THREAD_GetIndex
 * purpose : Returns the index of the running thread
 * input   : none
 * output  : none
 * return  : Index of the running thread
 ********************************************************************************************/
int RVAPI RVCALLCONV RV_THREAD_GetIndex(void);


/********************************************************************************************
 * RV_THREAD_GetThreadId
 * purpose : Returns the thread ID of the given thread handle.
 *           The returned value is the OS specific ID
 * input   : none
 * output  : none
 * return  : OS specific thread ID
 ********************************************************************************************/
int RVAPI RVCALLCONV RV_THREAD_GetThreadId(IN RV_THREAD_Handle threadId);





#ifdef __cplusplus
}
#endif

#endif

