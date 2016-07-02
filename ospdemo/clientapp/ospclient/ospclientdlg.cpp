#include "stdafx.h"
#include "clientcommon.h"
#include "ospclient.h"
#include "ospclientdlg.h"
#include "clientospapp.h"

COspClientDlg *pCwindow = NULL;						//该文件内使用，传递类窗口属性
static void CB_UpdateProgress(s8 chProgress);		//处理客户端上抛的消息，提供用户弹框消息

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
// COspClientDlg dialog

COspClientDlg::COspClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COspClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COspClientDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COspClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COspClientDlg)
	DDX_Control(pDX, IDC_IPADDRESS2, m_pAdress);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COspClientDlg, CDialog)
	//{{AFX_MSG_MAP(COspClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_PROGRESS1, OnCSendFileProgress)
	ON_BN_CLICKED(IDC_BUTTON1, OnConnect)
	ON_BN_CLICKED(IDC_BUTTON2, OnDisconnect)
	ON_BN_CLICKED(IDC_BUTTON3, OnChooseFile)
	ON_BN_CLICKED(IDC_BUTTON4, OnDeleteFile)
	ON_BN_CLICKED(IDC_BUTTON5, OnSendFile)
	ON_BN_CLICKED(IDC_BUTTON6, OnPauseSendFile)
	ON_BN_CLICKED(IDC_BUTTON7, OnRestartSendFile)
	ON_BN_CLICKED(IDC_BUTTON8, OnCancel)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS2, OnFieldchangedIpaddress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COspClientDlg message handlers

BOOL COspClientDlg::OnInitDialog()
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

	SetDlgItemText(IDC_EDIT2, SERVERNODE);				//初始化第二个设置框，默认6682 
	SetDlgItemText(IDC_EDIT3, "当前尚未连接服务器!");	//初始化状态框

	CString strIP	= SERVERIP;							//设置IP控件的地址
	DWORD dwIP	=   inet_addr(strIP);
	BYTE *pIP   =   (BYTE*)&dwIP;
	m_pAdress.SetAddress(*pIP, *(pIP+1), *(pIP+2), *(pIP+3));

	m_progress.SetRange(0,100);							//初始化进度条范围
	m_progress.SetPos(0);								//进度条起始位置
	
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);		//启用连接按键
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新上传按键
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//关闭发送按键
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//关闭删除按键
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);		//关闭断开灰掉
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//关闭选择文件按键

	nFileOwnFlag = FALSE;								//列表中已经有文件标志
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COspClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COspClientDlg::OnPaint() 
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
HCURSOR COspClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COspClientDlg::OnCSendFileProgress(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void COspClientDlg::OnFieldchangedIpaddress(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void COspClientDlg::OnOK() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON1)->SetFocus();									//设置空间焦点，响应回车
	OnConnect();
}

void COspClientDlg::OnCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void COspClientDlg::OnConnect()												//连接
{	
	BYTE *pIP = NULL;
	CString strServerIp;
	DWORD dwIP = 0;
	m_pAdress.GetAddress(dwIP);
	pIP = (unsigned char*)&dwIP;
	strServerIp.Format("%u.%u.%u.%u",*(pIP+3), *(pIP+2), *(pIP+1), *pIP);	//转化为CString型

	CString strServerPort;
	GetDlgItemText(IDC_EDIT2, strServerPort);								//获取端口号
	CSetServerIpandPort((LPCSTR)strServerIp, _ttoi(strServerPort));			//获得设置的ip跟端口号

	pCwindow = (COspClientDlg*)this;										//注册当前窗口属性指针
	CB_CCallBack(CB_UpdateProgress);										//调用回调函数实时显示发送百分比以及状态

	//判断是否初始化
	if ( !CInitClient() )
	{
		SetDlgItemText(IDC_EDIT3, "连接到服务器失败!");	
		MessageBox("初始化失败!");	
		return;
	}
	
	//判断APP是否创建成功
	if ( !CCreatApp() )
	{
		SetDlgItemText(IDC_EDIT3, "连接到服务器失败!");		
		MessageBox("连接失败，请检查IP或网络!");
		return;
	}
	CConnectServer();
}

void COspClientDlg::OnDisconnect()											//断开连接
{
	CDisconnectServer();
}

void COspClientDlg::OnChooseFile()											//选择文件
{
	CFileDialog dlg( TRUE );
 	dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY;  
	dlg.m_ofn.lpstrFilter  =  _T("File(*.*)\0*.*\0\0");;
 	dlg.m_ofn.nMaxFile = 262;  
	
	//当前列表非空
	if( nFileOwnFlag )
	{
		MessageBox("请先删除在添加，一次只能显示一个文件!");
		return;
	}

	if( IDOK == dlg.DoModal() )
	{
		CString filename = dlg.GetFileName();
		m_List.AddString(filename);
		nFileOwnFlag = TRUE;
	}	
}

void COspClientDlg::OnDeleteFile()											//删除文件	
{
	m_List.DeleteString(0);
	CRemoveFile();
	CB_CToSProgress(C_SEND_FILE_TO_S_DELETFILE);
	nFileOwnFlag = FALSE;
}

void COspClientDlg::OnSendFile()											//发送文件
{
	s8 achFileName[MAX_PATH] = {0};
	m_List.GetText(0,achFileName);											//只获取列表第一个文件名
	
	u32 dwerrcode = access(achFileName, 0);
	if ( dwerrcode == -1 )													//确保目录不为空
	{		
		MessageBox("获取文件失败!");
		return;
	}

	//判断上传文件是否大于1G
	HANDLE hFile = CreateFile(achFileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

	DWORD dwFileFullSize = 0;
	if ( hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox("请重新选择文件上传!");
		return;
	}
	//文件超过4G
	if ( dwFileSizeHigh != 0)
	{
		dwFileFullSize = (dwFileSize + dwFileSizeHigh) / 1024 / 1024 + 4096;
	}
	else
	{	//文件小于4G
		dwFileFullSize = dwFileSize / 1024 / 1024;
	}

	if ( dwFileFullSize > 1024 )
	{
		MessageBox("上传文件过大，单文件请不要超过1G!");
		CloseHandle(hFile);
		return;
	}

	CloseHandle(hFile);
	OspPrintf(TRUE, FALSE, "上传的文件大小为：%ld M\n", dwFileFullSize);
	CUploadFile(achFileName, dwFileSize);
}

void COspClientDlg::OnPauseSendFile()										//发送暂停
{
	CPauseUploadFile();
}

void COspClientDlg::OnRestartSendFile()										//重新发送
{
	CRestartUploadFile();
}

/////////////////回调函数用户实现部分:对参数进行处理////////////
//函数CB_UpdateProgress由用户实现对参数的处理：
//该参数的取值为1-100以及上面的enum客户的状态
static void CB_UpdateProgress(s8 chProgress)								//处理客户端上抛的消息，提供用户弹框消息
{
	static s8 chTmp = 0;	
	if ( chProgress < 100 )
	{
		if ( chTmp != chProgress)
		{
			pCwindow->m_progress.SetStep(chProgress - chTmp);				//设置走动的距离
			chTmp = chProgress;												//记录上次变动值
			pCwindow->m_progress.StepIt();									//进度条走动
		}		
	}
	else
	{
		switch(chProgress)
		{
		case C_SEND_FILE_TO_S_SETERR:										//C->S设置ip有误
			{
				pCwindow->MessageBox("设置IP有误，请重新设置!");
			}
			break;
		case C_SEND_FILE_TO_S_UPLOADING:									//C->S发送文件中
			{	
				pCwindow->SetDlgItemText(IDC_EDIT3, "上传文件中，请稍候...!");
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);		//启用暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//启用断开按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//关闭删除按键
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//关闭发送按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新发送按键
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//关闭选择文件按键
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//关闭连接按键
			}
			break;
		case C_SEND_FILE_TO_S_OVER:											//C->S发送文件结束
			{
				pCwindow->MessageBox("发送完毕!");
				pCwindow->SetDlgItemText(IDC_EDIT3, "当前已经连接到服务器!");
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//启用发送按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//启用删除按键
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//启用选择文件按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//启用断开按键
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//关闭连接按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新发送按键
				pCwindow->m_progress.SetPos(0);								//进度条清零
				chTmp = 0;
			}
			break;
		case C_SEND_FILE_TO_S_FAIL:											//C->S发送文件失败
			{
				pCwindow->MessageBox("发送失败!");
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//启用发送按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//启用删除按键
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//启用选择文件按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//启用断开按键
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//关闭连接按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新发送按键
				pCwindow->m_progress.SetPos(0);								//进度条清零
				chTmp = 0;
			}
			break;
		case C_SEND_FILE_TO_S_BUSYNODEL:									//C->S发送文件时不能删除文件
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "正在上传，请暂停或上传结束后删除!");
			}
			break;
		case C_SEND_FILE_TO_S_PAUSE:										//C->S发送文件暂停中
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "上传暂停中，请选择恢复上传或删除文件!");
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//关闭连接按键
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//关闭发送按键
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//关闭选择文件按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//启用删除按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);		//启用重新上传按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//启用断开按键
			}
			break;
		case C_SEND_FILE_TO_S_NOPAUSE:										//C->S发送文件小于50M，不允许暂停
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "发送文件小于50M，不允许暂停!");
			}
			break;
		case C_SEND_FILE_TO_S_COEENCTING:									//提示正在连接，防止网络不畅，用于用户等待提示
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "正在连接到服务器, 请稍候...");
			}
			break;
		case C_SEND_FILE_TO_S_COEENCT:										//C->S已经建立连接状态
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "当前已经连接到服务器!");
				pCwindow->MessageBox("建立连接!");
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//关闭连接按键
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新上传按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//启用断开按键
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//启用发送按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//启用删除按键
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//启用选择文件按键
			}
			break;
		case C_SEND_FILE_TO_S_NACKCOEENCT:									//C->S连接失败状态
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "当前连接服务器失败!");
				pCwindow->MessageBox("请求连接失败!");
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);		//启用连接按键
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新上传按键
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//关闭发送按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//关闭删除按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);		//关闭断开灰掉
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//关闭选择文件按键
			}
			break;
		case C_SEND_FILE_TO_S_DICCOEENCT:									//C->S断开连接的状态显示
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "当前已经断开连接!");
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);		//启用连接按键
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新上传按键
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);		//关闭发送按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);		//关闭删除按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);		//关闭断开灰掉
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);		//关闭选择文件按键
				pCwindow->m_progress.SetPos(0);								//进度条清空
				chTmp = 0;
			}
			break;
		case C_SEND_FILE_TO_S_DELETFILE:									//C->S删除文件的状态显示
			{
				pCwindow->SetDlgItemText(IDC_EDIT3, "当前已经连接到服务器!");
				pCwindow->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);		//启用选择文件按键
				pCwindow->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);		//启用断开按键
				pCwindow->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);		//启用删除按键
				pCwindow->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);		//启用发送按键
				pCwindow->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);		//关闭连接按键
				pCwindow->GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);		//关闭暂停按键
				pCwindow->GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);		//关闭重新发送按键
				pCwindow->m_progress.SetPos(0);								//进度条清空
				chTmp = 0;
			}
			break;
		default:
			{
				//不做处理
			}
			break;
		}
	}
}

//end




