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
#include "StringHelpers.h"
#include "stdio.h"

LPTSTR GetSizeString(double dData, LPTSTR pszBuffer, size_t stMaxBufferSize)
{
	if (dData < 0.0)
		dData=0.0;

	if (dData < 1200.0)
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%.2f %s"), dData, g_pscsShared->szSizes[0]);
	else if (dData < 1228800.0)
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%.2f %s"), static_cast<double>(dData)/1024.0, g_pscsShared->szSizes[1]);
	else if (dData < 1258291200.0)
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%.2f %s"), static_cast<double>(dData)/1048576.0, g_pscsShared->szSizes[2]);
	else
		_sntprintf(pszBuffer, stMaxBufferSize, _T("%.2f %s"), static_cast<double>(dData)/1073741824.0, g_pscsShared->szSizes[3]);

	return pszBuffer;
}
