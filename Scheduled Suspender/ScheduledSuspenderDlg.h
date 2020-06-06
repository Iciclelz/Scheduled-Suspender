
// ScheduledSuspenderDlg.h : header file
//

#pragma once


// CScheduledSuspenderDlg dialog
class CScheduledSuspenderDlg : public CDialogEx
{
private:
	bool moving = false;
	CPoint pt;
// Construction
public:
	CScheduledSuspenderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCHEDULEDSUSPENDER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnNewContextMenu();
	afx_msg void OnExitContextMenu();
	afx_msg void OnAboutContextMenu();
	afx_msg void OnBnClickedCheck1();
};
