// licensedlg.h : main header file for the LICENSEDLG application
//

#if !defined(AFX_LICENSEDLG_H__AD1524C5_FAF4_4313_AA23_E2C4B257D648__INCLUDED_)
#define AFX_LICENSEDLG_H__AD1524C5_FAF4_4313_AA23_E2C4B257D648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLicensedlgApp:
// See licensedlg.cpp for the implementation of this class
//

class CLicensedlgApp : public CWinApp
{
public:
	CLicensedlgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicensedlgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLicensedlgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEDLG_H__AD1524C5_FAF4_4313_AA23_E2C4B257D648__INCLUDED_)
