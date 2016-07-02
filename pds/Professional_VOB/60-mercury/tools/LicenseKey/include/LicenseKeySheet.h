// LicenseKeySheet.h : header file
//
// This class defines custom modal property sheet 
// CLicenseKeySheet.
 
#ifndef __LICENSEKEYSHEET_H__
#define __LICENSEKEYSHEET_H__

#include "LicensePage.h"

/////////////////////////////////////////////////////////////////////////////
// CLicenseKeySheet

class CLicenseKeySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CLicenseKeySheet)

// Construction
public:
	CLicenseKeySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CMcuLicensePage m_Page1;
	CMtLicensePage m_Page2;
	CT120LicensePage m_Page3;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseKeySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLicenseKeySheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CLicenseKeySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __LICENSEKEYSHEET_H__
