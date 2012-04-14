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
#ifndef __FEEDBACKFILEERRORDLG_H__
#define __FEEDBACKFILEERRORDLG_H__

// CFeedbackFileErrorDlg dialog
class CFeedbackFileErrorDlg : public ictranslate::CLanguageDialog
{
	DECLARE_DYNAMIC(CFeedbackFileErrorDlg)

public:
	CFeedbackFileErrorDlg(const tchar_t* pszSrcPath, const tchar_t* pszDstPath, ulong_t ulSysError, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFeedbackFileErrorDlg();

	afx_msg void OnBnClickedRetryButton();
	afx_msg void OnBnClickedSkipButton();
	afx_msg void OnBnClickedPauseButton();
	afx_msg void OnBnClickedCancel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAllItems;
	CStatic m_ctlErrorInfo;
	CString m_strSrcPath;
	CString m_strDstPath;
	ulong_t m_ulSysError;
	virtual BOOL OnInitDialog();
};

#endif
