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

#ifndef _RVH323CONNECTION_H
#define _RVH323CONNECTION_H

#include "rvsocket.h"
#include "rvselect.h"


#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************
 * RvH323ConnectionType
 * The type of connection we are currently using.
 * When we receive events on a connection we know how to handle it by this
 * type.
 ************************************************************************/
typedef enum
{
    RvH323ConnectionNone = 0,
    RvH323ConnectionH245,
    RvH323ConnectionQ931
} RvH323ConnectionType;

/************************************************************************
 * RvH323Connection
 * H.323 connection. Each I/O connection used in H.323 is wrapped with
 * this basic struct, allowing the stack to know how to handle incoming
 * events for it.
 ************************************************************************/
typedef struct
{
    RvSelectFd              fd;      /* select file descriptor */
    RvH323ConnectionType    type;    /* Type of connection */
    void*                   context; /* Context of the connection.
                                        RAS context is cmElem.
                                        Q931,H245 context is cmTransGlobals */
} RvH323Connection;



/* Connection structure used for RAS listening addresses */
typedef struct
{
    RvSelectFd  fd;      /* select file descriptor */
    RvBool      isMulticast; /* RV_TRUE for a multicast socket */
    RvInt       index; /* Index of this socket in the list of RAS listening addresses */
} RasConnection;





#ifdef __cplusplus
}
#endif

#endif  /* _RVH323CONNECTION_H */


