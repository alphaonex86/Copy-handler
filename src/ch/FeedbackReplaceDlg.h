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
#ifndef __FEEDBACKREPLACEDLG_H__
#define __FEEDBACKREPLACEDLG_H__

class CFileInfo;

class CFeedbackReplaceDlg : public ictranslate::CLanguageDialog
{
	DECLARE_DYNAMIC(CFeedbackReplaceDlg)

public:
	CFeedbackReplaceDlg(const CFileInfo* pfiSrcFile, const CFileInfo* pfiDstFile, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFeedbackReplaceDlg();

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void RefreshFilesInfo();
	void RefreshImages();

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_ctlSrcIcon;
	CStatic m_ctlDstIcon;
	CStatic m_ctlSrcInfo;
	CStatic m_ctlDstInfo;
	BOOL m_bAllItems;

protected:
	const CFileInfo* m_pfiSrcFile;
	const CFileInfo* m_pfiDstFile;
public:
	afx_msg void OnBnClickedReplaceButton();
	afx_msg void OnBnClickedCopyRestButton();
	afx_msg void OnBnClickedSkipButton();
	afx_msg void OnBnClickedPauseButton();
	afx_msg void OnBnClickedCancelButton();
};

#endif
