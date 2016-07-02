/*****************************************************************************
模块名      : updatelib
文件名      : bindupdate.h
相关文件    : 
文件实现功能: 会议数据信息更新接口头文件
作者        : 张宝卿
版本        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人		走读人      修改内容
2006/02/22  4.0         张宝卿					创建
******************************************************************************/
#ifndef _BINDUPDATE_H_
#define _BINDUPDATE_H_

#include "mcuutility.h"
#include "vccommon_old_ver060407.h"

API void TConfInfoUpdate( TConfInfo &tConfInfo, TConfInfoOld &tConfInfoOld );

//API void TConfStoreUpdate( TConfStore &tConfStore, TConfStoreOld &tConfInfoOld );

#endif	//_BINDUPDATE_H_
