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
#include "ShortcutsDlg.h"
#include "dialogs.h"
#include "shortcuts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShortcutsDlg dialog


CShortcutsDlg::CShortcutsDlg(CWnd* pParent /*=NULL*/)
	:ictranslate::CLanguageDialog(CShortcutsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShortcutsDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_bActualisation=false;
}


void CShortcutsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShortcutsDlg)
	DDX_Control(pDX, IDC_PATH_COMBOBOXEX, m_ctlPath);
	DDX_Control(pDX, IDC_SHORTCUT_LIST, m_ctlShortcuts);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShortcutsDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CShortcutsDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SHORTCUT_LIST, OnItemchangedShortcutList)
	ON_CBN_EDITCHANGE(IDC_PATH_COMBOBOXEX, OnEditchangePathComboboxex)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_CHANGE_BUTTON, OnChangeButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_BN_CLICKED(IDC_UP_BUTTON, OnUpButton)
	ON_BN_CLICKED(IDC_DOWN_BUTTON, OnDownButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShortcutsDlg message handlers

BOOL CShortcutsDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	AddResizableControl(IDC_001_STATIC, 0, 0, 1, 0);
	AddResizableControl(IDC_SHORTCUT_LIST, 0, 0, 1, 1);
	AddResizableControl(IDC_UP_BUTTON, 1, 0, 0, 0);
	AddResizableControl(IDC_DOWN_BUTTON, 1, 0, 0, 0);
	
	AddResizableControl(IDC_004_STATIC, 0, 1, 1, 0);
	AddResizableControl(IDC_002_STATIC, 0, 1, 0, 0);
	AddResizableControl(IDC_003_STATIC, 0, 1, 0, 0);

	AddResizableControl(IDC_NAME_EDIT, 0, 1, 1, 0);
	AddResizableControl(IDC_PATH_COMBOBOXEX, 0, 1, 1, 0);
	AddResizableControl(IDC_BROWSE_BUTTON, 1, 1, 0, 0);

	AddResizableControl(IDC_ADD_BUTTON, 0, 1, 0, 0);
	AddResizableControl(IDC_CHANGE_BUTTON, 0, 1, 0, 0);
	AddResizableControl(IDC_DELETE_BUTTON, 0, 1, 0, 0);
	AddResizableControl(IDOK, 1, 1, 0, 0);
	AddResizableControl(IDCANCEL, 1, 1, 0, 0);
	AddResizableControl(IDC_HELP_BUTTON, 1, 1, 0, 0);

	InitializeResizableControls();

	// system image list
	SHFILEINFO sfi;
	m_himl = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	m_hliml=(HIMAGELIST)SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX);
	m_ctlPath.SendMessage(CBEM_SETIMAGELIST, 0, (LPARAM)m_himl);
	m_ctlShortcuts.SendMessage(LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)m_himl);
	m_ctlShortcuts.SendMessage(LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, (LPARAM)m_hliml);

	// copy all of the recent paths to combo
	COMBOBOXEXITEM cbi;
	cbi.mask=CBEIF_IMAGE | CBEIF_TEXT;

	for (int i=0;i<(int)m_pcvRecent->size();i++)
	{
		cbi.iItem=i;
		cbi.pszText=m_pcvRecent->at(i);
		sfi.iIcon=-1;
		SHGetFileInfo(cbi.pszText, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		cbi.iImage=sfi.iIcon;

		m_ctlPath.InsertItem(&cbi);
	}

	// create columns in shortcuts list
	LVCOLUMN lvc;
	lvc.mask=LVCF_SUBITEM | LVCF_WIDTH | LVCF_TEXT;
	lvc.iSubItem=-1;
	lvc.cx=100;
	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_SHORTCUTNAME_STRING);
	m_ctlShortcuts.InsertColumn(0, &lvc);
	lvc.iSubItem=0;
	lvc.cx=200;
	lvc.pszText=(PTSTR)GetResManager().LoadString(IDS_SHORTCUTPATH_STRING);
	m_ctlShortcuts.InsertColumn(1, &lvc);

	// modify list style
	m_ctlShortcuts.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_INFOTIP | LVS_EX_UNDERLINEHOT);

	// update shortcut list
	CShortcut sc;
	for (int i=0;i<(int)m_cvShortcuts.size();i++)
	{
		sc=CString(m_cvShortcuts.at(i));
		sfi.iIcon=-1;
		SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		m_ctlShortcuts.InsertItem(i, sc.m_strName, sfi.iIcon);
		m_ctlShortcuts.SetItem(i, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);
	}

	return TRUE;
}

void CShortcutsDlg::OnItemchangedShortcutList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* plv = (NM_LISTVIEW*)pNMHDR;

	// current selection
	if (plv->iItem >= 0 && plv->iItem < (int)m_cvShortcuts.size())
	{
		CShortcut sc(CString(m_cvShortcuts.at(plv->iItem)));
		m_strName=sc.m_strName;
		UpdateData(FALSE);
		SetComboPath(sc.m_strPath);
	}

	*pResult = 0;
}

void CShortcutsDlg::SetComboPath(LPCTSTR lpszPath)
{
	// unselect
	m_ctlPath.SetCurSel(-1);

	SHFILEINFO sfi;
	sfi.iIcon=-1;

	COMBOBOXEXITEM cbi;

	cbi.mask=CBEIF_TEXT | CBEIF_IMAGE;
	cbi.iItem=-1;
	cbi.pszText=(LPTSTR)lpszPath;
	SHGetFileInfo(cbi.pszText, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	cbi.iImage=sfi.iIcon;

	m_ctlPath.SetItem(&cbi);
}

void CShortcutsDlg::UpdateComboIcon()
{
	// get combo text
	COMBOBOXEXITEM cbi;
	TCHAR szPath[_MAX_PATH];
	memset(szPath, 0, _MAX_PATH);
	cbi.mask=CBEIF_TEXT;
	cbi.iItem=m_ctlPath.GetCurSel()/*-1*/;
	cbi.pszText=szPath;
	cbi.cchTextMax=_MAX_PATH;

	if (!m_ctlPath.GetItem(&cbi))
		return;

	// unselect
	m_ctlPath.SetCurSel(-1);

	// icon update
	SHFILEINFO sfi;
	sfi.iIcon=-1;

	cbi.mask |= CBEIF_IMAGE;
	cbi.iItem=-1;

	CString str=(LPCTSTR)szPath;
	if (str.Left(2) != _T("\\\\") || str.Find(_T('\\'), 2) != -1)
		SHGetFileInfo(cbi.pszText, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	
	cbi.iImage=sfi.iIcon;
	
	m_ctlPath.SetItem(&cbi);

	// unselect text
	CEdit* pEdit=m_ctlPath.GetEditCtrl();
	if (!pEdit)
		return;

	pEdit->SetSel(-1, -1);
}

void CShortcutsDlg::OnEditchangePathComboboxex() 
{
	if (m_bActualisation)
		return;
	m_bActualisation=true;
	UpdateComboIcon();
	m_bActualisation=false;
}

void CShortcutsDlg::OnAddButton() 
{
	// create new shortcut
	UpdateData(TRUE);
	CShortcut sc;
	sc.m_strName=m_strName;
	m_ctlPath.GetWindowText(sc.m_strPath);

	// add to an array
	m_cvShortcuts.push_back((const PTSTR)(LPCTSTR)(CString)sc, true);

	// add with an icon
	SHFILEINFO sfi;
	sfi.iIcon=-1;
	SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
	m_ctlShortcuts.InsertItem((UINT)m_cvShortcuts.size() - 1, sc.m_strName, sfi.iIcon);
	m_ctlShortcuts.SetItem((int)m_cvShortcuts.size() - 1, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);
}

void CShortcutsDlg::OnChangeButton() 
{
	// get selection index
	POSITION pos=m_ctlShortcuts.GetFirstSelectedItemPosition();
	if (pos)
	{
		// index
		int iPos=m_ctlShortcuts.GetNextSelectedItem(pos);

		// get new shortcut
		UpdateData(TRUE);
		CShortcut sc;
		sc.m_strName=m_strName;
		m_ctlPath.GetWindowText(sc.m_strPath);

		// array update
		m_cvShortcuts.replace(m_cvShortcuts.begin()+iPos, (const PTSTR)(LPCTSTR)(CString)sc, true, true);

		// list
		SHFILEINFO sfi;
		sfi.iIcon=-1;
		SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		
		m_ctlShortcuts.DeleteItem(iPos);

		m_ctlShortcuts.InsertItem(iPos, sc.m_strName, sfi.iIcon);
		m_ctlShortcuts.SetItem(iPos, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);
	}
}

void CShortcutsDlg::OnDeleteButton() 
{
	POSITION pos=m_ctlShortcuts.GetFirstSelectedItemPosition();
	int iPos=-1;
	while (pos)
	{
		iPos=m_ctlShortcuts.GetNextSelectedItem(pos);
		m_cvShortcuts.erase(m_cvShortcuts.begin()+iPos, true);
		m_ctlShortcuts.DeleteItem(iPos);
	}

	if (iPos != -1)
		m_ctlShortcuts.SetItemState(iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void CShortcutsDlg::OnBrowseButton() 
{
	CString strPath;
	if (BrowseForFolder(GetResManager().LoadString(IDS_BROWSE_STRING), &strPath))
		SetComboPath(strPath);
}

void CShortcutsDlg::OnUpButton() 
{
	POSITION pos=m_ctlShortcuts.GetFirstSelectedItemPosition();
	int iPos=-1;
	CShortcut sc;
	while (pos)
	{
		// get current selected item
		iPos=m_ctlShortcuts.GetNextSelectedItem(pos);

		// if the first element is trying to go up to nowhere
		if (iPos == 0)
			break;

		// swap data in m_ascShortcuts
		m_cvShortcuts.swap_items(m_cvShortcuts.begin()+iPos-1, m_cvShortcuts.begin()+iPos);

		// do the same with list
		SHFILEINFO sfi;
		sfi.iIcon=-1;

		sc=CString(m_cvShortcuts.at(iPos-1));
		SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		m_ctlShortcuts.SetItem(iPos-1, -1, LVIF_TEXT | LVIF_IMAGE , sc.m_strName, sfi.iIcon, 0, 0, 0);
		m_ctlShortcuts.SetItem(iPos-1, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);

		sfi.iIcon=-1;
		sc=CString(m_cvShortcuts.at(iPos));
		SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		m_ctlShortcuts.SetItem(iPos, -1, LVIF_TEXT | LVIF_IMAGE, sc.m_strName, sfi.iIcon, 0, 0, 0);
		m_ctlShortcuts.SetItem(iPos, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);

		m_ctlShortcuts.SetItemState(iPos, 0, LVIS_SELECTED | LVIS_FOCUSED);
		m_ctlShortcuts.SetItemState(iPos-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}
}

void CShortcutsDlg::OnDownButton() 
{
	POSITION pos=m_ctlShortcuts.GetFirstSelectedItemPosition();
	int iPos=-1;
	CShortcut sc;
	while (pos)
	{
		// get current selected item
		iPos=m_ctlShortcuts.GetNextSelectedItem(pos);

		// if the last element is trying to go down to nowhere
		if (iPos == m_ctlShortcuts.GetItemCount()-1)
			break;

		// swap data in m_ascShortcuts
		m_cvShortcuts.swap_items(m_cvShortcuts.begin()+iPos, m_cvShortcuts.begin()+iPos+1);

		// do the same with list
		SHFILEINFO sfi;
		sfi.iIcon=-1;

		sc=CString(m_cvShortcuts.at(iPos));
		SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		m_ctlShortcuts.SetItem(iPos, -1, LVIF_TEXT | LVIF_IMAGE , sc.m_strName, sfi.iIcon, 0, 0, 0);
		m_ctlShortcuts.SetItem(iPos, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);

		sfi.iIcon=-1;
		sc=CString(m_cvShortcuts.at(iPos+1));
		SHGetFileInfo(sc.m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		m_ctlShortcuts.SetItem(iPos+1, -1, LVIF_TEXT | LVIF_IMAGE, sc.m_strName, sfi.iIcon, 0, 0, 0);
		m_ctlShortcuts.SetItem(iPos+1, 1, LVIF_TEXT, sc.m_strPath, 0, 0, 0, 0);

		m_ctlShortcuts.SetItemState(iPos, 0, LVIS_SELECTED | LVIS_FOCUSED);
		m_ctlShortcuts.SetItemState(iPos+1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}
}
