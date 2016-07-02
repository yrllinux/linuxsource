/*****************************************************************************
   模块名      : MtAdp
   文件名      : bindmtadp.h
   相关文件    : 
   文件实现功能: MtAdp模块启停接口
   作者        : 谭光
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11     0.1         谭光        创建
******************************************************************************/
#ifndef __BINDMTADP_H_
#define __BINDMTADP_H_

#include "cristruct.h"

//MtAdp信息
struct TMtAdp
{
protected:
    u32  m_dwIpAddr;        //本MtAdpIp地址
    u8   m_byMtAdpId;       //本MtAdp编号
    u8   m_byAttachMode;    //本MtAdp的附属方式
public:
    void SetMtadpId(u8 byMtAdpId){ m_byMtAdpId = byMtAdpId; }
    u8   GetMtadpId(void) const { return m_byMtAdpId; }
    void SetAttachMode(u8 byAttachMode) { m_byAttachMode = byAttachMode; }
    u8   GetAttachMode(void) const { return m_byAttachMode; }
    void SetIpAddr(u32 dwIP){ m_dwIpAddr = htonl(dwIP); }
    u32  GetIpAddr(void) { return ntohl(m_dwIpAddr); }

	TMtAdp( void )
	{
		memset( this, 0, sizeof( TMtAdp ) );
	};
};

struct TMtAdpConnectParam
{
public:
    u32  m_dwMcuTcpNode;  //0 when AttachVc
    u32  m_dwMcuIp;       //Mcu IP to connect to(net order)
    u16  m_wMcuPort;      //Mcu Port to connect to(host order)
    BOOL m_bSupportHD;    //Supoort or not
public:
    void SetMcuIpAddr(u32 dwIP){ m_dwMcuIp = htonl(dwIP); }
    u32  GetMcuIpAddr(void) { return ntohl(m_dwMcuIp); }
	
	TMtAdpConnectParam( void )
	{
		memset( this, 0, sizeof( TMtAdpConnectParam ) );
	};
};

/*=============================================================================
    函 数 名： MtAdpStart
    功    能： 开启MtAdp
    算法实现： 
    全局变量： 
    参    数： TMtAdpConnectParam *ptConnectParam
               TMtAdp *ptMtadp
               TMtAdpConnectParam *ptConnectParam2 = NULL（when doublelinking is used）
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/9/10   4.0			万春雷                  创建
=============================================================================*/
BOOL32 MtAdpStart(TMtAdpConnectParam *ptConnectParam, TMtAdp *ptMtadp, TMtAdpConnectParam *ptConnectParam2 = NULL, u8 *pabyBrdEthMac = NULL);

/*=============================================================================
    函 数 名： MtAdpStart
    功    能： 开启MtAdp
    算法实现： 
    全局变量： 
    参    数： TMtAdpConnectParam *ptConnectParam
               TMtAdp *ptMtadp
               TMtAdpConnectParam *ptConnectParam2 = NULL（when doublelinking is used）
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/9/10   4.0			pgf                   创建
=============================================================================*/
BOOL32 MtAdpStart(	TMtIpMapTab* tMtIpMapTab, 
					TMtAdpConnectParam *ptConnectParam, TMtAdp *ptMtadp,				   
				    TCriIpMapTab* tCriIpMapTab,
				    TGKIpMapTab* tGKIpMapTab,
				    s8 *pchV6Ip,
				    s16 swScopeId,
					TMtAdpConnectParam *ptConnectParam2 = NULL);

/*=============================================================================
    函 数 名： MtAdpStop
    功    能： 停止MtAdp
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void  
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/9/10   4.0			万春雷                  创建
=============================================================================*/
void MtAdpStop();

#endif // __BINDMTADP_H_
