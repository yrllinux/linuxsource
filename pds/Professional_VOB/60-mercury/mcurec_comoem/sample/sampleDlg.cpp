// sampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sample.h"
#include "sampleDlg.h"
#include "DlgProxy.h"


API void setreccomlog(u8 byLvl);
API void precmsg(void);
API void nprecmsg(void);
API void recver();

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
// CSampleDlg dialog

IMPLEMENT_DYNAMIC(CSampleDlg, CDialog);

CSampleDlg::CSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSampleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSampleDlg)
	m_nDebug = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CSampleDlg::~CSampleDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleDlg)
	DDX_Control(pDX, IDC_LS_VIEW, m_lsView);
	DDX_CBIndex(pDX, IDC_CB_DEBUG, m_nDebug);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSampleDlg, CDialog)
	//{{AFX_MSG_MAP(CSampleDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_CBN_SELCHANGE(IDC_CB_DEBUG, OnSelchangeCbDebug)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP

    ON_MESSAGE(WM_CONFUPDATE,             OnConfUpdate)
    ON_MESSAGE(WM_STREAMUPDATE,           OnStreamUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleDlg message handlers

BOOL CSampleDlg::OnInitDialog()
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
    // TODO: Add extra initialization here
    
    m_dwCookieStatus = 0;
    m_dwCookieStream = 0;
    HRESULT hr;    

//    hr = m_spMcStatus.CreateInstance( _T("Mcu.MCStatusCallback.1") );
    hr = m_spRecCtrl.CoCreateInstance(CLSID_MCCtrl); 
    if ( !m_spRecCtrl )
    {
        AfxMessageBox("初始化m_spRecCtrl 失败！");
        return TRUE;
    }
    m_spProperty.CoCreateInstance(CLSID_ConfProperty);        
    if( FAILED( hr ) )
    {
        AfxMessageBox("初始化 m_spProperty 失败！");
        return TRUE;
    }    

    hr = m_spMcStatus.CreateInstance( CLSID_MCStatusCallback );
    if( FAILED( hr ) )
    {
        AfxMessageBox( _T("没有注册CLSID_MCStatusCallback还是没有初始化？") );
        CDialog::OnCancel();
	} 
    
    hr = m_spMcStream.CreateInstance(CLSID_MCStreamCallback);
    if( FAILED( hr ) )
    {
        AfxMessageBox( _T("没有注册CLSID_MCStreamCallback还是没有初始化？") );
        CDialog::OnCancel();
    } 
    
    // 得到连接点容器接口
    CComQIPtr<IConnectionPointContainer> spContainer( m_spMcStatus );
    
    if( !spContainer )
    {
        AfxMessageBox( _T("组件没有提供连接点功能") );
        return TRUE;
    }
    // 得到连接点接口
    spContainer->FindConnectionPoint( __uuidof(_IMCStatusCallbackEvents), &m_spCPStatus );
    
    if( !m_spCPStatus )
    {
        AfxMessageBox( _T("没有找到连接点接口") );
        return TRUE;
    }
    
    hr = m_spCPStatus->Advise( &m_cRecorder, &m_dwCookieStatus );
    if( FAILED( hr ) )
    {
        AfxMessageBox( _T("连接失败") );
        return TRUE;
    }
    
    // 得到连接点容器接口
    CComQIPtr<IConnectionPointContainer> spContainer2( m_spMcStream );
    
    if( !spContainer2 )
    {
        AfxMessageBox( _T("组件没有提供连接点功能2") );
        return TRUE;
    }
    // 得到连接点接口
    spContainer2->FindConnectionPoint( __uuidof(_IMCStreamCallbackEvents), &m_spCPStream );
    
    if( !m_spCPStream )
    {
        AfxMessageBox( _T("没有找到连接点接口2") );
        return TRUE;
    }
    
    hr = m_spCPStream->Advise( &m_cStream, &m_dwCookieStream );
    if( FAILED( hr ) )
    {
        AfxMessageBox( _T("连接失败2") );
        return TRUE;
    }    

#ifdef _DEBUG
    m_nDebug = 1;   // warning mode
#else
    m_nDebug = 0;   
#endif
    UpdateData(FALSE);

    SetDlgItemText(IDC_ET_MCUIP, "172.16.162.25");

    int nIdx = 0;
    m_lsView.InsertColumn(nIdx++, "ID", LVCFMT_CENTER, 40);
    m_lsView.InsertColumn(nIdx++, "会议名", LVCFMT_CENTER, 120);
    m_lsView.InsertColumn(nIdx++, "录像名", LVCFMT_CENTER, 120);
    m_lsView.InsertColumn(nIdx++, "码率", LVCFMT_CENTER, 60);
    m_lsView.InsertColumn(nIdx++, "码流数", LVCFMT_CENTER, 60);
    m_lsView.InsertColumn(nIdx++, "流1", LVCFMT_CENTER, 100);
    m_lsView.InsertColumn(nIdx++, "流2", LVCFMT_CENTER, 100);
    m_lsView.InsertColumn(nIdx++, "流3", LVCFMT_CENTER, 100);

    g_hMain = GetSafeHwnd();   

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSampleDlg::OnPaint() 
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
HCURSOR CSampleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CSampleDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CSampleDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CSampleDlg::OnCancel() 
{
    if( m_spCPStatus )
    {        
        m_spCPStatus->Unadvise( m_dwCookieStatus );
        m_spCPStatus.Release();
	}

    if( m_spCPStream )
    {
        m_spCPStream->Unadvise( m_dwCookieStream );
        m_spCPStream.Release();
    }

    if (m_spProperty)
    {
        m_spProperty.Release();
    }
    
    if (m_spRecCtrl)
    {
        m_spRecCtrl->Term();
        m_spRecCtrl.Release();
    }
    
	if (CanExit())
    {
        CDialog::OnCancel();
    }		
}

BOOL CSampleDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CSampleDlg::OnBtnStart() 
{
    UpdateData();
    setreccomlog(m_nDebug);
        
    CString strMcuIp;
    TRecCfgInfo tCfg;
    GetDlgItemText(IDC_ET_MCUIP, strMcuIp);
    tCfg.SetMcuIp( inet_addr(strMcuIp) );
    tCfg.SetEqpId(17);
    tCfg.SetRecAlias("test_rec_dll");
    tCfg.SetRecStartPort(7200);
    
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 2, 2 );
    int err = WSAStartup( wVersionRequested, &wsaData );
    u32 adwLocalAddr[16] = {0};
    OspAddrListGet(adwLocalAddr, 16);
    tCfg.SetRecIp(adwLocalAddr[0]);

    try
    {
        HRESULT hr = m_spRecCtrl->Init((long)&tCfg);
    }
    catch (...)
    {
        TRecConfStatus tStatus;
        m_spRecCtrl->GetLastError((long)&tStatus);
        int nErr = tStatus.GetErrCode();
        CString cstErr;
        cstErr.Format("初始化录像机错误，错误码：%d", nErr);
        AfxMessageBox(cstErr);
        return;
    }
    
    OspPrintf(1, 0, "初始化成功！");
    
}

void CSampleDlg::OnSelchangeCbDebug() 
{
	// TODO: Add your control notification handler code here
    UpdateData();
    setreccomlog(m_nDebug);
    if (m_nDebug)
        precmsg();
    else
        nprecmsg();

    //recver();
}

LRESULT CSampleDlg::OnConfUpdate (WPARAM wParam, LPARAM lParam)
{
    emRecState emState = (emRecState)wParam;
    DWORD dwConfIdx = lParam;

    if (emState != emRecStateStop)
    {
        if (m_spProperty)
        {
            TConfProperty tProperty;
            TRecProperty tRec;
            HRESULT hr = m_spProperty->GetConfAttrib(dwConfIdx, (long)&tProperty);
            hr = m_spProperty->GetRecAttrib(dwConfIdx, (long)&tRec);
            if ( SUCCEEDED(hr) )
            {
                UpdateView(tProperty, tRec, emState);
            }
        }        
    }
    else
    {
        UpdateView(dwConfIdx);
    }

    return S_OK;
}

LRESULT CSampleDlg::OnStreamUpdate (WPARAM wParam, LPARAM lParam)
{
    TStreamProperty tStream = *(TStreamProperty*)wParam;
    UpdateView(tStream);
    return S_OK;
}

// guzh [11/13/2007] 统计值
u32 gs_Statis[32][4] = {{0}};
void CSampleDlg::UpdateView(TConfProperty &tConf, TRecProperty &tRec, emRecState emState )
{
    // 开始会议
    int nCount = m_lsView.GetItemCount();
    DWORD dwNewIdx = tConf.GetIndex();
    BOOL bFound = FALSE;
    CString cstr;
    for (int nLoop = 0; nLoop < nCount; nLoop ++)
    {
        if (dwNewIdx == m_lsView.GetItemData(nLoop))
        {
            bFound = TRUE;
            break;
        }
    }
    int nItem;
    if (!bFound)
    {
        cstr.Format("%u", dwNewIdx);
        nItem = m_lsView.InsertItem(nLoop, cstr);      
        m_lsView.SetItemData(nItem, dwNewIdx);
    }
    else
    {
        nItem = nLoop;
    }
    
    cstr = tConf.GetConfName();
    if (emRecStatePaused == emState)
    {
        cstr += "(暂停)";
    }
    m_lsView.SetItemText(nItem, 1, cstr);
    m_lsView.SetItemText(nItem, 2, tConf.GetRecName());
    cstr.Format("%u", tConf.GetTotalBitRate());
    m_lsView.SetItemText(nItem, 3, cstr);
    cstr.Format("%u", tRec.GetTotalStreamNum());
    m_lsView.SetItemText(nItem, 4, cstr);
}

void CSampleDlg::UpdateView(DWORD dwIdx)
{
    // 删除
    int nCount = m_lsView.GetItemCount();
    for (int nLoop = 0; nLoop < nCount; nLoop ++)
    {
        if (dwIdx == m_lsView.GetItemData(nLoop))
        {
            m_lsView.DeleteItem(nLoop);

            gs_Statis[dwIdx][0] = gs_Statis[dwIdx][1] = 0;
            gs_Statis[dwIdx][2] = gs_Statis[dwIdx][3] = 0;
            break;
        }
    }
}

void CSampleDlg::UpdateView(TStreamProperty &tStream)
{
    // 更新码流
    int nCount = m_lsView.GetItemCount();
    DWORD dwIdx = tStream.GetConfIndex();
    int nSubItem = 4 + tStream.GetStreamIndex();

    CString cstr;
    for (int nLoop = 0; nLoop < nCount; nLoop ++)
    {
        if (dwIdx == m_lsView.GetItemData(nLoop))
        {
            gs_Statis[dwIdx][tStream.GetStreamIndex()] ++;
            cstr.Format("%u(TS.%u)",  gs_Statis[dwIdx][tStream.GetStreamIndex()], tStream.GetFrameHead()->GetTStamp());
            m_lsView.SetItemText(nLoop, nSubItem, cstr);
            break;
        }
    }
}

void CSampleDlg::OnButton1() 
{
    /*
	// TODO: Add your control notification handler code here
	::SendMessage(g_hMain, WM_CONFUPDATE, 1, 12);

    TStreamProperty tStream;
    TFrameHead tHead;
    tStream.SetConfIndex(12);
    tStream.SetStreamIndex(2);
    tHead.SetTStamp(0x12345688);
    tStream.SetFrameHead(tHead);

    ::SendMessage(g_hMain, WM_STREAMUPDATE, (WPARAM)&tStream, NULL);
    */
}
