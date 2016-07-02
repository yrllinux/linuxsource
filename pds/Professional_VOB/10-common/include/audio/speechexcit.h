/*****************************************************************************
模块名      : speechexcit
文件名      : speechexcit.h
相关文件    : 
文件实现功能: 
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
作者        : 姜莎
-----------------------------------------------------------------------------
修改记录:
日    期      版本        修改人      修改内容
2008/03/10    V100        姜  莎      修改
******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

#include "algorithmtype.h"  

typedef struct
{
    l32 iStart;
    l32 iN2SCount;
    l32 iS2NCount;

    u32 iPostNoisEnergy;
    u32 iPostSpEnergy;
    
    l32 iPreFlag;
    l32 iPreVad;
    l32 iPcmLen;
    l32 iS2PCount;
    
    u32 iP10Noise[10];
    l32 iNCnt;
    l32 iSCnt;
}tExcitStruct;

#define SP_LEVEL_1  1
#define SP_LEVEL_2  2
#define SP_LEVEL_3  3
#define SP_LEVEL_4  4
#define SP_LEVEL_5  5
#define SP_LEVEL_6  6

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

l32 SpeechExcitInit(void **hHandle, l32 iPcmLen);
void SpeechExcitClose(void *hHandle);
l32 SpeechExcitProc(void *hHandle, s16 nPcmDat[]);

l32 SetSeSpLevel(void *hHandle, l32 iLevel);
void  GetSpeechEcxitVersion(s8 *Version, l32 StrLen, l32 * StrLenUsed);
l32 G711VoiceCheck(s8 *pSpeechBuf, l32 iBufLenInByte);

l32 G711AlowVoiceCheck(s8 *pSpeechBuf, l32 iBufLenInByte);
l32 G711UlowVoiceCheck(s8 *pSpeechBuf, l32 iBufLenInByte);
void SetG711AlowSpeechFlag(s8 *pSpeechBuf, l32 iBufLenInByte, l32 iIsSpeech);
void SetG711UlowSpeechFlag(s8 *pSpeechBuf, l32 iBufLenInByte, l32 iIsSpeech);


#ifdef __cplusplus
}
#endif
