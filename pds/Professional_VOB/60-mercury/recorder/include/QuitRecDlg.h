#if !defined(AFX_QUITRECDLG_H__1833DC63_0D69_4291_8895_D8EEC4ACA8B0__INCLUDED_)
#define AFX_QUITRECDLG_H__1833DC63_0D69_4291_8895_D8EEC4ACA8B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuitRecDlg.h : header file
//


#define WM_LANGTOCHANGE   (WM_USER + 3)    // ÓïÑÔ¸Ä±ä
/////////////////////////////////////////////////////////////////////////////
// CQuitRecDlg dialog

class CQuitRecDlg : public CDialog
{
// Construction
public:
	CQuitRecDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CQuitRecDlg)
	enum { IDD = IDD_QUITDLG };
	CString	m_achNotfyStr;
	//}}AFX_DATA

    
    BOOL m_bEnglishLang;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuitRecDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuitRecDlg)
	//}}AFX_MSG
    afx_msg void OnChangeLanguage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUITRECDLG_H__1833DC63_0D69_4291_8895_D8EEC4ACA8B0__INCLUDED_)
