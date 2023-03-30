
// SortAppDlg.h : header file
//

#pragma once
#include <atomic>
#include "NotifyCallback.h"

class CFileProcessor;

#define WM_UPDATECOUNT (WM_USER+100)

// CSortAppDlg dialog
class CSortAppDlg : public CDialogEx, public INotifyCallback
{
// Construction
public:
	CSortAppDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SORTAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:
	// INotifyCallback
	virtual void CompletedTaskCount(int completedCount, int totalCount);

	void EnableButtons();
	void ProcessFiles(CString inputPath, CString outputPath, int mode, int numThreads);
	void StopFp();
	CFileProcessor* m_fp;
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_nNumThreads;
	// TRUE - descending, FALSE - ascending
	BOOL m_bSortMode;
	CMFCEditBrowseCtrl m_ctlInputPath;
	CString m_strInputPath;
	CMFCEditBrowseCtrl m_ctlOutputPath;
	CString m_strOutputPath;
	std::atomic<int> m_numCompletedTasks;
	int m_numFiles;
	int m_numTotalFiles;
	afx_msg void OnClickedStart();
	afx_msg void OnClickedStop();
	CButton m_ctlStartBtn;
	CButton m_ctlStopBtn;
protected:
	afx_msg LRESULT OnUpdatecount(WPARAM wParam, LPARAM lParam);
};
