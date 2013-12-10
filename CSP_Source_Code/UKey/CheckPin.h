#if !defined(AFX_CHECKPIN_H__DCB7214D_2FB7_4C93_AE45_50941EE5211D__INCLUDED_)
#define AFX_CHECKPIN_H__DCB7214D_2FB7_4C93_AE45_50941EE5211D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckPin.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CCheckPin dialog

class CCheckPin : public CDialog
{
// Construction
public:
	CCheckPin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCheckPin)
	enum { IDD = IDD_DIALOG1 };
	CBitmapButton	m_btnChangePin;
	CBitmapButton	m_OK;
	CBitmapButton	m_Cancel;
	CString	m_Password;
	CString	m_Caption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckPin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCheckPin)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButton1();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_Language;
	HANDLE m_hUKey;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKPIN_H__DCB7214D_2FB7_4C93_AE45_50941EE5211D__INCLUDED_)
