// serial2wifi.h : main header file for the serial2wifi application
//

#if !defined(AFX_PCWIFI_H__775D25EE_5C77_4BD9_8DD2_F2530308AD72__INCLUDED_)
#define AFX_PCWIFI_H__775D25EE_5C77_4BD9_8DD2_F2530308AD72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPCWIFIApp:
// See serial2wifi.cpp for the implementation of this class
//

class CPCWIFIApp : public CWinApp
{
public:
	CPCWIFIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCWIFIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPCWIFIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCWIFI_H__775D25EE_5C77_4BD9_8DD2_F2530308AD72__INCLUDED_)
