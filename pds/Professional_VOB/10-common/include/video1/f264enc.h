/************************************************************************
* F264 encoder
*
* Copyright(c) 2004-2005 lc
*
* This file include all public structure and interface.
*
*************************************************************************/
#ifndef _F264ENC_H_
#define _F264ENC_H_

#include "algorithmtype.h"
#include "h264enc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ++ extern api section begin ++ */

l32 H264EncoderInitV3(void **ppvHandle, TH264EncParam * ptEncParam);

l32 H264EncoderFrameV3(void *pvHandle, TH264EncFramePara* ptFrame); // only one map

l32 H264EncoderOneFrameStage0(void *pvHandle, void *pvtEncFrame); // master of two map

l32 H264EncoderOneFrameStage1(void *pvHandle, void *pvtEncFrame); // slave of two map

l32 H264EncoderCloseV3(void *pvHandle);

l32 H264SetEncParamsV3(void *pvHandle, TH264EncParam * ptEncParam);

void GetF264EncoderVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

/* -- extern api section end -- */
#ifdef _cplusplus
};
#endif

#endif

