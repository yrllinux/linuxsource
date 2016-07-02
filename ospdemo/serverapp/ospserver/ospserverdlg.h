// ospserverdlg.h : header file
//

#if !defined(AFX_OSPSERVERDLG_H__765557A7_73B4_4EDE_A828_C8604F7421E1__INCLUDED_)
#define AFX_OSPSERVERDLG_H__765557A7_73B4_4EDE_A828_C8604F7421E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COspserverDlg dialog

class COspserverDlg : public CDialog
{
// Construction
public:
	COspserverDlg(CWnd* pParent = NULL);	// standard constructor
	CString strNum;
// Dialog Data
	//{{AFX_DATA(COspserverDlg)
	enum { IDD = IDD_OSPSERVER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COspserverDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COspserverDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnServerSpace();
	afx_msg void OnSetBaseDocument();
	afx_msg void OnBaseDocument();
	virtual void OnCancel();
	afx_msg void OnOk();
	afx_msg void OnBnClickedDecgen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OSPSERVERDLG_H__765557A7_73B4_4EDE_A828_C8604F7421E1__INCLUDED_)
