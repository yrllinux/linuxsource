// SearchPeopleTree.h : main header file for the SEARCHPEOPLETREE application
//

#if !defined(AFX_SEARCHPEOPLETREE_H__802148F5_6E00_417A_8C58_35FB35C7CD7A__INCLUDED_)
#define AFX_SEARCHPEOPLETREE_H__802148F5_6E00_417A_8C58_35FB35C7CD7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSearchPeopleTreeApp:
// See SearchPeopleTree.cpp for the implementation of this class
//

class CSearchPeopleTreeApp : public CWinApp
{
public:
	CSearchPeopleTreeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchPeopleTreeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSearchPeopleTreeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHPEOPLETREE_H__802148F5_6E00_417A_8C58_35FB35C7CD7A__INCLUDED_)
