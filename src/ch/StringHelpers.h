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
#ifndef __STRINGHELPERS_H__
#define __STRINGHELPERS_H__

// formatting routines
#ifdef _MFC_VER
void ExpandFormatString(CString* pstrFmt, DWORD dwError);
#endif

LPTSTR GetSizeString(double dData, LPTSTR pszBuffer, size_t stMaxBufferSize);
LPTSTR GetSizeString(ull_t ullData, LPTSTR pszBuffer, size_t stMaxBufferSize, bool bStrict = false);

#endif
