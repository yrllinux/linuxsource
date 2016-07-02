// prj_clientDlg.h : header file
//

#if !defined(AFX_PRJ_CLIENTDLG_H__9CF8F757_943E_44E0_95B6_F0D035B32FCB__INCLUDED_)
#define AFX_PRJ_CLIENTDLG_H__9CF8F757_943E_44E0_95B6_F0D035B32FCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COspClientDlg dialog

class COspClientDlg : public CDialog
{
// Construction
public:
	COspClientDlg(CWnd* pParent = NULL);	// standard constructor



// Dialog Data
	//{{AFX_DATA(COspClientDlg)
	enum { IDD = IDD_PRJ_CLIENT_DIALOG };
	CIPAddressCtrl	m_pAdress;
	CListBox	m_List;
	CProgressCtrl	m_progress;
	BOOL nFileOwnFlag;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COspClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COspClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCSendFileProgress(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnChooseFile();
	afx_msg void OnDeleteFile();
	afx_msg void OnSendFile();
	afx_msg void OnPauseSendFile();
	afx_msg void OnRestartSendFile();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnFieldchangedIpaddress(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
extern COspClientDlg *pCwindow;						//该文件内使用，传递类窗口属性

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_PRJ_CLIENTDLG_H__9CF8F757_943E_44E0_95B6_F0D035B32FCB__INCLUDED_)
