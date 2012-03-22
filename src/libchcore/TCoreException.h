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
#ifndef __TEXCEPTION_H__
#define __TEXCEPTION_H__

#include "libchcore.h"

BEGIN_CHCORE_NAMESPACE

// throws core exception object
#define THROW_CORE_EXCEPTION(error_code)\
	throw TCoreException(error_code, _t(""), __FILEW__, __LINE__, __FUNCTIONW__)
#define THROW_CORE_EXCEPTION_STR(error_code, error_string)\
	throw TCoreException(error_code, error_string, __FILEW__, __LINE__, __FUNCTIONW__)

class LIBCHCORE_API TCoreException
{
public:
	TCoreException(EGeneralErrors eErrorCode, const tchar_t* pszInternalError);
	TCoreException(EGeneralErrors eErrorCode, const tchar_t* pszInternalError, const tchar_t* pszFile, size_t stLineNumber, const tchar_t* pszFunction);

	// error information
	EGeneralErrors GetErrorCode() const { return m_eErrorCode; }
	tstring_t GetInternalErrorString() const { return m_strInternalMessage; }

	// location info
	tstring_t GetSourceFile() const { return m_strFile; }
	size_t GetSourceLineNumber() const { return m_strLineNumber; }
	tstring_t GetFunctionName() const { return m_strFunction; }

private:
	TCoreException() {}

protected:
	// what happened?
	tstring_t m_strInternalMessage;
	EGeneralErrors m_eErrorCode;

	// where it happened?
	tstring_t m_strFile;
	size_t m_strLineNumber;
	tstring_t m_strFunction;
};

END_CHCORE_NAMESPACE

#endif
