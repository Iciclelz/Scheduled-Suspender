
// Scheduled Suspender.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ScheduledSuspender.h"
#include "ScheduledSuspenderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScheduledSuspenderApp

BEGIN_MESSAGE_MAP(CScheduledSuspenderApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CScheduledSuspenderApp construction

CScheduledSuspenderApp::CScheduledSuspenderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CScheduledSuspenderApp object

CScheduledSuspenderApp theApp;


// CScheduledSuspenderApp initialization

BOOL CScheduledSuspenderApp::InitInstance()
{
	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization

	CScheduledSuspenderDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

