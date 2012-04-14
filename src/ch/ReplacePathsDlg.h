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
#ifndef __REPLACEPATHSDLG_H__
#define __REPLACEPATHSDLG_H__

class CTask;

/////////////////////////////////////////////////////////////////////////////
// CReplacePathsDlg dialog

class CReplacePathsDlg : public ictranslate::CLanguageDialog
{
// Construction
public:
	CReplacePathsDlg();   // standard constructor

	CTask* m_pTask;
// Dialog Data
	//{{AFX_DATA(CReplacePathsDlg)
	enum { IDD = IDD_REPLACE_PATHS_DIALOG };
	CListBox	m_ctlPathsList;
	CString	m_strDest;
	CString	m_strSource;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplacePathsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReplacePathsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePathsList();
	virtual void OnOK();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
