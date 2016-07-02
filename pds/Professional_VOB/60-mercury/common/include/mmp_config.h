/*****************************************************************************
   模块名      : Board Agent
   文件名      : mmpconfig.h
   相关文件    : 
   文件实现功能: 相关配置函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#ifndef MMPCONFIG_H
#define MMPCONFIG_H

#include "mcuagtstruct.h"
#include "mediabrdconfig.h"

class CMmpConfig : public CMediaBrdConfig
{
	friend class CBoardAgent;

public:
	CMmpConfig();
	virtual ~CMmpConfig();
};

#endif    /* MMPCONFIG_H */
