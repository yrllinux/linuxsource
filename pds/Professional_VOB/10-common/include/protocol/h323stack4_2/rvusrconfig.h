/***********************************************************************
Filename   : rvusrconfig.h
Description: let the user to put his own choices
************************************************************************
        Copyright (c) 2001 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/
#ifndef RV_USRCONFIG_H
#define RV_USRCONFIG_H


/* Validation checking options */
#undef  RV_CHECK_MASK
#define RV_CHECK_MASK RV_CHECK_ALL

/* Logging options */
// #if defined(RV_RELEASE) && ((RV_OS_TYPE == RV_OS_TYPE_WIN32) || (RV_OS_TYPE == RV_OS_TYPE_WINCE))
// #undef RV_LOGMASK
// #define RV_LOGMASK RV_LOGLEVEL_NONE
// #endif

/* NTP time support */
#undef  RV_NTPTIME_TYPE
#define RV_NTPTIME_TYPE RV_YES

/* 64-bit arithmetic */
#undef  RV_64BITS_TYPE
#define RV_64BITS_TYPE RV_64BITS_STANDARD

/* Threadness mode: Single / Multi */
#undef  RV_THREADNESS_TYPE
#define RV_THREADNESS_TYPE RV_THREADNESS_MULTI

/* Networking support: None, IPv4, IPv6 */
#undef  RV_NET_TYPE
//#define RV_NET_TYPE RV_NET_IPV4
#define RV_NET_TYPE RV_NET_IPV4
/* Select method: Select, Poll, Devpoll, Win32 WSA */
#undef  RV_SELECT_TYPE
#define RV_SELECT_TYPE RV_SELECT_DEFAULT

/* DNS support: None, Ares */
#undef  RV_DNS_TYPE
#define RV_DNS_TYPE RV_DNS_NONE

/* TLS support: None, OpenSSL */
#undef  RV_TLS_TYPE
#define RV_TLS_TYPE RV_TLS_NONE

/* Set the priority of the blocked commands for Nucleus (connect, close) 
 RV_THREAD_SOCKET_PRIORITY_DEFAULT by default is 3 */
/*#undef RV_THREAD_SOCKET_PRIORITY_DEFAULT
#define RV_THREAD_SOCKET_PRIORITY_DEFAULT  (2) */

#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)
#undef  RV_QOS_TYPE
#define RV_QOS_TYPE RV_QOS_NONE
#endif

/* Queue support: None, Standard */
#undef  RV_QUEUE_TYPE
#define RV_QUEUE_TYPE RV_QUEUE_NONE

#endif /* RV_USRCONFIG_H */
