// serial2wifiDlg.h : header file
//

#if !defined(AFX_PCWIFIDLG_H__03BE005D_CAA7_40AA_B485_F0552394DFC0__INCLUDED_)
#define AFX_PCWIFIDLG_H__03BE005D_CAA7_40AA_B485_F0552394DFC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPCWIFIDlg dialog

class CPCWIFIDlg : public CDialog
{
// Construction
public:
	CPCWIFIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPCWIFIDlg)
	enum { IDD = IDD_PCWIFI_DIALOG };
	CComboBoxEx	m_t;
	CComboBox	m_type;
	CComboBox	m_mode;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCWIFIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPCWIFIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnEditchangeCombo1();
	afx_msg void OnEditchangeCombo2();
	afx_msg void OnEditchangeComboboxex1();
	afx_msg void OnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEdit2();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnChangeEdit3();
	afx_msg void OnChangeEdit4();
	afx_msg void OnButton1();
	afx_msg static UINT OnStartUdpServer(LPVOID lpParam);
	afx_msg static UINT OnStartUdpClient(LPVOID lpParam);
	afx_msg static UINT OnStartTcpServer(LPVOID lpParam);
	afx_msg static UINT OnStartTcpClient(LPVOID lpParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCWIFIDLG_H__03BE005D_CAA7_40AA_B485_F0552394DFC0__INCLUDED_)
