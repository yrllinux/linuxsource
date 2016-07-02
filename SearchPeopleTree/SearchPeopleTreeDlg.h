// SearchPeopleTreeDlg.h : header file
//

#if !defined(AFX_SEARCHPEOPLETREEDLG_H__29DE2931_2DA1_4A04_9058_2B79E99F50B2__INCLUDED_)
#define AFX_SEARCHPEOPLETREEDLG_H__29DE2931_2DA1_4A04_9058_2B79E99F50B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "search.h"
/////////////////////////////////////////////////////////////////////////////
// CSearchPeopleTreeDlg dialog

class CSearchPeopleTreeDlg : public CDialog
{
// Construction
public:
	CSearchPeopleTreeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSearchPeopleTreeDlg)
	enum { IDD = IDD_SEARCHPEOPLETREE_DIALOG };
	CListCtrl	m_ctList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchPeopleTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSearchPeopleTreeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void UpdateList();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void ReadOrWriteInFile(TPeopleTree &pt, const s8 *sName = NULL, u8 byDo = NONE);
	afx_msg void OnSearchByKeyWord();
	afx_msg void OnDisPlay(TPeopleTree &pt);
	afx_msg void AddInfoToPt(TPeopleTree &pt); 
	afx_msg BOOL32 IsStrCmp(TPeopleTree &pt, const s8 *cKeyWord);
	afx_msg BOOL32 IsCmpByKeyWord(TPeopleTree &pt, CString cKeyWord1, CString cKeyWord2, CString cKeyWord3);

	afx_msg void OnChangeEdit1();
	afx_msg void OnChangeEdit2();
	afx_msg void OnChangeEdit3();
	afx_msg void OnSearch();
	afx_msg void OnChangeEdit4();
	afx_msg void OnChangeEdit5();
	afx_msg void OnChangeEdit6();
	afx_msg void OnChangeEdit7();
	afx_msg void OnChangeEdit8();
	afx_msg void OnChangeEdit9();
	afx_msg void OnChangeEdit10();
	afx_msg void OnChangeEdit11();
	afx_msg void OnChangeEdit13();
	afx_msg void OnChangeEdit12();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClickTree2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeList1();
	afx_msg void OnClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHPEOPLETREEDLG_H__29DE2931_2DA1_4A04_9058_2B79E99F50B2__INCLUDED_)
