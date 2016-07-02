/*=============================================================================
模   块   名: USB-KEY操作
文   件   名: UsbVerify.h
相 关  文 件: UsbVerify.cpp
文件实现功能: USB验证
作        者: 王昊
版        本: V4.0  Copyright(C) 2006-2006 Kedacom, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2006/10/11  1.0     王昊        创建
=============================================================================*/
// UsbVerify.h: interface for the CUsbVerify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBVERIFY_H__2C5C3A4E_F8EB_4189_BEEC_3D7054493825__INCLUDED_)
#define AFX_USBVERIFY_H__2C5C3A4E_F8EB_4189_BEEC_3D7054493825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UsbKey.h"


//验证, 请作为全局变量
class CUsbVerify : public CUsbKey
{
public:
    CUsbVerify();
    virtual ~CUsbVerify();

public:
    /*=============================================================================
    函 数 名:Verify
    功    能:验证模块授权信息
    参    数:EMUsbRight emUsbRight              [in]    模块
    注    意:DEBUG下不检验授权
    返 回 值:授权正确: TRUE; 无授权或授权错误: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/11  1.0     王昊    创建
    =============================================================================*/
    BOOL32 Verify(EMUsbRight emUsbRight);

	/*=============================================================================
    函 数 名:VerifyPCMT
    功    能:验证模块授权PCMT
    参    数:
    注    意:
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/11  1.0     王昊    创建
    =============================================================================*/
    BOOL32 VerifyPCMT();
    /*=============================================================================
    函 数 名:SetDeviceCallBack
    功    能:设置设备状态回调函数
    参    数:IN TUsbVerifyCallBack pFuncOnCallBack  [in]    插入回调函数
             IN TUsbVerifyCallBack pFuncOffCallBack [in]    拔出回调函数
             IN void *pCBData                       [in]    回调数据
    注    意:错误码用GetErrorDescription解释，请在Verify之后调用
    返 回 值:设置成功: 0; 设置失败: 错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/11  1.0     王昊    创建
    =============================================================================*/
    u32 SetDeviceCallBack(IN TUsbVerifyCallBack pFuncOnCallBack,
                          IN TUsbVerifyCallBack pFuncOffCallBack,
                          IN void *pCBData);

private:
    /*=============================================================================
    函 数 名:VerifyData
    功    能:验证数据
    参    数:IN LPSTR lptrVersion                   [in]    版本号
             IN EMUsbRight emRight                  [in]    模块
             IN const u8 *pbyBuf                    [in]    数据内容
             IN s32 nBufLen                         [in]    数据长度
    注    意:调用前请打开读卡器, 并独占设备; 调用后, 请解除独占, 并关闭设备
    返 回 值:授权正确: TRUE; 授权错误: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/12  1.0     王昊    创建
    =============================================================================*/
    BOOL32 VerifyData(IN LPSTR lptrVersion, IN EMUsbRight emRight,
                      IN const u8 *pbyBuf, IN s32 nBufLen);

    /*=============================================================================
    函 数 名:VerifyVersion40R31
    功    能:验证4.0 R3 第一版数据
    参    数:IN EMUsbRight emRight                  [in]    模块
             IN const u8 *pbyBuf                    [in]    数据内容
             IN s32 nBufLen                         [in]    数据长度
    注    意:调用前请打开读卡器, 并独占设备; 调用后, 请解除独占, 并关闭设备
    返 回 值:授权正确: TRUE; 授权错误: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/10/12  1.0     王昊    创建
    =============================================================================*/
    BOOL32 VerifyVersion40R31(IN EMUsbRight emRight, IN const u8 *pbyBuf,
                              IN s32 nBufLen);

  /*=============================================================================
    函 数 名:ReadVersion
    功    能:读取当前验证的类型
    参    数:IN HKHANDLE hCard						[in]    模块
             IN LPSTR lptrVersionf                  [in]    数据内容
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/08/18  1.0     王雄    创建
    =============================================================================*/
	u32 ReadVersion(IN HKHANDLE hCard, OUT LPSTR lptrVersion);
protected:
    s8      m_szCurReader[MAXLEN_USBKEY_READER_NAME];
};

#endif // !defined(AFX_USBVERIFY_H__2C5C3A4E_F8EB_4189_BEEC_3D7054493825__INCLUDED_)
