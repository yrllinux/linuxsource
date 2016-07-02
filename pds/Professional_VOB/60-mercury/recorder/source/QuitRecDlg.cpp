// QuitRecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "recorder.h"
#include "QuitRecDlg.h"
//#include "ConfAgent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CQuitRecDlg dialog


CQuitRecDlg::CQuitRecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuitRecDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuitRecDlg)
    m_achNotfyStr = _T("");
	//}}AFX_DATA_INIT

    m_bEnglishLang = FALSE;
}

void CQuitRecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuitRecDlg)
	DDX_Text(pDX, IDC_STATIC_NOTIFY, m_achNotfyStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuitRecDlg, CDialog)
	//{{AFX_MSG_MAP(CQuitRecDlg)
	//}}AFX_MSG_MAP
    ON_MESSAGE( WM_LANGTOCHANGE, OnChangeLanguage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuitRecDlg message handlers


/*=============================================================================
  函 数 名： OnChangeLanguage
  功    能： 更改语言
  算法实现： 
  全局变量： 
  参    数： WPARAM wParam
             LPARAM lParam
  返 回 值： void 
=============================================================================*/
void CQuitRecDlg::OnChangeLanguage(WPARAM wParam, LPARAM lParam)
{
    if( TRUE == wParam )
    {
        m_achNotfyStr = _T(" Quit Recorder Server?");
    }
    else
    {
        m_achNotfyStr = _T(" 退出录像机?");
    }
    AfxGetMainWnd()->UpdateData(FALSE);
    return;
}


/*=============================================================================
函 数 名： OnInitDialog
功    能：  
参    数： 
返 回 值： BOOL 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/1/23  4.0			许世林                  创建
=============================================================================*/
BOOL CQuitRecDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if( m_bEnglishLang ) 
    {
        m_achNotfyStr = _T("Quit Recorder Server?");
        GetDlgItem(IDOK)->SetWindowText("Ok");
        GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
        SetWindowText("Quit");
    }
    else
    {
        m_achNotfyStr = _T("退出录像机?");
        GetDlgItem(IDOK)->SetWindowText("确定");
        GetDlgItem(IDCANCEL)->SetWindowText("取消");
        SetWindowText("退出");
    }
    
    UpdateData(FALSE);

    return TRUE;
}
