/*****************************************************************************
   模块名      : Board Agent
   文件名      : driconfig.cpp
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
#include "mcuconst.h"
#include "vccommon.h"
#include "driconfig.h"

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
CDriConfig::CDriConfig()
{
	bIsRunPrs = FALSE;
    m_dwBrdIpAddr= 0;
    m_dwMpcIpAddr = 0;
    m_wMpcPort = 0;
    m_dwMpcIpAddrB = 0;
    m_wMpcPortB = 0;
    m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;
}

//析构函数
CDriConfig::~CDriConfig()
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
BOOL CDriConfig::IsRunPrs()
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
u8 CDriConfig::GetBoardId()
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
u32 CDriConfig::GetBrdIpAddr( )
{
	return htonl( m_dwBrdIpAddr );
}

/*=============================================================================
  函 数 名： GetMpcBIp
  功    能： 取MpcB的Ip
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u32 ：网络序
=============================================================================*/
u32 CDriConfig::GetMpcBIp(void)
{
    return htonl(m_dwMpcIpAddrB);
}

/*=============================================================================
  函 数 名： SetMpcBIp
  功    能： 设置MpcB的Ip
  算法实现： 
  全局变量： 
  参    数： u32 dwIp：网络序
  返 回 值： void 
=============================================================================*/
void CDriConfig::SetMpcBIp(u32 dwIp)
{
    m_dwMpcIpAddrB = ntohl(dwIp);
}

/*=============================================================================
  函 数 名： GetMpcAIp
  功    能： 设置MpcA的Ip
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u32 ：网络序
=============================================================================*/
u32 CDriConfig::GetMpcAIp(void)
{
    return htonl(m_dwMpcIpAddr);
}

/*=============================================================================
  函 数 名： SetMpcAIp
  功    能： 设置MpcB的Ip
  算法实现： 
  全局变量： 
  参    数： u32 dwIp：网络序
  返 回 值： void
=============================================================================*/
void CDriConfig::SetMpcAIp(u32 dwIp)
{
    m_dwMpcIpAddr = ntohl(dwIp);
}

/*=============================================================================
  函 数 名： GetMpcAPort
  功    能： 取MpcA端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16 
=============================================================================*/
u16 CDriConfig::GetMpcAPort()
{
    return m_wMpcPort;
}
void CDriConfig::SetMpcAPort(u16 wPort)
{
    m_wMpcPort = wPort;
}

/*=============================================================================
  函 数 名： GetMpcBPort
  功    能： 取MpcB端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16 
=============================================================================*/
u16 CDriConfig::GetMpcBPort()
{
    return m_wMpcPortB;
}
void CDriConfig::SetMpcBPort(u16 wPort)
{
    m_wMpcPortB = wPort;
}

/*=============================================================================
  函 数 名： SetDriIp
  功    能： 设置dri ip 
  算法实现： 
  全局变量： 
  参    数： u32 dwIp：网络序
  返 回 值： void 
=============================================================================*/
void CDriConfig::SetDriIp(u32 dwIp)
{
    m_dwBrdIpAddr = ntohl(dwIp);
}

/*=============================================================================
  函 数 名： GetDriIp
  功    能： 取Dri的Ip
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u32 ：网络序
=============================================================================*/
u32 CDriConfig::GetDriIp(void)
{
    return ntohl(m_dwBrdIpAddr);
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
u32 CDriConfig::GetConnectMcuIpAddr()
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
u16 CDriConfig::GetConnectMcuPort()
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
BOOL CDriConfig::GetPrsCfg( TPrsCfg* ptCfg )
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
BOOL CDriConfig::SetBrdIpAddr( u32 dwIp )
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
BOOL CDriConfig::ReadConnectMcuInfo()
{
	s8    achProfileName[32];
    BOOL    bResult;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[BoardAgent] Wrong profile while reading %s!\n", "MpcIpAddr" );
		return( FALSE );
	}
    m_dwMpcIpAddr = ntohl( INET_ADDR( achReturn ) );
	if( 0xffffffff == m_dwMpcIpAddr )
	{
		m_dwMpcIpAddr = 0;
	}

	bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPort", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "MpcPort" );
		return( FALSE );
	}
	m_wMpcPort = ( u16 )nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddrB", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, FALSE, "[Dri] Wrong profile while reading %s!\n", "MpcIpAddrB" );
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
        nValue = 0;
    }
    m_wMpcPortB = (u16)nValue;

    if(m_dwMpcIpAddr == m_dwMpcIpAddrB)// 做保护
    {
        m_dwMpcIpAddrB = 0;
        m_wMpcPortB = 0;
    }
	
    bResult = GetRegKeyInt(achProfileName, "IsFront", "Flag", 1, &nValue);
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "IsFront" );
        nValue = 1;
	}
    m_byChoice = (u8)nValue;

#ifdef WIN32
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Agent] Wrong profile while reading %s!\n", "Slot" );
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
        OspPrintf(TRUE, FALSE, "[Dri] The A and B Mpc's Ip are all 0.\n");
        return FALSE;
    }

    //读取osp断链检测参数
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatTime", DEF_OSPDISC_HBTIME, &nValue );
    if (nValue >= 3)
    {
        m_wDiscHeartBeatTime = (u16)nValue;
    }
    
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatNum", DEF_OSPDISC_HBNUM, &nValue );
    if (nValue >= 1)
    {
        m_byDiscHeartBeatNum = (u8)nValue;
    }    

	return( TRUE );
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
BOOL CDriConfig::SetPrsConfig( TEqpPrsEntry *ptPrsCfg )
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

