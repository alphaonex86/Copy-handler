// ============================================================================
//  Copyright (C) 2001-2009 by Jozef Starosczyk
//  ixen@copyhandler.com
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Library General Public License
//  (version 2) as published by the Free Software Foundation;
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public
//  License along with this program; if not, write to the
//  Free Software Foundation, Inc.,
//  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ============================================================================
/// @file ActionSelector.h
/// @date 2009/03/12
/// @brief Contains a class handling the action selection for drag&drop menu.
// ============================================================================
#ifndef __ACTIONSELECTOR_H__
#define __ACTIONSELECTOR_H__

class TActionSelector
{
public:
	/// State of the keyboard
	enum EStateKeys
	{
		eKey_None = 0,
		eKey_Ctrl = 1,
		eKey_Alt = 2,
		eKey_Shift = 4
	};

	/// Default operation
	enum EAction
	{
		eAction_None		= 0x0000,
		eAction_Copy		= 0x0001,
		eAction_Move		= 0x0002,
		eAction_Shortcut	= 0x0004
	};

	enum EActionSource
	{
		eSrc_None			= 0x0000,
		eSrc_CtxMenu		= 0x0100,
		eSrc_DropMenu		= 0x0200,
		eSrc_Keyboard		= 0x0400,
	};

	enum EActionOptions
	{
		eOption_None				= 0x0000,	///< No integration at all
		eOption_BasicIntegration	= 0x0001,	///< Basic integration (CH menu items, no override)
		eOption_DragAndDropOverride	= 0x0002,	///< Intercepts drag&drop operation using the left mouse button
		eOption_KeyboardOverride	= 0x0004	///< Intercepts Ctrl+V operation
	};

public:
	TActionSelector();
	~TActionSelector();

	/// Reads current keyboard state
	void ReadKeyboardState();
	/// Retrieves all informations available in IDataObject related to the action selection process
	HRESULT ReadStateFromDataObject(IDataObject* piDataObject, PCTSTR pszDestinationPath);
	/// Retrieves the state information from a menu handle
	void ReadStateFromMenu(HMENU hMenu);

	/// Retrieves the action source using the 
	ulong_t GetActionSource();

	/// Sets the action options (EActionOptions)
	void SetActionOptions(ulong_t ulOptions) { m_ulActionOptions = ulOptions; }

	/// Resets all states
	void ResetState();

protected:
	ulong_t m_ulKeysState;				///< State of the ctrl/shift/alt keys
	ulong_t m_ulPreferredDropEffect;	///< Preferred drop effect from IDataObject
	EAction m_eDefaultOperation;		///< Default operation determined from IDataObject (detected with simple heuristics)
	EAction m_eDefaultMenuItem;			///< Default operation retrieved using the menu handle
	unsigned long m_ulActionOptions;	///< Options determining the final action to be performed
};

#endif
