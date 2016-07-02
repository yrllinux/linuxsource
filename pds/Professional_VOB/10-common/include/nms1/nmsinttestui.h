/*=============================================================================
模   块   名: 网管服务器界面操作库
文   件   名: nmsinttestui.h
相 关  文 件: nmsinttestui.cpp
文件实现功能: 界面操作类
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/07/26  4.0     王昊        创建
=============================================================================*/

#ifndef _NMSINTTESTUI_20050726_H_
#define _NMSINTTESTUI_20050726_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuiUnit.h"
#include "nmsmacro.h"
#include "nmsuiconst.h"

//  nms配置参数，如果参数为0则不填
typedef struct tagNmsGuiCfg
{
    tagNmsGuiCfg()
    {
        ZeroMemory(this, sizeof (tagNmsGuiCfg));
    }

    TCHAR   m_aszServerIpAddr[16];          //  nms IP地址
    u16     m_wTrapPort;                    //  nms trap端口
    u16     m_wCmSyncPeriod;                //  配置轮询间隔
    u16     m_wFmSyncPeriod;                //  告警轮询间隔
    u8      m_wSnmpTimeout;                 //  snmp超时时间
    TCHAR   m_aszDBIpAddr[16];              //  数据库服务器IP地址
    TCHAR   m_aszDBUser[USER_LEN];          //  数据库用户名
    TCHAR   m_aszDBPwd[PWD_LEN];            //  数据库密码
    TCHAR   m_aszFTPRemoteUser[USER_LEN];   //  FTP远端用户名
    TCHAR   m_aszFTPRemotePwd[PWD_LEN];     //  FTP远端密码
    TCHAR   m_aszFTPLocalUser[USER_LEN];    //  FTP本地用户名
    TCHAR   m_aszFTPLocalPwd[PWD_LEN];      //  FTP本地密码
    TCHAR   m_aszFTPLocalPath[MAX_PATH];    //  FTP本地目录
    u8      m_byPrint2ScreenLevel;          //  打印到屏幕级别
    u8      m_byPrint2FileLevel;            //  打印到文件级别
} TNmsGuiCfg;

class CNmsIntTestUI
{
public:
    CNmsIntTestUI();
    virtual ~CNmsIntTestUI() {}

public:
    /*=============================================================================
    函 数 名:GetNms
    功    能:获取NMS进程
    参    数:LPCTSTR pszProcessName            [in]    进程名
    注    意:需要手工启动NMS
    返 回 值:TRUE:成功; FALSE:失败
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/17  4.0     王昊    创建
    =============================================================================*/
    virtual BOOL32 GetNms(LPCTSTR pszProcessName = NMS_EXE_NAME);

    /*=============================================================================
    函 数 名:StartNms
    功    能:开启nms
    参    数:无
    注    意:Nms界面已经最大化
    返 回 值:TRUE:操作成功; FALSE:操作失败
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/17  4.0     王昊    创建
    =============================================================================*/
    virtual BOOL32 StartNms(void);

    /*=============================================================================
    函 数 名:StopNms
    功    能:停止nms
    参    数:无
    注    意:Nms界面已经最大化
    返 回 值:TRUE:操作成功; FALSE:操作失败
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/17  4.0     王昊    创建
    =============================================================================*/
    virtual BOOL32 StopNms(void);

    /*=============================================================================
    函 数 名:CfgNms
    功    能:配置nms
    参    数:const TNmsGuiCfg &tNmsCfg          [in]    nms配置参数
    注    意:Nms界面已经最大化
    返 回 值:TRUE:操作成功; FALSE:操作失败
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/17  4.0     王昊    创建
    =============================================================================*/
    virtual BOOL32 CfgNms(const TNmsGuiCfg &tNmsCfg);

protected:
    CGuiUnit    m_cGuiUnit;             //界面驱动库接口

    HWND        m_hStartBtn;            //开始按钮句柄
    HWND        m_hStopBtn;             //停止按钮句柄
    HWND        m_hConfigBtn;           //配置按钮句柄
};

#endif  //  _NMSINTTESTUI_20050726_H_