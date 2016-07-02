/*****************************************************************************
   模块名      : mcu
   文件名      : udpnms.cpp
   相关文件    : udpnms.h
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
#include "eventid.h"
#include "Udpnms.h"
#include "evnms.h"

#include "mcuvc.h"

#ifdef WIN32
#include <ws2tcpip.h>
#endif

#ifdef _VXWORKS_
#include "mcuDrvLib.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUdpNmsApp	g_cUdpNmsApp;

CUdpNms::CUdpNms()
{
	m_hSocket = INVALID_SOCKET;
	m_hTimeSocket = INVALID_SOCKET;
}

CUdpNms::~CUdpNms()
{
	if( m_hSocket != INVALID_SOCKET )
		SockClose( m_hSocket );
	if( m_hTimeSocket != INVALID_SOCKET )
		SockClose( m_hTimeSocket );
}

void CUdpNms::InstanceEntry( CMessage * const pcMsg )
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

void CUdpNms::ProcPowerOn( CMessage * const pcMsg )
{
	SOCKADDR_IN tUdpAddr;
	SOCKADDR_IN tUdpTimeAddr;
	u32 dwIpAddr;
	u16 wPort;

    //SOCKET		tSocket;
	//BrdTimeSet,SetLocalTime

	u16 port;
	memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
	memset( &tUdpTimeAddr, 0, sizeof( tUdpTimeAddr ) );
	
    dwIpAddr = g_cMcuVcApp.GetTimeRefreshIpAddr();
    wPort = g_cMcuVcApp.GetTimeRefreshPort();
	port = g_cMcuVcApp.GetApplyRcvPort();

    tUdpAddr.sin_family = AF_INET; 
	tUdpAddr.sin_port = htons(port);

	tUdpTimeAddr.sin_family = AF_INET; 
	tUdpTimeAddr.sin_port = htons(wPort);

	//Allocate a socket
    m_hSocket = socket ( AF_INET, SOCK_DGRAM , 0 );

	if( m_hSocket == INVALID_SOCKET )
	{
		log( LOGLVL_EXCEPTION ,"CUdpNms: Create UDP Signal Socket Error.\n" );
	}
	if( m_hSocket != INVALID_SOCKET &&
        bind( m_hSocket, (SOCKADDR *)&tUdpAddr, sizeof( tUdpAddr ) ) == SOCKET_ERROR )
	{
		SockClose( m_hSocket );//bind failed!
		m_hSocket = INVALID_SOCKET;
		
		log( LOGLVL_EXCEPTION, "CUdpNms: Bind Signal socket Error.\n" );
	}
	if ( wPort != 0 )
	{
		m_hTimeSocket = socket ( AF_INET, SOCK_DGRAM , 0 );
		if( m_hTimeSocket == INVALID_SOCKET )
		{
			log( LOGLVL_EXCEPTION ,"CUdpNms: Create Time Receive Socket Error.\n" );
		}

		if ( ntohl(dwIpAddr) > ntohl(inet_addr("224.0.0.1")) &&
             ntohl(dwIpAddr) < ntohl(inet_addr("239.255.255.255")) )
		{
			struct ip_mreq mreq;
			OspPrintf( TRUE, FALSE,"Jion to time group %x\n",ntohl(dwIpAddr) );
			mreq.imr_multiaddr.s_addr = dwIpAddr;
			mreq.imr_interface.s_addr = INADDR_ANY;
			setsockopt( m_hTimeSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof (mreq) );

            //guzh 2008/06/13
            s32 nBufSize = 64<<10;
            setsockopt( m_hTimeSocket, SOL_SOCKET, SO_RCVBUF,
                        (s8 *)&nBufSize, sizeof(s32) );

            setsockopt( m_hTimeSocket, SOL_SOCKET, SO_SNDBUF,
                        (s8 *)&nBufSize, sizeof(s32) );
            
		}
		
		if( m_hTimeSocket != INVALID_SOCKET &&
            bind( m_hTimeSocket, (SOCKADDR *)&tUdpTimeAddr, sizeof( tUdpTimeAddr ) ) == SOCKET_ERROR )
		{
			SockClose( m_hTimeSocket );
			m_hTimeSocket = INVALID_SOCKET;
			
			log( LOGLVL_EXCEPTION, "CUdpNms: Bind Time Receive socket Error.\n" );
		}
	}
    
	//接受消息循环
	fd_set read;
	int ret_num;

    //guzh 缓冲区溢出？这里接收要拆包拼包？
	u8  byReceiveBuf[1024*32];

	u32		dwRcvLen;
	u16		wReceiveCmd;
	u32		dwSn;
	u16		wInstId;
	
	while ( TRUE ) 
	{
		FD_ZERO( &read );	
		
        //判断m_hSocket是否有读写操作
		if ( m_hSocket != INVALID_SOCKET )
        {
			FD_SET( ( unsigned int )m_hSocket, &read );
        }
		if ( m_hTimeSocket != INVALID_SOCKET )
        {
			FD_SET( ( unsigned int )m_hTimeSocket, &read );
        }
		
		ret_num = select( FD_SETSIZE, &read, NULL, NULL, NULL );
		
        OspPrintf(TRUE, FALSE, "[CUdpNms::ProcPowerOn] some msg selected here!\n");

		if( FD_ISSET( m_hSocket, &read ) )
		{
            memset(byReceiveBuf, 0,  sizeof( byReceiveBuf ));
			dwRcvLen = recvfrom( m_hSocket, (char*)&byReceiveBuf, sizeof( byReceiveBuf ), 0, NULL, NULL );
			if( dwRcvLen == SOCKET_ERROR )
            {
				continue;
            }

			//进行消息解析
			wReceiveCmd = ntohs(*(u16*)byReceiveBuf);
			if ( wReceiveCmd < EV_SATMCUNMS_BGN || wReceiveCmd > EV_SATMCUNMS_END )
			{
				OspPrintf( TRUE, FALSE, "Receive event id-%u(len.%d) overflow!\n", wReceiveCmd, dwRcvLen );
				continue;
			}
			dwSn = ntohl(*(u32*)(byReceiveBuf + sizeof(u16) ) );
			wInstId =  dwSn >> 16;

            //if (bConfMsg)
            {
                OspPrintf( TRUE, FALSE, "[UdpNms] Receive msg.%d to instance.%d len.%d\n", 
                    wReceiveCmd, wInstId, dwRcvLen);
            }

			OspPost( MAKEIID( AID_MCU_VC, wInstId ), wReceiveCmd, (u8*)&byReceiveBuf,dwRcvLen );
			
		}
		
		if( FD_ISSET( m_hTimeSocket, &read ) )
		{
            memset(byReceiveBuf, 0,  sizeof( byReceiveBuf ));
			dwRcvLen = recvfrom( m_hTimeSocket, (char*)&byReceiveBuf, sizeof( byReceiveBuf ), 0, NULL, NULL );
            //if (bConfMsg)
            {
                OspPrintf( TRUE, FALSE, "[TimeSyn] Receive len.%d\n", dwRcvLen);
            }

			if( dwRcvLen == SOCKET_ERROR || dwRcvLen != 32 )
				continue;
			//00-01	年值，ASCII 码，2位表示，如：06
			//02-03	月值，ASCII 码，
			//04-05	日值，ASCII 码，
			//	06-07	小时值，ASCII 码
			//	08-09	分钟值，ASCII 码
			//	10-11	秒值，ASCII 码
			//	12-31	全部0
			struct tm refreshtm;
#ifdef WIN32 
			long timeget = time(NULL);
			refreshtm = *(tm*)gmtime( &timeget );
#endif
            //guzh 2008/06/13
    		//得到的是UTC时间
			char ascch[2];
			time_t timelc;
			memcpy(ascch,byReceiveBuf,2);
            refreshtm.tm_year = 100 + atoi(ascch);  //1900based
			memcpy(ascch,byReceiveBuf+2,2);
			refreshtm.tm_mon = atoi(ascch) - 1;     //0-based
			memcpy(ascch,byReceiveBuf+4,2);
			refreshtm.tm_mday = atoi(ascch);
			memcpy(ascch,byReceiveBuf+6,2);
			refreshtm.tm_hour = atoi(ascch);
			memcpy(ascch,byReceiveBuf+8,2);
			refreshtm.tm_min = atoi(ascch);
			memcpy(ascch,byReceiveBuf+10,2);
			refreshtm.tm_sec = atoi(ascch);

            //if (bConfMsg)
            {
                OspPrintf( TRUE, FALSE, "[TimeSyn] Receive data: %s(GMT)\n", byReceiveBuf);
            }

			timelc = mktime(&refreshtm);
			timelc = timelc + 8*60*60;
			refreshtm = *(struct tm*)localtime(&timelc);
#ifdef WIN32
			SYSTEMTIME systime;
			systime.wYear = refreshtm.tm_year;
			systime.wSecond = refreshtm.tm_sec;
			systime.wMonth = refreshtm.tm_mon;
			systime.wMinute = refreshtm.tm_min;
			systime.wMilliseconds = 0;
			systime.wHour = refreshtm.tm_hour;
			systime.wDay = refreshtm.tm_mday;
			systime.wDayOfWeek = refreshtm.tm_wday;
			//OspPrintf(TRUE,FALSE,"Set Time\n");
		    //SetLocalTime( &systime );
#else
            //FIXME: 这里暂时不SetTime
		    //BrdTimeSet( &refreshtm );
#endif
            //if (bConfMsg)
            {
                OspPrintf( TRUE, FALSE, "[TimeSyn] Syn time to GMT+8: %04d-%02d-%02d %02d:%02d:%02d\n",
                                         refreshtm.tm_year+1900, refreshtm.tm_mon+1, refreshtm.tm_mday,
                                         refreshtm.tm_hour, refreshtm.tm_min, refreshtm.tm_sec );
            }
		}
	}
}