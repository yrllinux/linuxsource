// kdvmediavodDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kdvmediavodapp.h"
#include "kdvmediavoddlg.h"
#include "ServerConfigDlg.h"
#include "UserManageDlg.h"
// #include "kdvsys.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
#endif

extern int g_nLanguageType;
/////////////////////////////////////////////////////////////////////////////
// CKdvmediavodDlg dialog

CKdvmediavodDlg::CKdvmediavodDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKdvmediavodDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKdvmediavodDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKdvmediavodDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKdvmediavodDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKdvmediavodDlg, CDialog)
	//{{AFX_MSG_MAP(CKdvmediavodDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ENGLISH, OnBtnEnglish)
	ON_BN_CLICKED(IDC_BTN_CHINESE, OnBtnChinese)
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDC_USERMANAGE, OnUsermanage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKdvmediavodDlg message handlers

BOOL CKdvmediavodDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_nIsVodStart = 0;
	GetDlgItem(IDC_BTN_CHINESE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_ENGLISH)->ShowWindow(SW_SHOW);
	OnStartVod();
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKdvmediavodDlg::OnPaint() 
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
HCURSOR CKdvmediavodDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CKdvmediavodDlg::OnStartVod()
{
    m_nIsVodStart = vodstart();

    s8 buf[256];
    u8* pIP = (u8*)&g_tVodConfig.m_dwLocalIP;
    sprintf(buf, "%d.%d.%d.%d", pIP[0], pIP[1], pIP[2], pIP[3]);
    GetDlgItem(IDC_SERVERIP)->SetWindowText(buf);

    sprintf(buf, "%u", g_tVodConfig.m_wVodListenPort);
    GetDlgItem(IDC_SERVERPORT)->SetWindowText(buf);

    GetDlgItem(IDC_ROOTPATH)->SetWindowText(g_tVodConfig.m_achRootDir);
    if (m_nIsVodStart == 0)
    {
		switch( g_nLanguageType ) {
		case LANGUAGE_CHINESE:
			GetDlgItem(IDC_STATUS)->SetWindowText("流媒体点播服务器正在运行……");
			break;
		case LANGUAGE_ENGLISH:
			GetDlgItem(IDC_STATUS)->SetWindowText("The mediavod server is running...");
			break;
		default:
			break;
		}
        
    }
    else
    {
		switch(g_nLanguageType) {
		case LANGUAGE_CHINESE:
			GetDlgItem(IDC_STATUS)->SetWindowText("流媒体点播服务器启动失败");
			break;
		case LANGUAGE_ENGLISH:
			GetDlgItem(IDC_STATUS)->SetWindowText("Starting mediavod server unsuccess");
			break;
		default:
			break;
		}       
    }
}

void CKdvmediavodDlg::OnOK()
{
    vodend();
    CDialog::OnOK();
}

void CKdvmediavodDlg::OnCancel()
{
	switch(g_nLanguageType) {
	case LANGUAGE_CHINESE:
		GetDlgItem(IDC_STATUS)->SetWindowText("正在退出……");
		break;
	case LANGUAGE_ENGLISH:
		GetDlgItem(IDC_STATUS)->SetWindowText("Exiting now, please wait...");
		break;
	default:
		break;
	}    	
    vodend();
    CDialog::OnCancel();
}

void CKdvmediavodDlg::OnBtnEnglish() 
{
	// TODO: Add your control notification handler code here
	g_nLanguageType = LANGUAGE_ENGLISH;
	GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
    GetDlgItem(IDC_CONFIG)->SetWindowText("Config...");
	GetDlgItem(IDC_STATIC_SERVERIP)->SetWindowText("Server IP:");
	GetDlgItem(IDC_STATIC_SERVERPORT)->SetWindowText("Server Port:");
	GetDlgItem(IDC_STATIC_ROOTPATH)->SetWindowText("Root Path:");
    GetDlgItem(IDC_USERMANAGE)->SetWindowText("User...");
	if( 0 == m_nIsVodStart )
	{
		GetDlgItem(IDC_STATUS)->SetWindowText("The mediavod server is running...");
	}
	else
	{
		GetDlgItem(IDC_STATUS)->SetWindowText("Starting mediavod server unsuccess");
	}
	GetDlgItem(IDC_BTN_CHINESE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BTN_ENGLISH)->ShowWindow(SW_HIDE);
}

void CKdvmediavodDlg::OnBtnChinese() 
{
	// TODO: Add your control notification handler code here
	g_nLanguageType = LANGUAGE_CHINESE;
	GetDlgItem(IDCANCEL)->SetWindowText("退出");
    GetDlgItem(IDC_CONFIG)->SetWindowText("配置...");
	GetDlgItem(IDC_STATIC_SERVERIP)->SetWindowText("服务器IP：");
	GetDlgItem(IDC_STATIC_SERVERPORT)->SetWindowText("服务器端口：");
	GetDlgItem(IDC_STATIC_ROOTPATH)->SetWindowText("根路径：");
    GetDlgItem(IDC_USERMANAGE)->SetWindowText("用户管理...");
	if( 0 == m_nIsVodStart )
	{
		GetDlgItem(IDC_STATUS)->SetWindowText("流媒体点播服务器正在运行……");
	}
	else
	{
		GetDlgItem(IDC_STATUS)->SetWindowText("流媒体点播服务器启动失败");
	}
	GetDlgItem(IDC_BTN_CHINESE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_ENGLISH)->ShowWindow(SW_SHOW);
}


void CKdvmediavodDlg::OnConfig() 
{
	// TODO: Add your control notification handler code here
    CServerConfigDlg cConfigDlg;
	if ( IDOK == cConfigDlg.DoModal() )
    {
        memset( g_tVodConfig.m_achRootDir, 0, sizeof(g_tVodConfig.m_achRootDir) );
        strncpy( g_tVodConfig.m_achRootDir, (LPCTSTR)cConfigDlg.m_csFileSavePath, cConfigDlg.m_csFileSavePath.GetLength()+1 );
        g_tVodConfig.m_achRootDir[sizeof(g_tVodConfig.m_achRootDir)-1] = '\0';

        g_tVodConfig.m_dwLocalIP = htonl(cConfigDlg.m_dwServIp);
        g_tVodConfig.m_wMTUSize = cConfigDlg.m_unMtuSize;
        g_tVodConfig.m_wSendPort = cConfigDlg.m_unSendStartPort;
        g_tVodConfig.m_wVodListenPort = cConfigDlg.m_unServerPort;

        SaveConfig( &g_tVodConfig );
        if ( g_nLanguageType == LANGUAGE_CHINESE )
        {
            AfxMessageBox( "配置已修改, 需要重启才能生效!" );
        }
        else
        {
            AfxMessageBox( "Config changed, need reboot to make it useful!" );
        }
    }

    return;
}

void CKdvmediavodDlg::SaveConfig(TVodConfig* ptVodConfig)
{
    s8 szData[256];
    
    BOOL32 bRet;
    memset( szData, 0, sizeof(szData) );

    u32 dwIp = ntohl(ptVodConfig->m_dwLocalIP);
    sprintf( szData, "%d.%d.%d.%d", (dwIp>>24)&0xff, (dwIp>>16)&0xff, (dwIp>>8)&0xff, dwIp&0xff );
    bRet = SetRegKeyString(CONFIG_FILE_NAME,
                        "SERVERCONFIG",  
                        "SERVERIP", 
                        szData );

    if (!bRet)
    {
        OspPrintf( TRUE, FALSE, "[SaveConfig] Save local ip failed!\n" );
    }

    
    bRet = SetRegKeyInt(CONFIG_FILE_NAME,
                    "SERVERCONFIG",  
                    "SERVERPORT", 
                    ptVodConfig->m_wVodListenPort );
    if (!bRet)
    {
       OspPrintf( TRUE, FALSE, "[SaveConfig] Save listen port failed!\n" );
    }

    
    bRet = SetRegKeyInt(CONFIG_FILE_NAME,
                    "SERVERCONFIG",  
                    "SENDSTARTPORT", 
                    ptVodConfig->m_wSendPort );

    if (!bRet)
    {
       OspPrintf( TRUE, FALSE, "[SaveConfig] Save send port failed!\n" );
    }
    
    
    bRet = SetRegKeyString(CONFIG_FILE_NAME,
                        "SERVERCONFIG",  
                        "ROOTDIR", 
                        ptVodConfig->m_achRootDir );
    if (!bRet)
    {
        OspPrintf( TRUE, FALSE, "[SaveConfig] Save root directory failed!\n" );
    }
	
	// 增加从配置文件里读取MTU大小, zgc, 2007-04-02
    bRet = SetRegKeyInt(CONFIG_FILE_NAME,
                  "SERVERCONFIG",
                  "MTUSIZE",
				  ptVodConfig->m_wMTUSize);
	if( !bRet )
	{
        OspPrintf( TRUE, FALSE, "[SaveConfig] Save mtu size failed!\n" );
	}

    return;
}

void CKdvmediavodDlg::OnUsermanage() 
{
	// TODO: Add your control notification handler code here
	CUserManageDlg cUserManageDlg;
    if ( IDOK == cUserManageDlg.DoModal() )
    {
        ;
    }
}
