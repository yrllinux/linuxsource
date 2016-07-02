/*****************************************************************************
   模块名      : DCCInteTestQuery
   文件名      : DCCInteTestQuery.h
   相关文件    : DCCInteTestQuery.cpp
   文件实现功能: DCConsole集成测试查询类定义
   作者		   : 严晓治
   版本        : V1.0
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2005/01/25  1.0      严晓治      创建
******************************************************************************/

#if !defined(AFX_CDCCInteTestQuery_H__50DF2EB4_4FD2_48D1_B016_C9F0315611FF__INCLUDED_)
#define AFX_CDCCInteTestQuery_H__50DF2EB4_4FD2_48D1_B016_C9F0315611FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CppUnit\GuiUnit.h>
#include "kdvtype.h"
#include "dcstrc.h"

class CDCCInteTestQuery  
{
public:
	CDCCInteTestQuery();
	virtual ~CDCCInteTestQuery();

/*====================================================================
    功能        ：  与DCC的测试实例建链
    输入参数说明：  dwIPAddr－－DCC的IP地址
                    wListenPort－－DCC测试实例的侦听端口
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL ConnectToDCCTestApp(DWORD dwIPAddr=inet_addr("127.0.0.1") , WORD wListenPort=2095);

/*====================================================================
    功能        ：  与DCC的测试实例断链
    输入参数说明：  无
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL DisconnectToDCCTestApp();

    //////////////////////////////////////////////////////////////////////////
    //初始化操作 

/*====================================================================
    功能        ：  初始化启动时创建的各窗口的句柄
    输入参数说明：  无
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL Ini_AllStatic_Handle(CString strWndTitle); 
    
    //////////////////////////////////////////////////////////////////////////
    //以下为操作

/*====================================================================
    功能        ：  查询用户是否已登录
    输入参数说明：  tUser－－登录的用户信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsUserLogOn(TDCUser tUser);

/*====================================================================
    功能        ：  查询用户是否存在
    输入参数说明：  tUser－－登录的用户信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsUserExist(TDCUser tUser);

/*====================================================================
    功能        ：  查询终端是否存在
    输入参数说明：  tMt－－终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsMtEntryExist(TDCBaseMt tMt);

/*====================================================================
    功能        ：  查询模板是否存在
    输入参数说明：  tTemplate－－模板信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsTemplateExist(TDataConfTemplate tTemplate);

/*====================================================================
    功能        ：  查询会议是否存在
    输入参数说明：  tConfInfo－－会议信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsConfExist(TDataConfFullInfo tConfInfo);

/*====================================================================
    功能        ：  查询终端是否在会议中
    输入参数说明：  tConfInfo－－会议信息
                    tMt－－终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsMtInConf(TDataConfFullInfo tConfInfo, TDCMt tMt);
    
/*====================================================================
    功能        ：  查询终端是否上线
    输入参数说明：  tConfInfo－－会议信息
                    tMt－－终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsMtOnline(TDataConfFullInfo tConfInfo, TDCMt tMt);

/*====================================================================
    功能        ：  查询主席是否存在，如果存在，则判断是否就是tMt
    输入参数说明：  tConfInfo－－会议信息
                    tMt－－主席终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsChairmanExist(TDataConfFullInfo tConfInfo, TDCMt tMt);

/*====================================================================
    功能        ：  查询远端会议是否存在
    输入参数说明：  tDCSConfInfo－－远端会议信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL IsRemoteConfExist(TDCSConfInfo tDCSConfInfo);

protected:

    DWORD m_dwDCCTestNodeId;        //DCC测试实例的nodeid

    CGuiUnit m_GuiUnit;

    HWND m_hOutLookBar;             //导航栏的句柄
    //导航栏中控件的句柄
    HWND m_hOutLookBar_ConfMng;     
    HWND m_hOutLookBar_Template;    
    HWND m_hOutLookBar_AddrBook;    
    HWND m_hOutLookBar_SysLog;  
    
    HWND m_hMain;               //主窗口句柄
    HWND m_hMenu_Sys;           //菜单中的系统按钮句柄
    HWND m_hMenu_ConfMng;       //菜单中的会议管理按钮句柄
    HWND m_hMenu_FunMng;        //菜单中的功能管理按钮句柄
    HWND m_hMenu_Help;         //菜单中的帮助按钮句柄
    
    HWND m_hAddrBookMngDlg;     //地址簿对话框的句柄
    //地址簿对话框中控件的句柄
    HWND m_hAddrBookMngDlg_List_Mt;

    HWND m_hTemplateMngDlg;     //会议模板对话框的句柄
    //会议模板对话框中控件的句柄
    HWND m_hTemplateMngDlg_List_Template;

    HWND m_hConfMngDlg;     //会议管理对话框的句柄
    //会议管理对话框中控件的句柄
    HWND m_hConfMngDlg_List_Chairman;
    HWND m_hConfMngDlg_List_Mt;
    HWND m_hConfMngDlg_List_Conf;

    HWND m_hPopupUserMngDlg;     //弹出的用户管理对话框的句柄
    //弹出的用户管理对话框中的控件的句柄
    HWND m_hPopupUserMngDlg_List_User;
    HWND m_hPopupUserMngDlg_Btn_Cancel;

    HWND m_hPopupLogInDlg;     //弹出的登录对话框的句柄
    //弹出的登录对话框中的控件的句柄
    HWND m_hPopupLogInDlg_IpAddr_DCSIp;
    HWND m_hPopupLogInDlg_Edit_UserName;
    HWND m_hPopupLogInDlg_Edit_UserPasswd;
    HWND m_hPopupLogInDlg_Btn_OK;
    HWND m_hPopupLogInDlg_Btn_Cancel;

    BOOL GuiActive();                   //窗口在最顶层显示

    BOOL Ini_OutLookBar_Handle();       //初始化导航栏中控件的句柄
    
    BOOL Ini_Menu_Handle();             //初始化菜单按钮的句柄
    
    BOOL Ini_AddrBookMng_Handle();      //初始化地址簿对话框中控件的句柄
    
    BOOL Ini_TemplateMng_Handle();      //初始化会议模板对话框中控件的句柄
    
    BOOL Ini_ConfMng_Handle();          //初始化会议管理对话框中控件的句柄
    
    BOOL Ini_PopupUserMng_Handle();     //初始化弹出的用户管理对话框中控件的句柄
    
    BOOL Ini_PopupLogInDlg_Handle();    //初始化弹出的登录对话框中控件的句柄
};

#endif // !defined(AFX_CDCCInteTestQuery_H__50DF2EB4_4FD2_48D1_B016_C9F0315611FF__INCLUDED_)
