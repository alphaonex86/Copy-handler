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
#ifndef __SHORTCUTSDLG_H__
#define __SHORTCUTSDLG_H__

#include "charvect.h"

/////////////////////////////////////////////////////////////////////////////
// CShortcutsDlg dialog

class CShortcutsDlg : public ictranslate::CLanguageDialog
{
// Construction
public:
	CShortcutsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShortcutsDlg)
	enum { IDD = IDD_SHORTCUTEDIT_DIALOG };
	CComboBoxEx	m_ctlPath;
	CListCtrl	m_ctlShortcuts;
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShortcutsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	const char_vector *m_pcvRecent;	// one way only
	char_vector m_cvShortcuts;		// two-way - shortcuts are being returned through this member
protected:
	void UpdateComboIcon();
	void SetComboPath(LPCTSTR lpszPath);
	HIMAGELIST m_himl, m_hliml;
	bool m_bActualisation;

	// Generated message map functions
	//{{AFX_MSG(CShortcutsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedShortcutList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditchangePathComboboxex();
	afx_msg void OnAddButton();
	afx_msg void OnChangeButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnBrowseButton();
	afx_msg void OnUpButton();
	afx_msg void OnDownButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
