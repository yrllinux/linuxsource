// serial2wifiDlg.cpp : implementation file
//

#include "stdafx.h"
#include <winsock2.h>
#include "serial2wifi.h"
#include "serial2wifiDlg.h"

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPCWIFIDlg *pCwindow = NULL;	
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
// CPCWIFIDlg dialog

CPCWIFIDlg::CPCWIFIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPCWIFIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPCWIFIDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCWIFIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPCWIFIDlg)
	DDX_Control(pDX, IDC_COMBO2, m_type);
	DDX_Control(pDX, IDC_COMBO1, m_mode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPCWIFIDlg, CDialog)
	//{{AFX_MSG_MAP(CPCWIFIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_EDITCHANGE(IDC_COMBO1, OnEditchangeCombo1)
	ON_CBN_EDITCHANGE(IDC_COMBO2, OnEditchangeCombo2)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, OnFieldchangedIpaddress1)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit2)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, OnChangeEdit4)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPCWIFIDlg message handlers

BOOL CPCWIFIDlg::OnInitDialog()
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
	m_mode.InsertString(0, _T("Server"));
	m_mode.InsertString(1, _T("Client"));
	m_mode.SetCurSel(0);

	m_type.InsertString(0, _T("TCP"));
	m_type.InsertString(1, _T("UDP"));
	m_type.SetCurSel(0);

	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
	SetDlgItemText(IDC_IPADDRESS1, "");

	SetDlgItemText(IDC_EDIT2, "5678");
	SetDlgItemText(IDC_EDIT4, "Hello, I'm server!");

	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);

	pCwindow = (CPCWIFIDlg*)this;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPCWIFIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPCWIFIDlg::OnPaint() 
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
HCURSOR CPCWIFIDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

typedef struct s_serial2wifi
{
	CString mode;
	CString type;
	CString ip;
	CString port;
	CString reply;
	SOCKET	fd;
	int		flag;
	HANDLE	hThread;
	DWORD	ThreadID;  
}serial2wifi_t;

serial2wifi_t g_serial2wifi;

void CPCWIFIDlg::OnOK() 
{
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);

	GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);

	// TODO: Add extra validation here
	int nIndex = m_mode.GetCurSel();
	m_mode.GetLBText( nIndex, g_serial2wifi.mode );

	CString cur_type;
	nIndex = m_type.GetCurSel();
	m_type.GetLBText( nIndex, g_serial2wifi.type );

	GetDlgItemText(IDC_IPADDRESS1, g_serial2wifi.ip);
	GetDlgItemText(IDC_EDIT2, g_serial2wifi.port);
	GetDlgItemText(IDC_EDIT4, g_serial2wifi.reply);

	//char buf[256] = {0};
	//sprintf(buf, "%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n", g_serial2wifi.mode, g_serial2wifi.type, g_serial2wifi.ip, g_serial2wifi.port, g_serial2wifi.reply);
	//SetDlgItemText(IDC_EDIT3, buf);
	g_serial2wifi.flag = 1;

	if(g_serial2wifi.mode.Compare("Client") == 0)
	{
		if(g_serial2wifi.type.Compare("UDP") == 0)
			AfxBeginThread(OnStartUdpClient,NULL,THREAD_PRIORITY_NORMAL,0,0,NULL);
		else
			AfxBeginThread(OnStartTcpClient,NULL,THREAD_PRIORITY_NORMAL,0,0,NULL);
	}
	else
	{	
		if(g_serial2wifi.type.Compare("UDP") == 0)
			AfxBeginThread(OnStartUdpServer,NULL,THREAD_PRIORITY_NORMAL,0,0,NULL);
		else
			AfxBeginThread(OnStartTcpServer,NULL,THREAD_PRIORITY_NORMAL,0,0,NULL);
	}

	//CDialog::OnOK();
}

void CPCWIFIDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CPCWIFIDlg::OnEditchangeCombo1() 
{
	// TODO: Add your control notification handler code here
}

void CPCWIFIDlg::OnEditchangeCombo2() 
{
	// TODO: Add your control notification handler code here
	
}

void CPCWIFIDlg::OnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CPCWIFIDlg::OnChangeEdit2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CPCWIFIDlg::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	CString cur;
	int nIndex = m_mode.GetCurSel();
	m_mode.GetLBText( nIndex, cur );
	if(cur.Compare("Server") == 0)
	{
		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
		SetDlgItemText(IDC_IPADDRESS1, "");

		SetDlgItemText(IDC_EDIT4, "Hello, I'm server!");
		GetDlgItem(IDC_STATIC1)->SetWindowText("应答内容：");
	}
	else
	{
		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);	
		SetDlgItemText(IDC_IPADDRESS1, "192.168.1.1");

		SetDlgItemText(IDC_EDIT4, "Hello, I'm client!");
		GetDlgItem(IDC_STATIC1)->SetWindowText("发送内容：");
	}
}

void CPCWIFIDlg::OnChangeEdit3() 
{
 	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
}

void CPCWIFIDlg::OnChangeEdit4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CPCWIFIDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	g_serial2wifi.flag = 0;

	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
	SetDlgItemText(IDC_EDIT3, "");
	OnSelchangeCombo1();
}

UINT CPCWIFIDlg::OnStartUdpServer(LPVOID lpParam)
{
    struct sockaddr_in serAddr;
    struct sockaddr_in remoteAddr;
	int nAddrLen = 0;
	SOCKET serSocket = -1;
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2,2);
    if(WSAStartup(sockVersion, &wsaData) != 0)
    {
		pCwindow->OnButton1(); 
        return -1;
    }

	if(_ttoi(g_serial2wifi.port) <= 0 || _ttoi(g_serial2wifi.port) >= 65535)
	{
		pCwindow->MessageBox("端口（port）输入有误!");
		pCwindow->OnButton1(); 
		return -1;
	}

    serSocket = socket(AF_INET, SOCK_DGRAM, 0); 
    if(serSocket == INVALID_SOCKET)
    {
		pCwindow->MessageBox("socket error!");
		pCwindow->OnButton1(); 
        return -1;
    }

    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(_ttoi(g_serial2wifi.port));
    serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    if(bind(serSocket, (struct sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
 		pCwindow->MessageBox("bind error!");
        closesocket(serSocket);
		pCwindow->OnButton1(); 
        return -1;
    }

    pCwindow->SetDlgItemText(IDC_EDIT3, "等待连接...\r\n");

	CString str;
	char *sendData = (char *)(LPCTSTR)g_serial2wifi.reply;
	nAddrLen = sizeof(remoteAddr); 

	char recvbuf[146000] = {0}; 
	int count = 0;
    while (g_serial2wifi.flag)
    {
        char recvData[1460] = {0};
        int ret = recvfrom(serSocket, recvData, sizeof(recvData), 0, (struct sockaddr *)&remoteAddr, &nAddrLen);
        if (ret > 0)
        {
  			pCwindow->GetDlgItemText(IDC_EDIT3, str);

  			count += ret + str.GetLength();
			if(count >= sizeof(recvbuf))
				memset(recvbuf, 0, sizeof(recvbuf));

			recvData[ret] = 0x00;

			sprintf(recvbuf, "%s收到一个数据报文：[%s]：%s\r\n", str, inet_ntoa(remoteAddr.sin_addr), recvData);
		    pCwindow->SetDlgItemText(IDC_EDIT3, recvbuf);

			//滑动条自动到最下面
			((CEdit *)pCwindow->GetDlgItem(IDC_EDIT3))->SetSel(-1);

			sendto(serSocket, sendData, strlen(sendData), 0, (struct sockaddr *)&remoteAddr, nAddrLen);    
        }
    }

    closesocket(serSocket); 
    WSACleanup();
	pCwindow->SetDlgItemText(IDC_EDIT3, "");

    return 0;
}

UINT CPCWIFIDlg::OnStartUdpClient(LPVOID lpParam)
{
	pCwindow->SetDlgItemText(IDC_IPADDRESS1, g_serial2wifi.ip);

	struct sockaddr_in sin;	
	SOCKET sclient = -1;
	int len = 0;
	char *sendData = NULL;
	char recvData[1460] = {0};    
	int ret = 0;
	CString str;
	
    WORD socketVersion = MAKEWORD(2,2);
    WSADATA wsaData; 
    if(WSAStartup(socketVersion, &wsaData) != 0)
    {
		pCwindow->OnButton1(); 
        return -1;
    }
    
	if(_ttoi(g_serial2wifi.port) <= 0 || _ttoi(g_serial2wifi.port) >= 65535)
	{
		pCwindow->MessageBox("端口（port）输入有误!");
		pCwindow->OnButton1(); 
		return -1;
	}

	char *p = (char *)(LPCTSTR)g_serial2wifi.ip;
	//pCwindow->MessageBox(p);

	if(g_serial2wifi.ip.Compare("0.0.0.0") == 0 || 
		g_serial2wifi.ip.Compare("255.255.255.255") == 0 || 
		p[0] == '0')
	{
		pCwindow->MessageBox("地址（ip）输入有误!");
		pCwindow->OnButton1(); 
		return -1;
	}

	sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(_ttoi(g_serial2wifi.port));
    sin.sin_addr.S_un.S_addr = inet_addr(g_serial2wifi.ip);
    len = sizeof(sin);
    
    sendData = (char *)(LPCTSTR)g_serial2wifi.reply;
	sendto(sclient, sendData, strlen(sendData), 0, (struct sockaddr *)&sin, len);

	//等待1秒
	struct timeval tv_out;
	tv_out.tv_sec = 1;
	tv_out.tv_usec = 0;
	setsockopt(sclient, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_out, sizeof(tv_out));

	char recvbuf[146000] = {0}; 
	int count = 0;
	while(g_serial2wifi.flag)
	{
		ret = recvfrom(sclient, recvData, sizeof(recvData), 0, (struct sockaddr *)&sin, &len);
		if(ret > 0)
		{
			pCwindow->GetDlgItemText(IDC_EDIT3, str);

  			count += ret + str.GetLength();
			if(count >= sizeof(recvbuf))
				memset(recvbuf, 0, sizeof(recvbuf));

			recvData[ret] = 0x00;

			sprintf(recvbuf, "%s%s\r\n", str, recvData);
		    pCwindow->SetDlgItemText(IDC_EDIT3, recvbuf);

			//滑动条自动到最下面
			((CEdit *)pCwindow->GetDlgItem(IDC_EDIT3))->SetSel(-1);
		}
		
		Sleep(1000);
		sendto(sclient, sendData, strlen(sendData), 0, (struct sockaddr *)&sin, len);
	}
	  
    closesocket(sclient);
    WSACleanup();
	pCwindow->SetDlgItemText(IDC_EDIT3, "");

	return 0;
}

UINT CPCWIFIDlg::OnStartTcpServer(LPVOID lpParam)
{
   	SOCKET sClient = -1;
   	SOCKET slisten = -1;
   	struct sockaddr_in sin;
    struct sockaddr_in remoteAddr;
    int nAddrlen = 0;
    char recvData[1460] = {0}; 
  	char *sendData = (char *)(LPCTSTR)g_serial2wifi.reply;
	CString str;
	
    //初始化WSA
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
 		pCwindow->OnButton1();
		return -1;
    }

	if(_ttoi(g_serial2wifi.port) <= 0 || _ttoi(g_serial2wifi.port) >= 65535)
	{
		pCwindow->MessageBox("端口（port）输入有误!");
		pCwindow->OnButton1(); 
		return -1;
	}

    //创建套接字
    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SOCKET)
    {
		pCwindow->OnButton1();
		pCwindow->MessageBox("socket error!");
        return -1;
    }

	int optval = -1; 
    int optlen = -1; 
 
	//设置地址和端口号可以重复使用 
    optval = 1;  
    optlen = sizeof(optval);
    setsockopt(slisten, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, optlen);

    //绑定IP和端口
    //sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(_ttoi(g_serial2wifi.port));
    sin.sin_addr.S_un.S_addr = INADDR_ANY; 
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
		closesocket(slisten);
		pCwindow->OnButton1();
		pCwindow->MessageBox("bind error!");
		return -1;
    }

    //开始监听
    if(listen(slisten, 32) == SOCKET_ERROR)
    {
		closesocket(slisten);
 		pCwindow->OnButton1();
		pCwindow->MessageBox("listen error!");
		return -1;
    }

    //循环接收数据
    nAddrlen = sizeof(remoteAddr);
	pCwindow->SetDlgItemText(IDC_EDIT3, "等待连接...\r\n");
    
	char recvbuf[146000] = {0}; 

	fd_set readfds;
	FD_ZERO(&readfds);
	/*将服务器socket加入到集合中*/
	FD_SET(slisten, &readfds);

	int count = 0;
	while(g_serial2wifi.flag)   
	{  			   
		/*无限期阻塞，并测试文件描述符变动 */  
		int result = select(FD_SETSIZE, &readfds, (fd_set *)0, (fd_set *)0, (struct timeval *) 0);	
		if(result < 1)	 
		{	
			perror("select");	 
			continue;   
		}	
	
		SOCKET i = 0;
		for(; i < readfds.fd_count; i++)
		{  
			/*找到相关文件描述符*/	
			if(FD_ISSET(readfds.fd_array[i], &readfds))
			{	
				if(readfds.fd_array[i] == slisten)
				{	
					SOCKET tmpfd = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);

					sprintf(recvbuf, "收到一个连接：%s\r\n", inet_ntoa(remoteAddr.sin_addr));
					pCwindow->SetDlgItemText(IDC_EDIT3, recvbuf);

					/*将客户端socket加入到集合中*/
					FD_SET(tmpfd, &readfds);
					printf("adding client on fd %d\n", tmpfd);
				}	
				else
				{
					/*客户端socket中有数据请求时*/
					int ret = recv(readfds.fd_array[i], recvData, sizeof(recvData), 0);
					if(ret == 0)
					{
						closesocket(readfds.fd_array[i]);
						FD_CLR(readfds.fd_array[i], &readfds);
						continue;
					}
					
					if(ret > 0)
					{
						pCwindow->GetDlgItemText(IDC_EDIT3, str);

  						count += ret + str.GetLength();
						if(count >= sizeof(recvbuf))
							memset(recvbuf, 0, sizeof(recvbuf));

						recvData[ret] = 0x00;

						sprintf(recvbuf, "%s%s\r\n", str, recvData);
						pCwindow->SetDlgItemText(IDC_EDIT3, recvbuf);

						//滑动条自动到最下面
						((CEdit *)pCwindow->GetDlgItem(IDC_EDIT3))->SetSel(-1);

						send(readfds.fd_array[i], sendData, strlen(sendData), 0);
					}
				}	
			}	
		}	
	} 
    
    closesocket(sClient);
    closesocket(slisten);
    WSACleanup();
 	pCwindow->SetDlgItemText(IDC_EDIT3, "");

	return 0;
}

UINT CPCWIFIDlg::OnStartTcpClient(LPVOID lpParam)
{
	pCwindow->SetDlgItemText(IDC_IPADDRESS1, g_serial2wifi.ip);

	SOCKET sclient = -1;
  	struct sockaddr_in serAddr;
  	char *sendData = NULL;
	char recData[1460] = {0};
	int ret;
	CString str;
			   
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data; 
    if(WSAStartup(sockVersion, &data) != 0)
    {
  		pCwindow->OnButton1();
		return -1;
    }

	if(_ttoi(g_serial2wifi.port) <= 0 || _ttoi(g_serial2wifi.port) >= 65535)
	{
		pCwindow->MessageBox("端口（port）输入有误!");
		pCwindow->OnButton1(); 
		return -1;
	}

	char *p = (char *)(LPCTSTR)g_serial2wifi.ip;
	//pCwindow->MessageBox(p);

	if(g_serial2wifi.ip.Compare("0.0.0.0") == 0 || 
		g_serial2wifi.ip.Compare("255.255.255.255") == 0 || 
		p[0] == '0')
	{
		pCwindow->MessageBox("地址（ip）输入有误!");
		pCwindow->OnButton1(); 
		return -1;
	}

RESTART:

    sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sclient == INVALID_SOCKET)
    {
   		pCwindow->OnButton1();
		pCwindow->MessageBox("invalid socket !");
        return -1;
    }

	serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(_ttoi(g_serial2wifi.port));
    serAddr.sin_addr.S_un.S_addr = inet_addr(g_serial2wifi.ip); 
    while (connect(sclient, (struct sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
		Sleep(1000);
     	//pCwindow->OnButton1();
		//pCwindow->MessageBox("connect error !");
        //closesocket(sclient);
        //return -1;
    }
    
    sendData =  (char *)(LPCTSTR)g_serial2wifi.reply;
    send(sclient, sendData, strlen(sendData), 0);

	char recvbuf[146000] = {0}; 
	int count = 0;
	while(g_serial2wifi.flag)
	{
		ret = recv(sclient, recData, sizeof(recData), 0);
		if(ret > 0)
		{
			pCwindow->GetDlgItemText(IDC_EDIT3, str);

  			count += ret + str.GetLength();
			if(count >= sizeof(recvbuf))
				memset(recvbuf, 0, sizeof(recvbuf));

			recData[ret] = 0x00;

			sprintf(recvbuf, "%s%s\r\n", str, recData);
		    pCwindow->SetDlgItemText(IDC_EDIT3, recvbuf);

			//滑动条自动到最下面
			((CEdit *)pCwindow->GetDlgItem(IDC_EDIT3))->SetSel(-1);
		}

		Sleep(1000);
		int sendlen = send(sclient, sendData, strlen(sendData), 0);
		if (sendlen == -1)
		{
			//perror("send fail");
			closesocket(sclient);
			sclient = -1;
			goto RESTART;
		}
	}
		
    closesocket(sclient);
    WSACleanup();
 	pCwindow->SetDlgItemText(IDC_EDIT3, "");

	return 0;
}
