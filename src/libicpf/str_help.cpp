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
/** \file str_help.cpp
 *  \brief Contain implementation of some string helper functions.
 */
#include "str_help.h"

BEGIN_ICPF_NAMESPACE

/** Checks if the character is a whitespace.
 * \param[in] ch - character to check
 * \return True if the character is a whitespace one, false otherwise.
 */
bool string_tool::is_whitespace(tchar_t ch)
{
	return ((ch >= 0x09) && (ch <= 0x0d)) || (ch == 0x20);
}

END_ICPF_NAMESPACE
