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
#include "conv.h"
#include <string.h>

BEGIN_ICPF_NAMESPACE

char_t __hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

LIBICPF_API void bin2hex(const uchar_t* pbyIn, uint_t tInCount, char_t *pszOut)
{
	for (uint_t i=0;i<tInCount;i++)
	{
		*pszOut++=__hex[(pbyIn[i] >> 4) & 0x0f];
		*pszOut++=__hex[pbyIn[i] & 0x0f];
	}
}

LIBICPF_API bool hex2bin(const char_t* pszIn, uint_t tInCount, uchar_t *pbyOut)
{
	// we can pass -1 as in size - count it then
	if (tInCount == (uint_t)-1)
		tInCount=(uint_t)strlen(pszIn);

	// make sure the tInCount is even
	tInCount &= ~((size_t)1);
	byte_t by;
	for (size_t i=0;i<tInCount;i+=2)
	{
		// msb 4 bits
		if (*pszIn >= '0' && *pszIn <= '9')
			by=(byte_t)(*pszIn - '0') << 4;
		else if (*pszIn >= 'a' && *pszIn <= 'f')
			by=(byte_t)(*pszIn - 'a' + 10) << 4;
		else if (*pszIn >= 'A' && *pszIn <= 'F')
			by=(byte_t)(*pszIn - 'A' + 10) << 4;
		else
			return false;

		// lsb 4bits
		pszIn++;
		if (*pszIn >= '0' && *pszIn <= '9')
			by|=(*pszIn - '0');
		else if (*pszIn >= 'a' && *pszIn <= 'f')
			by|=(*pszIn - 'a' + 10);
		else if (*pszIn >= 'A' && *pszIn <= 'F')
			by|=(*pszIn - 'A' + 10);
		else
			return false;

		pszIn++;
		*pbyOut++=by;
	}

	return true;
}

END_ICPF_NAMESPACE
