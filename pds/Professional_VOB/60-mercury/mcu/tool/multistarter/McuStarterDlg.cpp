// McuStarterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "McuStarter.h"
#include "McuStarterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMcuStarterDlg dialog

CMcuStarterDlg::CMcuStarterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMcuStarterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMcuStarterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMcuStarterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMcuStarterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMcuStarterDlg, CDialog)
	//{{AFX_MSG_MAP(CMcuStarterDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMcuStarterDlg message handlers

BOOL CMcuStarterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    SetDlgItemInt(IDC_ET_NUM, 5);
    SetDlgItemText(IDC_ET_ALIAS, "TestMcu");
    SetDlgItemText(IDC_ET_E164, "53000");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMcuStarterDlg::OnPaint() 
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
HCURSOR CMcuStarterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMcuStarterDlg::OnStart() 
{

	TCHAR achDllName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achDllName, sizeof (achDllName));

	CString csOldPath = achDllName;
	csOldPath = csOldPath.Left( csOldPath.ReverseFind('\\') );    // 完整路径

	
    CString cstrPath0, cstrPath1, cstrPath2, cstrPath3, cstrPath4, cstrTmp, cstrTmp2, cstrTmp3;
    cstrPath0 = "mcu.exe";
    cstrPath1 = "mcucfg.ini";
    cstrPath2 = "mcudebug.ini";
    cstrPath3 = "mtadpdebug.ini";
    cstrPath4 = "kdv323debug.ini";

    CString cstrMcuAlias;
    CString cstrMcuE164;

    GetDlgItemText( IDC_ET_ALIAS, cstrMcuAlias );
    GetDlgItemText( IDC_ET_E164, cstrMcuE164);


    PROCESS_INFORMATION hInfo;
    STARTUPINFO si={0}; 
    si.cb = sizeof(si); 

    ZeroMemory( &hInfo, sizeof(hInfo) );
    BOOL bOk;

    m_vtInst.clear();


    int nNum =  GetDlgItemInt( IDC_ET_NUM );
    for (int i = 0; i <nNum; i++)
    {
        // 复制文件
        cstrTmp.Format("%s\\mcu%d", csOldPath, i);
        CreateDirectory ( cstrTmp, NULL );
        cstrTmp3.Format("%s\\%s", csOldPath, cstrPath0);
        cstrTmp2.Format("%s\\%s", cstrTmp, cstrPath0);
        CopyFile( cstrTmp3,  cstrTmp2, FALSE);
        cstrTmp3.Format("%s\\%s", csOldPath, cstrPath4);
        cstrTmp2.Format("%s\\%s", cstrTmp, cstrPath4);
        CopyFile( cstrTmp3,  cstrTmp2, FALSE);

        cstrTmp.Format("%s\\mcu%d\\conf", csOldPath, i);
        CreateDirectory ( cstrTmp, NULL );

        cstrTmp3.Format("%s\\conf\\%s", csOldPath, cstrPath1);
        cstrTmp2.Format("%s\\%s", cstrTmp, cstrPath1);
        CopyFile( cstrTmp3,  cstrTmp2, FALSE);

        cstrTmp3.Format("%s\\conf\\%s", csOldPath, cstrPath2);
        cstrTmp2.Format("%s\\%s", cstrTmp, cstrPath2);
        CopyFile( cstrTmp3,  cstrTmp2, FALSE);

        cstrTmp3.Format("%s\\conf\\%s", csOldPath, cstrPath3);
        cstrTmp2.Format("%s\\%s", cstrTmp, cstrPath3);
        CopyFile( cstrTmp3,  cstrTmp2, FALSE);

        
        // 更改配置文件    

        // cfg
        cstrTmp2.Format("%s\\mcu%d\\conf\\%s", csOldPath, i, cstrPath1);

        cstrTmp.Format("%s_%d", cstrMcuAlias,  i);
        
        WritePrivateProfileString(_T("mcuLocalInfo"), _T("mcuAlias"),
                            cstrTmp,
                            cstrTmp2);
        
        cstrTmp.Format("%s%d", cstrMcuE164, i);
        WritePrivateProfileString(_T("mcuLocalInfo"), _T("mcuE164Number"),
                            cstrTmp,
                            cstrTmp2);


        cstrTmp.Format("%d", 20000 + i * 1000);
        WritePrivateProfileString(_T("mcuNetwork"), _T("mcunetRecvStartPort"),
                            cstrTmp,
                            cstrTmp2);

        cstrTmp.Format("%d", 20000 + i * 1000 - 2);
        WritePrivateProfileString(_T("mcuNetwork"), _T("mcunet225245StartPort"),
                            cstrTmp,
                            cstrTmp2);
        
        // debug
        cstrTmp2.Format("%s\\mcu%d\\conf\\%s", csOldPath, i, cstrPath2);

        cstrTmp.Format("%d", 60000 + i);
        WritePrivateProfileString(_T("mcuNetWork"), _T("mcuListenPort"),
                            cstrTmp,
                            cstrTmp2);

        cstrTmp.Format("%d", 2800 + i);
        WritePrivateProfileString(_T("mcuNetWork"), _T("mcuTelnetPort"),
                            cstrTmp,
                            cstrTmp2);

        cstrTmp.Format("%d", 1720 + 10 * i);
        WritePrivateProfileString(_T("mcuNetWork"), _T("mcuQ931Port"),
                            cstrTmp,
                            cstrTmp2);

        cstrTmp.Format("%d", 1719 + 10 * i);
        WritePrivateProfileString(_T("mcuNetWork"), _T("mcuRasPort"),
                            cstrTmp,
                            cstrTmp2);
        
        // adp debug
        cstrTmp2.Format("%s\\mcu%d\\conf\\%s", csOldPath, i, cstrPath3);

        cstrTmp.Format("%d", 1720 + 10 * i);
        WritePrivateProfileString(_T("calldata"), _T("q931port"),
                            cstrTmp,
                            cstrTmp2);

        cstrTmp.Format("%d", 1719 + 10 * i);
        WritePrivateProfileString(_T("calldata"), _T("rasport"),
                            cstrTmp,
                            cstrTmp2);

        // 323 cas debug
        cstrTmp2.Format("%s\\mcu%d\\%s",csOldPath, i, cstrPath4);


        cstrTmp.Format("%d", 3337 + i);
        WritePrivateProfileString(_T("CONFIG"), _T("MCUCASCADEPORT"),
                            cstrTmp,
                            cstrTmp2);

        cstrTmp.Format("%s\\mcu%d\\", csOldPath, i);
        cstrTmp2.Format("%s%s", cstrTmp, cstrPath0);

        Sleep(150);
        cstrTmp3 = cstrTmp2; //  + " 0"; // 0 表示不启动telnet
        bOk = CreateProcess(cstrTmp2, (LPSTR)(LPCTSTR)cstrTmp3, NULL, NULL, FALSE, 0, NULL, 
                            cstrTmp, &si, &hInfo);

        if ( bOk )
        {
            m_vtInst.push_back( hInfo.hProcess );
        }

        Sleep(1500);
    }
}

void CMcuStarterDlg::OnCancel() 
{
    for (int i = 0; i < m_vtInst.size(); i++)
    {
        TerminateProcess( m_vtInst[i], 0 );

        Sleep(1000);
    }    
	
	CDialog::OnCancel();
}
