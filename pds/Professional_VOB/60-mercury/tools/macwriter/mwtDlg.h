// mwtDlg.h : header file
//

#if !defined(AFX_MWTDLG_H__5D89B876_1C71_4F0A_82A9_C5D0E554B651__INCLUDED_)
#define AFX_MWTDLG_H__5D89B876_1C71_4F0A_82A9_C5D0E554B651__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MacAddr.h"
#include "SNLogMgr.h"

#define FILENAME_CFG        "mtw.ini"
#define FILENAME_REPORT     "最新日志.LOG"

#define MAC_MIN             "00:14:10:00:00:00"
#define MAC_MAX             "00:14:10:FF:FF:FF"

/////////////////////////////////////////////////////////////////////////////
// CMwtDlg dialog

class CMwtDlg : public CDialog
{
// Construction
public:
	CMwtDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMwtDlg)
	enum { IDD = IDD_MWT_DIALOG };
	CListCtrl	m_lsMacSn;
	CProgressCtrl	m_prgWrite;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMwtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMwtDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnBrowse();
	afx_msg void OnBtnModifymac();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnSnlist();
	afx_msg void OnBtnLog();
	afx_msg void OnBtnRefresh();
	afx_msg void OnDestroy();
	afx_msg void OnBtnShowmac();
	afx_msg void OnCkAuto();
	afx_msg void OnBtnAddsn();
	afx_msg void OnBtnDelsn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    // 从文件里面读取默认Mac
    void LoadDefaultMacAddr();
    //保存默认Mac
    void SaveDefaultMacAddr();

    void ShowErr(LPCTSTR lpszErr, BOOL bMsgBox = TRUE);

    BOOL ValidateInput(SNItem &item);
    void LoadMacSN();
    
private:
    // 最小最大值
    CMacAddr m_cMacMin;
    CMacAddr m_cMacMax;
    // 当前值
    CMacAddr m_cMacCurr;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MWTDLG_H__5D89B876_1C71_4F0A_82A9_C5D0E554B651__INCLUDED_)
