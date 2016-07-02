// LicenseKeySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LicenseKeySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLicenseKeySheet

IMPLEMENT_DYNAMIC(CLicenseKeySheet, CPropertySheet)

CLicenseKeySheet::CLicenseKeySheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_Page1);
	AddPage(&m_Page2);
	AddPage(&m_Page3);
	
}

CLicenseKeySheet::~CLicenseKeySheet()
{
}


BEGIN_MESSAGE_MAP(CLicenseKeySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CLicenseKeySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLicenseKeySheet message handlers



BOOL CLicenseKeySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
	CWnd *pWnd = GetDlgItem(IDCANCEL);
	pWnd->ShowWindow( FALSE ); 
	pWnd = this->GetDlgItem(IDHELP);
	pWnd->ShowWindow( FALSE ); 

	CRect rectBtn;
	GetDlgItem(IDOK)->GetWindowRect (rectBtn);
	ScreenToClient(&rectBtn);
	int btnWidth = rectBtn.Width();
	rectBtn.left = rectBtn.left + (btnWidth + 6)* 2;
	rectBtn.right = rectBtn.right + (btnWidth + 6)* 2;
	GetDlgItem(IDOK)->MoveWindow(rectBtn);   

	return TRUE;
}
