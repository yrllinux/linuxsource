/*****************************************************************************
模块名      : 8KI混音器外壳
文件名      : mixer8ki.cpp
创建时间    : 2012年 04月 25日
实现功能    : 
作者        : 田志勇
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/04/25  1.0         田志勇        创建
******************************************************************************/
#ifndef _MIXER8KI_H
#define _MIXER8KI_H

#include "osp.h"
#include "mcuconst.h"
#include "libinit.h"
#include "boardconfigbasic.h"
#include "commonlib.h"

#ifdef _LINUX_
#include "nipdebug.h"
#endif//_LINUX_
BOOL32 g_bQuit = FALSE;
#endif//_LINUX_