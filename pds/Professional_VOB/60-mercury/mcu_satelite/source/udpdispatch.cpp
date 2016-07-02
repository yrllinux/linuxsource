/*****************************************************************************
   模块名      : mcu
   文件名      : udpdispatch.cpp
   相关文件    : udpdispatch.h
   文件实现功能: 接收MT信令
   作者        : zhangsh
   版本        : V4.6  Copyright(C) 2006-2009 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2003/12/25   0.9         zhangsh     创建
   2009/09/12   4.6         张宝卿      从卫星会议移植过来    
******************************************************************************/

// UdpDispatch.cpp: implementation of the CUdpDispatch class.
//
//////////////////////////////////////////////////////////////////////

#include "udpdispatch.h"
#include "mcuvc.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUdpDispatchApp	g_cUdpDispatchApp;


CUdpDispatchInst::CUdpDispatchInst()
{
	m_hSocket = INVALID_SOCKET;
	m_dwSN = 0;
	m_wEventId = 0;
}

CUdpDispatchInst::~CUdpDispatchInst()
{
	if( m_hSocket != INVALID_SOCKET )
		SockClose( m_hSocket );
	m_dwSN = 0;

}

void CUdpDispatchInst::InstanceEntry( CMessage * const pcMsg )
{
	switch( CurState() ) 
	{
	case STATE_IDLE:
		ProcPowerOn( pcMsg );
		break;
	default:
		OspPrintf(TRUE, FALSE, "Invalid message Id %d Receive.\n", pcMsg->event );
	}

}

/*
void CUdpDispatchInst::ProcPowerOn( CMessage * const pcMsg )
{
	SOCKADDR_IN tUdpAddr;
	SOCKET		tSocket;
	u16 wPort;

	memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
    tUdpAddr.sin_family = AF_INET; 
	
	wPort = g_cMcuVcApp.GetRcvMtSignalPort();

	tUdpAddr.sin_port = htons( wPort );
	tUdpAddr.sin_addr.s_addr = g_cMcuAgent.GetMpcIp();
	
    //Allocate a socket
    m_hSocket = socket ( AF_INET, SOCK_DGRAM , 0 );
	
	if( m_hSocket == INVALID_SOCKET )
	{
		log( LOGLVL_EXCEPTION ,"CUdpDispatchInst: Create UDP Socket Error.\n" );
		return ;
	}
	
	if( bind( m_hSocket, (SOCKADDR *)&tUdpAddr, sizeof( tUdpAddr ) ) == SOCKET_ERROR )
	{
		SockClose( m_hSocket );//bind failed!
		tSocket = INVALID_SOCKET;

		log( LOGLVL_EXCEPTION, "CUdpDispatchInst: Bind socket Error.\n" );
		return ;
	}

	//接受消息循环
    //接受消息循环
    fd_set read;
    int ret_num;
    CSatMsg cSatMsg;
    char	abyRcvBuff[40000];
    u32		dwRcvLen;
    
    while ( TRUE ) 
    {
        //判断m_hSocket是否有读写操作
        FD_ZERO( &read );
        FD_SET( ( unsigned int )m_hSocket, &read );
        
        ret_num = select( FD_SETSIZE, &read, NULL, NULL, NULL );
        
        /*
        *	Follow was the most important signalling recevie process!
        *	We Assume that the packet was receive just as it was send a time!!
        *
        if( FD_ISSET( m_hSocket, &read ) )
        {
            dwRcvLen = recvfrom( m_hSocket, abyRcvBuff, sizeof( abyRcvBuff ), 0, NULL, NULL );
            if( dwRcvLen == SOCKET_ERROR )
                continue;
            
            if( dwRcvLen < cSatMsg.GetSatMsgHeadLen() )
            {
                log( LOGLVL_EXCEPTION, "CUdpDispatchInst:Receive CSatMsg Head Too Small.\n" );
                continue;
            }
            
            if( dwRcvLen > sizeof( cSatMsg ) )
            {
                log( LOGLVL_EXCEPTION, "CUdpDispatchInst:Receive CSatMsg Too Large.\n" );
                continue;
            }
            
            memcpy( &cSatMsg, abyRcvBuff, dwRcvLen );
            
            if( !cSatMsg.IsValidSatMsg() )
            {
                log( LOGLVL_EXCEPTION, "CUdpDispatchInst: Invalidate CSatMsg Receive.\n" );
                continue;
            }
            
            OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::DAEMON ), cSatMsg.GetEventId(), cSatMsg.GetSatMsg(),
                cSatMsg.GetSatMsgLen() );
            
        }
	}
}
*/


void CUdpDispatchInst::ProcPowerOn( CMessage * const pcMsg )
{
	SOCKADDR_IN tUdpAddr;
	SOCKET		tSocket;
	u16 wPort;
	
	memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
    tUdpAddr.sin_family = AF_INET; 
	
	wPort = g_cMcuVcApp.GetRcvMtSignalPort();
	
	tUdpAddr.sin_port = htons( wPort );
	tUdpAddr.sin_addr.s_addr = g_cMcuAgent.GetMpcIp();

	//g_cMcuAgent.GetMcuRcvMtSignalPort( &wPort );
	//tUdpAddr.sin_port = htons( wPort );
	//tUdpAddr.sin_addr.s_addr = htonl( g_dwMcuVcIp );
	//Allocate a socket
    m_hSocket = socket ( AF_INET, SOCK_DGRAM , 0 );
	
	if( m_hSocket == INVALID_SOCKET )
	{
		log( LOGLVL_EXCEPTION ,"CUdpDispatchInst: Create UDP Socket Error.\n" );
		return ;
	}
	
	if( bind( m_hSocket, (SOCKADDR *)&tUdpAddr, sizeof( tUdpAddr ) ) == SOCKET_ERROR )
	{
		SockClose( m_hSocket );//bind failed!
		tSocket = INVALID_SOCKET;
		
		log( LOGLVL_EXCEPTION, "CUdpDispatchInst: Bind socket Error.\n" );
		return ;
	}
	
	//接受消息循环
	ITSatMsgHdr tSatMsgHdr;
	char	abyRcvBuff[40000];
	u32		dwRcvLen;
	
	while ( TRUE ) 
	{
		
		dwRcvLen = recvfrom( m_hSocket, abyRcvBuff, sizeof( abyRcvBuff ), 0, NULL, NULL );
		if( dwRcvLen == SOCKET_ERROR )
			continue;
		
		if( dwRcvLen < sizeof(ITSatMsgHdr) )
		{
			continue;
		}
		
		tSatMsgHdr = *(ITSatMsgHdr*)abyRcvBuff;
		
		if ( m_dwSN != tSatMsgHdr.GetMsgSN() || m_wEventId !=  tSatMsgHdr.GetEventId() )
		{
			OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::DAEMON ), tSatMsgHdr.GetEventId(), abyRcvBuff,
				dwRcvLen );
			m_dwSN = tSatMsgHdr.GetMsgSN();
			m_wEventId = tSatMsgHdr.GetEventId();
		}
	}
}


//END OF FILE