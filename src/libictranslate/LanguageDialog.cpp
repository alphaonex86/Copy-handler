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
#include "LanguageDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_ICTRANSLATE_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplate
CDlgTemplate::CDlgTemplate(const DLGTEMPLATE* pDlgTemplate)
{
	Open(pDlgTemplate);
}

CDlgTemplate::CDlgTemplate(const DLGTEMPLATEEX* pDlgTemplate)
{
	Open((DLGTEMPLATE*)pDlgTemplate);
}

CDlgTemplate::~CDlgTemplate()
{
	delete [] m_pszMenu;
	delete [] m_pszClass;
	delete [] m_pszTitle;
	delete [] m_pszFace;

	// items
	vector<_ITEM>::iterator it;
	for (it=m_vItems.begin();it != m_vItems.end();it++)
	{
		delete [] (*it).m_pbyCreationData;
		delete [] (*it).m_pszClass;
		delete [] (*it).m_pszTitle;
	}
}

bool CDlgTemplate::Open(const DLGTEMPLATE* pDlgTemplate)
{
	if (pDlgTemplate == NULL)
		return false;
	bool bExt=((DLGTEMPLATEEX*)pDlgTemplate)->signature == 0xffff;
	const BYTE* pData=((BYTE*)pDlgTemplate);
	if (bExt)
	{
		m_dlgTemplate=*((DLGTEMPLATEEX*)pDlgTemplate);
		pData+=sizeof(DLGTEMPLATEEX);
	}
	else
	{
		ConvertDlgToEx(pDlgTemplate, &m_dlgTemplate);
		pData+=sizeof(DLGTEMPLATE);
	}
	
	// here is the menu, class and title
	pData=ReadCompoundData(pData, &m_wMenu, &m_pszMenu);
	pData=ReadCompoundData(pData, &m_wClass, &m_pszClass);
	pData=ReadCompoundData(pData, &m_wTitle, &m_pszTitle);

	// font
	if (m_dlgTemplate.style & DS_SETFONT || m_dlgTemplate.style & DS_SHELLFONT)
	{
		m_wFontSize=*((WORD*)pData);
		pData+=sizeof(WORD);
		if (bExt)
		{
			m_wWeight=*((WORD*)pData);
			pData+=sizeof(WORD);
			m_byItalic=*((BYTE*)pData);
			pData+=sizeof(BYTE);
			m_byCharset=*((BYTE*)pData);
			pData+=sizeof(BYTE);
		}
		else
		{
			m_wWeight=FW_NORMAL;
			m_byItalic=FALSE;
			m_byCharset=DEFAULT_CHARSET;
		}
		DWORD dwLen=(DWORD)wcslen((wchar_t*)pData);
		m_pszFace=new TCHAR[dwLen+1];
#ifdef _UNICODE
		_tcscpy(m_pszFace, (wchar_t*)pData);
#else
		WideCharToMultiByte(CP_ACP, 0, (wchar_t*)pData, dwLen+1, m_pszFace, dwLen+1, NULL, NULL);
#endif
		pData+=(dwLen+1)*sizeof(wchar_t);
	}
	else
	{
		m_wFontSize=0xffff;
		m_pszFace=NULL;
	}

	// items
	_ITEM item;
	for (int i=0;i<m_dlgTemplate.cDlgItems;i++)
	{
		// align to the nearest DWORD
		pData=(BYTE*)((LONG_PTR)(pData + 3) & ~3);

		// item
		if (bExt)
		{
			item.m_itemTemplate=*((DLGITEMTEMPLATEEX*)pData);
			pData+=sizeof(DLGITEMTEMPLATEEX);
		}
		else
		{
			ConvertItemToEx((DLGITEMTEMPLATE*)pData, &item.m_itemTemplate);
			pData+=sizeof(DLGITEMTEMPLATE);
		}

		pData=ReadCompoundData(pData, &item.m_wClass, &item.m_pszClass);
		if (item.m_wClass == 0xffff && item.m_pszClass != NULL)
		{
			// check the class strings and translate to atom values
			if (_tcscmp(item.m_pszClass, _T("STATIC")) == 0)
			{
				item.m_wClass=0x0082;
				delete [] item.m_pszClass;
				item.m_pszClass=NULL;
			}
			else if (_tcscmp(item.m_pszClass, _T("BUTTON")) == 0)
			{
				item.m_wClass=0x0080;
				delete [] item.m_pszClass;
				item.m_pszClass=NULL;
			}
			else if (_tcscmp(item.m_pszClass, _T("EDIT")) == 0)
			{
				item.m_wClass=0x0081;
				delete [] item.m_pszClass;
				item.m_pszClass=NULL;
			}
			else if (_tcscmp(item.m_pszClass, _T("COMBOBOX")) == 0)
			{
				item.m_wClass=0x0085;
				delete [] item.m_pszClass;
				item.m_pszClass=NULL;
			}
			else if (_tcscmp(item.m_pszClass, _T("LISTBOX")) == 0)
			{
				item.m_wClass=0x0083;
				delete [] item.m_pszClass;
				item.m_pszClass=NULL;
			}
			else if (_tcscmp(item.m_pszClass, _T("SCROLLBAR")) == 0)
			{
				item.m_wClass=0x0084;
				delete [] item.m_pszClass;
				item.m_pszClass=NULL;
			}
		}
		pData=ReadCompoundData(pData, &item.m_wTitle, &item.m_pszTitle);

		// creation data
		item.m_wCreationDataSize=(*((WORD*)pData));
		pData+=sizeof(WORD);
		if (item.m_wCreationDataSize > 0)
		{
			item.m_wCreationDataSize-=sizeof(WORD);
			item.m_pbyCreationData=new BYTE[item.m_wCreationDataSize];
			memcpy(item.m_pbyCreationData, pData, item.m_wCreationDataSize);
		}
		else
			item.m_pbyCreationData=NULL;

		m_vItems.push_back(item);
	}

	return true;
}

void CDlgTemplate::ConvertItemToEx(const DLGITEMTEMPLATE* pSrc, DLGITEMTEMPLATEEX* pDst)
{
	pDst->helpID=0;
	pDst->exStyle=pSrc->dwExtendedStyle;
	pDst->style=pSrc->style;
	pDst->x=pSrc->x;
	pDst->y=pSrc->y;
	pDst->cx=pSrc->cx;
	pDst->cy=pSrc->cy;
	pDst->id=pSrc->id;
}

void CDlgTemplate::ConvertDlgToEx(const DLGTEMPLATE* pSrc, DLGTEMPLATEEX* pDst)
{
	pDst->dlgVer=1;
	pDst->signature=0x0000;
	pDst->helpID=(ULONG)0;
	pDst->exStyle=pSrc->dwExtendedStyle;
	pDst->style=pSrc->style;
	pDst->cDlgItems=pSrc->cdit;
	pDst->x=pSrc->x;
	pDst->y=pSrc->y;
	pDst->cx=pSrc->cx;
	pDst->cy=pSrc->cy;
}

const BYTE* CDlgTemplate::ReadCompoundData(const BYTE* pBuffer, WORD* pwData, PTSTR* ppszStr)
{
	if (*((WORD*)pBuffer) == 0xffff)
	{
		*pwData=*((WORD*)(pBuffer+2));
		*ppszStr=NULL;

		return pBuffer+4;
	}
	else
	{
		*pwData=0xffff;
		DWORD dwLen=(DWORD)wcslen((wchar_t*)pBuffer);
		*ppszStr=new TCHAR[dwLen+1];
#ifdef _UNICODE
		_tcscpy(*ppszStr, (wchar_t*)pBuffer);
#else
		WideCharToMultiByte(CP_ACP, 0, (wchar_t*)pBuffer, dwLen+1, *ppszStr, dwLen+1, NULL, NULL);
#endif
		return pBuffer+(dwLen+1)*sizeof(wchar_t);
	}

}

/////////////////////////////////////////////////////////////////////////////
// CControlResizeInfo dialog

// ============================================================================
/// ictranslate::CControlResizeInfo::CControlResizeInfo
/// @date 2009/04/18
///
/// @brief     Constructs the control resize information object.
/// @param[in] iCtrlID	      Control ID.
/// @param[in] dXPosFactor	  Multiplier for the dialog width change - used for x-offset.
/// @param[in] dYPosFactor    Multiplier for the dialog height change - used for y-offset.
/// @param[in] dXScaleFactor  Multiplier for the dialog width change - used for x-size.
/// @param[in] dYScaleFactor  Multiplier for the dialog height change - used for y-size.
// ============================================================================
CControlResizeInfo::CControlResizeInfo(int iCtrlID, double dXPosFactor, double dYPosFactor, double dXScaleFactor, double dYScaleFactor) :
	m_iControlID(iCtrlID),
	m_rcInitialPosition(0, 0, 0, 0),
	m_dXOffsetFactor(dXPosFactor),
	m_dYOffsetFactor(dYPosFactor),
	m_dXScaleFactor(dXScaleFactor),
	m_dYScaleFactor(dYScaleFactor)
{
}

// ============================================================================
/// ictranslate::CControlResizeInfo::SetInitialPosition
/// @date 2009/04/18
///
/// @brief     Sets the initial position of control.
/// @param[in] rcPos    Control position.
// ============================================================================
void CControlResizeInfo::SetInitialPosition(const CRect& rcPos)
{
	m_rcInitialPosition = rcPos;
}

// ============================================================================
/// ictranslate::CControlResizeInfo::SetFactors
/// @date 2009/04/18
///
/// @brief     Sets the offset and scaling factors for future placement calculations.
/// @param[in] dXPosFactor	  Multiplier for the dialog width change - used for x-offset.
/// @param[in] dYPosFactor    Multiplier for the dialog height change - used for y-offset.
/// @param[in] dXScaleFactor  Multiplier for the dialog width change - used for x-size.
/// @param[in] dYScaleFactor  Multiplier for the dialog height change - used for y-size.
// ============================================================================
/*
void CControlResizeInfo::SetFactors(double dXPosFactor, double dYPosFactor, double dXScaleFactor, double dYScaleFactor)
{
	m_dXOffsetFactor = dXPosFactor;
	m_dYOffsetFactor = dYPosFactor;
	m_dXScaleFactor = dXScaleFactor;
	m_dYScaleFactor = dYScaleFactor;
}*/


// ============================================================================
/// ictranslate::CControlResizeInfo::GetNewControlPlacement
/// @date 2009/04/18
///
/// @brief     Calculates new control position based on dialog positions and placement factors.
/// @param[in]  rcDlgInitial    Initial dialog size.
/// @param[in]  rcDlgCurrent    Current dialog size.
/// @param[out] rcNewPlacement   Receives the new control position and size.
// ============================================================================
void CControlResizeInfo::GetNewControlPlacement(const CRect& rcDlgInitial, const CRect& rcDlgCurrent, CRect& rcNewPlacement)
{
	long stXOffset = rcDlgCurrent.Width() - rcDlgInitial.Width();
	long stYOffset = rcDlgCurrent.Height() - rcDlgInitial.Height();

	rcNewPlacement.left = m_rcInitialPosition.left + (long)(m_dXOffsetFactor * stXOffset);
	rcNewPlacement.top = m_rcInitialPosition.top + (long)(m_dYOffsetFactor * stYOffset);

	rcNewPlacement.right = rcNewPlacement.left + m_rcInitialPosition.Width() + (long)(m_dXScaleFactor * stXOffset);
	rcNewPlacement.bottom = rcNewPlacement.top + m_rcInitialPosition.Height() + (long)(m_dYScaleFactor * stYOffset);
}

// ============================================================================
/// ictranslate::CControlResizeInfo::ResetInitState
/// @date 2009/04/18
///
/// @brief     Resets the state of this control information object (resets the initial position).
// ============================================================================
void CControlResizeInfo::ResetInitState()
{
	m_rcInitialPosition.left = 0;
	m_rcInitialPosition.top = 0;
	m_rcInitialPosition.right = 0;
	m_rcInitialPosition.bottom = 0;
}

// ============================================================================
/// ictranslate::CControlResizeInfo::IsInitialized
/// @date 2009/04/18
///
/// @brief     Checks if control information object has been initialized with initial control position.
/// @return    True if initial position has been set, false otherwise.
// ============================================================================
bool CControlResizeInfo::IsInitialized() const
{
	return m_rcInitialPosition.Width() != 0 && m_rcInitialPosition.Height() != 0;
}

/////////////////////////////////////////////////////////////////////////////
// CLanguageDialog dialog

BEGIN_MESSAGE_MAP(CLanguageDialog, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDHELP, OnHelpButton)
END_MESSAGE_MAP()


CResourceManager *CLanguageDialog::m_prm=NULL;

///////////////////////////////////////////////////////////////
// Standard constructor
// pLock [in] - specifies address of a bool value that'll be
//		used to check if another instance of this window has
//		already been shown. Should be declared in derived class
//		as 'static bool m_xxx;' and initialized to 0.
///////////////////////////////////////////////////////////////
CLanguageDialog::CLanguageDialog(bool* pLock) : CDialog()
{
	m_pszResName=NULL;
	m_uiResID=0;
	m_pParent=NULL;
	m_cType=-1;
	m_bAutoDelete=false;
	m_pFont=NULL;
	m_pbLock=pLock;
	m_bLockChanged=false;
	m_bLockInstance=false;
	m_iBaseX=m_iBaseY=0;
	_ASSERT(m_prm);			// make sure the CLanguageDialog::SetResManager() has been called aready
	if(m_prm)
		m_prm->m_lhDialogs.push_back(this);
}

///////////////////////////////////////////////////////////////
// Constructor that takes string based template name
// lpszTemplateName [in] - specifies the template name to load
//		and show as this dialog.
// pParent [in] - logical (everyone knows)
// pLock [in] - address of a bool for dialog instance checks
///////////////////////////////////////////////////////////////
CLanguageDialog::CLanguageDialog(PCTSTR lpszTemplateName, CWnd* pParent, bool* pLock) : CDialog()
{
	m_pszResName=lpszTemplateName;
	if (IS_INTRESOURCE(lpszTemplateName))
		m_uiResID=(WORD)lpszTemplateName;
	else
		m_uiResID=0;
	m_pParent=pParent;
	m_cType=-1;
	m_bAutoDelete=false;
	m_pFont=NULL;
	m_pbLock=pLock;
	m_bLockChanged=false;
	m_bLockInstance=false;
	m_iBaseX=m_iBaseY=0;
	_ASSERT(m_prm);			// make sure the CLanguageDialog::SetResManager() has been called aready
	if(m_prm)
		m_prm->m_lhDialogs.push_back(this);
}

///////////////////////////////////////////////////////////////
// Constructor that takes UINT based template name
// uiIDTemplate [in] - specifies the template ID to load
//		and show as this dialog.
// pParent [in] - logical (everyone knows)
// pLock [in] - address of a bool for dialog instance checks
///////////////////////////////////////////////////////////////
CLanguageDialog::CLanguageDialog(UINT uiIDTemplate, CWnd* pParent, bool* pLock) : CDialog()
{
	m_pszResName=MAKEINTRESOURCE(uiIDTemplate);
	m_uiResID=uiIDTemplate;
	m_pParent=pParent;
	m_cType=-1;
	m_bAutoDelete=false;
	m_pFont=NULL;
	m_pbLock=pLock;
	m_bLockChanged=false;
	m_bLockInstance=false;
	m_iBaseX=m_iBaseY=0;
	_ASSERT(m_prm);			// make sure the CLanguageDialog::SetResManager() has been called aready
	if(m_prm)
		m_prm->m_lhDialogs.push_back(this);
}

///////////////////////////////////////////////////////////////
// Standard destructor
// Removes itself from a list in CWinApp (not to get any window
// messages anymore).
///////////////////////////////////////////////////////////////
CLanguageDialog::~CLanguageDialog()
{
	list<CWnd*>::iterator it=m_prm->m_lhDialogs.begin();
	while (it != m_prm->m_lhDialogs.end())
	{
		if (*it == this)
		{
			m_prm->m_lhDialogs.erase(it);
			break;
		}

		it++;
	}

	delete m_pFont;
}

/////////////////////////////////////////////////////////////////////////////
// CLanguageDialog message handlers

///////////////////////////////////////////////////////////////
// Makes properly constructed dialog modal.
// RetVal [out] - value returned by dialog proc
///////////////////////////////////////////////////////////////
INT_PTR CLanguageDialog::DoModal() 
{
	if (m_pszResName)
	{
		HGLOBAL hDialog=m_prm->LoadResource(RT_DIALOG, m_pszResName);
		if (!InitModalIndirect(hDialog))
			return -1;
	}
	m_cType=0;
	return CDialog::DoModal();
}

///////////////////////////////////////////////////////////////
// Creates (and shows probably) this constructed dialog.
// RetVal [out] - if creation succeeded
///////////////////////////////////////////////////////////////
BOOL CLanguageDialog::Create() 
{
	_ASSERT(m_pszResName);		// nothing was set as a dialog template
    
	if (!m_bLockInstance || m_pbLock == NULL || !(*m_pbLock))
	{
		HGLOBAL hDialog=m_prm->LoadResource(RT_DIALOG, m_pszResName);

		// modeless dialog
		if (!CreateIndirect(hDialog, m_pParent))
			return FALSE;

		m_cType=1;
		if (m_pbLock)
		{
			*m_pbLock=true;
			m_bLockChanged=true;
		}

		return TRUE;
	}
	else
	{
		m_bLockChanged=false;
		Cleanup();
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////
// Changes values based on dialog units into the values in
// pixels. Change is based on std MapDialogRect if the language
// hasn't been changed otf or takes current font into
// consideration.
// pRect [in/out] - on [in] - dialog units, on [out] - pixels
///////////////////////////////////////////////////////////////
void CLanguageDialog::MapRect(RECT* pRect)
{
	if (m_pFont)
	{
		pRect->left=MulDiv(pRect->left, m_iBaseX, 4);
		pRect->right=MulDiv(pRect->right, m_iBaseX, 4);
		pRect->top=MulDiv(pRect->top, m_iBaseY, 8);
		pRect->bottom=MulDiv(pRect->bottom, m_iBaseY, 8);
	}
	else
		MapDialogRect(pRect);
}

///////////////////////////////////////////////////////////////
// Helper function - called when this dialog receives message
// WM_RMNOTIFY (with WPARAM == RMNT_LANGCHANGE). Updates the
// dialog with data from a new template. Passes params to
// virtual function OnLanguageChanged.
// wOldLang [in] - specifies the old language code
// wNewLang [in] - specifies the new language code.
///////////////////////////////////////////////////////////////
void CLanguageDialog::UpdateLanguage()
{
	// cannot update for string based template
	if (m_uiResID == 0)
		return;

	// set the title
	SetWindowText(m_prm->LoadString((WORD)m_uiResID, 0));

	// load the dialog template
	CDlgTemplate dt;
	if (!dt.Open(m_prm->LoadDialog(MAKEINTRESOURCE(m_uiResID))))
	{
		TRACE("Cannot open dialog template in UpdateLanguage\n");
		return;
	}

	// update the menu
	if (GetMenu())
		m_prm->UpdateMenu(GetMenu()->m_hMenu, dt.m_wMenu);

	// font
	if (!(GetLanguageUpdateOptions() & LDF_NODIALOGFONT))
	{
		// dialog font
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		HDC hdc=::GetDC(NULL);
		lf.lfHeight = -MulDiv(m_prm->m_ld.GetPointSize(), GetDeviceCaps(hdc, LOGPIXELSY), 72);
		::ReleaseDC(NULL, hdc);
		lf.lfWeight = FW_NORMAL;
		lf.lfCharSet = DEFAULT_CHARSET;
		_tcscpy(lf.lfFaceName, m_prm->m_ld.GetFontFace());
		
		delete m_pFont;
		m_pFont=new CFont();
		m_pFont->CreateFontIndirect(&lf);

		// change base dlg units
		CalcBaseUnits(dt.m_pszFace, dt.m_wFontSize);
	}

	if (!(GetLanguageUpdateOptions() & LDF_NODIALOGSIZE))
	{
		// dialog size
		CRect rcWin;
		GetWindowRect(&rcWin);
		
		CRect rcDialog(0, 0, dt.m_dlgTemplate.cx, dt.m_dlgTemplate.cy);
		MapRect(&rcDialog);

		rcDialog.bottom += 2 * GetSystemMetrics((dt.m_dlgTemplate.style & WS_THICKFRAME) ? SM_CYSIZEFRAME : SM_CYFIXEDFRAME) + GetSystemMetrics((dt.m_dlgTemplate.exStyle & WS_EX_TOOLWINDOW) ? SM_CYCAPTION : SM_CYSMCAPTION);
		rcDialog.right += 2 * GetSystemMetrics((dt.m_dlgTemplate.style & WS_THICKFRAME) ? SM_CXSIZEFRAME : SM_CXFIXEDFRAME);

		// correct the height by a menu height
		if ((dt.m_wMenu != 0xffff) || ((dt.m_pszMenu != NULL) && _tcslen(dt.m_pszMenu) != 0))
			rcDialog.bottom += GetSystemMetrics(SM_CYMENU);

		rcDialog.OffsetRect(rcWin.CenterPoint().x-rcDialog.Width()/2, rcWin.CenterPoint().y-rcDialog.Height()/2);
		
		//TEMP
		TRACE("Old dlg pos/size: x=%lu, y=%lu, cx=%lu, cy=%lu; \n\tNew dlg pos/size: x=%lu, y=%lu, cx=%lu, cy=%lu\n", rcWin.left, rcWin.top, rcWin.Width(), rcWin.Height(), rcDialog.left, rcDialog.top, rcDialog.Width(), rcDialog.Height());
		SetWindowPos(NULL, rcDialog.left, rcDialog.top, rcDialog.Width(), rcDialog.Height(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	}
	
	// the controls
	CWnd* pWnd;
	vector<CDlgTemplate::_ITEM>::iterator it;
	for (it=dt.m_vItems.begin();it != dt.m_vItems.end();it++)
	{
		// skip controls that cannot be modified
		if ( (*it).m_itemTemplate.id == 0xffff || (pWnd=GetDlgItem((*it).m_itemTemplate.id)) == NULL)
			continue;
		
		// the font
		if (!(GetLanguageUpdateOptions() & LDF_NODIALOGFONT))
			pWnd->SetFont(m_pFont, FALSE);
		
		// style&ex style
		// modify only the rtl/ltr reading order
		LONG lStyleEx=::GetWindowLong(pWnd->m_hWnd, GWL_EXSTYLE);
		if (lStyleEx & WS_EX_RTLREADING)
		{
			if (!m_prm->m_ld.GetDirection())
				lStyleEx &= ~WS_EX_RTLREADING;
		}
		else
		{
			if (m_prm->m_ld.GetDirection())
				lStyleEx |= WS_EX_RTLREADING;
		}
		
		::SetWindowLongPtr(pWnd->m_hWnd, GWL_EXSTYLE, lStyleEx);
		
		// size
		CRect rc((*it).m_itemTemplate.x, (*it).m_itemTemplate.y, (*it).m_itemTemplate.x+(*it).m_itemTemplate.cx, (*it).m_itemTemplate.y+(*it).m_itemTemplate.cy);
		MapRect(&rc);
		pWnd->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
		
		// text/caption
		if ( (*it).m_wClass == 0x0080 || (*it).m_wClass == 0x0082 || (*it).m_wClass == 0x0086 || ((*it).m_pszClass != NULL && _tcscmp((*it).m_pszClass, _T("STATICEX")) == 0) )
			pWnd->SetWindowText(m_prm->LoadString((WORD)m_uiResID, (*it).m_itemTemplate.id));
	}

	// ensure that we have current placement of dialog and controls remembered to allow safe repositioning later
	InitializeResizableControls();
}

///////////////////////////////////////////////////////////////
// Helper function - does the cleanup after destroying the
// dialog (that means releasing the instance lock, deleting
// unused fonts and in some cases deleting itself).
///////////////////////////////////////////////////////////////
void CLanguageDialog::Cleanup()
{
	TRACE("CLanguageDialog::Cleanup()\n");

	if (m_bLockChanged && m_pbLock)
		*m_pbLock=false;

	delete m_pFont;
	m_pFont = NULL;

	if(m_bAutoDelete)
		delete this;
}

///////////////////////////////////////////////////////////////
// Standard msg - initializes tool tip handling
///////////////////////////////////////////////////////////////
BOOL CLanguageDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateLanguage();		// because initially all the texts are empty

	EnableToolTips(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////
// Standard handler for pressing cancel button in a dialog.
// For modeless dialog causes dialog to be destroyed.
///////////////////////////////////////////////////////////////
void CLanguageDialog::OnCancel() 
{
	switch (m_cType)
	{
	case 0:
		CDialog::OnCancel();
		break;
	case 1:
		DestroyWindow();
		break;
	}
}

///////////////////////////////////////////////////////////////
// Standard handler for pressing OK button in a dialog.
// For modeless dialog causes destruction of a dialog.
///////////////////////////////////////////////////////////////
void CLanguageDialog::OnOK()
{
	switch(m_cType)
	{
	case 0:
		CDialog::OnOK();
		break;
	case 1:
		DestroyWindow();
		break;
	}
}

///////////////////////////////////////////////////////////////
// Standard override - calls cleanup.
///////////////////////////////////////////////////////////////
void CLanguageDialog::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	Cleanup();
}

///////////////////////////////////////////////////////////////
// This dialog's window procedure handler - look at ms docs.
///////////////////////////////////////////////////////////////
LRESULT CLanguageDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_RMNOTIFY:
		if ((UINT)wParam == RMNT_LANGCHANGE)
		{
			UpdateLanguage();

			// now update user controls (everybody has to do it him(her)self)
			OnLanguageChanged();
			break;
		}
	case WM_NOTIFY:
		{
			NMHDR* pnmh=(NMHDR*)lParam;
			if (pnmh->code == TTN_NEEDTEXT)
			{
				// establish the ID of a control
				TOOLTIPTEXT *ppt=(TOOLTIPTEXT*)pnmh;
				UINT nID;
				if (ppt->uFlags & TTF_IDISHWND)
					nID=(UINT)::GetDlgCtrlID((HWND)pnmh->idFrom);
				else
					nID=(UINT)pnmh->idFrom;

				return OnTooltipText(nID, ppt);
			}
			break;
		}
	case WM_SIZE:
		{
			RepositionResizableControls();
			break;
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

///////////////////////////////////////////////////////////////
// Helper function. Recalculates current sizes of a dialog base
// units (font dependent) and stores in the internal members.
// pszFacename [in] - font's face name.
// wPointSize [in] - size of the font in points.
///////////////////////////////////////////////////////////////
void CLanguageDialog::CalcBaseUnits(PCTSTR pszFacename, WORD wPointSize)
{
	LOGFONT lf;
	HDC hDC = ::GetDC(NULL);
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(wPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = DEFAULT_CHARSET;
	lstrcpy(lf.lfFaceName, pszFacename);

	HFONT hNewFont = CreateFontIndirect(&lf);
	if (hNewFont != NULL)
	{
		HFONT hFontOld = (HFONT)SelectObject(hDC, hNewFont);
		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);
		m_iBaseY = tm.tmHeight + tm.tmExternalLeading;
		SIZE size;
		::GetTextExtentPoint32(hDC, _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"), 52,	&size);
		m_iBaseX = (size.cx + 26) / 52;
		SelectObject(hDC, hFontOld);
		DeleteObject(hNewFont);
	}
	else
	{
		// Could not create the font so just use the system's values
		m_iBaseX = LOWORD(GetDialogBaseUnits());
		m_iBaseY = HIWORD(GetDialogBaseUnits());
	}
	::ReleaseDC(NULL, hDC);
}

BOOL CLanguageDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
	{
		pHelpInfo->dwContextId=(m_uiResID << 16) | pHelpInfo->iCtrlId;
		AfxGetApp()->HtmlHelp((DWORD_PTR)pHelpInfo, HH_DISPLAY_TEXT_POPUP);
		return true;
	}
	else
		return false;
}

void CLanguageDialog::OnContextMenu(CWnd* pWnd, CPoint point)
{
	HELPINFO hi;
	hi.cbSize=sizeof(HELPINFO);
	hi.iCtrlId=pWnd->GetDlgCtrlID();
	hi.dwContextId=(m_uiResID << 16) | hi.iCtrlId;
	hi.hItemHandle=pWnd->m_hWnd;
	hi.iContextType=HELPINFO_WINDOW;
	hi.MousePos=point;

	AfxGetApp()->HtmlHelp((DWORD_PTR)&hi, HH_DISPLAY_TEXT_POPUP);
}

void CLanguageDialog::OnHelpButton() 
{
	AfxGetApp()->HtmlHelp(m_uiResID+0x20000, HH_HELP_CONTEXT);
}

// ============================================================================
/// ictranslate::CLanguageDialog::InitializeResizableControls
/// @date 2009/04/18
///
/// @brief     Initializes resizable controls using current dialog as a base.
// ============================================================================
void CLanguageDialog::InitializeResizableControls()
{
	// remember current dialog position as the base one
	GetClientRect(&m_rcDialogInitialPosition);

	// enum through the controls to get their current positions
	CWnd* pWnd = NULL;
	CRect rcControl;

	std::map<int, CControlResizeInfo>::iterator iterControl = m_mapResizeInfo.begin();
	while(iterControl != m_mapResizeInfo.end())
	{
		// get the control if exists
		pWnd = GetDlgItem((*iterControl).second.GetControlID());
		if(!(*iterControl).second.IsInitialized())
		{
			if(pWnd && ::IsWindow(pWnd->m_hWnd))
			{
				pWnd->GetWindowRect(&rcControl);
				ScreenToClient(&rcControl);

				(*iterControl).second.SetInitialPosition(rcControl);
			}
			else
				(*iterControl).second.ResetInitState();
		}
		else if(!pWnd)
			(*iterControl).second.ResetInitState();		// mark control as uninitialized when it does not exist

		++iterControl;
	}
}

// ============================================================================
/// ictranslate::CLanguageDialog::ClearResizableControls
/// @date 2009/04/18
///
/// @brief     Clears the list of resizable controls.
// ============================================================================
void CLanguageDialog::ClearResizableControls()
{
	m_mapResizeInfo.clear();
}

// ============================================================================
/// ictranslate::CLanguageDialog::AddResizableControl
/// @date 2009/04/18
///
/// @brief     Adds the control to the list of resizable controls.
/// @param[in] iCtrlID        Control ID.
/// @param[in] dXPosFactor    X position multiply factor.
/// @param[in] dYPosFactor    Y position multiply factor.
/// @param[in] dXScaleFactor  X size multiply factor.
/// @param[in] dYScaleFactor  y size multiply factor.
/// @return    
// ============================================================================
void CLanguageDialog::AddResizableControl(int iCtrlID, double dXPosFactor, double dYPosFactor, double dXScaleFactor, double dYScaleFactor)
{
	CControlResizeInfo cri(iCtrlID, dXPosFactor, dYPosFactor, dXScaleFactor, dYScaleFactor);
	
	// read position if possible
	CRect rcControl;
	CWnd* pWnd = GetDlgItem(iCtrlID);
	if(pWnd && ::IsWindow(pWnd->m_hWnd))
	{
		pWnd->GetWindowRect(&rcControl);
		ScreenToClient(&rcControl);

		cri.SetInitialPosition(rcControl);
	}

	m_mapResizeInfo.insert(std::make_pair(iCtrlID, cri));
}

// ============================================================================
/// ictranslate::CLanguageDialog::RepositionResizableControls
/// @date 2009/04/18
///
/// @brief     Repositions all resizable controls according to the size change.
// ============================================================================
void CLanguageDialog::RepositionResizableControls()
{
	// retrieve current dialog size
	CRect rcControl;
	CWnd* pWnd = NULL;

	CRect rcCurrentDlgPos;
	GetClientRect(&rcCurrentDlgPos);

	bool bRepositioned = false;

	std::map<int, CControlResizeInfo>::iterator iterControl = m_mapResizeInfo.begin();
	while(iterControl != m_mapResizeInfo.end())
	{
		if((*iterControl).second.IsInitialized())
		{
			pWnd = GetDlgItem((*iterControl).second.GetControlID());
			if(pWnd && ::IsWindow(pWnd->m_hWnd))
			{
				(*iterControl).second.GetNewControlPlacement(m_rcDialogInitialPosition, rcCurrentDlgPos, rcControl);
				pWnd->SetWindowPos(NULL, rcControl.left, rcControl.top, rcControl.Width(), rcControl.Height(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
				bRepositioned = true;
			}
		}

		++iterControl;
	}

/*
	if(bRepositioned)
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);*/

}

END_ICTRANSLATE_NAMESPACE
