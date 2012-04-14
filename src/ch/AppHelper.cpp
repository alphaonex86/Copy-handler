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
#include "AppHelper.h"
#include "shlobj.h"
#include "../common/version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CH_MUTEX_NAME _T("_Copy handler_ instance")

CAppHelper::CAppHelper()
{
	// read program paths
	RetrievePaths();

	// retrieve VERSION-based info
	RetrieveAppInfo();

	// single-instance protection
	m_bFirstInstance=true;
	m_hMutex=NULL;
}

CAppHelper::~CAppHelper()
{
	if (m_hMutex)
		ReleaseMutex(m_hMutex);

	delete [] m_pszProgramPath;
	delete [] m_pszProgramName;
}

// inits mutex app protection
void CAppHelper::InitProtection()
{
	m_hMutex=CreateMutex(NULL, TRUE, CH_MUTEX_NAME);
	m_bFirstInstance=(m_hMutex != NULL && GetLastError() != ERROR_ALREADY_EXISTS);
}

// retrieves application path
void CAppHelper::RetrievePaths()
{
	// try to find '\\' in path to see if this is only exe name or fully qualified path
#ifdef _UNICODE
	TCHAR* pszArgv = __wargv[0];
#else
	TCHAR* pszArgv = __argv[0];
#endif

	TCHAR* pszName=_tcsrchr(pszArgv, _T('\\'));
	if (pszName != NULL)
	{
		// copy name
		m_pszProgramName=new TCHAR[_tcslen(pszName+1)+1];
		_tcscpy(m_pszProgramName, pszName+1);

		// path
		UINT uiSize=(UINT)(pszName-pszArgv);
        m_pszProgramPath=new TCHAR[uiSize+1];
		_tcsncpy(m_pszProgramPath, pszArgv, uiSize);
		m_pszProgramPath[uiSize]=_T('\0');
	}
	else
	{
		// copy name
		m_pszProgramName=new TCHAR[_tcslen(pszArgv)+1];
		_tcscpy(m_pszProgramName, pszArgv);

		// path
		TCHAR szPath[_MAX_PATH];
		UINT uiSize=GetCurrentDirectory(_MAX_PATH, szPath);
		_tcscat(szPath, _T("\\"));
		m_pszProgramPath=new TCHAR[uiSize+2];
		_tcsncpy(m_pszProgramPath, szPath, uiSize+2);
	}
}

void CAppHelper::RetrieveAppInfo()
{
	m_pszAppName = _T(PRODUCT_NAME);
	m_pszAppNameVer = PRODUCT_FULL_VERSION_T;
	m_pszAppVersion = _T(PRODUCT_VERSION);
}

// internal func - safe getting special folder locations
UINT CAppHelper::GetFolderLocation(int iFolder, PTSTR pszBuffer)
{
	LPITEMIDLIST piid;
	HRESULT h=SHGetSpecialFolderLocation(NULL, iFolder, &piid);
	if (!SUCCEEDED(h))
		return false;

	// get path
	BOOL bRes=SHGetPathFromIDList(piid, pszBuffer);

	// free piid
	LPMALLOC lpm;
	if (!SUCCEEDED(SHGetMalloc(&lpm)))
		return 0;

	lpm->Free((void*)piid);
	lpm->Release();

	// check for error
	if (!bRes)
		return 0;

	// strip the last '\\'
	UINT uiLen=(UINT)_tcslen(pszBuffer);
	if (pszBuffer[uiLen-1] == _T('\\'))
	{
		pszBuffer[uiLen-1]=_T('\0');
		return uiLen-1;
	}
	else
		return uiLen;
}

// expands given path
PTSTR CAppHelper::ExpandPath(PTSTR pszString)
{
	// check if there is need to perform all these checkings
	if (pszString[0] != _T('<'))
		return pszString;

	TCHAR szStr[_MAX_PATH];
	szStr[0]=_T('\0');

	// search for string to replace
	// _T("<WINDOWS>"), _T("<TEMP>"), _T("<SYSTEM>"), _T("<APPDATA>"), _T("<DESKTOP>"), 
	// _T("<PERSONAL>"), _T("<PROGRAM>")
	if (_tcsnicmp(pszString, _T("<PROGRAM>"), 9) == 0)
	{
		// get windows path
		_tcsncpy(szStr, m_pszProgramPath ? m_pszProgramPath : _t(""), _MAX_PATH);
		szStr[_MAX_PATH - 1] = _T('\0');
		_tcsncat(szStr, pszString+9, _MAX_PATH - _tcslen(szStr));
		szStr[_MAX_PATH - 1] = _T('\0');
	}
	else if (_tcsnicmp(pszString, _T("<WINDOWS>"), 9) == 0)
	{
		// get windows path
		UINT uiSize=GetWindowsDirectory(szStr, _MAX_PATH);
		if (szStr[uiSize-1] == _T('\\'))
			szStr[uiSize-1]=_T('\0');
		_tcsncat(szStr, pszString+9, _MAX_PATH - uiSize);
		szStr[_MAX_PATH - 1] = _T('\0');
	}
	else if (_tcsnicmp(pszString, _T("<TEMP>"), 6) == 0)	// temp dir
	{
		// get windows path
		UINT uiSize=GetTempPath(_MAX_PATH, szStr);
		if (szStr[uiSize-1] == _T('\\'))
			szStr[uiSize-1]=_T('\0');
		_tcsncat(szStr, pszString+6, _MAX_PATH - uiSize);
		szStr[_MAX_PATH - 1] = _T('\0');
	}
	else if (_tcsnicmp(pszString, _T("<SYSTEM>"), 8) == 0)	// system
	{
		// get windows path
		UINT uiSize=GetSystemDirectory(szStr, _MAX_PATH);
		if (szStr[uiSize-1] == _T('\\'))
			szStr[uiSize-1]=_T('\0');
		_tcsncat(szStr, pszString+8, _MAX_PATH - uiSize);
		szStr[_MAX_PATH - 1] = _T('\0');
	}
	else if (_tcsnicmp(pszString, _T("<APPDATA>"), 9) == 0)	// app data
	{
		// get windows path
		UINT uiSize=GetFolderLocation(CSIDL_LOCAL_APPDATA, szStr);
		if (szStr[uiSize-1] == _T('\\'))
			szStr[uiSize-1]=_T('\0');
		_tcsncat(szStr, pszString+9, _MAX_PATH - uiSize);
		szStr[_MAX_PATH - 1] = _T('\0');
	}
	else if (_tcsnicmp(pszString, _T("<DESKTOP>"), 9) == 0)	// desktop
	{
		// get windows path
		UINT uiSize=GetFolderLocation(CSIDL_DESKTOPDIRECTORY, szStr);
		if (szStr[uiSize-1] == _T('\\'))
			szStr[uiSize-1]=_T('\0');
		_tcsncat(szStr, pszString+9, _MAX_PATH - uiSize);
		szStr[_MAX_PATH - 1] = _T('\0');
	}
	else if (_tcsnicmp(pszString, _T("<PERSONAL>"), 10) == 0)	// personal...
	{
		// get windows path
		UINT uiSize=GetFolderLocation(CSIDL_PERSONAL, szStr);
		if (szStr[uiSize-1] == _T('\\'))
			szStr[uiSize-1]=_T('\0');
		_tcsncat(szStr, pszString+10, _MAX_PATH - uiSize);
		szStr[_MAX_PATH - 1] = _T('\0');
	}

	// copy to src string
	if (szStr[0] != _T('\0'))
		_tcscpy(pszString, szStr);

	return pszString;
}

bool CAppHelper::GetProgramDataPath(CString& rStrPath)
{
	HRESULT hResult = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, rStrPath.GetBufferSetLength(_MAX_PATH));
	rStrPath.ReleaseBuffer();
	if(FAILED(hResult))
		return false;

	if(rStrPath.Right(1) != _T('\\'))
		rStrPath += _T('\\');

	// make sure to create the required directories if they does not exist
	rStrPath += _T("Copy Handler");
	if(!CreateDirectory(rStrPath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return false;

	// create directory for tasks
//	rStrPath += _T("\\Tasks");
	if(!CreateDirectory(rStrPath + _T("\\Tasks"), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return false;

	return true;
}

bool CAppHelper::SetAutorun(bool bEnable)
{
	// check the current key value (to avoid irritating messages from some firewall software)
	HKEY hkeyRun = NULL;
	LSTATUS lStatus = ERROR_SUCCESS;
	CString strValue;
	CString strKey;
	DWORD dwType = REG_SZ;
	DWORD dwCount = _MAX_PATH * sizeof(TCHAR);

	lStatus = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_QUERY_VALUE, &hkeyRun);
	if(lStatus != ERROR_SUCCESS)
		return false;

	lStatus = RegQueryValueEx(hkeyRun, m_pszAppName, NULL, &dwType, (BYTE*)strValue.GetBufferSetLength(_MAX_PATH), &dwCount);
	RegCloseKey(hkeyRun);

	if(lStatus != ERROR_SUCCESS && lStatus != ERROR_FILE_NOT_FOUND)
	{
		strValue.ReleaseBuffer(0);
		return false;
	}
	if(lStatus == ERROR_FILE_NOT_FOUND)
	{
		strValue.ReleaseBuffer(0);

		// if there is no key in registry and we don't want it, then return with ok status
		if(!bEnable)
			return true;

		// format the data to be written to registry
		strKey.Format(_T("%s\\%s"), m_pszProgramPath, m_pszProgramName);
	}
	else
	{
		// key found
		strValue.ReleaseBuffer(dwCount / sizeof(TCHAR));

		if(bEnable)
		{
			// key exists in registry, check if the value is correct
			strKey.Format(_T("%s\\%s"), m_pszProgramPath, m_pszProgramName);

			if(strValue.CompareNoCase(strKey) == 0)
				return true;
		}
	}

	// we want to write information to the registry
	// storing key in registry
	lStatus = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hkeyRun);
	if(lStatus != ERROR_SUCCESS)
		return false;
	
	if(bEnable)
		lStatus = RegSetValueEx(hkeyRun, m_pszAppName, 0, REG_SZ, (BYTE*)(PCTSTR)strKey, (DWORD)(strKey.GetLength() + 1) * sizeof(TCHAR));
	else
		lStatus = RegDeleteValue(hkeyRun, m_pszAppName);
	
	RegCloseKey(hkeyRun);

	return lStatus == ERROR_SUCCESS;
}
