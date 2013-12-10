#if !defined(AFX_CHANGEPIN_H__57341A05_6559_460E_9788_91A530CBE1FE__INCLUDED_)
#define AFX_CHANGEPIN_H__57341A05_6559_460E_9788_91A530CBE1FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangePin.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CChangePin dialog

class CChangePin : public CDialog
{
// Construction
public:
	CChangePin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangePin)
	enum { IDD = IDD_DIALOG2 };
	CBitmapButton	m_OK;
	CBitmapButton	m_Cancel;
	CString	m_OldPin;
	CString	m_NewPin1;
	CString	m_NewPin2;
	CString	m_NewCaption1;
	CString	m_NewCaption2;
	CString	m_OldCaption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangePin)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_Language;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEPIN_H__57341A05_6559_460E_9788_91A530CBE1FE__INCLUDED_)
