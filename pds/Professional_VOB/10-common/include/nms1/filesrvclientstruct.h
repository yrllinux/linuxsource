/*=======================================================================
模块名      : 文件升级服务器
文件名      : filesrvclientstruct.h
相关文件    : 无
文件实现功能: 服务器结构定义(主要是与客户端的结构)
作者        : 王昊
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-------------------------------------------------------------------------
修改记录:
日      期  版本    修改人  修改内容
2006/12/28  1.0     王昊    创建
=======================================================================*/

#ifndef _FILESRVCLIENTSTRUCT_H
#define _FILESRVCLIENTSTRUCT_H

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif//WIN32

#include "filesrvstruct.h"
#include "ummessagestruct.h"

#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif//WIN32

/*------------------------------------------------------------------------------
  服务器与客户端交互结构
------------------------------------------------------------------------------*/

//软件版本号结构
typedef struct tagFCSoftwareVer//len: 16
{
public:
    tagFCSoftwareVer() : m_wMainVer(0), m_wSubVer(0), m_wInterfaceVer(0),
                           m_wModifyVer(0), m_dwDate(0), m_dwReserved1(0)
    {}

    tagFCSoftwareVer(u16 wMainVer, u16 wSubVer, u16 wIntVer, u16 wModVer,
                     u32 dwDate) : m_dwReserved1(0)
    {
        m_wMainVer      = htons( wMainVer );
        m_wSubVer       = htons( wSubVer );
        m_wInterfaceVer = htons( wIntVer );
        m_wModifyVer    = htons( wModVer );
        m_dwDate        = htonl( dwDate );
    }

    tagFCSoftwareVer(LPCSTR lptrVer) : m_dwReserved1(0)
    { FromString( lptrVer ); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagFCSoftwareVer) ); }

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
    inline void SetModifyVer(u16 wModifyVer)
    { m_wModifyVer = htons( wModifyVer ); }

    //获取修改版本号
    inline u16 GetModifyVer(void) const { return ntohs( m_wModifyVer ); }

    //设置日期
    inline void SetDate(u32 dwDate) { m_dwDate = htonl( dwDate ); }

    //获取日期
    inline u32 GetDate(void) const { return ntohl( m_dwDate ); }

    //比较版本
    inline BOOL32 operator<(const tagFCSoftwareVer &tVer)
    {
        return ( memcmp( this, &tVer, sizeof (tagFCSoftwareVer) ) < 0 );
    }
    inline BOOL32 operator>(const tagFCSoftwareVer &tVer)
    {
        return ( memcmp( this, &tVer, sizeof (tagFCSoftwareVer) ) > 0 );
    }
    inline BOOL32 operator==(const tagFCSoftwareVer &tVer)
    {
        return ( memcmp( this, &tVer, sizeof (tagFCSoftwareVer) ) == 0 );
    }

    //转成字符串
    inline void ToString(s8 szVer[MAX_FILESRV_SOFTVER_LEN]) const
    {
//         sprintf( szVer, "%.2d.%.2d.%.2d.%.2d.%.6d", ntohs( m_wMainVer ),
//                  ntohs( m_wSubVer ), ntohs( m_wInterfaceVer ),
//                  ntohs( m_wModifyVer ), ntohl( m_dwDate ) );
        sprintf( szVer, "%d.%d.%d.%d.%.6d", ntohs( m_wMainVer ),
                 ntohs( m_wSubVer ), ntohs( m_wInterfaceVer ),
                 ntohs( m_wModifyVer ), ntohl( m_dwDate ) );
        return;
    }

	inline int FindChar(LPCSTR lpStr, char chChar, int nStartPos)
	{
		int nPos = nStartPos;
		char chCurChar;
		int nLenth = strlen(lpStr);
		for (; nPos < nLenth; nPos++)
		{
			chCurChar = *(lpStr + nPos);
			if (chCurChar == chChar)
			{
				return nPos;
			}
		}
		return -1;

	}
    //从字符串读取
    inline BOOL32 FromString(LPCSTR lptrVer)
    {
//         if ( lptrVer == NULL || strlen(lptrVer) != 18 )
//         {
//             return FALSE;
//         }
//         m_wMainVer      = htons( atoi( lptrVer ) );
//         m_wSubVer       = htons( atoi( lptrVer + 3 ) );
//         m_wInterfaceVer = htons( atoi( lptrVer + 6 ) );
//         m_wModifyVer    = htons( atoi( lptrVer + 9 ) );
//         m_dwDate        = htonl( atoi( lptrVer + 12) );

		m_wMainVer = htons( atoi( lptrVer ) );

		int nPos = FindChar(lptrVer, '.', 0 );
		m_wSubVer = htons( atoi( lptrVer + nPos + 1 ) );

        nPos = FindChar(lptrVer, '.', nPos + 1 );
		m_wInterfaceVer = htons( atoi( lptrVer + nPos + 1 ) );

		nPos = FindChar(lptrVer, '.', nPos + 1 );
		m_wModifyVer = htons( atoi( lptrVer + nPos + 1 ) );

		nPos = FindChar(lptrVer, '.', nPos + 1 );
		m_dwDate = htonl( atoi( lptrVer + nPos + 1 ) );

     

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
IFCSoftwareVer
;

typedef struct tagFCHardwareVer//len: 32
{
public:
    tagFCHardwareVer() { SetNull(); }

    //置空
    inline void SetNull(void)
    { memset( this, 0, sizeof (tagFCHardwareVer) ); }

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
//
//    //指定硬件版本号是否在范围之内
//    BOOL32 IsHardwareVerValid(u8 byHardwareVer)
//    {
//        for ( s32 nIndex = 0; nIndex < sizeof (m_szHardwareVer) - 1; nIndex ++ )
//        {
//            if ( m_szHardwareVer[nIndex] == 0 )
//            {
//                return FALSE;
//            }
//            else if ( m_szHardwareVer[nIndex] == byHardwareVer + 1 )
//            {
//                return TRUE;
//            }
//        }
//        return FALSE;
//    }
//
//    //指定硬件版本号是否在范围之内
//    BOOL32 IsHardwareVerValid(const s8 szHardVer[MAX_FILESRV_HARDVER_LEN])
//    {
//        s32 nIndex = 0;
//        s32 nIdx = 0;
//        for ( ; m_szHardwareVer[nIndex] != '\0'; nIndex ++ )
//        {
//            for ( ; szHardVer[nIdx] != '\0'; nIdx ++ )
//            {
//                if ( m_szHardwareVer[nIndex] == szHardVer[nIdx] )
//                {
//                    return FALSE;
//                }
//            }
//        }
//        return (nIndex > 0 && nIdx > 0 ) ? TRUE : FALSE;
//    }
//
//    //清除
//    BOOL32 RemoveConflictVer(const s8 szHardVer[MAX_FILESRV_HARDVER_LEN])

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
IFCHardwareVer
;

class CFCSrvUserInfo : public CUserFullInfo//len: 257
{
public:
    CFCSrvUserInfo() : CUserFullInfo(), m_dwUserNo(0) {}

    ~CFCSrvUserInfo() {}

    //设置用户编号
    inline void SetUserNo(u32 dwUserNo) { m_dwUserNo = htonl( dwUserNo ); }

    //获取用户编号
    inline u32 GetUserNo(void) const { return ntohl( m_dwUserNo ); }

private:
    u32 m_dwUserNo;     //用户编号
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
;

//客户端登录结构
typedef struct tagFCSrvLoginInfo//len: 64
{
public:
    tagFCSrvLoginInfo() { SetNull(); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagFCSrvLoginInfo) ); }

    //获取用户名
    inline LPCSTR GetUserName(void) const { return m_szUserName; }

    //设置用户，密码不用加密
    BOOL32 SetUser(LPCSTR lptrUser, LPCSTR lptrPwd)
    {
        SetNull();
        if ( lptrUser == NULL )
        {
            return FALSE;
        }
        strncpy( m_szUserName, lptrUser, sizeof (m_szUserName) - 1 );
        if ( lptrPwd != NULL )
        {
            CMD5Encrypt cMd5;
            s8 szPwd[32];
            cMd5.GetEncrypted( const_cast<LPSTR>(lptrPwd),
                               const_cast<LPSTR>(szPwd) );
            strncpy( m_szPassword, szPwd, sizeof (m_szPassword) - 1 );
        }
        return TRUE;
    }

    //获取密码(已加密)
    inline LPCSTR GetPassword(void) const { return m_szPassword; }

private:
    s8      m_szUserName[MAX_FILESRV_USERNAME_LEN];//用户名
    s8      m_szPassword[MAX_FILESRV_PASSWORD_LEN];//经过MD5加密的密码
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvLoginInfo
;

//设备标识
typedef struct tagFCSrvDev//len: 4
{
public:
    tagFCSrvDev() : m_byDevType(0), m_byDevModel(0), m_byDevOsType(0),
    	            m_byReserved1(0)
    {}

    tagFCSrvDev(u8 byDevType, u8 byDevModel, u8 byDevOsType)
        : m_byDevType(byDevType), m_byDevModel(byDevModel),
          m_byDevOsType(byDevOsType), m_byReserved1(0) {}

    tagFCSrvDev(s8 szType[MAX_FILESRV_DEVSTRING_LEN]) : m_byReserved1(0)
    { FromString( szType ); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagFCSrvDev) ); }

    //设置设备类型
    inline void SetType(EMFileSrvDevType emDevType)
    { m_byDevType = static_cast<u8>(emDevType); }

    //获取设备类型
    inline EMFileSrvDevType GetType(void) const
    { return static_cast<EMFileSrvDevType>(m_byDevType); }

    //设置设备型号
    inline void SetModel(u8 byDevModel) { m_byDevModel = byDevModel; }

    //获取设备型号
    inline u8 GetModel(void) const { return m_byDevModel; }

    //设置设备操作系统类型
    inline void SetOsType(EMFileSrvOSType emOsType)
    { m_byDevOsType = static_cast<u8>(emOsType); }

    //获取操作系统类型
    inline EMFileSrvOSType GetOsType(void) const
    { return static_cast<EMFileSrvOSType>(m_byDevOsType); }

    //转成字符串
    void ToString(s8 szType[MAX_FILESRV_DEVSTRING_LEN]) const
    {
        memset( szType, '_', MAX_FILESRV_DEVSTRING_LEN - 1 );
        szType[MAX_FILESRV_DEVSTRING_LEN - 1] = '\0';
        switch ( m_byDevType )
        {
        case enumFileSrvDevTypeMT:
            memcpy( szType, "mt", 2 );
            break;
        case enumFileSrvDevTypeFileSrv:
            memcpy( szType, "sus", 3 );
            break;
        case enumFileSrvDevTypeClient:
            memcpy( szType, "suc", 3 );
            break;
        default:
            break;
        }
        switch ( m_byDevModel )
        {
        case enumFileSrvDevModelMTTS6610:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6610", 6 );
            break;
        case enumFileSrvDevModelMTTS5210:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS5210", 6 );
            break;
        case enumFileSrvDevModelMTV5:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "V5", 2 );
            break;
        case enumFileSrvDevModelMTTS3210:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS3210", 6 );
            break;
        case enumFileSrvDevModelMTTS6610E:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6610E", 7 );
            break;
        case enumFileSrvDevModelMTTS6210:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6210", 6 );
            break;
        case enumFileSrvDevModelMTTS6210E:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6210E", 7 );
            break;
		case enumFileSrvDevModelMTTS5610:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS5610", 6 );
            break;
		case enumFileSrvDevModelMTTS3610:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS3610", 6 );
            break;
		case enumFileSrvDevModelMTKDV7910:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "KDV7910", 7 );
            break;
		case enumFileSrvDevModelMTKDV7810:
            memcpy( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "KDV7810", 7 );
            break;
        default:
            break;
        }
        switch ( m_byDevOsType )
        {
        case enumFileSrvOSTypeVxworksRaw:
            memcpy( szType + FILESRV_DEVSTRING_OS_BEGIN, "Vxworks Raw", 11 );
            break;
        case enumFileSrvOsTypeVxworksTffs:
            memcpy( szType + FILESRV_DEVSTRING_OS_BEGIN, "Vxworks Tffs", 12 );
            break;
        case enumFileSrvOsTypeWindows:
            memcpy( szType + FILESRV_DEVSTRING_OS_BEGIN, "Windows", 7 );
            break;
        case enumFileSrvOsTypeLinuxPowerPC:
            memcpy( szType + FILESRV_DEVSTRING_OS_BEGIN, "Linux PowerPC", 13 );
            break;
        case enumFileSrvOsTypeLinuxDavinci:
            memcpy( szType + FILESRV_DEVSTRING_OS_BEGIN, "Linux ARM", 9 );
            break;
        default:
            break;
        }
    }

    //从字符串读取
    BOOL32 FromString(s8 szType[MAX_FILESRV_DEVSTRING_LEN])
    {
        if ( strlen( szType ) != MAX_FILESRV_DEVSTRING_LEN - 1 )
            return FALSE;

        if ( memcmp( szType, "mt", 2 ) == 0 )
            m_byDevType = enumFileSrvDevTypeMT;
        else if ( memcmp( szType, "sus", 3 ) == 0 )
            m_byDevType = enumFileSrvDevTypeFileSrv;
        else if ( memcmp( szType, "suc", 3 ) == 0 )
            m_byDevType = enumFileSrvDevTypeClient;
        else
            m_byDevType = enumFileSrvDevTypeUnknown;

        if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6610E", 7 ) == 0 )
            m_byDevModel = enumFileSrvDevModelMTTS6610E;
        else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS5210", 6 ) == 0 )
            m_byDevModel = enumFileSrvDevModelMTTS5210;
        else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "V5", 2 ) == 0 )
            m_byDevModel = enumFileSrvDevModelMTV5;
        else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS3210", 6 ) == 0 )
            m_byDevModel = enumFileSrvDevModelMTTS3210;
        else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6610", 6 ) == 0 )
            m_byDevModel = enumFileSrvDevModelMTTS6610;
		// 6210E一定要放在6210前判断
        else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6210E", 7 ) == 0 )
            m_byDevModel = enumFileSrvDevModelMTTS6210E;
        else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS6210", 6 ) == 0 )
            m_byDevModel = enumFileSrvDevModelMTTS6210;

		else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS5610", 6 ) == 0 )
			m_byDevModel = enumFileSrvDevModelMTTS5610;
		else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "TS3610", 6 ) == 0 )
			m_byDevModel = enumFileSrvDevModelMTTS3610;
		else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "KDV7910", 7 ) == 0 )
			m_byDevModel = enumFileSrvDevModelMTKDV7910;
		else if ( memcmp( szType + FILESRV_DEVSTRING_MODEL_BEGIN, "KDV7810", 7 ) == 0 )
			m_byDevModel = enumFileSrvDevModelMTKDV7810;

        else
            m_byDevModel = enumFileSrvDevModelMTUnknown;

        if ( memcmp( szType + FILESRV_DEVSTRING_OS_BEGIN, "Vxworks Raw", 11 ) == 0 )
            m_byDevOsType = enumFileSrvOSTypeVxworksRaw;
        else if ( memcmp( szType + FILESRV_DEVSTRING_OS_BEGIN, "Vxworks Tffs", 12 ) == 0 )
            m_byDevOsType = enumFileSrvOsTypeVxworksTffs;
        else if ( memcmp( szType + FILESRV_DEVSTRING_OS_BEGIN, "Windows", 7 ) == 0 )
            m_byDevOsType = enumFileSrvOsTypeWindows;
        else if ( memcmp( szType + FILESRV_DEVSTRING_OS_BEGIN, "Linux PowerPC", 13 ) == 0 )
            m_byDevOsType = enumFileSrvOsTypeLinuxPowerPC;
        else if ( memcmp( szType + FILESRV_DEVSTRING_OS_BEGIN, "Linux ARM", 9 ) == 0 )
            m_byDevOsType = enumFileSrvOsTypeLinuxDavinci;
        else
            m_byDevOsType = enumFileSrvOSTypeUnknown;

        return TRUE;
    }

private:
    u8      m_byDevType;                //设备类型(EMFileSrvDevType)
    u8      m_byDevModel;               //设备型号(EMFileSrvDevMTMode)
    u8      m_byDevOsType;              //设备操作系统类型(EMFileSrvOSType)
    u8      m_byReserved1;              //保留字段
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvDev
;

//文件基本信息
typedef struct tagFCSrvFileInfo//len: 72
{
public:
    tagFCSrvFileInfo() : m_dwFileType(0), m_dwFileSize(0)
    { SetNull(); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagFCSrvFileInfo) ); }

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
IFCSrvFileInfo
;

//设备版本基本信息
typedef struct tagFCSrvDevVerInfo//len: 856
{
public:
    tagFCSrvDevVerInfo() : m_byFileNum(0)
    { SetNull(); }

    //置空
    inline void SetNull(void)
    { memset( this, 0, sizeof (tagFCSrvDevVerInfo) ); }

    //设置设备
    inline void SetDev(const IFCSrvDev &tDev) { m_tDev = tDev; }

    //获取设备
    inline IFCSrvDev GetDev(void) const { return m_tDev; }

    //设置硬件版本号
    inline void SetHardwareVer(const IFCHardwareVer &tHardVer)
    { m_tHardVer = tHardVer; }

    //获取硬件版本号
    inline IFCHardwareVer GetHardwareVer(void) const { return m_tHardVer; }

    //设置软件版本号
    inline void SetSoftwareVer(const IFCSoftwareVer &tSoftVer)
    { m_tSoftVer = tSoftVer; }

    //获取软件版本号
    inline IFCSoftwareVer GetSoftwareVer(void) const { return m_tSoftVer; }

    //设置修复描述文字
    inline void SetBugReport(LPCSTR lptrBugReport)
    {
        memset( m_aszBugReport, 0, sizeof (m_aszBugReport) );
        if ( lptrBugReport != NULL )
        {
            strncpy( m_aszBugReport, lptrBugReport,
                     sizeof (m_aszBugReport) - 1 );
        }
    }

    //获取修复描述
    inline LPCSTR GetBugReport(void) const { return m_aszBugReport; }

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
                                    * sizeof (IFCSrvFileInfo) );
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
                                    * sizeof (IFCSrvFileInfo) );
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

    //清空列表
    inline void ClearFileList(void)
    {
        memset( m_atFileInfo, 0, sizeof (m_atFileInfo) );
        m_byFileNum = 0;
    }

    //获取文件总数
    inline s32 GetFileNum(void) const
    { return static_cast<s32>(m_byFileNum); }

    //获取一个文件
    inline IFCSrvFileInfo GetOneFile(s32 nIndex) const
    {
        if ( nIndex < static_cast<s32>(m_byFileNum) )
        {
            return m_atFileInfo[nIndex];
        }
        else
        {
            IFCSrvFileInfo tInfo;
            return tInfo;
        }
    }

    //指定类型的文件是否存在
    inline BOOL32 IsFileExist(EMFileSrvFileType emFileType)
    {
        for ( u8 byIndex = 0; byIndex < m_byFileNum; byIndex ++ )
        {
            if ( m_atFileInfo[byIndex].GetFileType() == emFileType )
            {
                return TRUE;
            }
        }
        return FALSE;
    }

private:
    IFCSrvDev       m_tDev;             //设备
    IFCHardwareVer  m_tHardVer;         //硬件版本号
    IFCSoftwareVer  m_tSoftVer;         //软件版本号
    s8              m_aszBugReport[MAX_FILESRV_BUG_REPORT_LEN];//文件修复信息描述
    u8              m_byFileNum;        //该设备版本需要的文件总个数
    u8              m_abyReserved1[3];  //保留字段
    IFCSrvFileInfo  m_atFileInfo[MAX_FILESRV_DEVFILE_NUM];//文件信息
}
//#ifndef WIN32
//__attribute__ ( (packed) )
//#endif//WIN32
IFCSrvDevVerInfo
;

//文件完整信息
typedef struct tagFCSrvFileFullInfo : public tagFCSrvFileInfo//len: 76
{
public:
    tagFCSrvFileFullInfo() : m_dwFileNo(0)
    { SetNull(); }

    //置空
    inline void SetNull(void)
    { memset( this, 0, sizeof (tagFCSrvFileFullInfo) ); }

    //设置文件编号
    inline void SetFileNo(u32 dwFileNo)
    { m_dwFileNo = htonl( dwFileNo ); }

    //获取文件编号
    inline u32 GetFileNo(void) const
    { return ntohl( m_dwFileNo ); }

private:
    u32     m_dwFileNo;         //文件编号
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvFileFullInfo
;

//设备版本完整信息
//设备版本基本信息
typedef struct tagFCSrvDevVerFullInfo//len: 876
{
public:
    tagFCSrvDevVerFullInfo() : m_dwDevNo(0), m_byFileNum(0), m_byRecommend(0)
    { SetNull(); }

    //置空
    inline void SetNull(void)
    { memset( this, 0, sizeof (tagFCSrvDevVerFullInfo) ); }

    //设置编号
    inline void SetDevNo(u32 dwDevNo)
    { m_dwDevNo = htonl( dwDevNo ); }

    //获取编号
    inline u32 GetDevNo(void) const
    { return ntohl( m_dwDevNo ); }

    //设置设备
    inline void SetDev(const IFCSrvDev &tDev) { m_tDev = tDev; }

    //获取设备
    inline const IFCSrvDev& GetDev(void) const { return m_tDev; }

    //设置硬件版本号
    inline void SetHardwareVer(const IFCHardwareVer &tHardVer)
    { m_tHardVer = tHardVer; }

    //获取硬件版本号
    inline IFCHardwareVer GetHardwareVer(void) const { return m_tHardVer; }

    //设置软件版本号
    inline void SetSoftwareVer(const IFCSoftwareVer &tSoftVer)
    { m_tSoftVer = tSoftVer; }

    //获取软件版本号
    inline IFCSoftwareVer GetSoftwareVer(void) const { return m_tSoftVer; }

    //设置修复描述文字
    inline void SetBugReport(LPCSTR lptrBugReport)
    {
        memset( m_aszBugReport, 0, sizeof (m_aszBugReport) );
        if ( lptrBugReport != NULL )
        {
            strncpy( m_aszBugReport, lptrBugReport,
                     sizeof (m_aszBugReport) - 1 );
        }
    }

    //获取修复描述
    inline LPCSTR GetBugReport(void) const { return m_aszBugReport; }

    //设置是否推荐
    inline void SetRecommend(BOOL32 bRecommend)
    { m_byRecommend = bRecommend ? 1 : 0; }

    //获取是否推荐
    inline BOOL32 IsRecommend(void) const
    { return (m_byRecommend == 1 ? TRUE : FALSE); }

    //添加文件
    BOOL32 AddOneFile(u32 dwFileNo, EMFileSrvFileType emFileType,
                      u32 dwFileSize, LPCSTR lptrFileName)
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

        m_atFileInfo[m_byFileNum].SetFileNo( dwFileNo );
        m_atFileInfo[m_byFileNum].SetFileType( emFileType );
        m_atFileInfo[m_byFileNum].SetFileSize( dwFileSize );
        m_atFileInfo[m_byFileNum].SetFileName( lptrFileName );
        m_byFileNum ++;
        return TRUE;
    }

    //修改文件
    BOOL32 EditOneFile(u32 dwFileNo, EMFileSrvFileType emFileType,
                       u32 dwFileSize, LPCSTR lptrFileName)
    {
        if ( m_byFileNum == 0 || lptrFileName == NULL || dwFileSize == 0 )//参数错误
        {
            return FALSE;
        }
        for ( u8 byIndex = 0; byIndex < m_byFileNum; byIndex ++ )
        {
            if ( m_atFileInfo[byIndex].GetFileType() == emFileType )//文件类型已存在
            {
                m_atFileInfo[byIndex].SetFileNo( dwFileNo );
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
                                    * sizeof (IFCSrvFileFullInfo) );
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
                                    * sizeof (IFCSrvFileFullInfo) );
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

    //清空列表
    inline void ClearFileList(void)
    {
        memset( m_atFileInfo, 0, sizeof (m_atFileInfo) );
        m_byFileNum = 0;
    }

    //获取文件总数
    inline s32 GetFileNum(void) const
    { return static_cast<s32>(m_byFileNum); }

    //获取一个文件
    inline IFCSrvFileFullInfo GetOneFile(s32 nIndex) const
    {
        if ( nIndex < static_cast<s32>(m_byFileNum) )
        {
            return m_atFileInfo[nIndex];
        }
        else
        {
            IFCSrvFileFullInfo tInfo;
            return tInfo;
        }
    }

    //指定类型的文件是否存在
    inline BOOL32 IsFileExist(EMFileSrvFileType emFileType)
    {
        for ( u8 byIndex = 0; byIndex < m_byFileNum; byIndex ++ )
        {
            if ( m_atFileInfo[byIndex].GetFileType() == emFileType )
            {
                return TRUE;
            }
        }
        return FALSE;
    }

private:
    u32             m_dwDevNo;          //设备编号
    IFCSrvDev       m_tDev;             //设备
    IFCHardwareVer  m_tHardVer;         //硬件版本号
    IFCSoftwareVer  m_tSoftVer;         //软件版本号
    s8              m_aszBugReport[MAX_FILESRV_BUG_REPORT_LEN];//文件修复信息描述
    u8              m_byFileNum;        //该设备版本需要的文件总个数
    u8              m_byRecommend;      //该版本是否为推荐版本
    u8              m_abyReserved1[2];  //保留字段
    IFCSrvFileFullInfo  m_atFileInfo[MAX_FILESRV_DEVFILE_NUM];//文件信息
}
//#ifndef WIN32
//__attribute__ ( (packed) )
//#endif//WIN32
IFCSrvDevVerFullInfo
;

//日志查询结构
typedef struct tagLogReqInfo//len: 64
{
public:
    tagLogReqInfo() { SetNull(); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagLogReqInfo) ); }

    //设置主类型
    inline void SetMainType(EMFileSrvLogReqMainType emType)
    { m_byMainType = static_cast<u8>(emType); }

    //获取主类型
    inline EMFileSrvLogReqMainType GetMainType(void) const
    { return static_cast<EMFileSrvLogReqMainType>(m_byMainType); }

    //设置日志类型子类型
    inline void SetSubType(EMFileSrvLogReqSubTypeCategory emType)
    { m_wSubTypeCategory = htons( static_cast<u16>(emType) ); }

    //获取日志类型子类型
    inline EMFileSrvLogReqSubTypeCategory GetCategorySubType(void) const
    { return static_cast<EMFileSrvLogReqSubTypeCategory>(ntohs(m_wSubTypeCategory)); }

    //设置日志级别子类型
    inline void SetSubType(EMFileSrvLogReqSubTypeLevel emType)
    { m_wSubTypeLevel = htons( static_cast<u16>(emType) ); }

    //获取日志级别子类型
    inline EMFileSrvLogReqSubTypeLevel GetLevelSubType(void) const
    { return static_cast<EMFileSrvLogReqSubTypeLevel>(ntohs(m_wSubTypeLevel)); }

    //设置时间子类型
    inline void SetSubType(EMFileSrvLogReqSubTypeTime emType)
    { m_wSubTypeTime = htons( static_cast<u16>(emType) ); }

    //获取时间子类型
    inline EMFileSrvLogReqSubTypeTime GetTimeSubType(void) const
    { return static_cast<EMFileSrvLogReqSubTypeTime>(ntohs(m_wSubTypeTime)); }

    //设置设备子类型
    inline void SetSubType(EMFileSrvLogReqSubTypeDev emType)
    { m_wSubTypeDev = htons( static_cast<u16>(emType) ); }

    //获取设备子类型
    inline EMFileSrvLogReqSubTypeDev GetDevSubType(void) const
    { return static_cast<EMFileSrvLogReqSubTypeDev>(ntohs(m_wSubTypeDev)); }

    //设置操作结果子类型
    inline void SetSubType(EMFileSrvLogReqSubTypeResult emType)
    { m_wSubTypeResult = htons( static_cast<u16>(emType) ); }

    //获取操作结果子类型
    inline EMFileSrvLogReqSubTypeResult GetResultSubType(void) const
    { return static_cast<EMFileSrvLogReqSubTypeResult>(ntohs(m_wSubTypeResult)); }

    //设置事件子类型
    inline void SetSubType(EMFileSrvLogReqSubTypeEvent emType)
    { m_wSubTypeEvent = htons( static_cast<u16>(emType) ); }

    //获取事件子类型
    inline EMFileSrvLogReqSubTypeEvent GetEventSubType(void) const
    { return static_cast<EMFileSrvLogReqSubTypeEvent>(ntohs(m_wSubTypeEvent)); }

    //设置事件二级子类型设备操作
    inline void SetEventSubType(EMFileSrvLogReqEventSubTypeDevOpr emType)
    { m_wSubTypeEventSubTypeDevOpr = htons( static_cast<u16>(emType) ); }

    //获取事件二级子类型设备操作
    inline EMFileSrvLogReqEventSubTypeDevOpr GetEventSubTypeDevOpr(void) const
    { return static_cast<EMFileSrvLogReqEventSubTypeDevOpr>(
                        ntohs(m_wSubTypeEventSubTypeDevOpr) ); }

    //设置事件二级子类型客户端操作
    inline void SetEventSubType(EMFileSrvLogReqEventSubTypeClientOpr emType)
    { m_wSubTypeEventSubTypeClientOpr = htons( static_cast<u16>(emType) ); }

    //获取事件二级子类型客户端操作
    inline EMFileSrvLogReqEventSubTypeClientOpr GetEventSubTypeClientOpr(void) const
    { return static_cast<EMFileSrvLogReqEventSubTypeClientOpr>(
                        ntohs(m_wSubTypeEventSubTypeClientOpr) ); }

    //设置设备
    inline void SetDev(const IFCSrvDev &tDev) { m_tDev = tDev; }

    //获取设备
    inline IFCSrvDev GetDev(void) const { return m_tDev; }

    //设置硬件版本号
    inline void SetHardwareVer(const IFCHardwareVer &tHardVer)
    { m_tHardVer = tHardVer; }

    //获取硬件版本号
    inline IFCHardwareVer GetHardwareVer(void) const { return m_tHardVer; }

    //设置起始时间(格林威治时间)
    inline void SetStartTime(time_t dwTime) { m_dwStartTime = htonl( dwTime ); }

    //获取起始时间
    inline time_t GetStartTime(void) const { return ntohl( m_dwStartTime ); }

    //设置结束时间(格林威治时间)
    inline void SetEndTime(time_t dwTime) { m_dwEndTime = htonl( dwTime ); }

    //获取结束时间
    inline time_t GetEndTime(void) const { return ntohl( m_dwEndTime ); }

private:
    u8          m_byMainType;       //主类型(EMFileSrvLogReqMainType，多种类型用 | 连接)
    u8          m_byReserved1;
    u16         m_wReserved2;
    u16         m_wSubTypeCategory; //子类型(EMFileSrvLogReqSubTypeCategory，多种类型用 | 连接)
    u16         m_wSubTypeLevel;    //子类型(EMFileSrvLogReqSubTypeLevel，多种类型用 | 连接)
    u16         m_wSubTypeTime;     //子类型(EMFileSrvLogReqSubTypeTime，多种类型用 | 连接)
    u16         m_wSubTypeDev;      //子类型(EMFileSrvLogReqSubTypeDev，多种类型用 | 连接)
    u16         m_wSubTypeResult;   //子类型(EMFileSrvLogReqSubTypeResult，多种类型用 | 连接)
    u16         m_wSubTypeEvent;    //子类型(EMFileSrvLogReqSubTypeEvent，多种类型用 | 连接)
    u16         m_wSubTypeEventSubTypeDevOpr;   //子类型(EMFileSrvLogReqEventSubTypeDevOpr，多种类型用 | 连接)
    u16         m_wSubTypeEventSubTypeClientOpr;//子类型(EMFileSrvLogReqEventSubTypeClientOpr，多种类型用 | 连接)
    IFCSrvDev           m_tDev;         //指定设备
    IFCHardwareVer      m_tHardVer;     //硬件版本号
    time_t      m_dwStartTime;   //指定起始时间(格林威治时间)
    time_t      m_dwEndTime;     //指定结束时间(格林威治时间)
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
ILogReqInfo
;

//日志查询结果
typedef struct tagLogRetInfo//len: 120
{
public:
    tagLogRetInfo() : m_dwSerialID(0), m_wCategory(0), m_wLevel(0),
                      m_wEvent(0), m_wEventSubType(0), m_dwTime(0),
                      m_dwErrCode(0)
    { memset( m_abyReserved, 0, sizeof (m_abyReserved) ); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagLogRetInfo) ); }

    //设置序列号
    inline void SetSerialNo(u32 dwSN) { m_dwSerialID = htonl( dwSN ); }

    //获取序列号
    inline u32 GetSerialNo(void) const { return ntohl( m_dwSerialID ); }

    //设置日志类型
    inline void SetCategory(EMFileSrvLogReqSubTypeCategory emType)
    { m_wCategory = htons( static_cast<u16>(emType) ); }

    //获取日志类型
    inline EMFileSrvLogReqSubTypeCategory GetCategory(void) const
    { return static_cast<EMFileSrvLogReqSubTypeCategory>(ntohs(m_wCategory)); }

    //设置日志级别
    inline void SetLevel(EMFileSrvLogReqSubTypeLevel emType)
    { m_wLevel = htons( static_cast<u16>(emType) ); }

    //获取日志级别
    inline EMFileSrvLogReqSubTypeLevel GetLevel(void) const
    { return static_cast<EMFileSrvLogReqSubTypeLevel>(ntohs(m_wLevel)); }

    //设置事件类型
    inline void SetEvent(EMFileSrvLogReqSubTypeEvent emType)
    { m_wEvent = htons( static_cast<u16>(emType) ); }

    //获取事件类型
    inline EMFileSrvLogReqSubTypeEvent GetEvent(void) const
    { return static_cast<EMFileSrvLogReqSubTypeEvent>(ntohs(m_wEvent)); }

    //设置事件二级子类型
    inline void SetEventSubType(u16 wType) { m_wEventSubType = htons(wType); }

    //获取事件二级子类型
    inline u16 GetEventSubType(void) const { return ntohs( m_wEventSubType ); }

    //设置设备
    inline void SetDev(const IFCSrvDev &tDev) { m_tDev = tDev; }

    //获取设备
    inline IFCSrvDev GetDev(void) const { return m_tDev; }

    //设置硬件版本号
    inline void SetHardwareVer(const IFCHardwareVer &tVer) { m_tHardVer = tVer; }

    //获取硬件版本号
    inline IFCHardwareVer GetHardwareVer(void) const { return m_tHardVer; }

    //设置时间(格林威治时间)
    inline void SetTime(time_t dwTime) { m_dwTime = htonl( dwTime ); }

    //获取时间(格林威治时间)
    inline time_t GetTime(void) const { return ntohl( m_dwTime ); }

    //设置错误码
    inline void SetErrorCode(u32 dwErrorCode) { m_dwErrCode = htonl(dwErrorCode); }

    //获取错误码
    inline u32 GetErrorCode(void) const { return ntohl( m_dwErrCode ); }

private:
    u32         m_dwSerialID;   //日志在数据库中存储的序列号
    u16         m_wCategory;    //日志类型(EMFileSrvLogReqSubTypeCategory)
    u16         m_wLevel;       //日志等级(EMFileSrvLogReqSubTypeLevel)
    u16         m_wEvent;       //事件(EMFileSrvLogReqSubTypeEvent)
    u16         m_wEventSubType;//事件子类型(EMFileSrvLogReqEventSubTypeDevOpr/ EMFileSrvLogReqEventSubTypeClientOpr)
    IFCSrvDev   m_tDev;         //设备
    IFCHardwareVer  m_tHardVer; //硬件版本号
    time_t      m_dwTime;       //时间(格林威治时间)
    u32         m_dwErrCode;    //结果错误码
    s8          m_abyReserved[64];  //保留值(用户操作时为操作的用户名，版本操作时为更新后的版本号)
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
ILogRetInfo
;


//数据库连接信息
typedef struct tagFCSrcDBConfig//len: 184
{
public:
    tagFCSrcDBConfig() : m_wDBPort(0), m_byReserved1(0)
    { SetNull(); }

    //置空
    inline void SetNull(void)
    { memset( this, 0, sizeof (tagFCSrcDBConfig) ); }

    //设置名称
    inline void SetName(LPCSTR lptrName)
    {
        memset( m_szDBName, 0, sizeof (m_szDBName) );
        if ( lptrName != NULL )
        {
            strncpy( m_szDBName, lptrName, sizeof (m_szDBName) - 1 );
        }
    }

    //获取名称
    inline LPCSTR GetName(void) const { return m_szDBName; }

    //设置IP
    inline void SetIP(u32 dwIP)
    {
        u8 *pbyBuf = reinterpret_cast<u8*>(&dwIP);
        sprintf( m_szDBIP, "%d.%d.%d.%d%c", pbyBuf[3], pbyBuf[2], pbyBuf[1],
                 pbyBuf[0], 0 );
    }

    //设置IP
    void SetIP(LPCSTR lptrIP)
    {
        memset( m_szDBIP, 0, sizeof (m_szDBIP) );
        if ( lptrIP != NULL )
        {
            strncpy( m_szDBIP, lptrIP, sizeof (m_szDBIP) - 1 );
        }
    }

    //获取IP
    inline LPCSTR GetIP(void) const { return m_szDBIP; }

    //设置端口
    inline void SetPort(u16 wPort) { m_wDBPort = htons( wPort ); }

    //获取端口
    inline u16 GetPort(void) const { return ntohs( m_wDBPort ); }

    //设置类型
    inline void SetEngineType(EMDBEngineType emType)
    { m_byDBEngineType = static_cast<u8>(emType); }

    //获取类型
    inline EMDBEngineType GetEngineType(void) const
    { return static_cast<EMDBEngineType>(m_byDBEngineType); }

    //设置用户
    inline void SetUser(LPCSTR lptrUsername, LPCSTR lptrPwd)
    {
        memset( m_szDBUsername, 0, sizeof (m_szDBUsername) );
        memset( m_szDBUserPwd, 0, sizeof (m_szDBUserPwd) );
        if ( lptrUsername != NULL )
        {
            strncpy( m_szDBUsername, lptrUsername, sizeof (m_szDBUsername) - 1 );
        }
        if ( lptrPwd != NULL )
        {
            strncpy( m_szDBUserPwd, lptrPwd, sizeof (m_szDBUserPwd) - 1 );
        }
    }

    //获取用户名
    inline LPCSTR GetUsername(void) const { return m_szDBUsername; }

    //获取密码
    inline LPCSTR GetPassword(void) const { return m_szDBUserPwd; }

private:
    s8  m_szDBName[MAX_DB_NAME_LEN];        //数据库名称
    s8  m_szDBIP[20];                       //数据库连接IP字串
    u16 m_wDBPort;                          //数据库连接端口
    u8  m_byDBEngineType;                   //数据库引擎类型
    u8  m_byReserved1;
    s8  m_szDBUsername[MAX_DB_USERNAME_LEN];//数据库连接用户名
    s8  m_szDBUserPwd[MAX_DB_USER_PWD_LEN]; //数据库连接密码
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvDBConfig
;


//服务器配置
typedef struct tagFCSrvConfig//len: 8
{
    tagFCSrvConfig() : m_dwListenIP(0), m_wTelnetPort(0), m_wListenPort(0)
    {}

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagFCSrvConfig) ); }

    //设置侦听IP
    void SetListenIP(u32 dwIP) { m_dwListenIP = htonl( dwIP ); }

    //获取侦听IP
    u32 GetListenIP(void) const { return ntohl( m_dwListenIP ); }

    //设置侦听端口
    void SetLitenPort(u16 wPort) { m_wListenPort = htons( wPort ); }

    //获取侦听端口
    u16 GetListenPort(void) const { return ntohs( m_wListenPort ); }

    //设置调试端口
    void SetTelnetPort(u16 wPort) { m_wTelnetPort = htons( wPort ); }

    //获取调试端口
    u16 GetTelnetPort(void) const { return ntohs( m_wTelnetPort ); }

private:
    u32     m_dwListenIP;   //侦听IP
    u16     m_wListenPort;  //侦听端口
    u16     m_wTelnetPort;  //调试端口
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvConfig
;

//文件数据配置
typedef struct tagFCSrvFileConfig//len: 328
{
public:
    tagFCSrvFileConfig() : m_dwFTPIP(0), m_wFTPPort(SUS_FTP_DEFAULT_PORT)
    { SetNull(); }

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagFCSrvFileConfig) ); }

//    //设置FTP IP地址
//    inline void SetFtpIP(u32 dwIP) { m_dwFTPIP = htonl( dwIP ); }
//
//    //获取FTP IP地址
//    inline u32 GetFtpIP(void) const { return ntohl( m_dwFTPIP ); }
//
//    //设置FTP 端口
//    inline void SetFtpPort(u16 wPort) { m_wFTPPort = htons( wPort ); }
//
//    //获取FTP 端口
//    inline u16 GetFtpPort(void) const { return ntohs( m_wFTPPort ); }
//
//    //设置FTP 路径
//    inline void SetFtpPath(LPCSTR lptrPath)
//    {
//        memset( m_szFtpPath, 0, sizeof (m_szFtpPath) );
//        strncpy( m_szFtpPath, lptrPath, sizeof (m_szFtpPath) - 1 );
//    }
//
//    //获取FTP 路径
//    inline LPCSTR GetFtpPath(void) const { return m_szFtpPath; }
//
//    //设置FTP 用户名
//    void SetFtpUser(LPCSTR lptrUsername, LPCSTR lptrPwd)
//    {
//        memset( m_szFtpUser, 0, sizeof (m_szFtpUser) );
//        if ( lptrUsername != NULL )
//        {
//            strncpy( m_szFtpUser, lptrUsername, sizeof (m_szFtpUser) - 1 );
//        }
//    }
//
//    //获取FTP 用户名
//    inline LPCSTR GetFtpUser(void) const { return m_szFtpUser; }
//
//    //设置FTP 密码
//    void SetFtpPassword(LPCSTR lptrPwd)
//    {
//        memset( m_szFtpPwd, 0, sizeof (m_szFtpPwd) );
//        if ( lptrPwd != NULL )
//        {
//            strncpy( m_szFtpPwd, lptrPwd, sizeof (m_szFtpPwd) - 1 );
//        }
//    }
//
//    //获取FTP 密码
//    inline LPCSTR GetFtpPassword(void) const { return m_szFtpPwd; }

    //设置本地存放路径
    void SetLocalPath(LPCSTR lptrPath)
    {
        memset( m_szLocalPath, 0, sizeof (m_szLocalPath) );
        if ( lptrPath != NULL )
        {
            strncpy( m_szLocalPath, lptrPath, sizeof (m_szLocalPath) - 1 );
        }
    }

    //获取本地存放路径
    inline LPCSTR GetLocalPath(void) const { return m_szLocalPath; }

private:
    u32 m_dwFTPIP;                              //FTP IP地址
    u16 m_wFTPPort;                             //FTP 端口
    u16 m_wReserved1;
    s8  m_szFtpPath[MAX_SUS_FTP_PATH_LEN];      //FTP路径
    s8  m_szFtpUser[MAX_SUS_FTP_USERNAME_LEN];  //FTP用户名
    s8  m_szFtpPwd[MAX_SUS_FTP_PASSWORD_LEN];   //FTP密码
    s8  m_szLocalPath[MAX_SUS_LOCAL_FILE_PATH]; //本地文件路径
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvFileConfig
;

//看门狗服务器配置
typedef struct tagFCSrvWdSrv//len: 8
{
public:
    tagFCSrvWdSrv() : m_wServerPort(0), m_byCheckCount(0), m_wCheckTime(0)
    { SetNull(); }

    //置空
    inline void SetNull(void)
    { memset( this, 0, sizeof (tagFCSrvWdSrv) ); }

    //设置端口
    inline void SetPort(u16 wPort) { m_wServerPort = htons( wPort ); }

    //获取端口
    inline u16 GetPort(void) const { return ntohs( m_wServerPort ); }

    //设置断链检测次数
    inline void SetCheckCount(u8 byCount) { m_byCheckCount = byCount; }

    //获取断链检测次数
    inline u8 GetCheckCount(void) const { return m_byCheckCount; }

    //设置断链检测时间
    inline void SetCheckTime(u16 wTime) { m_wCheckTime = htons( wTime ); }

    //获取断链检测时间
    inline u16 GetCheckTime(void) const { return ntohs( m_wCheckTime ); }

private:
    u16     m_wServerPort;      //看门狗服务器端口
    u8      m_byCheckCount;     //看门狗服务器断链检测次数
    u8      m_byReserved1;
    u16     m_wCheckTime;       //看门狗服务器断链检测时间
    u16     m_wReserved2;
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvWdSrv
;

//看门狗客户端配置
typedef struct tagFCSrvWdAgt//len: 8
{
public:
    tagFCSrvWdAgt() : m_dwCheckCount(0), m_dwRegTime(0) {}

    //置空
    inline void SetNull(void) { memset( this, 0, sizeof (tagFCSrvWdAgt) ); }

    //设置断链检测时间
    inline void SetCheckCount(u32 dwCount) { m_dwCheckCount = htonl( dwCount ); }

    //获取断链检测时间
    inline u32 GetCheckCount(void) const { return ntohl( m_dwCheckCount ); }

    //设置注册时间
    inline void SetRegTime(u32 dwTime) { m_dwRegTime = htonl( dwTime ); }

    //获取注册时间
    inline u32 GetRegTime(void) const { return ntohl( m_dwRegTime ); }

private:
    u32     m_dwCheckCount; //看门狗客户端断链检测次数
    u32     m_dwRegTime;    //看门狗客户端注册时间间隔
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvWdAgt
;

typedef struct tagFCSrvWdConfig//len: 20
{
public:
    tagFCSrvWdConfig() : m_byNeedStart(0) {}

    //置空
    inline void SetNull(void)
    {
        m_byNeedStart = 0;
        m_tSrv.SetNull();
        m_tAgt.SetNull();
    }

    //设置是否需要启动
    inline void SetNeedStart(BOOL32 bNeedStart)
    { m_byNeedStart = bNeedStart ? 1 : 0; }

    //判断是否需要启动
    inline BOOL32 IsNeedStart(void) const
    { return ( m_byNeedStart == 1 ? TRUE : FALSE ); }

private:
    u8      m_byNeedStart;
    u8      m_abyReserved1[3];
public:
    IFCSrvWdSrv  m_tSrv;
    IFCSrvWdAgt  m_tAgt;
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvWdConfig
;

typedef struct tagFCSrvThreadConfig//len: 12
{
public:
    tagFCSrvThreadConfig() : m_dwSUCDBConnNum(htonl(SUS_DB_SUC_CONNNUM)),
                             m_dwSUSDBConnNum(htonl(SUS_DB_SUS_CONNNUM)),
                             m_dwMTDBConnNum(htonl(SUS_DB_MT_CONNNUM)) {}

    //置空
    inline void SetNull(void)
    {
        m_dwSUCDBConnNum = htonl( SUS_DB_SUC_CONNNUM );
        m_dwSUSDBConnNum = htonl( SUS_DB_SUS_CONNNUM );
        m_dwMTDBConnNum = htonl( SUS_DB_MT_CONNNUM );
    }

//    inline void SetSucDbNum(u32 dwNum) { m_dwSUCDBConnNum = htonl( dwNum ); }

    inline u32 GetSucDbNum(void) const { return ntohl( m_dwSUCDBConnNum ); }

//    inline void SetSusDbNum(u32 dwNum) { m_dwSUSDBConnNum = htonl( dwNum ); }

    inline u32 GetSusDbNum(void) const { return ntohl( m_dwSUSDBConnNum ); }

//    inline void SetMtDbNum(u32 dwNum) { m_dwMTDBConnNum = htonl( dwNum ); }

    inline u32 GetMtDbNum(void) const { return ntohl( m_dwMTDBConnNum ); }

private:
    u32     m_dwSUCDBConnNum;   //客户端数据库连接线程个数
    u32     m_dwSUSDBConnNum;   //服务器数据库连接线程个数
    u32     m_dwMTDBConnNum;    //终端数据库连接线程个数
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
IFCSrvThreadConfig
;

//服务器全体配置
typedef struct tagSusConfig//len: 8 + 184 + 328 + 20
{
public:
    tagSusConfig() {}

    //置空
    void SetNull(void)
    {
        m_tSrvCfg.SetNull();
        m_tDbConnCfg.SetNull();
        m_tFileCfg.SetNull();
        m_tWdCfg.SetNull();
        m_tThreadCfg.SetNull();
    }

public:
    IFCSrvConfig        m_tSrvCfg;      //服务器配置
    IFCSrvDBConfig      m_tDbConnCfg;   //数据库连接配置
    IFCSrvFileConfig    m_tFileCfg;     //文件配置
    IFCSrvWdConfig      m_tWdCfg;       //看门狗配置
    IFCSrvThreadConfig  m_tThreadCfg;   //线程配置
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif//WIN32
ISusConfig
;

/*------------------------------------------------------------------------------
  服务器消息结构
------------------------------------------------------------------------------*/
//消息发送结构(千万不要清空，否则找不到版本号)
typedef struct tagFCFileSrvMsg//len: 28 * 1024
{
public:
    tagFCFileSrvMsg(const u8 *pbyMsgBody = NULL, u16 wMsgBodyLen = 0)
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
    inline void SetSrcVer(void) { m_dwSrcVer = htonl( FILESRV_CUR_VERSION ); }

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
    inline void SetMsgBody(const u8 *pbyMsgBody = NULL, u16 wMsgBodyLen = 0)
    {
        SetSrcVer();
        m_wMsgLen = 0;
        memset( m_abyMsgBody, 0, sizeof (m_abyMsgBody) );
        if ( pbyMsgBody != NULL && wMsgBodyLen > 0 )
        {
            m_wMsgLen = min( wMsgBodyLen, static_cast<u16>(MAX_FILESRV_MSG_LEN) );
            memcpy( m_abyMsgBody, pbyMsgBody, m_wMsgLen );
            m_wMsgLen = htons( m_wMsgLen );
        }
    }

    //在已设置的消息体尾部拼接消息体
    inline void CatMsgBody(const u8 *pbyMsgBody = NULL, u16 wMsgBodyLen = 0)
    {
        if ( pbyMsgBody != NULL && wMsgBodyLen > 0 )
        {
            u16 wCurLen = ntohs( m_wMsgLen );//当前已存入的消息体长度
            //本次追加的消息体长度
            u16 wThisLen = min( wMsgBodyLen,
                                static_cast<u16>(MAX_FILESRV_MSG_LEN - wCurLen) );
            memcpy( m_abyMsgBody + wCurLen, pbyMsgBody, wThisLen );
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
IFCFileSrvMsg
;


#ifdef WIN32
#pragma pack( pop )
#endif//WIN32


#endif//_FILESRVCLIENTSTRUCT_H
