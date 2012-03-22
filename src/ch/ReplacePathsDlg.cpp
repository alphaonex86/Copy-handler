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
#include "resource.h"
#include "ReplacePathsDlg.h"
#include "dialogs.h"
#include "ch.h"
#include "task.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReplacePathsDlg dialog


CReplacePathsDlg::CReplacePathsDlg()
	: ictranslate::CLanguageDialog(CReplacePathsDlg::IDD)
{
	//{{AFX_DATA_INIT(CReplacePathsDlg)
	m_strDest = _T("");
	m_strSource = _T("");
	//}}AFX_DATA_INIT
}


void CReplacePathsDlg::DoDataExchange(CDataExchange* pDX)
{
	CLanguageDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplacePathsDlg)
	DDX_Control(pDX, IDC_PATHS_LIST, m_ctlPathsList);
	DDX_Text(pDX, IDC_DESTINATION_EDIT, m_strDest);
	DDX_Text(pDX, IDC_SOURCE_EDIT, m_strSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReplacePathsDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CReplacePathsDlg)
	ON_LBN_SELCHANGE(IDC_PATHS_LIST, OnSelchangePathsList)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReplacePathsDlg message handlers

BOOL CReplacePathsDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	AddResizableControl(IDC_001_STATIC, 0.0, 0.0, 1.0, 0.0);
	AddResizableControl(IDC_PATHS_LIST, 0.0, 0.0, 1.0, 1.0);
	AddResizableControl(IDC_SOURCE_EDIT, 0.0, 1.0, 1.0, 0.0);
	
	AddResizableControl(IDC_002_STATIC, 0.0, 1.0, 1.0, 0.0);
	
	AddResizableControl(IDC_DESTINATION_EDIT, 0.0, 1.0, 1.0, 0.0);
	AddResizableControl(IDC_BROWSE_BUTTON, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDOK, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDCANCEL, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_HELP_BUTTON, 1.0, 1.0, 0.0, 0.0);

	InitializeResizableControls();

	for (int i=0;i<m_pTask->GetClipboardDataSize();i++)
		m_ctlPathsList.AddString(m_pTask->GetClipboardData(i)->GetPath());
	
	return TRUE;
}

void CReplacePathsDlg::OnSelchangePathsList() 
{
	int iSel=m_ctlPathsList.GetCurSel();
	if (iSel == LB_ERR)
		return;

	m_ctlPathsList.GetText(iSel, m_strSource);
	UpdateData(FALSE);
}

void CReplacePathsDlg::OnOK() 
{
	UpdateData(TRUE);
	if (m_strSource.IsEmpty())
		MsgBox(IDS_SOURCESTRINGMISSING_STRING);
	else
		CLanguageDialog::OnOK();
}

void CReplacePathsDlg::OnBrowseButton() 
{
	CString strPath;
	if (BrowseForFolder(GetResManager().LoadString(IDS_BROWSE_STRING), &strPath))
	{
		UpdateData(TRUE);
		m_strDest=strPath;
		UpdateData(FALSE);
	}
}
