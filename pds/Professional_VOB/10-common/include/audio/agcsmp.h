


#ifndef AGCSMP_H
#define	AGCSMP_H
//#include "algorithmtype_xc.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define	AGC_VERYHIGH_GAIN		0
#define	AGC_HIGH_GAIN			1
#define AGC_AVERAGE_GAIN 		2
#define AGC_LOW_GAIN			3
#define AGC_VERYLOW_GAIN		4

#ifdef WIN32
#ifdef __cplusplus
extern "C"
{
#endif
#endif
l32 AgcSmpInit(void **hHandle,l32 iPcmLen, l32 iAgcGain);
void AgcSmpClose(void *hHandle);
l32 AgcSmpProc(void *hHandle, s16 nPcmIn[], s16 nPcmOut[]);
void  GetAgcSmpVersion(s8 *Version, l32 StrLen, l32 * StrLenUsed);

#ifdef WIN32
#ifdef __cplusplus
}
#endif
#endif

#endif
