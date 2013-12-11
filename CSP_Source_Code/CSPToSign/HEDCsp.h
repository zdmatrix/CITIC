// HEDCsp.h : main header file for the HEDCSP DLL
//

#if !defined(AFX_HEDCSP_H__F8613AA7_675C_4672_8D28_5A34DCD5F189__INCLUDED_)
#define AFX_HEDCSP_H__F8613AA7_675C_4672_8D28_5A34DCD5F189__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHEDCspApp
// See HEDCsp.cpp for the implementation of this class
//

class CHEDCspApp : public CWinApp
{
public:
	CHEDCspApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHEDCspApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHEDCspApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEDCSP_H__F8613AA7_675C_4672_8D28_5A34DCD5F189__INCLUDED_)
