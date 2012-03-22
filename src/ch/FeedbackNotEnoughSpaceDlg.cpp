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
#include "FeedbackNotEnoughSpaceDlg.h"
#include "btnIDs.h"
#include "StringHelpers.h"
#include "..\Common\FileSupport.h"
#include "FeedbackHandler.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFeedbackNotEnoughSpaceDlg dialog


CFeedbackNotEnoughSpaceDlg::CFeedbackNotEnoughSpaceDlg(ull_t ullSizeRequired, const tchar_t* pszSrcPath, const tchar_t* pszDstPath)
	:ictranslate::CLanguageDialog(IDD_FEEDBACK_NOTENOUGHSPACE_DIALOG),
	m_bAllItems(FALSE),
	m_ullRequired(ullSizeRequired),
	m_strDisk(pszDstPath)
{
	m_vstrFiles.push_back(pszSrcPath);
}


void CFeedbackNotEnoughSpaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CLanguageDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFeedbackNotEnoughSpaceDlg)
	DDX_Control(pDX, IDC_FILES_LIST, m_ctlFiles);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_ALL_ITEMS_CHECK, m_bAllItems);
}


BEGIN_MESSAGE_MAP(CFeedbackNotEnoughSpaceDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CFeedbackNotEnoughSpaceDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RETRY_BUTTON, OnRetryButton)
	ON_BN_CLICKED(IDC_IGNORE_BUTTON, OnIgnoreButton)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CFeedbackNotEnoughSpaceDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFeedbackNotEnoughSpaceDlg message handlers
void CFeedbackNotEnoughSpaceDlg::UpdateDialog()
{
	// format needed text
	ictranslate::CFormat fmt(GetResManager().LoadString(IDS_NERPATH_STRING));
	fmt.SetParam(_t("%path"), m_strDisk);

	CWnd* pWnd=GetDlgItem(IDC_HEADER_STATIC);
	if (pWnd)
		pWnd->SetWindowText(fmt);

	// now the sizes
	TCHAR szData[128];
	pWnd=GetDlgItem(IDC_REQUIRED_STATIC);
	if (pWnd)
		pWnd->SetWindowText(GetSizeString(m_ullRequired, szData, 128));
	ull_t ullFree;
	pWnd=GetDlgItem(IDC_AVAILABLE_STATIC);
	if (pWnd && GetDynamicFreeSpace(m_strDisk, &ullFree, NULL))
		pWnd->SetWindowText(GetSizeString(ullFree, szData, 128));
}

BOOL CFeedbackNotEnoughSpaceDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	AddResizableControl(IDC_HEADER_STATIC, 0.0, 0.0, 1.0, 0.0);
	AddResizableControl(IDC_001_STATIC, 0.0, 0.0, 0.0, 0.0);

	AddResizableControl(IDC_FILES_LIST, 0.0, 0.0, 1.0, 1.0);

	AddResizableControl(IDC_003_STATIC, 0.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_004_STATIC, 0.0, 1.0, 0.0, 0.0);

	AddResizableControl(IDC_REQUIRED_STATIC, 0.0, 1.0, 1.0, 0.0);
	AddResizableControl(IDC_AVAILABLE_STATIC, 0.0, 1.0, 1.0, 0.0);

	AddResizableControl(IDC_RETRY_BUTTON, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDC_IGNORE_BUTTON, 1.0, 1.0, 0.0, 0.0);
	AddResizableControl(IDCANCEL, 1.0, 1.0, 0.0, 0.0);

	AddResizableControl(IDC_ALL_ITEMS_CHECK, 0.0, 1.0, 1.0, 0.0);

	InitializeResizableControls();

	// set to top
	SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE /*| SWP_SHOWWINDOW*/);

	// needed data
	for (size_t i=0;i<m_vstrFiles.size();i++)
		m_ctlFiles.AddString(m_vstrFiles.at(i).c_str());

	// format needed text
	UpdateDialog();

	SetTimer(1601, 1000, NULL);

	return TRUE;
}

void CFeedbackNotEnoughSpaceDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == 1601)
	{
		// update free space
		ull_t ullFree;
		CWnd *pWnd=GetDlgItem(IDC_AVAILABLE_STATIC);
		if (pWnd && GetDynamicFreeSpace(m_strDisk, &ullFree, NULL))
		{
			TCHAR szData[128];
			pWnd->SetWindowText(GetSizeString(ullFree, szData, 128));

			// end dialog if this is enough
			if (m_ullRequired <= ullFree)
			{
				CLanguageDialog::OnTimer(nIDEvent);
				EndDialog(ID_RETRY);
			}
		}
	}
	
	CLanguageDialog::OnTimer(nIDEvent);
}

void CFeedbackNotEnoughSpaceDlg::OnRetryButton() 
{
	UpdateData(TRUE);
	EndDialog(CFeedbackHandler::eResult_Retry);	
}

void CFeedbackNotEnoughSpaceDlg::OnIgnoreButton() 
{
	UpdateData(TRUE);
	EndDialog(CFeedbackHandler::eResult_Skip);
}

void CFeedbackNotEnoughSpaceDlg::OnLanguageChanged()
{
	UpdateDialog();
}

void CFeedbackNotEnoughSpaceDlg::OnBnClickedCancel()
{
	UpdateData(TRUE);
	EndDialog(CFeedbackHandler::eResult_Cancel);
}
