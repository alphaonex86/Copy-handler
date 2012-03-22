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
#pragma once

// CCrashDlg dialog

class CCrashDlg : public CDialog
{
	DECLARE_DYNAMIC(CCrashDlg)

public:
	CCrashDlg(bool bResult, PCTSTR pszFilename, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCrashDlg();

	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_CRASH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	bool m_bResult;
	CString m_strFilename;
public:
	CStatic m_ctlVersion;
	CEdit m_ctlLocation;
	CButton m_ctlOKButton;
	CStatic m_ctlInfo;
	CStatic m_ctlVersionInfo;
	CStatic m_ctlLocationInfo;
};
