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
#include <comutil.h>
#include "ShellExtControl.h"
#include "../common/version.h"

CShellExtControl::CShellExtControl() :
	m_pShellExtData(NULL),
	m_hMemory(NULL),
	m_hMutex(NULL)
{
	// create protection mutex
	m_hMutex = ::CreateMutex(NULL, FALSE, _T("CHShellExtControlDataMutex"));
	if(!m_hMutex)
		return;

	DWORD dwRes = WaitForSingleObject(m_hMutex, 10000);
	if(dwRes != WAIT_OBJECT_0)
		return;
	
	// memory mapped file
	m_hMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SHELLEXT_DATA), _T("CHShellExtControlData"));    // name of map object
	if(!m_hMemory) 
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
		return;
	}

	DWORD dwLastError = GetLastError();
	m_pShellExtData = (SHELLEXT_DATA*)MapViewOfFile(m_hMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if(!m_pShellExtData)
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
		CloseHandle(m_hMemory);
		m_hMemory = NULL;
		return;
	}

	if(dwLastError != ERROR_ALREADY_EXISTS)
	{
		m_pShellExtData->m_lFlags = 0;
		m_pShellExtData->m_lID = GetTickCount();
	}

	ReleaseMutex(m_hMutex);
}

CShellExtControl::~CShellExtControl()
{
	if(m_pShellExtData)
	{
		UnmapViewOfFile((LPVOID)m_pShellExtData); 

		// Close the process's handle to the file-mapping object.
		CloseHandle(m_hMemory); 
	}

	if(m_hMutex)
		CloseHandle(m_hMutex);
}

STDMETHODIMP CShellExtControl::GetVersion(LONG* plVersion, BSTR* pbstrVersion)
{
	if(!plVersion || !pbstrVersion || (*pbstrVersion))
		return E_INVALIDARG;

	(*plVersion) = PRODUCT_VERSION1 << 24 | PRODUCT_VERSION2 << 16 | PRODUCT_VERSION3 << 8 | PRODUCT_VERSION4;
	_bstr_t strVer(SHELLEXT_PRODUCT_FULL_VERSION);
	pbstrVersion = strVer.GetAddress();

	return S_OK;
}

STDMETHODIMP CShellExtControl::SetFlags(LONG lFlags, LONG lMask)
{
	if(!m_hMutex || !m_pShellExtData)
		return E_FAIL;

	DWORD dwRes = WaitForSingleObject(m_hMutex, 10000);
	if(dwRes != WAIT_OBJECT_0)
		return E_FAIL;
	m_pShellExtData->m_lFlags = (m_pShellExtData->m_lFlags & ~lMask) | (lFlags & lMask);

	ReleaseMutex(m_hMutex);

	return S_OK;
}

STDMETHODIMP CShellExtControl::GetFlags(LONG* plFlags)
{
	if(!m_hMutex || !m_pShellExtData)
		return E_FAIL;
	if(!plFlags)
		return E_INVALIDARG;

	DWORD dwRes = WaitForSingleObject(m_hMutex, 10000);
	if(dwRes != WAIT_OBJECT_0)
		return E_FAIL;

	(*plFlags) = m_pShellExtData->m_lFlags;

	ReleaseMutex(m_hMutex);

	return S_OK;
}
