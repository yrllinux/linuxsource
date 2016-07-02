// recloader.h : main header file for the RECLOADER application
//

#if !defined(AFX_RECLOADER_H__529EC05B_2923_4485_A3B1_17999A8A87CB__INCLUDED_)
#define AFX_RECLOADER_H__529EC05B_2923_4485_A3B1_17999A8A87CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRecloaderApp:
// See recloader.cpp for the implementation of this class
//

class CRecloaderApp : public CWinApp
{
public:
	CRecloaderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecloaderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRecloaderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECLOADER_H__529EC05B_2923_4485_A3B1_17999A8A87CB__INCLUDED_)
