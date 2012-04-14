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
#ifndef __BUFFERSIZEDLG_H__
#define __BUFFERSIZEDLG_H__

#include "DataBuffer.h"

/////////////////////////////////////////////////////////////////////////////
// CBufferSizeDlg dialog

class CBufferSizeDlg : public ictranslate::CLanguageDialog
{
// Construction
public:
	CBufferSizeDlg();   // standard constructor

	void SetLANSize(UINT uiSize);
	void SetCDSize(UINT uiSize);
	void SetTwoDisksSize(UINT uiSize);
	void SetOneDiskSize(UINT uiSize);
	void SetDefaultSize(UINT uiSize);
	UINT IndexToValue(int iIndex);

	int m_iActiveIndex;
	BUFFERSIZES m_bsSizes;

// Dialog Data
	//{{AFX_DATA(CBufferSizeDlg)
	enum { IDD = IDD_BUFFERSIZE_DIALOG };
	CComboBox	m_ctlTwoDisksMulti;
	CComboBox	m_ctlOneDiskMulti;
	CComboBox	m_ctlLANMulti;
	CComboBox	m_ctlDefaultMulti;
	CComboBox	m_ctlCDROMMulti;
	UINT	m_uiDefaultSize;
	UINT	m_uiLANSize;
	UINT	m_uiCDROMSize;
	UINT	m_uiOneDiskSize;
	UINT	m_uiTwoDisksSize;
	BOOL	m_bOnlyDefaultCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBufferSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnLanguageChanged();

	void EnableControls(bool bEnable=true);
	// Generated message map functions
	//{{AFX_MSG(CBufferSizeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnOnlydefaultCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
