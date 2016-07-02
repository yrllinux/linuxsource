/***********************************************************************
Filename   : RvSymint.h
Description: host related functions support for Symbian
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

#ifndef __RVSYMSELECT_H__
#define __RVSYMSELECT_H__

#if ((RV_NET_TYPE != RV_NET_NONE) && (RV_OS_TYPE == RV_OS_TYPE_SYMBIAN))

/********************************************************************************************
 *
 *  RvSymSelectTO class implements RvSelect timeout functionality
 *
 ********************************************************************************************/

class RvSymSelectTO: public CTimer {
public:
	RvSymSelectTO(RvSelectEngine *engine);
	~RvSymSelectTO();
	void RvSymSetTimer(
		IN RvUint64 nsecTimeout);
	RvStatus Construct();
protected:
    void RunL();
private:
	RvSelectEngine 	*selectEngine;
	TTime 			time;
};


#endif /* (RV_NET_TYPE != RV_NET_NONE) */
#endif // __RVSYMSELECT_H__