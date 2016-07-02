/*****************************************************************************
   模块名      : Recorder Loader
   文件名      : recloader.cpp
   相关文件    : recloaderdlg.cpp
   文件实现功能: 录像机安装、卸载和启动
   作者        : 
   版本        : V4.0  Copyright(C) 2004-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/08/23  4.0         顾振华      创建
******************************************************************************/

// recloader.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "recloader.h"
#include "recloaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecloaderApp

BEGIN_MESSAGE_MAP(CRecloaderApp, CWinApp)
	//{{AFX_MSG_MAP(CRecloaderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecloaderApp construction

CRecloaderApp::CRecloaderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRecloaderApp object

CRecloaderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRecloaderApp initialization


/*====================================================================
    函数名	     ：InitInstance
	功能		 ：程序入口，负责解析命令行
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/23	4.0			顾振华        创建
====================================================================*/
BOOL CRecloaderApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CRecloaderDlg dlg;    

    // 分析命令行
    LPCTSTR lpszCmdLine = GetCommandLine();
    
    CString cstrCmdLine = lpszCmdLine;   
    cstrCmdLine.MakeLower();
    
    int nStart = cstrCmdLine.Find(".exe");
    if (nStart == -1)
    {
        // 错误
        printf("Command line: recloader.exe [install <n> | remove]\n");
        return FALSE;
    }

    nStart += strlen(".exe")+1;
    cstrCmdLine = (lpszCmdLine+nStart);
    cstrCmdLine.TrimLeft(" ");
    cstrCmdLine.TrimRight(" ");

    if (cstrCmdLine.Find("install") >= 0)
    {
        // 安装
        nStart = cstrCmdLine.Find("install");
        int nCopies = atoi( (LPCTSTR)cstrCmdLine + nStart + strlen("install") );
        dlg.Install(nCopies);
    }
    else if (cstrCmdLine.Find("remove") >= 0)
    {
        // 删除
        dlg.UnInstall();
    }
    else
    {
        // 启动界面
        m_pMainWnd = &dlg;
        dlg.DoModal();
    }
         
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
