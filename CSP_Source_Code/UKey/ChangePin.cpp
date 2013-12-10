// ChangePin.cpp : implementation file
//

#include "stdafx.h"
#include "UKey.h"
#include "ChangePin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangePin dialog


CChangePin::CChangePin(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePin)
	m_OldPin = _T("");
	m_NewPin1 = _T("");
	m_NewPin2 = _T("");
	m_NewCaption1 = _T("");
	m_NewCaption2 = _T("");
	m_OldCaption = _T("");
	//}}AFX_DATA_INIT
}


void CChangePin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePin)
	//DDX_Control(pDX, IDOK, m_OK);
	//DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Text(pDX, IDC_EDIT1, m_OldPin);
	DDX_Text(pDX, IDC_EDIT2, m_NewPin1);
	DDX_Text(pDX, IDC_EDIT3, m_NewPin2);
	DDX_Text(pDX, IDC_STATICNEWPIN1, m_NewCaption1);
	DDX_Text(pDX, IDC_STATICNEWPIN2, m_NewCaption2);
	DDX_Text(pDX, IDC_STATICOLDPIN, m_OldCaption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePin, CDialog)
	//{{AFX_MSG_MAP(CChangePin)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePin message handlers

void CChangePin::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	UpdateData(true);
	if (m_Language == 2)
	{
		m_NewCaption1 = "New Pin";
		m_NewCaption2 = "New Pin Confirm";
		m_OldCaption = "Old Pin";
		m_OK.SetWindowText("OK");
		m_Cancel.SetWindowText("Cancel");
	}
	else
	{
		m_NewCaption1 = "新口令";
		m_NewCaption2 = "新口令确认";
		m_OldCaption = "旧口令";
		m_OK.SetWindowText("确定");
		m_Cancel.SetWindowText("取消");
	}
	UpdateData(false);
	GetDlgItem(IDC_EDIT1)->SetFocus;
	
}

BOOL CChangePin::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	
	m_OK.LoadBitmaps(IDB_BITMAP_OK_U, IDB_BITMAP_OK_F);
	m_Cancel.LoadBitmaps(IDB_BITMAP_CANCEL_U, IDB_BITMAP_CANCEL_F);
	
	m_OK.SubclassDlgItem(IDOK, this);
	m_Cancel.SubclassDlgItem(IDCANCEL, this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
