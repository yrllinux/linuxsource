// LicensePage.h : header file
//

#ifndef __LICENSEPAGE_H__
#define __LICENSEPAGE_H__

#define _EXCLUDE_VCCOMMON_H

#include "osp.h"

/////////////////////////////////////////////////////////////////////////////
// CMcuLicensePage dialog

class CMcuLicensePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMcuLicensePage)

// Construction
public:
	CString m_csAuthCode;
	CString m_csErrorCode;
	CString m_csLicensePath;
	CString m_csLicenseType;
	CString m_csLicenseVersion;
	CString m_csLicenseKey;
	
	CMcuLicensePage();
	~CMcuLicensePage();

// Dialog Data
	//{{AFX_DATA(CMcuLicensePage)
	enum { IDD = IDD_MCU_PROPPAGE };	
	CDateTimeCtrl	m_dtcExpireDate;
	UINT	m_uAccessNum;
	CString	m_sDeviceID;
	BOOL	m_bCheckDate;
	CComboBox	m_cboxMcuType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMcuLicensePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMcuLicensePage)
	afx_msg void OnBtnGenerate();
	afx_msg void OnBtnImport();
	afx_msg void OnCheckDate();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CMtLicensePage dialog

class CMtLicensePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMtLicensePage)

// Construction
public:
	CString m_csLicenseType;
	CString m_csLicenseVersion;
	CString m_csErrorCode;
	CString m_csLicenseKey;
	CString m_csLicensePath;
	CString m_csAuthCode;
	CMtLicensePage();
	~CMtLicensePage();

// Dialog Data
	//{{AFX_DATA(CMtLicensePage)
	enum { IDD = IDD_MT_PROPPAGE };
	CComboBox	m_cboxMtModal;
	CComboBox	m_cboxAccessNum;
	int		m_nRadioRunKDMMt;
	CString	m_sDeviceID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMtLicensePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMtLicensePage)
	afx_msg void OnBtnGenerate();
	afx_msg void OnBtnImport();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CT120LicensePage dialog

class CT120LicensePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CT120LicensePage)

// Construction
public:
	CString m_csLicenseType;
	CString m_csLicenseVersion;
	CString m_csErrorCode;
	CString m_csLicenseKey;
	CString m_csAuthCode;
	CString m_csLicensePath;
	CT120LicensePage();
	~CT120LicensePage();

// Dialog Data
	//{{AFX_DATA(CT120LicensePage)
	enum { IDD = IDD_T120_PROPPAGE };
	CComboBox	m_cboxAccessNum;
	CString	m_csDeviceID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CT120LicensePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CT120LicensePage)
	afx_msg void OnBtnGenerate();
	afx_msg void OnBtnImport();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

void WriteFile( CString csLicenseContent, CString csAuthCode, CString csLicenseKey, CString &csErrorCode );

BOOL MatchLicense( const s8 *pcLicenseKey, const s8 *pcLicenseVersion, const s8 *pcLicenseType, s8 *pcError);

BOOL IsDeviceIDInvalid(CString csDeviceID, s8 chSep);


#endif // __LICENSEPAGE_H__
