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
#ifndef __DROPMENUEXT_H_
#define __DROPMENUEXT_H_

#include "resource.h"       // main symbols
#include "ActionSelector.h"

/////////////////////////////////////////////////////////////////////////////
// CDropMenuExt
class ATL_NO_VTABLE CDropMenuExt : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDropMenuExt, &CLSID_DropMenuExt>,
	public IShellExtInit,
	public IContextMenu3
{
public:
	CDropMenuExt();
	~CDropMenuExt();

DECLARE_REGISTRY_RESOURCEID(IDR_DROPMENUEXT)
DECLARE_NOT_AGGREGATABLE(CDropMenuExt)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CDropMenuExt)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()

public:
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
	STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, IDataObject* piDataObject, HKEY /*hkeyProgID*/);
	STDMETHOD(GetCommandString)(UINT_PTR idCmd, UINT uFlags, UINT* /*pwReserved*/, LPSTR pszName, UINT cchMax);
	STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT /*idCmdLast*/, UINT uFlags);

	STDMETHOD(HandleMenuMsg)(UINT uMsg, WPARAM wParam, LPARAM lParam);
	STDMETHOD(HandleMenuMsg2)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

protected:
	HRESULT ReadFileData(IDataObject* piDataObject);

protected:
	TCHAR m_szDstPath[_MAX_PATH];

	IShellExtControl* m_piShellExtControl;
	TActionSelector m_asSelector;

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
};

#endif //__DROPMENUEXT_H_
