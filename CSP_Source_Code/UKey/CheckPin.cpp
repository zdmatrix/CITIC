// CheckPin.cpp : implementation file
//

#include "stdafx.h"
#include "UKey.h"
#include "CheckPin.h"
#include "UKeyAPI.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCheckPin dialog


CCheckPin::CCheckPin(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckPin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckPin)
	m_Password = _T("");
	m_Caption = _T("");
	//}}AFX_DATA_INIT
}


void CCheckPin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckPin)
	//DDX_Control(pDX, IDC_BUTTON1, m_btnChangePin);
	//DDX_Control(pDX, IDOK, m_OK);
	//DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Text(pDX, IDC_EDIT1, m_Password);
	DDX_Text(pDX, IDC_STATICCAPTION, m_Caption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckPin, CDialog)
	//{{AFX_MSG_MAP(CCheckPin)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckPin message handlers

void CCheckPin::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	UpdateData(true);
	if (m_Language == 2)
	{
		m_Caption = "Enter The Pin";
		m_OK.SetWindowText("OK");
		m_Cancel.SetWindowText("Cancel");
		m_btnChangePin.SetWindowText("Change Pin");
		
	}
	else
	{
		m_Caption = "请输入口令 ";
		m_OK.SetWindowText("确定");
		m_Cancel.SetWindowText("取消");
		m_btnChangePin.SetWindowText("修改口令");
	}
	UpdateData(false);	
	GetDlgItem(IDC_EDIT1)->SetFocus();
}

void CCheckPin::OnButton1() 
{
	// TODO: Add your control notification handler code here
	UKeyChangePIN(m_hUKey, m_Language);
}

BOOL CCheckPin::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_btnChangePin.LoadBitmaps(IDB_BITMAP_CP_U, IDB_BITMAP_CP_F);
	m_OK.LoadBitmaps(IDB_BITMAP_OK_U, IDB_BITMAP_OK_F);
	m_Cancel.LoadBitmaps(IDB_BITMAP_CANCEL_U, IDB_BITMAP_CANCEL_F);

	m_btnChangePin.SubclassDlgItem(IDC_BUTTON1, this);
	m_OK.SubclassDlgItem(IDOK, this);
	m_Cancel.SubclassDlgItem(IDCANCEL, this);

	this->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
