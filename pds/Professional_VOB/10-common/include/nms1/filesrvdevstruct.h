/*=======================================================================
模块名      : 文件升级服务器
文件名      : filesrvdevstruct.h
相关文件    : 无
文件实现功能: 服务器——设备结构定义
作者        : 王昊
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-------------------------------------------------------------------------
修改记录:
日      期  版本    修改人  修改内容
2006/12/28  1.0     王昊    创建
=======================================================================*/

#ifndef _FILESRVDEVSTRUCT_H
#define _FILESRVDEVSTRUCT_H

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif//WIN32

#include <ctype.h>
#include <kdvtype.h>
#include <stdio.h>
#include "filesrvconst.h"
#include "filesrvstruct.h"

#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif//WIN32

/*------------------------------------------------------------------------------
  服务器与设备交互结构
------------------------------------------------------------------------------*/
//软件版本结构(终端调用IDevSoftwareVer)
typedef struct tagDevSoftwareVer1//len: 16
{
public:
    tagDevSoftwareVer1() : m_wMainVer(0), m_wSubVer(0), m_wInterfaceVer(0),
                           m_wModifyVer(0), m_dwDate(0), m_dwReserved1(0)
    {}

    tagDevSoftwareVer1(u16 wMainVer, u16 wSubVer, u16 wIntVer, u16 wModVer,
                       u32 dwDate) : m_dwReserved1(0)
    {
        m_wMainVer      = htons( wMainVer );
        m_wSubVer       = htons( wSubVer );
        m_wInterfaceVer = htons( wIntVer );
        m_wModifyVer    = htons( wModVer );
        m_dwDate        = htonl( dwDate );
    }

    tagDevSoftwareVer1(LPSTR lptrVer) : m_dwReserved1(0)
    { FromString( lptrVer ); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagDevSoftwareVer1) ); }

    //设置主版本号
    inline void SetMainVer(u16 wMainVer) { m_wMainVer = htons( wMainVer ); }

    //获取主版本号
    inline u16 GetMainVer(void) const { return ntohs( m_wMainVer ); }

    //设置副版本号
    inline void SetSubVer(u16 wSubVer) { m_wSubVer = htons( wSubVer ); }

    //获取副版本号
    inline u16 GetSubVer(void) const { return ntohs( m_wSubVer ); }

    //设置接口版本号
    inline void SetInterfaceVer(u16 wInterfaceVer)
    { m_wInterfaceVer = htons( wInterfaceVer ); }

    //获取接口版本号
    inline u16 GetInterfaceVer(void) const { return ntohs( m_wInterfaceVer ); }

    //设置修改版本号
    inline void SetModifyVer(u16 wModifyVer) { m_wModifyVer = htons( wModifyVer ); }

    //获取修改版本号
    inline u16 GetModifyVer(void) const { return ntohs( m_wModifyVer ); }

    //设置日期
    inline void SetDate(u32 dwDate) { m_dwDate = htonl( dwDate ); }

    //获取日期
    inline u32 GetDate(void) const { return ntohl( m_dwDate ); }

    //比较版本
    inline BOOL32 operator<(const tagDevSoftwareVer1 &tVer)
    {
        return ( memcmp( this, &tVer, sizeof (tagDevSoftwareVer1) ) < 0 );
    }
    inline BOOL32 operator>(const tagDevSoftwareVer1 &tVer)
    {
        return ( memcmp( this, &tVer, sizeof (tagDevSoftwareVer1) ) > 0 );
    }
    inline BOOL32 operator==(const tagDevSoftwareVer1 &tVer)
    {
        return ( memcmp( this, &tVer, sizeof (tagDevSoftwareVer1) ) == 0 );
    }

    //转成字符串
    inline void ToString(s8 szVer[MAX_FILESRV_SOFTVER_LEN]) const
    {
        sprintf( szVer, "%.2d.%.2d.%.2d.%.2d.%.6d", ntohs( m_wMainVer ),
                 ntohs( m_wSubVer ), ntohs( m_wInterfaceVer ),
                 ntohs( m_wModifyVer ), ntohl( m_dwDate ) );
        return;
    }

    //从字符串读取
    inline BOOL32 FromString(LPSTR lptrVer)
    {
        if ( lptrVer == NULL || strlen(lptrVer) != 18 )
        {
            return FALSE;
        }
        m_wMainVer      = htons( atoi( lptrVer ) );
        m_wSubVer       = htons( atoi( lptrVer + 3 ) );
        m_wInterfaceVer = htons( atoi( lptrVer + 6 ) );
        m_wModifyVer    = htons( atoi( lptrVer + 9 ) );
        m_dwDate        = htonl( atoi( lptrVer + 12) );
        return TRUE;
    }

private:
    u16         m_wMainVer;//主版本号，40.30.01.02.061111，主版本号为40
    u16         m_wSubVer;//副版本号，40.30.01.02.061111，副版本号为30
    u16         m_wInterfaceVer;//接口版本号，40.30.01.02.061111，接口版本号为01
    u16         m_wModifyVer;//修改版本号，40.30.01.02.061111，修改版本号为02
    u32         m_dwDate;//日期版本号，40.30.01.02.061111，日期版本号为061111
    u32         m_dwReserved1;//保留字段
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IDevSoftwareVer1
;
typedef IDevSoftwareVer1        IDevSoftwareVer;


//硬件版本结构(终端调用IDevHardwareVer)
typedef struct tagDevHardwareVer1//len: 32
{
public:
    tagDevHardwareVer1() { SetNull(); }

    //置空
    inline void SetNull(void)
    { memset( this, 0, sizeof (tagDevHardwareVer1) ); }

    //设置硬件版本号
    inline void SetHardwareVer(u32 dwHardwareVer)
    {
        SetNull();
        *reinterpret_cast<u32*>(m_szHardwareVer) = htonl( dwHardwareVer );
    }

    //增加硬件版本号
//    BOOL32 AddHardwareVer(u8 byHardwareVer)
//    {
//        if ( m_szHardwareVer[sizeof (m_szHardwareVer) - 1] == 0 )
//        {
//            for ( s32 nIndex = 0; nIndex < sizeof (m_szHardwareVer); nIndex ++ )
//            {
//                if ( m_szHardwareVer[nIndex] == 0 )
//                {
//                    m_szHardwareVer[nIndex] = byHardwareVer + 1;
//                    break;
//                }
//                break;
//            }
//            return TRUE;
//        }
//        else
//        {
//            return FALSE;
//        }
//    }

    //设置硬件版本号
    inline void SetHardwareVer(LPSTR lptrHardwareVer)
    {
        SetNull();
        if ( lptrHardwareVer != NULL )
        {
            strncpy( m_szHardwareVer, lptrHardwareVer,
                     sizeof (m_szHardwareVer) - 1 );
        }
    }

    //获取硬件版本号
    inline LPCSTR GetHardwareVer(void) const { return m_szHardwareVer; }

private:
    s8          m_szHardwareVer[MAX_FILESRV_HARDVER_LEN];
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IDevHardwareVer1
;
typedef IDevHardwareVer1        IDevHardwareVer;


//文件信息
typedef struct tagDevFileInfo1//len: 72
{
public:
    tagDevFileInfo1() : m_dwFileType(0), m_dwFileSize(0)
    { SetNull(); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagDevFileInfo1) ); }

    //设置文件类型
    inline void SetFileType(EMFileSrvFileType emFileType)
    { m_dwFileType = htonl( static_cast<u32>(emFileType) ); }

    //获取文件类型
    inline EMFileSrvFileType GetFileType(void) const
    { return static_cast<EMFileSrvFileType>( ntohl( m_dwFileType ) ); }

    //设置文件大小
    inline void SetFileSize(u32 dwFileSize) { m_dwFileSize = htonl(dwFileSize); }

    //获取文件大小
    inline u32 GetFileSize(void) const { return ntohl(m_dwFileSize); }

    //设置文件名
    inline void SetFileName(LPCSTR lptrFileName)
    {
        memset( m_szFileName, 0, sizeof (m_szFileName) );
        if ( lptrFileName != NULL )
        {
            strncpy( m_szFileName, lptrFileName, sizeof (m_szFileName) - 1 );
        }
    }

    //获取文件名
    inline LPCSTR GetFileName(void) const { return m_szFileName; }

private:
    u32     m_dwFileType;       //文件类型(EMFileSrvFileType)
    u32     m_dwFileSize;       //文件大小(按字节计算)
    s8      m_szFileName[MAX_FILESRV_FILENAME_LEN];//文件名
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IDevFileInfo1
;
typedef IDevFileInfo1           IDevFileInfo;

//推荐版本信息(终端调用IDevVerInfo)
typedef struct tagDevVerInfo1//len: 820
{
    tagDevVerInfo1() : m_byFileNum(0) { SetNull(); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagDevVerInfo1) ); }

    //设置描述
    inline void SetBugReport(LPCSTR lptrBugReport)
    {
        memset( m_szBugReport, 0, sizeof (m_szBugReport) );
        if ( lptrBugReport != NULL )
        {
            strncpy( m_szBugReport, lptrBugReport, sizeof (m_szBugReport) - 1 );
        }
    }

    //获取描述
    inline LPCSTR GetBugReport(void) const { return m_szBugReport; }

    //设置推荐软件版本号 
    inline void SetSoftwareVer(const IDevSoftwareVer &tSoftVer)
    { m_tSoftVer = tSoftVer; }

    //获取推荐软件版本号
    inline IDevSoftwareVer GetSoftwareVer(void) const
    { return m_tSoftVer; }

    //添加文件
    BOOL32 AddOneFile(EMFileSrvFileType emFileType, u32 dwFileSize,
                      LPCSTR lptrFileName)
    {
        if ( m_byFileNum >= MAX_FILESRV_DEVFILE_NUM || lptrFileName == NULL
             || dwFileSize == 0 )//参数错误
        {
            return FALSE;
        }
        for ( u8 byIndex = 0; byIndex < m_byFileNum; byIndex ++ )//文件类型已存在
        {
            if ( m_atFileInfo[byIndex].GetFileType() == emFileType )
            {
                return FALSE;
            }
        }

        m_atFileInfo[m_byFileNum].SetFileType( emFileType );
        m_atFileInfo[m_byFileNum].SetFileSize( dwFileSize );
        m_atFileInfo[m_byFileNum].SetFileName( lptrFileName );
        m_byFileNum ++;
        return TRUE;
    }

    //修改文件
    BOOL32 EditOneFile(EMFileSrvFileType emFileType, u32 dwFileSize,
                       LPCSTR lptrFileName)
    {
        if ( m_byFileNum == 0 || lptrFileName == NULL || dwFileSize == 0 )//参数错误
        {
            return FALSE;
        }
        for ( u8 byIndex = 0; byIndex < m_byFileNum; byIndex ++ )
        {
            if ( m_atFileInfo[byIndex].GetFileType() == emFileType )//文件类型已存在
            {
                m_atFileInfo[byIndex].SetFileSize( dwFileSize );
                m_atFileInfo[byIndex].SetFileName( lptrFileName );
                return TRUE;
            }
        }
        return FALSE;
    }

    //删除文件
    BOOL32 DelOneFile(EMFileSrvFileType emFileType)
    {
        for ( u8 byIndex = 0; byIndex < m_byFileNum; byIndex ++ )
        {
            if ( m_atFileInfo[byIndex].GetFileType() == emFileType )//文件类型已存在
            {
                if ( byIndex < MAX_FILESRV_DEVFILE_NUM - 1 )
                {
                    memmove( m_atFileInfo + byIndex, m_atFileInfo + byIndex + 1,
                             (MAX_FILESRV_DEVFILE_NUM - byIndex - 1)
                                    * sizeof (IDevFileInfo) );
                }
                m_atFileInfo[MAX_FILESRV_DEVFILE_NUM - 1].SetNull();
                m_byFileNum --;
                return TRUE;
            }
        }
        return FALSE;
    }
    BOOL32 DelOneFile(s32 nIndex)
    {
        if ( nIndex < static_cast<s32>(m_byFileNum) )
        {
            if ( nIndex < MAX_FILESRV_DEVFILE_NUM - 1 )
            {
                memmove( m_atFileInfo + nIndex, m_atFileInfo + nIndex + 1,
                         (MAX_FILESRV_DEVFILE_NUM - nIndex - 1)
                                    * sizeof (IDevFileInfo) );
            }
            m_atFileInfo[MAX_FILESRV_DEVFILE_NUM - 1].SetNull();
            m_byFileNum --;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    //清空文件列表
    inline void ClearFileList(void)
    {
        memset( m_atFileInfo, 0, sizeof (m_atFileInfo) );
        m_byFileNum = 0;
    }

    //获取文件总数
    inline s32 GetFileNum(void) const
    { return static_cast<s32>(m_byFileNum); }

    //获取一个文件
    inline IDevFileInfo GetOneFile(s32 nIndex) const
    {
        if ( nIndex < static_cast<s32>(m_byFileNum) )
        {
            return m_atFileInfo[nIndex];
        }
        else
        {
            IDevFileInfo tInfo;
            return tInfo;
        }
    }

private:
    u8              m_byFileNum;                                //文件个数
    u8              m_abyReserved1[3];
    s8              m_szBugReport[MAX_FILESRV_BUG_REPORT_LEN];  //修复描述
    IDevSoftwareVer m_tSoftVer;                                 //推荐软件版本号
    IDevFileInfo    m_atFileInfo[MAX_FILESRV_DEVFILE_NUM];      //文件信息
}
//#ifndef WIN32
//__attribute__ ( (packed) )
//#endif//WIN32
IDevVerInfo1
;
typedef IDevVerInfo1            IDevVerInfo;


//设备(MCU/MT)注册信息(终端调用IDevLoginInfo)
typedef struct tagDevLoginInfo1//len: 212
{
public:
    tagDevLoginInfo1() : m_byDevType(enumFileSrvDevTypeMT),
    	                 m_byDevModel(enumFileSrvDevModelMTV5),
    	                 m_byOSType(enumFileSrvOsTypeLinuxPowerPC), m_byReserved1(0)
    { SetNull(); }

    //置空
    inline void SetNull(void)
    {
        memset( this, 0, sizeof (tagDevLoginInfo1) );
        m_byDevType = enumFileSrvDevTypeMT;
    }

    //获取设备类型
    inline EMFileSrvDevType GetDevType(void) const
    { return static_cast<EMFileSrvDevType>(m_byDevType); }

    //设置设备型号
    inline void SetDevModel(EMFileSrvDevMTModel emModel)
    { m_byDevModel = static_cast<u8>(emModel); }

    //获取设备型号
    inline EMFileSrvDevMTModel GetDevModel(void) const
    { return static_cast<EMFileSrvDevMTModel>(m_byDevModel); }

    //设置操作系统类型
    inline void SetOsType(EMFileSrvOSType emOsType)
    { m_byOSType = static_cast<u8>(emOsType); }

    //获取操作系统类型
    inline EMFileSrvOSType GetOsType(void) const
    { return static_cast<EMFileSrvOSType>(m_byOSType); }

    //设置序列号(现传入MAC地址)
    inline void SetSerialNo(LPCSTR lptrSN)
    {
        memset( m_szSerialNo, 0, sizeof (m_szSerialNo) );
        if ( lptrSN != NULL )
        {
            strncpy( m_szSerialNo, lptrSN, sizeof (m_szSerialNo) - 1 );
        }
    }

    //获取序列号
    inline LPCSTR GetSerialNo(void) const { return m_szSerialNo; }

    //设置软件版本号
    inline void SetSoftwareVer(const IDevSoftwareVer &tSoftVer)
    { m_tSoftwareVer = tSoftVer; }

    //获取软件版本号
    inline IDevSoftwareVer GetSoftwareVer(void) const { return m_tSoftwareVer; }

    //设置硬件版本号
    inline void SetHardwareVer(const IDevHardwareVer &tHardVer)
    { m_tHardwareVer = tHardVer; }

    //获取硬件版本号
    inline IDevHardwareVer GetHardwareVer(void) const { return m_tHardwareVer; }

private:
    u8          m_byDevType;    //设备类型(EMFileSrvDevType)，终端无须设置
    u8          m_byDevModel;   //设备型号(EMFileSrvDevMTMode)
    u8          m_byOSType;     //设备操作系统类型(EMFileSrvOSType)
    u8          m_byReserved1;
    s8          m_szSerialNo[MAX_FILESRV_SERIALNO_LEN]; //序列号(现传入MAC地址)
    IDevSoftwareVer     m_tSoftwareVer;                 //软件版本号
    IDevHardwareVer     m_tHardwareVer;                 //硬件版本号
    u8          m_abyReserved2[32];
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IDevLoginInfo1
;
typedef IDevLoginInfo1          IDevLoginInfo;


/*------------------------------------------------------------------------------
  服务器消息结构
------------------------------------------------------------------------------*/
//消息发送结构(千万不要清空，否则找不到版本号)
typedef struct tagDevFileSrvMsg//len: 28 * 1024
{
public:
    tagDevFileSrvMsg(const u8 *pbyMsgBody = NULL, u16 wMsgBodyLen = 0)
                    : m_dwServiceNo(0), m_dwSrcVer(FILESRV_CUR_VERSION),
                      m_dwSerialNo(0), m_dwErrorCode(0), m_wTotalPkt(0),
                      m_wCurPkt(0), m_wMsgLen(0)
    { SetMsgBody( pbyMsgBody, wMsgBodyLen ); }

    //设置服务号
    inline void SetServiceNo(u32 dwServiceNo)
    { m_dwServiceNo = htonl(dwServiceNo); }

    //获取服务号
    inline u32 GetServiceNo(void) const { return ntohl( m_dwServiceNo ); }

    //设置版本号(服务器专用)
    inline void SetSrcVer(EMFileSrvUsedVersion enumSUSVer) { m_dwSrcVer = htonl(enumSUSVer); }

    //获取版本号
    inline u32 GetSrcVer(void) const { return ntohl( m_dwSrcVer ); }

    //设置消息序列号
    inline void SetSN(u32 dwSN) { m_dwSerialNo = htonl( dwSN ); }

    //获取消息序列号
    inline u32 GetSN(void) const { return ntohl( m_dwSerialNo ); }

    //设置错误码
    inline void SetErrorCode(u32 dwErrorCode)
    { m_dwErrorCode = htonl( dwErrorCode ); }

    //获取错误码
    inline u32 GetErrorCode(void) const { return ntohl( m_dwErrorCode ); }

    //设置总包数
    inline void SetTotalPkt(u16 wPkt) { m_wTotalPkt = htons( wPkt ); }

    //获取总包数
    inline u32 GetTotalPkt(void) const { return ntohs( m_wTotalPkt ); }

    //设置当前包数
    inline void SetCurPkt(u16 wPkt) { m_wCurPkt = htons( wPkt ); }

    //获取当前包数
    inline u16 GetCurPkt(void) const { return ntohs( m_wCurPkt ); }

    //设置消息体
    inline void SetMsgBody(const void *pMsgBody = NULL, u16 wMsgBodyLen = 0)
    {
//         SetSrcVer();
        m_wMsgLen = 0;
        memset( m_abyMsgBody, 0, sizeof (m_abyMsgBody) );
        if ( pMsgBody != NULL && wMsgBodyLen > 0 )
        {
            m_wMsgLen = min( wMsgBodyLen, static_cast<u16>(MAX_FILESRV_MSG_LEN) );
            memcpy( m_abyMsgBody, pMsgBody, m_wMsgLen );
            m_wMsgLen = htons( m_wMsgLen );
        }
    }

    //在已设置的消息体尾部拼接消息体
    inline void CatMsgBody(const void *pMsgBody = NULL, u16 wMsgBodyLen = 0)
    {
        if ( pMsgBody != NULL && wMsgBodyLen > 0 )
        {
            u16 wCurLen = ntohs( m_wMsgLen );//当前已存入的消息体长度
            //本次追加的消息体长度
            u16 wThisLen = min( wMsgBodyLen,
                                static_cast<u16>(MAX_FILESRV_MSG_LEN - wCurLen) );
            memcpy( m_abyMsgBody + wCurLen, pMsgBody, wThisLen );
            m_wMsgLen = htons( wCurLen + wThisLen );
        }
    }

    //获取消息体体长度
    inline u16 GetBodyLen(void) const { return ntohs( m_wMsgLen ); }

    //获取消息体内容
    inline const u8* GetMsgBody(void) const { return m_abyMsgBody; }

    //发送OSP消息获取整个消息长度
    inline u16 GetSrvMsgLen(void) const 
    { return ntohs( m_wMsgLen ) + MAX_FILESRV_MSG_HEAD_LEN; }

    //发送OSP消息获取整个消息结构指针
    inline const u8 *GetSrvMsg(void) const
    { return reinterpret_cast<const u8*>(this); }

private:
    u32         m_dwServiceNo;      //*_LOGIN_ACK里给出的服务号，Req消息不用填写
    u32         m_dwSrcVer;         //消息体源版本号，不需要填写
    u32         m_dwSerialNo;       //流水号，Server端透明转发，主要用于消息同步处理(终端测每发一条REQ则自动+1，那么相应的ACK序列号一样)
    u32         m_dwErrorCode;      //错误码，NACK消息需要处理，从GetFileSrvErrorStr获取描述
    u16         m_wTotalPkt;        //文件传输总包数(单个文件)
    u16         m_wCurPkt;          //文件传输当前包数(单个文件)
    u16         m_wMsgLen;          //消息体长度
    u8          m_abyResrved[18];   //保留字段
    u8          m_abyMsgBody[MAX_FILESRV_MSG_LEN];  //消息体
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IDevFileSrvMsg
;



#ifdef WIN32
#pragma pack( pop )
#endif//WIN32


#endif//_FILESRVDEVSTRUCT_H
