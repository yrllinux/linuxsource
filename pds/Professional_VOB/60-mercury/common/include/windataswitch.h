
#ifndef WINDATASWITCH_H
#define WINDATASWITCH_H

#include "osp.h"
#include "kdvtype.h"
#include "kdvdef.h"
#include "mcuconst.h"
#include "dataswitch.h"

#define DSID    u32

#define DSOK    1
#define DSERROR 0

#define DS_TYPE_NONE           (u8)0         //未定义地址类型
#define DS_TYPE_IPV4           (u8)1         //IPv4地址类型
#define DS_TYPE_IPV6           (u8)2         //IPv6地址类型
#define IPV6_NAME_LEN          (u8)64
#define NULL 0

struct TDSNetAddr 
{
public:
	TDSNetAddr(void):m_bySockType(0), m_wPort(0)

	{
		memset(m_achIPStr, 0, sizeof(m_achIPStr));
	}
private:
	u8  m_bySockType;
	u16 m_wPort;	
	union
	{
		s8  m_achIPStr[IPV6_NAME_LEN];
		u32 m_dwV4Ip;
	};	

public:
	BOOL32 SetIPStr( u8 byType, s8* pchIPStr )
	{	
		if (DS_TYPE_IPV4 == byType)
		{
			m_bySockType = DS_TYPE_IPV4;
			m_dwV4Ip = INET_ADDR(pchIPStr);		
		}
		else if (DS_TYPE_IPV6 == byType)
		{
			m_bySockType = DS_TYPE_IPV6;
			memcpy(m_achIPStr, pchIPStr, min(sizeof(m_achIPStr), strlen(pchIPStr) + 1));
		}
		else
		{
			OspPrintf(1, 0, "[SetIPStr] unexpected byType.%d!\n", byType);
			return FALSE;
		}
		return TRUE;
	}
	
	u32 GetV4IPAddress()const 
	{
		return ntohl(m_dwV4Ip);
	}

	BOOL32 SetPort(u16 wPort)
	{
		m_wPort = wPort;
		return TRUE;
	}

	u16 GetPort()const
	{
		return m_wPort;
	}

	u8  GetType()const
	{
		return m_bySockType;
	}
};

/********************函数声明************************/
DSID dsCreate( u32 dwIfNum,  TDSNetAddr *ptDsNetAddr);
u32  dsAdd(DSID dsId,
				     TDSNetAddr* ptRcvAddr,
			         TDSNetAddr* ptInLocalIP,
			         TDSNetAddr* ptSendtoAddr,
                     TDSNetAddr* ptOutLocalIP = NULL);

u32 dsSetSendCallback( DSID dsId, 
						         TDSNetAddr* ptRcvAddr,
						         TDSNetAddr* ptSrcAddr,
						         SendCallback pfCallback);

u32 dsSetAppDataForSend( DSID dsId, 
							       TDSNetAddr* ptRcvAddr,
							       TDSNetAddr* ptSrcAddr,
							       TDSNetAddr* ptDstAddr,
							       void * pAppData);

u32 dsRemove(DSID dsId, TDSNetAddr* ptSendtoAddr);

u32 dsAddManyToOne(DSID dsId ,
						      TDSNetAddr* ptRecvAddr,
							  TDSNetAddr* ptInLocalIP,
							  TDSNetAddr* ptSendtoAddr,
							  TDSNetAddr* ptOutLocalIP = NULL);

u32 dsSpecifyFwdSrc(DSID dsId, 
							  TDSNetAddr* ptOrigAddr, 
						      TDSNetAddr* ptMappedAddr);

u32 dsRemoveManyToOne(DSID dsId ,
                          TDSNetAddr* ptRecvAddr,
                          TDSNetAddr* ptSendtoAddr);

u32 dsRemoveAllManyToOne(DSID dsId, TDSNetAddr* ptSendtoAddr);

u32 dsAddDump(DSID dsId, TDSNetAddr* ptRecvAddr, TDSNetAddr* ptInLocalIP);

u32 dsRemoveDump(DSID dsId, TDSNetAddr* ptRecvAddr);

#endif