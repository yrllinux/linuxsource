


#ifndef AGCSIN_H
#define	AGCSIN_H


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


int AgcSinInit(void **hHandle,int iPcmLen, int iAgcGain);
void AgcSinClose(void *hHandle);
int AgcSinProc(void *hHandle, short nPcmIn[], short nPcmOut[]);
void  GetAgcSinVersion(char *Version, int StrLen, int * StrLenUsed);


#endif
