/*****************************************************************************
   模块名      : Board Agent
   文件名      : dsiconfig.cpp
   相关文件    : 
   文件实现功能: 单板代理提供给上层应用的接口实现
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/09/12  1.0         jianghy       创建
   2004/12/05  3.5         李 博        新接口的修改
******************************************************************************/
#include "osp.h"
#include "dsiconfig.h"

#ifdef _VXWORKS_
#include "brddrvLib.h"
#endif

// [pengjie 2010/3/9] CRI2/MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

//构造函数
CDsiConfig::CDsiConfig()
{
	bIsRunPrs = FALSE;
}

//析构函数
CDsiConfig::~CDsiConfig()
{
}

/*====================================================================
    函数名      ：IsRunPrs
    功能        ：是否运行prs
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：运行返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
BOOL CDsiConfig::IsRunPrs()
{
	return bIsRunPrs;
}

/*====================================================================
    函数名      ：GetBoardId
    功能        ：获取单板的槽位号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：ID值，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
u8 CDsiConfig::GetBoardId()
{
	return m_byBrdId ;
}

/*====================================================================
    函数名      ：GetBrdIpAddr
    功能        ：得到单板IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：单板IP地址(网络序)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
u32 CDsiConfig::GetBrdIpAddr( )
{
	return htonl( m_dwBrdIpAddr );
}

/*====================================================================
    函数名      ：GetConnectMcuIpAddr
    功能        ：获取要连接的MCU的IP地址(网络序)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：MCU的IP地址(网络序)，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
u32 CDsiConfig::GetConnectMcuIpAddr()
{
	return htonl(m_dwMpcIpAddr);
}

/*====================================================================
    函数名      ：GetConnectMcuPort
    功能        ：获取连接的MCU的端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：MCU的端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
u16 CDsiConfig::GetConnectMcuPort()
{
	return m_wMpcPort;
}

/*====================================================================
    函数名      ：GetPrsCfg
    功能        ：获取prs配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：TRUE / FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
BOOL CDsiConfig::GetPrsCfg( TPrsCfg* ptCfg )
{
	if( ptCfg == NULL )
		return FALSE;

	if( bIsRunPrs == FALSE)
		return FALSE;

	*ptCfg = m_prsCfg;
	return TRUE;
}

/*====================================================================
    函数名      ：SetBrdIpAddr
    功能        ：得到单板IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：单板IP地址(网络序)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
BOOL CDsiConfig::SetBrdIpAddr( u32 dwIp )
{
	m_dwBrdIpAddr = dwIp;
	return TRUE;
}

/*====================================================================
    函数名      ：ReadConnectMcuInfo
    功能        ：读取连接的MCU信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：MCU的端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
BOOL CDsiConfig::ReadConnectMcuInfo()
{
	s8    achProfileName[32];
    BOOL    bResult;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[BoardAgent] Wrong profile while reading %s!\n", "MpcIpAddr" );
		m_dwMpcIpAddr = 0;
	}
    else
    {
        m_dwMpcIpAddr = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == m_dwMpcIpAddr )
		{
			m_dwMpcIpAddr = 0;
		}
    }

	bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPort", 0, &nValue );
	if( FALSE == bResult)  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "MpcPort" );
		nValue = MCU_LISTEN_PORT;
	}
	m_wMpcPort = ( u16 )nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddrB", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Dri] Wrong profile while reading %s!\n", "MpcIpAddrB" );
        m_dwMpcIpAddrB = 0;
	}
    else
    {
        m_dwMpcIpAddrB = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == m_dwMpcIpAddrB )
		{
			m_dwMpcIpAddrB = 0;
		}
    }
    
    bResult = GetRegKeyInt(achProfileName, "BoardSystem", "MpcPortB", 0, &nValue);
    if(FALSE == bResult)
    {
        OspPrintf(TRUE, FALSE, "[Dri] Wrong profile while reading %s\n", "MpcPortB");
        nValue = MCU_LISTEN_PORT;
    }
    m_wMpcPortB = (u16)nValue;

    if(m_dwMpcIpAddr == m_dwMpcIpAddrB)
    {
        m_dwMpcIpAddrB = 0;
        m_wMpcPortB = 0;
    }

    bResult = GetRegKeyInt(achProfileName, "IsFront", "Flag", 1, &nValue);
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "IsFront" );
        nValue = 1;
	}
    m_byChoice = (u8)nValue;

#ifdef WIN32
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	m_byBrdId = (byBrdLayer<<4) | ((u8)nValue+1);
#else
	TBrdPosition tBoardPosition;
	BrdQueryPosition( &tBoardPosition );
	m_byBrdId = (tBoardPosition.byBrdLayer<<4) | (tBoardPosition.byBrdSlot+1);
#endif	

    if(0 == m_dwMpcIpAddr && 0 == m_dwMpcIpAddrB)
    {
        OspPrintf(TRUE, FALSE, "[Dsi] The A and B Mpc's Ip are all 0.\n");
        return FALSE;
    }

	return( TRUE );
}


BOOL CDsiConfig::ResumeE1RelayMode()
{
#ifndef WIN32
	char    achProfileName[32];
    BOOL    bResult;
    s32  sdwValue;

	memset( achProfileName, 0, sizeof(achProfileName) );
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "linkcfg.ini");

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link0", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 0, byMode );*/
                BrdSetE1RelayLoopMode( 0, byMode );
	}

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link1", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 1, byMode );*/
                BrdSetE1RelayLoopMode( 1, byMode );
	}

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link2", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 2, byMode );*/
                BrdSetE1RelayLoopMode( 2, byMode );
	}

	bResult = GetRegKeyInt( achProfileName, "LinkConfig", "Link3", 0, &sdwValue );
	if( bResult == TRUE )  
	{
		u8 byMode = (u8)sdwValue;
		/*BrdDSISetRelayMode( 3, byMode );*/
                BrdSetE1RelayLoopMode( 3, byMode );
	}
#endif
	
	return TRUE;	
}


/*====================================================================
功能：将E1的RELAY MODE保存到文件
参数：无
返回值：TRUE/FALSE
====================================================================*/
BOOL CDsiConfig::SaveE1RelayMode( u8* byInfo, u16 wInfoLen )
{
#ifndef WIN32
	char    achProfileName[32];
    BOOL    bResult;
    s32  sdwValue;
    FILE    *stream;

	if( wInfoLen == 0 )
	{
		return FALSE;
	}
	
	memset( achProfileName, 0, sizeof(achProfileName) );
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "linkcfg.ini");

	//首先看有没有LINKCFG.INI文件
    stream = fopen( achProfileName, "rb" );
    if( stream != NULL )
    {
		fclose( stream );
    }
	else
	{
		//没有，要创建该文件
		stream = fopen( achProfileName, "w" );
		fclose( stream );
	}

	for( u8 byLoop = 0; byLoop < byInfo[0]; byLoop++ )
	{
		u8 abyKey[32];
		memset( abyKey, 0, sizeof(abyKey) );

		sprintf( (char*)abyKey, (char*)"Link%u", byInfo[ byLoop*2 + 1] );
		bResult = SetRegKeyInt( achProfileName, (s8*)"LinkConfig", (s8*)abyKey, byInfo[ byLoop*2 + 2 ] );
		if( bResult == FALSE)  
		{
			OspPrintf( TRUE, TRUE, "Error Save E1 Link%u Info\n", byInfo[ byLoop*2 + 1] );
		}
	}

#endif
	
	return TRUE;	
}

/*====================================================================
    函数名      ：SetPrsConfig
    功能        ：设置PRS的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：Prs配置信息结构指针
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建

====================================================================*/
BOOL CDsiConfig::SetPrsConfig( TEqpPrsEntry *ptPrsCfg )
{
	if( ptPrsCfg == NULL )
		return FALSE;

	bIsRunPrs = TRUE;

	m_prsCfg.wMcuId = ptPrsCfg->GetMcuId();
	m_prsCfg.dwConnectIP  = ptPrsCfg->GetMcuIp();
	m_prsCfg.wConnectPort = ptPrsCfg->GetMcuPort();
	m_prsCfg.byEqpType = ptPrsCfg->GetType();

	m_prsCfg.byEqpId = ptPrsCfg->GetEqpId();
	strncpy( m_prsCfg.achAlias, ptPrsCfg->GetAlias(), sizeof(m_prsCfg.achAlias) );
	m_prsCfg.achAlias[ sizeof(m_prsCfg.achAlias) - 1 ] = '\0';
	m_prsCfg.dwLocalIP = ptPrsCfg->GetIpAddr();
	m_prsCfg.wRcvStartPort = ptPrsCfg->GetEqpRecvPort();
	m_prsCfg.m_wFirstTimeSpan = ptPrsCfg->GetFirstTimeSpan();
	m_prsCfg.m_wSecondTimeSpan = ptPrsCfg->GetSecondTimeSpan();
	m_prsCfg.m_wThirdTimeSpan = ptPrsCfg->GetThirdTimeSpan();
	m_prsCfg.m_wRejectTimeSpan = ptPrsCfg->GetRejectTimeSpan();

    m_prsCfg.dwConnectIpB = m_dwMpcIpAddrB;
    m_prsCfg.wConnectPortB = m_wMpcPortB;
	return TRUE;
}

