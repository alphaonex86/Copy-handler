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
#ifndef __MENUEXT_H_
#define __MENUEXT_H_

#include "resource.h"       // main symbols

///////
// globals
void CutAmpersands(LPTSTR lpszString);

/////////////////////////////////////////////////////////////////////////////
// CMenuExt
class ATL_NO_VTABLE CMenuExt : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMenuExt, &CLSID_MenuExt>,
	public IShellExtInit,
	public IContextMenu3
{
public:
	CMenuExt();
	~CMenuExt();

DECLARE_REGISTRY_RESOURCEID(IDR_MENUEXT)
DECLARE_NOT_AGGREGATABLE(CMenuExt)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMenuExt)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
	COM_INTERFACE_ENTRY(IContextMenu2)
	COM_INTERFACE_ENTRY(IContextMenu3)
END_COM_MAP()

// IMenuExt
public:
	STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY /*hkeyProgID*/);
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
	STDMETHOD(GetCommandString)(UINT_PTR idCmd, UINT uFlags, UINT* /*pwReserved*/, LPSTR pszName, UINT cchMax);
	STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT /*idCmdLast*/, UINT /*uFlags*/);
	STDMETHOD(HandleMenuMsg)(UINT uMsg, WPARAM wParam, LPARAM lParam);
	STDMETHOD(HandleMenuMsg2)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

protected:
	void DrawMenuItem(LPDRAWITEMSTRUCT lpdis);
	void CreateShortcutsMenu(UINT uiIDBase, bool bOwnerDrawn);

protected:
	// class for making sure memory is freed
	class CBuffer
	{
	public:
		CBuffer() { m_pszFiles=NULL; m_iDataSize=0; };
		void Destroy() { delete [] m_pszFiles; m_pszFiles=NULL; m_iDataSize=0; };
		~CBuffer() { Destroy(); };

	public:
		TCHAR *m_pszFiles;
		UINT m_iDataSize;
	} m_bBuffer;

	TCHAR m_szDstPath[_MAX_PATH];

	// for making sure DestroyMenu would be called
	class CSubMenus
	{
	public:
		CSubMenus() { hShortcuts[0]=NULL; hShortcuts[1]=NULL; hShortcuts[2]=NULL; };
		void Destroy() { for (int i=0;i<3;i++) { if (hShortcuts[i] != NULL) DestroyMenu(hShortcuts[i]); } };
		~CSubMenus() { Destroy(); };

	public:
		HMENU hShortcuts[3];
	} m_mMenus;

	bool m_bBackground;		// folder or folder background
	bool m_bGroupFiles;		// if the group of files have a files in it

	UINT m_uiFirstID;		// first menu ID
	bool m_bShown;			// have the menu been already shown ?

	IShellExtControl* m_piShellExtControl;
};

#endif //__MENUEXT_H_
