#if !defined(AFX_USERMANAGEDLG_H__431EF714_4D9C_4EB1_89C5_7963CBB19BAF__INCLUDED_)
#define AFX_USERMANAGEDLG_H__431EF714_4D9C_4EB1_89C5_7963CBB19BAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserManageDlg.h : header file
//

#define COLUMN_USER	(int)0
#define COLUMN_PASS	(int)1

/////////////////////////////////////////////////////////////////////////////
// CUserManageDlg dialog

class CUserManageDlg : public CDialog
{
// Construction
public:
	BOOL32 m_bIsUserChanged;
	void ReadUserInfo( s8 * pchCfgFile );
	CUserManageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserManageDlg)
	enum { IDD = IDD_USERMANAGE_DIALOG };
	CListCtrl	m_cUserInfoList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserManageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserManageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdduser();
	afx_msg void OnDeluser();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERMANAGEDLG_H__431EF714_4D9C_4EB1_89C5_7963CBB19BAF__INCLUDED_)
