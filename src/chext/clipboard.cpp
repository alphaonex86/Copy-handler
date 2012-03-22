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
#include "clipboard.h"

void GetDataFromClipboard(HDROP hdrop, LPCTSTR pszDstPath, LPTSTR *pszBuffer, UINT* pSize)
{
	// get clipboard data
	UINT uiBufferSize = (pszDstPath == NULL) ? 0 : (UINT)_tcslen(pszDstPath) + 1;
	UINT uiFilesCount = DragQueryFile(hdrop, 0xffffffff, NULL, 0);
	
	// count size
	for(UINT i=0;i<uiFilesCount;i++)
	{
		uiBufferSize+=DragQueryFile(hdrop, i, NULL, 0)+1;
	}
	
	// new buffer
	*pszBuffer=new TCHAR[uiBufferSize];
	*pSize=uiBufferSize;
	TCHAR szPath[_MAX_PATH];
	
	// copy pszDstPath
	if (pszDstPath != NULL)
		_tcscpy(*pszBuffer, pszDstPath);
	
	// size of pszDstPath
	UINT uiOffset=(pszDstPath == NULL) ? 0 : (UINT)_tcslen(pszDstPath)+1;
	
	// get files and put it in a table
	UINT uiSize;
	for (UINT i=0;i<uiFilesCount;i++)
	{
		uiSize=DragQueryFile(hdrop, i, szPath, _MAX_PATH);
		_tcscpy(*pszBuffer+uiOffset, szPath);
		uiOffset+=uiSize+1;
	}
}
