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

#ifndef _RV_H323_WATCHDOG_H
#define _RV_H323_WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef CM_H
#error rvh323watchdog.h included improperly. Use #include <cm.h> instead.
#endif


typedef enum
{
    RvH323WatchdogInitialVal,
    RvH323WatchdogMaxVal,
    RvH323WatchdogMaxUsage,
    RvH323WatchdogCurrent,
    RvH323WatchdogDeltaVal
} RvH323WatchdogResourceType;



/********************************************************************************
    RvH323WatchdogPrint
    this function goes over all recources from the table, get their values and
    print it to the log file.
    INPUT -
        hApp  - the application handle
    OUTPUT - none
    RETURN - non negative value - if success.
             negative value     - if failed to initialize the ss instance.
*********************************************************************************/
RVAPI int RVCALLCONV RvH323WatchdogPrint(IN HAPP hApp);


/********************************************************************************
    RvH323WatchdogNextResource
    this function gets the names of resources from the H.323 stack. It can be used
    to loop on all the resources of the stack with RvH323WatchdogGetResource().
    To stop looping, make sure the return value is non-negative.
    INPUT -
        hApp            - the application handle
        prevResource    - Handle to previous resource.
                          First time should be called with NULL
    OUTPUT -
        resourceHandle  - Handle to the current resource
                          Should be used as prevResourcefor the next call to this
                          function.
        resourceName    - Name of the resource (maximum of 40 bytes)
                          Can be used for RvH323WatchdogGetResource()
    RETURN - non negative value - if success.
             negative value     - if failed or if there are no more resource.
*********************************************************************************/
RVAPI int RVCALLCONV RvH323WatchdogNextResource(
    IN  HAPP    hApp,
    IN  void*   prevResource,
    OUT void**  resourceHandle,
    OUT char*   resourceName);


/********************************************************************************
RvH323WatchdogGetResource
this function get the value of a specific resource and return it
    INPUT -
        hApp  - the application handle
        name  - the resource name, used to locate resource in the table
        type  - the type of data needed (current/max/....)
    OUTPUT -
        value - the value of the resource requested
        resourceGroupName - name of group this resource belongs to. Can be passed
                            as NULL. Up to 40 characters
    RETURN - non negative value - if success.
             negative value     - if failed to initialize the ss instance.
*********************************************************************************/
RVAPI int RVCALLCONV RvH323WatchdogGetResource(
    IN  HAPP                        hApp,
    IN  const char*                 name,
    IN  RvH323WatchdogResourceType  type,
    OUT RvUint32*                   value,
    OUT char*                       resourceGroupName);


#ifdef __cplusplus
}
#endif


#endif /* _RV_H323_WATCHDOG_H */
