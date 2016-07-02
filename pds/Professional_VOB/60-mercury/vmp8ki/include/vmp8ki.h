/*****************************************************************************
模块名      : vmp8ki
文件名      : vmp8ki.h
创建时间    : 2012年 05月 02日
实现功能    : 
作者        : 周翼亮
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/05/02  1.0         周翼亮        创建
******************************************************************************/
#ifndef _VMP8KI_H
#define _VMP8KI_H

#include "kdvtype.h"
#include "kdvlog.h"
#include "loguserdef.h"


#define LOGFILE_VMP8KI (s8*)"/usr/etc/config/conf/kdvlog_vmp8ki.ini"


void MmpFileLog( const s8 * pchLogFile, s8 * pszFmt, ... );

#endif //!_VMP8KI_H


