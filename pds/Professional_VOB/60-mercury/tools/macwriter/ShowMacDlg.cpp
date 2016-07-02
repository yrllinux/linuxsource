// ShowMacDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mwt.h"
#include "ShowMacDlg.h"
#include "MacSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowMacDlg dialog

const s8* GetRunPath();

CShowMacDlg::CShowMacDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowMacDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowMacDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    memset((void*)szFileName, 0, sizeof(szFileName));
}


void CShowMacDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowMacDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowMacDlg, CDialog)
	//{{AFX_MSG_MAP(CShowMacDlg)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_MODIFY, OnBtnModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowMacDlg message handlers

void CShowMacDlg::OnBtnBrowse() 
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
        // 刷新
        Refresh(cstrName);        
    }  
	
}

void CShowMacDlg::Refresh(LPCTSTR szFilename)
{
    SetDlgItemText(IDC_ET_FILENAME, szFilename);

    // 读取指定文件，获取Mac地址	
    CString cstr = szFilename;

    if (!g_cSerializer.LoadUBootBin(cstr))
    {
        AfxMessageBox( "指定的文件没有找到或不是合法的u-boot ROM 文件!" );
        return;
    }

    DWORD dwCount = g_cSerializer.GetMacCount();
    CMacAddr addr;
    if ( g_cSerializer.GetMac(0, addr) )
    {
        SetDlgItemText ( IDC_ET_MAC1, addr.ToString() );
    }
    if ( dwCount == 2 && g_cSerializer.GetMac(1, addr))
    {
        SetDlgItemText ( IDC_ET_MAC2, addr.ToString() );
    }
    GetDlgItem(IDC_ET_MAC2)->EnableWindow( dwCount == 2 ? TRUE : FALSE );

    SetDlgItemInt( IDC_ET_MACNUM, dwCount, FALSE );	    
}

void CShowMacDlg::SetFilename(LPCTSTR lpszFilename)
{
    strncpy( szFileName, lpszFilename, KDV_MAX_PATH-1 );
}

BOOL CShowMacDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // 上层指定了文件
	if (strlen(szFileName) > 0)
    {
        Refresh(szFileName);
        memset(szFileName, 0, sizeof(szFileName));
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShowMacDlg::OnBtnModify() 
{
    CString cstrMac1, cstrMac2;
    GetDlgItemText ( IDC_ET_MAC1, cstrMac1) ;
    GetDlgItemText ( IDC_ET_MAC2, cstrMac2) ;
    
    CString cstrFile;
    GetDlgItemText( IDC_ET_FILENAME, cstrFile );

    Refresh(cstrFile);

    CString cstr;
    CMacAddr cAddrTmp1, cAddrTmp2;
    
    g_cSerializer.SetFillFile(FALSE);

    if (g_cSerializer.GetMacCount() > 0)
    {
        DWORD dwCount = g_cSerializer.GetMacCount();
        
        if ( !cAddrTmp1.Str2u48 (cstrMac1) )
        {
            CString cstr2;
            cstr2.Format( "输入的 MAC %s 不合法! ", cstrMac1 );
            AfxMessageBox(cstr2);
            return;
        }
        if (dwCount == 2)
        {
            if ( !cAddrTmp2.Str2u48 (cstrMac2) )
            {
                CString cstr2;
                cstr2.Format( "输入的 MAC %s 不合法! ", cstrMac2 );
                AfxMessageBox(cstr2);
                return;
            }
        }

        g_cSerializer.WriteMac2Bin(cstrFile, cAddrTmp1, 0);
        g_cSerializer.WriteMac2Bin(cstrFile, cAddrTmp2, 1);
        
        AfxMessageBox("修改MAC地址成功!");

        Refresh(cstrFile);
    }

	
}
