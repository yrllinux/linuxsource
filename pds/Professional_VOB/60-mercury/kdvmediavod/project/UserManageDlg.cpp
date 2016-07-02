// UserManageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kdvmediavodapp.h"
#include "kdvmediavod.h"
#include "UserManageDlg.h"
#include "kdvmediavodinstance.h"
#include "AddUserDlg.h"
// #include "kdvsys.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
#endif

extern int g_nLanguageType;
/////////////////////////////////////////////////////////////////////////////
// CUserManageDlg dialog


CUserManageDlg::CUserManageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserManageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserManageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUserManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserManageDlg)
	DDX_Control(pDX, IDC_USERANDPASS_LIST, m_cUserInfoList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserManageDlg, CDialog)
	//{{AFX_MSG_MAP(CUserManageDlg)
	ON_BN_CLICKED(IDC_ADDUSER, OnAdduser)
	ON_BN_CLICKED(IDC_DELUSER, OnDeluser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserManageDlg message handlers

BOOL CUserManageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    DWORD dwStyle = m_cUserInfoList.GetExtendedStyle();
    dwStyle |= ( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
    m_cUserInfoList.SetExtendedStyle(dwStyle);
    
    if ( g_nLanguageType == LANGUAGE_CHINESE )
    {
        GetDlgItem(IDC_ADDUSER)->SetWindowText("添加...");
        GetDlgItem(IDC_DELUSER)->SetWindowText("删除");
        GetDlgItem(IDOK)->SetWindowText("确定");
        GetDlgItem(IDCANCEL)->SetWindowText("取消");
        m_cUserInfoList.InsertColumn( COLUMN_USER, "用户名", LVCFMT_CENTER, 150, MEDIAVOD_MAX_CLIENT);
        m_cUserInfoList.InsertColumn( COLUMN_PASS, "密码", LVCFMT_CENTER, 150, MEDIAVOD_MAX_CLIENT );
    }
    else
    {
        GetDlgItem(IDC_ADDUSER)->SetWindowText("Add...");
        GetDlgItem(IDC_DELUSER)->SetWindowText("Delete");
        GetDlgItem(IDOK)->SetWindowText("Ok");
        GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
        m_cUserInfoList.InsertColumn( COLUMN_USER, "User name", LVCFMT_CENTER, 150, MEDIAVOD_MAX_CLIENT);
        m_cUserInfoList.InsertColumn( COLUMN_PASS, "Password", LVCFMT_CENTER, 150, MEDIAVOD_MAX_CLIENT );        
    }


    ReadUserInfo( CONFIG_FILE_NAME );
    m_bIsUserChanged = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserManageDlg::ReadUserInfo(s8 *pchCfgFile )
{
    FILE* stream;
    stream = fopen(pchCfgFile, "r");
    if ( NULL == stream )
    {
        OspPrintf( TRUE, FALSE, "[ReadUserInfo] Open %s failed!\n", pchCfgFile );
        return;
    }

    u32 dwMaxKeyNum = 10;
    s8 **ppArray = new s8*[dwMaxKeyNum];
    for ( u32 dwLoop = 0; dwLoop < dwMaxKeyNum; dwLoop++ )
    {
        ppArray[dwLoop] = new s8[MAX_LINE_LEN];
        memset( ppArray[dwLoop], 0, sizeof(s8)*MAX_LINE_LEN );
    }

    u32 dwKeyNum = 0;    
    u32 dwItemIdx = 0;
    u32 dwOffset = 0;
    s8 achPass[MAX_LINE_LEN];
    BOOL32 bRet = TRUE;
    do
    {
        dwKeyNum = 0;
        for ( dwLoop = 0; dwLoop < dwMaxKeyNum; dwLoop++ )
        {
            ppArray[dwLoop] = new s8[MAX_LINE_LEN];
            memset( ppArray[dwLoop], 0, sizeof(s8)*MAX_LINE_LEN );
        }
        dwKeyNum = GetSectionKeyArray( stream, "USERCONFIG", ppArray, dwMaxKeyNum, dwOffset );

        for( dwLoop = 0; dwLoop < dwKeyNum; dwLoop++ )
        {
            
            memset( achPass, 0, sizeof(achPass) );
            bRet = GetRegKeyString(pchCfgFile,
                                "USERCONFIG",  
                                ppArray[dwLoop], 
                                "error",
                                achPass,
		                        sizeof(achPass));
            if ( bRet )
            {
                m_cUserInfoList.InsertItem( dwItemIdx, "" );
                m_cUserInfoList.SetItemText( dwItemIdx, COLUMN_USER, ppArray[dwLoop]);
                m_cUserInfoList.SetItemText( dwItemIdx, COLUMN_PASS, achPass );
                dwItemIdx++;
            }
        }
    }while( dwKeyNum != 0 && dwKeyNum <= 10 );
 
    for ( dwLoop = 0; dwLoop < dwMaxKeyNum; dwLoop++ )
    {
        delete [] ppArray[dwLoop];
    }
    delete [] ppArray;

    fclose( stream );

    return;
}

void CUserManageDlg::OnAdduser() 
{
	// TODO: Add your control notification handler code here
	CAddUserDlg cAddUserDlg;
    if ( IDOK == cAddUserDlg.DoModal() )
    {
        if ( cAddUserDlg.m_csUserName.GetLength() > 0 && 
             cAddUserDlg.m_csPassWord.GetLength() > 0 )
        {
            int nItemCount = m_cUserInfoList.GetItemCount();
            for ( int nLop = 0; nLop < nItemCount; nLop++ )
            {
                if ( m_cUserInfoList.GetItemText( nLop, COLUMN_USER ) == cAddUserDlg.m_csUserName )
                {
                    if ( g_nLanguageType == LANGUAGE_CHINESE )
                    {
                        AfxMessageBox( "该用户已存在" );
                    }
                    else
                    {
                        AfxMessageBox( "This user is exist" );
                    }
                    return;
                }
            }
            m_cUserInfoList.InsertItem( nItemCount, "" );
            m_cUserInfoList.SetItemText( nItemCount, COLUMN_USER, (LPCTSTR)cAddUserDlg.m_csUserName );
            m_cUserInfoList.SetItemText( nItemCount, COLUMN_PASS, (LPCTSTR)cAddUserDlg.m_csPassWord );

            m_bIsUserChanged = TRUE;
        }
    }

    return;
}

void CUserManageDlg::OnDeluser() 
{
	// TODO: Add your control notification handler code here
	int nSelect = m_cUserInfoList.GetSelectionMark();
    if ( nSelect != -1 )
    {
        m_cUserInfoList.DeleteItem( nSelect );
        m_bIsUserChanged = TRUE;
    }  
    return;
}

void CUserManageDlg::OnOK() 
{
	// TODO: Add extra validation here
    if ( !m_bIsUserChanged )
    {
        CDialog::OnOK();
        return;
    }

    FILE *stream = fopen( CONFIG_FILE_NAME, "r+" );
    if ( stream == NULL )
    {
        if ( g_nLanguageType == LANGUAGE_CHINESE )
        {
            AfxMessageBox( "打开配置文件失败" );
        }
        else
        {
            AfxMessageBox( "Open config file failed\n" );
        }
        return;
    }

    // 清空Section下的KEY
    DelSection( stream, "USERCONFIG" );
    fclose( stream );
    // 整理配置文件，消除空格
    TrimCfgFile( CONFIG_FILE_NAME );
    
	int nItemCount = m_cUserInfoList.GetItemCount();
    char achUserName[MAX_LINE_LEN] = {0};
    char achPassWord[MAX_LINE_LEN] = {0};
    for ( int nLop = 0; nLop < nItemCount; nLop++ )
    {
        memset( achUserName, 0, sizeof(achUserName) );
        memset( achPassWord, 0, sizeof(achPassWord) );

        m_cUserInfoList.GetItemText( nLop, COLUMN_USER, achUserName, MAX_LINE_LEN );
        m_cUserInfoList.GetItemText( nLop, COLUMN_PASS, achPassWord, MAX_LINE_LEN );

        achUserName[MAX_LINE_LEN-1] = '\0';
        achPassWord[MAX_LINE_LEN-1] = '\0';

        SetRegKeyString(CONFIG_FILE_NAME,
                        "USERCONFIG",  
                        achUserName, 
                        achPassWord);
    }
    
	CDialog::OnOK();
}
