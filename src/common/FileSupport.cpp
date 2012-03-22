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
#include "wtypes.h"
#include "FileSupport.h"
//#include "tchar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning (disable: 4711) 

__int64 SetFilePointer64(HANDLE hFile, __int64 llDistance, DWORD dwMoveMethod)
{
   LARGE_INTEGER li;

   li.QuadPart = llDistance;

   li.LowPart = SetFilePointer(hFile, li.LowPart, &li.HighPart, dwMoveMethod);

   if (li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
      li.QuadPart = -1;

   return li.QuadPart;
}

__int64 GetFilePointer64(HANDLE hFile)
{
	return SetFilePointer64(hFile, 0, FILE_CURRENT);
}

__int64 GetFileSize64(HANDLE hFile)
{
	ULARGE_INTEGER li;

	li.LowPart = GetFileSize(hFile, &li.HighPart); 
 
	// If we failed ... 
	if (li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
		li.QuadPart=static_cast<unsigned __int64>(-1);

	return li.QuadPart;
}

bool SetFileSize64(LPCTSTR lpszFilename, __int64 llSize)
{
	HANDLE hFile=CreateFile(lpszFilename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	if (SetFilePointer64(hFile, llSize, FILE_BEGIN) == -1)
	{
		CloseHandle(hFile);
		return false;
	}

	if (!SetEndOfFile(hFile))
	{
		CloseHandle(hFile);
		return false;
	}

	if (!CloseHandle(hFile))
		return false;
	
	return true;
}

// disk support routines

bool GetDynamicFreeSpace(LPCTSTR lpszPath, ull_t* pFree, ull_t* pTotal)
{
	ULARGE_INTEGER ui64Available, ui64Total;
	if(!GetDiskFreeSpaceEx(lpszPath, &ui64Available, &ui64Total, NULL))
	{
		if(pFree)
			*pFree = 0;
		if(pTotal)
			*pTotal = 0;
		return false;
	}
	else
	{
		if(pFree)
			*pFree=ui64Available.QuadPart;
		if(pTotal)
			*pTotal=ui64Total.QuadPart;
		return true;
	}
}
