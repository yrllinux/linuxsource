/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtsysinfo.h
    相关文件    : eqpagtsysinfo.h 
    文件实现功能: EqpAgt系统信息:
                        系统状态
                        软件版本号、硬件版本号、编译时间、设备操作系统、设备子类型
                        cpu占有率、内存占有率、磁盘空间占有率
                        IP、MASK、GATEWAY
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef _EQPAGT_SYSINFO_H_
#define _EQPAGT_SYSINFO_H_
#include "eqpagtutility.h"
#include "eqpagtipinfo.h"

// 版本号
#define  VER_EQPAGTSYSINFO       ( LPCSTR )"eqpagtsysinfo4.7.1.1.1.120727"

#define  CPU_THRESHOLD    85 // CPU占有率阀值，超过可视为异常
#define  MEM_THRESHOLD    85 // 内存占有率阀值，超过可视为异常
#define  DISK_THRESHOLD   80 // 磁盘空间占有率阀值，超过可视为异常

// CPU信息
typedef struct
{
    u8 m_byCpuAllocRate; // CPU核使用率百分比，0－100
}TCpuInfo;

// CPU所有信息
typedef struct
{
    u8 m_byCpuStatus;
    TCpuInfo tCpuInfo;
}TCpuAllInfo;

// 内存信息
typedef struct
{  
    u32 m_dwMemAllocSize;   // 已使用内存分区大小(KBytes)
    u32 m_dwMemPhysicsSize; // 内存大小(KByte)
}TMemInfo;

// 内存所有信息
typedef struct
{
    u8 m_byMemStatus;
    TMemInfo tMemInfo;
}TMemAllInfo;

// 磁盘分区信息
typedef struct
{
    u32 m_dwDiskAllocSize;   // 已使用磁盘分区大小(MBytes)
    u32 m_dwDiskPhysicsSize; // 磁盘分区大小(MByte)
}TDiskInfo;

// 磁盘分区所有信息
typedef struct
{
    u8 m_byDiskStatus;
    TDiskInfo tDiskInfo;
    s8 m_achPartionName[EQPAGT_MAX_LENGTH];    // 当前所用分区名
}TDiskAllInfo;

/*-------------------------------------------------------------
                     Nms设置系统状态函数
-------------------------------------------------------------*/
typedef void (*TNmsSetSysState)( u32 dwSysState );

/*-------------------------------------------------------------
                    Nms设置本地IP信息函数
-------------------------------------------------------------*/
typedef void (*TNmsSetIpInfo)( TNetAdaptInfoAll& tNetAdapterInfoAll );

/*******************************************************************
    EqpAgt ----> Nms   2字节和4字节的数值类型需要转成网络序
    Nms ----> EqpAgt   2字节和4字节的数值类型需要转成主机序
*******************************************************************/
class CEqpAgtSysInfo
{
public:
    CEqpAgtSysInfo();
    ~CEqpAgtSysInfo();
    
    // 初始化
    BOOL32 Initialize( TNmsSetSysState pfNmsSetSysState, TNmsSetIpInfo pfNmsSetIpInfo );  // 初始化，存储函数指针
    void   SupportNmsGetSysInfo( void );                    // 支持网管服务器获取系统信息
    void   SupportNmsSetSysInfo( void );                    // 支持网管服务器设置系统信息 
    void   SupportNmsTrapSysInfo( u32 dwScanTimeSpan );     // 支持扫描异常或改变的系统信息并上报给网管服务器

    /*------------------------------------------------------------------------------------------
                                          系统状态  begin
    ------------------------------------------------------------------------------------------*/
    // 用户调用接口
    void   SetSysState( u32 dwSysState );                     // 存储系统状态
    u32    GetSysState( void );                               // 获取系统状态

    // NMS读（Get）
    void   SysStateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen); // 系统状态 EqpAgt ----> Nms

    // NMS写（Set）
    BOOL32 SysStateOfNms2EqpAgt(void * pBuf);                 // 系统状态 Nms ----> EqpAgt
    /*------------------------------------------------------------------------------------------
                                          系统状态  end
    ------------------------------------------------------------------------------------------*/



    /*------------------------------------------------------------------------------------------
                  软件版本号、硬件版本号、编译时间、设备操作系统、设备子类型  begin
    ------------------------------------------------------------------------------------------*/
    // 用户调用接口
    BOOL32 SetSoftwareVer( LPCSTR lpszSoftwareVer );                // 存储软件版本号
    BOOL32 SetHardwareVer( LPCSTR lpszHardwareVer );                // 存储硬件版本号
    BOOL32 SetCompileTime( LPCSTR lpszCompileTime );                // 存储编译时间
    void   SetEqpOsType( u32 dwEqpOsType );                         // 存储设备操作系统
    void   SetEqpSubType( u32 dwEqpSubType );                       // 存储设备子类型
    BOOL32 GetSoftwareVer( LPSTR lpszSoftwareVer );                 // 获取软件版本号
    BOOL32 GetHardwareVer( LPSTR lpszHardwareVer );                 // 获取硬件版本号
    BOOL32 GetCompileTime( LPSTR lpszCompileTime );                 // 获取编译时间
    u32    GetEqpOsType( void );                                    // 获取设备操作系统
    u32    GetEqpSubType( void );                                   // 获取设备子类型

    // NMS读（Get）软件版本号、硬件版本号、编译时间、设备操作系统、设备子类型
    void   SoftwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // 软件版本号  EqpAgt ----> Nms
    void   HardwareVerOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // 硬件版本号  EqpAgt ----> Nms
    void   CompileTimeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // 编译时间    EqpAgt ----> Nms
    void   EqpOsTypeOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);      // 设备操作系统EqpAgt ----> Nms
    void   EqpSubTypOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);      // 设备子类型  EqpAgt ----> Nms
    /*------------------------------------------------------------------------------------------
                  软件版本号、硬件版本号、编译时间、设备操作系统、设备子类型  begin
    ------------------------------------------------------------------------------------------*/



    /*------------------------------------------------------------------------------------------
                      cpu占有率、内存占有率、磁盘空间占有率  begin
    ------------------------------------------------------------------------------------------*/
    // 用户调用接口
    BOOL32 SetDiskPartionName(LPCSTR lpszDiskPartionName);      // 存储所用磁盘分区名    
    BOOL32 GetCpuAllInfo( TCpuAllInfo* ptCpuAllInfo );          // 获取当前cpu占有率的所有信息，但一般用于debug 调试
    BOOL32 GetMemAllInfo( TMemAllInfo* ptMemAllInfo );          // 获取当前内存占有率的所有信息，但一般用于debug 调试
    BOOL32 GetDiskAllInfo( TDiskAllInfo* ptDiskAllInfo );       // 获取当前磁盘空间占有率的所有信息，但一般用于debug 调试

    // NMS读（Get）cpu占有率、内存占有率、磁盘空间占有率
    BOOL32 CpuRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // Cpu使用率     EqpAgt ----> Nms
    BOOL32 MemRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);    // 内存使用率    EqpAgt ----> Nms
    BOOL32 DiskRateOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);   // 磁盘空间使用率EqpAgt ----> Nms

    // 扫描cpu占有率、内存占有率、磁盘空间占有率
    u8     ScanCpuInfo( LPSTR lpszCpuInfo2Trap, u16& wCpuInfoLen );    // 扫描Cpu使用率，参量（相关节点Trap信息）EqpAgt ----> Nms
    u8     ScanMemInfo( LPSTR lpszMemInfo2Trap, u16& wMemInfoLen );    // 扫描内存使用率，参量（相关节点Trap信息）EqpAgt ----> Nms
    u8     ScanDiskInfo( LPSTR lpszDiskInfo2Trap, u16& wDiskInfoLen ); // 扫描磁盘空间使用率，参量（相关节点Trap信息）EqpAgt ----> Nms
    /*------------------------------------------------------------------------------------------
                      cpu占有率、内存占有率、磁盘空间占有率  end
    ------------------------------------------------------------------------------------------*/


    /************************************************************************/
    /*                暂时不处理，后续修改  begin                           */
    /************************************************************************/
    /*------------------------------------------------------------------------------------------
                                    IP、MASK、GATEWAY  begin
    ------------------------------------------------------------------------------------------*/
    // 用户调用接口
    BOOL32 SetIpInfo( TNetAdaptInfoAll& tNetAdapterInfoAll );         // 存储IP信息
    BOOL32 GetIpInfo( TNetAdaptInfoAll* ptNetAdapterInfoAll );        // 获取IP信息

    // NMS读（Get）
    void   LocalIpAddrOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen); // 本地IP       EqpAgt ----> Nms
    void   LocalIpMaskOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen); // 本地IP Mask  EqpAgt ----> Nms
    void   LocalGateWayOfEqpAgt2Nms(void * pBuf, u16 * pwBufLen);// 本地GateWay  EqpAgt ----> Nms

    // NMS写（Set）
    BOOL32 IpInfoOfNms2EqpAgt(u32 dwNodeValue, void * pBuf); // 本地IP信息   Nms ----> EqpAgt
    /*------------------------------------------------------------------------------------------
                                    IP、MASK、GATEWAY  end
    ------------------------------------------------------------------------------------------*/
    /************************************************************************/
    /*                暂时不处理，后续修改  end                             */
    /************************************************************************/

protected:    
    // 清空
    void   Free(void);

    // 获取cpu、内存、磁盘空间实际状态
    BOOL32 GetCpuInfo( TCpuInfo* ptCpuInfo );
    BOOL32 GetMemInfo( TMemInfo* ptMemInfo );
    BOOL32 GetDiskInfo( LPSTR lpszPartionName, TDiskInfo* ptDiskInfo );

private:
    // 系统状态
    u32 m_dwSysState;                          // 系统状态
    TNmsSetSysState m_pfNmsSetSysState;        // NMS设置系统状态回调函数指针

    // 软件版本号、硬件版本号、编译时间、设备操作系统、设备子类型
    s8  m_achSoftwareVer[EQPAGT_MAX_LENGTH];   // 软件版本号
    s8  m_achHardwareVer[EQPAGT_MAX_LENGTH];   // 硬件版本号
    s8  m_achCompileTime[EQPAGT_MAX_LENGTH];   // 编译时间
    u32 m_dwEqpOsType;                         // 设备操作系统（NMS依u32 网络序 解析）
    u32 m_dwEqpSubType;                        // 设备子类型（NMS依u32 网络序 解析）

    // cpu占有率、内存占有率、磁盘空间占有率
    s8  m_achPartionName[EQPAGT_MAX_LENGTH];   // 当前所用分区名
    u8  m_byCpuStatus;                         // 当前cpu状态
    u8  m_byMemStatus;                         // 当前内存状态
    u8  m_byDiskStatus;                        // 当前磁盘空间状态

    // IP、MASK、GATEWAY
    TNetAdaptInfoAll m_tNetAdapterInfoAll;     // IP信息
    TNmsSetIpInfo m_pfNmsSetIpInfo;            // Nms设置本地IP信息回调函数指针
};
extern CEqpAgtSysInfo	g_cEqpAgtSysInfo;

// 相关注册函数
u16 EqpAgtGetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen);  // 设置系统信息（回调具体实现）
u16 EqpAgtSetSysInfo(u32 dwNodeName, void * pBuf, u16 * pwBufLen);  // 获取系统信息（回调具体实现）
u16 EqpAgtTrapCpuRate( CNodes& cNodes );                            // 组织cpu trap值
u16 EqpAgtTrapMemRate( CNodes& cNodes );                            // 组织内存trap值
u16 EqpAgtTrapDiskRate( CNodes& cNodes );                           // 组织磁盘空间trap值

// 用于显示当前状态
API void showsysstate(void);
API void showsoftwarever(void);
API void showhardwarever(void);
API void showcompiletime(void);
API void showeqpostype(void);
API void showeqpsubtype(void);
API void showcpuinfo(void);
API void showmeminfo(void);
API void showdiskinfo(void);
// debug 调试命令，用于发送trap
API void trapcpu( u8 byCpuAllocRate, u8 byCpuStatus );
API void trapmem( u32 dwMemAllocSize, u32 dwMemPhysicsSize, u8 byMemStatus );
API void trapdisk( u32 dwDiskAllocSize, u32 dwDiskPhysicsSize, u8 byDiskStatus );
// debug 调试命令，用于打开/关闭相应功能trap
API void cputrapoper( u32 dwScanTimeSpan, BOOL32 bEnable );
API void memtrapoper( u32 dwScanTimeSpan, BOOL32 bEnable );
API void disktrapoper( u32 dwScanTimeSpan, BOOL32 bEnable );
// debug 调试命令，用于模拟NMS设置
API void setsysstate( u32 dwSysState );

#endif  // _EQPAGT_SYSINFO_H_