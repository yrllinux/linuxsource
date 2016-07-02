/* rvnet2host.h - converst network/host organized header bytes */

/************************************************************************
        Copyright (c) 2002 RADVISION Inc. and RADVISION Ltd.
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

#if !defined(RV_NET2HOST_H)
#define RV_NET2HOST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rvtypes.h"

/************************************************************************************************************************
 * RvNet2Host2Network
 *
 * Converts an array of 4-byte integers from host format to network format.  The integers can
 * then be sent over the network.
 *
 * INPUT   :  buff       : A pointer to the buffer location where the array of 4-byte integers in host format are located.
 *            startIndex : The exact byte location in the buffer where the integers in host format begin.
 *            size       : The number of integers to convert.
 * OUTPUT  :  None.
 * RETURN  :  None
 */
RVCOREAPI
void RVCALLCONV Rv2Net2Host2Network(
	IN RvUint8	*buff,
	IN RvInt	startIndex,
	IN RvInt	size);


/***********************************************************************************************************************
 * RvNet2Host2Host
 * Converts an array of 4-byte integers from network format to host format.
 * INPUT   : buff       : A pointer to the buffer location where the array of 4-byte integers in network format are located,
 *                        and where the conversion occurs.
 *           startIndex : The exact byte location in the buffer where the integers in network format begin.
 *           size       : The number of integers to convert.
 * OUTPUT  : None.
 * RETURN  : None.
 */
RVCOREAPI
void RVCALLCONV Rv2Net2Host2Host(
	IN RvUint8	*buff,
	IN RvInt	startIndex,
	IN RvInt	size);

#ifdef __cplusplus
}
#endif

#endif

