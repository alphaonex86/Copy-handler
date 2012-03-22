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
#ifndef __FILTERDLG_H__
#define __FILTERDLG_H__

#include "FileFilter.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

class CFilterDlg : public ictranslate::CLanguageDialog
{
// Construction
public:
	CFilterDlg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilterDlg)
	enum { IDD = IDD_FILTER_DIALOG };
	CComboBox	m_ctlExcludeMask;
	CSpinButtonCtrl	m_ctlSpin2;
	CSpinButtonCtrl	m_ctlSpin1;
	CDateTimeCtrl	m_ctlTime2;
	CDateTimeCtrl	m_ctlTime1;
	CComboBox	m_ctlSizeType2;
	CComboBox	m_ctlSizeType1;
	CComboBox	m_ctlSize2Multi;
	CComboBox	m_ctlSize1Multi;
	CComboBox	m_ctlFilter;
	CComboBox	m_ctlDateType;
	CComboBox	m_ctlDateType2;
	CDateTimeCtrl	m_ctlDate2;
	CComboBox	m_ctlDateType1;
	CDateTimeCtrl	m_ctlDate1;
	int		m_iArchive;
	BOOL	m_bAttributes;
	BOOL	m_bDate;
	BOOL	m_bDate2;
	int		m_iDirectory;
	BOOL	m_bFilter;
	int		m_iHidden;
	int		m_iReadOnly;
	BOOL	m_bSize;
	UINT	m_uiSize1;
	BOOL	m_bSize2;
	UINT	m_uiSize2;
	int		m_iSystem;
	BOOL	m_bExclude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSize2(unsigned __int64 ullSize);
	CFileFilter m_ffFilter;
	CStringArray m_astrAddMask;
	CStringArray m_astrAddExcludeMask;

protected:
	virtual void OnLanguageChanged();
	int GetMultiplier(int iIndex);
	void EnableControls();
	void SetSize1(unsigned __int64 ullSize);

	// Generated message map functions
	//{{AFX_MSG(CFilterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAttributesCheck();
	afx_msg void OnDateCheck();
	afx_msg void OnDate2Check();
	afx_msg void OnFilterCheck();
	afx_msg void OnSizeCheck();
	afx_msg void OnSize2Check();
	afx_msg void OnExcludemaskCheck();
	afx_msg void OnDatetimechangeTime1Datetimepicker(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDate1Datetimepicker(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
