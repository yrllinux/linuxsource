/*****************************************************************************
   模块名      : license generate
   文件名      : licenseinst.cpp
   相关文件    : licenseinst.h
   文件实现功能: 通讯类，实现跟服务端的消息通讯
   作者        : john
   版本        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/06/23  0.9         john         创建
******************************************************************************/
#include "licenseinst.h"
#include "evmcu.h"
#include "mcuconst.h"
#include "mcustruct.h"

#define IN
#define OUT 

CLicenseGenApp	g_cLicenseGenApp;

CLicenseGen::CLicenseGen()
{
	ClearInst();
}

CLicenseGen::~CLicenseGen()
{
	ClearInst();
}

void CLicenseGen::InstanceEntry( CMessage* const pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}

	switch( pcMsg->event ) 
	{
	case OSP_POWERON:
		ProcPowerOn( pcMsg );
		break;

	case LGEN_MCUGD_GENERATELICENSE_REQ:
		ProcGenerateLicenseReq( pcMsg );
		break;

	case MCUGD_LGEN_GENERATELICENSE_ACK:
	case MCUGD_LGEN_GENERATELICENSE_NACK:
		ProcGenerateLicenseRsp( pcMsg );
		break;
		
	default:
		break;
	}
	return;
}

/*=============================================================================
  函 数 名： ClearInst
  功    能： 清空实例
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CLicenseGen::ClearInst( void )
{
	m_dwservernode = INVALID_NODE;
	m_dwserveripaddr = 0;
	m_wserverport = 0;
}

/*=============================================================================
  函 数 名： ProcPowerOn
  功    能： 上电（连接服务器端）
  算法实现： 
  全局变量： 
  参    数： const CMessage *const pcMsg
  返 回 值： void 
=============================================================================*/
void CLicenseGen::ProcPowerOn( const CMessage *const pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}

	m_dwserveripaddr = htonl( *(u32*)pcMsg->content ); // host 
	m_wserverport = *(u16*)( pcMsg->content + sizeof(u32) ); // host

	
	if( INVALID_NODE != m_dwservernode )  // first disconnect to server
	{
		::OspDisconnectTcpNode( m_dwservernode );
	}

	m_dwservernode = OspConnectTcpNode( m_dwserveripaddr, m_wserverport );// connect to server
	
	if( INVALID_NODE == m_dwservernode )
	{
		// fail to connect to sever 
		OspPrintf(TRUE, FALSE, "connect to server fail.\n");
	}
	else
	{
		OspPrintf(TRUE, FALSE, "connect to server success.\n");
	}
	
	return;
}

/*=============================================================================
  函 数 名： ProcGenerateLicenseRsp
  功    能： 处理从服务端返回的生成license的回应
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CLicenseGen::ProcGenerateLicenseRsp(const CMessage * pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}
	
	if( MCUGD_LGEN_GENERATELICENSE_NACK == pcMsg->event )
	{
		OspPrintf( TRUE, FALSE, " invalid user name.\n");
		AfxMessageBox("generate license key failed");
		return;
	}
	else
	{
		AfxMessageBox("generate license key success");
	}
	return;

}

/*=============================================================================
  函 数 名： ProcGenerateLicenseReq
  功    能： 处理从界面发过来的生成license请求
  算法实现： 
  全局变量： 
  参    数： const CMessage *const pcMsg
  返 回 值： void 
=============================================================================*/
void CLicenseGen::ProcGenerateLicenseReq( const CMessage *const pcMsg )
{
	if( NULL == pcMsg )
	{
		return;
	}

	// 消息体：TLicenseParam
	post( MAKEIID( AID_MCU_GUARD, 1), LGEN_MCUGD_GENERATELICENSE_REQ,
		       pcMsg->content, pcMsg->length, m_dwservernode);
	
	return;
}
