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
/** \file exception.cpp
 *  \brief Contain an implementation of an exception handling class.
 */
#include "exception.h"
#include <stdio.h>
#include <string.h>

BEGIN_ICPF_NAMESPACE

/// Defines max length of the exception description
#define MAX_EXCEPTION	4096

/** Constructor that takes the const description. The description (along with other tchar_t* parameters)
 *  are copied to the internal members (so there is some memory allocation).
 *
 * \param[in] pszDesc - exception description (currently should be in english)
 * \param[in] pszFilename - source file name from which the exception is thrown
 * \param[in] pszFunction - function name from which the exception is thrown
 * \param[in] uiLine - line in the source file from which the exception is thrown
 * \param[in] uiAppCode - application defined error code
 * \param[in] uiSystemCode - system error code (platform dependent)
 * \param[in] uiReserved - currently unused; must be 0
 */
exception::exception(const tchar_t* pszDesc, const tchar_t* pszFilename, const tchar_t* pszFunction, uint_t uiLine, uint_t uiAppCode, uint_t uiSystemCode, uint_t uiReserved) :
	m_pszDesc(NULL),
	m_pszFilename(NULL),
	m_pszFunction(NULL),
	m_uiLine(uiLine),
	m_uiAppCode(uiAppCode),
	m_uiSystemCode(uiSystemCode),
	m_uiReserved(uiReserved)
{
	set_string(&m_pszDesc, pszDesc);
	set_string(&m_pszFilename, pszFilename);
	set_string(&m_pszFunction, pszFunction);
#if defined(_DEBUG) && (defined(_WIN32) || defined(_WIN64))
	tchar_t szInfo[MAX_EXCEPTION];
	OutputDebugString(get_info(szInfo, MAX_EXCEPTION));
#endif
}

/** Constructor that takes the ptr to a buffer as a description. The pointer to a buffer is
 *  stored in the internal member and will be deleted in the destructor. Other tchar_t* parameters
 *  are copied to the internal members (so there is some memory allocated).
 *
 * \param[in] pszDesc - ptr to exception description (currently should be in english) allocated with new operator
 * \param[in] pszFilename - source file name from which the exception is thrown
 * \param[in] pszFunction - function name from which the exception is thrown
 * \param[in] uiLine - line in the source file from which the exception is thrown
 * \param[in] uiAppCode - application defined error code
 * \param[in] uiSystemCode - system error code (platform dependent)
 * \param[in] uiReserved - currently unused; must be 0
 */
exception::exception(tchar_t* pszDesc, const tchar_t* pszFilename, const tchar_t* pszFunction, uint_t uiLine, uint_t uiAppCode, uint_t uiSystemCode, uint_t uiReserved) :
	m_pszDesc(pszDesc),
	m_pszFilename(NULL),
	m_pszFunction(NULL),
	m_uiLine(uiLine),
	m_uiAppCode(uiAppCode),
	m_uiSystemCode(uiSystemCode),
	m_uiReserved(uiReserved)
{
	set_string(&m_pszFilename, pszFilename);
	set_string(&m_pszFunction, pszFunction);
#if defined(_DEBUG) && (defined(_WIN32) || defined(_WIN64))
	tchar_t szInfo[MAX_EXCEPTION];
	OutputDebugString(get_info(szInfo, MAX_EXCEPTION));
#endif
}

/** Copy constructor for the exception class.
 *
 * \param[in] rSrc - source exception to copy data from
 */
exception::exception(const exception& rSrc) :
	m_pszDesc(NULL),
	m_pszFilename(NULL),
	m_pszFunction(NULL),
	m_uiLine(rSrc.m_uiLine),
	m_uiAppCode(rSrc.m_uiAppCode),
	m_uiSystemCode(rSrc.m_uiSystemCode),
	m_uiReserved(rSrc.m_uiReserved)
{
	set_string(&m_pszDesc, rSrc.m_pszDesc);
	set_string(&m_pszFilename, rSrc.m_pszFilename);
	set_string(&m_pszFunction, rSrc.m_pszFunction);
}

/** Destructor deletes all the allocated memory for the exception object
 */
exception::~exception()
{
	delete [] m_pszDesc;
	delete [] m_pszFilename;
	delete [] m_pszFunction;
}

/** Assigns another exception data to this object.
 *
 * \param[in] e - source exception to copy from.
 * \return Reference to this object.
 */
exception& exception::operator=(const exception& eSrc)
{
	if (this != &eSrc)
	{
		delete [] m_pszDesc;
		delete [] m_pszFilename;
		delete [] m_pszFunction;

		set_string(&m_pszDesc, eSrc.m_pszDesc);
		set_string(&m_pszFilename, eSrc.m_pszFilename);
		set_string(&m_pszFunction, eSrc.m_pszFunction);
		m_uiLine=eSrc.m_uiLine;
		m_uiAppCode=eSrc.m_uiAppCode;
		m_uiSystemCode=eSrc.m_uiSystemCode;
		m_uiReserved=eSrc.m_uiReserved;
	}

	return *this;
}

/** Function retrieves the full information about the exception into
 *  the string buffer specified by user.
 * \param[out] pszInfo - buffer fot the full exception description
 * \param[in] tMaxLen - size of the specified buffer
 * \return Pointer to the exception description (to the pszInfo to be specific)
 */
const tchar_t* exception::get_info(tchar_t* pszInfo, size_t stMaxLen)
{
	_sntprintf(pszInfo, stMaxLen, _t("description: ") TSTRFMT _t("\nfile: ") TSTRFMT _t("\nfunction: ") TSTRFMT _t("\nline: ") ULFMT _t("\napp code: ") ULFMT _t("\nsys code: ") ULFMT _t("\nreserved: ") ULFMT _t("\n"),
			m_pszDesc, m_pszFilename, m_pszFunction, m_uiLine, m_uiAppCode, m_uiSystemCode, m_uiReserved);
	pszInfo[stMaxLen-1]=_t('\0');
	
	return pszInfo;
}

/** Function logs the full information about an exception to the specified log file.
 * \param[in] pszDesc - additional description of the exception object
 * \param[in] plog - pointer to a log file to log the exception to
 */
void exception::log(const tchar_t* pszDesc, log_file* plog)
{
	plog->logev(TSTRFMT _t("\n\tdesc: ") TSTRFMT _t("\n\tfile: ") TSTRFMT _t("\n\tfunc: ") TSTRFMT _t("\n\tline: ") ULFMT _t("\n\tapp code: ") ULFMT _t("\n\tsys code: ") ULFMT _t("\n\treserved: ") ULFMT _t("\n"),
		pszDesc, m_pszDesc, m_pszFilename, m_pszFunction, m_uiLine, m_uiAppCode, m_uiSystemCode, m_uiReserved);
}

/** Function logs the full information about an exception to the specified log file.
 *  Function takes two additional descriptions - will be logged as separated by space one description.
 * \param[in] pszDesc - additional description of the exception object
 * \param[in] pszDesc2 - the second part of an additional description
 * \param[in] plog - pointer to a log file to log the exception to
 */
void exception::log(const tchar_t* pszDesc, const tchar_t* pszDesc2, log_file* plog)
{
	plog->logev(TSTRFMT _t(" ") TSTRFMT _t("\n\tdesc: ") TSTRFMT _t("\n\tfile: ") TSTRFMT _t("\n\tfunc: ") TSTRFMT _t("\n\tline: ") ULFMT _t("\n\tapp code: ") ULFMT _t("\n\tsys code: ") ULFMT _t("\n\treserved: ") ULFMT _t("\n"),
		pszDesc, pszDesc2, m_pszDesc, m_pszFilename, m_pszFunction, m_uiLine, m_uiAppCode, m_uiSystemCode, m_uiReserved);
}

/** Exception's description formatting routine. Acts just as normal sprintf
 *  function, but allocates a buffer for the output result and returns a pointer
 *  to it. Used for formatting the exception description - usage:
 *  THROW(exception::format(_t("test ") TSTRFMT , _t("abc")), ...).
 *  It will enforce compiler to use the second constructor (non-const description).
 *  And the allocated buffer (result of this func) will be freed.
 * \param[in] pszFormat - format string followed by some additional data (as in printf)
 * \return Pointer to the newly allocated buffer with formatted output.
 */
tchar_t* exception::format(const tchar_t* pszFormat, ...)
{
	va_list vl;
	va_start(vl, pszFormat);

	// alloc some space - no more than MAX_EXCEPTION chracters
	tchar_t* psz=new tchar_t[(size_t)MAX_EXCEPTION];
	_vsntprintf(psz, (size_t)MAX_EXCEPTION, pszFormat, vl);
	psz[MAX_EXCEPTION-1]=_t('\0');
	return psz;
}

/** Allocates a string buffer and makes a copy of an input data. Used to 
 *  make a copy of the constructor string parameteres.
 * \param[out] pszOut - pointer to tchar_t* which will receive the new buffer address
 * \param[in] pszIn - string to make a copy of
 */
void exception::set_string(tchar_t** pszOut, const tchar_t* pszIn) const
{
	*pszOut=new tchar_t[_tcslen(pszIn)+(uint_t)1];
	_tcscpy(*pszOut, pszIn);
}

END_ICPF_NAMESPACE
