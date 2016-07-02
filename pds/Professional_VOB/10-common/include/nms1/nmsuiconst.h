/*=============================================================================
模   块   名: 网管服务器界面操作库
文   件   名: nmsuiconst.h
相 关  文 件: 无
文件实现功能: 界面操作宏定义
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/08/17  4.0     王昊        创建
=============================================================================*/

#ifndef _NMSUICONST_20050817_H
#define _NMSUICONST_20050817_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NMS_EXE_NAME        _T("NmsServer.exe")     //  NMS进程名

//以下资源ID取自网管服务器resource.h中的id
//  主界面资源ID---------------------------------------------------------------
#define IDD_MAIN                        102         //  主对话框
#define IDC_BTN_START                   1000        //  开始按钮
#define IDC_BTN_STOP                    1001        //  停止按钮
#define IDC_BTN_CONFIG                  1002        //  配置按钮


//  配置界面资源ID-------------------------------------------------------------
#define IDC_IP_SVR                      1012        //  服务器IP地址
#define IDC_EDIT_TRAPPORT               1013        //  Trap端口
#define IDC_EDIT_CMPERIOD               1014        //  配置轮询间隔
#define IDC_EDIT_FMPERIOD               1015        //  告警轮询间隔
#define IDC_EDIT_TIMEOUT                1016        //  超时时间
#define IDC_IP_DB                       1017        //  数据库IP地址
#define IDC_EDIT_DBUSER                 1019        //  数据库用户名
#define IDC_EDIT_DBPWD                  1020        //  数据库密码
#define IDC_EDIT_FTPREMOTEUSER          1021        //  FTP远端用户名
#define IDC_EDIT_FTPREMOTEPWD           1022        //  FTP远端密码
#define IDC_EDIT_FTPLOCALUSER           1023        //  FTP本地用户名
#define IDC_EDIT_FTPLOCALPWD            1024        //  FTP本地密码
#define IDC_EDIT_FTPDIR                 1025        //  FTP本地目录
#define IDC_EDIT_PRT2SCREEN             1026        //  打印到屏幕级别
#define IDC_EDIT_PRT2FILE               1027        //  打印到文件级别
#define IDC_BTN_SAVE                    1030        //  保存按钮

#endif  //  _NMSUICONST_20050817_H