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

#ifndef _CMDEBPRN_H
#define _CMDEBPRN_H

#include "rvlog.h"
#include "cm.h"


#ifdef __cplusplus
extern "C" {
#endif




#if (!(RV_LOGMASK & RV_LOGLEVEL_ENTER))
#define cmiAPIEnter(_args)
#define cmiCBEnter(_args)
#define _cmiAPIEnter NULL
#define _cmiCBEnter NULL
#else
#define cmiAPIEnter(_args) _cmiAPIEnter _args
#define cmiCBEnter(_args) _cmiCBEnter _args
void _cmiAPIEnter(HAPP hApp,const char*line,...);
void _cmiCBEnter(HAPP hApp,const char*line,...);
#endif

#if (!(RV_LOGMASK & RV_LOGLEVEL_LEAVE))
#define cmiAPIExit(_args)
#define cmiCBExit(_args)
#define _cmiAPIExit NULL
#define _cmiCBExit NULL
#else
#define cmiAPIExit(_args) _cmiAPIExit _args
#define cmiCBExit(_args) _cmiCBExit _args
void _cmiAPIExit(HAPP hApp,const char*line,...);
void _cmiCBExit(HAPP hApp,const char*line,...);
#endif



#ifdef __cplusplus
}
#endif

#endif  /* _CMDEBPRN_H */
