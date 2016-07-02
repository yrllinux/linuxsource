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

#ifndef h460pinholetiny_h__
#define h460pinholetiny_h__
#include "osp.h"




typedef enum
{
	pinhole_rtp,
	pinhole_rtcp,
	pinhole_no_nat_rtp,
	pinhole_no_nat_rtcp,
}EMGwPinholeType;



typedef struct tagPinholeInfo
{
	u32             m_dwLocalIP;
	u32				m_dwDestIP;
    u16             m_wLocalPort;
	u16				m_wDestPort;
	u32				m_dwSN;
	u8				m_byPayLoad;
	u8				m_byPinholeType;	//0为 RTP打洞包 1为 RTCP打洞包
	u32				m_dwInterval;
	tagPinholeInfo()
	{
		memset(this, 0, sizeof(tagPinholeInfo));
	}
}TGwPinholeInfo;


typedef void (*CB_HOLEPACKET)(void *pData);


typedef struct tagNetInfo
{	
	u32 dwRcvIP;
	u16 wRcvPort;
	union{
		u32 dwKeepAliveIP;		//Rtp 打洞KeepAlive地址
		u32 dwSrcIP;			//Rtcp 源地址
		u32	dwDst;				//Rtp 非打洞目的地址
	};
	union
	{
		u16 wKeepAlivePort;		//Rtp 
		u16 wSrcPort;			//Rtcp
		u16 wDst;				//
	};
	EMGwPinholeType emType;
	CB_HOLEPACKET func;
	void *pData;
	tagNetInfo()
	{
		memset(this, 0, sizeof(tagNetInfo));
	}
}TNetInfo;


typedef struct tagSingleTcp
{
	u32 dwLocalIP;
	u32 dwPeerIP;
	u16 wLocalPort;
	u16 wPeerPort;
	BOOL32 bAtive;				//false被动,true主动
}TSingleTcp;

typedef struct tagTcpSwitchNode
{
	TSingleTcp tTcpSrc;
	TSingleTcp tTcpDst;
	tagTcpSwitchNode()
	{
		clear();
	}
	
	BOOL32 operator == (tagTcpSwitchNode &tNode)
	{
		return (memcmp(this , &tNode , sizeof(tagTcpSwitchNode)) == 0);	
	}

	void clear()
	{
		memset(this , 0 ,sizeof(*this));
	}

}TTcpSwitchNode;

//
API BOOL32 InitPinHole(u32 dwMaxCall , u32 dwIP , u16 aid);

API void DestroyPinHole();


//客户端发送　RTP或RTCP的打洞包
API BOOL KdvAddPinhole(TGwPinholeInfo& tInfo);
API BOOL KdvDelPinhole(u16 wPort);

API BOOL32 KdvAddTcpSwitch(TTcpSwitchNode &tTcpNode);

API	BOOL32 KdvDelTcpSwitch(u32 dwSrcLocalIP , u16 wSrcLocalPort , u32 dwDstLocalIP , u16 wDstLocalPort);
//1 服务端处理　RTP打洞包 过程
//           nat
//		  -------->dwKeepAliveIp  dwRcvIP<------
//			 |						|
//			 |						|
//			 ----------------(dwKeepAliveIP)

//2 服务端处理　RTP打洞包 过程
//           
//		  
//		(dst)dwKeepAliveIp		dwRcvIP<------
//			 |						|
//			 |						|
//			 -----------------------


API BOOL KdvAddDsRtpHole(TNetInfo &tInfo);

API BOOL KdvDelDsRtpHole(TNetInfo &tInfo);

// 1 服务端处理　RTCP打洞包 过程
//             nat							nat
//	SrcIp1 -----------> dwRcv1  dwRcv2 <----------- SrcIp2
//	   			|		  |			|		|
//				|	(dwRcv2)-----------------
//				|					|
//				--------------------(dwRcv1)
//

// 2 服务端处理　RTCP打洞包 过程
//											nat
//	SrcIp1 -----------> dwRcv1  dwRcv2 <----------- SrcIp2
//	  | 				  |			|		|
//	  |				      ------------------
//	  |								|
//	   -----------------------(dwRcv1)
//

// 3 服务端处理　RTCP打洞包 过程
//             							
//	SrcIp1 -----------> dwRcv1  dwRcv2 <----------- SrcIp2
//	 | 					  |			|		          |
//	 |				      ----------------------------
//	 |								|
//	  ------------------------------
//


API BOOL KdvAddDsRtcpHole(TNetInfo &tInfo1 , TNetInfo &tInfo2);
API BOOL KdvDelDsRtcpHole(TNetInfo &tInfo1 , TNetInfo &tInfo2);
#endif//h460pinhole_h__


