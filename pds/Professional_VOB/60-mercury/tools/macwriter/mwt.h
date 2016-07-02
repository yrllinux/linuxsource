// mwt.h : main header file for the MWT application
//

#if !defined(AFX_MWT_H__F566DC89_421F_49BD_897B_21AA953291A1__INCLUDED_)
#define AFX_MWT_H__F566DC89_421F_49BD_897B_21AA953291A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMwtApp:
// See mwt.cpp for the implementation of this class
//

class CMwtApp : public CWinApp
{
public:
	CMwtApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMwtApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMwtApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MWT_H__F566DC89_421F_49BD_897B_21AA953291A1__INCLUDED_)
