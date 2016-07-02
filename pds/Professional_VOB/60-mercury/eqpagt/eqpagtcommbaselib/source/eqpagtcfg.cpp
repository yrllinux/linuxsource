/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtcfg.cpp
    相关文件    : 
    文件实现功能: 配置设备代理信息
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#include "eqpagtcfg.h"

CEqpAgtCfg	g_cEqpAgtCfg;
CEqpAgtCfgDefValueList   g_cEqpAgtCfgDefValueList;
// 信号量保护
SEMHANDLE   g_hEqpAgtCfg;


// 构造
CEqpAgtCfg::CEqpAgtCfg() : m_byTrapSrvNum(0)
{
    memset( m_achCfgPath, '\0', sizeof(m_achCfgPath));
    memset( m_achCfgName, '\0', sizeof(m_achCfgName));
/*    memset( m_achCfgBakName, '\0', sizeof(m_achCfgBakName));*/
    memset( m_atEqpAgtInfoTable, '\0', sizeof(m_atEqpAgtInfoTable));
}

// 析构
CEqpAgtCfg::~CEqpAgtCfg()
{
}

/*====================================================================
    函数名      : EqpAgtCfgInit
    功能        : 初始化配置文件数据区
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszCfgFileFullPath  配置文件全路径
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::EqpAgtCfgInit( LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{
    if( NULL == lpszCfgPath || NULL == lpszCfgName )
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtCfgInit] The param is null!\n");
        return FALSE;
    }

    // 记录配置文件全路径
    memcpy( m_achCfgPath, lpszCfgPath, sizeof( m_achCfgPath ) );
    memcpy( m_achCfgName, lpszCfgName, sizeof( m_achCfgName ) );
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    // eqpagt配置文件名
    sprintf( achProfileName, "%s/%s", m_achCfgPath, m_achCfgName );
    // 1、读Trap表
    BOOL32 bRet = TRUE;
    bRet = ReadTrapTable( achProfileName );
    if ( bRet )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtCfgInit] Read cfg file successed!\n" );
        return TRUE;
    }
  
    // 生成默认配置文件
    if( TRUE == PretreatCfgFile( m_achCfgPath, m_achCfgName ) )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtCfgInit] Create default cfg file successed!\n" );	
    }
    else
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtCfgInit] Create default cfg file failed!\n" );
        return FALSE;
    }

    return TRUE;
}

/*====================================================================
    函数名      : ReadEqpAgtInfoTable
    功能        : 整体读EqpAgt信息表
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: u8 byTrapSrvNum                   trap服务器总数
                  TEqpAgtInfo* ptEqpAgtInfoTable    EqpAgt信息表
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::ReadEqpAgtInfoTable( u8* pbyTrapSrvNum, 
                                       TEqpAgtInfo* ptEqpAgtInfoTable )
{
    // 配置信息读/写信号量保护
    ENTER( g_hEqpAgtCfg );

    BOOL32 bResult = TRUE;
    if( NULL == pbyTrapSrvNum || NULL == ptEqpAgtInfoTable )
    {
        EqpAgtLog( LOG_ERROR, "[ReadTrapTable] The param is null!\n");
        bResult =  FALSE;
        return bResult;
    }

    // 从配置文件中读取（防止写文件时，文件内容和变量不统一）
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    // eqpagt配置文件名
    sprintf( achProfileName, "%s/%s", m_achCfgPath, m_achCfgName );
    ReadTrapTable( achProfileName );
    
    *pbyTrapSrvNum = m_byTrapSrvNum;
    EqpAgtLog( LOG_KEYSTATUS, "[ReadTrapTable] The trap server num: %d !\n", m_byTrapSrvNum );
    for( u8 byLoop = 0; byLoop < m_byTrapSrvNum; byLoop++ )
    {
        if( NULL != &ptEqpAgtInfoTable[byLoop] )
        {
            memcpy( &ptEqpAgtInfoTable[byLoop], &m_atEqpAgtInfoTable[byLoop], sizeof(TEqpAgtInfo) );
            EqpAgtLog( LOG_KEYSTATUS, "[ReadTrapTable] Read trap server id: %d !\n", byLoop );
        }
        else
        { 
            EqpAgtLog( LOG_ERROR, "[ReadTrapTable] The EqpAgtInfo list num is too small!\n");
            bResult =  FALSE;
        }
    }
    return bResult;
}

/*====================================================================
    函数名      : WriteEqpAgtInfoTable
    功能        : 整体写EqpAgt信息表
    算法实现    :
    引用全局变量:
    输入参数说明: u8 byTrapSrvNum                   trap服务器总数
                  TEqpAgtInfo* ptEqpAgtInfoTable    EqpAgt信息表
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::WriteEqpAgtInfoTable( u8 byTrapSrvNum, 
                                        TEqpAgtInfo* ptEqpAgtInfoTable )
{
    // 配置信息读/写信号量保护
    ENTER( g_hEqpAgtCfg );

    BOOL32 bRet = TRUE;
    s8  szEqpAgtSectionInfo[MAX_WRITE_SECTIONONECE_LEN ] = {'\0'};
    s8  szRowEqpAgtInfo[MAX_VALUE_LEN + 1 ]={'\0'};
    u16 wSectionOffSet = 0;

    // 判断指针是否为空
    if( NULL == ptEqpAgtInfoTable )
    {
        EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] The param is null!\n");
        return FALSE;
    }
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    sprintf( achProfileName, "%s/%s", m_achCfgPath, m_achCfgName );

    // 写Trap服务器数目
    bRet = SetRegKeyInt( achProfileName, (s8*)SECTION_EqpAgtInfoTable, (s8*)ENTRY_NUM, (s32)byTrapSrvNum );
    if( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] Fail to write profile in %s\n", SECTION_EqpAgtInfoTable );
        return FALSE;
    }

    // 写配置成功后，同步更新变量，防止更新了变量但写配置不成功
    m_byTrapSrvNum = byTrapSrvNum;

    // 写EqpAgt信息表项
    for( u8 byLoop = 0; byLoop < m_byTrapSrvNum; byLoop++ )
    {
        memset(szRowEqpAgtInfo, 0, sizeof(szRowEqpAgtInfo));
        bRet = OrganizeEqpAgtInfoTable( byLoop, &ptEqpAgtInfoTable[byLoop], szRowEqpAgtInfo );
        if (strlen(szRowEqpAgtInfo) > MAX_VALUE_LEN  )
        {
            szRowEqpAgtInfo[MAX_VALUE_LEN] = '\0';
        }
        //写的内容多于2k，分为多块写
        if ( (strlen(szEqpAgtSectionInfo) + strlen(szRowEqpAgtInfo)) > MAX_WRITE_SECTIONONECE_LEN  )
        {
            if ( !SetRegKeySection(achProfileName, (s8*)SECTION_EqpAgtInfoTable, szEqpAgtSectionInfo, wSectionOffSet, ENTRY_NUM) )
            {
                EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] SetRegKeySection error!\n");
            }
            wSectionOffSet += strlen(szEqpAgtSectionInfo);
            memset(szEqpAgtSectionInfo, 0, sizeof(szEqpAgtSectionInfo));            
        }
     	sprintf( szEqpAgtSectionInfo, "%s%s", szEqpAgtSectionInfo, szRowEqpAgtInfo);
        if( !bRet )
        {
            break;            
		}
    }

    if ( strlen(szEqpAgtSectionInfo) > 0 )
    {
        if ( !SetRegKeySection(achProfileName, (s8*)SECTION_EqpAgtInfoTable, szEqpAgtSectionInfo, wSectionOffSet, ENTRY_NUM) )
        {
            EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] SetRegKeySection error!\n");
            return FALSE;
        }
        // 写配置成功后，同步更新变量，防止更新了变量但写配置不成功
        memcpy( m_atEqpAgtInfoTable, ptEqpAgtInfoTable, sizeof(m_atEqpAgtInfoTable) );    
	 }
    else
    {
        if ( !SetRegKeySection(achProfileName, (s8*)SECTION_EqpAgtInfoTable, NULL, wSectionOffSet, ENTRY_NUM) )
        {
            EqpAgtLog( LOG_ERROR, "[WriteEqpAgtInfoTable] SetRegKeySection error!\n");
            return FALSE;
        }
        // 写配置成功后，同步更新变量，防止更新了变量但写配置不成功
        memset( &m_atEqpAgtInfoTable, 0, sizeof(m_atEqpAgtInfoTable) ); 
    }
    return TRUE;
}

/*====================================================================
    函数名      : TrapIpsOfEqpAgt2Nms
    功能        : Trap Ip信息 EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::TrapIpsOfEqpAgt2Nms( void *pBuf, u16 *pwBufLen )
{
    BOOL32 bRet = TRUE;
    // 读配置，防止配置与变量不符
    u8 byTrapSrvNum = 0;
    TEqpAgtInfo tEqpAgtInfoTable[MAXNUM_EQPAGTINFO_LIST];
    memset( tEqpAgtInfoTable, 0, sizeof(tEqpAgtInfoTable));
    bRet = ReadEqpAgtInfoTable( &byTrapSrvNum, tEqpAgtInfoTable );
    if ( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[TrapIpsOfEqpAgt2Nms] ReadEqpAgtInfoTable wrong!\n" );
        return FALSE;
    }
    
    // pBuf 结构：
    // u8(N个 IP) + u32(第一个IP) + …… + u32(第N个IP)

    u32 dwTempIp = 0;
    s8  abyTrapIps[1024] = {'\0'};
    s8  abyBuf[1024] = {'\0'};
    s8 *pBuff = abyBuf;
    pBuff += sprintf(pBuff, "[TrapIpsOfEqpAgt2Nms] Trap server Ip Info: total num %d\n", byTrapSrvNum);

    // 1、u8(N个 IP)
    memcpy( abyTrapIps, &byTrapSrvNum, sizeof(u8) );
    // 2、记录N个 IP
    if ( 0 < byTrapSrvNum )
    {
        for ( u8 byLoop = 0; byLoop < byTrapSrvNum; byLoop++)
        {
            dwTempIp = htonl(tEqpAgtInfoTable[byLoop].GetTrapIp());
            pBuff += sprintf(pBuff, "No: %u, Ip(net): %x\n", byLoop, dwTempIp);
            // 取IP并赋值
            memcpy( abyTrapIps + sizeof(u8) + sizeof(u32) * byLoop, &dwTempIp, sizeof(u32) );
        }
    }
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u32) * byTrapSrvNum); 
    memcpy( pBuf, abyTrapIps, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, abyBuf);
    return TRUE;
}


/*====================================================================
    函数名      : TrapIpsOfNms2EqpAgt
    功能        : Trap Ip信息 Nms ----> EqpAgt
    算法实现    : 因为需求，此处仅设置trap ip，其余采用默认值
    引用全局变量:
    输入参数说明: void * pBuf:       输入数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::TrapIpsOfNms2EqpAgt( void *pBuf, u16 *pwBufLen )
{
    // pBuf 结构：
    // u8(N个 IP) + u32(第一个IP) + …… + u32(第N个IP)

    BOOL32 bRet = TRUE;
    u32 dwTempIp = 0;
    s8  abyTrapIps[sizeof(u32)*MAXNUM_EQPAGTINFO_LIST+1+1] = {'\0'};
    u8 byLoop = 0;
    u8 byTrapSrvNum = 0;
    TEqpAgtInfo tEqpAgtInfoTable[MAXNUM_EQPAGTINFO_LIST];	// EqpAgt信息表    
    memcpy( abyTrapIps, pBuf, *pwBufLen );
    memcpy( &byTrapSrvNum, abyTrapIps, sizeof(byTrapSrvNum));
    EqpAgtLog( LOG_KEYSTATUS, "[TrapIpsOfNms2EqpAgt] Total Trap server num :%d\n", byTrapSrvNum);

    // 清空IP记录（置为0）
    for ( byLoop = 0; byLoop < m_byTrapSrvNum; byLoop++)
    {
        tEqpAgtInfoTable[byLoop].SetTrapIp( 0 );
    }
    
    // 记录IP
    for ( byLoop = 0; byLoop < byTrapSrvNum; byLoop++)
    {
        memcpy( &dwTempIp, abyTrapIps + sizeof(u8) + sizeof(u32) * byLoop, sizeof(dwTempIp));
        dwTempIp = ntohl( dwTempIp );
        EqpAgtLog( LOG_KEYSTATUS, "[TrapIpsOfNms2EqpAgt] Trap server No: %u, Ip(host order): %x \n", byLoop, dwTempIp);
        tEqpAgtInfoTable[byLoop].SetTrapIp( dwTempIp );
    }

    bRet = WriteEqpAgtInfoTable( byTrapSrvNum, tEqpAgtInfoTable );
    if( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[TrapIpsOfNms2EqpAgt] Write EqpAgt info table failed!\n" );
        return FALSE;
    }

    return TRUE;
}

/*====================================================================
    函数名      : GetTrapServerNum
    功能        : 获取Trap服务器数
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : u8 m_byTrapSrvNum Trap服务器数
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u8 CEqpAgtCfg::GetTrapServerNum( void )
{
    return m_byTrapSrvNum;
}

/*====================================================================
    函数名      : GetTrapTarget
    功能        : 取某一Trap服务器对应的Trap信息结构
    算法实现    :
    引用全局变量:
    输入参数说明: u8 byIdx              序号
    输出参数说明: TTarget& tTrapTarget  对应的Trap信息结构
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtCfg::GetTrapTarget( u8 byIdx, TTarget& tTrapTarget )
{
    tTrapTarget.dwIp = m_atEqpAgtInfoTable[byIdx].GetTrapIp();
    tTrapTarget.wPort = m_atEqpAgtInfoTable[byIdx].GetTrapPort();
    memcpy( tTrapTarget.achCommunity, 
        m_atEqpAgtInfoTable[byIdx].GetReadCom(), 
        strlen(m_atEqpAgtInfoTable[byIdx].GetReadCom()) );
    return;
}

/*====================================================================
    函数名      : GetSnmpParam
    功能        : 取SNMP参数
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: TSnmpAdpParam& tParam   snmp参数
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::GetSnmpParam( TSnmpAdpParam& tParam ) const
{
    BOOL32 bRet = FALSE;
    memset( &tParam, 0, sizeof(tParam) );
    
    if( 0 != m_byTrapSrvNum )
    {
        tParam.dwLocalIp = htonl(m_atEqpAgtInfoTable[0].GetTrapIp());
        
        TEqpAgtInfo tEqpAgtInfo;
        memcpy( &tEqpAgtInfo, &m_atEqpAgtInfoTable[0], sizeof(tEqpAgtInfo) );
        
        strncpy( tParam.achReadCommunity, tEqpAgtInfo.GetReadCom(), MAX_COMMUNITY_LEN );
        
        tParam.achReadCommunity[MAX_COMMUNITY_LEN-1] = '\0';
        
        strncpy( tParam.achWriteCommunity, tEqpAgtInfo.GetWriteCom(), MAX_COMMUNITY_LEN );
        
        tParam.achWriteCommunity[MAX_COMMUNITY_LEN-1] = '\0';
        
        tParam.wGetSetPort = m_atEqpAgtInfoTable[0].GetGSPort();
        tParam.wTrapPort = m_atEqpAgtInfoTable[0].GetTrapPort();
        
        EqpAgtLog( LOG_KEYSTATUS, "GetSnmpParam succeed!\n");
       
        bRet = TRUE;
    }
    else
    {
        memset(&tParam, 0, sizeof(tParam));
    }
    
	return bRet;
}

/*====================================================================
    函数名      : HasSnmpNms
    功能        : 是否配置了网管
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::HasSnmpNms( void ) const
{
    return ( 0 != m_byTrapSrvNum );
}


/*====================================================================
    函数名      : OrganizeEqpAgtInfoTable
    功能        : 组织用于写文件的单个EqpAgt服务器表项
    算法实现    :
    引用全局变量:
    输入参数说明: u8 byIdx                  序号
                  TEqpAgtInfo* pEqpAgtInfo  单个EqpAgt服务器表项内容
    输出参数说明: s8* ptInfTobeWrited       用于写文件的单个EqpAgt服务器表项内容
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE 
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::OrganizeEqpAgtInfoTable(u8 byIdx,
                                           TEqpAgtInfo* pEqpAgtInfo,
                                           s8* ptInfTobeWrited )
{
    if ( MAXNUM_EQPAGTINFO_LIST <= byIdx 
        || NULL == pEqpAgtInfo 
        || NULL == ptInfTobeWrited )
    {
        BOOL32 bRet1 = (NULL == pEqpAgtInfo);
        BOOL32 bRet2 = (NULL == ptInfTobeWrited);
        EqpAgtLog( LOG_ERROR, "[OrganizeEqpAgtInfoTable] organize error,byIdx:%d,pEqpAgtInfo Null?:%d,ptInfTobeWrited Null?:%d",
            byIdx, bRet1, bRet2 );
        return FALSE;
    }
    
    s8  achSnmpIp[32];
    s8  achEntryStr[32];
    memset(achSnmpIp, '\0', sizeof(achSnmpIp));
    memset(achEntryStr, '\0', sizeof(achEntryStr));    
    
    sprintf(achEntryStr, "%s%d", ENTRY_KEY, byIdx);
    GetIpFromU32( achSnmpIp, pEqpAgtInfo->GetTrapIp() );
    
    sprintf(ptInfTobeWrited, 
        "%s = %s\t%s\t%s\t%d\t%d\n", 
        achEntryStr,
        achSnmpIp,
        pEqpAgtInfo->GetReadCom(), 
        pEqpAgtInfo->GetWriteCom(),
        pEqpAgtInfo->GetGSPort(),
        pEqpAgtInfo->GetTrapPort() );
    return TRUE;
} 

/*====================================================================
    函数名      : ReadTrapTable
    功能        : 读配置文件
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszCfgFileFullPath  文件全路径
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE 
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::ReadTrapTable( LPCSTR lpszCfgFileFullPath )
{
    // 判断配置文件是否存在、读配置文件
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( lpszCfgFileFullPath, "r" );
    if( NULL != hFileR ) // exist
    {
        // 读配置文件        
        s8    achSeps[] = " \t";       // 分隔符
        s8    *pchToken = NULL;
        s32    nMemEntryNum = 0;
        s8     achPortStr[32];        // 临时存放字符串
        memset(achPortStr, '\0', sizeof(achPortStr));
        
        BOOL32 bSucceedRead = TRUE;
        BOOL32 bResult = GetRegKeyInt( lpszCfgFileFullPath, SECTION_EqpAgtInfoTable, 
                                     ENTRY_NUM, 0, &nMemEntryNum );
        if( !bResult ) 
        {
            EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s %s!\n", 
                SECTION_EqpAgtInfoTable, ENTRY_NUM );
            bSucceedRead = FALSE;
        }

        // 接收TRAP陷阱的服务器表项数为0，则可直接退出
        m_byTrapSrvNum = (u8)nMemEntryNum;
        if( 0 < nMemEntryNum ) 
        {
            // 分配空间
            s8* *ppszTable = new s8*[(u32)nMemEntryNum];
            if( NULL == ppszTable )
            {
                EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Error to alloc memory!\n" );
                return FALSE;
            }
            
            u32  dwLoop = 0;
            for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
            {
                ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
                if( NULL == ppszTable[dwLoop] )
                {
                    FREE_TABLE_MEM( ppszTable, dwLoop );
                    return FALSE;
                }
            }
            
            // 取整个表
			bResult = GetRegKeyStringTable(lpszCfgFileFullPath, SECTION_EqpAgtInfoTable,
				"fail", ppszTable, (u32 *)&nMemEntryNum, MAX_VALUE_LEN + 1 );
			if( !bResult)
			{
				FREE_TABLE_MEM( ppszTable, nMemEntryNum );
				EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s snmp infomation!\n", SECTION_EqpAgtInfoTable );				
				m_byTrapSrvNum = 0;
				return FALSE;
			}
            
            // 分析字串
            for( dwLoop = 0; dwLoop < m_byTrapSrvNum; dwLoop++ )
            {
                // IpAddr 
                pchToken = strtok( ppszTable[dwLoop], achSeps );
                if( NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_TrapSrvIpAddr );				
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    m_atEqpAgtInfoTable[dwLoop].SetTrapIp( ntohl(INET_ADDR( pchToken )) );
                }
                
                // 读共同体
                pchToken = strtok( NULL, achSeps );
                if( NULL == pchToken )
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_ReadCommunity );				
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    m_atEqpAgtInfoTable[dwLoop].SetReadCom( pchToken );
                }
                
                // 写共同体
                pchToken = strtok(NULL, achSeps);
                if(NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_WriteCommunity );
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    m_atEqpAgtInfoTable[dwLoop].SetWriteCom( pchToken );
                }
                
                // 取、设置端口
                pchToken = strtok(NULL, achSeps);
                if(NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_GetSetPort );
                    m_atEqpAgtInfoTable[dwLoop].SetGSPort( SNMP_PORT );
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    strncpy( achPortStr, pchToken, MAXLEN_PORT);
                    m_atEqpAgtInfoTable[dwLoop].SetGSPort( atoi(achPortStr) );
                }
                
                // Trap 端口
                memset(achPortStr, '\0', MAXLEN_PORT);
                pchToken = strtok(NULL, achSeps);
                if(NULL == pchToken)
                {
                    EqpAgtLog( LOG_ERROR, "[ReadCfgFile] Wrong profile while reading %s !\n", FIELD_SendTrapPort );
                    m_atEqpAgtInfoTable[dwLoop].SetTrapPort( TRAP_PORT );
                    bSucceedRead = FALSE;
                    continue;
                }
                else
                {
                    strncpy(achPortStr, pchToken, MAXLEN_PORT);
                    m_atEqpAgtInfoTable[dwLoop].SetTrapPort( atoi(achPortStr) );
                }
            }
            // 释放临时空间
            FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        }

        fclose( hFileR );
/*lint -save -esym(438, hFileR)*/
        hFileR = NULL;
/*lint -restore*/
        return bSucceedRead;
    }
    else
    {
        m_byTrapSrvNum = 0;

#ifdef _LINUX_
        nFopenErr = errno;	
#endif
        
#ifdef WIN32
        nFopenErr = GetLastError(); 
#endif
        //打开配置文件失败的错误号和时间
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[ReadCfgFile] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
        return FALSE;
    }
}

/*====================================================================
    函数名      : CreateDir
    功能        : 创建目录
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR pPathName   默认目录名
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtCfg::CreateDir( LPCSTR pPathName )
{
#ifdef WIN32
    /* =======================================
    不进行路径比对原因:
    路径比对 c:\tt  但它是一个文件(无扩展名），
    路径比对也会得到真值，实际上目录并不存在
    ========================================*/
    CreateDirectory( ( LPCTSTR )pPathName, NULL );
#endif

#ifdef _LINUX_
    mkdir( ( s8* )pPathName, 700 );
#endif
    
	return;
}

/*====================================================================
    函数名      : PretreatCfgFile
    功能        : 创建默认配置文件
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszCfgPath   配置路径
                  LPCSTR lpszCfgName   配置文件名
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE 
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfg::PretreatCfgFile( LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{
    // 创建目录
    CreateDir( lpszCfgPath );
    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};
    // eqpagt配置文件名
    sprintf( achProfileName, "%s/%s", lpszCfgPath, lpszCfgName );

    FILE* hFileW = NULL;
    hFileW = fopen( achProfileName, "w" );
	if( NULL == hFileW )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatCfgFile] Create eqpagtcfg file failed!\n" );
		return FALSE;	
	}
    
    // 生成TRAP地址默认值
    BOOL32 bRet = SetRegKeyInt( achProfileName, SECTION_EqpAgtInfoTable, (s8*)ENTRY_NUM, 0 );
    if ( !bRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[PretreatCfgFile] Write default EqpAgtInfoTable entrynum failed!\n" );
    }

    u16 wFuncNum = g_cEqpAgtCfgDefValueList.Size();
    if ( 0!= wFuncNum )
    {
        TEqpAgtCfgDefValue pfTempFunc = NULL;
        for( u16 wLoop = 0; wLoop < wFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtCfgDefValueList.GetEqpAgtCfgDefValue( wLoop, &pfTempFunc );
            if ( bRet )
            {
                (*pfTempFunc)( achProfileName );
            }
        }
    }

	fclose(hFileW);	
/*lint -save -esym(438, hFileW)*/
    hFileW = NULL;
/*lint -restore*/    
    return TRUE;
}

// 构造
CEqpAgtCfgDefValueList::CEqpAgtCfgDefValueList():m_wEqpAgtCfgDefValueSize(0)
{
    m_pEqpAgtCfgDefValueHead = NULL;
}

// 析构
CEqpAgtCfgDefValueList::~CEqpAgtCfgDefValueList()
{
    m_wEqpAgtCfgDefValueSize = 0;
    m_pEqpAgtCfgDefValueHead = NULL;
}

/*====================================================================
    函数名      : GetEqpAgtCfgDefValue
    功能        : 获取对应序号的回调函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: u16 wIdx                   序号
    输出参数说明: TEqpAgtCfgDefValue* pfFunc     对应的回调函数指针
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCfgDefValueList::GetEqpAgtCfgDefValue( u16 wIdx, TEqpAgtCfgDefValue* pfFunc )
{
    EqpAgtCfgDefValue *pEqpAgtCfgDefValue = m_pEqpAgtCfgDefValueHead;
    while( NULL != pEqpAgtCfgDefValue ) 
    {
        if( wIdx == pEqpAgtCfgDefValue->m_wIdx ) 
        {
            *pfFunc = pEqpAgtCfgDefValue->m_pfFunc;
            return TRUE;
        } 
        pEqpAgtCfgDefValue = pEqpAgtCfgDefValue->m_pNext; 
    }

    EqpAgtLog( LOG_ERROR, "[GetEqpAgtCfgDefValue] Do not find right param match to the index %u !\n", wIdx );
    return FALSE;
}

/*====================================================================
    函数名      : PushBack
    功能        : 在链表末尾存储回调函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: TEqpAgtCfgDefValue pfFunc 回调函数指针
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtCfgDefValueList::PushBack( TEqpAgtCfgDefValue pfFunc )
{
    if( NULL == pfFunc )
    {
        EqpAgtLog( LOG_ERROR, "[CEqpAgtCfgDefValueList::PushBack] The input param is null!\n" );
        return;
    }
    
    EqpAgtCfgDefValue *pNew = new EqpAgtCfgDefValue[1];
    if( NULL == pNew )
    {
        EqpAgtLog( LOG_ERROR, "[CEqpAgtCfgDefValueList::PushBack] Allocate pNew Memory failed!\n" );
        return;
    }
    pNew->m_pfFunc = pfFunc;
    pNew->m_wIdx = m_wEqpAgtCfgDefValueSize;
    m_wEqpAgtCfgDefValueSize++;
    EqpAgtCfgDefValue *pTail = m_pEqpAgtCfgDefValueHead;
    if ( NULL == pTail )
    {
        pTail = pNew;
        pTail->m_pNext = NULL;
    }
    else
    {
        while ( NULL != pTail->m_pNext )
        {
            pTail = pTail->m_pNext;
        }
        pTail->m_pNext = pNew;
        pTail = pTail->m_pNext;
        pTail->m_pNext = NULL;
    }
    
    if( NULL == m_pEqpAgtCfgDefValueHead )
    {
        m_pEqpAgtCfgDefValueHead = pTail;
    }    
    return;
}

/*====================================================================
    函数名      : EqpAgtCfgDefValueOper
    功能        : 设置EqpAgtCfg配置文件默认值的回调函数注册
    算法实现    :
    引用全局变量: g_cEqpAgtCfgDefValueList
    输入参数说明: TEqpAgtCfgDefValue pfFunc 回调函数业务处理具体实现
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void EqpAgtCfgDefValueOper( TEqpAgtCfgDefValue pfFunc )
{
    // 1、判断链表中是否已经存在对应函数,防止反复注册同一函数
    u16 wFuncNum = g_cEqpAgtCfgDefValueList.Size();
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wFuncNum )
    {
        TEqpAgtCfgDefValue pfTempFunc = NULL;
        for( wLoop = 0; wLoop < wFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtCfgDefValueList.GetEqpAgtCfgDefValue( wLoop, &pfTempFunc );
            if ( bRet )
            {
                if( pfFunc == pfTempFunc ) 
                {                   
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        } 
    }    
    
    // 2、不存在，添加
    g_cEqpAgtCfgDefValueList.PushBack( pfFunc );
    return;
}