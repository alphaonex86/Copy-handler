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
#ifndef __DESTPATH_H__
#define __DESTPATH_H__

#include "../libicpf/file.h"

class CDestPath
{
public:
	CDestPath() { m_iDriveNumber=-1; m_uiDriveType=static_cast<UINT>(-1); };
	void SetPath(LPCTSTR lpszPath);
	const CString& GetPath() const { return m_strPath; };

	int GetDriveNumber() const { return m_iDriveNumber; };
	UINT GetDriveType() const { return m_uiDriveType; };

	void Serialize(icpf::archive& ar);

protected:
	CString m_strPath;	// always with ending '\\'
	int m_iDriveNumber;	// initialized within setpath (std -1)
	UINT m_uiDriveType;	// disk type - -1 if none, the rest like in GetDriveType
};

#endif