// McuSpyAnaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "McuSpyAna.h"
#include "McuSpyAnaDlg.h"
#include "winsock2.h"
#include "osp.h"
#include "mcuconst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CMcuSpyAnaDlg dialog

CMcuSpyAnaDlg::CMcuSpyAnaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMcuSpyAnaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMcuSpyAnaDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMcuSpyAnaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMcuSpyAnaDlg)
	DDX_Control(pDX, IDC_CB_TYPE, m_cbType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMcuSpyAnaDlg, CDialog)
	//{{AFX_MSG_MAP(CMcuSpyAnaDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_RUN, OnBtnRun)
	ON_BN_CLICKED(IDC_BTN_QUIT, OnBtnQuit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMcuSpyAnaDlg message handlers

BOOL CMcuSpyAnaDlg::OnInitDialog()
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
    SetDlgItemInt(IDC_ET_NUM, 14);

    SetDlgItemInt(IDC_ET_LAYER, 1);

    SetDlgItemInt(IDC_ET_SLOT, 0);
    
    m_cbType.SetItemData( m_cbType.AddString("MMP"), BRD_TYPE_MMP);
    m_cbType.SetItemData( m_cbType.AddString("DRI"), BRD_TYPE_DRI);
    m_cbType.SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMcuSpyAnaDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMcuSpyAnaDlg::OnPaint() 
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
HCURSOR CMcuSpyAnaDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMcuSpyAnaDlg::IncPercent(CString &line, LPCTSTR tar, long &nTarget)
{
    int nStart = line.Find( tar );

    CString cstrNum = line.Mid(nStart + strlen(tar) +1, 12);
      
    nTarget += atoi(cstrNum);
}

void FillIp(CString &strHostIP)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
 
    strHostIP = "172.16.5.30";

    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return;
    }

	char szHostName[32] = _T("");
    gethostname(szHostName, 16);
    hostent *tHost = gethostbyname(szHostName);
    if (tHost == NULL)
    {
        return ;
    }
	sockaddr_in sa;
    if ( tHost->h_addr_list[0] )
    {
		memcpy (&sa.sin_addr.s_addr, tHost->h_addr_list[0],tHost->h_length);
		strHostIP = inet_ntoa(sa.sin_addr);
    }
}

void CMcuSpyAnaDlg::Anaylise()
{
    PROCESS_INFORMATION hInfo;
    STARTUPINFO si={0}; 
    si.cb = sizeof(si); 

    ZeroMemory( &hInfo, sizeof(hInfo) );
    BOOL bOk;

    m_vtInst.clear();

    CString cstrPath, cstrTmp, cstrTmp2;
    cstrPath = ".\\conf\\brdcfgdebug.ini";

    int nNum = GetDlgItemInt( IDC_ET_NUM );
    int nLayer = GetDlgItemInt( IDC_ET_LAYER );

    CString cstrIp;
    FillIp(cstrIp);

	TCHAR achDllName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achDllName, sizeof (achDllName));

	CString csPath = achDllName;
	csPath = csPath.Left( csPath.ReverseFind('\\') );

    s32 nType = m_cbType.GetItemData(m_cbType.GetCurSel());

    CString cstrEqpName ;
    if (nType == BRD_TYPE_MMP)
    {
        cstrEqpName.Format("%s\\%s", csPath, "eqpsimu.exe");
    }
    else
    {
        cstrEqpName.Format("%s\\%s", csPath, "mtadp.exe");
    }

    
    int idx = GetDlgItemInt(IDC_ET_SLOT);    

    CStdioFile file;
    for (int i = 0; i < nNum; i++)
    {
        file.Open( cstrPath, CFile::modeCreate | CFile::modeWrite );

/*
[BoardDebug]
IsTest = 1

[BoardConfig]
Layer =1
Slot = 0
Type = 5
BoardIpAddr = 172.16.5.30        
*/
        file.WriteString( "[BoardDebug]\n" );
        file.WriteString( "IsTest = 1\n\n" );
        file.WriteString( "[BoardConfig]\n");

        cstrTmp.Format("Layer = %d\n", nLayer);
        file.WriteString(cstrTmp);

        cstrTmp.Format("Slot = %d\n", idx);
        file.WriteString(cstrTmp);
        
        cstrTmp.Format("Type = %d\n", nType);
        file.WriteString( cstrTmp );

        cstrTmp.Format("BoardIpAddr = %s\n", cstrIp);
        file.WriteString(cstrTmp);

        file.Flush();
        file.Close();

        bOk = CreateProcess(cstrEqpName, NULL, NULL, NULL, FALSE, 0, NULL, 
                            csPath, &si, &hInfo);

        if ( bOk )
        {
            m_vtInst.push_back( hInfo.hProcess );
        }

        Sleep(3000);

        idx ++;
        if (idx == 7)
            idx ++;
        if (idx >14)
        {
            nLayer ++;
            idx = 0;
            if (nLayer > 3)
                break;
        }

    }
}

void CMcuSpyAnaDlg::OnBtnRun() 
{
    Anaylise();
	
}

void CMcuSpyAnaDlg::OnBtnQuit() 
{
    for (int i = 0; i < m_vtInst.size(); i++)
    {
        TerminateProcess( m_vtInst[i], 0 );

        Sleep(1000);
    }

    m_vtInst.clear();
	
    WSACleanup();

    EndDialog(IDOK);
	
}
