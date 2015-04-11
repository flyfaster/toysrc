// greenroad.h : main header file for the GREENROAD application
//

#if !defined(AFX_GREENROAD_H__B3C86741_A622_48CC_B0A9_66149AD3BC73__INCLUDED_)
#define AFX_GREENROAD_H__B3C86741_A622_48CC_B0A9_66149AD3BC73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGreenroadApp:
// See greenroad.cpp for the implementation of this class
//

class CGreenroadApp : public CWinApp
{
public:
	CGreenroadApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGreenroadApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGreenroadApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GREENROAD_H__B3C86741_A622_48CC_B0A9_66149AD3BC73__INCLUDED_)
