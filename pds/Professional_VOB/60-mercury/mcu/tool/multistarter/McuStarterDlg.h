// McuStarterDlg.h : header file
//

#if !defined(AFX_MCUSTARTERDLG_H__665115AE_F58B_4A3D_BF26_BECDC70A4D40__INCLUDED_)
#define AFX_MCUSTARTERDLG_H__665115AE_F58B_4A3D_BF26_BECDC70A4D40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMcuStarterDlg dialog

class CMcuStarterDlg : public CDialog
{
// Construction
public:
	CMcuStarterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMcuStarterDlg)
	enum { IDD = IDD_MCUSTARTER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcuStarterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMcuStarterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    // Î¬»¤Æô¶¯µÄmcu
    vector<HANDLE> m_vtInst;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCUSTARTERDLG_H__665115AE_F58B_4A3D_BF26_BECDC70A4D40__INCLUDED_)
