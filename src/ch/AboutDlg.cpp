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
#include "AboutDlg.h"
#include "StaticEx.h"
#include "../common/version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CAboutDlg::m_bLock=false;

CAboutDlg::CAboutDlg() :ictranslate::CLanguageDialog(CAboutDlg::IDD, NULL, &m_bLock)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	RegisterStaticExControl(AfxGetInstanceHandle());
}

CAboutDlg::~CAboutDlg()
{
}

BEGIN_MESSAGE_MAP(CAboutDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAboutDlg::UpdateProgramVersion()
{
	CWnd* pCtl = GetDlgItem(IDC_PROGRAM_STATICEX);
	CWnd* pCtl2 = GetDlgItem(IDC_FULLVERSION_STATICEX);
	CWnd* pWndCopyright = GetDlgItem(IDC_COPYRIGHT_STATIC);
	CWnd* pctlSite = GetDlgItem(IDC_HOMEPAGELINK_STATIC);
	CWnd* pctlLanguage = GetDlgItem(IDC_LANGUAGE_STATIC);
	CWnd* pctlContact = GetDlgItem(IDC_CONTACT1LINK_STATIC);

	if (!pCtl || !pCtl2 || !pWndCopyright || !pctlSite || !pctlLanguage || !pctlContact)
		return;

	// Readable version
	pCtl->SetWindowText(GetApp().GetAppNameVer());

	// full version
	CString strText;
	strText.Format(_T("%s: %ld.%ld.%ld.%ld"), GetResManager().LoadString(IDS_ABOUTVERSION_STRING),
		PRODUCT_VERSION1, PRODUCT_VERSION2, PRODUCT_VERSION3, PRODUCT_VERSION4);
	pCtl2->SetWindowText(strText);
	// Copyright information
	pWndCopyright->SetWindowText(_T(COPYRIGHT_INFO));

	// web page link
	pctlSite->SetWindowText(_T(PRODUCT_SITE) _T("|") _T(PRODUCT_SITE));
	pctlContact->SetWindowText(_T(CONTACT_INFO) _T("|") _T(CONTACT_INFO));

	// language information
	ictranslate::CResourceManager& rResManager = GetResManager();
	const ictranslate::CLangData* pLangData = rResManager.GetLanguageData();
	if(pLangData)
	{
		ictranslate::CFormat fmt(rResManager.LoadString(IDS_ABOUT_LANGUAGE_STRING));
		fmt.SetParam(_t("%langname"), pLangData->GetLangName());
		fmt.SetParam(_t("%authors"), pLangData->GetAuthor());

		pctlLanguage->SetWindowText(fmt);
	}
}

BOOL CAboutDlg::OnInitDialog()
{
	CLanguageDialog::OnInitDialog();

	UpdateProgramVersion();

	return TRUE;
}

void CAboutDlg::OnLanguageChanged()
{
	UpdateProgramVersion();
}

BOOL CAboutDlg::OnTooltipText(UINT uiID, TOOLTIPTEXT* pTip)
{
	switch(uiID)
	{
	case IDC_HOMEPAGELINK_STATIC:
	case IDC_CONTACT1LINK_STATIC:
	case IDC_CONTACT2LINK_STATIC:
		{
			HWND hWnd=::GetDlgItem(this->m_hWnd, uiID);
			if (!hWnd)
				return FALSE;
			::SendMessage(hWnd, SEM_GETLINK, (WPARAM)79, (LPARAM)pTip->szText);
			pTip->szText[79]=_T('\0');
			return TRUE;
		}
	default:
		return FALSE;
	}
}
