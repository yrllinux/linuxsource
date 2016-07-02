// prj_client.h : main header file for the PRJ_CLIENT application
//

#if !defined(AFX_PRJ_CLIENT_H__5238F299_1EE2_4134_ACB6_5B1CCB30107C__INCLUDED_)
#define AFX_PRJ_CLIENT_H__5238F299_1EE2_4134_ACB6_5B1CCB30107C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COspClientApp:
// See prj_client.cpp for the implementation of this class
//

class COspClientApp : public CWinApp
{
public:
	COspClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COspClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COspClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJ_CLIENT_H__5238F299_1EE2_4134_ACB6_5B1CCB30107C__INCLUDED_)
