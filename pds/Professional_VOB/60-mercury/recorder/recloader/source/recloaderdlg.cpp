/*****************************************************************************
   模块名      : Recorder Loader
   文件名      : recloaderdlg.cpp
   相关文件    : recloader.cpp
   文件实现功能: 录像机安装、卸载和启动
   作者        : 
   版本        : V4.0  Copyright(C) 2004-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/08/23  4.0         顾振华      创建
******************************************************************************/

// recloaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "recloader.h"
#include "recloaderDlg.h"
#include "winsock2.h"
#include "kdvsys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LANGUAGE_CHINESE        MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)

#define RECAPPNAME          (LPCTSTR)"Recorder.exe"
#define RECCFGPATH          (LPCTSTR)"conf"
#define RECCFGNAME          (LPCTSTR)"reccfg.ini"

#define SEC_SYS             (LPCTSTR)"RecorderSystem"


// 提示
#define RES_TITLE           1
#define RES_LAUNCH          2
#define RES_INSTALLED       3
#define RES_EXIT            4
#define RES_EXIT_ALONE      5
#define RES_ALL             6
#define RES_NONE            7

#define RES_REINSTALL       100

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecloaderDlg dialog

CRecloaderDlg::CRecloaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecloaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecloaderDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRecloaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecloaderDlg)
	DDX_Control(pDX, IDC_CK_REC14, m_ckRec14);
	DDX_Control(pDX, IDC_CK_REC13, m_ckRec13);
	DDX_Control(pDX, IDC_CK_REC12, m_ckRec12);
	DDX_Control(pDX, IDC_CK_REC11, m_ckRec11);
	DDX_Control(pDX, IDC_CK_REC10, m_ckRec10);
	DDX_Control(pDX, IDC_CK_REC9, m_ckRec9);
	DDX_Control(pDX, IDC_CK_REC8, m_ckRec8);
	DDX_Control(pDX, IDC_CK_REC7, m_ckRec7);
	DDX_Control(pDX, IDC_CK_REC6, m_ckRec6);
	DDX_Control(pDX, IDC_CK_REC5, m_ckRec5);
	DDX_Control(pDX, IDC_CK_REC4, m_ckRec4);
	DDX_Control(pDX, IDC_CK_REC3, m_ckRec3);
	DDX_Control(pDX, IDC_CK_REC2, m_ckRec2);
	DDX_Control(pDX, IDC_CK_REC1, m_ckRec1);
	DDX_Control(pDX, IDC_CK_REC0, m_ckRec0);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRecloaderDlg, CDialog)
	//{{AFX_MSG_MAP(CRecloaderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_ALL, OnBtnAll)
	ON_BN_CLICKED(IDC_BTN_NONE, OnBtnNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecloaderDlg message handlers

BOOL CRecloaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    m_tLangId = GetSystemDefaultLangID();
    
    InitRecChecks();

	// Os是简体中文，切换语言到中文
    if( LANGUAGE_CHINESE == m_tLangId )
    {
        ChgUILang();
    }
    
    if ( m_nValidRecNum == 1 )
    {
        OnBtnStart();

        if (m_vtInst.size() == 1)
        {
            OnCancel();
        }
    }
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRecloaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRecloaderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRecloaderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*====================================================================
    函数名	     ：InitRecChecks
	功能		 ：初始化界面
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/23	4.0			顾振华        创建
====================================================================*/
void CRecloaderDlg::InitRecChecks()
{
    m_apckRec[0] = &m_ckRec0;
    m_apckRec[1] = &m_ckRec1;
    m_apckRec[2] = &m_ckRec2;
    m_apckRec[3] = &m_ckRec3;
    m_apckRec[4] = &m_ckRec4;
    m_apckRec[5] = &m_ckRec5;
    m_apckRec[6] = &m_ckRec6;
    m_apckRec[7] = &m_ckRec7;
    m_apckRec[8] = &m_ckRec8;
    m_apckRec[9] = &m_ckRec9;
    m_apckRec[10] = &m_ckRec10;
    m_apckRec[11] = &m_ckRec11;
    m_apckRec[12] = &m_ckRec12;
    m_apckRec[13] = &m_ckRec13;
    m_apckRec[14] = &m_ckRec14;

    m_nValidRecNum = 0;
    
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // 完整路径

    CString cstrOldCfgName;
    cstrOldCfgName.Format("%s\\%s\\%s", cstrExePath, RECCFGPATH, RECCFGNAME);
    
    int nCopies = 0;
    nCopies = GetPrivateProfileInt( SEC_SYS,
                                    "InstalledCopies",
                                    0,
                                    cstrOldCfgName );    
    
    if (nCopies == 0)
    {
        AfxMessageBox(GetText(RES_REINSTALL));
        return;
    }

    CString cstrNewCfgName;
    int nID;
    char szAlias[KDV_MAX_PATH] = {0};
    CString cstrTmp;
    
    CFileFind cff;
    for (int i = 0; i < nCopies; i++)
    {
        // 获取各个录像机ID/名字
        cstrNewCfgName.Format("%s\\%d\\%s\\%s", cstrExePath, i, RECCFGPATH, RECCFGNAME);

        if (!cff.FindFile(cstrNewCfgName))
        {
            // 没找到，错误？
            cff.Close();
            continue;
        }
        cff.Close();
        
        // ID
        cstrTmp.Format("%d", 17+i);        
        nID = GetPrivateProfileInt(SEC_SYS, 
                                   _T("ID"),
                                   17,
                                   cstrNewCfgName);

        // Alias
        GetPrivateProfileString(SEC_SYS, 
                                _T("Alias"),
                                "Recorder",
                                szAlias,
                                KDV_MAX_PATH,
                                cstrNewCfgName);

        cstrTmp.Format("ID%d: %s", nID, szAlias);

        m_apckRec[m_nValidRecNum]->SetWindowText( cstrTmp );
        m_apckRec[m_nValidRecNum]->ShowWindow(SW_SHOW);
        m_apckRec[m_nValidRecNum]->SetCheck(BST_CHECKED);

        // 保存下应用名
        m_astrAppName[m_nValidRecNum].Format("%s\\%d\\%s", cstrExePath, i, RECAPPNAME);

        m_nValidRecNum ++;
    }
}

/*====================================================================
    函数名	     ：GetText
	功能		 ：双语支持
	引用全局变量 ：无
    输入参数说明 ：nResID - 语言资源
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/23	4.0			顾振华        创建
====================================================================*/
CString CRecloaderDlg::GetText(int nResID)
{
    if (LANGUAGE_CHINESE == m_tLangId)
    {
        switch (nResID)
        {
            case RES_TITLE:
                return "启动录像机";
            case RES_LAUNCH:
                return "启动录像机";
            case RES_INSTALLED:
                return "安装的录像机";
            case RES_EXIT:
                return "退出并结束所有录像机";
            case RES_EXIT_ALONE:
                return "直接退出";
            case RES_REINSTALL:
                return "没有检测到安装的录像机，请修复安装!";
            case RES_ALL:
                return "所有";
            case RES_NONE:
                return "清空";
            default:
                return "";
        }
    }
    else
    {
        switch (nResID)
        {
            case RES_TITLE:
                return "Kedacom Recorder Loader";
            case RES_LAUNCH:
                return "Launch";
            case RES_INSTALLED:
                return "Recorder Installed";
            case RES_EXIT:
                return "Exit with All Recorders";
            case RES_EXIT_ALONE:
                return "Exit Alone";
            case RES_REINSTALL:
                return "No Recorder Detected, please repair installation!";
            case RES_ALL:
                return "All";
            case RES_NONE:
                return "None";
            default:
                return "";
        }
    }
}

/*====================================================================
    函数名	     ：ChgUILang
	功能		 ：切换语言
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/23	4.0			顾振华        创建
====================================================================*/
void CRecloaderDlg::ChgUILang()
{
    AfxGetMainWnd()->SetWindowText(GetText(RES_TITLE));
    
    GetDlgItem(IDC_GRP_REC)->SetWindowText(GetText(RES_INSTALLED));
    GetDlgItem(IDC_BTN_START)->SetWindowText(GetText(RES_LAUNCH));
    GetDlgItem(IDC_BTN_ALL)->SetWindowText(GetText(RES_ALL));
    GetDlgItem(IDC_BTN_NONE)->SetWindowText(GetText(RES_NONE));
    GetDlgItem(IDOK)->SetWindowText(GetText(RES_EXIT));
    GetDlgItem(IDCANCEL)->SetWindowText(GetText(RES_EXIT_ALONE));
}


/*====================================================================
    函数名	     ：GetSpecPath
	功能		 ：获取桌面等特殊文件夹地址
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/24	4.0			顾振华        创建
====================================================================*/
BOOL GetSpecPath(int csidlType, char *szPath)
{
    LPITEMIDLIST ppidl;
    HRESULT hRes = SHGetSpecialFolderLocation(NULL, csidlType , &ppidl);    // | CSIDL_FLAG_CREATE
    BOOL bRet = FALSE;
    if (hRes == S_OK)
    {
        bRet = SHGetPathFromIDList( ppidl, szPath );
    }    
    return bRet;
}

/*====================================================================
    函数名	     ：InitCOM
	功能		 ：初始化COM接口
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/24	4.0			顾振华        创建
====================================================================*/
BOOL InitCOM(IShellLink* &pisl, IPersistFile* &pipf)
{
    ::CoInitialize(NULL);            
    HRESULT hRes = CoCreateInstance (CLSID_ShellLink, 
                                     NULL, CLSCTX_INPROC_SERVER,
                                     IID_IShellLink, 
                                     (void **)&pisl); 
    if (!SUCCEEDED (hRes))
    {
        return FALSE;
    }
    else
    {
        // 从ISHELLLINK对象中获得IpersistFile对象的接口
        hRes = pisl->QueryInterface(IID_IPersistFile, (void **)&pipf);         
        if (!SUCCEEDED (hRes))
        { 
            pisl->Release();
            return FALSE;
        }
        else
        {
            return TRUE;
        }    
    } 
}

/*====================================================================
    函数名	     ：Install
	功能		 ：安装录像机的多个拷贝，并安装快捷方式
	引用全局变量 ：无
    输入参数说明 ：nCopies - 拷贝数。如果是-1表示修复安装，从配置里面读取
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/24	4.0			顾振华        创建
====================================================================*/
void CRecloaderDlg::Install(int nCopies)
{    
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

    // 获取快捷方式的目标路径
    TCHAR szShortcutPath[KDV_MAX_PATH] = {0};
    BOOL bCreateShortcut = FALSE;
    bCreateShortcut = GetSpecPath(CSIDL_COMMON_PROGRAMS, szShortcutPath);  //CSIDL_DESKTOP, CSIDL_PROGRAMS, 
    strcat(szShortcutPath, "\\Kedacom\\Recorder4.0");

    IShellLink *pisl;    
    IPersistFile *pipf;
    if (bCreateShortcut)
    {
        bCreateShortcut = InitCOM(pisl, pipf);
    }

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // 完整路径

    CString cstrOldRecName;
    cstrOldRecName.Format("%s\\%s", cstrExePath, RECAPPNAME);
    CString cstrOldCfgName;
    cstrOldCfgName.Format("%s\\%s\\%s", cstrExePath, RECCFGPATH, RECCFGNAME);

    CFileFind cff;
    if (!cff.FindFile(cstrOldRecName))
    {
        // 失败
        cff.Close();
        return;
    }
    cff.Close();
    if (!cff.FindFile(cstrOldCfgName))
    {
        // 失败
        cff.Close();
        return;
    }
    cff.Close();
    
	//改变配置文件的属性为Normal, 以保存修改后的参数. 
	CFile* pFile = NULL;
	CFileStatus cFileOrignalStatus;
	memset( &cFileOrignalStatus, 0, sizeof(CFileStatus) );
	CFileStatus cFileChangedStatus;
	memset( &cFileChangedStatus, 0, sizeof(CFileStatus) );
	pFile->GetStatus( cstrOldCfgName, cFileOrignalStatus );
	pFile->GetStatus( cstrOldCfgName, cFileChangedStatus );

	if ( 0x00 != cFileOrignalStatus.m_attribute ) 
	{
		cFileChangedStatus.m_attribute = 0x00;
		pFile->SetStatus(cstrOldCfgName, cFileChangedStatus);
	}


    CString cstrPath, cstrPath2;
    CString cstrAppName, cstrCfgName;
    CString cstrTmp;

    if (nCopies == -1)
    {
        // 修复，获取原来配置里面保存的
        nCopies = GetPrivateProfileInt( SEC_SYS,
                                        "InstalledCopies",
                                        1,
                                        cstrOldCfgName );    

        if (nCopies == 0)
            nCopies = 1;
    }
    // 写入拷贝数配置
    cstrTmp.Format("%d", nCopies);
    WritePrivateProfileString(SEC_SYS,
                              "InstalledCopies",
                              cstrTmp,
                              cstrOldCfgName );

	 
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	char szHostName[32] = _T("");
    CString cstrHostIp;
            
    // 网络名
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );

    if ( err != 0 ) 
    {
        // 取默认配置
        strcpy( szHostName, "KEDACOM" );
        cstrHostIp = "127.0.0.1";
    }
    else
    {
        gethostname(szHostName, 31);
        if (strlen(szHostName) == 0)
        {
            strcpy( szHostName, "KEDACOM" );
        }
        else
        {
            hostent *tHost = gethostbyname(szHostName);
            if (tHost == NULL)
            {
                cstrHostIp = "127.0.0.1";
            }
            else
            {
	            sockaddr_in sa;
                if ( tHost->h_addr_list[0] )
                {
		            memcpy (&sa.sin_addr.s_addr, tHost->h_addr_list[0],tHost->h_length);
		            cstrHostIp = inet_ntoa(sa.sin_addr);
                }    
                else
                {
                    cstrHostIp = "127.0.0.1";
                }
            }    
        }
    } 

    for (int i = 0; i < nCopies; i++)
    {
        // 创建目录
        cstrPath.Format("%s\\%d", cstrExePath, i);
        if (!cff.FindFile(cstrPath))
        {
            CreateDirectory ( cstrPath, NULL );
        }
        cff.Close();
        
        cstrPath2.Format("%s\\%s", cstrPath, RECCFGPATH);
        if (!cff.FindFile(cstrPath2))
        {
            CreateDirectory ( cstrPath2, NULL );
        }
        cff.Close();        

        // 复制文件
        cstrAppName.Format("%s\\%s", cstrPath, RECAPPNAME);
        CopyFile( cstrOldRecName,  cstrAppName, FALSE);

        cstrCfgName.Format("%s\\%s", cstrPath2, RECCFGNAME);
        CopyFile( cstrOldCfgName,  cstrCfgName, FALSE);
        
        // 更改配置文件    

        // ID
        cstrTmp.Format("%d", 17+i);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("ID"),
                                  cstrTmp,
                                  cstrCfgName);

        // Alias
        cstrTmp.Format("Recorder%d", 17+i);        
        WritePrivateProfileString(SEC_SYS, 
                                  _T("Alias"),
                                  cstrTmp,
                                  cstrCfgName);

        // IPAddr、机器名
        WritePrivateProfileString(SEC_SYS, 
                                  _T("IpAddr"),
                                  cstrHostIp,
                                  cstrCfgName);

        WritePrivateProfileString(SEC_SYS, 
                                  _T("MachineName"),
                                  szHostName,
                                  cstrCfgName);

        // 端口
        cstrTmp.Format("%d", 60000+i*200);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("RecorderRcvStartPort"),
                                  cstrTmp,
                                  cstrCfgName);

        cstrTmp.Format("%d", 60100+i*200);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("RecorderPlayStartPort"),
                                  cstrTmp,
                                  cstrCfgName);     
        
        // 录放像通道
        cstrTmp.Format("%d", 3);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("RecordChannels"),
                                  cstrTmp,
                                  cstrCfgName);

        cstrTmp.Format("%d", 3);
        WritePrivateProfileString(SEC_SYS, 
                                  _T("PlayChannels"),
                                  cstrTmp,
                                  cstrCfgName);   

/*  以下代码可以生成快捷方式到指定位置 
        // 创建快捷方式到程序菜单
        if (bCreateShortcut)
        {
            pisl->SetPath (cstrAppName);

            cstrTmp.Format("%s\\Recorder %d.lnk", szShortcutPath, 17+i);

            WORD uniszLnk [KDV_MAX_PATH] = {(WORD)0}; 
            // 确保快捷方式路径由ANSI字符组成
            MultiByteToWideChar (CP_ACP, 0, cstrTmp, -1, uniszLnk, KDV_MAX_PATH);

            //保存快捷方式
            pipf->Save(uniszLnk, TRUE);  
        }                
*/
    }
    if (bCreateShortcut)
    {
        pipf->Release ();
        pisl->Release ();
    }

    ::CoUninitialize();
}

/*====================================================================
    函数名	     ：UnInstall
	功能		 ：卸载录像机的多个拷贝。
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/24	4.0			顾振华        创建
====================================================================*/
void CRecloaderDlg::UnInstall()
{
    // 删除所有我们生成的目录和文件，而不是从配置文件里面去取。因为配置文件可能被反复修改。

    // 获取快捷方式的目标路径
    TCHAR szShortcutPath[KDV_MAX_PATH] = {0};
    BOOL bCreateShortcut = FALSE;
    bCreateShortcut = GetSpecPath(CSIDL_COMMON_PROGRAMS, szShortcutPath);  //CSIDL_DESKTOP, CSIDL_PROGRAMS
    strcat(szShortcutPath, "\\Kedacom\\Recorder4.0");

    CFileFind cff;
    
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // 完整路径

    CString cstrTarget;
    cstrTarget.Format("%s\\*.*", cstrExePath);

    BOOL bFound = cff.FindFile( cstrTarget );
    CString cstrPath;
    CString cstrTmp;
    
    while (bFound)
    {
        // 查找下一个目录
        bFound = cff.FindNextFile();
        if (!cff.IsDirectory())
        {
            continue;
        }
        if (cff.IsDots())
        {
            continue;
        }
        if (cff.GetFileName() == RECCFGPATH)
        {
            continue;
        }
        cstrPath = cff.GetFilePath();
        cstrTmp.Format("%s\\%s", cstrPath, RECAPPNAME);
        ::DeleteFile( cstrTmp );

        cstrTmp.Format("%s\\%s\\%s", cstrPath, RECCFGPATH, RECCFGNAME);
        ::DeleteFile( cstrTmp );

        cstrTmp.Format("%s\\%s", cstrPath, RECCFGPATH);
        ::RemoveDirectory( cstrTmp );

        ::RemoveDirectory( cstrPath );
    }
    cff.Close();

/*  以下代码可以删除快捷方式
    // 删除快捷方式。注意：这里只能删除 \\开始菜单\程序\Kedacom\Recorder4.0\*.lnk
    cstrTarget.Format("%s\\*.lnk", szShortcutPath);

    bFound = cff.FindFile( cstrTarget );
    while (bFound)
    {
        // 查找下一个目录
        bFound = cff.FindNextFile();
        if (cff.IsDots())
        {
            continue;
        }

        cstrPath = cff.GetFilePath();
        ::DeleteFile( cstrPath );
    }
    cff.Close();
*/
    ::CoUninitialize();

}

/*====================================================================
    函数名	     ：OnBtnStart
	功能		 ：启动录像机程序
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	06/08/24	4.0			顾振华        创建
====================================================================*/
void CRecloaderDlg::OnBtnStart() 
{
    BOOL bOk;
    PROCESS_INFORMATION hInfo;
    STARTUPINFO si = {0}; 
    si.cb = sizeof(si); 
    CString cstrPath;

    ZeroMemory( &hInfo, sizeof(hInfo) );
    
    for ( int i = 0; i < m_nValidRecNum; i ++ )
    {
        if (m_apckRec[i]->GetCheck() == BST_CHECKED)
        {
            // 用户选中    
            cstrPath = m_astrAppName[i].Left( m_astrAppName[i].ReverseFind('\\') );    // 完整路径
                
            bOk = ::CreateProcess(m_astrAppName[i], (LPSTR)(LPCTSTR)m_astrAppName[i], NULL, NULL, FALSE, 0, NULL, 
                                  cstrPath, &si, &hInfo);
            if ( bOk )
            {
                m_vtInst.push_back( hInfo.hProcess );
                Sleep(100);
            }            
        }
    }
   
    if ( m_vtInst.size() > 0 )
    {
        // 启动成功后
	    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

        // 退出程序
        // OnCancel();
    }
}

void CRecloaderDlg::OnOK() 
{
    for (int i = 0; i < m_vtInst.size(); i++)
    {
        TerminateProcess( m_vtInst[i], 0 );

        Sleep(1000);
    }    
	
	OnCancel();
}

void CRecloaderDlg::OnCancel() 
{	
	CDialog::OnCancel();
}

void CRecloaderDlg::OnBtnAll() 
{
    for (int i = 0; i < m_nValidRecNum; i++)
	    m_apckRec[i]->SetCheck(BST_CHECKED);
	
}

void CRecloaderDlg::OnBtnNone() 
{
    for (int i = 0; i < m_nValidRecNum; i++)
	    m_apckRec[i]->SetCheck(BST_UNCHECKED);	
}
