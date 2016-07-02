// sampleDlg.h : header file
//



#if !defined(AFX_SAMPLEDLG_H__3B697FDF_280D_42F2_A339_556F51308E12__INCLUDED_)
#define AFX_SAMPLEDLG_H__3B697FDF_280D_42F2_A339_556F51308E12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Sink.h"	
#include "SinkStream.h"
#include "..\prj_win32\mcu_i.c"
#include "RecInterface.h"

class CSampleDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CSampleDlg dialog

class CSampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSampleDlg);
	friend class CSampleDlgAutoProxy;

// Construction
public:
	CSampleDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CSampleDlg();

// Dialog Data
	//{{AFX_DATA(CSampleDlg)
	enum { IDD = IDD_SAMPLE_DIALOG };
	CListCtrl	m_lsView;
	int		m_nDebug;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSampleDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	//{{AFX_MSG(CSampleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnStart();
	afx_msg void OnSelchangeCbDebug();
    afx_msg LRESULT OnConfUpdate (WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnStreamUpdate (WPARAM wParam, LPARAM lParam);
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


    CComQIPtr< IMCCtrl >       m_spRecCtrl;  // 录像机控制指针
    CComQIPtr< IConfProperty > m_spProperty; // 会议属性获取指针

    IMCStatusCallbackPtr m_spMcStatus; 		 // 会议状态组件接口指针
    IMCStreamCallbackPtr m_spMcStream;       // 会议码流组件接口指针

	CComQIPtr< IConnectionPoint > m_spCPStatus;	// 会议状态通知连接点指针
    CComQIPtr< IConnectionPoint > m_spCPStream;	// 码流通知连接点指针
    
    CSink m_cRecorder;						// 接受器对象
    CSinkStream m_cStream;
    DWORD m_dwCookieStatus;					// 连接的 cookie
    DWORD m_dwCookieStream;					// 连接的 cookie

    void UpdateView(DWORD dwIdx);
    void UpdateView(TConfProperty &tConf, TRecProperty &tRec, emRecState emState );
    void UpdateView(TStreamProperty &tStream);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEDLG_H__3B697FDF_280D_42F2_A339_556F51308E12__INCLUDED_)
