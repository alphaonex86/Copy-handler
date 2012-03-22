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
#include "register.h"
#include "objbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT RegisterShellExtDll(LPCTSTR lpszPath, bool bRegister)
{
	// first try - load dll and register it manually.
	HRESULT hResult = S_OK;
	// if failed - try by loading extension manually (would fail on vista when running as user)
	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(SUCCEEDED(hResult))
	{
		HRESULT (STDAPICALLTYPE *pfn)(void);
		HINSTANCE hMod = LoadLibrary(lpszPath);	// load the dll
		if(hMod == NULL)
			hResult = HRESULT_FROM_WIN32(GetLastError());
		if(SUCCEEDED(hResult) && !hMod)
			hResult = E_FAIL;
		if(SUCCEEDED(hResult))
		{
			(FARPROC&)pfn = GetProcAddress(hMod, (bRegister ? "DllRegisterServer" : "DllUnregisterServer"));
			if(pfn == NULL)
				hResult = E_FAIL;
			if(SUCCEEDED(hResult))
				hResult = (*pfn)();

			CoFreeLibrary(hMod);
		}
		CoUninitialize();
	}

	// if previous operation failed (ie. vista system) - try running regsvr32 with elevated privileges
	if(SCODE_CODE(hResult) == ERROR_ACCESS_DENIED)
	{
		hResult = S_FALSE;
		// try with regsvr32
		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_UNICODE;
		sei.lpVerb = _T("runas");
		sei.lpFile = _T("regsvr32.exe");
		CString strParams;
		if(bRegister)
			strParams = CString(_T(" \"")) + lpszPath + CString(_T("\""));
		else
			strParams = CString(_T("/u \"")) + lpszPath + CString(_T("\""));
		sei.lpParameters = strParams;
		sei.nShow = SW_SHOW;

		if(!ShellExecuteEx(&sei))
			hResult = E_FAIL;
	}

	return hResult;
}
