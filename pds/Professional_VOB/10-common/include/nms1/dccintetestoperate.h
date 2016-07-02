/*****************************************************************************
   模块名      : DCCInteTestOperate
   文件名      : DCCInteTestOperate.h
   相关文件    : DCCInteTestOperate.cpp
   文件实现功能: DCConsole集成测试操作类定义
   作者		   : 严晓治
   版本        : V1.0
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2005/01/25  1.0      严晓治      创建
******************************************************************************/

#if !defined(AFX_DCCINTETESTOPERATE_H__61AEC7E5_BC88_48DC_BF1A_BB0C37BE6007__INCLUDED_)
#define AFX_DCCINTETESTOPERATE_H__61AEC7E5_BC88_48DC_BF1A_BB0C37BE6007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CppUnit\GuiUnit.h>
#include "kdvtype.h"
#include "dcstrc.h"

class CDCCInteTestOperate  
{
public:
	CDCCInteTestOperate();
	virtual ~CDCCInteTestOperate();

/*====================================================================
    功能        ：  自动启动数据会议控制台并初始化
    输入参数说明：  lpszDCCHtmlFile－－DCConsole.htm文件的路径
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL StartDCCAndIni(LPCSTR lpszDCCHtmlFile);

/*====================================================================
    功能        ：  关闭数据会议控制台
    输入参数说明：  无
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/   
	BOOL StopDCC();

/*====================================================================
    功能        ：  手动启动控制台后初始化各窗口的句柄
    输入参数说明：  strWndTitle－－控制台的窗口title
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/ 
    BOOL Ini_AllStatic_Handle(CString strWndTitle);            
    
    //////////////////////////////////////////////////////////////////////////
    //以下为操作
    
/*====================================================================
    功能        ：  用户登录
    输入参数说明：  dwSvrIp－－服务器地址
                    tUser－－登录的用户信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL UserLogOn(DWORD dwSvrIp, TDCUser tUser);

/*====================================================================
    功能        ：  用户注销
    输入参数说明：  无
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL UserLogOut();

/*====================================================================
    功能        ：  增加终端条目
    输入参数说明：  tMt－－增加的终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL AddMtEntry(TDCBaseMt tMt);

/*====================================================================
    功能        ：  从模板界面增加终端条目
    输入参数说明：  tMt－－增加的终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL AddMtEntryFromTemplate(TDCBaseMt tMt);

/*====================================================================
    功能        ：  修改终端条目
    输入参数说明：  tMt－－修改的终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL ModifyMtEntry(TDCBaseMt tMt);

/*====================================================================
    功能        ：  删除终端条目
    输入参数说明：  lpszMtAliast－－被删除终端的别名
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
	BOOL DelMtEntry(LPCSTR lpszMtAlias);
    
/*====================================================================
    功能        ：  删除所有终端条目
    输入参数说明：  无
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL DelAllMtEntry();

/*====================================================================
    功能        ：  增加会议模板
    输入参数说明：  tConfEntry－－增加的模板信息
                    bAddAllMt－－是否将地址簿中的所有终端加入模板
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL AddConfEntry(TDataConfTemplate tConfEntry, BOOL bAddAllMt=FALSE);

/*====================================================================
    功能        ：  修改会议模板
    输入参数说明：  tConfEntry－－修改的模板信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL ModifyConfEntry(TDataConfTemplate tConfEntry);

/*====================================================================
    功能        ：  删除会议模板
    输入参数说明：  lpszTemplateName－－被删除模板的名称
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL DelConfEntry(LPCSTR lpszTemplateName);
    
/*====================================================================
    功能        ：  删除所有会议模板
    输入参数说明：  无
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL DelAllConfEntry();

/*====================================================================
    功能        ：  增加用户
    输入参数说明：  tUser－－增加的用户信息
                    lpszConfirmPasswd－－输入的确认密码
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL AddDCUser(TDCUser tUser, LPCSTR lpszConfirmPasswd="");

/*====================================================================
    功能        ：  修改用户
    输入参数说明：  tUser－－修改的用户信息
                    lpszConfirmPasswd－－输入的确认密码
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL ModifyDCUser(TDCUser tUser, LPCSTR lpszConfirmPasswd="");

/*====================================================================
    功能        ：  删除用户
    输入参数说明：  lpszUserName－－被删除的用户别名
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL DelDCUser(LPCSTR lpszUserName);

/*====================================================================
    功能        ：  删除所有用户，Admin除外
    输入参数说明：  
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL DelAllDCUser();

/*====================================================================
    功能        ：  手动输入会议信息，创建会议
    输入参数说明：  tConfInfo－－会议信息
                    bAddAllMt－－是否将地址簿中所有终端都加入会议
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL CreateConfDirect(TDataConfFullInfo tConfInfo, BOOL bAddAllMt=FALSE);
    
/*====================================================================
    功能        ：  选择模板，创建会议
    输入参数说明：  lpszTemplateName－－模板名
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL CreateConfByTemplate(LPCSTR lpszTemplateName);

/*====================================================================
    功能        ：  结束会议
    输入参数说明：  lpszConfName－－会议名
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL TermConf(LPCSTR lpszConfName);

/*====================================================================
    功能        ：  邀请会议中的终端
    输入参数说明：  lpszConfName－－会议名
                    tMt－－终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL InviteMtInConf(LPCSTR lpszConfName, TDCMt tMt);

/*====================================================================
    功能        ：  邀请不在会议中的终端
    输入参数说明：  lpszConfName－－会议名
                    tMt－－终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL InviteMtOutConf(LPCSTR lpszConfName, TDCMt tMt);

/*====================================================================
    功能        ：  挂断终端
    输入参数说明：  lpszConfName－－会议名
                    tMt－－终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL EjectMt(LPCSTR lpszConfName, TDCMt tMt);

/*====================================================================
    功能        ：  删除终端
    输入参数说明：  lpszConfName－－会议名
                    tMt－－终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL RemoveMt(LPCSTR lpszConfName, TDCMt tMt);

/*====================================================================
    功能        ：  指定主席
    输入参数说明：  lpszConfName－－会议名
                    tMt－－主席终端信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL SpecChairman(LPCSTR lpszConfName, TDCMt tMt);
    
/*====================================================================
    功能        ：  取消主席
    输入参数说明：  lpszConfName－－会议名
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL CancelChairman(LPCSTR lpszConfName);

/*====================================================================
    功能        ：  结束所有会议
    输入参数说明：  无
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL TermAllConf();

/*====================================================================
    功能        ：  加入远端会议
    输入参数说明：  tDCSConfInfo－－远端会议信息
    返回值说明  ：  true    －－成功
                    false   －－失败
====================================================================*/
    BOOL JoinRemoteConf(TDCSConfInfo tDCSConfInfo);
    
protected:

    CGuiUnit m_GuiUnit;

    HWND m_hOutLookBar;             //导航栏的句柄
    //导航栏中控件的句柄
    HWND m_hOutLookBar_ConfMng;     
    HWND m_hOutLookBar_Template;    
    HWND m_hOutLookBar_AddrBook;    
    HWND m_hOutLookBar_SysLog;      
    
    HWND m_hMain;           //主窗口句柄
    HWND m_hMenu_Sys;       //菜单中的系统按钮句柄
    HWND m_hMenu_ConfMng;   //菜单中的会议管理按钮句柄
    HWND m_hMenu_FunMng;    //菜单中的功能管理按钮句柄
    HWND m_hMenu_Help;      //菜单中的帮助按钮句柄

    HWND m_hAddrBookMngDlg;     //地址簿对话框的句柄
    //地址簿对话框中控件的句柄
    HWND m_hAddrBookMngDlg_List_Mt;
    HWND m_hAddrBookMngDlg_Btn_Add;
    HWND m_hAddrBookMngDlg_Btn_Modify;
    HWND m_hAddrBookMngDlg_Btn_Del;

    HWND m_hPopupAddrBookCfgDlg;    //弹出的终端设置对话框
    //弹出的终端设置对话框中的控件句柄
    HWND m_hPopupAddrBookCfgDlg_Edit_Alias;
    HWND m_hPopupAddrBookCfgDlg_Combo_CallType;
    HWND m_hPopupAddrBookCfgDlg_IpAddr_MtIp;
    HWND m_hPopupAddrBookCfgDlg_Edit_MtPort;
    HWND m_hPopupAddrBookCfgDlg_Edit_MtE164;
    HWND m_hPopupAddrBookCfgDlg_Edit_MtRemark;
    HWND m_hPopupAddrBookCfgDlg_Btn_OK;
    HWND m_hPopupAddrBookCfgDlg_Btn_Cancel;

    HWND m_hTemplateMngDlg;     //会议模板对话框的句柄
    //会议模板对话框中控件的句柄
    HWND m_hTemplateMngDlg_List_Template;
    HWND m_hTemplateMngDlg_Btn_Add;
    HWND m_hTemplateMngDlg_Btn_Modify;
    HWND m_hTemplateMngDlg_Btn_Del;

    HWND m_hPopupTemplateCfgDlg;    //弹出的会议模板设置对话框
    //弹出的会议模板设置对话框中的控件句柄
    HWND m_hPopupTemplateCfgDlg_Edit_Name;
    HWND m_hPopupTemplateCfgDlg_Edit_BandWidth;
    HWND m_hPopupTemplateCfgDlg_Edit_PassWd;
    HWND m_hPopupTemplateCfgDlg_List_MtInConf;
    HWND m_hPopupTemplateCfgDlg_List_MtOutConf;
    HWND m_hPopupTemplateCfgDlg_List_Chairman;
    HWND m_hPopupTemplateCfgDlg_Check_H323;
    HWND m_hPopupTemplateCfgDlg_Check_PassWd;
    HWND m_hPopupTemplateCfgDlg_Check_WhiteBoard;
    HWND m_hPopupTemplateCfgDlg_Check_AppShare;
    HWND m_hPopupTemplateCfgDlg_Check_Chat;
    HWND m_hPopupTemplateCfgDlg_Check_FileTrans;
    HWND m_hPopupTemplateCfgDlg_Check_MtCallIn;
    HWND m_hPopupTemplateCfgDlg_Check_AutoReInvite;
    HWND m_hPopupTemplateCfgDlg_Check_Chairman;
    HWND m_hPopupTemplateCfgDlg_Btn_OK;
    HWND m_hPopupTemplateCfgDlg_Btn_Cancel;
    HWND m_hPopupTemplateCfgDlg_Btn_AddAllMtToConf;
    HWND m_hPopupTemplateCfgDlg_Btn_AddOneMtToConf;
    HWND m_hPopupTemplateCfgDlg_Btn_DelAllMtFromConf;
    HWND m_hPopupTemplateCfgDlg_Btn_DelOneMtFromConf;
    HWND m_hPopupTemplateCfgDlg_Btn_SpecChairman;
    HWND m_hPopupTemplateCfgDlg_Btn_CancelChairman;
    HWND m_hPopupTemplateCfgDlg_Btn_AddMt;

    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg;    //弹出的会议模板设置对话框中弹出的新增终端对话框
    //弹出的会议模板设置对话框中弹出的新增终端对话框中的控件句柄
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_Edit_Alias;
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_Combo_CallType;
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_IpAddr_MtIp;
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_Edit_MtPort;
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_Edit_MtE164;
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_Edit_MtRemark;
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_Btn_OK;
    HWND m_hPopupTemplateCfgDlg_PopupAddMtDlg_Btn_Cancel;

    HWND m_hConfMngDlg;     //会议管理对话框的句柄
    //会议管理对话框中控件的句柄
    HWND m_hConfMngDlg_List_Chairman;
    HWND m_hConfMngDlg_List_Mt;
    HWND m_hConfMngDlg_List_Conf;
    HWND m_hConfMngDlg_Btn_SpecChairman;
    HWND m_hConfMngDlg_Btn_CancelChairman;
    HWND m_hConfMngDlg_Btn_InviteMt;
    HWND m_hConfMngDlg_Btn_EjectMt;
    HWND m_hConfMngDlg_Btn_RemoveMt;
    HWND m_hConfMngDlg_Btn_CreateConf;
    HWND m_hConfMngDlg_Btn_TermConf;

    HWND m_hConfMngDlg_PopupCreateConfDlg;     //会议管理对话框中弹出的创建会议对话框的句柄
    //会议管理对话框中弹出的创建会议对话框中的控件的句柄
    HWND m_hConfMngDlg_PopupCreateConfDlg_Edit_Name;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Edit_BandWidth;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Edit_PassWd;
    HWND m_hConfMngDlg_PopupCreateConfDlg_List_Template;
    HWND m_hConfMngDlg_PopupCreateConfDlg_List_MtInConf;
    HWND m_hConfMngDlg_PopupCreateConfDlg_List_MtOutConf;
    HWND m_hConfMngDlg_PopupCreateConfDlg_List_Chairman;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_H323;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_PassWd;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_WhiteBoard;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_AppShare;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_Chat;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_FileTrans;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_MtCallIn;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_AutoReInvite;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Check_Chairman;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_OK;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_Cancel;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_AddAllMtToConf;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_AddOneMtToConf;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_DelAllMtFromConf;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_DelOneMtFromConf;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_SpecChairman;
    HWND m_hConfMngDlg_PopupCreateConfDlg_Btn_CancelChairman;

    HWND m_hConfMngDlg_PopupInviteMtDlg;     //会议管理对话框中弹出的邀请终端对话框的句柄
    //会议管理对话框中弹出的邀请终端对话框中的控件的句柄
    HWND m_hConfMngDlg_PopupInviteMtDlg_List_MtOutConf;
    HWND m_hConfMngDlg_PopupInviteMtDlg_Btn_OK;
    HWND m_hConfMngDlg_PopupInviteMtDlg_Btn_Cancel;

    HWND m_hPopupUserMngDlg;     //弹出的用户管理对话框的句柄
    //弹出的用户管理对话框中的控件的句柄
    HWND m_hPopupUserMngDlg_List_User;
    HWND m_hPopupUserMngDlg_Btn_Add;
    HWND m_hPopupUserMngDlg_Btn_Modify;
    HWND m_hPopupUserMngDlg_Btn_Del;
    HWND m_hPopupUserMngDlg_Btn_Cancel;

    HWND m_hPopupUserCfgDlg;     //弹出的用户设置对话框的句柄
    //弹出的用户设置对话框中的控件的句柄
    HWND m_hPopupUserCfgDlg_Edit_Name;
    HWND m_hPopupUserCfgDlg_Edit_Passwd;
    HWND m_hPopupUserCfgDlg_Edit_ConfirmPasswd;
    HWND m_hPopupUserCfgDlg_Combo_Level;
    HWND m_hPopupUserCfgDlg_Btn_OK;
    HWND m_hPopupUserCfgDlg_Btn_Cancel;

    HWND m_hPopupLogInDlg;     //弹出的登录对话框的句柄
    //弹出的登录对话框中的控件的句柄
    HWND m_hPopupLogInDlg_IpAddr_DCSIp;
    HWND m_hPopupLogInDlg_Edit_UserName;
    HWND m_hPopupLogInDlg_Edit_UserPasswd;
    HWND m_hPopupLogInDlg_Btn_OK;
    HWND m_hPopupLogInDlg_Btn_Cancel;

    HWND m_hPopupGetRemoteConfDlg;     //弹出的查询远端会议对话框的句柄
    //弹出的查询远端会议对话框中的控件的句柄
    HWND m_hPopupGetRemoteConfDlgDlg_Combo_CallType;
    HWND m_hPopupGetRemoteConfDlgDlg_Edit_SvrE164;
    HWND m_hPopupGetRemoteConfDlgDlg_IpAddr_SvrIp;
    HWND m_hPopupGetRemoteConfDlgDlg_Edit_SvrPort;
    HWND m_hPopupGetRemoteConfDlgDlg_Check_H323;
    HWND m_hPopupGetRemoteConfDlgDlg_Btn_OK;
    HWND m_hPopupGetRemoteConfDlgDlg_Btn_Cancel;

    HWND m_hPopupJoinRemoteConfDlg;     //弹出的加入远端会议对话框的句柄
    //弹出的加入远端会议对话框中的控件的句柄
    HWND m_hPopupJoinRemoteConfDlgDlg_List_RemoteConf;
    HWND m_hPopupJoinRemoteConfDlgDlg_Edit_ConfPasswd;
    HWND m_hPopupJoinRemoteConfDlgDlg_Btn_OK;
    HWND m_hPopupJoinRemoteConfDlgDlg_Btn_Cancel;
    
    BOOL GuiActive();                           //窗口在最顶层显示
    
    BOOL CloseInfoWindow(HWND hParentWnd);      //关闭信息弹出窗口

    BOOL Ini_OutLookBar_Handle();               //初始化导航栏中控件的句柄
    
    BOOL Ini_Menu_Handle();                     //初始化菜单按钮的句柄
    
    BOOL Ini_AddrBookMng_Handle();              //初始化地址簿对话框中控件的句柄
    
    BOOL Ini_PopupAddrBookCfg_Handle();         //初始化弹出的地址簿对话框中控件的句柄
    
    BOOL Ini_TemplateMng_Handle();              //初始化会议模板对话框中控件的句柄
    
    BOOL Ini_PopupTemplateCfg_Handle();         //初始化弹出的会议模板对话框中控件的句柄
    
    BOOL Ini_PopupTemplateCfg_PopupAddMt_Handle(); //初始化弹出的增加终端对话框中控件的句柄
    
    BOOL Ini_ConfMng_Handle();                  //初始化会议管理对话框中控件的句柄
    
    BOOL Ini_PopupConfCreate_Handle();          //初始化弹出的创建会议对话框中控件的句柄
    
    BOOL Ini_PopupInviteMt_Handle();            //初始化弹出的邀请终端对话框中控件的句柄
    
    BOOL Ini_PopupUserMng_Handle();             //初始化弹出的用户管理对话框中控件的句柄
    
    BOOL Ini_PopupUserCfg_Handle();             //初始化弹出的用户设置对话框中控件的句柄
    
    BOOL Ini_PopupLogInDlg_Handle();            //初始化弹出的登录对话框中控件的句柄
    
    BOOL Ini_PopupGetRemoteConfDlg_Handle();    //查询远端会议对话框中控件的句柄
    
    BOOL Ini_PopupJoinRemoteConfDlg_Handle();   //加入远端会议对话框中控件的句柄
};

#endif // !defined(AFX_DCCINTETESTOPERATE_H__61AEC7E5_BC88_48DC_BF1A_BB0C37BE6007__INCLUDED_)
