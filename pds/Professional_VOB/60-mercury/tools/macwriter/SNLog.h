#if !defined(AFX_SNLOG_H__13FD459F_6662_4290_9762_0F0549E7CDB4__INCLUDED_)
#define AFX_SNLOG_H__13FD459F_6662_4290_9762_0F0549E7CDB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SNLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSNLog dialog

class CSNLog : public CDialog
{
// Construction
public:
	CSNLog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSNLog)
	enum { IDD = IDD_LOG };
	CComboBox	m_cbSearchType;
	CListCtrl	m_lsMacSn;
	CTime	m_tStart;
	CTime	m_tEnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSNLog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSNLog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCbSearchtype();
	afx_msg void OnBtnSearch();
	afx_msg void OnBtnSearch2();
	afx_msg void OnBtnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    LoadHistorySN();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNLOG_H__13FD459F_6662_4290_9762_0F0549E7CDB4__INCLUDED_)
