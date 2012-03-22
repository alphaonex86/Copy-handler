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
#include "DirTreeCtrl.h"
#include "FolderDialog.h"
#include "memdc.h"
#include "Theme Helpers.h"
#include "shlobj.h"
#include "..\Common\FileSupport.h"
#include "StringHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// dialog jako taki
const unsigned long __g_DlgTemplate[]={ 
	0x82cf0040, 0x00000000, 0x00000000, 0x011b0000, 0x000000b4, 0x00000000, 0x00540008, 0x00680061, 
	0x006d006f, 0x00000061 };

/////////////////////////////////////////////////////////////////////////////
// additional messages
#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED                 0x031A
#endif

/////////////////////////////////////////////////////////////////////////////
// widow procedures - group subclassing for flicker free drawing
WNDPROC __g_pfButton;
WNDPROC __g_pfStatic;
WNDPROC __g_pfList;
WNDPROC __g_pfCombo;
WNDPROC __g_pfBaseCombo;
WNDPROC __g_pfEdit;

/////////////////////////////////////////////////////////////////////////////
// Draws and calls the old window proc
LRESULT CALLBACK InternalWindowProc(WNDPROC pfWndProc, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
		return (LRESULT)0;
		break;
	case WM_PAINT:
		CWnd* pWnd=CWnd::FromHandle(hwnd);
		CPaintDC dc(pWnd);

		// exclude header from update rect (only in report view)
		int iID=GetDlgCtrlID(hwnd);
		if (iID == IDC_SHORTCUT_LIST)
		{
			DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE); 
			if ((dwStyle & LVS_TYPEMASK) == LVS_REPORT)
			{
				CRect headerRect;
				HWND hHeader=ListView_GetHeader(hwnd);
				GetWindowRect(hHeader, &headerRect);
				CPoint pt=headerRect.TopLeft();
				ScreenToClient(hwnd, &pt);
				headerRect.left=pt.x;
				headerRect.top=pt.y;
				pt=headerRect.BottomRight();
				ScreenToClient(hwnd, &pt);
				headerRect.right=pt.x;
				headerRect.bottom=pt.y;
				dc.ExcludeClipRect(&headerRect);
			}
		}

		CMemDC memdc(&dc, &dc.m_ps.rcPaint);

		if (dc.m_ps.fErase)
			memdc.FillSolidRect(&dc.m_ps.rcPaint, GetSysColor(COLOR_WINDOW));

		CallWindowProc(pfWndProc, hwnd, WM_PAINT, (WPARAM)memdc.GetSafeHdc(), 0);
		return 0;
		break;
	}

	return CallWindowProc(pfWndProc, hwnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////
// window proc for edit ctrl contained in combo
LRESULT CALLBACK EditWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return InternalWindowProc(__g_pfEdit, hwnd, uMsg, wParam, lParam);
};

////////////////////////////////////////////////////////////////////////////
// procedure for combo box contained in comboboxex
LRESULT CALLBACK ComboWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return InternalWindowProc(__g_pfBaseCombo, hwnd, uMsg, wParam, lParam);
};

////////////////////////////////////////////////////////////////////////////
// other visual elements' window proc
LRESULT CALLBACK CustomWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iID=GetDlgCtrlID(hwnd);
	WNDPROC pfOld=NULL;
	switch(iID)
	{
	case IDC_TOGGLE_BUTTON:
	case IDOK:
	case IDCANCEL:
		pfOld=__g_pfButton;
		break;
	case IDC_TITLE_STATIC:
		pfOld=__g_pfStatic;
		break;
	case IDC_PATH_COMBOBOXEX:
		pfOld=__g_pfCombo;
		break;
	case IDC_SHORTCUT_LIST:
		pfOld=__g_pfList;
		break;
	default:
		ASSERT(false); // used CustomWindowProc, but no ID has been recognized
	}
	
	return InternalWindowProc(pfOld, hwnd, uMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog dialog

CFolderDialog::CFolderDialog(CWnd* /*pParent*/ /*=NULL*/)
				:ictranslate::CLanguageDialog()
{
	//{{AFX_DATA_INIT(CFolderDialog)
	//}}AFX_DATA_INIT
	m_hImages=NULL;
	m_hLargeImages=NULL;
	m_bIgnoreUpdate=false;
	m_bIgnoreTreeRefresh=false;
}

CFolderDialog::~CFolderDialog()
{
}

BEGIN_MESSAGE_MAP(CFolderDialog,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CFolderDialog)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SHORTCUT_LIST, OnEndLabelEditShortcutList)
	ON_BN_CLICKED(IDC_ADDSHORTCUT_BUTTON, OnAddShortcut)
	ON_BN_CLICKED(IDC_REMOVESHORTCUT_BUTTON, OnRemoveShortcut)
	ON_BN_CLICKED(IDC_TOGGLE_BUTTON, OnToggleButton)
	ON_NOTIFY(TVN_SELCHANGED, IDC_FOLDER_TREE, OnSelchangedFolderTree)
	ON_NOTIFY(TVN_GETINFOTIP, IDC_FOLDER_TREE, OnGetInfoTipFolderTree)
	ON_BN_CLICKED(IDC_NEWFOLDER_BUTTON, OnNewfolderButton)
	ON_CBN_EDITCHANGE(IDC_PATH_COMBOBOXEX, OnPathChanging)
	ON_BN_CLICKED(IDC_LARGEICONS_BUTTON, OnIconsRadio)
	ON_BN_CLICKED(IDC_SMALLICONS_BUTTON, OnSmalliconsRadio)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListRadio)
	ON_BN_CLICKED(IDC_REPORT_BUTTON, OnReportRadio)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SHORTCUT_LIST, OnItemchangedShortcutList)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_SHORTCUT_LIST, OnGetShortcutInfoTip)
	ON_NOTIFY(LVN_KEYDOWN, IDC_SHORTCUT_LIST, OnShortcutKeyDown)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog message handlers

/////////////////////////////////////////////////////////////////////////////
// creating all needed controls

int CFolderDialog::CreateControls()
{
	// std rect - size doesn't matter for now
	CRect rc(0, 0, 0, 0);

	if (!m_ctlTitle.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, IDC_TITLE_STATIC) ||
		(__g_pfStatic=(WNDPROC)SetWindowLongPtr(m_ctlTitle.GetSafeHwnd(), GWLP_WNDPROC, (LONG_PTR)CustomWindowProc)) == 0)
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlTitle.SetFont(GetFont(), FALSE);

	// buttons - small with bitmaps
	if (!m_ctlLargeIcons.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, IDC_LARGEICONS_BUTTON))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlLargeIcons.SetImage(&m_ilList, 0);

	if (!m_ctlSmallIcons.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, IDC_SMALLICONS_BUTTON))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlSmallIcons.SetImage(&m_ilList, 1);

	if (!m_ctlList.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, IDC_LIST_BUTTON))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlList.SetImage(&m_ilList, 2);

	if (!m_ctlReport.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, IDC_REPORT_BUTTON))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlReport.SetImage(&m_ilList, 3);

	if (!m_ctlNewFolder.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, IDC_NEWFOLDER_BUTTON))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlNewFolder.SetImage(&m_ilList, 4);

	// listview
	if (!m_ctlShortcuts.Create(WS_CHILD | WS_VISIBLE | LVS_SINGLESEL | LVS_SHAREIMAGELISTS | LVS_EDITLABELS | WS_TABSTOP | LVS_SMALLICON | LVS_SHOWSELALWAYS, rc, this, IDC_SHORTCUT_LIST) ||
		(__g_pfList=(WNDPROC)SetWindowLongPtr(m_ctlShortcuts.GetSafeHwnd(), GWLP_WNDPROC, (LONG_PTR)CustomWindowProc)) == 0)
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlShortcuts.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_INFOTIP | LVS_EX_UNDERLINEHOT);
	m_ctlShortcuts.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	// dir tree ctrl
	if (!m_ctlTree.Create(WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS
		| TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | TVS_TRACKSELECT | TVS_SINGLEEXPAND | TVS_INFOTIP |
		WS_TABSTOP | WS_VSCROLL, rc, this, IDC_FOLDER_TREE)
		|| !m_ctlTree.ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE))
	{
		TRACE("Error creating control...");
		return -1;
	}

	// combobox
	rc.bottom=rc.top+200;
	if (!m_ctlPath.Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWN | CBS_SORT | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rc, this, IDC_PATH_COMBOBOXEX) ||
		(__g_pfCombo=(WNDPROC)SetWindowLongPtr(m_ctlPath.GetSafeHwnd(), GWLP_WNDPROC, (LONG_PTR)CustomWindowProc)) == 0)
	{
		TRACE("Error creating control...");
		return -1;
	}
	HWND hCombo=(HWND)m_ctlPath.SendMessage(CBEM_GETCOMBOCONTROL, 0, 0);
	if ((__g_pfBaseCombo=(WNDPROC)SetWindowLongPtr(hCombo, GWLP_WNDPROC, (LONG_PTR)ComboWindowProc)) == 0)
		return -1;
	HWND hEdit=(HWND)m_ctlPath.SendMessage(CBEM_GETEDITCONTROL, 0, 0);
	if ((__g_pfEdit=(WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)EditWindowProc)) == 0)
		return -1;

	// buttons OK & Cancel
	rc.bottom=rc.top;
	if (!m_ctlOk.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, rc, this, IDOK) ||
		(__g_pfButton=(WNDPROC)SetWindowLongPtr(m_ctlOk.GetSafeHwnd(), GWLP_WNDPROC, (LONG_PTR)CustomWindowProc)) == 0)
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlOk.SetFont(GetFont());

	if (!m_ctlCancel.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rc, this, IDCANCEL) ||
		!((WNDPROC)SetWindowLongPtr(m_ctlCancel.GetSafeHwnd(), GWLP_WNDPROC, (LONG_PTR)CustomWindowProc)))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlCancel.SetFont(GetFont());

	if (!m_ctlToggle.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rc, this, IDC_TOGGLE_BUTTON) ||
		!((WNDPROC)SetWindowLongPtr(m_ctlToggle.GetSafeHwnd(), GWLP_WNDPROC, (LONG_PTR)CustomWindowProc)))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlToggle.SetFont(GetFont());

	// add&remove shortcut
	if (!m_ctlRemoveShortcut.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, IDC_REMOVESHORTCUT_BUTTON))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlRemoveShortcut.SetImage(&m_ilList, 6);

	if (!m_ctlAddShortcut.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, IDC_ADDSHORTCUT_BUTTON))
	{
		TRACE("Error creating control...");
		return -1;
	}
	m_ctlAddShortcut.SetImage(&m_ilList, 5);

	return 0;
}

///////////////////////////////////////////////////////////////////////////
// tworzy image listê z wszystkimi potrzebnymi ikonami
void CFolderDialog::InitImageList()
{
	m_ilList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);

	HICON hIcon=(HICON)GetResManager().LoadImage(MAKEINTRESOURCE(IDI_LARGEICONS_ICON), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	m_ilList.Add(hIcon);

	hIcon=(HICON)GetResManager().LoadImage(MAKEINTRESOURCE(IDI_SMALLICONS_ICON), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	m_ilList.Add(hIcon);

	hIcon=(HICON)GetResManager().LoadImage(MAKEINTRESOURCE(IDI_LIST_ICON), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	m_ilList.Add(hIcon);

	hIcon=(HICON)GetResManager().LoadImage(MAKEINTRESOURCE(IDI_REPORT_ICON), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	m_ilList.Add(hIcon);

	hIcon=(HICON)GetResManager().LoadImage(MAKEINTRESOURCE(IDI_NEWFOLDER_ICON), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	m_ilList.Add(hIcon);

	hIcon=(HICON)GetResManager().LoadImage(MAKEINTRESOURCE(IDI_ADDSHORTCUT_ICON), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	m_ilList.Add(hIcon);

	hIcon=(HICON)GetResManager().LoadImage(MAKEINTRESOURCE(IDI_DELETESHORTCUT_ICON), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	m_ilList.Add(hIcon);
}

////////////////////////////////////////////////////////////////////////////
// applies showing/hiding shortcuts list
void CFolderDialog::ApplyExpandState(bool bExpand)
{
	// change button text and hide/show needed elements
	m_ctlToggle.SetWindowText(GetResManager().LoadString(bExpand ? IDS_BDRIGHT_STRING : IDS_BDLEFT_STRING));
	m_ctlShortcuts.ShowWindow(bExpand ? SW_SHOW : SW_HIDE);
	m_ctlLargeIcons.ShowWindow(bExpand ? SW_SHOW : SW_HIDE);
	m_ctlSmallIcons.ShowWindow(bExpand ? SW_SHOW : SW_HIDE);
	m_ctlList.ShowWindow(bExpand ? SW_SHOW : SW_HIDE);
	m_ctlReport.ShowWindow(bExpand ? SW_SHOW : SW_HIDE);
	m_ctlAddShortcut.ShowWindow(bExpand ? SW_SHOW : SW_HIDE);
	m_ctlRemoveShortcut.ShowWindow(bExpand ? SW_SHOW : SW_HIDE);
}

////////////////////////////////////////////////////////////////////////////
// toggle extended view
void CFolderDialog::OnToggleButton()
{
	m_bdData.bExtended=!m_bdData.bExtended;
	ApplyExpandState(m_bdData.bExtended);

	CRect rcDialog;
	GetClientRect(&rcDialog);
	ResizeControls(rcDialog.Width(), rcDialog.Height());
}

/////////////////////////////////////////////////////////////////////////////
// initialization of most important params - reading text, bitmaps
BOOL CFolderDialog::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	// image list
	InitImageList();

	if (CreateControls() == -1)
		EndDialog(-1);

	// size of a dialog
	CRect rcDialog;
	GetClientRect(&rcDialog);
	if (m_bdData.cx != 0)
		rcDialog.right=rcDialog.left+m_bdData.cx;
	if (m_bdData.cy != 0)
		rcDialog.bottom=rcDialog.top+m_bdData.cy;
	if (m_bdData.cy != 0 || m_bdData.cx != 0)
		SetWindowPos(&wndTop, rcDialog.left, rcDialog.top, rcDialog.Width(), rcDialog.Height(), SWP_NOMOVE);

	// show needed text
	SetWindowText(m_bdData.strCaption);
	m_ctlTitle.SetWindowText(m_bdData.strText);
	m_ctlTree.SetIgnoreShellDialogs(m_bdData.bIgnoreDialogs);
	m_ctlTree.PostMessage(WM_SETPATH, 0, (LPARAM)((LPCTSTR)m_bdData.strInitialDir));

	// buttons text
	m_ctlOk.SetWindowText(GetResManager().LoadString(IDS_BDOK_STRING));
	m_ctlCancel.SetWindowText(GetResManager().LoadString(IDS_BDCANCEL_STRING));
	ApplyExpandState(m_bdData.bExtended);

	// sys img list
	SHFILEINFO sfi;
	m_hImages = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	m_hLargeImages=(HIMAGELIST)SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX);
	m_ctlPath.SendMessage(CBEM_SETIMAGELIST, 0, (LPARAM)m_hImages);
	m_ctlShortcuts.SendMessage(LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)m_hImages);
	m_ctlShortcuts.SendMessage(LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, (LPARAM)m_hLargeImages);

	// add all the paths from m_bdData.astrRecent
	COMBOBOXEXITEM cbi;
	CString strText;
	cbi.mask=CBEIF_IMAGE | CBEIF_TEXT;

	for (int i=0;i<(int)m_bdData.cvRecent.size();i++)
	{
		cbi.iItem=i;
		cbi.pszText=m_bdData.cvRecent.at(i);
		sfi.iIcon=-1;
		SHGetFileInfo(m_bdData.cvRecent.at(i), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO),
			SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		cbi.iImage=sfi.iIcon;

		m_ctlPath.InsertItem(&cbi);
	}

	// additional columns in a list
	LVCOLUMN lvc;
	lvc.mask=LVCF_SUBITEM | LVCF_WIDTH | LVCF_TEXT;
	lvc.iSubItem=-1;
	lvc.cx=80;
	lvc.pszText=(LPTSTR)GetResManager().LoadString(IDS_BDNAME_STRING);
	m_ctlShortcuts.InsertColumn(0, &lvc);
	lvc.iSubItem=0;
	lvc.cx=110;
	lvc.pszText=(LPTSTR)GetResManager().LoadString(IDS_BDPATH_STRING);
	m_ctlShortcuts.InsertColumn(1, &lvc);

	// select view-style button
	SetView(m_bdData.iView);
	
	// update shortcuts' list
	CShortcut sc;
	for (int i=0;i<(int)m_bdData.cvShortcuts.size();i++)
	{
		sc=CString(m_bdData.cvShortcuts.at(i));
		sfi.iIcon=-1;
		SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		m_ctlShortcuts.InsertItem(i, sc.m_strName, sfi.iIcon);
		m_ctlShortcuts.SetItem(i, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);
	}

	// now resize and ok.
	GetClientRect(&rcDialog);
	ResizeControls(rcDialog.Width(), rcDialog.Height());

	// set to top - the strange way
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetFocus();
	SetForegroundWindow();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// tooltip handling for controls in this dialog
BOOL CFolderDialog::OnTooltipText(UINT uiID, TOOLTIPTEXT* pTip)
{
	switch (uiID)
	{
	case IDC_NEWFOLDER_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDNEWFOLDER_STRING);
		break;
	case IDC_LARGEICONS_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDLARGEICONS_STRING);
		break;
	case IDC_SMALLICONS_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDSMALLICONS_STRING);
		break;
	case IDC_LIST_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDLIST_STRING);
		break;
	case IDC_REPORT_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDREPORT_STRING);
		break;
	case IDC_TOGGLE_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDDETAILS_STRING);
		break;
	case IDC_ADDSHORTCUT_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDADDSHORTCUT_STRING);
		break;
	case IDC_REMOVESHORTCUT_BUTTON:
		pTip->lpszText=(LPTSTR)GetResManager().LoadString(IDS_BDREMOVESHORTCUT_STRING);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// cancels the dialog or cancels editing of an item
void CFolderDialog::OnCancel() 
{
	if (m_ctlTree.IsEditing())
	{
		TreeView_EndEditLabelNow(m_ctlTree.GetSafeHwnd(), TRUE);
		return;
	}
	
	CRect rcDlg;
	GetWindowRect(&rcDlg);
	m_bdData.cx=rcDlg.Width();
	m_bdData.cy=rcDlg.Height();

	CLanguageDialog::OnCancel();
}

///////////////////////////////////////////////////////////////////////////
// finishes dialog's work or finished editing of an item
void CFolderDialog::OnOK() 
{
	// if we edit an item in m_ctlTree
	if (m_ctlTree.IsEditing())
	{
		TreeView_EndEditLabelNow(m_ctlTree.GetSafeHwnd(), FALSE);
		return;
	}

	// update path, get rid of '\\'
	m_ctlPath.GetWindowText(m_strPath);
	if (m_strPath.Right(1) == _T('\\') || m_strPath.Right(1) == _T('/'))
		m_strPath=m_strPath.Left(m_strPath.GetLength()-1);

	// does it exist as a folder ?
/*	CFileFind fnd;
	BOOL bExist=fnd.FindFile(m_strPath+_T("\\*"));
	fnd.Close();*/
//	WIN32_FIND_DATA wfd;
//	HANDLE hFind;

//	if (!bExist)
	if ( GetFileAttributes(m_strPath) == INVALID_FILE_ATTRIBUTES)
	{
		MsgBox(IDS_BDPATHDOESNTEXIST_STRING, MB_OK | MB_ICONERROR);
		return;
	}

	m_bdData.cvRecent.insert(m_bdData.cvRecent.begin(), (const PTSTR)(LPCTSTR)(m_strPath+_T('\\')), true);

	CRect rcDlg;
	GetWindowRect(&rcDlg);
	m_bdData.cx=rcDlg.Width();
	m_bdData.cy=rcDlg.Height();

	CLanguageDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////
// Displays dialog from __g_DlgTemplate
INT_PTR CFolderDialog::DoModal()
{
	if (!InitModalIndirect((LPCDLGTEMPLATE)__g_DlgTemplate))
		return -1;
	else
		return ictranslate::CLanguageDialog::DoModal();
}

///////////////////////////////////////////////////////////////////////////
// handles creation of a new folder
void CFolderDialog::OnNewfolderButton() 
{
	// currently selected item
	HTREEITEM hItem=m_ctlTree.GetSelectedItem();
	if (hItem == NULL)
		return;

	// insert child item
	m_ctlTree.InsertNewFolder(hItem, _T(""));
}

/////////////////////////////////////////////////////////////////////////////
// other messages
LRESULT CFolderDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
	case WM_CREATEFOLDERRESULT:
		if (((int)lParam) != 1)
			MsgBox(IDS_BDCANNOTCREATEFOLDER_STRING);
		break;
	case WM_THEMECHANGED:
		CRect rcDialog;
		GetClientRect(&rcDialog);
		ResizeControls(rcDialog.Width(), rcDialog.Height());
		break;
	}

	return ictranslate::CLanguageDialog::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////
// tooltips for folder tree
void CFolderDialog::OnGetInfoTipFolderTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTVGETINFOTIP* pit = (NMTVGETINFOTIP*)pNMHDR;
	*pResult = 0;

	// get rid of old tip
	m_strTip.Empty();

	// get type of an item - for floppy skip
	SHDESCRIPTIONID shdi;
	bool bSkipFreeSpace=false;
	if (m_ctlTree.GetItemShellData(pit->hItem, SHGDFIL_DESCRIPTIONID, &shdi, sizeof(SHDESCRIPTIONID))
		&& ( shdi.dwDescriptionId == SHDID_COMPUTER_DRIVE35 
		  || shdi.dwDescriptionId == SHDID_COMPUTER_DRIVE525 ) )
		bSkipFreeSpace=true;


	// some about network
	bool bNet;
	CString strData;
	NETRESOURCE* pnet=(NETRESOURCE*)m_szBuffer;
	if ( (bNet=m_ctlTree.GetItemShellData(pit->hItem, SHGDFIL_NETRESOURCE, pnet, 2048)) == true)
	{
		if (pnet->lpRemoteName && _tcscmp(pnet->lpRemoteName, _T("")) != 0)
			m_strTip+=GetResManager().LoadString(IDS_BDREMOTENAME_STRING)+CString(pnet->lpRemoteName)+_T("\n");
		if ( pnet->lpLocalName && _tcscmp(pnet->lpLocalName, _T("")) != 0)
			m_strTip+=GetResManager().LoadString(IDS_BDLOCALNAME_STRING)+CString(pnet->lpLocalName)+_T("\n");
		if ( pnet->dwDisplayType != 0)
			m_strTip+=GetResManager().LoadString(IDS_BDTYPE_STRING)+CString(GetResManager().LoadString(IDS_BDDOMAIN_STRING+pnet->dwDisplayType))+_T("\n");
		if ( pnet->lpProvider && _tcscmp(pnet->lpProvider, _T("")) != 0)
			m_strTip+=GetResManager().LoadString(IDS_BDNETTYPE_STRING)+CString(pnet->lpProvider)+_T("\n");
		if ( pnet->lpComment && _tcscmp(pnet->lpComment, _T("")) != 0)
			m_strTip+=GetResManager().LoadString(IDS_BDDESCRIPTION_STRING)+CString(pnet->lpComment)+_T("\n");
	}

	// try to get path
	CString strPath, strMask;
	TCHAR szSizeFree[32], szSizeTotal[32];
	bool bPath;
	if ( (bPath=m_ctlTree.GetPath(pit->hItem, strPath.GetBuffer(_MAX_PATH))) == true )
	{
		strPath.ReleaseBuffer();
		if (!bNet && !strPath.IsEmpty())
			m_strTip+=strPath+_T("\n");

		if (!bSkipFreeSpace)
		{
			// get disk free space
			ull_t ullFree, ullTotal;
			if (GetDynamicFreeSpace(strPath, &ullFree, &ullTotal))
			{
				m_strTip+=GetResManager().LoadString(IDS_BDFREESPACE_STRING)+CString(GetSizeString(ullFree, szSizeFree, 32, false))+_T("\n");
				m_strTip+=GetResManager().LoadString(IDS_BDCAPACITY_STRING)+CString(GetSizeString(ullTotal, szSizeTotal, 32, false))+_T("\n");
			}
		}
	}

	if (!bNet && !bPath)
	{
		// get std shell msg
		m_ctlTree.GetItemInfoTip(pit->hItem, &m_strTip);
	}
	else
	{
		// get rid of '\n'
		m_strTip=m_strTip.Left(m_strTip.GetLength()-1);
	}

	// set
	pit->pszText=m_strTip.GetBuffer(1);		// tip doesn't change - skip RelaseBuffer
	pit->cchTextMax=lstrlen(pit->pszText);
}

/////////////////////////////////////////////////////////////////////////////
// tooltip support for shortcuts list
void CFolderDialog::OnGetShortcutInfoTip(NMHDR* pNMHDR, LRESULT* /*pResult*/)
{
	NMLVGETINFOTIP* pit=(NMLVGETINFOTIP*)pNMHDR;
	m_strTip.Empty();

	if (pit->iItem < 0 || pit->iItem >= (int)m_bdData.cvShortcuts.size())
		return;	// out of range

	CShortcut sc=CString(m_bdData.cvShortcuts.at(pit->iItem));
	m_strTip=sc.m_strName+_T("\r\n")+CString(GetResManager().LoadString(IDS_BDPATH2_STRING))+sc.m_strPath;

	// get disk free space
	ull_t ullFree, ullTotal;
	if (GetDynamicFreeSpace(sc.m_strPath, &ullFree, &ullTotal))
	{
		m_strTip+=CString(_T("\r\n"))+GetResManager().LoadString(IDS_BDFREESPACE_STRING)+CString(GetSizeString(ullFree, m_szBuffer, 2048, false))+_T("\n");
		m_strTip+=GetResManager().LoadString(IDS_BDCAPACITY_STRING)+CString(GetSizeString(ullTotal, m_szBuffer, 2048, false));
	}

	pit->pszText=(LPTSTR)(LPCTSTR)m_strTip;
	pit->cchTextMax=m_strTip.GetLength()+1;
}

////////////////////////////////////////////////////////////////////////////
// updates text in combo when the selection in a tree changes
void CFolderDialog::OnSelchangedFolderTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TCHAR szPath[_MAX_PATH];
	
	if (m_bIgnoreUpdate)
		return;

	if (m_ctlTree.GetPath(pNMTreeView->itemNew.hItem, szPath))
		SetComboPath(szPath);
	
	*pResult = 0;
}

////////////////////////////////////////////////////////////////////////////
// updates folder tree when combo text changes
void CFolderDialog::OnPathChanging() 
{
	if (m_bIgnoreTreeRefresh)
		return;

	COMBOBOXEXITEM cbi;
	cbi.mask=CBEIF_TEXT;
	cbi.iItem=-1;
	cbi.pszText=m_szBuffer;
	cbi.cchTextMax=_MAX_PATH;

	if (!m_ctlPath.GetItem(&cbi))
		return;

	UpdateComboIcon();

	m_bIgnoreUpdate=true;
	m_ctlTree.SetPath(cbi.pszText);
	m_bIgnoreUpdate=false;
}

/////////////////////////////////////////////////////////////////////////
// sets text in comboboxex edit and updates icon
void CFolderDialog::SetComboPath(LPCTSTR lpszPath)
{
	_ASSERTE(lpszPath);
	if(!lpszPath)
		return;
	// set current select to -1
	m_bIgnoreTreeRefresh=true;
	m_ctlPath.SetCurSel(-1);
	m_bIgnoreTreeRefresh=false;

	SHFILEINFO sfi;
	sfi.iIcon=-1;

	COMBOBOXEXITEM cbi;

	cbi.mask=CBEIF_TEXT | CBEIF_IMAGE;
	cbi.iItem=-1;
	_tcscpy(m_szBuffer, lpszPath);
	cbi.pszText=m_szBuffer;
	SHGetFileInfo(cbi.pszText, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	cbi.iImage=sfi.iIcon;
	m_bIgnoreTreeRefresh=true;
	m_ctlPath.SetItem(&cbi);
	m_bIgnoreTreeRefresh=false;
}

//////////////////////////////////////////////////////////////////////////
// updates icon in comboex
void CFolderDialog::UpdateComboIcon()
{
	// get text from combo
	COMBOBOXEXITEM cbi;
	cbi.mask=CBEIF_TEXT;
	cbi.iItem=m_ctlPath.GetCurSel()/*-1*/;
	cbi.pszText=m_szBuffer;
	cbi.cchTextMax=_MAX_PATH;

	if (!m_ctlPath.GetItem(&cbi))
		return;

	// select no item
	m_bIgnoreTreeRefresh=true;
	m_ctlPath.SetCurSel(-1);
	m_bIgnoreTreeRefresh=false;

	// icon update
	SHFILEINFO sfi;
	sfi.iIcon=-1;

	cbi.mask |= CBEIF_IMAGE;
	cbi.iItem=-1;

	CString str=(LPCTSTR)m_szBuffer;
	if (str.Left(2) != _T("\\\\") || str.Find(_T('\\'), 2) != -1)
		SHGetFileInfo(cbi.pszText, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	
	cbi.iImage=sfi.iIcon;
	
	m_bIgnoreTreeRefresh=true;
	m_ctlPath.SetItem(&cbi);
	m_bIgnoreTreeRefresh=false;

	// unselect text in combo's edit
	CEdit* pEdit=m_ctlPath.GetEditCtrl();
	if (!pEdit)
		return;

	pEdit->SetSel(-1, -1);
}

/////////////////////////////////////////////////////////////////////////////
// combo and treeview update
void CFolderDialog::OnItemchangedShortcutList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* plv=(NMLISTVIEW*)pNMHDR;

	// current selection
	if (plv->iItem >= 0 && plv->iItem < (int)m_bdData.cvShortcuts.size())
	{
		CShortcut sc=CString(m_bdData.cvShortcuts.at(plv->iItem));
		m_ctlTree.SetPath(sc.m_strPath);
		SetComboPath(sc.m_strPath);
	}
	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// adding shortcut
void CFolderDialog::OnAddShortcut()
{
	// get current text
	COMBOBOXEXITEM cbi;
	cbi.mask=CBEIF_TEXT;
	cbi.iItem=m_ctlPath.GetCurSel()/*-1*/;
	cbi.pszText=m_szBuffer;
	cbi.cchTextMax=_MAX_PATH;

	if (!m_ctlPath.GetItem(&cbi))
	{
		MsgBox(IDS_BDNOSHORTCUTPATH_STRING);
		return;
	}

	// create new item - update shortcut list
	CShortcut sc;
	sc.m_strPath=cbi.pszText;
	SHFILEINFO sfi;
	sfi.iIcon=-1;
	SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

	// add to an array and to shortcuts list
	m_bdData.cvShortcuts.push_back((const PTSTR)(LPCTSTR)(CString)(sc), true);
	int iIndex = (int)m_bdData.cvShortcuts.size() - 1;
	m_ctlShortcuts.InsertItem(iIndex, sc.m_strName, sfi.iIcon);
	m_ctlShortcuts.SetItem(iIndex, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);

	// edit item
	m_ctlShortcuts.SetFocus();
	if (m_ctlShortcuts.EditLabel(iIndex) == NULL)
	{
		TRACE("Couldn't edit shortcut list's item label\n");
		return;
	}
}

////////////////////////////////////////////////////////////////////////////
// keyboard's delete button handling
void CFolderDialog::OnShortcutKeyDown(NMHDR* pNMHDR, LRESULT* /*pResult*/)
{
	LPNMLVKEYDOWN lpkd=(LPNMLVKEYDOWN)pNMHDR;
	if (lpkd->wVKey == VK_DELETE)
		OnRemoveShortcut();
}

////////////////////////////////////////////////////////////////////////////
// deleting shortcut from list
void CFolderDialog::OnRemoveShortcut()
{
	POSITION pos=m_ctlShortcuts.GetFirstSelectedItemPosition();
	if (pos)
	{
		int iSel=m_ctlShortcuts.GetNextSelectedItem(pos);
		m_ctlShortcuts.DeleteItem(iSel);
		m_bdData.cvShortcuts.erase(m_bdData.cvShortcuts.begin()+iSel, true);

		m_ctlShortcuts.Arrange(LVA_DEFAULT);
	}
}

///////////////////////////////////////////////////////////////////////////
// finishing editing shortcut name
void CFolderDialog::OnEndLabelEditShortcutList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pdi = (NMLVDISPINFO*)pNMHDR;

	// editing has been cancelled - delete item
	if (pdi->item.pszText == NULL)
	{
		m_ctlShortcuts.DeleteItem(pdi->item.iItem);
		m_bdData.cvShortcuts.erase(m_bdData.cvShortcuts.begin()+pdi->item.iItem, true);
		
		*pResult=0;
		return;
	}

	// std editing - success
	CShortcut sc=CString(m_bdData.cvShortcuts.at(pdi->item.iItem));
	sc.m_strName=pdi->item.pszText;

	m_bdData.cvShortcuts.replace(m_bdData.cvShortcuts.begin()+pdi->item.iItem, (const PTSTR)(LPCTSTR)(CString)sc, true, true);
	
	*pResult=1;
}

////////////////////////////////////////////////////////////////////////////
// sets the style of a shortcuts list
void CFolderDialog::SetView(int iView)
{
	DWORD dwView=0;
	switch (iView)
	{
	case 0:
		dwView=LVS_ICON;
		break;
	case 2:
		dwView=LVS_SMALLICON;
		break;
	case 3:
		dwView=LVS_REPORT;
		break;
	default:
		dwView=LVS_LIST;
		break;
	}

	DWORD dwStyle = GetWindowLong(m_ctlShortcuts.GetSafeHwnd(), GWL_STYLE); 
 
	// Only set the window style if the view bits have changed.
	if ((dwStyle & LVS_TYPEMASK) != dwView) 
		SetWindowLongPtr(m_ctlShortcuts.GetSafeHwnd(), GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	m_ctlShortcuts.Arrange(LVA_ALIGNTOP);
}

///////////////////////////////////////////////////////////////////////////
// large icons
void CFolderDialog::OnIconsRadio() 
{
	m_bdData.iView=0;
	SetView(0);
	m_ctlLargeIcons.Invalidate();
	m_ctlSmallIcons.Invalidate();
	m_ctlList.Invalidate();
	m_ctlReport.Invalidate();
}

///////////////////////////////////////////////////////////////////////////
// small icons
void CFolderDialog::OnSmalliconsRadio() 
{
	m_bdData.iView=1;
	SetView(1);
	m_ctlLargeIcons.Invalidate();
	m_ctlSmallIcons.Invalidate();
	m_ctlList.Invalidate();
	m_ctlReport.Invalidate();
}

///////////////////////////////////////////////////////////////////////////
// list button
void CFolderDialog::OnListRadio() 
{
	m_bdData.iView=2;
	SetView(2);
	m_ctlLargeIcons.Invalidate();
	m_ctlSmallIcons.Invalidate();
	m_ctlList.Invalidate();
	m_ctlReport.Invalidate();
}

///////////////////////////////////////////////////////////////////////////
// report button
void CFolderDialog::OnReportRadio() 
{
	m_bdData.iView=3;
	SetView(3);
	m_ctlLargeIcons.Invalidate();
	m_ctlSmallIcons.Invalidate();
	m_ctlList.Invalidate();
	m_ctlReport.Invalidate();
}

////////////////////////////////////////////////////////////////////////////
// resize handling
void CFolderDialog::OnSize(UINT nType, int cx, int cy) 
{
	CLanguageDialog::OnSize(nType, cx, cy);
	
	ResizeControls(cx, cy);
	InvalidateRect(&m_rcGripper);
}

////////////////////////////////////////////////////////////////////////////
// repositions controls within the dialog
void CFolderDialog::ResizeControls(int cx, int cy)
{
	// is app themed ?
	CUxThemeSupport uxt;
	bool bThemed=uxt.IsThemeSupported() && uxt.IsAppThemed();

	// settings
	const int iMargin=7;	// dialog units
	const int iTextCount=3;	// iloœæ linii textu w staticu

	// small buttons
	const int iBmpMargin=1;	// margin between button and a bitmap within
	const int iButtonWidth=(bThemed ? 20 : 16)+2*GetSystemMetrics(SM_CXEDGE)+2*iBmpMargin;	// fixed size
	const int iButtonHeight=(bThemed ? 20 : 16)+2*GetSystemMetrics(SM_CYEDGE)+2*iBmpMargin;

	// large buttons
	const int iLargeButtonHeight=14;	// OK&Cancel buttons (dialog units)
	const int iLargeButtonWidth=50;		// width in DU
	const int iToggleButtonWidth=18;	// toggle size

	// combo
	const int iComboHeight=12;
	const int iComboExpandedHeight=100;

	// static text at the top of dialog
	CRect rcStatic( iMargin, iMargin, 0, iMargin+iTextCount*8 );
	MapDialogRect(&rcStatic);
	rcStatic.right=cx-rcStatic.left;
	
	CWnd* pWnd=GetDlgItem(IDC_TITLE_STATIC);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcStatic);
		pWnd->Invalidate();
	}

	// 4*button (**********)
	CRect rcButton(iMargin, iMargin+iTextCount*8+5, 0, 0);
	MapDialogRect(&rcButton);
	rcButton.right=rcButton.left+iButtonWidth;
	rcButton.bottom=rcButton.top+iButtonHeight;
	pWnd=GetDlgItem(IDC_LARGEICONS_BUTTON);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	rcButton.left+=iButtonWidth;
	rcButton.right+=iButtonWidth;
	pWnd=GetDlgItem(IDC_SMALLICONS_BUTTON);
	if (pWnd)
		pWnd->MoveWindow(&rcButton);

	rcButton.left+=iButtonWidth;
	rcButton.right+=iButtonWidth;
	pWnd=GetDlgItem(IDC_LIST_BUTTON);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	rcButton.left+=iButtonWidth;
	rcButton.right+=iButtonWidth;
	pWnd=GetDlgItem(IDC_REPORT_BUTTON);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	// new folder button 
	rcButton=CRect(iMargin, iMargin+iTextCount*8+5, iMargin, 0);
	MapDialogRect(&rcButton);
	rcButton.left=cx-rcButton.left-iButtonWidth-1;	// off by 1 - better looks
	rcButton.right=cx-rcButton.right-1;
	rcButton.bottom=rcButton.top+iButtonHeight;
	pWnd=GetDlgItem(IDC_NEWFOLDER_BUTTON);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	// shortcuts list (**********)
	CRect rcShortcuts(iMargin, iMargin+iTextCount*8+5+1, 2*iMargin+5, iMargin+iLargeButtonHeight+5+iComboHeight+3);
	MapDialogRect(&rcShortcuts);
	rcShortcuts.top+=iButtonHeight;	// small button size
	rcShortcuts.right=MulDiv((cx-rcShortcuts.right), 35, 120);
	rcShortcuts.bottom=cy-rcShortcuts.bottom;
	pWnd=GetDlgItem(IDC_SHORTCUT_LIST);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcShortcuts);
		pWnd->Invalidate();
	}

	// button toggle
	rcButton=CRect(iMargin+(m_bdData.bExtended ? 5 : 0), iMargin+iTextCount*8+5, iToggleButtonWidth, iLargeButtonHeight);
	MapDialogRect(&rcButton);
	if (m_bdData.bExtended)
		rcButton.left+=rcShortcuts.Width()+1;
	rcButton.right+=rcButton.left;
	rcButton.bottom+=rcButton.top;
	pWnd=GetDlgItem(IDC_TOGGLE_BUTTON);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	// tree ctrl
	CRect rcTree(iMargin+(m_bdData.bExtended ? 5 : 0), iMargin+iTextCount*8+5+1, iMargin, iMargin+iLargeButtonHeight+5+iComboHeight+3 );
	MapDialogRect(&rcTree);
	rcTree.top+=iButtonHeight;
	rcTree.bottom=cy-rcTree.bottom;
	rcTree.right=cx-rcTree.right;
	if (m_bdData.bExtended)
		rcTree.left+=rcShortcuts.Width();
	pWnd=GetDlgItem(IDC_FOLDER_TREE);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcTree);
		pWnd->Invalidate();
	}

	// combo
	CRect rcCombo(iMargin+(m_bdData.bExtended ? 5 : 0), iMargin+iLargeButtonHeight+5+iComboHeight, iMargin, iComboExpandedHeight/*iMargin+iLargeButtonHeight+5*/);
	MapDialogRect(&rcCombo);
	if (m_bdData.bExtended)
		rcCombo.left+=rcShortcuts.Width();
	rcCombo.top=cy-rcCombo.top;
	rcCombo.right=cx-rcCombo.right;
	rcCombo.bottom+=rcCombo.top;
	pWnd=GetDlgItem(IDC_PATH_COMBOBOXEX);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcCombo);
		pWnd->Invalidate();
	}

	// button - add shortcut/remove shortcut
	rcButton=CRect(0, iMargin+iLargeButtonHeight+5+iComboHeight, 0, 0);
	MapDialogRect(&rcButton);
	rcButton.top=cy-rcButton.top;
	rcButton.left=rcShortcuts.right-iButtonWidth;
	rcButton.right=rcButton.left+iButtonWidth;
	rcButton.bottom=rcButton.top+iButtonHeight;
	pWnd=GetDlgItem(IDC_ADDSHORTCUT_BUTTON);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	rcButton.left-=iButtonWidth;
	rcButton.right-=iButtonWidth;
	pWnd=GetDlgItem(IDC_REMOVESHORTCUT_BUTTON);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	// buttony - ok & cancel
	rcButton=CRect(iMargin+2*iLargeButtonWidth+3, iMargin+iLargeButtonHeight, iMargin+iLargeButtonWidth+3, iMargin);
	MapDialogRect(&rcButton);
	rcButton.left=cx-rcButton.left;
	rcButton.top=cy-rcButton.top;
	rcButton.right=cx-rcButton.right;
	rcButton.bottom=cy-rcButton.bottom;
	pWnd=GetDlgItem(IDOK);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}

	rcButton=CRect(iMargin+iLargeButtonWidth, iMargin+iLargeButtonHeight, iMargin, iMargin);
	MapDialogRect(&rcButton);
	rcButton.left=cx-rcButton.left;
	rcButton.top=cy-rcButton.top;
	rcButton.right=cx-rcButton.right;
	rcButton.bottom=cy-rcButton.bottom;
	pWnd=GetDlgItem(IDCANCEL);
	if (pWnd)
	{
		pWnd->MoveWindow(&rcButton);
		pWnd->Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////////
// minimum size of a window
void CFolderDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CRect rcMin(0, 0, 200, 160);
	MapDialogRect(&rcMin);
	lpMMI->ptMinTrackSize=rcMin.BottomRight();
}

/////////////////////////////////////////////////////////////////////////////
// paints a gripper
void CFolderDialog::OnPaint() 
{
	if (!IsZoomed())
	{
		CPaintDC dc(this); // device context for painting
		
		CRect rc;
		GetClientRect(rc);
		
		rc.left = rc.right-GetSystemMetrics(SM_CXHSCROLL);
		rc.top = rc.bottom-GetSystemMetrics(SM_CYVSCROLL);
		m_rcGripper=rc;
		dc.DrawFrameControl(rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}
	else
	{
		CPaintDC dc(this);
		DefWindowProc(WM_PAINT, (WPARAM)dc.GetSafeHdc(), 0);
	}
}

////////////////////////////////////////////////////////////////////////////
// hit testing in a gripper cause
LRESULT CFolderDialog::OnNcHitTest(CPoint point) 
{
	LRESULT uiRes = CLanguageDialog::OnNcHitTest(point);
	if (uiRes == HTCLIENT)
	{
		CRect rc;
		GetWindowRect(&rc);
		rc.left = rc.right-GetSystemMetrics(SM_CXHSCROLL);
		rc.top = rc.bottom-GetSystemMetrics(SM_CYVSCROLL);
		if (rc.PtInRect(point))
			uiRes = HTBOTTOMRIGHT;
	}

	return uiRes;
}

////////////////////////////////////////////////////////////////////////////
// returns combo's path after dialog finishes
void CFolderDialog::GetPath(LPTSTR pszPath)
{
	_tcscpy(pszPath, m_strPath);
}

////////////////////////////////////////////////////////////////////////////
// returns combo's path after dialog finishes
void CFolderDialog::GetPath(CString &rstrPath)
{
	rstrPath=m_strPath;
}

////////////////////////////////////////////////////////////////////////////
// opens choose folder dialog
INT_PTR BrowseForFolder(CFolderDialog::BROWSEDATA* pData, LPTSTR pszPath)
{
	_ASSERTE(pData && pszPath);
	if(!pData || !pszPath)
		return IDCANCEL;
	ASSERT(pData);
	ASSERT(pszPath);
	CFolderDialog dlg;
	dlg.m_bdData=*pData;

	INT_PTR iResult = dlg.DoModal();
	if (iResult == IDOK)
	{
		dlg.GetPath(pszPath);		// returned path
		*pData=dlg.m_bdData;		// future
	}
	else
		_tcscpy(pszPath, _T(""));

	return iResult;
}
