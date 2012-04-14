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

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

#include "TrayIcon.h"
#include "structs.h"
#include "task.h"

class CMiniViewDlg;
class CStatusDlg;

class CMainWnd : public CWnd
{
public:
	CMainWnd();
	DECLARE_DYNCREATE(CMainWnd)

	BOOL Create();

// Attributes
public:
	CTrayIcon m_ctlTray;
	
	CTaskArray m_tasks;
	chcore::IFeedbackHandlerFactory* m_pFeedbackFactory;

	CMiniViewDlg* m_pdlgMiniView;
	CStatusDlg* m_pdlgStatus;

	DWORD m_dwLastTime;
	UINT m_uiTaskbarRestart;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainWnd();

// Generated message map functions
protected:
	BOOL RegisterClass();
	int ShowTrayIcon();
	void ShowStatusWindow(const CTask* pSelect=NULL);
	void PrepareToExit();
	//{{AFX_MSG(CMainWnd)
	afx_msg void OnPopupShowStatus();
	afx_msg void OnPopupShowOptions();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowMiniView();
	afx_msg void OnPopupCustomCopy();
	afx_msg void OnAppAbout();
	afx_msg void OnPopupMonitoring();
	afx_msg void OnPopupShutafterfinished();
	afx_msg void OnPopupRegisterdll();
	afx_msg void OnPopupUnregisterdll();
	afx_msg void OnAppExit();
	afx_msg void OnPopupHelp();
	//}}AFX_MSG
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPopupCheckForUpdates();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
