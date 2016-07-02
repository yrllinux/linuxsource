// licensedlgDlg.cpp : implementation file
//

#include "licensedlg.h"
#include "licensedlgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CLicenseGenApp	g_cLicenseGenApp;
#define KEDA_AES_KEY						"kedacom200606231"			// 保证16位
#define LEN_KEYFILE_CONT                    512
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
// CLicensedlgDlg dialog

CLicensedlgDlg::CLicensedlgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLicensedlgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicensedlgDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLicensedlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicensedlgDlg)
	DDX_Control(pDX, IDC_CHECK_DATE, m_ckDate);
	DDX_Control(pDX, IDC_DATE_EXPIREDATE, m_cExpireDate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLicensedlgDlg, CDialog)
	//{{AFX_MSG_MAP(CLicensedlgDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnGeneratelicense)
	ON_BN_CLICKED(IDC_BTN_IMPORT, OnBtnImport)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicensedlgDlg message handlers

BOOL CLicensedlgDlg::OnInitDialog()
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
	const int telnetport = 3600;
	OspInit( FALSE, telnetport );

    //Osp telnet 初始授权 [11/28/2006-zbq]
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
	
	// g_cLicenseGenApp.CreateApp("licensegen", AID_LICENSEGEN, 80, 20);
    SetDlgItemInt(IDC_EDIT_AUTHNUM, 0);

    CTime cStart = CTime::GetCurrentTime();
    CTime cEnd(2037, 12, 31, 0, 0, 0);
    m_cExpireDate.SetRange(&cStart, &cEnd);

    m_ckDate.SetCheck(TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLicensedlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLicensedlgDlg::OnPaint() 
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


void CLicensedlgDlg::GenEncodeData(u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect)
{
    s8 achIV[MAX_IV_SIZE] = {0};
	
    s8 achKey[32] = {0};
	strncpy( achKey, KEDA_AES_KEY, sizeof(KEDA_AES_KEY) );// 保持key为16的倍数
    u16 wKeyLen = strlen(achKey);

    s32 nRet;
	if( bDirect ) // eccrypt 
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_ENCRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}
	else  // decrypt
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_DECRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}

	return;
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLicensedlgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLicensedlgDlg::OnGeneratelicense() 
{	   
    CString cstrUserID;
    GetDlgItemText(IDC_EDIT_AUTHUSER, cstrUserID);
    CString cstrDeviceID;
    GetDlgItemText(IDC_EDIT_DEVICEID, cstrDeviceID);
    CTime cTime;
    m_cExpireDate.GetTime(cTime);
    int nNum = GetDlgItemInt(IDC_EDIT_AUTHNUM);

    if (nNum <= 0 || nNum > MAXNUM_MCU_MT)
    {
        AfxMessageBox("错误的接入数，请重新输入!");
        return;
    }
    
    // 不允许 |
    if (cstrUserID.IsEmpty() ||
        cstrUserID.SpanExcluding("|") != cstrUserID)
    {
        AfxMessageBox("用户 ID 为空或者含有非法字符，请重新输入!");
        return;
    }
    
    if (cstrDeviceID.IsEmpty())
    {
        AfxMessageBox("MCU 设备 ID 非法，请重新输入!");
        return;        
    }

    time_t timeData = cTime.GetTime();
    TKdvTime tDate;
    if (m_ckDate.GetCheck())
    {
        tDate.SetTime(&timeData);
    }    

    TLicenseParam tlicenseparam;
	tlicenseparam.SetAuthName( cstrUserID );
    tlicenseparam.SetDeviceID(cstrDeviceID);
	tlicenseparam.SetAuthNum( nNum );
    tlicenseparam.SetExpireDate( tDate );

    if (GenLicense(tlicenseparam))
    {
        AfxMessageBox("License 文件已经成功生成到当前文件夹!");
    }
    else
    {
        AfxMessageBox("License 文件生成错误!");
    }

	return;

}


BOOL CLicensedlgDlg::GenLicense(const TLicenseParam &tLicense)
{   
    u8 achEncText[LEN_KEYFILE_CONT] = {0};

	// 加密原始数据中用'|'分隔    

    s8 achSequenceStr[1024];
	sprintf(achSequenceStr, "%s|%s|%d|%d|%d|%d", 
            tLicense.GetAuthName(), 
            tLicense.GetDeviceID(),
            tLicense.GetAuthNum(),
            tLicense.GetExpireDate().GetYear(),
            tLicense.GetExpireDate().GetMonth(),
            tLicense.GetExpireDate().GetDay());
    
	u16 wSequenceStrLen = strlen(achSequenceStr);
	
	while( 0 != wSequenceStrLen % 16 )  // 补足16的倍数
	{
		strcat(achSequenceStr, "|");
		wSequenceStrLen += 1;
	}

    s8 achKeyFileName[KDV_MAX_PATH] = {0};
    sprintf(achKeyFileName, "%s", KEY_FILENAME);
	FILE *pHandler = fopen( achKeyFileName, "wb" );
    
	if( NULL == pHandler )
	{
		return FALSE;
	}
	
	GenEncodeData( achEncText, wSequenceStrLen, (u8*)achSequenceStr, TRUE);

	s8 szCopyright[128];
	memset( szCopyright, 0 ,sizeof(szCopyright) );
	memcpy( szCopyright, KEDA_COPYRIGHT, strlen(KEDA_COPYRIGHT) );
	
	fwrite( szCopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );
	
	u16 wEncTextLen = htons(LEN_KEYFILE_CONT);
	fwrite( &wEncTextLen, sizeof(wEncTextLen), 1, pHandler );	
    
	fwrite( achEncText, sizeof(achEncText), 1, pHandler );
	
	fwrite( szCopyright, sizeof(KEDA_COPYRIGHT), 1, pHandler );
	
	memset( szCopyright, 0, sizeof(szCopyright) );
	strcpy( szCopyright, ". Copyright 2000 - 2006." );

	fwrite( szCopyright, strlen( szCopyright ), 1, pHandler );

	fclose( pHandler );
	pHandler = NULL;		
    
    return TRUE;
}

void CLicensedlgDlg::OnBtnImport() 
{
    u8 achEncText[LEN_KEYFILE_CONT] = {0};

    //打开文件对话框
    OPENFILENAME ofn;
    TCHAR lpstrFilename[KDV_MAX_PATH] = _T("");

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);  
    ofn.hwndOwner = GetSafeHwnd();
    ofn.lpstrFile = lpstrFilename;
    ofn.nMaxFile = KDV_MAX_PATH;
    ofn.lpstrFilter = _T("授权文件(*.key)\0*.key\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    BOOL32 bRet = GetOpenFileName(&ofn);    
    
    if (!bRet)
    {
        return;
    }
    
	FILE *pHandler = fopen( lpstrFilename, "rb" );
	if( NULL == pHandler )
	{
		AfxMessageBox("打开文件失败!");
		return;
	}
	
	fseek(pHandler, 0, SEEK_SET);

    s8 szCopyright[128] = {0};
	fread( szCopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );

    if ( strncmp(szCopyright, KEDA_COPYRIGHT, strlen(KEDA_COPYRIGHT) ) != 0 )
    {
        AfxMessageBox("文件格式错误!请选择正确的授权文件!");
        return;
    }
	
	u16 wEncTextLen = 0;
	fread( &wEncTextLen, sizeof(wEncTextLen), 1, pHandler );    // 这里读出来应该是 LEN_KEYFILE_CONT
    wEncTextLen = ntohs(wEncTextLen);
	if (wEncTextLen > LEN_KEYFILE_CONT)
    {
        // 如果大于LEN_KEYFILE_CONT，忽略后面的内容
        wEncTextLen = LEN_KEYFILE_CONT;
    }

	fread( achEncText, wEncTextLen, 1, pHandler );

	fclose( pHandler );
	pHandler = NULL;
	
    s8 achDecText[LEN_KEYFILE_CONT] = {0};
	GenEncodeData( (u8*)achDecText, wEncTextLen, achEncText, FALSE );

	s8      achSeps[] = "|"; 

    CString cstrUserID;
    CString cstrDeviceID;
    int     nNum;

	s8 *pchToken = strtok( achDecText, achSeps );
	if( NULL == pchToken )
	{
        AfxMessageBox("文件格式错误!请选择正确的授权文件!");
        return;
	}

    cstrUserID = pchToken;    	

	pchToken = strtok( NULL, achSeps );

	if( NULL == pchToken )
	{
		AfxMessageBox("文件格式错误!请选择正确的授权文件!");
		return;
	}

    cstrDeviceID = pchToken;

    pchToken = strtok( NULL, achSeps );

	if( NULL == pchToken )
	{
		AfxMessageBox("文件格式错误!请选择正确的授权文件!");
		return;
	}

    nNum = atoi( pchToken );
    if (nNum <= 0 || nNum > MAXNUM_MCU_MT)
    {
		AfxMessageBox("文件格式错误!请选择正确的授权文件!");
		return;
    }
    
    pchToken = strtok( NULL, achSeps );
    if (NULL != pchToken)
    {
        TKdvTime tDate;
        tDate.SetYear(atoi(pchToken));        
        pchToken = strtok( NULL, achSeps );
        if ( NULL != pchToken )
        {
            tDate.SetMonth(atoi(pchToken));
            pchToken = strtok( NULL, achSeps );
            if (NULL != pchToken)
            {
                tDate.SetDay(atoi(pchToken));
            }            
        } 

        time_t timeDate;
        tDate.GetTime(timeDate);
        CTime cDate(timeDate);
        m_cExpireDate.SetTime(&cDate);

        m_ckDate.SetCheck(tDate.GetYear() != 0);
        OnCheckDate();
    }

    SetDlgItemText( IDC_EDIT_AUTHUSER, cstrUserID );
    SetDlgItemText( IDC_EDIT_DEVICEID, cstrDeviceID );
    SetDlgItemInt ( IDC_EDIT_AUTHNUM,  nNum );
    
}

void CLicensedlgDlg::OnCheckDate() 
{
    m_cExpireDate.EnableWindow(m_ckDate.GetCheck());
}
