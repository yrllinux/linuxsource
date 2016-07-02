// kdvmediavodDlg.h : header file
//

#if !defined(AFX_KDVMEDIAVODDLG_H__44EDB28F_4E17_4D6B_95D7_7A050DEA8674__INCLUDED_)
#define AFX_KDVMEDIAVODDLG_H__44EDB28F_4E17_4D6B_95D7_7A050DEA8674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kdvmediavodinstance.h"

/////////////////////////////////////////////////////////////////////////////
// CKdvmediavodDlg dialog
class CKdvmediavodDlg : public CDialog
{
// Construction
public:
	void SaveConfig(TVodConfig* ptVodConfig);
	CKdvmediavodDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CKdvmediavodDlg)
	enum { IDD = IDD_KDVMEDIAVOD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKdvmediavodDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	//int m_nLanguageType;
	int m_nIsVodStart;

	// Generated message map functions
	//{{AFX_MSG(CKdvmediavodDlg)
    virtual void OnOK();
    virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnEnglish();
	afx_msg void OnBtnChinese();
	afx_msg void OnConfig();
	afx_msg void OnUsermanage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void OnStartVod();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDVMEDIAVODDLG_H__44EDB28F_4E17_4D6B_95D7_7A050DEA8674__INCLUDED_)
