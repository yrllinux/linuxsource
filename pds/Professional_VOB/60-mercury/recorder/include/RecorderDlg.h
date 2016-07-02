// RecorderDlg.h : header file
//

#if !defined(AFX_RECORDERDLG_H__5CAF029C_875C_4E45_905C_C484E7CB341A__INCLUDED_)
#define AFX_RECORDERDLG_H__5CAF029C_875C_4E45_905C_C484E7CB341A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRecorderDlg dialog

#include "RecApp.h"

class CRecorderDlg : public CDialog
{
// Construction
public:
	CRecorderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRecorderDlg)
	enum { IDD = IDD_RECORDER_DIALOG };
	CStatic	m_ctrStatusNote;
	int		m_nEqpId;
	int		m_nMcuId;
	int		m_nMcuPort;
	int		m_nPlayChnNum;
	int		m_nRecChnNum;
	int		m_nRecStartPort;
	CString	m_cDbPassWord;
	CString	m_cDbAddr;
	CString	m_cDbUserName;
	CString	m_cEqpAlias;
	int		m_nEqpType;
	CString	m_cMcuAddr;
	CString	m_cRecAddr;
	CString	m_cRecMachName;
	CString	m_cRecSavePath;
	CString	m_cVodFtpDir;
	CString	m_cVodFtpPW;
	CString	m_cVodFtpUserName;
	int		m_nMcuIdB;
	int		m_nMcuPortB;
	CString	m_cMcuAddrB;
	int		m_nPlayStartPort;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecorderDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	CString m_strConsole;
//	TTrcInfo    m_tTrcInfo[MAXLINE_RUNINFO];
//	int     m_nTrcInfoBeginPos;
//	int    m_nTrcInfoSize;
	DWORD   m_dwShowInfoType;
	NOTIFYICONDATA m_tnidTray;//托盘结构

    //add by libo
    BOOL	m_bModifyConfig;    //是否可以修改配置信息
	BOOL	m_bModified;        //是否配置被修改
    BOOL    m_bCriticalModify;  //是否是关键更改，即需要提示用户手动重启
	BOOL    m_bSetFlag;
    LANGID  m_tLangId;  // language id
    BOOL    m_bOSEn;  // 是否英文版

protected:
	void Init();
	void InitTree();
    void HideModifyConfig();
    void ShowModifyConfig();
    BOOL SaveModifyConfig();
	void ShowConfig();
    void ChangeServiceInfo(BOOL bIsValid);
//	void ShowTrcInfo();
	void ShowRecChnInfo( WORD wChn ); 
	void ShowPlayChnInfo( WORD wChn );
//	void OnShowTrc(WPARAM w, LPARAM l);
    void LanguageToEnglish(void);
    void LanguageToChinese(void);

	void ChangeStringFormat(LPSTR pchDst, LPSTR pchStr, u8 bNum);

    void GetEnglishConfigureString(char* pchStr);
    void GetChineseConfigureString(char* pchStr);

    void GetChinesePlayChannelInfo(char* pchChlStr, TPlayerStatus& tPlayerStatus, TPlayerStatis& tPlayerStatis);
    void GetEnglishPlayChannelInfo(char* pchChlStr, TPlayerStatus& tPlayerStatus, TPlayerStatis& tPlayerStatis);

    void GetChineseRecorderChannelInfo(char* pchChlStr, TRecorderStatus& tRecStatus, TRecorderStatis& tRecStatis);
    void GetEnglishRecorderChannelInfo(char* pchChlStr, TRecorderStatus& tRecStatus, TRecorderStatis& tRecStatis);

    void ChangeTopic(BOOL32 bNew);
    void ChangeMenuText(void);

	BOOL32 IsValidIpV4(char* psIPStr);

public:
	void Quit(BOOL32 bForce = FALSE);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRecorderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClickChnTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedChnTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnCheckIspublish();
	afx_msg void OnChangeEditEqptype();
	afx_msg void OnChangeEditMcuid();
	afx_msg void OnChangeEditMcuport();
	afx_msg void OnChangeEditPlaychnlnum();
	afx_msg void OnChangeEditRecchnlnum();
	afx_msg void OnChangeEditEqpid();
    afx_msg void OnChangeEditMcuPortB();
	afx_msg void OnChangeEditStartplayport();
	afx_msg void OnChangeEditRecstartport();
	//}}AFX_MSG
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);	
    afx_msg void OnOpenDataBase(WPARAM wParam, LPARAM lParam);    
    afx_msg void OnRestart(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRecQuit(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()    
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDERDLG_H__5CAF029C_875C_4E45_905C_C484E7CB341A__INCLUDED_)
