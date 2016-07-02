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

/**************************************************************************
 *
 * This module is responsible for manipulation of singleton objects that
 * have to be allocated per thread.
 * This includes the timers and select engine.
 *
 **************************************************************************/

#ifndef _RV_THREAD_INSTANCE_H
#define _RV_THREAD_INSTANCE_H

/*-----------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                           */
/*-----------------------------------------------------------------------*/

#include "rverror.h"


#ifdef __cplusplus
extern "C" {
#endif




/*-----------------------------------------------------------------------*/
/*                           TYPE DEFINITIONS                            */
/*-----------------------------------------------------------------------*/



/******************************************************************************
 * ThreadInstanceDeleteFunc
 * ----------------------------------------------------------------------------
 * General: Deletion function for a module. This function is called when all
 *          initialization calls to the given module in the current thread have
 *          been rolled back using ThreadInstanceEndObject().
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : objectName  - Name of the object.
 *          context     - Context used for creation and deletion functions.
 *          objectPtr   - Object pointer used
 * Output : none
 *****************************************************************************/
typedef RvStatus (* ThreadInstanceDeleteFunc) (
    IN  const RvChar*   objectName,
    IN  void*           context,
    IN  void*           objectPtr);


/******************************************************************************
 * ThreadInstanceCreateFunc
 * ----------------------------------------------------------------------------
 * General: Creation function for a module. This function is called when an
 *          object has to be created for the first time in the current thread.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : objectName      - Name of the object.
 *          context         - Context used for creation and deletion functions.
 *          amountNeeded    - Amount of sub-objects we need to allocate.
 *          objectPtr       - Object pointer we already have (or NULL on first
 *                            call in this thread).
 * Output : objectPtr       - Object pointer allocated
 *          amountAllocated - Amount of sub-objects we have allocated in this
 *                            specific call.
 *          deleteFunc      - Function to call for deletion of the object.
 *****************************************************************************/
typedef RvStatus (* ThreadInstanceCreateFunc) (
    IN    const RvChar*             objectName,
    IN    void*                     context,
    IN    RvSize_t                  amountNeeded,
    INOUT void**                    objectPtr,
    OUT   RvSize_t*                 amountAllocated,
    OUT   ThreadInstanceDeleteFunc* deleteFunc);




/*-----------------------------------------------------------------------*/
/*                           FUNCTIONS HEADERS                           */
/*-----------------------------------------------------------------------*/


/******************************************************************************
 * ThreadInstanceInitObject
 * ----------------------------------------------------------------------------
 * General: Initialize a specific object instance, making sure it is only
 *          constructed once for each thread. Multiple calls to this function
 *          with the same arguments from the same thread will be discarded.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : objectName      - The name of the constructed object.
 *          func            - Function for creation of the object. Only called once for
 *                            each thread (or if the amount of sub-objects needed is not
 *                            satisfied.
 *          context         - Context used for the creation function.
 *          amountNeeded    - Amount of sub-objects the initialized object should
 *                            support. Can be set to 0 if not relevant.
 * Output : objectPtr       - Pointer to the cosntructed object (as returned by the
 *                            creation function func).
 *          count           - Number of times this object was called for construction
 *                            in the current thread.
 *          index           - Index to use for faster access.
 *****************************************************************************/
RvStatus ThreadInstanceInitObject(
    IN  const RvChar*               objectName,
    IN  ThreadInstanceCreateFunc    func,
    IN  void*                       context,
    IN  RvSize_t                    amountNeeded,
    OUT void**                      objectPtr,
    OUT RvUint32*                   count,
    OUT RvUint32*                   index);


/******************************************************************************
 * ThreadInstanceGetObject
 * ----------------------------------------------------------------------------
 * General: Get the object that was initialized for the current thread.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : index       - Index of the object as given by ThreadInstanceInitObject().
 * Output : objectPtr   - Object pointer initialized.
 *****************************************************************************/
RvStatus ThreadInstanceGetObject(
    IN  RvUint32    index,
    OUT void**      objectPtr);


/******************************************************************************
 * ThreadInstanceFindIndex
 * ----------------------------------------------------------------------------
 * General: Find the index of an object by its name.
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : objectName  - Name of the object we're looking for.
 * Output : index       - Index of the object
 *****************************************************************************/
RvStatus ThreadInstanceFindIndex(
    IN  const RvChar*   objectName,
    OUT RvUint32*       index);


/******************************************************************************
 * ThreadInstanceEndObject
 * ----------------------------------------------------------------------------
 * General: Deinitialize a specific object instance, making sure it is only
 *          destructed once for each thread (when no one is referencing it).
 * Return Value: RV_OK  - if successful.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input  : index   - Index of the object as given by ThreadInstanceInitObject().
 *          amount  - Amount of sub-objects we won't need from now on. Can be set
 *                    to 0 if this should be discarded.
 * Output : count   - Number of times this object is still referenced.
 *****************************************************************************/
RvStatus ThreadInstanceEndObject(
    IN  RvUint32        index,
    IN  RvSize_t        amount,
    OUT RvUint32*       count);



#ifdef __cplusplus
}
#endif

#endif  /*END OF: define _RV_THREAD_INSTANCE_H */
