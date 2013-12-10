// UKey.h : main header file for the UKEY DLL
//

#if !defined(AFX_UKEY_H__C0348B9C_75A3_479F_A020_33312D9B4F05__INCLUDED_)
#define AFX_UKEY_H__C0348B9C_75A3_479F_A020_33312D9B4F05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUKeyApp
// See UKey.cpp for the implementation of this class
//

class CUKeyApp : public CWinApp
{
public:
	CUKeyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUKeyApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CUKeyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UKEY_H__C0348B9C_75A3_479F_A020_33312D9B4F05__INCLUDED_)
