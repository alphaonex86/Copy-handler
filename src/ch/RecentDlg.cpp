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
#include "RecentDlg.h"
#include "dialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecentDlg dialog


CRecentDlg::CRecentDlg(CWnd* pParent /*=NULL*/)
	:ictranslate::CLanguageDialog(CRecentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecentDlg)
	m_strPath = _T("");
	//}}AFX_DATA_INIT
}


void CRecentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecentDlg)
	DDX_Control(pDX, IDC_RECENT_LIST, m_ctlRecent);
	DDX_Text(pDX, IDC_PATH_EDIT, m_strPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecentDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CRecentDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RECENT_LIST, OnItemchangedRecentList)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_CHANGE_BUTTON, OnChangeButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecentDlg message handlers

BOOL CRecentDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();
	
	AddResizableControl(IDC_001_STATIC, 0.0, 0.0, 1.0, 0.0);
	AddResizableControl(IDC_RECENT_LIST, 0.0, 0.0, 1.0, 1.0);
	
	AddResizableControl(IDC_002_STATIC, 0.0, 1.0, 1.0, 0.0);

	AddResizableControl(IDC_PATH_EDIT, 0.0, 1.0, 1.0, 0.0);
	AddResizableControl(IDC_BROWSE_BUTTON, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_ADD_BUTTON, 0.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_CHANGE_BUTTON, 0.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_DELETE_BUTTON, 0.0, 1.0, 0.0, 0.0);
	
	AddResizableControl(IDOK, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDCANCEL, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_HELP_BUTTON, 1.0, 1.0, 0.0, 0.0);

	InitializeResizableControls();

	// system image list
	SHFILEINFO sfi;
	m_himl = (HIMAGELIST)SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	m_hliml=(HIMAGELIST)SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX);
	m_ctlRecent.SendMessage(LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)m_himl);
	m_ctlRecent.SendMessage(LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, (LPARAM)m_hliml);

	// modify list style
	m_ctlRecent.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_INFOTIP | LVS_EX_UNDERLINEHOT);

	// update recent paths
	for (int i=0;i<(int)m_cvRecent.size();i++)
	{
		sfi.iIcon=-1;
		SHGetFileInfo(m_cvRecent.at(i), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		m_ctlRecent.InsertItem(i, m_cvRecent.at(i), sfi.iIcon);
	}

	return TRUE;
}

void CRecentDlg::OnItemchangedRecentList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* plv = (NM_LISTVIEW*)pNMHDR;

	// current selection
	if (plv->iItem >= 0 && plv->iItem < (int)m_cvRecent.size())
	{
		m_strPath=m_cvRecent.at(plv->iItem);
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void CRecentDlg::OnBrowseButton() 
{
	CString strPath;
	if (BrowseForFolder(GetResManager().LoadString(IDS_BROWSE_STRING), &strPath))
	{
		m_strPath=strPath;
		UpdateData(FALSE);
	}
}

void CRecentDlg::OnAddButton() 
{
	UpdateData(TRUE);
	if (m_strPath.IsEmpty())
		return;

	// add to a table
	m_cvRecent.push_back((const PTSTR)(LPCTSTR)m_strPath, true);

	// add to list with an icon
	SHFILEINFO sfi;
	sfi.iIcon=-1;
	SHGetFileInfo(m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
	m_ctlRecent.InsertItem((UINT)m_cvRecent.size() - 1, m_strPath, sfi.iIcon);
}

void CRecentDlg::OnChangeButton() 
{
	// read selection index
	POSITION pos=m_ctlRecent.GetFirstSelectedItemPosition();
	if (pos)
	{
		// index
		int iPos=m_ctlRecent.GetNextSelectedItem(pos);

		UpdateData(TRUE);
		
		if (m_strPath.IsEmpty())
			return;

		// array update
		m_cvRecent.replace(m_cvRecent.begin()+iPos, (const PTSTR)(LPCTSTR)m_strPath, true, true);

		// list
		SHFILEINFO sfi;
		sfi.iIcon=-1;
		SHGetFileInfo(m_strPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		
		m_ctlRecent.DeleteItem(iPos);
		m_ctlRecent.InsertItem(iPos, m_strPath, sfi.iIcon);
	}
}

void CRecentDlg::OnDeleteButton() 
{
	POSITION pos=m_ctlRecent.GetFirstSelectedItemPosition();
	int iPos=-1;
	while (pos)
	{
		iPos=m_ctlRecent.GetNextSelectedItem(pos);
		m_cvRecent.erase(m_cvRecent.begin()+iPos, true);
		m_ctlRecent.DeleteItem(iPos);
	}

	if (iPos != -1)
		m_ctlRecent.SetItemState(iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}
