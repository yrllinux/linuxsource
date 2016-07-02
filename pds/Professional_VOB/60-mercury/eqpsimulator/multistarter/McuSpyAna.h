// McuSpyAna.h : main header file for the MCUSPYANA application
//

#if !defined(AFX_MCUSPYANA_H__82E079A5_AD3A_4087_96F2_0BBE84A9709C__INCLUDED_)
#define AFX_MCUSPYANA_H__82E079A5_AD3A_4087_96F2_0BBE84A9709C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMcuSpyAnaApp:
// See McuSpyAna.cpp for the implementation of this class
//

class CMcuSpyAnaApp : public CWinApp
{
public:
	CMcuSpyAnaApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcuSpyAnaApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMcuSpyAnaApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCUSPYANA_H__82E079A5_AD3A_4087_96F2_0BBE84A9709C__INCLUDED_)
