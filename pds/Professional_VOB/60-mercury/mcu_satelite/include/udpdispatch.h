// UdpDispatch.h: interface for the CUdpDispatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPDISPATCH_H__D054F3D0_7D88_45BF_ADDE_CAFE79879362__INCLUDED_)
#define AFX_UDPDISPATCH_H__D054F3D0_7D88_45BF_ADDE_CAFE79879362__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "osp.h"

#ifndef SOCKET 
#define SOCKET   int
#endif

class CUdpDispatchInst : public CInstance  
{
	enum
	{
		STATE_IDLE = 0,
		STATE_NORMAL,
	};
protected:
	SOCKET m_hSocket;
	u32 m_dwSN;
	u16 m_wEventId;
	
public:
	CUdpDispatchInst();
	virtual ~CUdpDispatchInst();

	void InstanceEntry( CMessage * const pcMsg );
protected:
	void ProcPowerOn( CMessage * const pcMsg );

};

typedef zTemplate< CUdpDispatchInst, 1 > CUdpDispatchApp;
extern CUdpDispatchApp	g_cUdpDispatchApp;	//信令接受应用

#endif // !defined(AFX_UDPDISPATCH_H__D054F3D0_7D88_45BF_ADDE_CAFE79879362__INCLUDED_)
