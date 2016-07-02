/***********************************************************************
Filename   : rvoscomp.c
Description: Complementary OS functions
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

#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)

#define strcasecmp(a,b)		_stricmp(a,b)
#define strncasecmp(a,b,n)	_strnicmp(a,b,n)

#define NEED_WIN32_WRITEV

#define IOVEC           WSABUF
#define iov_base        buf
#define iov_len         len

int writev(int s, IOVEC *iov, int iovcnt);


#else  /* all others */

#define IOVEC struct iovec


#if (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN) || (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS) || \
	(RV_OS_TYPE == RV_OS_TYPE_MOPI)

#define NEED_MANUAL_WRITEV

/* Structure for scatter/gather I/O.  */
struct iovec {
    void *iov_base;     /* Pointer to data.  */
    size_t iov_len;     /* Length of data.  */
};

int writev(int s, IOVEC *iov, int iovcnt);

#endif

#if (RV_OS_TYPE == RV_OS_TYPE_VXWORKS) || (RV_OS_TYPE == RV_OS_TYPE_NUCLEUS) || \
	(RV_OS_TYPE == RV_OS_TYPE_MOPI)
#define NEED_STRCASECMP

#endif

#endif  /* all others */


#ifdef NEED_STRCASECMP
int strcasecmp(const char *s1, const char *s2);
#endif
