#if !defined(AFX_SHOWMACDLG_H__23108E09_A575_40A6_B02A_96233A40C0E5__INCLUDED_)
#define AFX_SHOWMACDLG_H__23108E09_A575_40A6_B02A_96233A40C0E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowMacDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowMacDlg dialog

class CShowMacDlg : public CDialog
{
// Construction
public:
	CShowMacDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowMacDlg)
	enum { IDD = IDD_SHOWMAC };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowMacDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShowMacDlg)
	afx_msg void OnBtnBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnModify();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    void Refresh(LPCTSTR szFilename);

    void SetFilename(LPCTSTR lpszFilename);
private:
    s8 szFileName[KDV_MAX_PATH];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWMACDLG_H__23108E09_A575_40A6_B02A_96233A40C0E5__INCLUDED_)
