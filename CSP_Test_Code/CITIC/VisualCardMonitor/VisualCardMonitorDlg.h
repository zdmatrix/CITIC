// VisualCardMonitorDlg.h : header file
//

#pragma once

#define WM_SHOWTASK (WM_USER+199) 

// CVisualCardMonitorDlg dialog
class CVisualCardMonitorDlg : public CDialog
{
// Construction
public:
	CVisualCardMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VISUALCARDMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	
	// User define functions
	afx_msg LRESULT onShowTask(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

private:
	BOOL m_isNotify;
	NOTIFYICONDATA  m_nid;

	DECLARE_MESSAGE_MAP()
};
