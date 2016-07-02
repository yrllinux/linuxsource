// ospserverdlg.cpp : implementation file
//

#include "stdafx.h"
#include "ospserver.h"
#include "ospserverdlg.h"
#include "direct.h"
#include "serverospapp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

s8	g_achServerBase[MAX_NAME_LEN];						//默认设置根目录D:\\tmp
s32	g_nServerPort = SERVER_LISTEN_PORT;					//默认设置服务器端口号6682

#ifdef __cplusplus
}
#endif  /* __cplusplus */

static void CB_SRecProgress(s8 chProgress);				//处理客户端上抛的消息，提供用户弹框消息

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
// COspserverDlg dialog

COspserverDlg::COspserverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COspserverDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COspserverDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COspserverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COspserverDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COspserverDlg, CDialog)
	//{{AFX_MSG_MAP(COspserverDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_OK, OnOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedDecgen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COspserverDlg message handlers

BOOL COspserverDlg::OnInitDialog()
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

	CB_ServerCallBack(CB_SRecProgress);						//注册回调函数

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	SetDlgItemText(IDC_EDIT3, BASEDOCUMENT);	//初始化第二个设置框，默认根目录D:\\tmp 
	SetDlgItemText(IDC_EDIT4, SERVERNODE);		//初始化第三个设置框，默认端口号6682
	SetDlgItemText(IDC_EDIT2, "服务器未初始化!");

	mkdir(BASEDOCUMENT);						//创建默认目录

	ZeroMemory(g_achServerBase, MAX_NAME_LEN);
	memcpy(g_achServerBase, BASEDOCUMENT, MAX_NAME_LEN);	//初始化为D:\\tmp

	g_zServerOspApp.CInitServer();							//初始化server
	g_zServerOspApp.CCreateServerTcpNode();					//连接结点并创建APP

	return TRUE;  
}

void COspserverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COspserverDlg::OnPaint() 
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
HCURSOR COspserverDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COspserverDlg::OnServerSpace() 
{

}

void COspserverDlg::OnSetBaseDocument() 
{
	
}

void COspserverDlg::OnBaseDocument() 
{
	
}

void COspserverDlg::OnCancel() 
{
	CDialog::OnCancel();
}

/*==========================================================
函数名 OnBnClickedDecgen
功能 ：服务器界面选择文件存放的目录
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void COspserverDlg::OnBnClickedDecgen() 
{
	s8 szPath[MAX_PATH];     //存放选择的目录路径 
    CString str;	
    ZeroMemory(szPath, sizeof(szPath));   
	
    BROWSEINFO bi;   
    bi.hwndOwner = m_hWnd;   
    bi.pidlRoot = NULL;   
    bi.pszDisplayName = szPath;   
    bi.lpszTitle = "请选择文件存放的目录：";   
    bi.ulFlags = 0;   
    bi.lpfn = NULL;   
    bi.lParam = 0;   
    bi.iImage = 0;   
    //弹出选择目录对话框
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);   
	
    if(lp && SHGetPathFromIDList(lp, szPath))   
    {
        str.Format("%s", szPath);
        SetDlgItemText(IDC_EDIT3, str);	//设置第二个设置框 		       
    }
    else 
	{
        MessageBox("无效的目录，请重新选择"); 
	}	
}

/*==========================================================
函数名 OnOK
功能 ：服务器界面设置后点击确定
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void COspserverDlg::OnOk() 
{
	CString strNum3;
	CString strNum4;
	
	GetDlgItemText(IDC_EDIT3, strNum3);											//对话框3设置根目录
	GetDlgItemText(IDC_EDIT4, strNum4);											//对话框4设置端口号
	
	strncpy(g_achServerBase, (LPCSTR)strNum3, MAX_NAME_LEN);					//转化CString to char/arrary
	g_nServerPort = _ttoi(strNum4);												//转化CString to unsigned long
	

	MessageBox("设置成功!");
	mkdir(g_achServerBase);														//创建目录							
	OspLog(LOGPRINTLEVELH, "\n根目录:%s, 端口号:%d\n", g_achServerBase, g_nServerPort);
	GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);		//关闭设置根目录框
	GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);		//关闭设置端口号框
	GetDlgItem(ID_OK)->EnableWindow(FALSE);			//关闭确定键

}

/////////////////回调函数用户实现部分:对参数进行处理////////////
static void CB_SRecProgress(s8 chProgress)					//处理客户端上抛的消息，提供用户弹框消息
{
	switch(chProgress)
	{
	case SERVER_INIT_OK:									//服务器初始化成功
		{
			SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd, IDC_EDIT2, "服务器初始化成功!");
		}
		break;
	case SERVER_INIT_FAIL:									//服务器初始化失败
		{
			SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd, IDC_EDIT2, "服务器初始化失败!");
		}
		break;
	case SERVER_NOSPACE:									//服务器空间不足
		{
			MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "服务器空间不足!", NULL, MB_OK);
		}
		break;
	default:
		{
			//不做处理
		}
		break;
	}
}

//end


