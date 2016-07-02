/*****************************************************************************
   模块名      : license generate
   文件名      : licenseinst.h
   相关文件    : licenseinst.cpp
   文件实现功能: 通讯类，实现跟服务端的消息通讯
   作者        : john
   版本        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/06/23  0.9         john         创建
******************************************************************************/
#ifndef _LICENSE_H
#define _LICENSE_H
#include "osp.h"

#define	AID_LICENSEGEN	  60

class CLicenseGen : public CInstance  
{
public:
	CLicenseGen();
	~CLicenseGen();

// 消息处理函数
protected:
	
	void InstanceEntry( CMessage * const pcMsg );

	void ProcPowerOn( const CMessage *const pcMsg );
	void ProcGenerateLicenseReq( const CMessage *const pcMsg );
	void ProcGenerateLicenseRsp( const CMessage * pcMsg );

// 功能函数
private:
	void ClearInst( void );

private:
	u32  m_dwservernode;
	u32  m_dwserveripaddr;
	u16  m_wserverport;
};

typedef zTemplate< CLicenseGen, 1 > CLicenseGenApp;
extern CLicenseGenApp	g_cLicenseGenApp;	

// 没有压缩，只用于Win32
struct TNetAddr
{
	u32 dwAddr;
	u16 wPort;
};

#endif
