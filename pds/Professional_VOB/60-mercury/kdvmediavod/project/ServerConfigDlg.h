#if !defined(AFX_SERVERCONFIGDLG_H__0109BC03_9FF5_4544_B3B2_14154F81387C__INCLUDED_)
#define AFX_SERVERCONFIGDLG_H__0109BC03_9FF5_4544_B3B2_14154F81387C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerConfigDlg dialog

class CServerConfigDlg : public CDialog
{
// Construction
public:
	u32 m_dwServIp;
	CServerConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CServerConfigDlg)
	enum { IDD = IDD_SERVERCONFIG_DIALOG };
	CIPAddressCtrl	m_cServIp;
	UINT	m_unMtuSize;
	CString	m_csFileSavePath;
	UINT	m_unSendStartPort;
	UINT	m_unServerPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CServerConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnScanfile();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERCONFIGDLG_H__0109BC03_9FF5_4544_B3B2_14154F81387C__INCLUDED_)
