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
#include "DestPath.h"
//#include "../libicpf/file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GetDriveData(LPCTSTR lpszPath, int* piDrvNum, UINT* puiDrvType)
{
	TCHAR drv[_MAX_DRIVE+1];
	
	_tsplitpath(lpszPath, drv, NULL, NULL, NULL);
	if(lstrlen(drv) != 0)
	{
		// add '\\'
		lstrcat(drv, _T("\\"));
		_tcsupr(drv);
		
		// disk number
		if (piDrvNum)
			*piDrvNum=drv[0]-_T('A');

		// disk type
		if (puiDrvType)
		{
			*puiDrvType=GetDriveType(drv);
			if (*puiDrvType == DRIVE_NO_ROOT_DIR)
				*puiDrvType=DRIVE_UNKNOWN;
		}
	}
	else
	{
		// there's no disk in a path
		if (piDrvNum)
			*piDrvNum=-1;

		if (puiDrvType)
		{
			// check for unc path
			if (_tcsncmp(lpszPath, _T("\\\\"), 2) == 0)
				*puiDrvType=DRIVE_REMOTE;
			else
				*puiDrvType=DRIVE_UNKNOWN;
		}
	}
}

void CDestPath::SetPath(LPCTSTR lpszPath)
{
	m_strPath=lpszPath;

	// make sure '\\' has been added
	if (m_strPath.Right(1) != _T('\\'))
		m_strPath+=_T('\\');

	GetDriveData(m_strPath, &m_iDriveNumber, &m_uiDriveType);
}

void CDestPath::Serialize(icpf::archive& ar)
{
	if (ar.is_storing())
	{
		ar<<m_strPath;
		ar<<m_iDriveNumber;
		ar<<m_uiDriveType;
	}
	else
	{
		ar>>m_strPath;
		ar>>m_iDriveNumber;
		ar>>m_uiDriveType;
	}
}
