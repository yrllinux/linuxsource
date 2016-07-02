// McuStarter.h : main header file for the MCUSTARTER application
//

#if !defined(AFX_MCUSTARTER_H__2820845E_8B24_40A2_AD56_0ED7759F3FA9__INCLUDED_)
#define AFX_MCUSTARTER_H__2820845E_8B24_40A2_AD56_0ED7759F3FA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMcuStarterApp:
// See McuStarter.cpp for the implementation of this class
//

class CMcuStarterApp : public CWinApp
{
public:
	CMcuStarterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcuStarterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMcuStarterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCUSTARTER_H__2820845E_8B24_40A2_AD56_0ED7759F3FA9__INCLUDED_)
