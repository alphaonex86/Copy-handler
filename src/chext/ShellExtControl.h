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
#ifndef __SHELLEXTCONTROL_H_
#define __SHELLEXTCONTROL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDropMenuExt
class ATL_NO_VTABLE CShellExtControl : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CShellExtControl, &CLSID_CShellExtControl>,
	public IDispatchImpl<IShellExtControl, &IID_IShellExtControl, &LIBID_CHEXTLib>
{
public:
	CShellExtControl();
	~CShellExtControl();

	STDMETHOD(GetVersion)(LONG* plVersion, BSTR* pbstrVersion);
	STDMETHOD(SetFlags)(LONG lFlags, LONG lMask);
	STDMETHOD(GetFlags)(LONG* plFlags);

DECLARE_REGISTRY_RESOURCEID(IDR_SHELLEXTCONTROL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CShellExtControl)
	COM_INTERFACE_ENTRY(IUnknown)
	COM_INTERFACE_ENTRY(IShellExtControl)
END_COM_MAP()

protected:
	HANDLE m_hMemory;
	HANDLE m_hMutex;
	struct SHELLEXT_DATA
	{
		long m_lID;
		long m_lFlags;
	} *m_pShellExtData;

	CComAutoCriticalSection m_lock;
};

#endif //__SHELLEXTCONTROL_H_
