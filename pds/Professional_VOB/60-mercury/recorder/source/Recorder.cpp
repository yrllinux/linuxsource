// Recorder.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Recorder.h"
#include "RecorderDlg.h"
//#include "ConfAgent.h"

#include "evrec.h"
#include "usbverify.h"
#include "usbkey.h"
//CUsbVerify	g_cUsbVerify;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp

BEGIN_MESSAGE_MAP(CRecorderApp, CWinApp)
	//{{AFX_MSG_MAP(CRecorderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp construction

CRecorderApp::CRecorderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRecorderApp object

CRecorderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp initialization

/*=============================================================================
  函 数 名： USBKeyOutCallBack
  功    能： USB Key 拔出回调
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
u32 __stdcall USBKeyOutCallBack(IN void *pCBData)
{
	HWND hMainWnd = theApp.GetMainWnd()->GetSafeHwnd();
    if ( LANGUAGE_CHINESE == GetSystemDefaultLangID() )
	{
		MessageBox(hMainWnd, "请插入正确的 USB Key !\n", "错误", MB_ICONSTOP);
	}
	else
	{
		MessageBox(hMainWnd, "Please insert USB Key !\n", "Error", MB_ICONSTOP);
	}
    SendMessage(hMainWnd, WM_QUITREC, 0, 0);

	return 0;
}

/*====================================================================
    函数名	     ：InitInstance
	功能		 ：应用程序实例初始化函数
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：TRUE - 成功   FALSE - 失败
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    05/03/31    3.6         李博          禁止程序运行多次
====================================================================*/
BOOL CRecorderApp::InitInstance()
{
	AfxEnableControlContainer();

    BOOL32 m_bDefaultLang = GetSystemDefaultLangID();

    // 顾振华 [6/8/2006] 允许多个实例
	s8 achProfileName[KDV_MAX_PATH] = _T("");
	CString csPath = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achProfileName, sizeof (achProfileName));

	csPath = achProfileName;
	csPath = csPath.Left( csPath.ReverseFind('\\') + 1 );
	csPath += RECCFG_FILE;

    g_tCfg.SetCfgFilename( csPath );
    g_tCfg.ReadConfigFromFile();

    if (!g_tCfg.IsInited())
    {
        // 初始化失败，退出
        return FALSE;
    }

    // 如果不允许多个，则创建互斥量
    HANDLE hMutex = NULL;
    // guzh [9/8/2006] 总是允许不同路径录像机启动，同时禁止相同路径的录像机启动
    // 解决方案：以程序全名创建互斥量
    //if (!g_tCfg.bMultiInst)
    {
        // 互斥量名字不能带\，总长度为 KDV_MAX_PATH
        CString cstrMutexName = achProfileName;
        cstrMutexName.Replace("\\", "/");   
        hMutex = CreateMutex(NULL, FALSE, cstrMutexName);
        if (hMutex == NULL)
        {
            if ( LANGUAGE_CHINESE == m_bDefaultLang ) 
            {
                AfxMessageBox("KDV 录放像服务器初始化失败!");
            }
            else
            {
                AfxMessageBox("KDV Record Server initialize failed !");
            }
            return FALSE;
        }
        else if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CWnd* pCBWnd;
            if ( LANGUAGE_CHINESE == m_bDefaultLang )
            {
                AfxMessageBox("KDV 录放像服务器已经运行!");
                pCBWnd = CWnd::FindWindow(NULL, "KDV 录放像服务器");
            }
            else
            {
                AfxMessageBox("KDV Record Server has been running already!");
                pCBWnd = CWnd::FindWindow(NULL, "KDV Recorder Server");                
            }
		    if (pCBWnd)
		    {
			    pCBWnd->ShowWindow(SW_SHOWNORMAL);
			    pCBWnd->SetForegroundWindow();
		    }

            return FALSE;
        }
    }

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


    // [11/23/2006-zbq] 关于USB Key的支持临注释掉
//	//检测 USB Key
//	if ( g_cUsbVerify.Verify(enumUsbRightVOS) )
//	{
//		g_cUsbVerify.SetDeviceCallBack( NULL, USBKeyOutCallBack, NULL );
//	}
//	else
//	{
//		if ( LANGUAGE_CHINESE == m_bDefaultLang )
//		{
//			AfxMessageBox( "请插入正确的 USB Key !\n" );
//		}
//		else
//		{
//			AfxMessageBox( "Please insert USB Key first !\n" );
//		}
//		return FALSE;
//	}

	
    // 初始化Osp应用
    UserInit();  

    // 启动对话框
	CRecorderDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

    if (hMutex != NULL)
    {
         ReleaseMutex(hMutex);
    }
	return FALSE;
}
