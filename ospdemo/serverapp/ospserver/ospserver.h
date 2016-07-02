// ospserver.h : main header file for the OSPSERVER application
//

#if !defined(AFX_OSPSERVER_H__F9BF3509_D414_4DA0_8DAE_CCB4D75B66A4__INCLUDED_)
#define AFX_OSPSERVER_H__F9BF3509_D414_4DA0_8DAE_CCB4D75B66A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COspserverApp:
// See ospserver.cpp for the implementation of this class
//

class COspserverApp : public CWinApp
{
public:
	COspserverApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COspserverApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COspserverApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OSPSERVER_H__F9BF3509_D414_4DA0_8DAE_CCB4D75B66A4__INCLUDED_)
