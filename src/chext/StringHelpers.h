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

#include "..\common\ipcstructs.h"

extern CSharedConfigStruct* g_pscsShared;

LPTSTR GetSizeString(double dData, LPTSTR pszBuffer, size_t stMaxBufferSize);

template<class T>
LPTSTR GetSizeString(T tData, LPTSTR pszBuffer, size_t stMaxBufferSize, bool bStrict = false)
{
	if (tData < 0)
		tData=0;

	if (tData >= 1258291200 && (!bStrict || (tData % 1073741824) == 0))
	{
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%.2f %s"), static_cast<double>(tData)/1073741824.0, g_pscsShared->szSizes[3]);
		return pszBuffer;
	}
	else if (tData >= 1228800 && (!bStrict || (tData % 1048576) == 0))
	{
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%.2f %s"), static_cast<double>(tData)/1048576.0, g_pscsShared->szSizes[2]);
		return pszBuffer;
	}
	else if (tData >= 1200 && (!bStrict || (tData % 1024) == 0))
	{
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%.2f %s"), static_cast<double>(tData)/1024.0, g_pscsShared->szSizes[1]);
		return pszBuffer;
	}
	else
	{
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%lu %s"), (unsigned long)tData, g_pscsShared->szSizes[0]);
		return pszBuffer;
	}
}

#endif
