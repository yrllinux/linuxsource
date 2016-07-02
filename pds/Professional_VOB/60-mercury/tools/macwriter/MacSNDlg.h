#if !defined(AFX_MACSNDLG_H__069B1379_1D72_4254_AA94_9037EEC7E868__INCLUDED_)
#define AFX_MACSNDLG_H__069B1379_1D72_4254_AA94_9037EEC7E868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MacSNDlg.h : header file
//

#include "MacAddr.h"
#include "SNLogMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CMacSNDlg dialog

class CMacSNDlg : public CDialog
{
// Construction
public:
	CMacSNDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMacSNDlg)
	enum { IDD = IDD_SNCFG };
	CComboBox	m_cbMacNum;
	CListCtrl	m_lsMacSn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacSNDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMacSNDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAdd();
	afx_msg void OnSelchangeCbNum();
	afx_msg void OnClickLsMacsn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnEdit();
	afx_msg void OnBtnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    void SetCurrMac(CMacAddr &cAddr)
    {
        m_cCurMac = cAddr;
    }
    void SetMacPerDevice(DWORD dwCount)
    {
        m_dwMacNum = dwCount;
    }

    

private:
    BOOL ValidateInput(SNItem &item);

    void LoadMacSN();
private:
    CMacAddr m_cCurMac;
    DWORD    m_dwMacNum;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACSNDLG_H__069B1379_1D72_4254_AA94_9037EEC7E868__INCLUDED_)
