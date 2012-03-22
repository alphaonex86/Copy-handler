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
/** \file str_help.h
 *  \brief Contain some string helper functions.
 */
#ifndef __STRHELP_H__
#define __STRHELP_H__

#include "libicpf.h"
#include "gen_types.h"

BEGIN_ICPF_NAMESPACE

// some cross-platform compatibility macros
#ifndef _WIN32
    #define stricmp strcasecmp
    #define wcsicmp wcscasecmp
    #define strnicmp strncasecmp
    #define wcsnicmp wcsncasecmp
#endif

class LIBICPF_API string_tool
{
public:
	/// Checks if a given character is a whitespace character
	static bool is_whitespace(tchar_t ch);
};

END_ICPF_NAMESPACE

#endif
