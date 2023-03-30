
// SortApp.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "SortApp.h"
#include "SortAppDlg.h"
#include "XMLLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSortAppApp

BEGIN_MESSAGE_MAP(CSortAppApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSortAppApp construction

CSortAppApp::CSortAppApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSortAppApp object

CSortAppApp theApp;


// CSortAppApp initialization

BOOL CSortAppApp::InitInstance()
{
	if (!AfxOleInit())
		return FALSE;

	TCHAR path[MAX_PATH];
	_tcscpy_s(path, _T("c:\\"));
	if (GetModuleFileName(NULL, path, MAX_PATH))
	{
		TCHAR* ptr = _tcsrchr(path, '\\');
		if (ptr != NULL)
		{
			*(ptr + 1) = 0;
		}
	}
	TCHAR logFilePath[MAX_PATH];
	_stprintf_s(logFilePath, _T("%sSortApp.log"), path);

	// log program start
	{
		XMLLog log(logFilePath);
		LogItem item1{ L"Entry", std::time(nullptr), GetCurrentProcessId() };
		log.Write(item1);
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

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
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CSortAppDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// log program exit
	{
		XMLLog log(logFilePath);
		LogItem item2{ L"Exit", std::time(nullptr), GetCurrentProcessId() };
		log.Write(item2);
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

