/*****************************************************************************
   模块名      : Recorder
   文件名      : RecApp.h
   相关文件    : 
   文件实现功能: 录像机模块入口及一些全局函数的定义
   作者        : 
   版本        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#ifndef	RECAPP_H
#define RECAPP_H

#include "osp.h"
#include "ConfAgent.h"
#include "RPCtrl.h"

API void UserInit();
API void RestartRecorder(HWND hMainDlg);

API void rechelp();
API void recSetDebugScreen();
API void recSetReleaseScreen();
//API void recSetDebugFile();
//API void recSetReleaseFile();
API void recQuit();

#endif  /* !RECAPP_H */
