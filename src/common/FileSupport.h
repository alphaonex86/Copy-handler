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
#ifndef __FILESUPPORT_ROUTINES_H__
#define __FILESUPPORT_ROUTINES_H__

// file support routines
__int64 SetFilePointer64(HANDLE hFile, __int64 llDistance, DWORD dwMoveMethod);
__int64 GetFilePointer64(HANDLE hFile);
__int64 GetFileSize64(HANDLE hFile);
bool SetFileSize64(LPCTSTR lpszFilename, __int64 llSize);

// disk support routines
bool GetDynamicFreeSpace(LPCTSTR lpszPath, ull_t* pFree, ull_t* pTotal);

#endif