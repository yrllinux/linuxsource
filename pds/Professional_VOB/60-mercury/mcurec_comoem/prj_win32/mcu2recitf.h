#ifndef _MCU2RECITF_H_
#define _MCU2RECITF_H_

#include "RecInterface.h"

// 全局回调函数定义，设置给底层 recorder.lib 
void CBRecStart( TRecProperty &tRecProperty, TConfProperty &tConfProperty, u8 &byErrId );

void CBRecStop(DWORD dwIndex, u8 &byErrId);

void CBRecPause(DWORD dwIndex, u8 &byErrId);

void CBRecResume(DWORD dwIndex, u8 &byErrId);

void CBRecStream( TStreamProperty *ptSProperty, u8 &byErrId );

extern u8 g_byComLogLevel;

#define LOGLV_EXC   0
#define LOGLV_WARN  1
#define LOGLV_INFO  2

void RecComLog( u8 byLvl, s8 * fmt, ... );

API void setreccomlog(u8 byLvl);

#endif //_MCU2RECITF_H_