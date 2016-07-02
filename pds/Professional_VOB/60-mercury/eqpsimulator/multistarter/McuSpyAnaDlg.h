// McuSpyAnaDlg.h : header file
//

#if !defined(AFX_MCUSPYANADLG_H__0B6387A9_720C_4219_B7E8_A7ACACD05F43__INCLUDED_)
#define AFX_MCUSPYANADLG_H__0B6387A9_720C_4219_B7E8_A7ACACD05F43__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;


/////////////////////////////////////////////////////////////////////////////
// CMcuSpyAnaDlg dialog

class CMcuSpyAnaDlg : public CDialog
{
// Construction
public:
	CMcuSpyAnaDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMcuSpyAnaDlg)
	enum { IDD = IDD_MCUSPYANA_DIALOG };
	CComboBox	m_cbType;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcuSpyAnaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMcuSpyAnaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnRun();
	afx_msg void OnBtnQuit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void Anaylise();
    void IncPercent(CString &line, LPCTSTR tar, long &nTarget);

    vector<HANDLE> m_vtInst;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCUSPYANADLG_H__0B6387A9_720C_4219_B7E8_A7ACACD05F43__INCLUDED_)
