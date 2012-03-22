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
// CrashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ch.h"
#include "CrashDlg.h"
#include "../common/version.h"

#define IDS_CRASH_TITLE			_T("Application crashed")
#define IDS_STATIC_INFO			_T("Copy Handler encountered an internal problem and will be closed.\n\nIf you want to help correct this problem in the future releases of program you can send the crash information to the author of this program (e-mail it to ixen@copyhandler.com).")
#define IDS_VERSIONINFO_STATIC	_T("Program version:")
#define IDS_LOCATIONINFO_STATIC _T("Crash dump location:")
#define IDS_LOCATION_STATIC		_T("Error encountered while trying to create crash dump")
#define IDS_OK					_T("&Close")

// CCrashDlg dialog

IMPLEMENT_DYNAMIC(CCrashDlg,ictranslate::CLanguageDialog)

CCrashDlg::CCrashDlg(bool bResult, PCTSTR pszFilename, CWnd* pParent /*=NULL*/)
	: CDialog(CCrashDlg::IDD, pParent),
	m_bResult(bResult),
	m_strFilename(pszFilename)
{
}

CCrashDlg::~CCrashDlg()
{
}

void CCrashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION_STATIC, m_ctlVersion);
	DDX_Control(pDX, IDC_LOCATION_EDIT, m_ctlLocation);
	DDX_Control(pDX, IDOK, m_ctlOKButton);
	DDX_Control(pDX, IDC_STATIC_INFO, m_ctlInfo);
	DDX_Control(pDX, IDC_VERSIONINFO_STATIC, m_ctlVersionInfo);
	DDX_Control(pDX, IDC_LOCATIONINFO_STATIC, m_ctlLocationInfo);
}


BEGIN_MESSAGE_MAP(CCrashDlg, CDialog)
END_MESSAGE_MAP()


// CCrashDlg message handlers

BOOL CCrashDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(IDS_CRASH_TITLE);
	m_ctlInfo.SetWindowText(IDS_STATIC_INFO);
	m_ctlVersionInfo.SetWindowText(IDS_VERSIONINFO_STATIC);
	m_ctlVersion.SetWindowText(PRODUCT_FULL_VERSION_T);
	m_ctlLocationInfo.SetWindowText(IDS_LOCATIONINFO_STATIC);
	if(m_bResult)
		m_ctlLocation.SetWindowText(m_strFilename);
	else
		m_ctlLocation.SetWindowText(IDS_LOCATION_STATIC);

	m_ctlOKButton.SetWindowText(IDS_OK);
	m_ctlOKButton.EnableWindow(m_bResult);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
