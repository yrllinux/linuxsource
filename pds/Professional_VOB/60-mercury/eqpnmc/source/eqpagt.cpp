/*****************************************************************************
    模块名      : EqpNmc
    文件名      : eqpagt.cpp
    相关文件    : 
    文件实现功能: EqpAgt接口及实现
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#include "eqpagt.h"

CEqpAgt g_cEqpAgt;
// 信号量保护
extern SEMHANDLE g_hEqpAgtCfg;

// 构造函数
CEqpAgt::CEqpAgt()
{
    memset( &m_achIpCfgFile, 0, sizeof(m_achIpCfgFile));
    memset( &m_tNetAdaptInfoAll, 0, sizeof(m_tNetAdaptInfoAll));
}

// 析构函数
CEqpAgt::~CEqpAgt()
{
    memset( &m_achIpCfgFile, 0, sizeof(m_achIpCfgFile));
    memset( &m_tNetAdaptInfoAll, 0, sizeof(m_tNetAdaptInfoAll));
}

/*====================================================================
    函数名      : NmsSetSysState
    功能        : 网管设置系统状态（主要用于reboot）
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwSysState 系统状态
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void NmsSetSysState( u32 dwSysState )
{
    EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] System State: %u!\n", dwSysState);

    BOOL32 bRet = TRUE;

    if ( EQP_SYSSTATE_REBOOT == dwSysState )
    {
#ifdef WIN32
        u32 dwVersion;          // 版本号
        dwVersion = GetVersion(); //得到WINDOWS NT或Win32的版本号
        if(dwVersion < 0x80000000)
        {
            // 先提升权限
            // 重启关机需要SE_SHUTDOWN_NAME权限
            // 远程关机需要SE_REMOTE_SHUTDOWN_NAME权限
            // 调试程序需要提升到SE_DEBUG_NAME权限。
            HANDLE hToken;
            TOKEN_PRIVILEGES tkp;
            // 得到当前进程的access token
            bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
            EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] OpenProcessToken: %d!\n", bRet );
            if ( bRet )
            {
                // 得到指定权限的LUID
                bRet = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
                EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] LookupPrivilegeValue: %d!\n", bRet );
                tkp.PrivilegeCount = 1;
                tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                // 在指定的access token上打开或关闭特定权限，需要 TOKEN_ADJUST_PRIVILEGES 权限来访问
                bRet = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
                EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] AdjustTokenPrivileges: %d!\n", bRet );
                bRet = ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
                EqpAgtLog( LOG_DETAIL, "[NmsSetSysState] ExitWindowsEx: %d!\n", bRet );
            }
        }
        else //WIN系列其它系统
        {
            ExitWindowsEx(EWX_REBOOT | EWX_FORCE,0);
        }

#endif

#ifdef _LINUX_ //  linux
// 后续支持        
#endif
    }
}

/*====================================================================
    函数名      : NmsSetIpInfo
    功能        : 网管设置IP信息
    算法实现    :
    引用全局变量:
    输入参数说明: TNetAdaptInfoAll& tNetAdapterInfoAll IP信息
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void NmsSetIpInfo( TNetAdaptInfoAll& tNetAdapterInfoAll )
{
    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/


    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
}

/*====================================================================
    函数名      : GetSysState
    功能        : 获取系统状态 （暂时返回 running）
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : u32 系统状态
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u32 CEqpAgt::GetSysState( void )
{
    return EQP_SYSSTATE_RUNNING;
}

/*====================================================================
    函数名      : GetSoftwareVer
    功能        : 获取软件版本号 （暂时置为 unknown）
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszSoftwareVer 软件版本号
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgt::GetSoftwareVer( LPSTR lpszSoftwareVer )
{
    if ( NULL == lpszSoftwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n");
        return;
    }
    // 暂时置为未知
    sprintf(lpszSoftwareVer, "software unknown");
}

/*====================================================================
    函数名      : GetHardwareVer
    功能        : 获取硬件版本号 （暂时置为 unknown）
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszHardwareVer 硬件版本号
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgt::GetHardwareVer( LPSTR lpszHardwareVer )
{
    if ( NULL == lpszHardwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetHardwareVer] The input param is null!\n");
        return;
    }
    // 暂时置为未知
    sprintf(lpszHardwareVer, "hardware unknown");
}

/*====================================================================
    函数名      : GetCompileTime
    功能        : 获取编译时间
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszCompileTime 编译时间
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgt::GetCompileTime( LPSTR lpszCompileTime )
{
	if ( NULL == lpszCompileTime )
	{
        EqpAgtLog( LOG_ERROR, "[GetCompileTime] The input param is null!\n");
        return;
    }    
    sprintf(lpszCompileTime, "%s %s", __DATE__, __TIME__);
}

/*====================================================================
    函数名      : GetOsType
    功能        : 获取设备操作系统
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : u32 操作系统
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u32 CEqpAgt::GetOsType()
{    
    u32 dwOsType = EQP_OSTYPE_UNKNOWN;
#ifdef WIN32
    dwOsType = EQP_OSTYPE_WIN32;
#elif defined _LINUX_
    dwOsType = EQP_OSTYPE_LINUX;
#endif
    return dwOsType;
}

/*====================================================================
    函数名      : GetSubType
    功能        : 获取设备子类型
    算法实现    :
    引用全局变量:
    输出参数说明: u32* pdwSubType 设备子类型
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::GetSubType( u32* pdwSubType )
{    
    if ( NULL == pdwSubType )
    {
        EqpAgtLog( LOG_ERROR, "[GetSubType] The input param is null!\n");
        return FALSE;
    }

    // 配置信息读/写信号量保护
    ENTER( g_hEqpAgtCfg );

    s8      achProfileName[32] = {0};
    s8      achDefStr[] = "Cannot find the section or key";
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, EQPAGTCFGFILENAME );
    
    // 判断配置文件是否存在、读配置文件
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( achProfileName, "r" );
    if( NULL != hFileR ) // exist
    {
        BOOL32  bResult = FALSE;
        s8      achReturn[EQPAGT_MAX_LENGTH] = {0};       
        
        BOOL32 bSucceedRead = TRUE;
        bResult = GetRegKeyString( achProfileName, SECTION_EqpSubType, KEY_EqpSubType, 
            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
        if( bResult == FALSE )  
        {
            EqpAgtLog( LOG_ERROR, "[GetSubType] Wrong profile while reading %s!\n", KEY_EqpSubType );
            bSucceedRead = FALSE;
        }
        else
        {

            if ( !strcmp( EQPSUBTYPE_KDV2K, achReturn ) )            // 2k
            {
                *pdwSubType = EQP_SUBTYPE_KDV2K;
            }
            else if ( !strcmp( EQPSUBTYPE_KDV2KE, achReturn ) )      // 2ke
            {
                *pdwSubType = EQP_SUBTYPE_KDV2KE;
            }
        }
        fclose( hFileR );
        /*lint -save -esym(438, hFileR)*/
        hFileR = NULL;
        /*lint -restore*/
        return bSucceedRead;
    }
    else
    {
        
#ifdef _LINUX_
        nFopenErr = errno;	
#endif
        
#ifdef WIN32
        nFopenErr = GetLastError(); 
#endif
        //打开配置文件失败的错误号和时间
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[GetSubType] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
        return FALSE;
    }
}

/*====================================================================
    函数名      : GetDiskPartionName
    功能        : 获取盘符名
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszDiskPartionName 盘符名
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::GetDiskPartionName( LPSTR lpszDiskPartionName )
{
    if ( NULL == lpszDiskPartionName )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskPartionName] The input param is null!\n");
        return FALSE;
    }
    
    // 配置信息读/写信号量保护
    ENTER( g_hEqpAgtCfg );

    s8      achProfileName[32] = {0};
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, EQPAGTCFGFILENAME );

    // 判断配置文件是否存在、读配置文件
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( achProfileName, "r" );
    if( NULL != hFileR ) // exist
    {
        BOOL32  bResult = FALSE;
        s8      achReturn[EQPAGT_MAX_LENGTH] = {0};       

        BOOL32 bSucceedRead = TRUE;
        bResult = GetRegKeyString( achProfileName, SECTION_DiskPartionName, KEY_DiskPartionName, 
            DEF_DiskPartionName, achReturn, MAX_VALUE_LEN + 1 );
        if( bResult == FALSE )  
        {
            EqpAgtLog( LOG_ERROR, "[GetDiskPartionName] Wrong profile while reading %s!\n", KEY_DiskPartionName );
            bSucceedRead = FALSE;
        }
        else
        {
            memcpy( lpszDiskPartionName, achReturn, sizeof(achReturn) );
        }
        fclose( hFileR );
        hFileR = NULL;
        return bSucceedRead;
    }
    else
    {
        nFopenErr = GetLastError(); 
        //打开配置文件失败的错误号和时间
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[GetDiskPartionName] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
        return FALSE;
    }
}

/*====================================================================
    函数名      : GetIpInfoAll
    功能        : 获取IP信息
    算法实现    :
    引用全局变量:
    输入参数说明: TNetAdaptInfoAll& tNetAdapterInfoAll IP信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::GetIpInfoAll( TNetAdaptInfoAll* ptNetAdapterInfoAll )
{
    if( NULL == ptNetAdapterInfoAll )
    {
        EqpAgtLog( LOG_ERROR, "[GetIpInfoAll] The param is null!\n");
        return FALSE;
    }

    if ( !GetNetAdapterInfoAll( ptNetAdapterInfoAll ) ) 
    {
        EqpAgtLog( LOG_ERROR, "[GetIpInfoAll] GetNetAdapterInfoAll wrong!\n");
        return FALSE;
    }  
    EqpAgtLog( LOG_DETAIL, "[GetIpInfoAll] GetNetAdapterInfoAll right!\n");
    return TRUE;
}

/*====================================================================
    函数名      : SetIpInfoAll
    功能        : 设置IP信息
    算法实现    :
    引用全局变量:
    输入参数说明: TNetAdaptInfoAll& tNetAdapterInfoAll IP信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::SetIpInfoAll( TNetAdaptInfoAll& tNetAdapterInfoAll )
{
    /************************************************************************/
    /*                暂时屏蔽掉，后续支持                                  */
    /************************************************************************/

    return TRUE;    
}

/*====================================================================
    函数名      : IpCfgInit
    功能        : 检查ipConfig.ini文件是否存在
                  存在，读IP配置信息到内存中，设置IP信息
                  不存在，读系统IP信息，生成IP配置文件
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::IpCfgInit(void)
{
    sprintf( m_achIpCfgFile, "%s/%s", DIR_CONFIG, IPCFGFILENAME );

    // 读IP配置信息
    BOOL32 bRet = ReadIpCfg( m_achIpCfgFile );
    if ( !bRet )
    {
        // 生成默认配置文件
        bRet = PretreatIpCfgFile( DIR_CONFIG, IPCFGFILENAME );
        if ( !bRet )
        {
            EqpAgtLog( LOG_ERROR, "[IpCfgInit] Create ip config file failed!\n");
            return FALSE;
        }
        EqpAgtLog( LOG_KEYSTATUS, "[IpCfgInit] Create ip config file succecced!\n");
        return TRUE;
    }

    // 根据配置文件设置IP
    SetIpInfoAll( m_tNetAdaptInfoAll );
    EqpAgtLog( LOG_KEYSTATUS, "[IpCfgInit] Ip config !\n");
    return TRUE;
}

/*====================================================================
    函数名      : ReadIpCfg
    功能        : 读IP配置信息到内存中
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszCfgFileFullPath  文件全路径
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::ReadIpCfg( LPCSTR lpszCfgFileFullPath )
{
    if( NULL == lpszCfgFileFullPath )
    {
        EqpAgtLog( LOG_ERROR, "[ReadIpCfg] The param is null!\n");
        return FALSE;
    }

    // 判断配置文件是否存在、读配置文件
    FILE* hFileR = NULL;
    s32 nFopenErr = 0;
    hFileR = fopen( lpszCfgFileFullPath, "r" );
    if( NULL != hFileR ) // exist
    {
        // 获取配置文件中的网卡信息
        TNetAdaptInfoAll tNetAdapterInfoAll;
        memset( &tNetAdapterInfoAll, 0, sizeof(tNetAdapterInfoAll));
        BOOL32 bRet = ReadIpInfoTable( &tNetAdapterInfoAll );
        fclose( hFileR );
        /*lint -save -esym(438, hFileR)*/
        hFileR = NULL;
        /*lint -restore*/
        if ( !bRet )
        {
            EqpAgtLog( LOG_ERROR, "[ReadIpCfg] GetIpInfoAll failed!\n");
            return FALSE;
        }
        // 各网卡信息保存到成员变量
        MEMCPY( &m_tNetAdaptInfoAll, &tNetAdapterInfoAll, sizeof(m_tNetAdaptInfoAll) );
        return TRUE;
    }
    else
    {
        nFopenErr = GetLastError(); 
        //打开配置文件失败的错误号和时间
        time_t tiCurTime = ::time(NULL);
        EqpAgtLog( LOG_ERROR, "[ReadIpCfg] error:%s,time:%s !\n", strerror(nFopenErr), ctime(&tiCurTime) );
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
void CEqpAgt::CreateDir( LPCSTR pPathName )
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
    函数名      : PretreatIpCfgFile
    功能        : 生成默认的IP信息配置文件
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszCfgPath  文件路径
                  LPCSTR lpszCfgName  文件名
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::PretreatIpCfgFile( LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{

    if( NULL == lpszCfgPath || NULL == lpszCfgName )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] The param is null!\n");
        return FALSE;
    }

    CreateDir( lpszCfgPath );

    s8 achProfileName[EQPAGT_MAX_LENGTH] = {0};    
    sprintf( achProfileName, "%s/%s", lpszCfgPath, lpszCfgName );

    // 创建文件
    FILE* hFileC = NULL;
    hFileC = fopen( achProfileName, "w" );
    if( NULL == hFileC )
    { 
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] Create config file failed!\n");
        return FALSE;
    }
    
    BOOL32 bRet = TRUE;
    //获取活动的网卡信息
    TNetAdaptInfoAll tNetAdapterInfoAll;
    memset( &tNetAdapterInfoAll, 0, sizeof(tNetAdapterInfoAll));
    bRet = GetIpInfoAll( &tNetAdapterInfoAll );
    if ( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] GetIpInfoAll failed!\n");
        return FALSE;
    }    

    // 各网卡信息读取保存(该信息会保存到成员变量和IP配置文件中)
    MEMCPY( &m_tNetAdaptInfoAll, &tNetAdapterInfoAll, sizeof(m_tNetAdaptInfoAll) );

    // 输出到配置文件中
    bRet = WriteIpInfoTable( tNetAdapterInfoAll );
    if ( !bRet )
    {
        EqpAgtLog( LOG_ERROR, "[PretreatIpCfgFile] WriteIpInfoTable failed!\n");
    }
    
    fclose(hFileC);	
    /*lint -save -esym(438, hFileC)*/
    hFileC = NULL;
    /*lint -restore*/
    return TRUE;
}

/*====================================================================
    函数名      : ReadIpInfoTable
    功能        : 从IP配置文件中读取信息
    算法实现    :
    引用全局变量:
    输入参数说明: TNetAdaptInfoAll tNetAdapterInfoAll  IP信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::ReadIpInfoTable( TNetAdaptInfoAll* ptNetAdapterInfoAll )
{
    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
    return TRUE;
} 
/*====================================================================
    函数名      : WriteIpInfoTable
    功能        : 将IP信息写到配置文件中
    算法实现    :
    引用全局变量:
    输入参数说明: TNetAdaptInfoAll tNetAdapterInfoAll  IP信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgt::WriteIpInfoTable( TNetAdaptInfoAll tNetAdapterInfoAll )
{
    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
    return TRUE;
}

/*====================================================================
    函数名      : SetEqpAgtCfgDefValue
    功能        : 设置EqpAgtCfg配置文件相关默认值
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszFileName  （无需用户填参数）
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void SetEqpAgtCfgDefValue( LPCSTR lpszFileName )
{
    // 生成盘符默认值 D:
    BOOL32 bResult = SetRegKeyString( lpszFileName, SECTION_DiskPartionName, KEY_DiskPartionName, "D:");
    if ( !bResult)
    {
        EqpAgtLog( LOG_ERROR,"[PretreatCfgFile->SetEqpAgtCfgDefValue] Write default DiskPartionName failed" );
    }

    // 生成设备子类型默认值(置空)
    bResult = SetRegKeyString( lpszFileName, SECTION_EqpSubType, KEY_EqpSubType, "");
    if ( !bResult)
    {
        EqpAgtLog( LOG_ERROR,"[PretreatCfgFile->SetEqpAgtCfgDefValue] Write default eqp subtype failed" );
    }

}

/*====================================================================
    函数名      : EqpAgtInit
    功能        : EqpAgt初始化
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgt::EqpAgtInit(void)
{
    BOOL32 bRet = TRUE;

    if( !CreateSemHandle(g_hEqpAgtCfg) ) // 创建信号量 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtInit] Fail to create cfg semHandle\n");
        FreeSemHandle( g_hEqpAgtCfg );
    }

    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    // IP配置信息初始化
    /*
    bRet = IpCfgInit();
    if ( !bRet )
    {
        printf( "[EqpAgtInit] IpCfgInit error!\n" );
        EqpAgtLog( LOG_ERROR,"[EqpAgtInit] IpCfgInit error" );
    }
    */
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/

    // 注册函数 生成配置文件的默认值
    EqpAgtCfgDefValueOper( SetEqpAgtCfgDefValue );
    
    // EqpAgt基础组件初始化
    u16 wRet = EqpAgtCommBaseInit( AID_EQPAGT_SCAN, APPPRI_EQPAGT_SCAN, DIR_CONFIG, EQPAGTCFGFILENAME );
    if ( EQPAGT_SUCCESS != wRet )
    {
        printf( "[EqpAgtInit] init error code %u!\n", wRet );
        EqpAgtLog( LOG_ERROR,"[EqpAgtInit] EqpAgtCommBaseInit error code %u", wRet );
    }

    // 相关接口初始化
    bRet = g_cEqpAgtSysInfo.Initialize( NmsSetSysState, NmsSetIpInfo );
    if ( TRUE == bRet )
    {
        g_cEqpAgtSysInfo.SupportNmsSetSysInfo();
    }
    g_cEqpAgtSysInfo.SupportNmsGetSysInfo();
    g_cEqpAgtSysInfo.SupportNmsTrapSysInfo( SCAN_STATE_TIME_OUT );

    // 系统状态
    g_cEqpAgtSysInfo.SetSysState( GetSysState() );
    // 软件版本
    s8 achSoftwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
	GetSoftwareVer(achSoftwareVer);
    g_cEqpAgtSysInfo.SetSoftwareVer(achSoftwareVer);
    // 硬件版本
    s8 achHardwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
	GetHardwareVer(achHardwareVer);
    g_cEqpAgtSysInfo.SetHardwareVer(achHardwareVer);
    // 编译时间
    s8 achCompileTime[EQPAGT_MAX_LENGTH] = {'\0'};
	GetCompileTime(achCompileTime);
    g_cEqpAgtSysInfo.SetCompileTime(achCompileTime);
    // 操作系统
    g_cEqpAgtSysInfo.SetEqpOsType(GetOsType());
    // 设备子类型
    u32 dwSubType = EQP_SUBTYPE_UNKNOWN;
    GetSubType( &dwSubType );
    g_cEqpAgtSysInfo.SetEqpSubType( dwSubType );
    // 所用盘符名
    s8 achDiskPartionName[EQPAGT_MAX_LENGTH] = {'\0'};
	GetDiskPartionName(achDiskPartionName);
    g_cEqpAgtSysInfo.SetDiskPartionName(achDiskPartionName); 


    /************************************************************************/
    /*                暂时屏蔽掉，后续修改  begin                           */
    /************************************************************************/
    // 本地ip信息
    /*
    TLocalIpInfo tLocalIpInfo;
    memset( &tLocalIpInfo, 0, sizeof(tLocalIpInfo) );
    GetLocalIpInfo( &tLocalIpInfo );
    g_cEqpAgtSysInfo.SetLocalIpInfo( tLocalIpInfo );
    */
    /************************************************************************/
    /*                 暂时屏蔽掉，后续修改  end                            */
    /************************************************************************/
}

/*====================================================================
    函数名      : EqpAgtQuit
    功能        : EqpAgt退出
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgt::EqpAgtQuit(void)
{
    EqpAgtCommBaseQuit();
}

// 版本信息
API void eqpnmcver( void )
{
    OspPrintf( TRUE, FALSE, "eqpnmc:         %s          compile time: %s    %s\n", VER_EQPNMC, __DATE__, __TIME__ );
    OspPrintf( TRUE, FALSE, "eqpagtcommbase: %s  compile time: %s    %s\n", VER_EQPAGTCOMMBASE, __DATE__, __TIME__ );
    OspPrintf( TRUE, FALSE, "eqpagtsysinfo:  %s   compile time: %s    %s\n", VER_EQPAGTSYSINFO, __DATE__, __TIME__ );
}

// 帮助信息
API void eqpnmchelp( void )
{   
    eqpnmcver();    
    OspPrintf(TRUE, FALSE, "  \teqpnmcver       ----> show eqpnmc ver info.\n");

    // 用于显示当前状态
    OspPrintf(TRUE, FALSE, "  \tshowsysstate    ----> show syetem state.\n");
    OspPrintf(TRUE, FALSE, "  \tshowsoftwarever ----> show software vertion.\n");
    OspPrintf(TRUE, FALSE, "  \tshowhardwarever ----> show hardware vertion.\n");
    OspPrintf(TRUE, FALSE, "  \tshowcompiletime ----> show compile time.\n");
    OspPrintf(TRUE, FALSE, "  \tshoweqpostype   ----> show eqp ostype.\n");
    OspPrintf(TRUE, FALSE, "  \tshoweqpsubtype  ----> show eqp subtype.\n");
    OspPrintf(TRUE, FALSE, "  \tshowcpuinfo     ----> show cpu info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowmeminfo     ----> show mem inf.\n");
    OspPrintf(TRUE, FALSE, "  \tshowdiskinfo    ----> show disk info.\n");

    // debug 调试命令，用于发送trap
    OspPrintf(TRUE, FALSE, "  \ttrapcpu         ----> trap cpu info info.\n");
    OspPrintf(TRUE, FALSE, "  \ttrapmem         ----> trap mem info.\n");
    OspPrintf(TRUE, FALSE, "  \ttrapdisk        ----> trap disk info.\n");
    // debug 调试命令，用于打开/关闭相应功能trap
    OspPrintf(TRUE, FALSE, "  \tcputrapoper     ----> open/close and change scan time span of cpu trap.\n");
    OspPrintf(TRUE, FALSE, "  \tmemtrapoper     ----> open/close and change scan time span of mem trap.\n");
    OspPrintf(TRUE, FALSE, "  \tdisktrapoper    ----> open/close and change scan time span of disk trap.\n");
    // debug 调试命令，用于模拟NMS设置
    OspPrintf(TRUE, FALSE, "  \tsetsysstate     ----> set syetem state.\n");
    
    OspPrintf(TRUE, FALSE, "  \tIsAutoRun       ----> Judge the exe is autorun or not.\n");
    OspPrintf(TRUE, FALSE, "  \tAutoRun         ----> Start autorun.\n");
    OspPrintf(TRUE, FALSE, "  \tCancelAutoRun   ----> Cancel autorun.\n");
    OspPrintf(TRUE, FALSE, "  \tquit            ----> quit exe.\n");
    return;
}