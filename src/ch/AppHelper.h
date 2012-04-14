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
#ifndef __APPHELPER_H__
#define __APPHELPER_H__

class CAppHelper
{
public:
	CAppHelper();
	virtual ~CAppHelper();

	bool SetAutorun(bool bState);		// changes state of "run with system" option
	PTSTR ExpandPath(PTSTR pszString);	// expands path string - ie. <windows> into c:\windows

	bool IsFirstInstance() const { return m_bFirstInstance; };

	PCTSTR GetAppName() const { return m_pszAppName; };
	PCTSTR GetAppNameVer() const { return m_pszAppNameVer; };
	PCTSTR GetAppVersion() const { return m_pszAppVersion; };

	PCTSTR GetProgramPath() const { return m_pszProgramPath; };
	PCTSTR GetProgramName() const { return m_pszProgramName; };

	static bool GetProgramDataPath(CString& rStrPath);

protected:
	void InitProtection();		// optional call - protects from running multiple instance
	void RetrievePaths();							// reads program's path and name
	void RetrieveAppInfo();							// reads app name and version from VERSION resource
	UINT GetFolderLocation(int iFolder, PTSTR pszBuffer);

protected:
	HANDLE m_hMutex;
	bool m_bFirstInstance;		// tells if it is first instance(true) or second(or third, ...)

	// program placement
	TCHAR* m_pszProgramPath;	// path from which this program was run
	TCHAR* m_pszProgramName;	// name of this program (ie. CH.exe)

	TCHAR* m_pszAppName;		// app-name string of this app
	TCHAR* m_pszAppNameVer;		// extended app-name-with small version
	TCHAR* m_pszAppVersion;		// app-version string of this app (VERSION based)
};

#endif