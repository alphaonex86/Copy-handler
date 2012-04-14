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
#include "shortcuts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CShortcut::FromString(const CString& strText)
{
	int iPos=strText.ReverseFind(_T('|'));
	if (iPos != -1 && iPos < strText.GetLength()-1)
	{
		m_strName=strText.Left(iPos);
		m_strPath=strText.Mid(iPos+1);

		return true;
	}
	else
		return false;
}

CShortcut::CShortcut(const CString& strText)
{
	FromString(strText);
}

CShortcut::operator CString()
{
	if (m_strPath.IsEmpty())
		return _T("");
	else
		return m_strName+_T("|")+m_strPath;
}
