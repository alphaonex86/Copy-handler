/***************************************************************************
*   Copyright (C) 2001-2008 by Józef Starosczyk                           *
*   ixen@copyhandler.com                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License          *
*   (version 2) as published by the Free Software Foundation;             *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#ifndef __STATUSDLG_H__
#define __STATUSDLG_H__

#include "task.h"
#include "FFListCtrl.h"

#define WM_UPDATESTATUS WM_USER+6
#define WM_STATUSCLOSING WM_USER+12

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg dialog
class CStatusDlg : public ictranslate::CLanguageDialog
{
// Construction
public:
	CStatusDlg(CTaskArray* pTasks, CWnd* pParent = NULL);   // standard constructor
	~CStatusDlg();
	void PostCloseMessage();
	void SetBufferSizesString(UINT uiValue, int iIndex);
	void RefreshStatus();
	LPTSTR FormatTime(long lSeconds, LPTSTR lpszBuffer, size_t stMaxBufferSize);
	int GetImageFromStatus(UINT nStatus);

	void ApplyButtonsState();
	void ApplyDisplayDetails(bool bInitial=false);
	CTask* GetSelectedItemPointer();

	void AddTaskInfo(int nPos, CTask *pTask, DWORD dwCurrentTime);
	void EnableControls(bool bEnable=true);

	CTaskArray* m_pTasks;
	CTask* pSelectedItem;
	const CTask *m_pLastSelected;
	const CTask* m_pInitialSelection;

	TCHAR m_szData[_MAX_PATH];
	TCHAR m_szTimeBuffer1[40];
	TCHAR m_szTimeBuffer2[40];
	TCHAR m_szTimeBuffer3[40];

	__int64 m_i64LastProcessed;
	__int64 m_i64LastAllTasksProcessed;
	DWORD m_dwLastUpdate;

	LVITEM lvi;
	TASK_DISPLAY_DATA td;
	CString m_strTemp, m_strTemp2;

	CImageList m_images;

	static bool m_bLock;				// locker

// Dialog Data
	//{{AFX_DATA(CStatusDlg)
	enum { IDD = IDD_STATUS_DIALOG };
	CEdit	m_ctlErrors;
	CProgressCtrl	m_ctlCurrentProgress;
	CFFListCtrl	m_ctlStatusList;
	CProgressCtrl	m_ctlProgressAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnLanguageChanged();

	void PrepareResizableControls();

	// Generated message map functions
	//{{AFX_MSG(CStatusDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPauseButton();
	afx_msg void OnCancelButton();
	afx_msg void OnRollUnrollButton();
	afx_msg void OnSetPriorityButton();
	afx_msg void OnSetBuffersizeButton();
	afx_msg void OnStartAllButton();
	afx_msg void OnRestartButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnPauseAllButton();
	afx_msg void OnRestartAllButton();
	afx_msg void OnCancelAllButton();
	afx_msg void OnRemoveFinishedButton();
	afx_msg void OnKeydownStatusList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectionChanged(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/);
	virtual void OnCancel();
	afx_msg void OnAdvancedButton();
	afx_msg void OnPopupReplacePaths();
	afx_msg void OnShowLogButton();
	afx_msg void OnStickButton();
	afx_msg void OnResumeButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
