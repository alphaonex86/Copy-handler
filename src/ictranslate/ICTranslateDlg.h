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
// ICTranslateDlg.h : header file
//

#pragma once
#include "../libictranslate/ResourceManager.h"

// CICTranslateDlg dialog
class CICTranslateDlg : public CDialog
{
// Construction
public:
	CICTranslateDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ICTRANSLATE_DIALOG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFileOpenBaseTranslation();
	afx_msg void OnFileOpenYourTranslation();
	afx_msg void OnItemChangedSrcDataList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemChangedDstDataList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCopyButton();
	afx_msg void OnBnClickedApply();
	afx_msg void OnEnKillFocusDstAuthorEdit();
	afx_msg void OnEnKillFocusDstLanguageNameEdit();
	afx_msg void OnEnKillFocusDstHelpFilenameEdit();
	afx_msg void OnBnClickedDstRtlCheck();
	afx_msg void OnEditPreviousToTranslate();
	afx_msg void OnEditNextToTranslate();
	afx_msg void OnEditApplyChange();
	afx_msg void OnEditApplyAndNext();
	afx_msg void OnBnClickedChooseFontButton();
	afx_msg void OnEditCleanupTranslation();
	afx_msg void OnFileNewTranslation();
	afx_msg void OnFileSaveTranslationAs();
	afx_msg void OnFileSaveTranslation();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	static void EnumLngStrings(uint_t uiID, const ictranslate::CTranslationItem* pTranslationItem, ptr_t pData);

	void UpdateBaseLanguageList();
	void UpdateCustomLanguageList();

	void UpdateCustomListImages();
	void UpdateCustomListImage(int iItem, bool bUpdateText);

	static int CALLBACK ListSortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	bool WarnModified() const;

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_ilImages;
	ictranslate::CLangData m_ldBase;
	ictranslate::CLangData m_ldCustom;

	HACCEL m_hAccel;

	// Controls
	CListCtrl m_ctlBaseLanguageList;
	CListCtrl m_ctlCustomLanguageList;
	CRichEditCtrl m_ctlSrcText;
	CRichEditCtrl m_ctlDstText;
	CEdit m_ctlSrcFilename;
	CEdit m_ctlSrcAuthor;
	CEdit m_ctlSrcLanguageName;
	CEdit m_ctlSrcHelpFilename;
	CEdit m_ctlSrcFont;
	CButton m_ctlSrcRTL;
	CEdit m_ctlDstFilename;
	CEdit m_ctlDstAuthor;
	CEdit m_ctlDstLanguageName;
	CEdit m_ctlDstHelpFilename;
	CEdit m_ctlDstFont;
	CButton m_ctlDstRTL;
};
