
// SortAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SortApp.h"
#include "SortAppDlg.h"
#include "afxdialogex.h"
#include "FileProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSortAppDlg dialog



CSortAppDlg::CSortAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SORTAPP_DIALOG, pParent)
	, m_nNumThreads(2)
	, m_bSortMode(FALSE)
	, m_strInputPath(_T(""))
	, m_strOutputPath(_T(""))
	, m_numFiles(0)
	, m_numTotalFiles(0)
	, m_fp(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSortAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NUMTHREADS, m_nNumThreads);
	DDV_MinMaxInt(pDX, m_nNumThreads, 2, 4);
	DDX_Check(pDX, IDC_SORTMODE, m_bSortMode);
	DDX_Control(pDX, IDC_INPUTPATH, m_ctlInputPath);
	DDX_Text(pDX, IDC_INPUTPATH, m_strInputPath);
	DDX_Control(pDX, IDC_OUTPUTPATH, m_ctlOutputPath);
	DDX_Text(pDX, IDC_OUTPUTPATH, m_strOutputPath);
	DDX_Text(pDX, IDC_NUMFILES, m_numFiles);
	DDX_Control(pDX, IDC_START, m_ctlStartBtn);
	DDX_Control(pDX, IDC_STOP, m_ctlStopBtn);
}

BEGIN_MESSAGE_MAP(CSortAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CSortAppDlg::OnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CSortAppDlg::OnClickedStop)
	ON_MESSAGE(WM_UPDATECOUNT, &CSortAppDlg::OnUpdatecount)
END_MESSAGE_MAP()


// CSortAppDlg message handlers

BOOL CSortAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_ctlInputPath.EnableFolderBrowseButton();
	m_ctlOutputPath.EnableFolderBrowseButton();

	EnableButtons();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSortAppDlg::EnableButtons()
{
	if (m_fp)
	{
		m_ctlStartBtn.EnableWindow(FALSE);
		m_ctlStopBtn.EnableWindow(TRUE);
	}
	else
	{
		m_ctlStartBtn.EnableWindow(TRUE);
		m_ctlStopBtn.EnableWindow(FALSE);
	}
}

void CSortAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSortAppDlg::OnPaint()
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
HCURSOR CSortAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSortAppDlg::OnClickedStart()
{
	UpdateData(TRUE);
	m_numFiles = 0;
	m_numCompletedTasks = 0;
	m_numTotalFiles = 0;
	UpdateData(FALSE);
	m_fp = new CFileProcessor(m_strInputPath, m_strOutputPath, m_bSortMode, m_nNumThreads, this);
	m_fp->run();
	EnableButtons();
}


void CSortAppDlg::OnClickedStop()
{
	StopFp();
}

void CSortAppDlg::CompletedTaskCount(int completedCount, int totalCount)
{
	m_numCompletedTasks = completedCount;
	m_numTotalFiles = totalCount;
	PostMessage(WM_UPDATECOUNT);
}


afx_msg LRESULT CSortAppDlg::OnUpdatecount(WPARAM wParam, LPARAM lParam)
{
	m_numFiles = m_numCompletedTasks;
	UpdateData(FALSE);
	if (m_numFiles == m_numTotalFiles)
	{
		StopFp();
	}
	return 0;
}

void CSortAppDlg::StopFp()
{
	m_fp->stop();
	delete m_fp;
	m_fp = NULL;
	EnableButtons();
}
