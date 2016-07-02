// ServerConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kdvmediavodapp.h"
#include "kdvmediavod.h"
#include "ServerConfigDlg.h"
#include "kdvmediavodinstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
#endif

extern TVodConfig g_tVodConfig;
extern int g_nLanguageType;
/////////////////////////////////////////////////////////////////////////////
// CServerConfigDlg dialog


CServerConfigDlg::CServerConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerConfigDlg)
	m_unMtuSize = 0;
	m_csFileSavePath = _T("");
	m_unSendStartPort = 0;
	m_unServerPort = 0;
    m_dwServIp = 0;
	//}}AFX_DATA_INIT
}


void CServerConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerConfigDlg)
	DDX_Control(pDX, IDC_SERVER_IP, m_cServIp);
	DDX_Text(pDX, IDC_EDIT_MTUSIZE, m_unMtuSize);
	DDX_Text(pDX, IDC_EDIT_FILESAVEPATH, m_csFileSavePath);
	DDX_Text(pDX, IDC_EDIT_SENDSTARTPORT, m_unSendStartPort);
	DDV_MinMaxUInt(pDX, m_unSendStartPort, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_unServerPort);
	DDV_MinMaxUInt(pDX, m_unServerPort, 0, 65535);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CServerConfigDlg)
	ON_BN_CLICKED(IDC_SCANFILE, OnScanfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerConfigDlg message handlers



BOOL CServerConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if ( g_nLanguageType == LANGUAGE_CHINESE )
    {
        GetDlgItem(IDC_SERVERIP)->SetWindowText("服务器IP");
        GetDlgItem(IDC_SERVERPORT)->SetWindowText("服务器端口");
        GetDlgItem(IDC_SENDSTARTPORT)->SetWindowText("发送起始端口");
        GetDlgItem(IDC_FILESAVEPATH)->SetWindowText("根路径");
        GetDlgItem(IDC_MTUSIZE)->SetWindowText("MTU大小");
        GetDlgItem(IDC_SCANFILE)->SetWindowText("浏览...");
        GetDlgItem(IDOK)->SetWindowText("确定");
        GetDlgItem(IDCANCEL)->SetWindowText("取消");
    }
    else
    {
        GetDlgItem(IDC_SERVERIP)->SetWindowText("Server Ip");
        GetDlgItem(IDC_SERVERPORT)->SetWindowText("Server Port");
        GetDlgItem(IDC_SENDSTARTPORT)->SetWindowText("Send start port");
        GetDlgItem(IDC_FILESAVEPATH)->SetWindowText("Root path");
        GetDlgItem(IDC_MTUSIZE)->SetWindowText("MTU size");
        GetDlgItem(IDC_SCANFILE)->SetWindowText("File...");
        GetDlgItem(IDOK)->SetWindowText("Ok");
        GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
    }

    m_cServIp.SetAddress(ntohl(g_tVodConfig.m_dwLocalIP));
    m_unServerPort = g_tVodConfig.m_wVodListenPort;
    m_unSendStartPort = g_tVodConfig.m_wSendPort;
    m_unMtuSize = g_tVodConfig.m_wMTUSize;
    m_csFileSavePath = g_tVodConfig.m_achRootDir;
    
    UpdateData( FALSE );
    

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CServerConfigDlg::OnScanfile() 
{
	// TODO: Add your control notification handler code here
    char   Dir[256] = {0};   
    BROWSEINFO   bi;     
    ITEMIDLIST   *pidl;     
    
    bi.hwndOwner        =   NULL;     
    bi.pidlRoot         =   NULL;     
    bi.pszDisplayName   =   Dir;     
    bi.lpszTitle        =   "Select a directory";     
    bi.ulFlags          =   BIF_RETURNONLYFSDIRS|BIF_DONTGOBELOWDOMAIN;     
    bi.lpfn             =   NULL;     
    bi.lParam           =   0;     
    bi.iImage           =   0;     

    pidl = SHBrowseForFolder( &bi );     
    /*   Display   "Select   Folder"   dialog   box,   Get   the   folder   name   and   convert   it   into   a   ITEMLIST   data   structure.   */     
    if ( pidl == NULL )     
    {
        Dir[0] = '\0';
    }
    /*   Retrieve   foldernam   e   from   ITEMLIST   structure.   */     
    if ( !SHGetPathFromIDList( pidl, Dir ) )     
    {
        Dir[0] = '\0'; 
    }
    m_csFileSavePath = Dir;
    UpdateData( FALSE );

    LPMALLOC   ppMalloc   =   NULL;   
    if( SHGetMalloc( &ppMalloc ) == NOERROR )   
    {   
        ppMalloc->Free( pidl );   
    } 
}

void CServerConfigDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData( TRUE );
    m_cServIp.GetAddress( m_dwServIp );
	CDialog::OnOK();
}
