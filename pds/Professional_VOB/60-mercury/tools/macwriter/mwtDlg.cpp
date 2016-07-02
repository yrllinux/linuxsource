// mwtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mwt.h"
#include "mwtDlg.h"
#include "MacSerializer.h"
#include "ShowMacDlg.h"
#include "MacSNDlg.h"
#include "snlog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMacSerializer g_cSerializer;

const s8* GetRunPath()
{
    static s8 szPath[KDV_MAX_PATH] = {0};

    ::GetModuleFileName(::AfxGetInstanceHandle() , szPath, sizeof(szPath));

    CString cstrPath = szPath;
    cstrPath = cstrPath.Left( cstrPath.ReverseFind('\\') + 1 );

    memset( szPath, 0, sizeof(szPath) );
    strncpy( szPath, cstrPath, sizeof(szPath) - 1 );
    return szPath;
}

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
// CMwtDlg dialog

CMwtDlg::CMwtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMwtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMwtDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMwtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMwtDlg)
	DDX_Control(pDX, IDC_LS_MACSN, m_lsMacSn);
	DDX_Control(pDX, IDC_PROGRESS_WRITE, m_prgWrite);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMwtDlg, CDialog)
	//{{AFX_MSG_MAP(CMwtDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_MODIFYMAC, OnBtnModifymac)
	ON_BN_CLICKED(IDC_BTN_SNLIST, OnBtnSnlist)
	ON_BN_CLICKED(IDC_BTN_LOG, OnBtnLog)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnBtnRefresh)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SHOWMAC, OnBtnShowmac)
	ON_BN_CLICKED(IDC_CK_AUTO, OnCkAuto)
	ON_BN_CLICKED(IDC_BTN_ADDSN, OnBtnAddsn)
	ON_BN_CLICKED(IDC_BTN_DELSN, OnBtnDelsn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMwtDlg message handlers

BOOL CMwtDlg::OnInitDialog()
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
    CheckDlgButton(IDC_RD_DIFFFOLDER, BST_CHECKED);
    CheckDlgButton(IDC_RD_DIFFNAME,   BST_UNCHECKED);

    GetDlgItem(IDC_CK_FILL)->ShowWindow(SW_HIDE);
    CheckDlgButton(IDC_CK_FILL,  BST_UNCHECKED);

    LoadDefaultMacAddr();

    g_SNLogMsg.LoadHistorySNFromFile();

    SetDlgItemText( IDC_ET_MACADDR, m_cMacCurr.ToString() );
    SetDlgItemText( IDC_ET_MAC4SN, m_cMacCurr.ToString() );

    SetDlgItemInt( IDC_ET_TARGETNUM, 1, FALSE );

    SetDlgItemText( IDC_ET_FILENAME, "u-boot.bin" );
	
    // 初始化列表
    m_lsMacSn.InsertColumn( 0,  "序列号",  LVCFMT_CENTER, 160 );
    m_lsMacSn.InsertColumn( 1, "地址个数", LVCFMT_CENTER, 70);
    m_lsMacSn.InsertColumn( 2, "MAC地址0", LVCFMT_CENTER, 140 );
    m_lsMacSn.InsertColumn( 3, "MAC地址1", LVCFMT_CENTER, 140);
    
	m_lsMacSn.SetExtendedStyle(m_lsMacSn.GetExtendedStyle()
					   		   | LVS_EX_FULLROWSELECT );
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMwtDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMwtDlg::OnPaint() 
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
HCURSOR CMwtDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMwtDlg::OnBtnBrowse() 
{
    // 浏览文件
    OPENFILENAME ofn;      
    TCHAR szFile[KDV_MAX_PATH] = _T("");   
    TCHAR szFilter[256];

    _stprintf( szFilter, _T("%s(*.bin)|*.bin||"), "u-boot ROM 文件" );
    LPTSTR lptrFilter = szFilter;
    while ( ( lptrFilter = _tcschr( lptrFilter, '|' ) ) != NULL )
    {
        lptrFilter[0] = '\0';
        lptrFilter ++;
    }

    // Initialize OPENFILENAME
    ZeroMemory( &ofn, sizeof (ofn) );
    ofn.lStructSize = sizeof (ofn);
    ofn.hwndOwner = GetSafeHwnd();
    ofn.lpstrFile = szFile;

    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof (szFile);
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = GetRunPath();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    BOOL bRet = GetOpenFileName(&ofn);	

    if (bRet)
    {
		CString cstrName = szFile;
		SetDlgItemText(IDC_ET_FILENAME, cstrName);

        // 刷新
        OnBtnRefresh();        
    }    	
}

void CMwtDlg::OnBtnModifymac() 
{
	// 允许用户修改当前缺省Mac地址
    GetDlgItem(IDC_ET_MACADDR)->EnableWindow(TRUE);	
}


void CMwtDlg::OnBtnSnlist() 
{
    //配置日志列表	
    CMacSNDlg dlg;
    DWORD dwCount = GetDlgItemInt( IDC_ET_MACNUM );
    if (dwCount == 0)
    {
        dwCount = 1;
    }
    dlg.SetMacPerDevice( dwCount );
    dlg.SetCurrMac( m_cMacCurr );

    dlg.DoModal();
}

void CMwtDlg::OnBtnLog() 
{
    CSNLog dlg;
    dlg.DoModal();
}


void CMwtDlg::OnBtnRefresh() 
{
    // 读取指定文件，获取Mac地址	
    CString cstr;
    GetDlgItemText( IDC_ET_FILENAME, cstr );
    DWORD dwCount = 0;
    if (g_cSerializer.LoadUBootBin(cstr))
    {
        dwCount= g_cSerializer.GetMacCount();   
    }
    else
    {
        ShowErr( "指定的文件没有找到或不是合法的u-boot ROM 文件!" );
    }
    SetDlgItemInt( IDC_ET_MACNUM, dwCount, FALSE );	
}


void CMwtDlg::OnOK() 
{

    DWORD dwDeviceCount = GetDlgItemInt( IDC_ET_TARGETNUM, NULL, FALSE );

    ShowErr("正在分析原始文件...", FALSE);
    // 刷新
	OnBtnRefresh();

    if (g_cSerializer.GetMacCount() == 0)
    {
        // 不合法的文件
        ShowErr("原始文件不合法...", FALSE);
        return ;
    }    
    ShowErr("原始文件合法.", FALSE);
    
    if (dwDeviceCount == 0)
    {
        ShowErr("请指定需要写入的设备个数!");
        return;
    }

    // 获取用户指定的MAC地址
    CString cstrMac;
    GetDlgItemText(IDC_ET_MACADDR, cstrMac);
    CMacAddr cTmpMac;
    if ( !cTmpMac.Str2u48( cstrMac ) )
    {
        ShowErr("用户指定的MAC地址非法");
        return;
    }
    m_cMacCurr = cTmpMac;

    ShowErr("正在匹配序列号...", FALSE);
    // 检测日志配置是否正确
    BOOL bIsAuto = IsDlgButtonChecked(IDC_CK_AUTO);
    if (bIsAuto)
    {
        CString cstrSNBegin;
        GetDlgItemText( IDC_ET_SN, cstrSNBegin );
        if (cstrSNBegin.IsEmpty())
        {
            ShowErr("请输入起始序列号!", FALSE);
            return;
        }
        g_SNLogMsg.SetAutoInc( cstrSNBegin );
    }
    else
    {
        // 检查每个MAC是否配置了序列号
        g_SNLogMsg.SetNoAutoInc();
        CMacAddr cAddr = m_cMacCurr;

        DWORD dwTotalMacNum = g_cSerializer.GetMacCount() * dwDeviceCount;
        CString cstrTmp;
        SNItem item;
        for ( int i = 0; i < dwTotalMacNum; i ++)
        {
            if ( !g_SNLogMsg.GetMatchedSNbyMacInNew(cAddr, item) )
            {
                cstrTmp.Format("MAC地址 %s 还没有指定序列号，请首先为其分配序列号!", cAddr.ToString());
                ShowErr(cstrTmp);
                return;
            }

            cAddr ++;
        }
    }
    
    ShowErr("正在准备写入...", FALSE);
    m_prgWrite.SetRange32(0, dwDeviceCount);
    m_prgWrite.SetStep(1);

    // 自动填充
    g_cSerializer.SetFillFile( IsDlgButtonChecked(IDC_CK_FILL) );
    
    // 根据用户设置循环写入
    CString cstrFilename = GetRunPath();
    CString cstrTmp;
    // 文件名类型
    BYTE byFilenameType = IsDlgButtonChecked(IDC_RD_DIFFFOLDER) ? 1 : 2;
    cstrFilename += "Output";
    CreateDirectory(cstrFilename, NULL);

    CStdioFile cFileReport;
    CString cstrFilenameReport = cstrFilename + "\\" + FILENAME_REPORT;
    cFileReport.Open(cstrFilenameReport, CFile::modeCreate | CFile::modeReadWrite);

    time_t tNow = time(NULL);
    tm when = *localtime( &tNow );
    cstrTmp.Format("本次MAC地址写入是在 %s\n", asctime( &when ));
    cFileReport.WriteString(cstrTmp);

    SNItem item;
    CMacAddr cAddrTmp;
    for (DWORD i = 0; i < dwDeviceCount; i++)
    {
        cstrTmp.Format("正在写入 %s ...", m_cMacCurr.ToString());
        ShowErr(cstrTmp, FALSE);
        BOOL bSingle = (g_cSerializer.GetMacCount() == 1);
        if (!bSingle)
        {
            cAddrTmp = m_cMacCurr;
            cAddrTmp ++;
        }

        // 判断2个MAC地址是否均在范围内
        CString cstr, cstr1, cstr2, cstr3;
        if ( !m_cMacCurr.IsInRange( m_cMacMin, m_cMacMax ) )
        {            
            cstr1 = m_cMacCurr.ToString();
            cstr2 = m_cMacMin.ToString();
            cstr3 = m_cMacMax.ToString();
            cstr.Format("MAC 地址 %s 不在允许的范围%s --- %s内!", 
                cstr1, 
                cstr2, 
                cstr3);
            ShowErr(cstr);
            break;
        }
        if ( !bSingle && !cAddrTmp.IsInRange( m_cMacMin, m_cMacMax ) )
        {
            cstr1 = cAddrTmp.ToString();
            cstr2 = m_cMacMin.ToString();
            cstr3 = m_cMacMax.ToString();
            cstr.Format("MAC 地址 %s 不在允许的范围%s --- %s内!", 
                cstr1, 
                cstr2, 
                cstr3);
            ShowErr(cstr);
            break;

        }
        else             
        {
            if ( byFilenameType == 1 )
            {
                cstrFilename = GetRunPath();
                cstrTmp.Format("%.5d", i);
                cstrFilename += "Output\\";
                cstrFilename += cstrTmp;
                CreateDirectory(cstrFilename, NULL);

                cstrFilename += "\\u-boot.bin";
            }
            else
            {
                cstrFilename= GetRunPath();
                cstrFilename += "Output\\";
                cstrTmp.Format("u-boot_%.5d.bin", i);
                cstrFilename += cstrTmp;
            }

            // 写入
            if (!g_cSerializer.WriteMac2Bin( cstrFilename, m_cMacCurr, 0xFF ))
            {
                CString cstr;    
                cstr.Format("写入文件 %s 时发生错误!", cstrFilename);
                ShowErr(cstr);
                break;
            }

            // 写日志，记录时间            
            g_SNLogMsg.GetMatchedSNbyMacInNew(m_cMacCurr, item);

            // 写说明文件            
            cAddrTmp = item.atU48[0];
            cstrTmp.Format("%s<---->%s", item.cstrSN, cAddrTmp.ToString());
            if ( item.dwMACCount == 2)
            {
                cstrTmp += "\t";
                cAddrTmp = item.atU48[1];
                cstrTmp += cAddrTmp.ToString();
            }
            cstrTmp += "\r\n";
            cFileReport.WriteString(cstrTmp);

            // 写日志
            item.tTime = time(NULL);
            g_SNLogMsg.AddHistoryItem(item);

            m_cMacCurr ++;
            if ( !bSingle )
            {
                m_cMacCurr ++;
            }

            m_prgWrite.StepIt();
            
        }
    }
   
    // 更新日志到文件
    g_SNLogMsg.SaveHistorySN();

    SaveDefaultMacAddr();
       
    //更新Mac地址
    SetDlgItemText( IDC_ET_MACADDR, m_cMacCurr.ToString() );

    //读取并显示结果文件
    cFileReport.SeekToBegin();
    CString cstrFull;
    while (cFileReport.ReadString(cstrTmp))
    {
        cstrFull += cstrTmp;
        cstrFull += "\r\n";
    }
    cFileReport.Close();

    SetDlgItemText( IDC_ET_RESULT, cstrFull );

    ShowErr("所有文件写入完成。结果已经记录到日志。", FALSE);
    m_prgWrite.SetPos(0);

}

void CMwtDlg::OnCancel() 
{
	CDialog::OnCancel();
}


// 从配置文件里面读入
void CMwtDlg::LoadDefaultMacAddr()
{
    CString cstrName = GetRunPath() ;
    cstrName += FILENAME_CFG;

    // 读取范围
    s8 szValue[KDV_MAX_PATH];
	GetPrivateProfileString(_T("MAC"), _T("Min"), MAC_MIN,
                            szValue, KDV_MAX_PATH, cstrName);

    m_cMacMin = szValue;

	GetPrivateProfileString(_T("MAC"), _T("Max"), MAC_MAX,
                            szValue, KDV_MAX_PATH, cstrName);

    m_cMacMax = szValue;

	GetPrivateProfileString(_T("MAC"), _T("Current"), MAC_MIN,
                            szValue, KDV_MAX_PATH, cstrName);

    m_cMacCurr = szValue;    
}

// 写入到配置文件，目前只写当前值
void CMwtDlg::SaveDefaultMacAddr()
{
    CString cstrName = GetRunPath() ;
    cstrName += FILENAME_CFG;

    ::WritePrivateProfileString(_T("MAC"), _T("Current"), m_cMacCurr.ToString(), cstrName); 
}


void CMwtDlg::OnDestroy() 
{
    // 保存当前Mac地址
    SaveDefaultMacAddr();

    // 更新日志
    g_SNLogMsg.SaveHistorySN();

	CDialog::OnDestroy();
}

void CMwtDlg::OnBtnShowmac() 
{
	OnBtnRefresh();
    if ( g_cSerializer.GetMacCount() == 0)
    {
        return;
    }

    CString cstr;
    GetDlgItemText( IDC_ET_FILENAME, cstr );

    CShowMacDlg dlg;
    dlg.SetFilename( cstr );

    dlg.DoModal();	

    OnBtnRefresh();
}

void CMwtDlg::OnCkAuto() 
{
	BOOL bIsAuto = IsDlgButtonChecked(IDC_CK_AUTO);

    GetDlgItem(IDC_BTN_SNLIST)->EnableWindow(!bIsAuto);
	
}

void CMwtDlg::ShowErr(LPCTSTR lpszErr, BOOL bMsgBox)
{
    
    CWnd *pcStLable = GetDlgItem(IDC_ST_PROGRESS);
    if (lpszErr == NULL)
    {
        pcStLable->SetWindowText("");
        return;
    }

    pcStLable->SetWindowText(lpszErr);
    if (bMsgBox)
    {
        AfxMessageBox( lpszErr );
    }    
}

void CMwtDlg::OnBtnAddsn() 
{
    DWORD dwMacNum = GetDlgItemInt(IDC_ET_MACNUM, NULL, FALSE);
    if (dwMacNum == 0)
    {
        AfxMessageBox("请先指定合法的BIN文件!");
        return;
    }

    SNItem item;

    if (!ValidateInput(item))
    {
        return;
    }

    if ( g_SNLogMsg.IsSNExistInHistory(item) )
    {
        AfxMessageBox("该序列号在历史记录中已经被使用!");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return ;
    }

    if ( g_SNLogMsg.IsSNExistInNew(item) )
    {
        AfxMessageBox("该序列号本次已添加!");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return ;
    }

    g_SNLogMsg.AddNewItem(item);

    LoadMacSN();    

    // 自动增量
    CMacAddr cAddr;
    if ( dwMacNum == 1 )
    {
        cAddr = item.atU48[0];
    }
    else
    {
        cAddr = item.atU48[1];
    }
    cAddr++;    

    SetDlgItemText( IDC_ET_MAC4SN, cAddr.ToString() );

    GetDlgItem(IDC_ET_SN)->SetFocus();	
}

void CMwtDlg::OnBtnDelsn() 
{
    POSITION pos = m_lsMacSn.GetFirstSelectedItemPosition();

    if (pos == NULL)
    {
        AfxMessageBox("请先选择需要删除的条目!");
        return;
    }

    int nItem = m_lsMacSn.GetNextSelectedItem(pos);
    DWORD dwIndex = m_lsMacSn.GetItemData( nItem );

    if (dwIndex >= g_SNLogMsg.m_vtAddedSNItem.size())
    {
        return;
    }	

    g_SNLogMsg.m_vtAddedSNItem.erase( &g_SNLogMsg.m_vtAddedSNItem[dwIndex] );

    LoadMacSN();

    GetDlgItem(IDC_ET_SN)->SetFocus();
}

// 从数据结构载入到界面
void CMwtDlg::LoadMacSN()
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtAddedSNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtAddedSNItem.end();

    m_lsMacSn.DeleteAllItems();

    int i = 0;
    CString cstr;
    CMacAddr cAddr;
    while ( citr != citrEnd )
    {
        int nItem = m_lsMacSn.InsertItem(m_lsMacSn.GetItemCount(), citr->cstrSN );
        m_lsMacSn.SetItemData( nItem, i );
        
        cstr.Format("%u", citr->dwMACCount);            
        m_lsMacSn.SetItemText( nItem, 1, cstr );
        cAddr = citr->atU48[0];
        m_lsMacSn.SetItemText( nItem, 2, cAddr.ToString() );
        if ( citr->dwMACCount == 2)
        {
            cAddr = citr->atU48[1];
            m_lsMacSn.SetItemText( nItem, 3, cAddr.ToString() );
        }

        i++;
        citr++;
    }
}

// 验证界面用户输入
BOOL CMwtDlg::ValidateInput(SNItem &item)
{
    CString cstrSN;
    GetDlgItemText( IDC_ET_SN, cstrSN);

    CString cstr;
    CMacAddr cAddr1, cAddr2;
    GetDlgItemText(IDC_ET_MAC4SN, cstr);
    BOOL bValid1 = cAddr1.Str2u48(cstr);

    DWORD dwMacNum = GetDlgItemInt(IDC_ET_MACNUM, NULL, FALSE); 
    // 两个Mac
    BOOL bValid2 = TRUE;
    if ( dwMacNum == 2 )
    {
        cAddr2 = cAddr1;
        cAddr2 ++;
        bValid2 = TRUE;        
    }

    if ( cstrSN.IsEmpty() )
    {
        AfxMessageBox("序列号不能为空");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return FALSE;
    }
	
    if (!bValid1)
    {
        CString cstr2;
        GetDlgItemText(IDC_ET_MAC1, cstr);
        cstr2.Format("MAC 地址 %s 非法!", cstr);
        AfxMessageBox(cstr2);
        GetDlgItem(IDC_ET_MAC1)->SetFocus();
        return FALSE;
    }
/*
    if (!bValid2)
    {
        CString cstr2;
        GetDlgItemText(IDC_ET_MAC2, cstr);
        cstr2.Format("MAC 地址 %s 非法!", cstr);
        AfxMessageBox(cstr2);
        GetDlgItem(IDC_ET_MAC2)->SetFocus();
        return FALSE;
    }
*/
    
    item.cstrSN = cstrSN;
    item.dwMACCount = dwMacNum;
    item.atU48[0] = cAddr1.GetU48();
    if ( dwMacNum == 2 )
    {
        item.atU48[1] = cAddr2.GetU48();
    }
    return TRUE;
}
