/*****************************************************************************
   模块名      : Board Agent
   文件名      : criconfig.cpp
   相关文件    : 
   文件实现功能: 单板代理提供给上层应用的接口实现
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/09/12  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#include "osp.h"
#include "cri_config.h"

// 构造函数
CCriConfig::CCriConfig()
{
	bIsRunPrs = FALSE;
}

// 析构函数
CCriConfig::~CCriConfig()
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
BOOL CCriConfig::IsRunPrs()
{
	return bIsRunPrs;
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
BOOL CCriConfig::GetPrsCfg( TPrsCfg* ptCfg )
{
	if( ptCfg == NULL )
		return FALSE;

	if( bIsRunPrs == FALSE)
		return FALSE;

	*ptCfg = m_prsCfg;
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
BOOL CCriConfig::SetPrsConfig( TEqpPrsEntry *ptPrsCfg )
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

    m_prsCfg.dwConnectIpB = GetMpcIpB();
    m_prsCfg.wConnectPortB = GetMpcPortB();
	return TRUE;
}

/*=============================================================================
函 数 名： IsPrsCfgEqual
功    能： 
算法实现： 
全局变量： 
参    数： const TPrsCfg &tPrsCfg
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/18   4.0			周广程                  创建
=============================================================================*/
BOOL32 CCriConfig::IsPrsCfgEqual( const TPrsCfg &tPrsCfg)
{
	if ( tPrsCfg.m_wFirstTimeSpan == m_prsCfg.m_wFirstTimeSpan
		&& tPrsCfg.m_wRejectTimeSpan == m_prsCfg.m_wRejectTimeSpan
		&& tPrsCfg.m_wSecondTimeSpan == m_prsCfg.m_wSecondTimeSpan
		&& tPrsCfg.m_wThirdTimeSpan == m_prsCfg.m_wThirdTimeSpan )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CCriConfig::GetBoardSequenceNum( u8 *pBuf, s8* achSep )
{
//#ifndef WIN32
	TBrdEthParam tBrdEthParam;

#if defined (_IS22_)
	s8    achProfileName[32] = {0};
	s32   dwFlag;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

	BOOL32 bResult = GetRegKeyInt(achProfileName, SECTION_IsFront, KEY_Flag, 1, &dwFlag);
	 if(FALSE == bResult)
	 {
		 printf( "[GetBoardSequenceNum] Wrong profile while reading %s!\n", "IsFront" );
		 dwFlag = 1;
	 }
	u8 byEthId = (u8)dwFlag;   // 0: front , 1: back
	printf("===========Is2.2 mtadp eth choice.%d===========\n",byEthId);
#else

	u8 byEthId = GetBrdEthChoice();//  GetInterface(); // 前后网口选择
#endif

	
    BrdGetEthParam( byEthId, &tBrdEthParam );
	
	s8 achMacBuf[64];
	memset( achMacBuf, 0, sizeof(achMacBuf) );
    s8 szTmp[3] = {0};
	u8 byLp = 0;
	while(byLp < 6)
	{
        sprintf(szTmp, "%.2X", tBrdEthParam.byMacAdrs[byLp] );
        strcat( achMacBuf, szTmp);
        if (byLp < 5)
        {
            // guzh [7/3/2007] 自定义分割符
            strcat(achMacBuf, achSep);
        }
        byLp ++;
	}
	
	memcpy( pBuf, achMacBuf, strlen(achMacBuf) );
	
	return;
}

/*====================================================================
    函数名      : InitLocalCfgInfoByCfgFile
    功能        : 依配置文件初始化本地配置信息
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2012/05/05  4.7         liaokang      创建
====================================================================*/
BOOL32 CCriConfig::InitLocalCfgInfoByCfgFile()
{    
    s8     achProfileName[32] = {0};
    BOOL32 bResult = FALSE;
    s8     achDefStr[] = "Cannot find the section or key";
    s8     achReturn[MAX_VALUE_LEN + 1] = {0};
    s32    nValue = 0;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    // 支持通过配置文件配置单板主系统ip
    BOOL32 bIsSupportIpConfig = 0;         // 是否支持配置单板主系统 否：0，是：1
    u8     byEthChoice = 1;                // 单板主系统 前/后网口  前网口：0，后网口：1
    u32    dwBrdMasterSysIP = 0;           // 单板主系统 primary ip
    u32    dwBrdMasterSysInnerIP = 0;      // 单板主系统 内部通信(second ip)
    u32    dwBrdMasterSysIPMask = 0;       // 单板主系统 掩码
    u32    dwBrdMasterSysDefGateway = 0;   // 单板主系统 默认网关
    TBrdEthParam tEthParam;
    memset( &tEthParam, 0, sizeof( TBrdEthParam ) );
    TBrdEthParamAll tBrdEthParamAll;
    memset( &tBrdEthParamAll, 0, sizeof( TBrdEthParamAll ) );

    // 1、是否支持配置单板主系统
    bResult = GetRegKeyInt( achProfileName, SECTION_IpConfig, KEY_IsSupportIpConfig, DEFVALUE_IsSupportIpConfig, &nValue );
    if( bResult == FALSE )  
    {
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_IsSupportIpConfig );
        return FALSE;
    }
	bIsSupportIpConfig = ( BOOL )nValue; 
    if ( 0 == bIsSupportIpConfig ) // 不支持
    {
        LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Do not support Ip Config in config file!\n" );
        return TRUE;
    }



    // 2、单板主系统网关
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysDefGateway, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysDefGateway );
    }
    dwBrdMasterSysDefGateway =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdMasterSysDefGateway || 0 == dwBrdMasterSysDefGateway )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, 
            "[InitLocalCfgInfoByCfgFile] The default gateway is 0 in config file\n" );
#ifdef _LINUX_
        u32 dwDefGateway = ntohl( BrdGetDefGateway() );
        if ( 0 == dwDefGateway )
        {
            LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Get default gateway failed!\n" );
            return FALSE;
        }  
#endif
    }
    else
    {
#ifdef _LINUX_
        // 设置单板主系统网关
        s32 nRet = BrdSetDefGateway( htonl( dwBrdMasterSysDefGateway ) );
        if ( ERROR == nRet )
        {
            LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, 
                "[InitLocalCfgInfoByCfgFile] Set default gateway : %0x failed!\n", dwBrdMasterSysDefGateway );
            return FALSE;
        }
        else
        {
            LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set default gateway : %0x success!\n", dwBrdMasterSysDefGateway );
        }
#endif
    }

    // 3、单板主系统 前/后网口,默认后网口
    bResult = GetRegKeyInt( achProfileName, SECTION_IpConfig, KEY_EthChoice, DEFVALUE_EthChoice, &nValue );
    if( bResult == FALSE )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_EthChoice );
    }
	byEthChoice = ( u8 )nValue; 
    LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set Ethernet Choice : %u!\n", byEthChoice );  

    // 4、单板主系统 掩码
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysIPMask, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysIPMask );
    }
    dwBrdMasterSysIPMask =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdMasterSysIPMask || 0 == dwBrdMasterSysIPMask )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] The ip mask is 0 in config file!\n" );
        // 默认采用原有掩码
        memset( &tBrdEthParamAll, 0, sizeof( TBrdEthParamAll ) );
        s32 nRet = BrdGetEthParamAll( byEthChoice, &tBrdEthParamAll );
        if ( ERROR == nRet
            || tBrdEthParamAll.dwIpNum == 0 
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpMask ) )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,
                "[InitLocalCfgInfo] The primary IpMask is not exist in board system! Please config...\n" );
            return FALSE;
        }
        dwBrdMasterSysIPMask = ntohl( tBrdEthParamAll.atBrdEthParam[0].dwIpMask );
    }

    // 5、单板主系统 ip
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysIp, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysIp );
    }
    dwBrdMasterSysIP =  ntohl( INET_ADDR( achReturn ) );
     /* 添加第二IP之前，第一IP必须存在 */
    if( 0xffffffff == dwBrdMasterSysIP || 0 == dwBrdMasterSysIP )
    { 
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] The primary IpAddr is 0 in config file!\n" );  
#ifdef _LINUX_
        // 判断 单板主系统 primary ip 是否已经存在        
        memset( &tBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
        s32 nRet = BrdGetEthParamAll( byEthChoice, &tBrdEthParamAll );
        if ( ERROR == nRet
            || tBrdEthParamAll.dwIpNum == 0 
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs )
            || ( tBrdEthParamAll.dwIpNum > 0 && 0 == tBrdEthParamAll.atBrdEthParam[0].dwIpMask ) )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,
                "[InitLocalCfgInfo] The primary IpAddr or IpMask is not exist in board system! Please config...\n" );
            return FALSE;
        }
#endif
    }
    else
    {
#ifdef _LINUX_
        // 设置单板主系统 primary ip
        memset( &tEthParam, 0, sizeof(tEthParam) );
        tEthParam.dwIpAdrs = htonl( dwBrdMasterSysIP );
        tEthParam.dwIpMask = htonl( dwBrdMasterSysIPMask );        
        s32 nRet = BrdSetEthParam( byEthChoice, Brd_SET_IP_AND_MASK, &tEthParam );
        if ( ERROR == nRet )
        {
            LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set primary IpAddr:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysIP, dwBrdMasterSysIPMask, byEthChoice );
            return FALSE;       
        }
        else
        {
            LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                 "[InitLocalCfgInfoByCfgFile] Set primary IpAddr:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysIP, dwBrdMasterSysIPMask, byEthChoice );
        }
#endif
    }

    // 6、单板主系统 内部通信 ip
    bResult = GetRegKeyString( achProfileName, SECTION_IpConfig, KEY_BrdMasterSysInnerIp, 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[InitLocalCfgInfoByCfgFile] Wrong profile while reading %s!\n", KEY_BrdMasterSysInnerIp );
        return FALSE;
    }
    dwBrdMasterSysInnerIP =  ntohl( INET_ADDR( achReturn ) );
    if( 0xffffffff == dwBrdMasterSysInnerIP ||  0 == dwBrdMasterSysInnerIP )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, 
            "[InitLocalCfgInfoByCfgFile] The inner ip is 0 in config file!\n" );
        return FALSE;
    }
    // 设置单板主系统内部通信ip
#ifdef _LINUX12_  // 目前仅nipwrapper.h支持第二IP
    /* 直接添加second ip 可能结果: 如存在同网段ip，则冲突返回失败
                                   如不存在，则添加一个second ip */ 

    /* 约定 内部通信 ip 作为 second ip 的第一个ip */ 
    
    memset( &tBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
    s32 nRet = BrdGetEthParamSecIP( byEthChoice, &tBrdEthParamAll );
    // 不存在second ip
    if ( OK == nRet && 0 == tBrdEthParamAll.dwIpNum )
    {
        // 添加内部通信 ip
        memset( &tEthParam, 0, sizeof(tEthParam) );
        tEthParam.dwIpAdrs = htonl( dwBrdMasterSysInnerIP );
        tEthParam.dwIpMask = htonl( dwBrdMasterSysIPMask ); 
        nRet = BrdSetEthParam( byEthChoice, Brd_SET_ETH_SEC_IP, &tEthParam );
        if ( ERROR == nRet )
        {
            LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set inner Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
        }
        else
        {
            LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                "[InitLocalCfgInfoByCfgFile] Set inner Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
        }
        return FALSE;
    }
    // 存在second ip
    else if ( OK == nRet && tBrdEthParamAll.dwIpNum > 0 )
    {
        // <1> 按序记录到临时数组
        TBrdEthParamAll tTempBrdEthParamAll;
        memset( &tTempBrdEthParamAll, 0, sizeof(TBrdEthParamAll) );
        // 将需设置的内部通信 ip存储到临时数组第0位
        tTempBrdEthParamAll.atBrdEthParam[0].dwIpAdrs = htonl( dwBrdMasterSysInnerIP );
        tTempBrdEthParamAll.atBrdEthParam[0].dwIpMask = htonl( dwBrdMasterSysIPMask );
        tTempBrdEthParamAll.dwIpNum = 1;
        // 按顺序将第1到Num - 1位的ip存储到临时数组的第1到Num - 1位
        for ( u8 byLop = 1; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
        {
            tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs = tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpMask = tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask;
            tTempBrdEthParamAll.dwIpNum ++;
        }
        // <2> 删除原有second ip 
        for ( u8 byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
        {
            memset( &tEthParam, 0, sizeof(tEthParam) );
            tEthParam.dwIpAdrs = tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tEthParam.dwIpMask = tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask; 
            nRet = BrdDelEthParamSecIP( byEthChoice, &tEthParam );
            if ( ERROR == nRet )
            {
                LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
            else
            {
                LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
        }
        // <3> 设置临时数组中所记录的second ip
        for ( u8 byLop = 0; byLop < tTempBrdEthParamAll.dwIpNum; byLop++ )
        {
            memset( &tEthParam, 0, sizeof(tEthParam) );
            tEthParam.dwIpAdrs = tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs;
            tEthParam.dwIpMask = tTempBrdEthParamAll.atBrdEthParam[byLop].dwIpMask; 
            nRet = BrdSetEthParam( byEthChoice, Brd_SET_ETH_SEC_IP, &tEthParam );
            if ( ERROR == nRet )
            {
                LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u failed!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
            else
            {
                LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS,
                    "[InitLocalCfgInfoByCfgFile] Set second Ip:%0x, IPMask:%0x in Ethernet Choice : %u success!\n", dwBrdMasterSysInnerIP, dwBrdMasterSysIPMask, byEthChoice );
            }
        }
    }
#endif //end _LINUX12_

    return TRUE;
}

/*====================================================================
    函数名      : SetBrdMasterSysListenPort
    功能        : 设置单板主系统的监听端口
    算法实现    :
    引用全局变量:
    输入参数说明: u16 wListenPort 监听端口号
    返回值说明  : BOOL32 
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2012/05/05  4.7         liaokang      创建
====================================================================*/
BOOL32 CCriConfig::SetBrdMasterSysListenPort( u16 wListenPort )
{
    s8     achProfileName[32] = {0};
    BOOL32 bResult = FALSE;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);
    // 设置主系统（IS2.2 8548） Listen Port
    bResult = SetRegKeyInt( achProfileName, SECTION_BoardMasterSystem, KEY_BrdMasterSysListenPort, wListenPort );
    if( bResult == FALSE )
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[GetBrdMasterSysListenPort] Wrong profile while writeing %s!\n", KEY_BrdMasterSysListenPort );
    }  
    return bResult;
}

/*====================================================================
    函数名      : GetBrdMasterSysListenPort
    功能        : 获取单板主系统的监听端口
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  : u16 监听端口号
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2012/05/05  4.7         liaokang      创建
====================================================================*/
u16 CCriConfig::GetBrdMasterSysListenPort(void)
{
    s8     achProfileName[32] = {0};
    BOOL32 bResult = FALSE;
    s32    nValue = 0;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    // 获取主系统（IS2.2 8548） Listen Port
    bResult = GetRegKeyInt( achProfileName, SECTION_BoardMasterSystem, KEY_BrdMasterSysListenPort, 
        DEFVALUE_BrdMasterSysListenPort, &nValue );
    if( bResult == FALSE )  
    {
        LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,
            "[GetBrdMasterSysListenPort] Wrong profile while reading %s!\n", KEY_BrdMasterSysListenPort );
    }  
    return ( u16 )nValue;
}
