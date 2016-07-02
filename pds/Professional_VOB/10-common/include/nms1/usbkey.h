/*=============================================================================
模   块   名: USB-KEY操作
文   件   名: UsbKey.h
相 关  文 件: UsbKey.cpp
文件实现功能: USB基本操作
作        者: 王昊
版        本: V4.0  Copyright(C) 2006-2006 Kedacom, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2006/10/11  1.0     王昊        创建
=============================================================================*/

// UsbKey.h: interface for the CUsbKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBKEY_H__07466CA1_8127_40AA_A02F_7C86D612ADE0__INCLUDED_)
#define AFX_USBKEY_H__07466CA1_8127_40AA_A02F_7C86D612ADE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kdvtype.h"
#include <wtypes.h>
#include "hkapi.h"


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

//授权模块信息
enum EMUsbRight
{
    enumUsbRightMCS     = 1,    //MCS
    enumUsbRightPCMT    = 2,    //PCMT
    enumUsbRightNMS     = 3,    //NMS
    enumUsbRightVOS     = 4     //Recorder、VOD
};

//模块插入/拔出的回调函数
typedef u32 (__stdcall* TUsbVerifyCallBack)(IN void *pCBData);

//USBKEY 设备信息定义
#define VERSION_40_R3_1                 (LPCSTR)"kdv40.30.01.01.20061013"

#define CURRENT_VERSION                 VERSION_40_R3_1


#define MAXLEN_USBKEY_READER_NAME       (s32)128                //USBKEY读卡器名长度
#define MAXLEN_USBKEY_READER_NUM        (s32)12                 //USBKEY读卡器最大个数
//所有USBKEY设备名最大长度, 含'\0'
#define MAXLEN_USBKEY_NAME              MAXLEN_USBKEY_READER_NAME * MAXLEN_USBKEY_READER_NUM
#define DEFAULT_USBKEY_SO_PIN           (s8*)"1111"             //默认管理员密码
#define DEFAULT_USBKEY_USER_PIN         (s8*)"1111"             //默认用户密码
#define DEFAULT_USBKEY_TAG_PREFIX       (LPCSTR)"kdv40.30.01.01.20061013"//USBKEY设备标签名前缀
#define MAXLEN_USBKEY_DEVICE_TAG        (u32)128                //最大USBKEY设备标签名长度
#define DEFAULT_USBKEY_DIRID            (u32)0xDF02             //USBKEY设备缺省的授权信息目录ID
//#define USBKEY_VERSION_DIRID            (u32)0xDF21
#define USBKEY_VERSION_FILEID           (u32)0xEF21
//#define USBKEY_MCS_DIRID                (u32)0xDF22
#define USBKEY_MCS_FILEID               (u32)0xEF22
//#define USBKEY_PCMT_DIRID               (u32)0xDF23
#define USBKEY_PCMT_FILEID              (u32)0xEF23
//#define USBKEY_NMS_DIRID                (u32)0xDF24
#define USBKEY_NMS_FILEID               (u32)0xEF24
//#define USBKEY_VOS_DIRID                (u32)0xDF25
#define USBKEY_VOS_FILEID               (u32)0xEF25
#define DEFAULT_USBKEY_FILESIZE         (u32)512                //USBKEY设备缺省的授权信息文件空间尺寸(按字节计算)
#define DEFAULT_USBKEY_AES_KEY          (s8*)"kedacom.shanghai" //USBKEY设备缺省的授权信息AES密钥
#define DEFAULT_USBKEY_AES_INITKEY      (s8*)"2003091919190900" //USBKEY设备缺省的授权信息AES初始KEY
#define DEFAULT_USBKEY_AES_MODE         MODE_CBC                //USBKEY设备缺省的授权信息AES模式


//保存信息
typedef struct tagUsbSaveInfo
{
public:
    tagUsbSaveInfo()
    {
        memset( this, 0, sizeof (tagUsbSaveInfo) );
    }

public:
    EMUsbRight      m_emUsbRight;       //所属模块
} TUsbSaveInfo;

typedef struct tagUsbMcsSaveInfo : TUsbSaveInfo
{
public:
    tagUsbMcsSaveInfo() : m_bEnable(FALSE)
    {
        m_emUsbRight = enumUsbRightMCS;
    }

public:
    BOOL32  m_bEnable;      //是否使能
} TUsbMcsSaveInfo;

typedef struct tagUsbPcmtSaveInfo : TUsbSaveInfo
{
public:
    tagUsbPcmtSaveInfo() : m_bEnable(FALSE)
    {
        m_emUsbRight = enumUsbRightPCMT;
    }

public:
    BOOL32  m_bEnable;      //是否使能
} TUsbPcmtSaveInfo;

typedef struct tagUsbNmsSaveInfo : TUsbSaveInfo
{
public:
    tagUsbNmsSaveInfo() : m_bEnable(FALSE)
    {
        m_emUsbRight = enumUsbRightNMS;
    }

public:
    BOOL32  m_bEnable;      //是否使能
} TUsbNmsSaveInfo;

typedef struct tagUsbVosSaveInfo : TUsbSaveInfo
{
public:
    tagUsbVosSaveInfo() : m_bEnable(FALSE)
    {
        m_emUsbRight = enumUsbRightVOS;
    }

public:
    BOOL32  m_bEnable;      //是否使能
} TUsbVosSaveInfo;



//基类, 外部不要调用
class CUsbKey  
{
public:
	CUsbKey();
	virtual ~CUsbKey();

protected:
    /*=============================================================================
    函 数 名:EnumReaders
    功    能:枚举所有读卡器名
    参    数:OUT s8 aaszAllReaders[][]                  [out]   所有读卡器名, 用0x00分隔
             OUT s32 &nReaderNum                        [out]   读卡器个数
    注    意:错误码用GetErrorDescription解释
    返 回 值:读取成功: 0; 读取失败: 错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/11  1.0     王昊    创建
    =============================================================================*/
    u32 EnumReaders(OUT s8 aaszAllReaders[MAXLEN_USBKEY_READER_NUM][MAXLEN_USBKEY_READER_NAME],
                    OUT s32 &nReaderNum);

    /*=============================================================================
    函 数 名:SaveData
    功    能:保存数据
    参    数:IN HKHANDLE hCard                      [in]    读卡器句柄
             IN EMUsbRight emRight                  [in]    模块
             IN const u8 *pbyBuf                    [in]    保存数据
             IN s32 nBufLen                         [in]    数据长度
    注    意:错误码用GetErrorDescription解释; 调用前请打开读卡器, 并独占设备
             调用后, 请解除独占, 并关闭设备
    返 回 值:保存成功: 0; 保存失败: 错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/12  1.0     王昊    创建
    =============================================================================*/
    u32 SaveData(IN HKHANDLE hCard, IN EMUsbRight emRight,
                 IN const u8 *pbyBuf, IN s32 nBufLen);

    /*=============================================================================
    函 数 名:ReadData
    功    能:读取数据
    参    数:IN HKHANDLE hCard                      [in]    读卡器句柄
             IN EMUsbRight emRight                  [in]    模块
             OUT const u8 *pbyBuf                   [out]   数据
             IN OUT s32 nBufLen                     [in]    数据最大长度
    注    意:错误码用GetErrorDescription解释; 调用前请打开读卡器, 并独占设备
             调用后, 请解除独占, 并关闭设备
    返 回 值:读取成功: 0; 读取失败: 错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/12  1.0     王昊    创建
    =============================================================================*/
    u32 ReadData(IN HKHANDLE hCard, IN EMUsbRight emRight,
                 OUT u8 *pbyBuf, IN OUT s32 nBufLen);

    /*=============================================================================
    函 数 名:SaveVersion
    功    能:保存版本号
    参    数:IN HKHANDLE hCard                      [in]    读卡器句柄
    注    意:错误码用GetErrorDescription解释; 调用前请打开读卡器, 并独占设备
             调用后, 请解除独占, 并关闭设备
    返 回 值:保存成功: 0; 保存失败: 错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/12  1.0     王昊    创建
    =============================================================================*/
    u32 SaveVersion(IN HKHANDLE hCard);

    /*=============================================================================
    函 数 名:ReadVersion
    功    能:读取版本号
    参    数:IN HKHANDLE hCard                      [in]    读卡器句柄
             OUT LPSTR lptrVersion                  [out]   版本号
    注    意:错误码用GetErrorDescription解释; 调用前请打开读卡器, 并独占设备
             调用后, 请解除独占, 并关闭设备
    返 回 值:读取成功: 0; 读取失败: 错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/12  1.0     王昊    创建
    =============================================================================*/
    u32 ReadVersion(IN HKHANDLE hCard, OUT LPSTR lptrVersion);

public:
    /*=============================================================================
    函 数 名:GetWorkingReaders
    功    能:获取当前所有可用读卡器
    参    数:OUT s8 aaszReaders[][]                         [out]   当前所有可用读卡器名
             OUT s32 &nReaderNum                            [out]   可用读卡器个数
    注    意:错误码用GetErrorDescription解释
    返 回 值:读取成功: 0; 读取失败: 错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/11  1.0     王昊    创建
    =============================================================================*/
    u32 GetWorkingReaders(OUT s8 aaszReaders[MAXLEN_USBKEY_READER_NUM][MAXLEN_USBKEY_READER_NAME],
                          OUT s32 &nReaderNum);

    /*=============================================================================
    函 数 名:GetErrorDescription
    功    能:获取错误描述
    参    数:IN TUsbVerifyCallBack pFuncCallBack    [in]    回调函数
             IN u8 byLanguage                       [in]    语言类型(0: 中文; 1: 英文)
    注    意:现在只有中文
    返 回 值:错误描述
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/11  1.0     王昊    创建
    =============================================================================*/
    static LPCTSTR GetErrorDescription(IN u32 dwErrCode, IN u8 byLanguage = 0);
};

#endif // !defined(AFX_USBKEY_H__07466CA1_8127_40AA_A02F_7C86D612ADE0__INCLUDED_)
