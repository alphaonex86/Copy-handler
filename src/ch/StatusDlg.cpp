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
#include "stdafx.h"
#include "ch.h"
#include "resource.h"
#include "StatusDlg.h"
#include "BufferSizeDlg.h"
#include "ReplacePathsDlg.h"
#include "StringHelpers.h"
#include "StaticEx.h"
#include "Structs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CStatusDlg::m_bLock=false;

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg dialog

CStatusDlg::CStatusDlg(CTaskArray* pTasks, CWnd* pParent /*=NULL*/)
	: ictranslate::CLanguageDialog(CStatusDlg::IDD, pParent, &m_bLock)
{
	//{{AFX_DATA_INIT(CStatusDlg)
	//}}AFX_DATA_INIT
	m_i64LastProcessed=0;
	m_i64LastAllTasksProcessed=0;
	m_pTasks=pTasks;
	m_dwLastUpdate=0;

	RegisterStaticExControl(AfxGetInstanceHandle());
}

CStatusDlg::~CStatusDlg()
{

}

void CStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CLanguageDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusDlg)
	DDX_Control(pDX, IDC_ERRORS_EDIT, m_ctlErrors);
	DDX_Control(pDX, IDC_TASK_PROGRESS, m_ctlCurrentProgress);
	DDX_Control(pDX, IDC_STATUS_LIST, m_ctlStatusList);
	DDX_Control(pDX, IDC_ALL_PROGRESS, m_ctlProgressAll);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStatusDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CStatusDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PAUSE_BUTTON, OnPauseButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_BN_CLICKED(IDC_ROLL_UNROLL_BUTTON, OnRollUnrollButton)
	ON_BN_CLICKED(IDC_SET_PRIORITY_BUTTON, OnSetPriorityButton)
	ON_BN_CLICKED(IDC_SET_BUFFERSIZE_BUTTON, OnSetBuffersizeButton)
	ON_BN_CLICKED(IDC_START_ALL_BUTTON, OnStartAllButton)
	ON_BN_CLICKED(IDC_RESTART_BUTTON, OnRestartButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_PAUSE_ALL_BUTTON, OnPauseAllButton)
	ON_BN_CLICKED(IDC_RESTART_ALL_BUTTON, OnRestartAllButton)
	ON_BN_CLICKED(IDC_CANCEL_ALL_BUTTON, OnCancelAllButton)
	ON_BN_CLICKED(IDC_REMOVE_FINISHED_BUTTON, OnRemoveFinishedButton)
	ON_NOTIFY(LVN_KEYDOWN, IDC_STATUS_LIST, OnKeydownStatusList)
	ON_NOTIFY(LVN_CHANGEDSELECTION, IDC_STATUS_LIST, OnSelectionChanged)
	ON_BN_CLICKED(IDC_ADVANCED_BUTTON, OnAdvancedButton)
	ON_COMMAND(ID_POPUP_REPLACE_PATHS, OnPopupReplacePaths)
	ON_BN_CLICKED(IDC_SHOW_LOG_BUTTON, OnShowLogButton)
	ON_BN_CLICKED(IDC_STICK_BUTTON, OnStickButton)
	ON_BN_CLICKED(IDC_RESUME_BUTTON, OnResumeButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg message handlers

BOOL CStatusDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	PrepareResizableControls();

	// get size of list ctrl
	CRect rcList;
	m_ctlStatusList.GetWindowRect(&rcList);
	int iWidth=rcList.Width();

	// set additional styles
	m_ctlStatusList.SetExtendedStyle(m_ctlStatusList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	// add columns
	LVCOLUMN lvc;
	lvc.mask=LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt=LVCFMT_LEFT;

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNSTATUS_STRING); /*_T("Status")*/;
    lvc.cchTextMax = lstrlen(lvc.pszText); 
    lvc.cx = static_cast<int>(0.27*iWidth);
	lvc.iSubItem=-1;
	m_ctlStatusList.InsertColumn(1, &lvc);

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNSOURCE_STRING);/*_T("File");*/
    lvc.cchTextMax = lstrlen(lvc.pszText); 
    lvc.cx = static_cast<int>(0.3*iWidth);
	lvc.iSubItem=0;
	m_ctlStatusList.InsertColumn(2, &lvc);

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNDESTINATION_STRING);/*_T("To:");*/
    lvc.cchTextMax = lstrlen(lvc.pszText);
    lvc.cx = static_cast<int>(0.27*iWidth);
	lvc.iSubItem=1;
	m_ctlStatusList.InsertColumn(3, &lvc);

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNPROGRESS_STRING);/*_T("Progress");*/
    lvc.cchTextMax = lstrlen(lvc.pszText);
    lvc.cx = static_cast<int>(0.15*iWidth);
	lvc.iSubItem=2;
	m_ctlStatusList.InsertColumn(4, &lvc);

	// images
	m_images.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 3);
	m_images.Add(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_WORKING_ICON)));
	m_images.Add(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ERROR_ICON)));
	m_images.Add(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_PAUSED_ICON)));
	m_images.Add(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_FINISHED_ICON)));
	m_images.Add(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_CANCELLED_ICON)));
	m_images.Add(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_WAITING_ICON)));

	m_ctlStatusList.SetImageList(&m_images, LVSIL_SMALL);

	// set fixed progresses ranges
	m_ctlCurrentProgress.SetRange32(0, 100);
	m_ctlProgressAll.SetRange32(0, 100);

	// change the size of a dialog
	ApplyDisplayDetails(true);
//	ApplyButtonsState();
//	EnableControls(false);

	// refresh data
	RefreshStatus();

	// select needed element
	int i=0;
	while (i < m_pTasks->GetSize())
	{
		if (m_pTasks->GetAt(i) == m_pInitialSelection)
		{
			m_ctlStatusList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			break;
		}

		i++;
	};

	// refresh data timer
	SetTimer(777, (UINT)GetConfig().get_signed_num(PP_STATUSREFRESHINTERVAL), NULL);

	return TRUE;
}

void CStatusDlg::EnableControls(bool bEnable)
{
	// enable/disable controls
	GetDlgItem(IDC_SET_BUFFERSIZE_BUTTON)->EnableWindow(bEnable);
	GetDlgItem(IDC_SET_PRIORITY_BUTTON)->EnableWindow(bEnable);

	if (!bEnable)
	{
		// get rid of text id disabling
		GetDlgItem(IDC_OPERATION_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYOPERATIONTEXT_STRING));
		GetDlgItem(IDC_SOURCE_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYSOURCETEXT_STRING));
		GetDlgItem(IDC_DESTINATION_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYDESTINATIONTEXT_STRING));
		GetDlgItem(IDC_BUFFERSIZE_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYBUFFERSIZETEXT_STRING));
		GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYPRIORITYTEXT_STRING));
		
		const TCHAR *pszText=GetResManager().LoadString(IDS_EMPTYERRORTEXT_STRING);
		m_ctlErrors.GetWindowText(m_strTemp);
		if (m_strTemp != pszText)
			m_ctlErrors.SetWindowText(pszText);
		
		GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYPROCESSEDTEXT_STRING));
		GetDlgItem(IDC_TRANSFER_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYTRANSFERTEXT_STRING));
		GetDlgItem(IDC_TIME_STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYTIMETEXT_STRING));
		GetDlgItem(IDC_ASSOCIATEDFILES__STATIC)->SetWindowText(GetResManager().LoadString(IDS_EMPTYASSOCFILE_STRING));

		m_ctlCurrentProgress.SetPos(0);
	}
}

void CStatusDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == 777)	// refreshing data
	{
		// turn off timer for some time
		KillTimer(777);

		RefreshStatus();

		// reenable
		SetTimer(777, (UINT)GetConfig().get_signed_num(PP_STATUSREFRESHINTERVAL), NULL);
	}

	CLanguageDialog::OnTimer(nIDEvent);
}

void CStatusDlg::AddTaskInfo(int nPos, CTask *pTask, DWORD dwCurrentTime)
{
	// index to string
	_itot(nPos, m_szData, 10);

	// get data snapshot from task
	pTask->GetSnapshot(&td);

	// index subitem
	lvi.mask=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvi.iItem=nPos;
	lvi.iSubItem=0;
	lvi.pszText=td.m_szStatusText;
	lvi.cchTextMax=lstrlen(lvi.pszText);
	lvi.lParam=reinterpret_cast<LPARAM>(pTask);
	lvi.iImage=GetImageFromStatus(td.m_uiStatus);
	if (nPos < m_ctlStatusList.GetItemCount())
		m_ctlStatusList.SetItem(&lvi);
	else
		m_ctlStatusList.InsertItem(&lvi);

	// status subitem
	lvi.mask=LVIF_TEXT;	// zmieñ maskê
	lvi.iSubItem=1;
	m_strTemp=td.m_fi.GetFileName();
	lvi.pszText=m_strTemp.GetBuffer(0);
	m_strTemp.ReleaseBuffer();
	lvi.cchTextMax=lstrlen(lvi.pszText);
	m_ctlStatusList.SetItem(&lvi);

	// insert 'file' subitem
	lvi.iSubItem=2;
	m_strTemp=td.m_pdpDestPath->GetPath();
	lvi.pszText=m_strTemp.GetBuffer(0);
	m_strTemp.ReleaseBuffer();
	lvi.cchTextMax=lstrlen(lvi.pszText);
	m_ctlStatusList.SetItem(&lvi);

	// insert dest subitem
	lvi.iSubItem=3;
	_itot( td.m_nPercent, m_szData, 10 );
	_tcscat(m_szData, _T(" %"));
	lvi.pszText=m_szData;
	lvi.cchTextMax=lstrlen(lvi.pszText);
	m_ctlStatusList.SetItem(&lvi);

	// right side update
	if (pTask == pSelectedItem && GetConfig().get_bool(PP_STATUSSHOWDETAILS))
	{
		// data that can be changed by a thread
		GetDlgItem(IDC_OPERATION_STATIC)->SetWindowText(td.m_szStatusText);	// operation
		GetDlgItem(IDC_SOURCE_STATIC)->SetWindowText(td.m_fi.GetFullFilePath());	// src object
		
		// error message
		if ( (td.m_uiStatus & ST_WORKING_MASK) == ST_ERROR )
		{
			m_ctlErrors.GetWindowText(m_strTemp);
			if (m_strTemp != td.m_strErrorDesc)
				m_ctlErrors.SetWindowText(td.m_strErrorDesc);
		}
		else
		{
			const TCHAR *pszText=GetResManager().LoadString(IDS_EMPTYERRORTEXT_STRING);
			
			m_ctlErrors.GetWindowText(m_strTemp2);
			if (m_strTemp2 != pszText)
				m_ctlErrors.SetWindowText(pszText);
		}

		// count of processed data/overall count of data
		_sntprintf(m_szData, _MAX_PATH, _T("%d/%d ("), td.m_iIndex, td.m_iSize);
		m_strTemp=CString(m_szData);
		m_strTemp+=GetSizeString(td.m_ullProcessedSize, m_szData, _MAX_PATH)+CString(_T("/"));
		m_strTemp+=GetSizeString(td.m_ullSizeAll, m_szData, _MAX_PATH)+CString(_T(")"));
		_sntprintf(m_szData, _MAX_PATH, _T(" (%s%d/%d)"), GetResManager().LoadString(IDS_CURRENTPASS_STRING), td.m_ucCurrentCopy, td.m_ucCopies);
		m_strTemp+=m_szData;
		GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(m_strTemp);
		
		// transfer
		if (m_i64LastProcessed == 0)	// if first time - show average
			m_strTemp=GetSizeString( td.m_lTimeElapsed ? td.m_ullProcessedSize/td.m_lTimeElapsed : 0, m_szData, _MAX_PATH);	// last avg
		else
			if ( (dwCurrentTime-m_dwLastUpdate) != 0)
				m_strTemp=GetSizeString( (static_cast<double>(td.m_ullProcessedSize) - static_cast<double>(m_i64LastProcessed))/(static_cast<double>(dwCurrentTime-m_dwLastUpdate)/1000.0), m_szData, _MAX_PATH);
			else
				m_strTemp=GetSizeString( 0ULL, m_szData, _MAX_PATH);

		// avg transfer
		GetDlgItem(IDC_TRANSFER_STATIC)->SetWindowText(m_strTemp+_T("/s (")+CString(GetResManager().LoadString(IDS_AVERAGEWORD_STRING))
			+CString(GetSizeString(td.m_lTimeElapsed ? td.m_ullProcessedSize/td.m_lTimeElapsed : 0, m_szData, _MAX_PATH))+_T("/s )")
			);
		
		// elapsed time / estimated total time (estimated time left)
		FormatTime(td.m_lTimeElapsed, m_szTimeBuffer1, 40);
		long lTotalTime = (td.m_ullProcessedSize == 0) ? 0 : (long)(td.m_ullSizeAll * td.m_lTimeElapsed / td.m_ullProcessedSize);
		FormatTime(lTotalTime, m_szTimeBuffer2, 40);
		FormatTime(max(0, lTotalTime - td.m_lTimeElapsed), m_szTimeBuffer3, 40);

		_sntprintf(m_szData, _MAX_PATH, _T("%s / %s (%s)"), m_szTimeBuffer1, m_szTimeBuffer2, m_szTimeBuffer3);
		GetDlgItem(IDC_TIME_STATIC)->SetWindowText(m_szData);

		// remember current processed data (used for calculating transfer)
		m_i64LastProcessed=td.m_ullProcessedSize;

		// set progress
		m_ctlCurrentProgress.SetPos(td.m_nPercent);

		SetBufferSizesString(td.m_pbsSizes->m_auiSizes[td.m_iCurrentBufferIndex], td.m_iCurrentBufferIndex);

		// data that can be changed only by user from outside the thread
		// refresh only when there are new selected item
//		if (pTask != m_pLastSelected)
		{
			GetDlgItem(IDC_DESTINATION_STATIC)->SetWindowText(td.m_pdpDestPath->GetPath());
			GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(td.m_nPriority)));
			GetDlgItem(IDC_ASSOCIATEDFILES__STATIC)->SetWindowText(*td.m_pstrUniqueName+_T(".atd (.atp, .log)"));
		}

		// refresh m_pLastSelected
		m_pLastSelected=pTask;
	}
}

void CStatusDlg::OnSetBuffersizeButton()
{
	CTask* pTask;
	if ( (pTask=GetSelectedItemPointer()) == NULL )
		return;

	CBufferSizeDlg dlg;
	dlg.m_bsSizes=*pTask->GetBufferSizes();
	dlg.m_iActiveIndex=pTask->GetCurrentBufferIndex();
	if (dlg.DoModal() == IDOK)
	{
		// if the task has been deleted - skip
		if ( pTask != GetSelectedItemPointer() )
		{
			TRACE("Task were finished and deleted when trying to change buffer sizes");
			return;
		}
		
		TRACE("bOnlyDefault=%d\n", dlg.m_bsSizes.m_bOnlyDefault);
		pTask->SetBufferSizes(&dlg.m_bsSizes);
	}
}

CTask* CStatusDlg::GetSelectedItemPointer()
{
//	TRACE("inside GetSelectedItemPointer()\n");
	// returns ptr to a CTask for a given element in listview
	if (m_ctlStatusList.GetSelectedCount() == 1)
	{
		POSITION pos=m_ctlStatusList.GetFirstSelectedItemPosition();
		int nPos=m_ctlStatusList.GetNextSelectedItem(pos);
		CTask* pSelectedItem=reinterpret_cast<CTask*>(m_ctlStatusList.GetItemData(nPos));
//		if (AfxIsValidAddress(pSelectedItem, sizeof(CTask)))
		return pSelectedItem;
	}
//	TRACE("exiting GetSelectedItemPointer()\n");

	return NULL;
}

void CStatusDlg::OnRollUnrollButton() 
{
	// change settings in config dialog
	GetConfig().set_bool(PP_STATUSSHOWDETAILS, !GetConfig().get_bool(PP_STATUSSHOWDETAILS));

	ApplyDisplayDetails();
}

void CStatusDlg::ApplyDisplayDetails(bool bInitial)
{
	// get coord of screen and window
	CRect rcScreen, rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	GetWindowRect(&rect);

	bool bDetails=GetConfig().get_bool(PP_STATUSSHOWDETAILS);

	// stick cause
	if (rect.right == rcScreen.right && rect.bottom == rcScreen.bottom)
		bInitial=true;

	GetDlgItem(IDC_ROLL_UNROLL_BUTTON)->SetWindowText(bDetails ? _T("<<") : _T(">>"));
	
	CRect list, progress;
	m_ctlProgressAll.GetWindowRect(&progress);
	ScreenToClient(&progress);
	m_ctlStatusList.GetWindowRect(&list);
	ScreenToClient(&list);

	// set dialog size
	CRect destRect;
	if (!bInitial)
	{
		destRect.left=0;
		destRect.top=0;
		destRect.right=bDetails ? progress.right+list.left+3*GetSystemMetrics(SM_CXBORDER) : list.right+list.left+3*GetSystemMetrics(SM_CXBORDER);
		destRect.bottom=rect.Height();
		SetWindowPos(NULL, destRect.left, destRect.top, destRect.right, destRect.bottom, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	else
	{
		SetWindowPos(NULL, rcScreen.right-(bDetails ? progress.right+list.left+3*GetSystemMetrics(SM_CXBORDER) : list.right+list.left+3*GetSystemMetrics(SM_CXBORDER)),
			rcScreen.bottom-rect.Height(), (bDetails ? progress.right+list.left+3*GetSystemMetrics(SM_CXBORDER) : list.right+list.left+3*GetSystemMetrics(SM_CXBORDER)),
			rect.Height(), SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
}

void CStatusDlg::ApplyButtonsState()
{
	// remember ptr to CTask
	pSelectedItem=GetSelectedItemPointer();
	bool bShowLog=GetConfig().get_bool(PP_CMCREATELOG);

	// set status of buttons pause/resume/cancel
	if (pSelectedItem != NULL)
	{
		GetDlgItem(IDC_RESTART_BUTTON)->EnableWindow(true);
		GetDlgItem(IDC_SHOW_LOG_BUTTON)->EnableWindow(bShowLog);
		GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(true);
		
		if (pSelectedItem->GetStatus(ST_STEP_MASK) == ST_FINISHED
			|| pSelectedItem->GetStatus(ST_STEP_MASK) == ST_CANCELLED)
		{
			GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(false);
			GetDlgItem(IDC_PAUSE_BUTTON)->EnableWindow(false);
			GetDlgItem(IDC_RESUME_BUTTON)->EnableWindow(false);
		}	
		else
		{
			// pause/resume
			if (pSelectedItem->GetStatus(ST_WORKING_MASK) & ST_PAUSED)
			{
				GetDlgItem(IDC_PAUSE_BUTTON)->EnableWindow(false);
				GetDlgItem(IDC_RESUME_BUTTON)->EnableWindow(true);
			}
			else
			{
				GetDlgItem(IDC_PAUSE_BUTTON)->EnableWindow(true);
				if (pSelectedItem->GetStatus(ST_WAITING_MASK) & ST_WAITING)
					GetDlgItem(IDC_RESUME_BUTTON)->EnableWindow(true);
				else
					GetDlgItem(IDC_RESUME_BUTTON)->EnableWindow(false);
			}
			
			GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(true);
		}
	}
	else
	{
		GetDlgItem(IDC_SHOW_LOG_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_PAUSE_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_RESUME_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_RESTART_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(false);
	}
}

void CStatusDlg::OnSetPriorityButton() 
{
	CMenu menu;
	HMENU hMenu=GetResManager().LoadMenu(MAKEINTRESOURCE(IDR_PRIORITY_MENU));
	if (!menu.Attach(hMenu))
	{
		DestroyMenu(hMenu);
		return;
	}
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	if(pPopup)
	{
		// set point in which to set menu
		CRect rect;
		GetDlgItem(IDC_SET_PRIORITY_BUTTON)->GetWindowRect(&rect);

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.right+1, rect.top, this);
	}
}

BOOL CStatusDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (HIWORD(wParam) == 0)
	{
		if (LOWORD(wParam) >= ID_POPUP_TIME_CRITICAL && LOWORD(wParam) <= ID_POPUP_IDLE)
		{
			// processing priority
			if ( (pSelectedItem=GetSelectedItemPointer()) == NULL )
				return ictranslate::CLanguageDialog::OnCommand(wParam, lParam);
			
			switch (LOWORD(wParam))
			{
			case ID_POPUP_TIME_CRITICAL:
				pSelectedItem->SetPriority(THREAD_PRIORITY_TIME_CRITICAL);
				GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(THREAD_PRIORITY_TIME_CRITICAL)));
				break;
			case ID_POPUP_HIGHEST:
				pSelectedItem->SetPriority(THREAD_PRIORITY_HIGHEST);
				GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(THREAD_PRIORITY_HIGHEST)));
				break;
			case ID_POPUP_ABOVE_NORMAL:
				pSelectedItem->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL);
				GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(THREAD_PRIORITY_ABOVE_NORMAL)));
				break;
			case ID_POPUP_NORMAL:
				pSelectedItem->SetPriority(THREAD_PRIORITY_NORMAL);
				GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(THREAD_PRIORITY_NORMAL)));
				break;
			case ID_POPUP_BELOW_NORMAL:
				pSelectedItem->SetPriority(THREAD_PRIORITY_BELOW_NORMAL);
				GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(THREAD_PRIORITY_BELOW_NORMAL)));
				break;
			case ID_POPUP_LOWEST:
				pSelectedItem->SetPriority(THREAD_PRIORITY_LOWEST);
				GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(THREAD_PRIORITY_LOWEST)));
				break;
			case ID_POPUP_IDLE:
				pSelectedItem->SetPriority(THREAD_PRIORITY_IDLE);
				GetDlgItem(IDC_PRIORITY_STATIC)->SetWindowText(GetResManager().LoadString(IDS_PRIORITY0_STRING+PriorityToIndex(THREAD_PRIORITY_IDLE)));
				break;
			}
		}
	}
	return ictranslate::CLanguageDialog::OnCommand(wParam, lParam);
}

void CStatusDlg::OnPauseButton() 
{
	CTask* pTask;
	if ( (pTask=GetSelectedItemPointer()) == NULL )
		return;

	TRACE("PauseProcessing call...\n");
	pTask->PauseProcessing();

	RefreshStatus();
}

void CStatusDlg::OnResumeButton() 
{
	CTask* pTask;
	if ( (pTask=GetSelectedItemPointer()) == NULL )
		return;

	TRACE("ResumeProcessing call ");
	if (pTask->GetStatus(ST_WAITING_MASK) & ST_WAITING)
	{
		TRACE("by setting force flag\n");
		pTask->SetForceFlag();
	}
	else
	{
		TRACE("by function ResumeProcessing\n");
		pTask->ResumeProcessing();
	}

	RefreshStatus();
}

void CStatusDlg::OnCancelButton() 
{
	CTask* pTask;
	if ( (pTask=GetSelectedItemPointer()) != NULL )
	{
		TRACE("CancelProcessing call...\n");
		pTask->CancelProcessing();
	}
	RefreshStatus();
}

void CStatusDlg::OnRestartButton() 
{
	CTask* pTask;
	if ( (pTask=GetSelectedItemPointer()) == NULL )
		return;

	TRACE("RestartProcessing call...\n");
	pTask->RestartProcessing();
	RefreshStatus();
}

void CStatusDlg::OnDeleteButton() 
{
	CTask* pTask;
	if ( (pTask=GetSelectedItemPointer()) == NULL )
		return;

	UINT uiStatus=pTask->GetStatus(ST_STEP_MASK);
	if ( (uiStatus & ST_STEP_MASK) != ST_FINISHED && (uiStatus & ST_STEP_MASK) != ST_CANCELLED )
	{
		// ask if cancel
		if (MsgBox(IDS_CONFIRMCANCEL_STRING, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			// cancel
			if ( (pTask=GetSelectedItemPointer()) == NULL )
				return;

			pTask->CancelProcessing();
		}
		else
			return;
	}

		m_pTasks->RemoveFinished(&pTask);
	RefreshStatus();
}

void CStatusDlg::OnPauseAllButton() 
{
	TRACE("Pause All...\n");
	m_pTasks->TasksPauseProcessing();
	RefreshStatus();
}

void CStatusDlg::OnStartAllButton() 
{
	TRACE("Resume Processing...\n");
	m_pTasks->TasksResumeProcessing();
	RefreshStatus();
}

void CStatusDlg::OnRestartAllButton() 
{
	TRACE("Restart Processing...\n");
	m_pTasks->TasksRestartProcessing();	
	RefreshStatus();
}

void CStatusDlg::OnCancelAllButton() 
{
	TRACE("Cancel Processing...\n");
	m_pTasks->TasksCancelProcessing();	
	RefreshStatus();
}

void CStatusDlg::OnRemoveFinishedButton() 
{
	m_pTasks->RemoveAllFinished();
	RefreshStatus();
}

void CStatusDlg::OnKeydownStatusList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	switch (pLVKeyDow->wVKey)
	{
	case VK_DELETE:
		OnDeleteButton();
		break;
	case VK_SPACE:
		{
			CTask* pTask;
			if ( (pTask=GetSelectedItemPointer()) == NULL )
				return;
		
			if (pTask->GetStatus(ST_WORKING_MASK) & ST_PAUSED)
				OnResumeButton();
			else
				OnPauseButton();
			break;
		}
	}

	*pResult = 0;
}

int CStatusDlg::GetImageFromStatus(UINT nStatus)
{
	if ( (nStatus & ST_STEP_MASK) == ST_CANCELLED )
		return 4;
	if ( (nStatus & ST_STEP_MASK) == ST_FINISHED )
		return 3;
	if ( (nStatus & ST_WAITING_MASK) == ST_WAITING )
		return 5;
	if ( (nStatus & ST_WORKING_MASK) == ST_PAUSED )
		return 2;
	if ( (nStatus & ST_WORKING_MASK) == ST_ERROR )
		return 1;
	return 0;
}

LPTSTR CStatusDlg::FormatTime(long lSeconds, LPTSTR lpszBuffer, size_t stMaxBufferSize)
{
	long lDays=lSeconds/86400;
	lSeconds%=86400;
	long lHours=lSeconds/3600;
	lSeconds%=3600;
	long lMinutes=lSeconds/60;
	lSeconds%=60;

	if (lDays != 0)
		_sntprintf(lpszBuffer, stMaxBufferSize, _T("%02d:%02d:%02d:%02d"), lDays, lHours, lMinutes, lSeconds);
	else
		if (lHours != 0)
			_sntprintf(lpszBuffer, stMaxBufferSize, _T("%02d:%02d:%02d"), lHours, lMinutes, lSeconds);
		else
			_sntprintf(lpszBuffer, stMaxBufferSize, _T("%02d:%02d"), lMinutes, lSeconds);

	return lpszBuffer;
}

void CStatusDlg::RefreshStatus()
{
	// remember address of a current selection
	pSelectedItem=GetSelectedItemPointer();

	// current time
	DWORD dwCurrentTime=GetTickCount();
	
	// get rid of item after the current part
	m_ctlStatusList.LimitItems(m_pTasks->GetSize());
	
	// add task info
	for (int i=0;i<m_pTasks->GetSize();i++)
		AddTaskInfo(i, m_pTasks->GetAt(i), dwCurrentTime);
	
	// percent
	int nPercent=m_pTasks->GetPercent();
	
	// set title
	if (m_pTasks->GetSize() != 0)
		_sntprintf(m_szData, _MAX_PATH, _T("%s [%d %%]"), GetResManager().LoadString(IDS_STATUSTITLE_STRING), m_pTasks->GetPercent());
	else
		_sntprintf(m_szData, _MAX_PATH, _T("%s"), GetResManager().LoadString(IDS_STATUSTITLE_STRING));
	
	// if changed
	GetWindowText(m_strTemp);
	if (m_strTemp != CString(m_szData)) 
		SetWindowText(m_szData);
	
	// refresh overall progress
	if (GetConfig().get_bool(PP_STATUSSHOWDETAILS))
	{
		m_ctlProgressAll.SetPos(nPercent);
		
		// progress - count of processed data/count of data
		m_strTemp=GetSizeString(m_pTasks->GetPosition(), m_szData, _MAX_PATH)+CString(_T("/"));
		m_strTemp+=GetSizeString(m_pTasks->GetRange(), m_szData, _MAX_PATH);
		GetDlgItem(IDC_OVERALL_PROGRESS_STATIC)->SetWindowText(m_strTemp);
		
		// transfer
		if (m_i64LastAllTasksProcessed == 0)
			m_i64LastAllTasksProcessed=m_pTasks->GetPosition();
		
		if (dwCurrentTime-m_dwLastUpdate != 0)
			m_strTemp=GetSizeString( (static_cast<double>(m_pTasks->GetPosition()) - static_cast<double>(m_i64LastAllTasksProcessed))/static_cast<double>(static_cast<double>(dwCurrentTime-m_dwLastUpdate)/1000.0), m_szData, _MAX_PATH);
		else
			m_strTemp=GetSizeString( 0ULL, m_szData, _MAX_PATH);
		
		GetDlgItem(IDC_OVERALL_TRANSFER_STATIC)->SetWindowText(m_strTemp+_T("/s"));
		m_i64LastAllTasksProcessed=m_pTasks->GetPosition();
		m_dwLastUpdate=dwCurrentTime;
	}

	// if selection's missing - hide controls
	if (m_ctlStatusList.GetSelectedCount() == 0)
	{
		EnableControls(false);
		m_pLastSelected=NULL;
		m_i64LastProcessed=0;
	}
	else
		EnableControls();		// enable controls
	
	// apply state of the resume, cancel, ... buttons
	ApplyButtonsState();
}

void CStatusDlg::OnSelectionChanged(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/)
{
	TRACE("Received LVN_CHANGEDSELECTION\n");
	RefreshStatus();
}

void CStatusDlg::OnCancel() 
{
	PostCloseMessage();
	CLanguageDialog::OnCancel();
}

void CStatusDlg::OnAdvancedButton() 
{
	CMenu menu;
	HMENU hMenu=GetResManager().LoadMenu(MAKEINTRESOURCE(IDR_ADVANCED_MENU));
	if (!menu.Attach(hMenu))
	{
		DestroyMenu(hMenu);
		return;
	}
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	if(pPopup)
	{
		// get the point to show menu at
		CRect rect;
		GetDlgItem(IDC_ADVANCED_BUTTON)->GetWindowRect(&rect);

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.right+1, rect.top, this);
	}
}

void CStatusDlg::OnPopupReplacePaths() 
{
	// check if there's a selection currently
	if ( (pSelectedItem=GetSelectedItemPointer()) != NULL )
	{
		if (pSelectedItem->GetStatus(ST_WORKING_MASK) & ST_PAUSED)
		{
			bool bContinue=false;
			if (pSelectedItem->GetStatus(ST_WORKING_MASK) == ST_ERROR)
			{
				pSelectedItem->PauseProcessing();
				bContinue=true;
			}

			// assuming here that there's selection and task is paused
			CReplacePathsDlg dlg;
			dlg.m_pTask=pSelectedItem;
			if (dlg.DoModal() == IDOK)
			{
				// change 'no case'
				int iClipboard=pSelectedItem->ReplaceClipboardStrings(dlg.m_strSource, dlg.m_strDest);

				ictranslate::CFormat fmt(GetResManager().LoadString(IDS_REPLACEPATHSTEXT_STRING));
				fmt.SetParam(_t("%count"), iClipboard);
				AfxMessageBox(fmt);
			}

			// resume if earlier was an error
			if (bContinue)
				pSelectedItem->ResumeProcessing();
		}
		else
			MsgBox(IDS_TASKNOTPAUSED_STRING);
	}
	else
		MsgBox(IDS_TASKNOTSELECTED_STRING);
}

void CStatusDlg::OnShowLogButton() 
{
	// show log
	CTask* pTask;
	if ( (pTask=GetSelectedItemPointer()) == NULL || !GetConfig().get_bool(PP_CMCREATELOG))
		return;

	// call what's needed
	unsigned long lResult=(unsigned long)(ShellExecute(this->m_hWnd, _T("open"), _T("notepad.exe"),
			CString(pTask->GetTaskPath())+pTask->GetUniqueName()+_T(".log"), NULL, SW_SHOWNORMAL));
	if (lResult < 32)
	{
		CString str=CString(pTask->GetTaskPath())+pTask->GetUniqueName()+_T(".log");
		ictranslate::CFormat fmt(GetResManager().LoadString(IDS_SHELLEXECUTEERROR_STRING));
		fmt.SetParam(_t("%errno"), lResult);
		fmt.SetParam(_t("%path"), str);
		AfxMessageBox(fmt);
	}
}

LRESULT CStatusDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_UPDATESTATUS)
	{
		TRACE("Received WM_UPDATESTATUS\n");
		RefreshStatus();
	}
	return ictranslate::CLanguageDialog::WindowProc(message, wParam, lParam);
}

void CStatusDlg::OnStickButton() 
{
	ApplyDisplayDetails(true);
}

void CStatusDlg::SetBufferSizesString(UINT uiValue, int iIndex)
{
	TCHAR szData[1024];
	switch(iIndex)
	{
	case BI_DEFAULT:
		GetResManager().LoadStringCopy(IDS_BSDEFAULT_STRING, szData, 256);
		break;
	case BI_ONEDISK:
		GetResManager().LoadStringCopy(IDS_BSONEDISK_STRING, szData, 256);
		break;
	case BI_TWODISKS:
		GetResManager().LoadStringCopy(IDS_BSTWODISKS_STRING, szData, 256);
		break;
	case BI_CD:
		GetResManager().LoadStringCopy(IDS_BSCD_STRING, szData, 256);
		break;
	case BI_LAN:
		GetResManager().LoadStringCopy(IDS_BSLAN_STRING, szData, 256);
		break;
	default:
		_ASSERTE(false);
		szData[0] = _T('\0');
	}

	_tcscat(szData, GetSizeString((ull_t)uiValue, m_szData, _MAX_PATH));

	GetDlgItem(IDC_BUFFERSIZE_STATIC)->SetWindowText(szData);
}

void CStatusDlg::PostCloseMessage()
{
	GetParent()->PostMessage(WM_STATUSCLOSING);
}

void CStatusDlg::OnLanguageChanged()
{
	// remove all columns
	int iCnt=m_ctlStatusList.GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	for (int i=0;i<iCnt;i++)
		m_ctlStatusList.DeleteColumn(0);

	// get size of list ctrl
	CRect rcList;
	m_ctlStatusList.GetWindowRect(&rcList);
	int iWidth=rcList.Width();

	// refresh the header in a list
	LVCOLUMN lvc;
	lvc.mask=LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt=LVCFMT_LEFT;

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNSTATUS_STRING); /*_T("Status")*/;
    lvc.cchTextMax = lstrlen(lvc.pszText); 
    lvc.cx = static_cast<int>(0.27*iWidth);
	lvc.iSubItem=-1;
	m_ctlStatusList.InsertColumn(1, &lvc);

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNSOURCE_STRING);/*_T("File");*/
    lvc.cchTextMax = lstrlen(lvc.pszText); 
    lvc.cx = static_cast<int>(0.3*iWidth);
	lvc.iSubItem=0;
	m_ctlStatusList.InsertColumn(2, &lvc);

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNDESTINATION_STRING);/*_T("To:");*/
    lvc.cchTextMax = lstrlen(lvc.pszText);
    lvc.cx = static_cast<int>(0.27*iWidth);
	lvc.iSubItem=1;
	m_ctlStatusList.InsertColumn(3, &lvc);

	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_COLUMNPROGRESS_STRING);/*_T("Progress");*/
    lvc.cchTextMax = lstrlen(lvc.pszText);
    lvc.cx = static_cast<int>(0.15*iWidth);
	lvc.iSubItem=2;
	m_ctlStatusList.InsertColumn(4, &lvc);

	RefreshStatus();
}

// ============================================================================
/// CStatusDlg::PrepareResizableControls
/// @date 2009/04/18
///
/// @brief     Prepares the resizable controls.
// ============================================================================
void CStatusDlg::PrepareResizableControls()
{
	ClearResizableControls();

	AddResizableControl(IDC_001_STATIC, 0, 0, 0.5, 0.0);
	AddResizableControl(IDC_STATUS_LIST, 0, 0, 0.5, 1.0);
	AddResizableControl(IDC_ROLL_UNROLL_BUTTON, 0.5, 0, 0, 0);

	AddResizableControl(IDC_PAUSE_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_RESTART_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_RESUME_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_CANCEL_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_DELETE_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_PAUSE_ALL_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_START_ALL_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_CANCEL_ALL_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_REMOVE_FINISHED_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_RESTART_ALL_BUTTON, 0, 1.0, 0, 0);
	AddResizableControl(IDC_ADVANCED_BUTTON, 0, 1.0, 0, 0);

	AddResizableControl(IDC_STICK_BUTTON, 1.0, 1.0, 0, 0);

	// sections separators
	AddResizableControl(IDC_014_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_015_STATIC, 0.5, 0.0, 0.0, 0);

	AddResizableControl(IDC_018_STATIC, 0.5, 0.0, 0.25, 0);
	AddResizableControl(IDC_019_STATIC, 0.5, 0.0, 0.25, 0);
	AddResizableControl(IDC_016_STATIC, 0.75, 0.0, 0.25, 0);
	AddResizableControl(IDC_017_STATIC, 0.75, 0.0, 0.25, 0);

	// left part of right column
	AddResizableControl(IDC_002_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_003_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_004_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_005_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_006_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_007_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_008_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_009_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_010_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_011_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_012_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_013_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_020_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_021_STATIC, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_SHOW_LOG_BUTTON, 0.5, 0.0, 0.0, 0);
	AddResizableControl(IDC_SHOW_LOG_BUTTON, 0.5, 0.0, 0.0, 0.0);

	// full length right column
	AddResizableControl(IDC_ALL_PROGRESS, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_TASK_PROGRESS, 0.5, 0.0, 0.5, 0);

	// right part of right column
	AddResizableControl(IDC_ERRORS_EDIT, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_ASSOCIATEDFILES__STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_OPERATION_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_SOURCE_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_DESTINATION_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_PROGRESS_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_TIME_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_TRANSFER_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_OVERALL_PROGRESS_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_OVERALL_TRANSFER_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_BUFFERSIZE_STATIC, 0.5, 0.0, 0.5, 0);
	AddResizableControl(IDC_PRIORITY_STATIC, 0.5, 0.0, 0.5, 0);

	AddResizableControl(IDC_SET_BUFFERSIZE_BUTTON, 1.0, 0.0, 0.0, 0.0);
	AddResizableControl(IDC_SET_PRIORITY_BUTTON, 1.0, 0.0, 0.0, 0.0);

	InitializeResizableControls();
}
