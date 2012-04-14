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
#include "FilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

CFilterDlg::CFilterDlg()
	:ictranslate::CLanguageDialog(CFilterDlg::IDD)
{
	//{{AFX_DATA_INIT(CFilterDlg)
	m_iArchive = FALSE;
	m_bAttributes = FALSE;
	m_bDate = FALSE;
	m_bDate2 = FALSE;
	m_iDirectory = FALSE;
	m_bFilter = FALSE;
	m_iHidden = FALSE;
	m_iReadOnly = FALSE;
	m_bSize = FALSE;
	m_uiSize1 = 0;
	m_bSize2 = FALSE;
	m_uiSize2 = 0;
	m_iSystem = FALSE;
	m_bExclude = FALSE;
	//}}AFX_DATA_INIT
}


void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CLanguageDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterDlg)
	DDX_Control(pDX, IDC_FILTEREXCLUDE_COMBO, m_ctlExcludeMask);
	DDX_Control(pDX, IDC_SIZE2_SPIN, m_ctlSpin2);
	DDX_Control(pDX, IDC_SIZE1_SPIN, m_ctlSpin1);
	DDX_Control(pDX, IDC_TIME2_DATETIMEPICKER, m_ctlTime2);
	DDX_Control(pDX, IDC_TIME1_DATETIMEPICKER, m_ctlTime1);
	DDX_Control(pDX, IDC_SIZETYPE2_COMBO, m_ctlSizeType2);
	DDX_Control(pDX, IDC_SIZETYPE1_COMBO, m_ctlSizeType1);
	DDX_Control(pDX, IDC_SIZE2MULTI_COMBO, m_ctlSize2Multi);
	DDX_Control(pDX, IDC_SIZE1MULTI_COMBO, m_ctlSize1Multi);
	DDX_Control(pDX, IDC_FILTER_COMBO, m_ctlFilter);
	DDX_Control(pDX, IDC_DATETYPE_COMBO, m_ctlDateType);
	DDX_Control(pDX, IDC_DATE2TYPE_COMBO, m_ctlDateType2);
	DDX_Control(pDX, IDC_DATE2_DATETIMEPICKER, m_ctlDate2);
	DDX_Control(pDX, IDC_DATE1TYPE_COMBO, m_ctlDateType1);
	DDX_Control(pDX, IDC_DATE1_DATETIMEPICKER, m_ctlDate1);
	DDX_Check(pDX, IDC_ARCHIVE_CHECK, m_iArchive);
	DDX_Check(pDX, IDC_ATTRIBUTES_CHECK, m_bAttributes);
	DDX_Check(pDX, IDC_DATE_CHECK, m_bDate);
	DDX_Check(pDX, IDC_DATE2_CHECK, m_bDate2);
	DDX_Check(pDX, IDC_DIRECTORY_CHECK, m_iDirectory);
	DDX_Check(pDX, IDC_FILTER_CHECK, m_bFilter);
	DDX_Check(pDX, IDC_HIDDEN_CHECK, m_iHidden);
	DDX_Check(pDX, IDC_READONLY_CHECK, m_iReadOnly);
	DDX_Check(pDX, IDC_SIZE_CHECK, m_bSize);
	DDX_Text(pDX, IDC_SIZE1_EDIT, m_uiSize1);
	DDX_Check(pDX, IDC_SIZE2_CHECK, m_bSize2);
	DDX_Text(pDX, IDC_SIZE2_EDIT, m_uiSize2);
	DDX_Check(pDX, IDC_SYSTEM_CHECK, m_iSystem);
	DDX_Check(pDX, IDC_EXCLUDEMASK_CHECK, m_bExclude);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterDlg,ictranslate::CLanguageDialog)
	//{{AFX_MSG_MAP(CFilterDlg)
	ON_BN_CLICKED(IDC_ATTRIBUTES_CHECK, OnAttributesCheck)
	ON_BN_CLICKED(IDC_DATE_CHECK, OnDateCheck)
	ON_BN_CLICKED(IDC_DATE2_CHECK, OnDate2Check)
	ON_BN_CLICKED(IDC_FILTER_CHECK, OnFilterCheck)
	ON_BN_CLICKED(IDC_SIZE_CHECK, OnSizeCheck)
	ON_BN_CLICKED(IDC_SIZE2_CHECK, OnSize2Check)
	ON_BN_CLICKED(IDC_EXCLUDEMASK_CHECK, OnExcludemaskCheck)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME1_DATETIMEPICKER, OnDatetimechangeTime1Datetimepicker)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATE1_DATETIMEPICKER, OnDatetimechangeDate1Datetimepicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg message handlers

BOOL CFilterDlg::OnInitDialog() 
{
	CLanguageDialog::OnInitDialog();

	// fill the combos with data
	const TCHAR *pszData;
	for (int i=0;i<3;i++)
	{
		pszData=GetResManager().LoadString(IDS_BYTE_STRING+i);
		m_ctlSize1Multi.AddString(pszData);
		m_ctlSize2Multi.AddString(pszData);
	}

	// strings <, <=, ...
	for (int i=0;i<5;i++)
	{
		pszData=GetResManager().LoadString(IDS_LT_STRING+i);
		m_ctlSizeType1.AddString(pszData);
		m_ctlSizeType2.AddString(pszData);
		m_ctlDateType1.AddString(pszData);
		m_ctlDateType2.AddString(pszData);
	}

	for (int i=0;i<3;i++)
	{
		m_ctlDateType.AddString(GetResManager().LoadString(IDS_DATECREATED_STRING+i));
	}

	// copy data from CFileFilter to a dialog - mask
	m_bFilter=m_ffFilter.m_bUseMask;

	CString strData;
	m_ctlFilter.SetCurSel(m_ctlFilter.AddString(m_ffFilter.GetCombinedMask(strData)));
	for (int i=0;i<m_astrAddMask.GetSize();i++)
		m_ctlFilter.AddString(m_astrAddMask.GetAt(i));

	m_bExclude=m_ffFilter.m_bUseExcludeMask;
	m_ctlExcludeMask.SetCurSel(m_ctlExcludeMask.AddString(m_ffFilter.GetCombinedExcludeMask(strData)));
	for (int i=0;i<m_astrAddExcludeMask.GetSize();i++)
		m_ctlExcludeMask.AddString(m_astrAddExcludeMask.GetAt(i));

	// size
	m_bSize=m_ffFilter.m_bUseSize;
	m_bSize2=m_ffFilter.m_bUseSize2;

	m_ctlSizeType1.SetCurSel(m_ffFilter.m_iSizeType1);
	m_ctlSizeType2.SetCurSel(m_ffFilter.m_iSizeType2);
	
	SetSize1(m_ffFilter.m_ullSize1);
	SetSize2(m_ffFilter.m_ullSize2);

	m_ctlSpin1.SetRange32(0, INT_MAX);
	m_ctlSpin2.SetRange32(0, INT_MAX);

	// date
	m_bDate=m_ffFilter.m_bUseDate;
	m_bDate2=m_ffFilter.m_bUseDate2;

	m_ctlDateType.SetCurSel(m_ffFilter.m_iDateType);

	m_ctlDateType1.SetCurSel(m_ffFilter.m_iDateType1);
	m_ctlDateType2.SetCurSel(m_ffFilter.m_iDateType2);

	SYSTEMTIME st;
	m_ffFilter.m_tDate1.GetAsSystemTime(st);
	m_ctlDate1.SendMessage(DTM_SETSYSTEMTIME, m_ffFilter.m_bDate1 ? (WPARAM)GDT_VALID : (WPARAM)GDT_NONE, (LPARAM)&st);
	m_ffFilter.m_tDate2.GetAsSystemTime(st);
	m_ctlDate2.SendMessage(DTM_SETSYSTEMTIME, m_ffFilter.m_bDate2 ? (WPARAM)GDT_VALID : (WPARAM)GDT_NONE, (LPARAM)&st);
	m_ffFilter.m_tTime1.GetAsSystemTime(st);
	m_ctlTime1.SendMessage(DTM_SETSYSTEMTIME, m_ffFilter.m_bTime1 ? (WPARAM)GDT_VALID : (WPARAM)GDT_NONE, (LPARAM)&st);
	m_ffFilter.m_tTime2.GetAsSystemTime(st);
	m_ctlTime2.SendMessage(DTM_SETSYSTEMTIME, m_ffFilter.m_bTime2 ? (WPARAM)GDT_VALID : (WPARAM)GDT_NONE, (LPARAM)&st);

	// attributes
	m_bAttributes=m_ffFilter.m_bUseAttributes;
	m_iArchive=m_ffFilter.m_iArchive;
	m_iReadOnly=m_ffFilter.m_iReadOnly;
	m_iHidden=m_ffFilter.m_iHidden;
	m_iSystem=m_ffFilter.m_iSystem;
	m_iDirectory=m_ffFilter.m_iDirectory;
	
	UpdateData(FALSE);

	EnableControls();

	return TRUE;
}

void CFilterDlg::OnLanguageChanged()
{
	// empty combos
	int iPos[4];
	iPos[0]=m_ctlSize1Multi.GetCurSel();
	iPos[1]=m_ctlSize2Multi.GetCurSel();
	m_ctlSize1Multi.ResetContent();
	m_ctlSize2Multi.ResetContent();

	// fill the combos with data
	const TCHAR *pszData;
	for (int i=0;i<3;i++)
	{
		pszData=GetResManager().LoadString(IDS_BYTE_STRING+i);
		m_ctlSize1Multi.AddString(pszData);
		m_ctlSize2Multi.AddString(pszData);
	}

	// selection
	m_ctlSize1Multi.SetCurSel(iPos[0]);
	m_ctlSize2Multi.SetCurSel(iPos[1]);

	iPos[0]=m_ctlSizeType1.GetCurSel();
	iPos[1]=m_ctlSizeType2.GetCurSel();
	iPos[2]=m_ctlDateType1.GetCurSel();
	iPos[3]=m_ctlDateType2.GetCurSel();

	m_ctlSizeType1.ResetContent();
	m_ctlSizeType2.ResetContent();
	m_ctlDateType1.ResetContent();
	m_ctlDateType2.ResetContent();

	// strings <, <=, ...
	for (int i=0;i<5;i++)
	{
		pszData=GetResManager().LoadString(IDS_LT_STRING+i);
		m_ctlSizeType1.AddString(pszData);
		m_ctlSizeType2.AddString(pszData);
		m_ctlDateType1.AddString(pszData);
		m_ctlDateType2.AddString(pszData);
	}

	m_ctlSizeType1.SetCurSel(iPos[0]);
	m_ctlSizeType2.SetCurSel(iPos[1]);
	m_ctlDateType1.SetCurSel(iPos[2]);
	m_ctlDateType2.SetCurSel(iPos[3]);

	iPos[0]=m_ctlDateType.GetCurSel();
	m_ctlDateType.ResetContent();
	for (int i=0;i<3;i++)
	{
		m_ctlDateType.AddString(GetResManager().LoadString(IDS_DATECREATED_STRING+i));
	}
	m_ctlDateType.SetCurSel(iPos[0]);
}

void CFilterDlg::SetSize1(unsigned __int64 ullSize)
{
	if ((ullSize % 1048576) == 0 && ullSize != 0)
	{
		m_uiSize1=static_cast<UINT>(ullSize/1048576);
		m_ctlSize1Multi.SetCurSel(2);
	}
	else if ((ullSize % 1024) == 0 && ullSize != 0)
	{
		m_uiSize1=static_cast<UINT>(ullSize/1024);
		m_ctlSize1Multi.SetCurSel(1);
	}
	else
	{
		m_uiSize1=static_cast<unsigned int>(ullSize);
		m_ctlSize1Multi.SetCurSel(0);
	}
}

void CFilterDlg::SetSize2(unsigned __int64 ullSize)
{
	if ((ullSize % 1048576) == 0 && ullSize != 0)
	{
		m_uiSize2=static_cast<UINT>(ullSize/1048576);
		m_ctlSize2Multi.SetCurSel(2);
	}
	else if ((ullSize % 1024) == 0 && ullSize != 0)
	{
		m_uiSize2=static_cast<UINT>(ullSize/1024);
		m_ctlSize2Multi.SetCurSel(1);
	}
	else
	{
		m_uiSize2=static_cast<unsigned int>(ullSize);
		m_ctlSize2Multi.SetCurSel(0);
	}
}

void CFilterDlg::EnableControls()
{
	UpdateData(TRUE);
	// mask
	m_ctlFilter.EnableWindow(m_bFilter);

	m_ctlExcludeMask.EnableWindow(m_bExclude);

	// size
	m_ctlSizeType1.EnableWindow(m_bSize);
	m_ctlSizeType2.EnableWindow(m_bSize && m_bSize2);
	GetDlgItem(IDC_SIZE1_EDIT)->EnableWindow(m_bSize);
	GetDlgItem(IDC_SIZE2_EDIT)->EnableWindow(m_bSize && m_bSize2);
	GetDlgItem(IDC_SIZE1_SPIN)->EnableWindow(m_bSize);
	GetDlgItem(IDC_SIZE2_SPIN)->EnableWindow(m_bSize && m_bSize2);
	GetDlgItem(IDC_SIZE2_CHECK)->EnableWindow(m_bSize);
	m_ctlSize1Multi.EnableWindow(m_bSize);
	m_ctlSize2Multi.EnableWindow(m_bSize && m_bSize2);

	// date
	CTime tmTemp;
	bool bSecond=((m_ctlDate1.GetTime(tmTemp) == GDT_VALID) || (m_ctlTime1.GetTime(tmTemp) == GDT_VALID));
	m_ctlDateType.EnableWindow(m_bDate);
	GetDlgItem(IDC_DATE2_CHECK)->EnableWindow(m_bDate && bSecond);
	m_ctlDateType1.EnableWindow(m_bDate);
	m_ctlDateType2.EnableWindow(m_bDate && m_bDate2 && bSecond);
	m_ctlDate1.EnableWindow(m_bDate);
	m_ctlDate2.EnableWindow(m_bDate && m_bDate2 && bSecond);
	m_ctlTime1.EnableWindow(m_bDate);
	m_ctlTime2.EnableWindow(m_bDate && m_bDate2 && bSecond);

	// attrib
	GetDlgItem(IDC_ARCHIVE_CHECK)->EnableWindow(m_bAttributes);
	GetDlgItem(IDC_READONLY_CHECK)->EnableWindow(m_bAttributes);
	GetDlgItem(IDC_HIDDEN_CHECK)->EnableWindow(m_bAttributes);
	GetDlgItem(IDC_SYSTEM_CHECK)->EnableWindow(m_bAttributes);
	GetDlgItem(IDC_DIRECTORY_CHECK)->EnableWindow(m_bAttributes);
}

void CFilterDlg::OnOK() 
{
	UpdateData(TRUE);
	
	// CFileFilter --> dialogu - mask
	CString strText;
	m_ctlFilter.GetWindowText(strText);
	m_ffFilter.m_bUseMask=((m_bFilter != 0) && !strText.IsEmpty());
	m_ffFilter.SetCombinedMask(strText);

	m_ctlExcludeMask.GetWindowText(strText);
	m_ffFilter.m_bUseExcludeMask=(m_bExclude != 0) && !strText.IsEmpty();
	m_ffFilter.SetCombinedExcludeMask(strText);
	
	// size
	m_ffFilter.m_bUseSize=(m_bSize != 0);
	m_ffFilter.m_bUseSize2=(m_bSize2 != 0);

	m_ffFilter.m_iSizeType1=m_ctlSizeType1.GetCurSel();
	m_ffFilter.m_iSizeType2=m_ctlSizeType2.GetCurSel();
	
	m_ffFilter.m_ullSize1=static_cast<unsigned __int64>(m_uiSize1)*static_cast<unsigned __int64>(GetMultiplier(m_ctlSize1Multi.GetCurSel()));
	m_ffFilter.m_ullSize2=static_cast<unsigned __int64>(m_uiSize2)*static_cast<unsigned __int64>(GetMultiplier(m_ctlSize2Multi.GetCurSel()));

	// date
	m_ffFilter.m_iDateType=m_ctlDateType.GetCurSel();

	m_ffFilter.m_iDateType1=m_ctlDateType1.GetCurSel();
	m_ffFilter.m_iDateType2=m_ctlDateType2.GetCurSel();

	m_ffFilter.m_bDate1=m_ctlDate1.GetTime(m_ffFilter.m_tDate1) == GDT_VALID;
	m_ffFilter.m_bDate2=m_ctlDate2.GetTime(m_ffFilter.m_tDate2) == GDT_VALID;
	m_ffFilter.m_bTime1=m_ctlTime1.GetTime(m_ffFilter.m_tTime1) == GDT_VALID;
	m_ffFilter.m_bTime2=m_ctlTime2.GetTime(m_ffFilter.m_tTime2) == GDT_VALID;

	m_ffFilter.m_bUseDate=(m_bDate != 0) && (m_ffFilter.m_bDate1 || m_ffFilter.m_bTime1);
	m_ffFilter.m_bUseDate2=(m_bDate2 != 0) && (m_ffFilter.m_bDate2 || m_ffFilter.m_bTime2);

	// attributes
	m_ffFilter.m_bUseAttributes=(m_bAttributes != 0);
	m_ffFilter.m_iArchive=m_iArchive;
	m_ffFilter.m_iReadOnly=m_iReadOnly;
	m_ffFilter.m_iHidden=m_iHidden;
	m_ffFilter.m_iSystem=m_iSystem;
	m_ffFilter.m_iDirectory=m_iDirectory;

	CLanguageDialog::OnOK();
}

int CFilterDlg::GetMultiplier(int iIndex)
{
	switch (iIndex)
	{
	case 0:
		return 1;
	case 1:
		return 1024;
	case 2:
		return 1048576;
	default:
		ASSERT(true);	// bad index
		return 1;
	}
}

void CFilterDlg::OnAttributesCheck() 
{
	EnableControls();
}

void CFilterDlg::OnDateCheck() 
{
	EnableControls();
}

void CFilterDlg::OnDate2Check() 
{
	EnableControls();
}

void CFilterDlg::OnFilterCheck() 
{
	EnableControls();
}

void CFilterDlg::OnSizeCheck() 
{
	EnableControls();
}

void CFilterDlg::OnSize2Check() 
{
	EnableControls();
}

void CFilterDlg::OnExcludemaskCheck() 
{
	EnableControls();
}

void CFilterDlg::OnDatetimechangeTime1Datetimepicker(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	EnableControls();
	
	*pResult = 0;
}

void CFilterDlg::OnDatetimechangeDate1Datetimepicker(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	EnableControls();
	
	*pResult = 0;
}
