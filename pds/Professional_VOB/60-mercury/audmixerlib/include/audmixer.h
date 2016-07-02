/*****************************************************************************
   模块名      : 混音器
   文件名      : audiomixer.h
   创建时间    : 2003年 12月 9日
   实现功能    : 
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/09    3.0         zmy         创建
******************************************************************************/
#ifndef _KDV_MCU_MIXER_H_
#define _KDV_MCU_MIXER_H_
#include "osp.h"
#include "evmixer.h"

BOOL32 UserInit();

API void mixerver();
API void amixSetDebugScreen();
API void amixSetReleaseScreen();
API void amixSetDebugFile();
API void amixSetReleaseFile();

#endif //!_KDV_MCU_MIXER_H_
