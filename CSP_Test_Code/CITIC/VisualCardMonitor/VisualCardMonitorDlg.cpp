// VisualCardMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VisualCardMonitor.h"
#include "VisualCardMonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVisualCardMonitorDlg dialog




CVisualCardMonitorDlg::CVisualCardMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVisualCardMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVisualCardMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVisualCardMonitorDlg, CDialog)
	
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_SHOWTASK,onShowTask)

	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CVisualCardMonitorDlg message handlers

BOOL CVisualCardMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

//	ShowWindow(SW_MINIMIZE);

	// TODO: Add extra initialization here
	m_isNotify = FALSE;
	if (!m_isNotify)  
	{  
		m_isNotify = TRUE;
		m_nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);  
		m_nid.hWnd=this->m_hWnd;  
		m_nid.uID=IDR_MAINFRAME;  
		m_nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;  
		strcpy(m_nid.szInfo, "");
		strcpy(m_nid.szTip, "");
		m_nid.uCallbackMessage=WM_SHOWTASK;//自定义的消息名称,注意:这里的消息是用户自定义消息   
		m_nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));  
		strcpy(m_nid.szTip, "HED多功能可视卡监视程序");//信息提示条为"计划任务提醒"   
		   
		Shell_NotifyIcon(NIM_ADD,&m_nid);//在托盘区添加图标   
	}  

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVisualCardMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVisualCardMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//wParam接收的是图标的ID，而lParam接收的是鼠标的行为   
LRESULT CVisualCardMonitorDlg::onShowTask(WPARAM wParam,LPARAM lParam)
{  
    if(wParam!=IDR_MAINFRAME)  
		return 1;  
    switch(lParam)  
    {  
        case WM_RBUTTONUP:          //右键起来时弹出快捷菜单，这里只有一个"关闭"   
	    {  

			LPPOINT lpoint=new tagPOINT;  
			::GetCursorPos(lpoint);         //得到鼠标位置   
			CMenu menu;  
			menu.CreatePopupMenu();             //声明一个弹出式菜单            
			menu.AppendMenu(MF_STRING,WM_DESTROY,_T("关闭"));   //增加菜单项"关闭"，点击则发送消息WM_DESTROY给主窗口（已隐藏），将程序结束。            
			menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this);//确定弹出式菜单的位置             
						SetForegroundWindow();  

			HMENU hmenu=menu.Detach();   //资源回收   
			menu.DestroyMenu();  
			delete lpoint;  
		 }break;  
		case WM_LBUTTONDBLCLK:    //双击左键的处理   
		{  
			this->ShowWindow(SW_SHOW);    //简单的显示主窗口完事儿   
		}break;  
    }  
    return 0;  
}

void CVisualCardMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}