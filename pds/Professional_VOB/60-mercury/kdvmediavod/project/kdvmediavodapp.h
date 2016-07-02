// kdvmediavod.h : main header file for the KDVMEDIAVOD application
//

#if !defined(AFX_KDVMEDIAVOD_H__9399049C_8DB5_4663_9830_1AAEA852A31F__INCLUDED_)
#define AFX_KDVMEDIAVOD_H__9399049C_8DB5_4663_9830_1AAEA852A31F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKdvmediavodApp:
// See kdvmediavod.cpp for the implementation of this class
//

class CKdvmediavodApp : public CWinApp
{
public:
	CKdvmediavodApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKdvmediavodApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKdvmediavodApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDVMEDIAVOD_H__9399049C_8DB5_4663_9830_1AAEA852A31F__INCLUDED_)
