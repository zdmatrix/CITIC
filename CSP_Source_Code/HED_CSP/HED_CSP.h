// HED_CSP.h : main header file for the HED_CSP DLL
//

#if !defined(AFX_HED_CSP_H__4B083A82_2F02_438F_BD03_0AC3F9EC6037__INCLUDED_)
#define AFX_HED_CSP_H__4B083A82_2F02_438F_BD03_0AC3F9EC6037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHED_CSPApp
// See HED_CSP.cpp for the implementation of this class
//

class CHED_CSPApp : public CWinApp
{
public:
	CHED_CSPApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHED_CSPApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHED_CSPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HED_CSP_H__4B083A82_2F02_438F_BD03_0AC3F9EC6037__INCLUDED_)
