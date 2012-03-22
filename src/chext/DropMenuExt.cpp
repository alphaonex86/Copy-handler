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
#include "chext.h"
#include "DropMenuExt.h"
#include "clipboard.h"
#include "chext-utils.h"
#include "..\Common\ipcstructs.h"

/////////////////////////////////////////////////////////////////////////////
// CDropMenuExt

extern CSharedConfigStruct* g_pscsShared;

CDropMenuExt::CDropMenuExt() :
	m_piShellExtControl(NULL)
{
	m_szDstPath[0] = _T('\0');
	CoCreateInstance(CLSID_CShellExtControl, NULL, CLSCTX_ALL, IID_IShellExtControl, (void**)&m_piShellExtControl);
}

CDropMenuExt::~CDropMenuExt()
{
	if(m_piShellExtControl)
	{
		m_piShellExtControl->Release();
		m_piShellExtControl = NULL;
	}
}

HRESULT CDropMenuExt::ReadFileData(IDataObject* piDataObject)
{
	_ASSERTE(piDataObject);
	if(!piDataObject)
		return E_INVALIDARG;

	// retrieve some informations from the data object
	STGMEDIUM medium;
	FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

	// retrieve the CF_HDROP-type data from data object
	HRESULT hResult = piDataObject->QueryGetData(&fe);
	if(hResult != S_OK)
		return hResult;
	hResult = piDataObject->GetData(&fe, &medium);
	if(SUCCEEDED(hResult))
		GetDataFromClipboard(static_cast<HDROP>(medium.hGlobal), m_szDstPath, &m_bBuffer.m_pszFiles, &m_bBuffer.m_iDataSize);

	ReleaseStgMedium(&medium);

	return hResult;
}

STDMETHODIMP CDropMenuExt::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* piDataObject, HKEY /*hkeyProgID*/)
{
	ATLTRACE(_T("CDropMenuExt::Initialize()\n"));
	if(!piDataObject)
		return E_FAIL;

	// remember the keyboard state for later
	m_asSelector.ResetState();
	m_asSelector.ReadKeyboardState();

	// check if this extension is enabled
	HRESULT hResult = IsShellExtEnabled(m_piShellExtControl);
	if(FAILED(hResult) || hResult == S_FALSE)
		return hResult;

	// find window
	HWND hWnd = ::FindWindow(_T("Copy Handler Wnd Class"), _T("Copy handler"));
	if(hWnd == NULL)
		return E_FAIL;

	// retrieve config from CH
	::SendMessage(hWnd, WM_GETCONFIG, GC_DRAGDROP, 0);

	// get dest folder
	m_szDstPath[0]=_T('\0');
	if(!SHGetPathFromIDList(pidlFolder, m_szDstPath))
		return E_FAIL;

	// now retrieve the preferred drop effect from IDataObject
	hResult = m_asSelector.ReadStateFromDataObject(piDataObject, m_szDstPath);
	if(SUCCEEDED(hResult))
		hResult = ReadFileData(piDataObject);

	return hResult;
}

STDMETHODIMP CDropMenuExt::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT /*idCmdLast*/, UINT /*uFlags*/)
{
	ATLTRACE(_T("CDropMenuExt::QueryContextMenu()\n"));
	// check options
	HRESULT hResult = IsShellExtEnabled(m_piShellExtControl);
	if(FAILED(hResult) || hResult == S_FALSE)
		return hResult;

	// find CH's window
	HWND hWnd;
	hWnd=::FindWindow(_T("Copy Handler Wnd Class"), _T("Copy handler"));
	if(!hWnd)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	// retrieve the default menu item; if not available, fallback to the default heuristics
	m_asSelector.ReadStateFromMenu(hmenu);

	// retrieve the action information to be performed
	ulong_t ulActionSource = m_asSelector.GetActionSource();

	// determine if we want to perform override based on user options and detected action source
	bool bIntercept = (g_pscsShared->uiFlags & CSharedConfigStruct::eFlag_InterceptDragAndDrop && ulActionSource & TActionSelector::eSrc_DropMenu ||
		g_pscsShared->uiFlags & CSharedConfigStruct::eFlag_InterceptKeyboardActions && ulActionSource & TActionSelector::eSrc_Keyboard ||
		g_pscsShared->uiFlags & CSharedConfigStruct::eFlag_InterceptCtxMenuActions && ulActionSource & TActionSelector::eSrc_CtxMenu);

	// now convert our information to the 
	// got a config
	_COMMAND* pCommand = g_pscsShared->GetCommandsPtr();
	int iCommandCount=0;

	// ad new menu items, depending on the received configuration
	if(g_pscsShared->uiFlags & CSharedConfigStruct::DD_COPY_FLAG)
	{
		::InsertMenu(hmenu, indexMenu+iCommandCount, MF_BYPOSITION | MF_STRING, idCmdFirst+0, pCommand[0].szCommand);
		if(bIntercept && ulActionSource & TActionSelector::eAction_Copy)
			::SetMenuDefaultItem(hmenu, idCmdFirst+0, FALSE);
		iCommandCount++;
	}

	if(g_pscsShared->uiFlags & CSharedConfigStruct::DD_MOVE_FLAG)
	{
		::InsertMenu(hmenu, indexMenu+iCommandCount, MF_BYPOSITION | MF_STRING, idCmdFirst+1, pCommand[1].szCommand);
		if(bIntercept && ulActionSource & TActionSelector::eAction_Move)
			::SetMenuDefaultItem(hmenu, idCmdFirst+1, FALSE);
		iCommandCount++;
	}

	if(g_pscsShared->uiFlags & CSharedConfigStruct::DD_COPYMOVESPECIAL_FLAG)
	{
		::InsertMenu(hmenu, indexMenu+iCommandCount, MF_BYPOSITION | MF_STRING, idCmdFirst+2, pCommand[2].szCommand);
/*
		if(g_pscsShared->bOverrideDefault && m_eDropEffect == eEffect_Special)
			::SetMenuDefaultItem(hmenu, idCmdFirst+2, FALSE);
*/
		iCommandCount++;
	}

	if(iCommandCount)
	{
		::InsertMenu(hmenu, indexMenu+iCommandCount, MF_BYPOSITION | MF_SEPARATOR, idCmdFirst+3, NULL);
		iCommandCount++;
	}

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 4);
}

STDMETHODIMP CDropMenuExt::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* /*pwReserved*/, LPSTR pszName, UINT cchMax)
{
	// check options
	HRESULT hResult = IsShellExtEnabled(m_piShellExtControl);
	if(FAILED(hResult) || hResult == S_FALSE)
	{
		pszName[0] = _T('\0');
		return hResult;
	}

	if(uFlags == GCS_HELPTEXTW)
	{
		USES_CONVERSION;

		// find CH's window
		HWND hWnd;
		hWnd=::FindWindow(_T("Copy Handler Wnd Class"), _T("Copy handler"));
		if(hWnd)
		{
			_COMMAND* pCommand = g_pscsShared->GetCommandsPtr();
			
			switch (idCmd)
			{
			case 0:
			case 1:
			case 2:
				{
					CT2W ct2w(pCommand[idCmd].szDesc);
					wcsncpy(reinterpret_cast<wchar_t*>(pszName), ct2w, cchMax);
					break;
				}
			default:
				wcsncpy(reinterpret_cast<wchar_t*>(pszName), L"", cchMax);
				break;
			}
		}
		else
			wcsncpy(reinterpret_cast<wchar_t*>(pszName), L"", cchMax);
	}
	if(uFlags == GCS_HELPTEXTA)
	{
		// find CH's window
		HWND hWnd;
		hWnd=::FindWindow(_T("Copy Handler Wnd Class"), _T("Copy handler"));
		
		if(hWnd)
		{
			_COMMAND* pCommand = g_pscsShared->GetCommandsPtr();

			switch (idCmd)
			{
			case 0:
			case 1:
			case 2:
				{
					CT2A ct2a(pCommand[idCmd].szDesc);
					strncpy(pszName, ct2a, cchMax);
					break;
				}
			default:
				strncpy(pszName, "", cchMax);
				break;
			}
		}
		else
			strncpy(pszName, "", cchMax);
	}

	return S_OK;
}

STDMETHODIMP CDropMenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
{
	ATLTRACE(_T("CDropMenuExt::InvokeCommand()\n"));
	HRESULT hResult = IsShellExtEnabled(m_piShellExtControl);
	if(FAILED(hResult) || hResult == S_FALSE)
		return E_FAIL;		// required to process other InvokeCommand handlers.

	// find window
	HWND hWnd=::FindWindow(_T("Copy Handler Wnd Class"), _T("Copy handler"));
	if(hWnd == NULL)
		return E_FAIL;

	// commands
	_COMMAND* pCommand = g_pscsShared->GetCommandsPtr();

	// IPC struct
	COPYDATASTRUCT cds;
	cds.dwData=pCommand[LOWORD(lpici->lpVerb)].uiCommandID;	// based on command's number (0-copy, 1-move, 2-special (copy), 3-special (move))
	cds.cbData=m_bBuffer.m_iDataSize * sizeof(TCHAR);
	cds.lpData=m_bBuffer.m_pszFiles;

	// send a message to ch
	::SendMessage(hWnd, WM_COPYDATA, reinterpret_cast<WPARAM>(lpici->hwnd), reinterpret_cast<LPARAM>(&cds));

	m_bBuffer.Destroy();

	return S_OK;
}

STDMETHODIMP CDropMenuExt::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return HandleMenuMsg2(uMsg, wParam, lParam, NULL);
}

STDMETHODIMP CDropMenuExt::HandleMenuMsg2(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* /*plResult*/)
{
	ATLTRACE(_T("CDropMenuExt::HandleMenuMsg2(): uMsg = %lu, wParam = %lu, lParam = %lu\n"), uMsg, wParam, lParam);
	return S_FALSE;
}
