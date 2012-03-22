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
#ifndef __SHAREDDATA_H__
#define __SHAREDDATA_H__

// messages used
#define WM_GETCONFIG	WM_USER+20

// config type to get from program
#define GC_DRAGDROP		0x00
#define GC_EXPLORER		0x01

// command properties (used in menu displaying)
#pragma pack(push, 1)
struct _COMMAND
{
	UINT uiCommandID;		// command ID - would be send be
	TCHAR szCommand[128];	// command name
	TCHAR szDesc[128];		// and it's description
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _SHORTCUT
{
	TCHAR szName[128];
	TCHAR szPath[_MAX_PATH];
};
#pragma pack(pop)

// shared memory size in bytes
#define SHARED_BUFFERSIZE	65536

// structure used for passing data from program to DLL
// the rest is a dynamic texts
class CSharedConfigStruct
{
public:
	enum EFlags
	{
		// drag&drop flags
		OPERATION_MASK					= 0x00ffffff,
		DD_COPY_FLAG					= 0x00000001,
		DD_MOVE_FLAG					= 0x00000002,
		DD_COPYMOVESPECIAL_FLAG			= 0x00000004,

		EC_PASTE_FLAG					= 0x00000010,
		EC_PASTESPECIAL_FLAG			= 0x00000020,
		EC_COPYTO_FLAG					= 0x00000040,
		EC_MOVETO_FLAG					= 0x00000080,
		EC_COPYMOVETOSPECIAL_FLAG		= 0x00000100,

		eFlag_InterceptDragAndDrop		= 0x00000200,
		eFlag_InterceptKeyboardActions	= 0x00000400,
		eFlag_InterceptCtxMenuActions	= 0x00000800
	};
public:
	_SHORTCUT* GetShortcutsPtr() const { return (_SHORTCUT*)(byData + iCommandCount * sizeof(_COMMAND)); }
	_COMMAND* GetCommandsPtr() const { return (_COMMAND*)byData; }

public:
	UINT uiFlags;				// what items and how to display in drag&drop ctx menu & explorer.ctx.menu

	bool bShowFreeSpace;		// showthe free space by the shortcuts ?
	TCHAR szSizes[6][64];		// names of the kB, GB, ...
	bool bShowShortcutIcons;	// show shell icons with shortcuts ?

	int iCommandCount;			// count of commands stored at the beginning of a buffer
	int iShortcutsCount;		// count of shortcuts to display in submenus
	
	BYTE byData[SHARED_BUFFERSIZE];		// buffer for texts and other stuff
};

#endif
