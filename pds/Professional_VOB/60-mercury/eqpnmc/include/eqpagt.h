/*****************************************************************************
    模块名      : EqpNmc
    文件名      : eqpagt.h
    相关文件    : 
    文件实现功能: EqpAgt接口及实现 (for win32)
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef EQPAGT_H_
#define EQPAGT_H_

#include "eqpagtcommon.h"
#include "eqpagtsysinfo.h"
#include "eqpagtcommbase.h"
#include "kdvdef.h"
#include "eqpagtipinfo.h"

// 版本号
#define VER_EQPNMC              ( LPCSTR )"eqpnmc4.7.1.1.1.120727"

//  ---------------   eqpagtcfg配置文件相关 begin  -------------------
// 设备子类型
#define SECTION_EqpSubType      ( LPCSTR )"EqpSubType"
#define KEY_EqpSubType          ( LPCSTR )"EqpSubType"

// 盘符名
#define SECTION_DiskPartionName ( LPCSTR )"DiskPartionName"
#define KEY_DiskPartionName     ( LPCSTR )"DiskPartionName"
#if defined( WIN32 ) || defined(_WIN32)
#define DEF_DiskPartionName		( LPCSTR )"c:"
#else
#define DEF_DiskPartionName	    ( LPCSTR )"/"
#endif
//  ---------------   eqpagtcfg配置文件相关 end    -------------------

// 线程ID及优先级
#define AID_EQPAGT_SCAN         AID_AGT_BGN
#define APPPRI_EQPAGT_SCAN      90

#define EXEFILE                 ( LPCSTR )"c:\\WINDOWS\\system32\\netsh.exe"

// 定时扫描设备状态定时器时长（单位: s）
#define SCAN_STATE_TIME_OUT		5

class CEqpAgt
{
public:
    CEqpAgt();
    ~CEqpAgt();
    
    // 初始化
    void   EqpAgtInit(void);
    // 退出
    void   EqpAgtQuit(void);

    // 获取系统状态
    u32    GetSysState( void );
    // 获取软件版本号
    void   GetSoftwareVer( LPSTR lpszSoftwareVer );
    // 获取硬件版本号
    void   GetHardwareVer( LPSTR lpszHardwareVer );
    // 获取编译时间
    void   GetCompileTime( LPSTR lpszCompileTime );
    // 获取设备操作系统
    u32    GetOsType( void );
    // 获取设备子类型
    BOOL32 GetSubType( u32* pdwSubType );
    // 获取盘符名
    BOOL32 GetDiskPartionName( LPSTR lpszDiskPartionName );

    // 获取IP信息
    BOOL32 GetIpInfoAll( TNetAdaptInfoAll* ptNetAdapterInfoAll );
    // 设置IP信息
    BOOL32 SetIpInfoAll( TNetAdaptInfoAll& tNetAdapterInfoAll );

private:
        
    // 检查ipConfig.ini文件是否存在，如果不存在则新建一个
    BOOL32 IpCfgInit(void);                                                 // 配置初始化
    BOOL32 ReadIpCfg( LPCSTR lpszCfgFileFullPath );                         // 读配置
    void   CreateDir( LPCSTR pPathName );			                        // 创建目录
    BOOL32 PretreatIpCfgFile( LPCSTR lpszCfgPath, LPCSTR lpszCfgName );     // 生成默认配置文件
   
    // ip信息表维护
    BOOL32 ReadIpInfoTable( TNetAdaptInfoAll* ptNetAdapterInfoAll );
    BOOL32 WriteIpInfoTable( TNetAdaptInfoAll tNetAdapterInfoAll );

private:
    s8     m_achIpCfgFile[EQPAGT_MAX_LENGTH];    // ipConfig.ini的路径
    TNetAdaptInfoAll m_tNetAdaptInfoAll;         // IP信息
};
extern CEqpAgt g_cEqpAgt;

#endif  // EQPAGT_H_