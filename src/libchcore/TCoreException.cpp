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
#include "TCoreException.h"

// ============================================================================
/// chcore::TCoreException::TCoreException
/// @date 2009/11/30
///
/// @brief     Constructs the core exception object.
/// @param[in] eErrorCode		-	error code
/// @param[in] pszInternalError -   internal error message
// ============================================================================
BEGIN_CHCORE_NAMESPACE

TCoreException::TCoreException(EGeneralErrors eErrorCode, const tchar_t* pszInternalError) :
	m_eErrorCode(eErrorCode),
	m_strInternalMessage(pszInternalError ? pszInternalError : _t(""))
{
}

// ============================================================================
/// chcore::TCoreException::TCoreException
/// @date 2009/11/30
///
/// @brief     Constructs core exception object with additional data.
/// @param[in] eErrorCode -	       error code
/// @param[in] pszInternalError -  error description
/// @param[in] pszFile -           source file name
/// @param[in] stLineNumber -      source line number
/// @param[in] pszFunction -       function name in which the problem occured.
// ============================================================================
TCoreException::TCoreException(EGeneralErrors eErrorCode, const tchar_t* pszInternalError, const tchar_t* pszFile, size_t stLineNumber, const tchar_t* pszFunction) :
	m_eErrorCode(eErrorCode),
	m_strInternalMessage(pszInternalError ? pszInternalError : _t("")),
	m_strFile(pszFile ? pszFile : _t("")),
	m_strLineNumber(stLineNumber),
	m_strFunction(pszFunction ? pszFunction : _t(""))
{
}

END_CHCORE_NAMESPACE
