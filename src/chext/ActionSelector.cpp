// ============================================================================
//   Copyright (C) 2001-2008 by Jozef Starosczyk
//   ixen@copyhandler.com
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License
//   (version 2) as published by the Free Software Foundation;
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public
//   License along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ============================================================================
/// @file ActionSelector.cpp
/// @brief Implementation of TActionSelector.
// ============================================================================
#include "stdafx.h"
#include "ActionSelector.h"

// ============================================================================
/// TActionSelector::TActionSelector
/// @date 2009/03/12
///
/// @brief     Constructs the action selector object.
// ============================================================================
TActionSelector::TActionSelector() :
	m_ulKeysState(eKey_None),
	m_ulPreferredDropEffect(0),
	m_eDefaultOperation(eAction_None),
	m_eDefaultMenuItem(eAction_None),
	m_ulActionOptions(eOption_None)
{
}

// ============================================================================
/// TActionSelector::~TActionSelector
/// @date 2009/03/12
///
/// @brief     Destructs the action selection object.
// ============================================================================
TActionSelector::~TActionSelector()
{
}

// ============================================================================
/// TActionSelector::ResetState
/// @date 2009/03/12
///
/// @brief     Resets the state contained in this class.
// ============================================================================
void TActionSelector::ResetState()
{
	m_ulKeysState = eKey_None;
	m_ulPreferredDropEffect = 0;
	m_eDefaultOperation = eAction_None;
	m_eDefaultMenuItem = eAction_None;
	m_ulActionOptions = eOption_None;
}

// ============================================================================
/// TActionSelector::ReadKeysState
/// @date 2009/03/12
///
/// @brief     Retrieves the keyboard state at a time of call.
// ============================================================================
void TActionSelector::ReadKeyboardState()
{
	m_ulKeysState = ((GetKeyState(VK_SHIFT) & 0x80) ? eKey_Shift : 0) |
		((GetKeyState(VK_CONTROL) & 0x80) ? eKey_Ctrl : 0) |
		((GetKeyState(VK_MENU) & 0x80) ? eKey_Alt : 0);
}

// ============================================================================
/// TActionSelector::ReadStateFromDataObject
/// @date 2009/03/12
///
/// @brief     Retrieves the state from data object.
/// @param[in] piDataObject			Data object to retrieve the information from.
/// @param[in] pszDestinationPath   Destination path for the operation (used with simple heuristics).
/// @return    Result of the operation.
// ============================================================================
HRESULT TActionSelector::ReadStateFromDataObject(IDataObject* piDataObject, PCTSTR pszDestinationPath)
{
	if(!piDataObject || !pszDestinationPath || pszDestinationPath[0] == _T('\0'))
		return E_INVALIDARG;

	// try to retrieve the preferred drop effect from the input data object
	UINT uiPreferredDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	if(!uiPreferredDropEffect)
		return E_FAIL;

	FORMATETC fe = { (CLIPFORMAT)uiPreferredDropEffect, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	STGMEDIUM medium;

	// if the drop effect does not exist - just report it
	m_ulPreferredDropEffect = 0;
	HRESULT hResult = piDataObject->QueryGetData(&fe);
	if(hResult == S_OK)
	{
		hResult = piDataObject->GetData(&fe, &medium);
		if(SUCCEEDED(hResult) && !medium.hGlobal)
		{
			ReleaseStgMedium(&medium);
			hResult = E_FAIL;
		}
		if(SUCCEEDED(hResult))
		{
			// specify operation
			DWORD* pdwData = (DWORD*)GlobalLock(medium.hGlobal);
			if(pdwData)
			{
				m_ulPreferredDropEffect = *pdwData;
				GlobalUnlock(medium.hGlobal);
			}
			else
				hResult = E_FAIL;
			ReleaseStgMedium(&medium);
		}
	}

	// now try to get into the CF_HDROP information and use a simple heuristic to determine operation
	m_eDefaultOperation = eAction_None;
	fe.cfFormat = CF_HDROP;
	
	// are the data available?
	hResult = piDataObject->QueryGetData(&fe);
	if(hResult != S_OK)
		return S_FALSE;

	hResult = piDataObject->GetData(&fe, &medium);
	if(SUCCEEDED(hResult) && !medium.hGlobal)
	{
		ReleaseStgMedium(&medium);
		hResult = E_FAIL;
	}
	if(SUCCEEDED(hResult))
	{
		// copy is the default
		m_eDefaultOperation = eAction_Copy;

		// autodetecting - copy or move - check the last path
		UINT uiCount = DragQueryFile((HDROP)medium.hGlobal, 0xffffffff, NULL, 0);
		TCHAR szPath[_MAX_PATH];
		if(DragQueryFile((HDROP)medium.hGlobal, uiCount - 1, szPath, _MAX_PATH))
		{
			// check if the path has some common elements with destination paths
			if(_tcsncmp(szPath, _T("\\\\"), 2) == 0)
			{
				TCHAR* pFnd = _tcsstr(szPath+2, _T("\\"));
				if(pFnd)
				{
					size_t stCount;
					// find another
					TCHAR *pSecond = _tcsstr(pFnd + 1, _T("\\"));
					if(pSecond)
						stCount = pSecond - szPath;
					else
						stCount = _tcslen(szPath);

					// found - compare
					if(_tcsnicmp(szPath, pszDestinationPath, stCount) == 0)
						m_eDefaultOperation = eAction_Move;
				}
			}
			else if(pszDestinationPath[0] == szPath[0])	// when processing within the same disk drive - we move by default
				m_eDefaultOperation = eAction_Move;
		}

		ReleaseStgMedium(&medium);
	}

	return hResult;
}

// ============================================================================
/// TActionSelector::ReadStateFromMenu
/// @date 2009/03/12
///
/// @brief     Reads the state from menu handle.
/// @param[in] hMenu   Handle to a menu containing basic items.
// ============================================================================
void TActionSelector::ReadStateFromMenu(HMENU hMenu)
{
	// it's none by default
	m_eDefaultMenuItem = eAction_None;

	_ASSERTE(hMenu != NULL);
	if(!hMenu)
		return;

	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;

	if(::GetMenuItemInfo(hMenu, 1, FALSE, &mii) && mii.fState & MFS_DEFAULT)
		m_eDefaultMenuItem = eAction_Copy;
	if(::GetMenuItemInfo(hMenu, 2, FALSE, &mii) && mii.fState & MFS_DEFAULT)
		m_eDefaultMenuItem = eAction_Move;
	if(::GetMenuItemInfo(hMenu, 3, FALSE, &mii) && mii.fState & MFS_DEFAULT)
		m_eDefaultMenuItem = eAction_Shortcut;
}

// ============================================================================
/// TActionSelector::GetActionSource
/// @date 2009/03/12
///
/// @brief     Calculates the action source based on the state collected so far.
/// @return    Combination of eSrc_* with eAction_*
// ============================================================================
ulong_t TActionSelector::GetActionSource()
{
	unsigned long ulSrc = (m_ulPreferredDropEffect != 0) ? (m_ulKeysState != eKey_None ? eSrc_Keyboard : eSrc_CtxMenu) : eSrc_DropMenu;
	unsigned long ulAction = m_eDefaultMenuItem != eAction_None ? m_eDefaultMenuItem : m_eDefaultOperation;

	return ulSrc | ulAction;
}
