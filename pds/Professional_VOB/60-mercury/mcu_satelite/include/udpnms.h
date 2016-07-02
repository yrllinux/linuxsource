// UdpDispatch.h: interface for the CUdpDispatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UDPNMS_H_)
#define _UDPNMS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "osp.h"

#ifndef SOCKET 
#define SOCKET   int
#endif
class CUdpNms : public CInstance  
{
	enum
	{
		STATE_IDLE = 0,
		STATE_NORMAL,
	};
protected:
	SOCKET m_hSocket;
	SOCKET m_hTimeSocket;
public:
	CUdpNms();
	virtual ~CUdpNms();

	void InstanceEntry( CMessage * const pcMsg );
protected:
	void ProcPowerOn( CMessage * const pcMsg );

};

typedef zTemplate< CUdpNms, 1 > CUdpNmsApp;
extern CUdpNmsApp g_cUdpNmsApp;	//信令接受应用

#endif // !defined(AFX_UDPDISPATCH_H__D054F3D0_7D88_45BF_ADDE_CAFE79879362__INCLUDED_)
