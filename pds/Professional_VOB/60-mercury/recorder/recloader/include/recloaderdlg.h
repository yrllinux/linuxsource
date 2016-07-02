// recloaderDlg.h : header file
//

#if !defined(AFX_RECLOADERDLG_H__FAEB6DBD_5178_4BCB_825B_889D781064C8__INCLUDED_)
#define AFX_RECLOADERDLG_H__FAEB6DBD_5178_4BCB_825B_889D781064C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

#define MAX_REC        15

/////////////////////////////////////////////////////////////////////////////
// CRecloaderDlg dialog

class CRecloaderDlg : public CDialog
{
// Construction
public:
	CRecloaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRecloaderDlg)
	enum { IDD = IDD_RECLOADER_DIALOG };
	CButton	m_ckRec14;
	CButton	m_ckRec13;
	CButton	m_ckRec12;
	CButton	m_ckRec11;
	CButton	m_ckRec10;
	CButton	m_ckRec9;
	CButton	m_ckRec8;
	CButton	m_ckRec7;
	CButton	m_ckRec6;
	CButton	m_ckRec5;
	CButton	m_ckRec4;
	CButton	m_ckRec3;
	CButton	m_ckRec2;
	CButton	m_ckRec1;
	CButton	m_ckRec0;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecloaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRecloaderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnStart();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnAll();
	afx_msg void OnBtnNone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    void Install(int nCopies);
    void UnInstall();
    
private:
    void ChgUILang();
    CString GetText(int nResID);
    
    void InitRecChecks();
private:

    LANGID m_tLangId;

    int      m_nValidRecNum;
    CButton* m_apckRec[MAX_REC];
    CString  m_astrAppName[MAX_REC]; // 对应的启动程序

    // 维护启动的录像机
    vector<HANDLE> m_vtInst;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECLOADERDLG_H__FAEB6DBD_5178_4BCB_825B_889D781064C8__INCLUDED_)
