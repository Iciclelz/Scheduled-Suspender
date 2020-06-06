
// ScheduledSuspenderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScheduledSuspender.h"
#include "ScheduledSuspenderDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <strsafe.h>
#include <string>
#include "Suspender.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScheduledSuspenderDlg dialog



CScheduledSuspenderDlg::CScheduledSuspenderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SCHEDULEDSUSPENDER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CScheduledSuspenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CScheduledSuspenderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDC_NEW, &CScheduledSuspenderDlg::OnNewContextMenu)
	ON_COMMAND(IDC_EXIT, &CScheduledSuspenderDlg::OnExitContextMenu)
	ON_COMMAND(IDC_ABOUT, &CScheduledSuspenderDlg::OnAboutContextMenu)
	ON_BN_CLICKED(IDC_CHECK1, &CScheduledSuspenderDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CScheduledSuspenderDlg message handlers

BOOL CScheduledSuspenderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	TCHAR szMenuName[128];
	StringCchPrintf(szMenuName, STRSAFE_MAX_CCH, L" scheduled suspender | Process Id: %08X (%d)", GetCurrentProcessId(), static_cast<int32_t>(GetCurrentProcessId()));
	SetDlgItemText(IDC_STATIC, szMenuName);

	CSpinButtonCtrl *spin = reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN1));
	if (spin) {
		spin->SetBuddy(GetDlgItem(IDC_EDIT2));
		spin->SetRange(1, UD_MAXVAL);
	}

	reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT2))->SetWindowText(TEXT("1000"));

	CComboBox* combobox = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO1));
	if (combobox) {
		combobox->AddString(TEXT("suspend all threads"));
		combobox->AddString(TEXT("suspend process"));
		combobox->AddString(TEXT("debug suspend process"));

		combobox->SetCurSel(0);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScheduledSuspenderDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScheduledSuspenderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



HBRUSH CScheduledSuspenderDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired

	//
	pDC->SetBkMode(TRANSPARENT);

	CString windowtext;
	pWnd->GetWindowTextW(windowtext);


	switch (nCtlColor) {
	case CTLCOLOR_BTN:
		return CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	case CTLCOLOR_DLG:
		return CreateSolidBrush(RGB(24, 24, 24));
	case CTLCOLOR_EDIT:
		pDC->SetTextColor(RGB(255, 255, 255));
		return CreateSolidBrush(RGB(0, 0, 0));
		break;
	case CTLCOLOR_LISTBOX:
		break;
	case CTLCOLOR_MSGBOX:
		break;
	case CTLCOLOR_SCROLLBAR:
		break;
	case CTLCOLOR_STATIC:
		if (windowtext.Find(L"Process Id") > -1) {
			pDC->SetTextColor(RGB(255, 255, 255));
		}
		if (pWnd->GetDlgCtrlID() == IDC_CHECK1)
			return CreateSolidBrush(RGB(24, 24, 24));
		else if (windowtext.Compare(L"technique:") == 0
			|| windowtext.Compare(L"process name: ") == 0
			|| windowtext.Compare(L"delay (ms): ") == 0) {
			pDC->SetTextColor(RGB(255, 255, 255));
return CreateSolidBrush(RGB(24, 24, 24));
		}
		else if (windowtext.Compare(L"") == 0)
			return CreateSolidBrush(RGB(255, 0, 0));
		else if (windowtext.Compare(L"automatic suspend") == 0) {
			pDC->SetTextColor(RGB(255, 255, 255));
			return CreateSolidBrush(RGB(24, 24, 24));
		}
		else
			return CreateSolidBrush(RGB(0, 0, 0));
	}




	return hbr;
}


void CScheduledSuspenderDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (nFlags == MK_LBUTTON) {
		if (moving) {
			CPoint pos;
			GetCursorPos(&pos);
			SetWindowPos(NULL, pos.x - pt.x, pos.y - pt.y, 0, 0, SWP_NOSIZE);
		}
	}
	else {
		moving = false;
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CScheduledSuspenderDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	moving = true;
	pt = point;

	CRect window;
	GetClientRect(window);

	CRect close_region(window.right - 20, 0, window.right, 15);
	if (PtInRect(close_region, point)) {
		PostQuitMessage(0);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CScheduledSuspenderDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	moving = false;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CScheduledSuspenderDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here

	CMenu menu;
	menu.CreatePopupMenu();
	//menu.LoadMenuW(IDR_MENU1);
	menu.AppendMenuW(MF_STRING | MF_ENABLED, IDC_NEW, L"new");
	menu.AppendMenuW(MF_STRING | MF_ENABLED, IDC_EXIT, L"exit");
	menu.AppendMenuW(MF_SEPARATOR | MF_ENABLED, 0, L"");
	menu.AppendMenuW(MF_STRING | MF_ENABLED, IDC_ABOUT, L"about scheduled suspender");

	//menu.EnableMenuItem(ID_PACKETEDITOR_SEND, MF_ENABLED);
	//menu.EnableMenuItem(ID_PACKETEDITOR_RECEIVE, MF_ENABLED);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, this);
}


void CScheduledSuspenderDlg::OnNewContextMenu() {
	wchar_t szPath[260];
	GetModuleFileNameW(NULL, szPath, MAX_PATH);
	ShellExecuteW(NULL, L"OPEN", szPath, NULL, NULL, SW_SHOWNORMAL);
}
void CScheduledSuspenderDlg::OnExitContextMenu() {
	PostQuitMessage(0);
}
void CScheduledSuspenderDlg::OnAboutContextMenu() {
	MessageBox(L"Scheduled Suspender				1.0.3.0\n\nCreated by Ryukuo", L"Scheduled Suspender", MB_OK | MB_ICONINFORMATION | MB_APPLMODAL | MB_SETFOREGROUND);
}

bool is_button_checked = false;
void CScheduledSuspenderDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here

	CString process_name;
	GetDlgItem(IDC_EDIT1)->GetWindowText(process_name);
	
	if (process_name.Trim().Compare(TEXT("")) == 0) {
		reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK1))->SetCheck(BST_UNCHECKED);
		return;
	}

	CString szDelay;
	GetDlgItem(IDC_EDIT2)->GetWindowText(szDelay);

	int delay = 0;
	wscanf_s(szDelay, "%d", &delay);

	SUSPEND_METHOD method;
	switch (reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO1))->GetCurSel()) {
	case 0:
		method = THREADS;
		break;
	case 1:
		method = NT;
		break;
	case 2:
		method = DBG;
		break;
	}

	if ((reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK1))->GetState() & BST_CHECKED) != 0) {
		GetDlgItem(IDC_COMBO1)->EnableWindow(0);
		GetDlgItem(IDC_EDIT1)->EnableWindow(0);
		GetDlgItem(IDC_EDIT2)->EnableWindow(0);

		tagCSuspender *tag = new tagCSuspender();
		is_button_checked = true;
		tag->process_name = std::wstring(process_name);
		tag->method = method;
		tag->delay = delay;
		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&CSuspender::watch_for_process), tag, 0, 0);
		//checked
	}
	else {
		//uncheced
		is_button_checked = false;
		GetDlgItem(IDC_COMBO1)->EnableWindow();
		GetDlgItem(IDC_EDIT1)->EnableWindow();
		GetDlgItem(IDC_EDIT2)->EnableWindow();

		CSuspender::ResumeProcess(CSuspender::GetProcessIdByName(process_name), method);
		CSuspender::MessageDialog(false, process_name);
	}
}
