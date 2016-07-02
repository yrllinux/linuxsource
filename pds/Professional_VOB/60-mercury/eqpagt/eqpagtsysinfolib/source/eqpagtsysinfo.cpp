/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtsysinfo.cpp
    相关文件    : 
    文件实现功能: 业务子模块: EqpAgt 系统信息 
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#include "eqpagtsysinfo.h"
#include "eqpagtsnmp.h"

CEqpAgtSysInfo	g_cEqpAgtSysInfo;

// 构造
CEqpAgtSysInfo::CEqpAgtSysInfo()
{
/*lint -save -e1566 */
    Free();
/*lint -restore*/
}

// 析构
CEqpAgtSysInfo::~CEqpAgtSysInfo()
{
/*lint -save -e1551 */
    Free();
/*lint -restore*/
}

/*====================================================================
    函数名      : Free
    功能        : 清空
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::Free(void)
{
    m_dwSysState = 0;
    m_dwEqpOsType = 0;
    m_dwEqpSubType = 0;
    m_byCpuStatus = 0;
    m_byMemStatus = 0;
    m_byDiskStatus = 0;
    memset( m_achSoftwareVer, 0, sizeof(m_achSoftwareVer) );
    memset( m_achHardwareVer, 0, sizeof(m_achHardwareVer) );
    memset( m_achCompileTime, 0, sizeof(m_achCompileTime) );
    memset( m_achPartionName, 0, sizeof(m_achPartionName) );
    memset( &m_tNetAdapterInfoAll, 0, sizeof(m_tNetAdapterInfoAll) );
    m_pfNmsSetSysState = NULL;
    m_pfNmsSetIpInfo = NULL;
}

/*====================================================================
    函数名      : Initialize
    功能        : 初始化，存储函数指针
    算法实现    :
    引用全局变量:
    输入参数说明: TNmsSetSysState pfNmsSetSysState  系统状态函数指针
                  TNmsSetIpInfo pfNmsSetIpInfo      ip信息函数指针
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::Initialize( TNmsSetSysState pfNmsSetSysState,
                                   TNmsSetIpInfo pfNmsSetIpInfo )
{
    if ( NULL == pfNmsSetSysState || NULL == pfNmsSetIpInfo )
    {
        EqpAgtLog( LOG_ERROR, "[Initialize] The input param is null!\n" );
        return FALSE;
    }
    
    m_pfNmsSetSysState = pfNmsSetSysState;
    m_pfNmsSetIpInfo = pfNmsSetIpInfo;
    return TRUE;
}



/*====================================================================
    函数名      : SupportNmsGetBaseInfo
    功能        : 支持网管服务器获取系统信息
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SupportNmsGetSysInfo( void )
{ 
    EqpAgtGetOper( NODE_COMMSYSSTATE, EqpAgtGetSysInfo );       // 系统状态
    EqpAgtGetOper( NODE_COMMSOFTWAREVER, EqpAgtGetSysInfo );    // 软件版本号
    EqpAgtGetOper( NODE_COMMHARDWAREVER, EqpAgtGetSysInfo );    // 硬件版本号
    EqpAgtGetOper( NODE_COMMCOMPILETIME, EqpAgtGetSysInfo );    // 编译时间
    EqpAgtGetOper( NODE_COMMOSTYPE, EqpAgtGetSysInfo );         // 设备操作系统   
    EqpAgtGetOper( NODE_COMMSUBTYPE, EqpAgtGetSysInfo );        // 设备子类型
    EqpAgtGetOper( NODE_COMMCPURATE, EqpAgtGetSysInfo );        // cpu 使用率    
    EqpAgtGetOper( NODE_COMMMEMRATE, EqpAgtGetSysInfo );        // 内存 使用率   
    EqpAgtGetOper( NODE_COMMDISKRATE, EqpAgtGetSysInfo );       // 磁盘空间 使用率

    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/ 
    /*
    EqpAgtGetOper( NODE_COMMIP, EqpAgtGetSysInfo );             // ip    
    EqpAgtGetOper( NODE_COMMMASKIP, EqpAgtGetSysInfo );         // ip mask   
    EqpAgtGetOper( NODE_COMMGATEWAYIP, EqpAgtGetSysInfo );      // gateway
    */
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
}

/*====================================================================
    函数名      : SupportNmsSetSysInfo
    功能        : 支持网管服务器设置系统信息
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SupportNmsSetSysInfo( void )
{  
    EqpAgtSetOper( NODE_COMMSYSSTATE, EqpAgtSetSysInfo );       // 系统状态

    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/       
    /*
    EqpAgtSetOper( NODE_COMMIP, EqpAgtSetSysInfo );             // ip    
    EqpAgtSetOper( NODE_COMMMASKIP, EqpAgtSetSysInfo );         // ip mask   
    EqpAgtSetOper( NODE_COMMGATEWAYIP, EqpAgtSetSysInfo );      // gateway
    */
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
}

/*====================================================================
    函数名      : SupportNmsTrapBaseInfo
    功能        : 支持扫描异常或改变的系统信息并上报给网管服务器
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwScanTimeSpan 扫描时间间隔
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SupportNmsTrapSysInfo( u32 dwScanTimeSpan )
{
    EqpAgtTrapOper( NODE_COMMCPURATE, EqpAgtTrapCpuRate, dwScanTimeSpan );      // cpu 使用率   
    EqpAgtTrapOper( NODE_COMMMEMRATE, EqpAgtTrapMemRate, dwScanTimeSpan );      // 内存 使用率
    EqpAgtTrapOper( NODE_COMMDISKRATE, EqpAgtTrapDiskRate, dwScanTimeSpan );    // 磁盘空间 使用率
}

/*====================================================================
    函数名      : SetSysState
    功能        : 存储系统状态
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwSysState 系统状态
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SetSysState( u32 dwSysState )
{
    m_dwSysState = dwSysState;
}

/*====================================================================
    函数名      : GetSysState
    功能        : 获取系统状态（但一般用于 EqpAgt ----> Nms ）
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : u32 系统状态
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u32 CEqpAgtSysInfo::GetSysState( void )
{    
    return m_dwSysState;
}

/*====================================================================
    函数名      : SysStateOfEqpAgt2Nms
    功能        : NMS读（Get）系统状态 EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SysStateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    u32 dwSysState = htonl(m_dwSysState);
    *pwBufLen = sizeof(u32);
    memcpy( pBuf, &dwSysState, *pwBufLen);
}

/*====================================================================
    函数名      : SysStateOfNms2EqpAgt
    功能        : Nms设置系统状态
    算法实现    :
    引用全局变量:
    输入参数说明: void * pBuf  输入数据
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::SysStateOfNms2EqpAgt(void * pBuf)
{
    if ( NULL == m_pfNmsSetSysState )
    {
        EqpAgtLog( LOG_ERROR, "[SysStateOfNms2EqpAgt] m_pfNmsSetSysState is Null!\n" );
        return FALSE;
    }
    u32 dwSysState =  *(u32*)pBuf;
    dwSysState = ntohl(dwSysState);
    (*m_pfNmsSetSysState)( dwSysState );
    return TRUE; 
}

/*====================================================================
    函数名      : SetSoftwareVer
    功能        : 存储软件版本号
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszSoftwareVer 软件版本号
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetSoftwareVer( LPCSTR lpszSoftwareVer )
{
    if ( NULL == lpszSoftwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[SetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }

    memcpy( m_achSoftwareVer, lpszSoftwareVer, sizeof(m_achSoftwareVer) ); 
    m_achSoftwareVer[sizeof(m_achSoftwareVer)-1] = '\0';
    EqpAgtLog( LOG_DETAIL, "[SetSoftwareVer] Set software ver: %s!\n", m_achSoftwareVer );
    return TRUE;
}

/*====================================================================
    函数名      : SetHardwareVer
    功能        : 存储硬件版本号
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszHardwareVer 硬件版本号
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetHardwareVer( LPCSTR lpszHardwareVer )
{
    if ( NULL == lpszHardwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[SetHardwareVer] The input param is null!\n" );
        return FALSE;
    }
    
    memcpy( m_achHardwareVer, lpszHardwareVer, sizeof(m_achHardwareVer) ); 
    m_achHardwareVer[sizeof(m_achHardwareVer)-1] = '\0';
    EqpAgtLog( LOG_DETAIL, "[SetHardwareVer] Set hardware ver: %s!\n", m_achHardwareVer );
    return TRUE;
}

/*====================================================================
    函数名      : SetCompileTime
    功能        : 存储编译时间
    算法实现    :
    引用全局变量:
    输入参数说明: LPCSTR lpszCompileTime 编译时间
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetCompileTime( LPCSTR lpszCompileTime )
{
    if ( NULL == lpszCompileTime )
    {
        EqpAgtLog( LOG_ERROR, "[SetCompileTime] The input param is null!\n" );
        return FALSE;
    }
    
    memcpy( m_achCompileTime, lpszCompileTime, sizeof(m_achCompileTime) ); 
    m_achCompileTime[sizeof(m_achCompileTime)-1] = '\0';
    EqpAgtLog( LOG_DETAIL, "[SetCompileTime] Set compile time: %s!\n", m_achCompileTime );
    return TRUE;
}

/*====================================================================
    函数名      : SetEqpOsType
    功能        : 存储设备操作系统
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwEqpOsType 操作系统
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SetEqpOsType( u32 dwEqpOsType )
{
    m_dwEqpOsType = dwEqpOsType;
    EqpAgtLog( LOG_DETAIL, "[SetEqpOsType] Set eqp OS type: %d!\n", m_dwEqpOsType );
}

/*====================================================================
    函数名      : SetEqpSubType
    功能        : 存储设备子类型
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwEqpSubType 设备子类型
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SetEqpSubType( u32 dwEqpSubType )
{
    m_dwEqpSubType = dwEqpSubType;
    EqpAgtLog( LOG_DETAIL, "[SetEqpSubType] Set eqp sub type: %d!\n", m_dwEqpSubType );
}

/*====================================================================
    函数名      : GetSoftwareVer
    功能        : 获取软件版本号
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszSoftwareVer 软件版本号
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetSoftwareVer( LPSTR lpszSoftwareVer )
{
    if ( NULL == lpszSoftwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( lpszSoftwareVer, m_achSoftwareVer, sizeof(m_achSoftwareVer) ); 
    return TRUE;
}

/*====================================================================
    函数名      : GetHardwareVer
    功能        : 获取硬件版本号
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszHardwareVer 硬件版本号
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetHardwareVer( LPSTR lpszHardwareVer )
{
    if ( NULL == lpszHardwareVer )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( lpszHardwareVer, m_achHardwareVer, sizeof(m_achHardwareVer) ); 
    return TRUE;
}

/*====================================================================
    函数名      : GetCompileTime
    功能        : 获取编译时间
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszCompileTime 编译时间
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetCompileTime( LPSTR lpszCompileTime )
{
    if ( NULL == lpszCompileTime )
    {
        EqpAgtLog( LOG_ERROR, "[GetSoftwareVer] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( lpszCompileTime, m_achCompileTime, sizeof(m_achCompileTime) ); 
    return TRUE;
}

/*====================================================================
    函数名      : GetEqpOsType
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
u32 CEqpAgtSysInfo::GetEqpOsType( void )
{    
    return m_dwEqpOsType;
}

/*====================================================================
    函数名      : GetEqpSubType
    功能        : 获取设备子类型
    算法实现    :
    引用全局变量:
    输入参数说明: void
    返回值说明  : u32 设备子类型
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u32 CEqpAgtSysInfo::GetEqpSubType( void )
{
    return m_dwEqpSubType;
}

/*====================================================================
    函数名      : SoftwareVerOfEqpAgt2Nms
    功能        : 软件版本号  EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::SoftwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf值
    *pwBufLen = (u16)(strlen(m_achSoftwareVer)); 
    memcpy( pBuf, m_achSoftwareVer, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[SoftwareVerOfEqpAgt2Nms] pBuf: %s, len: %u !\n", pBuf, *pwBufLen );
}

/*====================================================================
    函数名      : HardwareVerOfEqpAgt2Nms
    功能        : 硬件版本号  EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::HardwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf值
    *pwBufLen = (u16)(strlen(m_achHardwareVer)); 
    memcpy( pBuf, m_achHardwareVer, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[HardwareVerOfEqpAgt2Nms] pBuf: %s, len: %u !\n", pBuf, *pwBufLen );
}

/*====================================================================
    函数名      : CompileTimeOfEqpAgt2Nms
    功能        : 编译时间    EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::CompileTimeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf值
    *pwBufLen = (u16)(strlen(m_achCompileTime)); 
    memcpy( pBuf, m_achCompileTime, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[CompileTimeOfEqpAgt2Nms] pBuf: %s, len: %u !\n", pBuf, *pwBufLen );
}

/*====================================================================
    函数名      : EqpOsTypeOfEqpAgt2Nms
    功能        : 设备操作系统EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::EqpOsTypeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf值
    *pwBufLen = (u16)(sizeof(u32)); 
    u32 dwEqpOsType = htonl( m_dwEqpOsType );
    memcpy( pBuf, &dwEqpOsType, *pwBufLen );
}

/*====================================================================
    函数名      : EqpSubTypOfEqpAgt2Nms
    功能        : 设备子类型  EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::EqpSubTypOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf值
    *pwBufLen = (u16)(sizeof(u32)); 
    u32 dwEqpSubType = htonl( m_dwEqpSubType );
    memcpy( pBuf, &dwEqpSubType, *pwBufLen );
} 

/*====================================================================
    函数名      : SetDiskPartionName
    功能        : 存储所用磁盘分区名
    算法实现    :
    引用全局变量:
    输入参数说明:LPSTR lpszPartionName 分区名 （最大长度 255）
                                        如：win32 : "c:"
					                    Linux : "/"
    返回值说明  : TRUE   成功
                  FALSE  失败
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetDiskPartionName(LPCSTR lpszDiskPartionName)
{
    if ( NULL == lpszDiskPartionName )
    {
        EqpAgtLog( LOG_ERROR, "[SetDiskPartionName] Null param!\n" );
        return FALSE;
    }
    strncpy( m_achPartionName, lpszDiskPartionName, sizeof(m_achPartionName) );
    m_achPartionName[sizeof(m_achPartionName) - 1] = '\0';
    return TRUE;
}

/*====================================================================
    函数名      : GetCpuInfo
    功能        : 获取cpu信息
    算法实现    :
    引用全局变量:
    输入参数说明:
    输出参数说明: u8& byCpuRate  cpu使用率（百分比）
    返回值说明  : TRUE   成功
                  FALSE  失败
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetCpuInfo( TCpuInfo* ptCpuInfo )
{
    // 目前通过OSP获取cpu使用率，仅有一个值 
    TOspCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    if ( !OspGetCpuInfo( &tCpuInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetCpuRate] Get cpu rate failed!\n" );
        return FALSE;
    }
    ptCpuInfo->m_byCpuAllocRate = 100 - tCpuInfo.m_byIdlePercent;
    return TRUE;
}

/*====================================================================
    函数名      : GetMemInfo
    功能        : 获取内存信息
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: TMemInfo* ptMemInfo 内存信息
    返回值说明  : TRUE   成功
                  FALSE  失败
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetMemInfo( TMemInfo* ptMemInfo )
{
    TOspMemInfo tOspMemInfo;
    memset( &tOspMemInfo, 0, sizeof(tOspMemInfo) );
    if ( !OspGetMemInfo( &tOspMemInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetMemRate] Get mem rate failed!\n" );
        return FALSE;
    }
    ptMemInfo->m_dwMemAllocSize = tOspMemInfo.m_dwAllocSize;
    ptMemInfo->m_dwMemPhysicsSize = tOspMemInfo.m_dwPhysicsSize;

    return TRUE;
}

/*====================================================================
    函数名      : GetDiskInfo
    功能        : 获取磁盘空间信息
    算法实现    :
    引用全局变量:
    输入参数说明: LPSTR lpszPartionName 分区名 （最大长度 255）
                                    如：win32 : "c:"
					                    Linux : "/"
    输出参数说明: TDiskInfo* ptDiskInfo 磁盘空间信息
    返回值说明  : TRUE   成功
                  FALSE  失败
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetDiskInfo( LPSTR lpszPartionName, 
                                    TDiskInfo* ptDiskInfo )
{
    TOspDiskInfo tOspDiskInfo;
    memset( &tOspDiskInfo, 0, sizeof(tOspDiskInfo) );
    if ( !OspGetDiskInfo( lpszPartionName, &tOspDiskInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskInfo] Get disk info failed!\n" );
        return FALSE;
    }
    ptDiskInfo->m_dwDiskAllocSize = tOspDiskInfo.m_dwUsedSize;
    ptDiskInfo->m_dwDiskPhysicsSize = tOspDiskInfo.m_dwPhysicsSize;

    return TRUE;
}

/*====================================================================
    函数名      : GetCpuAllInfo
    功能        : 获取cpu当前的所有信息
    算法实现    :
    引用全局变量:
    输入参数说明:
    输出参数说明: TCpuAllInfo* ptCpuAllInfo  cpu所有信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetCpuAllInfo( TCpuAllInfo* ptCpuAllInfo )
{
    if ( NULL == ptCpuAllInfo )
    {
        EqpAgtLog( LOG_ERROR, "[GetCpuAllInfo] Null param!\n" );
        return FALSE;
    }

    TCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    if ( !GetCpuInfo( &tCpuInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetCpuAllInfo] GetCpuInfo wrong!\n" );
        return FALSE;
    }

    ptCpuAllInfo->m_byCpuStatus = m_byCpuStatus;
    ptCpuAllInfo->tCpuInfo.m_byCpuAllocRate = tCpuInfo.m_byCpuAllocRate; 
    return TRUE;
}

/*====================================================================
    函数名      : GetMemAllInfo
    功能        : 获取内存当前的所有信息
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: TMemAllInfo* ptMemAllInfo  内存所有信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetMemAllInfo( TMemAllInfo* ptMemAllInfo )
{

    if ( NULL == ptMemAllInfo )
    {
        EqpAgtLog( LOG_ERROR, "[GetMemAllInfo] Null param!\n" );
        return FALSE;
    }

    TMemInfo tMemInfo;
    memset( &tMemInfo, 0, sizeof(tMemInfo) );
    if ( !GetMemInfo( &tMemInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetMemAllInfo] GetMemInfo wrong!\n" );
        return FALSE; 
    }
    
    ptMemAllInfo->m_byMemStatus = m_byMemStatus;
    ptMemAllInfo->tMemInfo.m_dwMemAllocSize = tMemInfo.m_dwMemAllocSize;
    ptMemAllInfo->tMemInfo.m_dwMemPhysicsSize = tMemInfo.m_dwMemPhysicsSize;

    return TRUE;
}

/*====================================================================
    函数名      : GetDiskAllInfo
    功能        : 获取磁盘空间当前的所有信息
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: TDiskAllInfo* ptDiskAllInfo  磁盘空间所有信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetDiskAllInfo( TDiskAllInfo* ptDiskAllInfo )
{
    if ( NULL == ptDiskAllInfo )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskAllInfo] Null param!\n" );
        return FALSE;
    }

    TDiskInfo tDiskInfo;
    memset( &tDiskInfo, 0, sizeof(tDiskInfo) );
    if ( !GetDiskInfo( m_achPartionName, &tDiskInfo ) )
    {
        EqpAgtLog( LOG_ERROR, "[GetDiskAllInfo] GetDiskInfo wrong!\n" );
        return FALSE; 
    }

    memcpy( ptDiskAllInfo->m_achPartionName, m_achPartionName, sizeof(ptDiskAllInfo->m_achPartionName) );
    ptDiskAllInfo->m_byDiskStatus = m_byDiskStatus;
    ptDiskAllInfo->tDiskInfo.m_dwDiskAllocSize = tDiskInfo.m_dwDiskAllocSize;
    ptDiskAllInfo->tDiskInfo.m_dwDiskPhysicsSize = tDiskInfo.m_dwDiskPhysicsSize;
    return TRUE;
}

/*====================================================================
    函数名      : CpuRateOfEqpAgt2Nms
    功能        : 获取cpu信息，并将其组织为相应节点buf结构
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void *pBuf       磁盘空间信息（组织为相应节点buf结构）
                  u16  *pwBufLen   数据长度
    返回值说明  : TRUE   成功
                  FALSE  失败
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::CpuRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf 结构：
    // u8(N个 cpu) + u8(第一个 cpu使用率) + …… + u8(第N个 cpu使用率)
    // 实际（目前通过OSP获取cpu使用率:一个 + 百分比 ）
    // 获取CPU占用率信息
    TCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    u8  byCpuNum = 1;
    s8  abyCpuInfo[3] = {'\0'};
    if ( !GetCpuInfo( &tCpuInfo ) )
    {
        return FALSE;
    }

    // pBuf值
    memcpy( abyCpuInfo, &byCpuNum, sizeof(u8) );
    memcpy( abyCpuInfo + sizeof(u8), &tCpuInfo.m_byCpuAllocRate, sizeof(u8) );
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u8)); 
    memcpy( pBuf, abyCpuInfo, *pwBufLen );
    EqpAgtLog( LOG_DETAIL, "[CpuRateOfEqpAgt2Nms] Cpu AllocRate %d !\n",tCpuInfo.m_byCpuAllocRate );
    return TRUE;
}

/*====================================================================
    函数名      : MemRateOfEqpAgt2Nms
    功能        : 获取内存信息，并将其组织为相应节点buf结构
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void *pBuf       磁盘空间信息（组织为相应节点buf结构）
                  u16  *pwBufLen   数据长度
    返回值说明  : TRUE   成功
                  FALSE  失败
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::MemRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf 结构：
    // 第1个字节标识内存单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用内存（网络序）
    // 第6-9个字节代表总共有多少内存（网络序）
    u8  byMemUnitType = 0;
    TMemInfo tMemInfo;
    memset( &tMemInfo, 0, sizeof(tMemInfo) );
    s8  achMemInfo[10] = {'\0'};
    if ( !GetMemInfo( &tMemInfo ) )
    {  
        return FALSE; 
    }

    // pBuf值
    byMemUnitType = 2;  // MB
    u32 dwMemAllocSize = tMemInfo.m_dwMemAllocSize>>10;      // KB ----> MB
    u32 dwMemPhysicsSize = tMemInfo.m_dwMemPhysicsSize>>10;  // KB ----> MB
    EqpAgtLog( LOG_DETAIL, "[MemRateOfEqpAgt2Nms] Memory Info: UsedSize %d (MB), TotalSize %d (MB)\n",
        dwMemAllocSize, dwMemPhysicsSize);
    dwMemAllocSize = htonl(dwMemAllocSize);
    dwMemPhysicsSize = htonl(dwMemPhysicsSize);
    memcpy( achMemInfo, &byMemUnitType, sizeof(u8) );
    memcpy( achMemInfo + sizeof(u8), &dwMemAllocSize, sizeof(u32) );
    memcpy( achMemInfo + sizeof(u8) + sizeof(u32), &dwMemPhysicsSize, sizeof(u32) );
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u32) + sizeof(u32)); 
    memcpy( pBuf, achMemInfo, *pwBufLen );
    return TRUE;
}

/*====================================================================
    函数名      : DiskRateOfEqpAgt2Nms
    功能        : 获取磁盘空间信息，并将其组织为相应节点buf结构
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: void *pBuf       磁盘空间信息（组织为相应节点buf结构）
                  u16  *pwBufLen   数据长度
    返回值说明  : TRUE   成功
                  FALSE  失败
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::DiskRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    // pBuf 结构：
    // 第1个字节标识磁盘空间单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用磁盘空间（网络序）
    // 第6-9个字节代表总共有多少磁盘空间（网络序）
    u8  byDiskUnitType = 0;
    TDiskInfo tDiskInfo;
    memset( &tDiskInfo, 0, sizeof(tDiskInfo) );
    s8  achDiskInfo[10] = {'\0'};
    if ( !GetDiskInfo( m_achPartionName, &tDiskInfo ) )
    {  
        return FALSE; 
    }
    
    // pBuf值
    byDiskUnitType = 3;  // GB
    u32 dwDiskAllocSize = tDiskInfo.m_dwDiskAllocSize>>10;      // MB ----> GB
    u32 dwDiskPhysicsSize = tDiskInfo.m_dwDiskPhysicsSize>>10;  // MB ----> GB
    EqpAgtLog( LOG_DETAIL, "[DiskRateOfEqpAgt2Nms] Disk %s Info: UsedSize %d (GB), TotalSize %d (GB)\n", 
        m_achPartionName, dwDiskAllocSize, dwDiskPhysicsSize);
    dwDiskAllocSize = htonl(dwDiskAllocSize);
    dwDiskPhysicsSize = htonl(dwDiskPhysicsSize);
    memcpy( achDiskInfo, &byDiskUnitType, sizeof(u8) );
    memcpy( achDiskInfo + sizeof(u8), &dwDiskAllocSize, sizeof(u32) );
    memcpy( achDiskInfo + sizeof(u8) + sizeof(u32), &dwDiskPhysicsSize, sizeof(u32) );
    *pwBufLen = (u16)(sizeof(u8) + sizeof(u32) + sizeof(u32)); 
    memcpy( pBuf, achDiskInfo, *pwBufLen );
    return TRUE;
}

/*====================================================================
    函数名      : ScanCpuInfo
    功能        : 扫描cpu
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszCpuInfo2Trap  cpu告警信息（组织为相应节点buf结构）
                  u16& wCpuInfoLen        告警数据长度
    返回值说明  : NO_ALARM   无告警
                  ON_ALARM   告警产生
                  OFF_ALARM  告警恢复
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u8 CEqpAgtSysInfo::ScanCpuInfo( LPSTR lpszCpuInfo2Trap, u16& wCpuInfoLen )
{
    u8 byRet = NO_ALARM;

    // pBuf 结构：
    // u8(N个 cpu) + u8(第一个 cpu使用率) + …… + u8(第N个 cpu使用率)
    // 实际（目前通过OSP获取cpu使用率:一个 + 百分比 ）
    TCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(tCpuInfo) );
    u8  byCpuNum = 1;
    s8  achCpuInfo[3] = {'\0'};
    wCpuInfoLen = 2;
    if ( !GetCpuInfo( &tCpuInfo ) )
    {
        return FALSE;
    }

    // Cpu占用率超过85%，但记录为正常状态
    if ( tCpuInfo.m_byCpuAllocRate > CPU_THRESHOLD && m_byCpuStatus == STATUS_NORMAL)
    {
        m_byCpuStatus = STATUS_ABNORMAL;
        byRet = ON_ALARM;
    }    
    // Cpu占用率未超过85%，但记录为异常状态
    if ( ( tCpuInfo.m_byCpuAllocRate <= CPU_THRESHOLD) && ( m_byCpuStatus == STATUS_ABNORMAL) )
    {
        m_byCpuStatus = STATUS_NORMAL;
        byRet = OFF_ALARM;
    }    
    EqpAgtLog( LOG_DETAIL, "[ScanCpuInfo] Cpu AllocRate %d, Cpu status is %d <0:normal,1:abnormal>, Alarm: %d <0:no alarm,1:on alarm,2:off alarm>!\n",
        tCpuInfo.m_byCpuAllocRate, m_byCpuStatus, byRet );

    memcpy( achCpuInfo, &byCpuNum, sizeof(u8) );
    memcpy( achCpuInfo + sizeof(u8), &tCpuInfo.m_byCpuAllocRate, sizeof(u8) );
    memcpy( lpszCpuInfo2Trap, achCpuInfo, wCpuInfoLen );

    return byRet;
}

/*====================================================================
    函数名      : ScanMemInfo
    功能        : 扫描内存
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszMemInfo2Trap  内存告警信息（组织为相应节点buf结构）
                  u16& wMemInfoLen        告警数据长度
    返回值说明  : NO_ALARM   无告警
                  ON_ALARM   告警产生
                  OFF_ALARM  告警恢复
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u8 CEqpAgtSysInfo::ScanMemInfo( LPSTR lpszMemInfo2Trap, u16& wMemInfoLen )
{  
    u8 byRet = NO_ALARM;

    // pBuf 结构：
    // 第1个字节标识内存单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用内存（网络序）
    // 第6-9个字节代表总共有多少内存（网络序）
    u8  byMemUnitType = 0;
    TMemInfo tMemInfo;
    memset( &tMemInfo, 0, sizeof(tMemInfo) );
    s8  achMemInfo[10] = {'\0'};
    wMemInfoLen = 9;
    if ( !GetMemInfo( &tMemInfo ) )
    {  
        return FALSE; 
    }

    // pBuf值
    byMemUnitType = 2;  // MB
    tMemInfo.m_dwMemAllocSize = tMemInfo.m_dwMemAllocSize>>10;      // KB ----> MB
    tMemInfo.m_dwMemPhysicsSize = tMemInfo.m_dwMemPhysicsSize>>10;  // KB ----> MB
    
    u8 byMemAllocRate = (u8)(tMemInfo.m_dwMemAllocSize * 100 / tMemInfo.m_dwMemPhysicsSize );
    // 内存占有率超过85%，但记录为正常状态
    if ( byMemAllocRate > DISK_THRESHOLD && m_byMemStatus == STATUS_NORMAL)
    {
        m_byMemStatus = STATUS_ABNORMAL;
        byRet = ON_ALARM;
    }    
    // 内存占有率未超过85%，但记录为异常状态
    if ( ( byMemAllocRate <= DISK_THRESHOLD) && ( m_byMemStatus == STATUS_ABNORMAL) )
    {
        m_byMemStatus = STATUS_NORMAL;
        byRet = OFF_ALARM;
    }
    EqpAgtLog( LOG_DETAIL, "[ScanMemInfo] MemAllocRate:%d(MB)/%d(MB), Memory status is %d <0:normal,1:abnormal>, Alarm: %d <0:no alarm,1:on alarm,2:off alarm>!\n",
        tMemInfo.m_dwMemAllocSize, tMemInfo.m_dwMemPhysicsSize, m_byMemStatus, byRet );

    tMemInfo.m_dwMemAllocSize = htonl(tMemInfo.m_dwMemAllocSize);
    tMemInfo.m_dwMemPhysicsSize = htonl(tMemInfo.m_dwMemPhysicsSize);
    memcpy( achMemInfo, &byMemUnitType, sizeof(u8) );
    memcpy( achMemInfo + sizeof(u8), &tMemInfo.m_dwMemAllocSize, sizeof(u32) );
    memcpy( achMemInfo + sizeof(u8) + sizeof(u32), &tMemInfo.m_dwMemPhysicsSize, sizeof(u32) );
    memcpy( lpszMemInfo2Trap, achMemInfo, wMemInfoLen );    
    return byRet;
}

/*====================================================================
    函数名      : ScanDiskInfo
    功能        : 扫描磁盘空间
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: LPSTR lpszDiskInfo2Trap  磁盘空间告警信息（组织为相应节点buf结构）
                  u16& wDiskInfoLen        告警数据长度
    返回值说明  : NO_ALARM   无告警
                  ON_ALARM   告警产生
                  OFF_ALARM  告警恢复
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u8 CEqpAgtSysInfo::ScanDiskInfo( LPSTR lpszDiskInfo2Trap, u16& wDiskInfoLen )
{
    u8 byRet = NO_ALARM;
    
    // pBuf 结构：
    // 第1个字节标识磁盘空间单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用磁盘空间（网络序）
    // 第6-9个字节代表总共有多少磁盘空间（网络序）
    u8  byDiskUnitType = 0;
    TDiskInfo tDiskInfo;
    memset( &tDiskInfo, 0, sizeof(tDiskInfo) );
    s8  achDiskInfo[10] = {'\0'};
    wDiskInfoLen = 9;
    if ( !GetDiskInfo( m_achPartionName, &tDiskInfo ) )
    { 
        return FALSE; 
    }  
    
    byDiskUnitType = 3;  // GB
    tDiskInfo.m_dwDiskAllocSize = tDiskInfo.m_dwDiskAllocSize>>10;      // MB ----> GB
    tDiskInfo.m_dwDiskPhysicsSize = tDiskInfo.m_dwDiskPhysicsSize>>10;  // MB ----> GB

    u8 byDiskAllocRate = (u8)(tDiskInfo.m_dwDiskAllocSize * 100 / tDiskInfo.m_dwDiskPhysicsSize);
    // 磁盘空间占有率超过80%，但记录为正常状态
    if ( byDiskAllocRate > DISK_THRESHOLD && m_byDiskStatus == STATUS_NORMAL)
    {
        m_byDiskStatus = STATUS_ABNORMAL;
        byRet = ON_ALARM;
    }    
    // 磁盘空间占有率未超过80%，但记录为异常状态
    if ( ( byDiskAllocRate <= DISK_THRESHOLD) && ( m_byDiskStatus == STATUS_ABNORMAL) )
    {
        m_byDiskStatus = STATUS_NORMAL;
        byRet = OFF_ALARM;
    }
    EqpAgtLog( LOG_DETAIL, "[ScanDiskInfo] DiskAllocRate:%d(GB)/%d(GB), Disk status is %d <0:normal,1:abnormal>, Alarm: %d <0:no alarm,1:on alarm,2:off alarm>!\n",
        tDiskInfo.m_dwDiskAllocSize, tDiskInfo.m_dwDiskPhysicsSize, m_byDiskStatus, byRet );

    tDiskInfo.m_dwDiskAllocSize = htonl(tDiskInfo.m_dwDiskAllocSize);
    tDiskInfo.m_dwDiskPhysicsSize = htonl(tDiskInfo.m_dwDiskPhysicsSize);
    memcpy( achDiskInfo, &byDiskUnitType, sizeof(u8) );
    memcpy( achDiskInfo + sizeof(u8), &tDiskInfo.m_dwDiskAllocSize, sizeof(u32) );
    memcpy( achDiskInfo + sizeof(u8) + sizeof(u32), &tDiskInfo.m_dwDiskPhysicsSize, sizeof(u32) );
    memcpy( lpszDiskInfo2Trap, achDiskInfo, wDiskInfoLen );

    return byRet;
}

/*====================================================================
    函数名      : SetLocalIpInfo
    功能        : 存储本地IP信息
    算法实现    :
    引用全局变量:
    输入参数说明: TNetAdaptInfoAll* ptNetAdapterInfoAll IP信息
    输出参数说明: 
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::SetIpInfo( TNetAdaptInfoAll& tNetAdapterInfoAll )
{
    if ( NULL == &tNetAdapterInfoAll )
    {
        EqpAgtLog( LOG_ERROR, "[SetLocalIpInfo] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( &m_tNetAdapterInfoAll, &tNetAdapterInfoAll, sizeof(m_tNetAdapterInfoAll) ); 
    return TRUE;
}
/*====================================================================
    函数名      : SetLocalIpInfo
    功能        : 获取本地IP信息
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: TNetAdaptInfoAll* ptNetAdapterInfoAll IP信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::GetIpInfo( TNetAdaptInfoAll* ptNetAdapterInfoAll )
{
    if ( NULL == ptNetAdapterInfoAll )
    {
        EqpAgtLog( LOG_ERROR, "[GetLocalIpInfo] The input param is null!\n" );
        return FALSE;
    }    
    memcpy( ptNetAdapterInfoAll, &m_tNetAdapterInfoAll, sizeof(m_tNetAdapterInfoAll) ); 
    return TRUE;
}

/*====================================================================
    函数名      : LocalIpAddrOfEqpAgt2Nms
    功能        : NMS读（Get）本地IP EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: 
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::LocalIpAddrOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
}

/*====================================================================
    函数名      : LocalIpMaskOfEqpAgt2Nms
    功能        : NMS读（Get）本地IP Mask  EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: 
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::LocalIpMaskOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
}

/*====================================================================
    函数名      : LocalGateWayOfEqpAgt2Nms
    功能        : NMS读（Get）本地GateWay  EqpAgt ----> Nms
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: 
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtSysInfo::LocalGateWayOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen)
{
    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    
    
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
}

/*====================================================================
    函数名      : LocalIpInfoOfNms2EqpAgt
    功能        : NMS写（Set）本地IP信息     Nms ----> EqpAgt
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwNodeName:    构造节点名
                  void * pBuf:       输入数据
    输出参数说明: 
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtSysInfo::IpInfoOfNms2EqpAgt(u32 dwNodeValue, void * pBuf)
{
    if ( NULL == m_pfNmsSetIpInfo )
    {
        EqpAgtLog( LOG_ERROR, "[LocalIpInfoOfNms2EqpAgt] m_pfNmsSetSysState is Null!\n" );
        return FALSE;
    }

    TNetAdaptInfoAll tNetAdapterInfoAll;
    memset( &tNetAdapterInfoAll, 0, sizeof(tNetAdapterInfoAll) );
    memcpy( &tNetAdapterInfoAll, &m_tNetAdapterInfoAll, sizeof(tNetAdapterInfoAll) );

    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    /*   
    switch(dwNodeValue) 
    {        
 
        
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtSetSysInfo] unexpected node(0x%x) received !\n", dwNodeValue);
        return FALSE;
    }
    */
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/

    (*m_pfNmsSetIpInfo)( tNetAdapterInfoAll );
    return TRUE; 
}

/*====================================================================
    函数名      : EqpAgtGetBaseInfo
    功能        : 获取系统信息（回调具体实现）
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwNodeName:    构造节点名
    输出参数说明: void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : SNMP_SUCCESS       成功
                  SNMP_GENERAL_ERROR 错误
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u16 EqpAgtGetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen)
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    u16    wResult = SNMP_SUCCESS;
    BOOL32 bRet = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_COMMSYSSTATE :
        g_cEqpAgtSysInfo.SysStateOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMSOFTWAREVER :
        g_cEqpAgtSysInfo.SoftwareVerOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMHARDWAREVER :
        g_cEqpAgtSysInfo.HardwareVerOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMCOMPILETIME :
        g_cEqpAgtSysInfo.CompileTimeOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMOSTYPE :
        g_cEqpAgtSysInfo.EqpOsTypeOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMSUBTYPE :
        g_cEqpAgtSysInfo.EqpSubTypOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMCPURATE : 
        bRet = g_cEqpAgtSysInfo.CpuRateOfEqpAgt2Nms( pBuf, pwBufLen );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    case NODE_COMMMEMRATE : 
        bRet = g_cEqpAgtSysInfo.MemRateOfEqpAgt2Nms( pBuf, pwBufLen );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    case NODE_COMMDISKRATE : 
        bRet = g_cEqpAgtSysInfo.DiskRateOfEqpAgt2Nms( pBuf, pwBufLen );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/
    /*
    case NODE_COMMIP :
        g_cEqpAgtSysInfo.LocalIpAddrOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMMASKIP :
        g_cEqpAgtSysInfo.LocalIpMaskOfEqpAgt2Nms( pBuf, pwBufLen );
        break;

    case NODE_COMMGATEWAYIP :
        g_cEqpAgtSysInfo.LocalGateWayOfEqpAgt2Nms( pBuf, pwBufLen );
        break;
    */
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/

    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtGetBaseInfo] unexpected node(0x%x) received !\n", dwNodeValue);
        wResult = SNMP_GENERAL_ERROR;
        break;
    }
    return wResult;
}

/*====================================================================
    函数名      : EqpAgtSetSysInfo
    功能        : 设置系统信息（回调具体实现）
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwNodeName:    构造节点名
                  void * pBuf:       输出数据
                  u16 *pwBufLen:     数据长度
    输出参数说明: 
    返回值说明  : SNMP_SUCCESS       成功
                  SNMP_GENERAL_ERROR 错误
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
/*lint -save -esym(715, pwBufLen) */
u16 EqpAgtSetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen)
/*lint -restore*/
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    u16    wResult = SNMP_SUCCESS;
    BOOL32 bRet = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_COMMSYSSTATE :
        bRet = g_cEqpAgtSysInfo.SysStateOfNms2EqpAgt( pBuf );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;

    /************************************************************************/
    /*                暂时屏蔽掉，后续支持  begin                           */
    /************************************************************************/        
    /*
    case NODE_COMMIP :
    case NODE_COMMMASKIP :
    case NODE_COMMGATEWAYIP :
        bRet = g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( dwNodeValue, pBuf );
        if ( !bRet )
        {
            wResult = SNMP_GENERAL_ERROR;
        }
        break;
    */
    /************************************************************************/
    /*                 暂时屏蔽掉，后续支持  end                            */
    /************************************************************************/
        
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtSetSysInfo] unexpected node(0x%x) received !\n", dwNodeValue);
        wResult = SNMP_GENERAL_ERROR;
        break;
    }

    return wResult;
}

/*====================================================================
    函数名      : EqpAgtTrapCpuRate
    功能        : 组织cpu trap值
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: CNodes& cNodes trap值
    返回值说明  : NO_TRAP        无trap产生
                  GENERATE_TRAP  产生trap
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u16 EqpAgtTrapCpuRate( CNodes& cNodes )
{
    u16  wResult = NO_TRAP;
    u8   byRet = NO_ALARM;
    u16  wTrapType = INVALID_TRAP_TYPE;
    u16  wRet = SNMP_GENERAL_ERROR;

    // pBuf 结构：
    // u8(N个 cpu) + u8(第一个 cpu使用率) + …… + u8(第N个 cpu使用率)
    // 实际（目前通过OSP获取cpu使用率:一个 + 百分比 ）
    s8   achCpuInfo2Trap[3] = {'\0'};
    u16  wCpuInfoLen = 0;
    byRet = g_cEqpAgtSysInfo.ScanCpuInfo( achCpuInfo2Trap, wCpuInfoLen );
    switch (byRet) 
    {
    case NO_ALARM:
        return wResult;
    case ON_ALARM:
        wTrapType = TRAP_ALARM_GENERATE;
        break;
    case OFF_ALARM:
        wTrapType = TRAP_ALARM_RESTORE;
        break; 
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] Wrong return value of ScanCpuInfo !\n" );
        return wResult; 
    }
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes clear failed !\n" );
        return wResult;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes set trap type failed !\n" );
        return wResult;
    }

    // pBuf结构：
    // u8（告警节点N个）+ u32（第一个告警节点号 网络序）+ …… + u32（第N个告警节点号 网络序）
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // 仅CPU节点号
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMCPURATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return wResult;
    }
    
    wRet = cNodes.AddNodeValue( NODE_COMMCPURATE, achCpuInfo2Trap, wCpuInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapCpuRate] cNodes add node(0x%x) failed !\n", NODE_COMMCPURATE );
        return wResult;
    }

    wResult = GENERATE_TRAP;
    return wResult;
}

/*====================================================================
    函数名      : EqpAgtTrapMemRate
    功能        : 组织内存trap值
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: CNodes& cNodes trap值
    返回值说明  : NO_TRAP        无trap产生
                  GENERATE_TRAP  产生trap
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u16 EqpAgtTrapMemRate( CNodes& cNodes )
{
    u16  wResult = NO_TRAP;
    u8   byRet = NO_ALARM;
    u16  wTrapType = INVALID_TRAP_TYPE;
    u16  wRet = SNMP_GENERAL_ERROR;
    
    // pBuf 结构：
    // 第1个字节标识内存单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用内存（网络序）
    // 第6-9个字节代表总共有多少内存（网络序）
    s8   achMemInfo2Trap[10] = {'\0'};
    u16  wMemInfoLen = 0;
    byRet = g_cEqpAgtSysInfo.ScanMemInfo( achMemInfo2Trap, wMemInfoLen );
    switch (byRet) 
    {
    case NO_ALARM:
        return wResult;
    case ON_ALARM:
        wTrapType = TRAP_ALARM_GENERATE;
        break;
    case OFF_ALARM:
        wTrapType = TRAP_ALARM_RESTORE;
        break; 
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] Wrong return value of ScanMemInfo !\n" );
        return wResult; 
    }
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes clear failed !\n" );
        return wResult;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes set trap type failed !\n" );
        return wResult;
    }
    
    // pBuf结构：
    // u8（告警节点N个）+ u32（第一个告警节点号 网络序）+ …… + u32（第N个告警节点号 网络序）
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // 仅CPU节点号
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMMEMRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return wResult;
    }
    
    wRet = cNodes.AddNodeValue( NODE_COMMMEMRATE, achMemInfo2Trap, wMemInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapMemRate] cNodes add node(0x%x) failed !\n", NODE_COMMMEMRATE );
        return wResult;
    }
    
    wResult = GENERATE_TRAP;
    return wResult;
}

/*====================================================================
    函数名      : EqpAgtTrapDiskRate
    功能        : 组织磁盘空间trap值
    算法实现    :
    引用全局变量:
    输入参数说明: 
    输出参数说明: CNodes& cNodes trap值
    返回值说明  : NO_TRAP        无trap产生
                  GENERATE_TRAP  产生trap
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u16 EqpAgtTrapDiskRate( CNodes& cNodes )
{
    u16  wResult = NO_TRAP;
    u8   byRet = NO_ALARM;
    u16  wTrapType = INVALID_TRAP_TYPE;
    u16  wRet = SNMP_GENERAL_ERROR;
    
    // pBuf 结构：
    // 第1个字节标识磁盘空间单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用磁盘空间（网络序）
    // 第6-9个字节代表总共有多少磁盘空间（网络序）
    s8   achDiskInfo2Trap[10] = {'\0'};
    u16  wDiskInfoLen = 0;
    byRet = g_cEqpAgtSysInfo.ScanDiskInfo( achDiskInfo2Trap, wDiskInfoLen );
    switch (byRet) 
    {
    case NO_ALARM:
        return wResult;
    case ON_ALARM:
        wTrapType = TRAP_ALARM_GENERATE;
        break;
    case OFF_ALARM:
        wTrapType = TRAP_ALARM_RESTORE;
        break; 
    default:
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] Wrong return value of ScanDiskInfo !\n" );
        return wResult; 
    }
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes clear failed !\n" );
        return wResult;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes set trap type failed !\n" );
        return wResult;
    }
    
    // pBuf结构：
    // u8（告警节点N个）+ u32（第一个告警节点号 网络序）+ …… + u32（第N个告警节点号 网络序）
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // 仅CPU节点号
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMDISKRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return wResult;
    }
    
    wRet = cNodes.AddNodeValue( NODE_COMMDISKRATE, achDiskInfo2Trap, wDiskInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtTrapDiskRate] cNodes add node(0x%x) failed !\n", NODE_COMMDISKRATE );
        return wResult;
    }
    
    wResult = GENERATE_TRAP;
    return wResult;
}

/*====================================================================
    函数名      : showsysstate
    功能        : 用于显示系统状态
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showsysstate(void)
{
    StaticLog( "[showsysstate] The system state is %u", g_cEqpAgtSysInfo.GetSysState() );
}

/*====================================================================
    函数名      : showsoftwarever
    功能        : 用于显示软件版本号
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showsoftwarever(void)
{
    s8  achSoftwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
    g_cEqpAgtSysInfo.GetSoftwareVer( achSoftwareVer );
    StaticLog( "[showsoftwarever] The software version is %s", achSoftwareVer );
}

/*====================================================================
    函数名      : showhardwarever
    功能        : 用于显示硬件版本号
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showhardwarever(void)
{    
    s8  achHardwareVer[EQPAGT_MAX_LENGTH] = {'\0'};
    g_cEqpAgtSysInfo.GetHardwareVer( achHardwareVer );
    StaticLog( "[showhardwarever] The hardware version is %s", achHardwareVer );
}

/*====================================================================
    函数名      : showcompiletime
    功能        : 用于显示编译时间
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showcompiletime(void)
{
    s8  achCompileTime[EQPAGT_MAX_LENGTH] = {'\0'};
    g_cEqpAgtSysInfo.GetCompileTime( achCompileTime );
    StaticLog( "[showcompiletime] The compile time is %s", achCompileTime );
}

/*====================================================================
    函数名      : showeqpostype
    功能        : 用于显示当前设备操作系统
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showeqpostype(void)
{
    StaticLog( "[showeqpostype] The eqp os type is %u", g_cEqpAgtSysInfo.GetEqpOsType() );
}

/*====================================================================
    函数名      : showeqpsubtype
    功能        : 用于显示当前设备子类型
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showeqpsubtype(void)
{
    StaticLog( "[showeqpsubtype] The eqp subtype is %u", g_cEqpAgtSysInfo.GetEqpSubType() );
}

/*====================================================================
    函数名      : showcpuinfo
    功能        : 用于显示当前cpu信息
    算法实现    :
    引用全局变量: g_CEqpAgtSysInfo
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showcpuinfo(void)
{
    TCpuAllInfo tCpuAllInfo;
    memset( &tCpuAllInfo, 0, sizeof(tCpuAllInfo));
    BOOL32 bRet = g_cEqpAgtSysInfo.GetCpuAllInfo( &tCpuAllInfo );
    if ( !bRet )
    {
        StaticLog( "[showcpuinfo] GetCpuAllInfo wrong");
        return;
    }
    StaticLog( "[showcpuinfo] Cpu AllocRate:%u, Status is %u <0:normal,1:abnormal>",
        tCpuAllInfo.tCpuInfo.m_byCpuAllocRate, tCpuAllInfo.m_byCpuStatus );
    return;
}

/*====================================================================
    函数名      : showmeminfo
    功能        : 用于显示当前内存信息
    算法实现    :
    引用全局变量: g_CEqpAgtSysInfo
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showmeminfo(void)
{
    TMemAllInfo tMemAllInfo;
    memset( &tMemAllInfo, 0, sizeof(tMemAllInfo));
    BOOL32 bRet = g_cEqpAgtSysInfo.GetMemAllInfo( &tMemAllInfo );
    if ( !bRet )
    {
        StaticLog( "[showmeminfo] GetMemAllInfo wrong");
        return;
    }
    StaticLog( "[showmeminfo] Memory AllocRate:%u(MB)/%u(MB), Status is %u <0:normal,1:abnormal>",
        tMemAllInfo.tMemInfo.m_dwMemAllocSize>>10, tMemAllInfo.tMemInfo.m_dwMemPhysicsSize>>10, tMemAllInfo.m_byMemStatus );
    return;
}

/*====================================================================
    函数名      : showdiskinfo
    功能        : 用于显示当前磁盘空间信息
    算法实现    :
    引用全局变量: g_CEqpAgtSysInfo
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void showdiskinfo(void)
{
    TDiskAllInfo tDiskAllInfo;
    memset( &tDiskAllInfo, 0, sizeof(tDiskAllInfo));
    BOOL32 bRet = g_cEqpAgtSysInfo.GetDiskAllInfo( &tDiskAllInfo );
    if ( !bRet )
    {
        StaticLog( "[showdiskinfo] GetDiskAllInfo wrong");
        return;
    }
    StaticLog( "[showdiskinfo] Disk: %s AllocRate:%u(GB)/%u(GB), Status is %u <0:normal,1:abnormal>!\n",
        tDiskAllInfo.m_achPartionName, tDiskAllInfo.tDiskInfo.m_dwDiskAllocSize>>10, 
        tDiskAllInfo.tDiskInfo.m_dwDiskPhysicsSize>>10, tDiskAllInfo.m_byDiskStatus );
    return;
}

/*====================================================================
    函数名      : 
    功能        : 
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
/************************************************************************/
/*                暂时屏蔽掉，后续修改  begin                           */
/************************************************************************/
/*
API void showipinfo(void)
{
    TLocalIpInfo tLocalIpInfo;
    memset( &tLocalIpInfo, 0, sizeof(tLocalIpInfo));
    g_cEqpAgtSysInfo.GetLocalIpInfo( &tLocalIpInfo );
    s8  achIpAddr[32] = {'\0'};
    s8  achIpMask[32] = {'\0'};
    s8  achGateway[32] = {'\0'};
    GetIpFromU32( achIpAddr, tLocalIpInfo.GetLocalIPAddr());
    GetIpFromU32( achIpMask, tLocalIpInfo.GetLocalIPMask());
    GetIpFromU32( achGateway, tLocalIpInfo.GetLocalGateWay());
    OspPrintf( TRUE, FALSE, "[showlocalipinfo] IP:%s, Mask:%s, Gateway:%s!\n", achIpAddr, achIpMask, achGateway );
}
*/
/************************************************************************/
/*                 暂时屏蔽掉，后续修改  end                            */
/************************************************************************/


/*====================================================================
    函数名      : trapcpu
    功能        : debug 调试命令，用于发送cpu trap
    算法实现    :
    引用全局变量: g_cEqpAgtSnmp
    输入参数说明: u8 byCpuAllocRate   cpu使用率
                  u8 byCpuStatus      cpu状态（正常/异常）
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void trapcpu( u8 byCpuAllocRate, u8 byCpuStatus )
{
    u16  wTrapType = INVALID_TRAP_TYPE;    
    // Cpu占用率超过85%，但记录为正常状态
    if ( byCpuAllocRate > CPU_THRESHOLD && byCpuStatus == STATUS_NORMAL)
    {
        wTrapType = TRAP_ALARM_GENERATE;
    }    
    // Cpu占用率未超过85%，但记录为异常状态
    if ( ( byCpuAllocRate <= CPU_THRESHOLD) && ( byCpuStatus == STATUS_ABNORMAL) )
    {
        wTrapType = TRAP_ALARM_RESTORE;
    } 

    if ( INVALID_TRAP_TYPE == wTrapType )
    {   
        StaticLog( "[trapcpu] No Trap!" );
        return;
    }

    CNodes cNodes;
    cNodes.Clear();
    u16  wRet = SNMP_GENERAL_ERROR; 

    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapcpu] cNodes clear failed !\n" );
        return;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapcpu] cNodes set trap type failed !\n" );
        return;
    }
    
    // pBuf结构：
    // u8（告警节点N个）+ u32（第一个告警节点号 网络序）+ …… + u32（第N个告警节点号 网络序）
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // 仅CPU节点号
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMCPURATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapcpu] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return;
    }
    
    // pBuf 结构：
    // u8(N个 cpu) + u8(第一个 cpu使用率) + …… + u8(第N个 cpu使用率)
    // 实际（目前通过OSP获取cpu使用率:一个 + 百分比 ）
    u8  byCpuNum = 1;
    s8  achCpuInfo2Trap[3] = {'\0'};
    u16 wCpuInfoLen = 2;
    memcpy( achCpuInfo2Trap, &byCpuNum, sizeof(u8) );
    memcpy( achCpuInfo2Trap + sizeof(u8), &byCpuAllocRate, sizeof(u8) );
    wRet = cNodes.AddNodeValue( NODE_COMMCPURATE, achCpuInfo2Trap, wCpuInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapcpu] cNodes add node(0x%x) failed !\n", NODE_COMMCPURATE );
        return;
    } 

    wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
    if ( SNMP_GENERAL_ERROR == wRet ) // 发送错误
    {                    
        StaticLog( "[trapcpu] Send Trap failed!\n" );
    }
    else // 发送正确
    {
        StaticLog( "[trapcpu] Send Trap Success!\n" );
    }
    return;
}

/*====================================================================
    函数名      : trapmem
    功能        : debug 调试命令，用于发送内存trap
    算法实现    :
    引用全局变量: g_cEqpAgtSnmp
    输入参数说明: u8 byMemAllocSize      使用内存大小(MByte)
                  u8 byMemPhysicsSize    物理内存大小(MByte)
                  u8 byMemStatus         内存状态（正常/异常）
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void trapmem( u32 dwMemAllocSize, u32 dwMemPhysicsSize, u8 byMemStatus )
{
    u16  wTrapType = INVALID_TRAP_TYPE;
    u8 byMemAllocRate = (u8)(dwMemAllocSize * 100 / dwMemPhysicsSize );
    // 内存占有率超过85%，但记录为正常状态
    if ( byMemAllocRate > DISK_THRESHOLD && byMemStatus == STATUS_NORMAL)
    {
        wTrapType = TRAP_ALARM_GENERATE;
    }    
    // 内存占有率未超过85%，但记录为异常状态
    if ( ( byMemAllocRate <= DISK_THRESHOLD) && ( byMemStatus == STATUS_ABNORMAL) )
    {
        wTrapType = TRAP_ALARM_RESTORE;
    }    
    if ( INVALID_TRAP_TYPE == wTrapType )
    {   
        StaticLog( "[trapmem] No Trap!" );
        return;
    }
    
    CNodes cNodes;
    cNodes.Clear();
    u16  wRet = SNMP_GENERAL_ERROR; 
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapmem] cNodes clear failed !\n" );
        return;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapmem] cNodes set trap type failed !\n" );
        return;
    }
    
    // pBuf结构：
    // u8（告警节点N个）+ u32（第一个告警节点号 网络序）+ …… + u32（第N个告警节点号 网络序）
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // 仅CPU节点号
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMMEMRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapmem] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return;
    }    

    // pBuf 结构：
    // 第1个字节标识内存单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用内存（网络序）
    // 第6-9个字节代表总共有多少内存（网络序）
    u8  byMemUnitType = 2;  // MB
    dwMemAllocSize = htonl(dwMemAllocSize);
    dwMemPhysicsSize = htonl(dwMemPhysicsSize);
    s8  achMemInfo2Trap[10] = {'\0'};
    u16 wMemInfoLen = 9;
    memcpy( achMemInfo2Trap, &byMemUnitType, sizeof(u8) );
    memcpy( achMemInfo2Trap + sizeof(u8), &dwMemAllocSize, sizeof(u32) );
    memcpy( achMemInfo2Trap + sizeof(u8) + sizeof(u32), &dwMemPhysicsSize, sizeof(u32) );
    wRet = cNodes.AddNodeValue( NODE_COMMMEMRATE, achMemInfo2Trap, wMemInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapmem] cNodes add node(0x%x) failed !\n", NODE_COMMMEMRATE );
        return;
    }
    
    wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
    if ( SNMP_GENERAL_ERROR == wRet ) // 发送错误
    {                    
        StaticLog( "[trapmem] Send Trap failed!\n" );
    }
    else // 发送正确
    {
        StaticLog( "[trapmem] Send Trap Success!\n" );
    }
    return;
}

/*====================================================================
    函数名      : trapdisk
    功能        : debug 调试命令，用于发送磁盘空间trap
    算法实现    :
    引用全局变量: g_cEqpAgtSnmp
    输入参数说明: u8 dwDiskAllocSize      使用磁盘空间大小(GByte)
                  u8 dwDiskPhysicsSize    物理磁盘空间存大小(GByte)
                  u8 byDiskStatus         磁盘空间状态（正常/异常）
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void trapdisk( u32 dwDiskAllocSize, u32 dwDiskPhysicsSize, u8 byDiskStatus )
{
    u16  wTrapType = INVALID_TRAP_TYPE;
    u8 byDiskAllocRate = (u8)(dwDiskAllocSize * 100 / dwDiskPhysicsSize );
    // 内存占有率超过85%，但记录为正常状态
    if ( byDiskAllocRate > DISK_THRESHOLD && byDiskStatus == STATUS_NORMAL)
    {
        wTrapType = TRAP_ALARM_GENERATE;
    }    
    // 内存占有率未超过85%，但记录为异常状态
    if ( ( byDiskAllocRate <= DISK_THRESHOLD) && ( byDiskStatus == STATUS_ABNORMAL) )
    {
        wTrapType = TRAP_ALARM_RESTORE;
    }    
    if ( INVALID_TRAP_TYPE == wTrapType )
    {   
        StaticLog( "[trapmem] No Trap!" );
        return;
    }
    
    CNodes cNodes;
    cNodes.Clear();
    u16  wRet = SNMP_GENERAL_ERROR; 
    
    wRet = cNodes.Clear();
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapdisk] cNodes clear failed !\n" );
        return;
    }
    
    wRet = cNodes.SetTrapType(wTrapType);
    if ( SNMP_SUCCESS != wRet ) 
    {
        StaticLog( "[trapdisk] cNodes set trap type failed !\n" );
        return;
    }
    
    // pBuf结构：
    // u8（告警节点N个）+ u32（第一个告警节点号 网络序）+ …… + u32（第N个告警节点号 网络序）
    s8  achNodeInfo[sizeof(u32) * MAXNUM_ALARM_NODES + 1 + 1 ]={'\0'};
    u16 wNodeInfoLen = 0;
    u32 dwNodeName = 0;
    u8  byNodeNum = 0;
    byNodeNum = 1;  // 仅CPU节点号
    memcpy( achNodeInfo, &byNodeNum, sizeof(u8) );
    dwNodeName = NODE_COMMDISKRATE;
    dwNodeName = htonl(dwNodeName);
    memcpy( achNodeInfo + sizeof(u8), &dwNodeName, sizeof(u32) );
    wNodeInfoLen = 5;
    wRet = cNodes.AddNodeValue( NODE_COMMALARMNOTIFY, achNodeInfo, wNodeInfoLen);
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapdisk] cNodes add node(0x%x) failed !\n", NODE_COMMALARMNOTIFY );
        return;
    }
    
    // pBuf 结构：
    // 第1个字节标识内存单位（1代表KB 2代表MB  3代表GB  4代表TB）
    // 第2-5个字节代表当前使用磁盘空间（网络序）
    // 第6-9个字节代表总共有多少磁盘空间（网络序）
    u8  byDiskUnitType = 3;  // GB
    dwDiskAllocSize = htonl(dwDiskAllocSize);
    dwDiskPhysicsSize = htonl(dwDiskPhysicsSize);
    s8  achDiskInfo2Trap[10] = {'\0'};
    u16 wDiskInfoLen = 9;
    memcpy( achDiskInfo2Trap, &byDiskUnitType, sizeof(u8) );
    memcpy( achDiskInfo2Trap + sizeof(u8), &dwDiskAllocSize, sizeof(u32) );
    memcpy( achDiskInfo2Trap + sizeof(u8) + sizeof(u32), &dwDiskPhysicsSize, sizeof(u32) );
    wRet = cNodes.AddNodeValue( NODE_COMMDISKRATE, achDiskInfo2Trap, wDiskInfoLen );
    if (SNMP_SUCCESS != wRet) 
    {
        StaticLog( "[trapmem] cNodes add node(0x%x) failed !\n", NODE_COMMDISKRATE );
        return;
    }
    
    wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
    if ( SNMP_GENERAL_ERROR == wRet ) // 发送错误
    {                    
        StaticLog( "[trapdisk] Send Trap failed!\n" );
    }
    else // 发送正确
    {
        StaticLog( "[trapdisk] Send Trap Success!\n" );
    }
    return;
}

/*====================================================================
    函数名      : cputrapoper
    功能        : 打开/关闭 cpu 使用率 trap
    算法实现    :
    引用全局变量: 
    输入参数说明: u32 dwScanTimeSpan  扫描时长
                  BOOL32 bEnable      打开/关闭
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void cputrapoper( u32 dwScanTimeSpan, BOOL32 bEnable )
{
    EqpAgtTrapOper( NODE_COMMCPURATE, EqpAgtTrapCpuRate, dwScanTimeSpan, bEnable);
}

/*====================================================================
    函数名      : memtrapoper
    功能        : 打开/关闭 内存 使用率 trap
    算法实现    :
    引用全局变量: 
    输入参数说明: u32 dwScanTimeSpan  扫描时长
                  BOOL32 bEnable      打开/关闭
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void memtrapoper( u32 dwScanTimeSpan, BOOL32 bEnable )
{ 
    EqpAgtTrapOper( NODE_COMMMEMRATE, EqpAgtTrapMemRate, dwScanTimeSpan, bEnable);
}

/*====================================================================
    函数名      : disktrapoper
    功能        : 打开/关闭 磁盘空间 使用率 trap
    算法实现    :
    引用全局变量: 
    输入参数说明: u32 dwScanTimeSpan  扫描时长
                  BOOL32 bEnable      打开/关闭
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void disktrapoper( u32 dwScanTimeSpan, BOOL32 bEnable )
{ 
    EqpAgtTrapOper( NODE_COMMDISKRATE, EqpAgtTrapDiskRate, dwScanTimeSpan, bEnable);
}

/*====================================================================
    函数名      : setsysstate
    功能        : 设置系统状态 debug 调试命令，用于模拟NMS设置
    算法实现    :
    引用全局变量: 
    输入参数说明: u32 dwSysState 系统状态
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
API void setsysstate( u32 dwSysState )
{
    dwSysState = htonl( dwSysState );
    g_cEqpAgtSysInfo.SysStateOfNms2EqpAgt( &dwSysState );
}

/*====================================================================
    函数名      : 
    功能        : 
    算法实现    :
    引用全局变量: 
    输入参数说明: void
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
/************************************************************************/
/*                暂时屏蔽掉，后续修改  begin                           */
/************************************************************************/
/*
API void setipinfo( u32 dwIPAddr, u32 dwIPMask, u32 dwGateWay )
{ 
    g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( NODE_COMMIP, &dwIPAddr );
    g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( NODE_COMMMASKIP, &dwIPMask );
    g_cEqpAgtSysInfo.LocalIpInfoOfNms2EqpAgt( NODE_COMMGATEWAYIP, &dwGateWay );
}
*/
/************************************************************************/
/*                 暂时屏蔽掉，后续修改  end                            */
/************************************************************************/