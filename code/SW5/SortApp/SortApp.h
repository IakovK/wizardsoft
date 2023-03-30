
// SortApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSortAppApp:
// See SortApp.cpp for the implementation of this class
//

class CSortAppApp : public CWinApp
{
public:
	CSortAppApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSortAppApp theApp;
