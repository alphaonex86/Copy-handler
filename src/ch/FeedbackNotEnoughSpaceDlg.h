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
#ifndef __FEEDBACKNOTENOUGHSPACEDLG_H__
#define __FEEDBACKNOTENOUGHSPACEDLG_H__

/////////////////////////////////////////////////////////////////////////////
// CFeedbackNotEnoughSpaceDlg dialog

class CFeedbackNotEnoughSpaceDlg : public ictranslate::CLanguageDialog
{
// Construction
public:
	CFeedbackNotEnoughSpaceDlg(ull_t ullSizeRequired, const tchar_t* pszSrcPath, const tchar_t* pszDstPath);   // standard constructor

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	CString	m_strDisk;
	ull_t m_ullRequired;
	std::vector<tstring_t> m_vstrFiles;
	CListBox	m_ctlFiles;

protected:
	void UpdateDialog();
	virtual void OnLanguageChanged();

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRetryButton();
	afx_msg void OnIgnoreButton();

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAllItems;
	afx_msg void OnBnClickedCancel();
};

#endif
