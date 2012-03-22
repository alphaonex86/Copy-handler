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
#include "ShutdownDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShutdownDlg dialog


CShutdownDlg::CShutdownDlg()
	:ictranslate::CLanguageDialog(CShutdownDlg::IDD)
{
	//{{AFX_DATA_INIT(CShutdownDlg)
	m_strTime = _T("");
	//}}AFX_DATA_INIT
}


void CShutdownDlg::DoDataExchange(CDataExchange* pDX)
{
	CLanguageDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShutdownDlg)
	DDX_Control(pDX, IDC_TIME_PROGRESS, m_ctlProgress);
	DDX_Text(pDX, IDC_TIME_STATIC, m_strTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShutdownDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CShutdownDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShutdownDlg message handlers

BOOL CShutdownDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();
	
	// make on top
	SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE /*| SWP_SHOWWINDOW*/);

	// init progress
	m_iTime=m_iOverallTime;
	m_ctlProgress.SetRange32(0, m_iOverallTime);

	// init timer
	SetTimer(6678, 200, NULL);
	
	return TRUE;
}

void CShutdownDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == 6678)
	{
		m_iTime-=200;
		if (m_iTime < 0)
			m_iTime=0;

		m_ctlProgress.SetPos(m_iOverallTime-m_iTime);
		FormatTimeString(m_iTime, &m_strTime);
		UpdateData(FALSE);

		if (m_iTime == 0)
			EndDialog(IDOK);
	}
	
	CLanguageDialog::OnTimer(nIDEvent);
}

void CShutdownDlg::FormatTimeString(int iTime, CString *pstrData)
{
	pstrData->Format(_T("%lu s."), iTime/1000);
}
