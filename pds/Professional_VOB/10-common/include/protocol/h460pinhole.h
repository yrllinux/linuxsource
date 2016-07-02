/*******************************************************************************
 *  模块名   :                                                                 *
 *  文件名   : h460pinhole.h                                                *
 *  相关文件 :                                                                 *
 *  实现功能 :                                                                 *
 *  作者     : 翟小刚                                                          *
 *  版本     : V1.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  30/8/2010     4.0         翟小刚      创建                                 *
 *                                                                             *
 *******************************************************************************/

#ifndef h460pinhole_h__
#define h460pinhole_h__
#include "osp.h"
#include "dataswitch.h"

typedef struct tagInitInfo
{
	u32				m_dwLocalIP;
	BOOL32			m_bStartTimeMgr;
	u32				m_dwMaxCall;
	tagInitInfo()
	{
		m_dwLocalIP		= 0;
		m_bStartTimeMgr = FALSE;
		m_dwMaxCall		= 0;
	}
}TDsInitInfo;

typedef enum
{
	pinhole_rtp,
	pinhole_rtcp,
	pinhole_tpkt,
}EMGwPinholeType;

typedef enum
{
	tcp_h225_pinhole,
	tcp_h245_pinhole,
};

typedef struct tagPinholeInfo
{
	u32             m_dwLocalIP;
	u32				m_dwDestIP;
    u16             m_wLocalPort;
	u16				m_wDestPort;
	u32				m_dwSN;
	u8				m_byPayLoad;
	u8				m_byPinholeType;
	
	tagPinholeInfo()
	{
		memset(this, 0, sizeof(tagPinholeInfo));
	}
}TGwPinholeInfo;

typedef struct tagTpktPinhole
{
	u32             m_dwCall;
	u16				m_wType;
    
	tagTpktPinhole()
	{
		memset(this, 0, sizeof(tagTpktPinhole));
	}
}TGwTpktPinhole;
typedef u32 (*DSFilterTpktFun)(TGwTpktPinhole& tTpktInfo);

API void StartupDataSwitchApp(TDsInitInfo& tInfo);

void KdvTSDestoryDataSwitch();
BOOL KdvTSAddManyToOne( u32  dwLocalIP, u16  wLocalPort, u32 dwLocalIfIP,
					   u32  dwDstIP, u16  wDstPort, u32  dwDstOutIfIP);
BOOL KdvTSRemoveAllManyToOne( u32 dwDstIP, u16 wDstPort );
BOOL KdvTSRemoveManyToOne( u32 dwLocalIP, u16 wLocalPort, u32 dwDstIP, u16 wDstPort );
BOOL KdvTSSetFilterFunc(  u32  dwIP, u16  wPort , FilterFunc pfFilter);
BOOL KdvTSSpecifyFwdSrc(u32 dwSrcIp, u16 wSrcPort, u32 dwMapIp, u16 wMapPort);
BOOL KdvTSResetFwdSrc( u32 OrigIP, u16 OrigPort);

//RTP和RTCP的打洞
BOOL KdvTSAddPinhole(TGwPinholeInfo& tInfo, u32 dwInterval, EMGwPinholeType emType);
BOOL KdvTSDelPinhole(u16 wPort);

//h225和h245的打洞
BOOL KdvTSAddTpktPinhole(TGwTpktPinhole& tInfo, u32 dwInterval, DSFilterTpktFun pCallBack);
BOOL KdvTSDelTpktPinhole(TGwTpktPinhole& tInfo);


#endif//h460pinhole_h__


