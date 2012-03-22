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
/** \file exception.h
 *  \brief Contain an exception handling class.
 */
 
#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "log.h"
#include "libicpf.h"
#include "gen_types.h"

#undef THROW
/** \brief Macro used for throwing an exception the easy way.
 *
 *  Macro throws an exception specified by the parameters. Other params needed by
 *  the exception class are taken indirectly from the position of a macro in the
 *  source file.
 * \param[in] desc - error description; if any formatting should be used use exception::format() function
 * \param[in] app_code - application error code (definitions are in engine_defs.h)
 * \param[in] sys_code - system error code (platform specific)
 * \param[in] reserved_code - currently unused; must be 0
 */
#define THROW(desc,app_code,sys_code,reserved_code) throw icpf::exception(desc, _t(__FILE__), _t(__FUNCTION__), __LINE__, app_code, sys_code, reserved_code)
/// Logs an exception in a log file
#define LOG_EXCEPTION(except, ptr_log) (except)->log(_t("Caught an exception in "), _t(__FUNCTION__), ptr_log)
/// Logs an unknown exception in a log file
#define LOG_UEXCEPTION(ptr_log) (ptr_log)->loge(_t("Caught an unknown exception in ") TSTRFMT, _t(__FUNCTION__))

BEGIN_ICPF_NAMESPACE

/** \brief Exception class used by most of the engine.
 *
 *  Exception class thrown by most of the engine functions. Provides user
 *  with an additional formatting and outputting capabilities.
 */
class LIBICPF_API exception
{
public:
/** \name Construction/destruction */
/**@{*/
	/// Standard constructor that takes the const description
	exception(const tchar_t* pszDesc, const tchar_t* pszFilename, const tchar_t* pszFunction, uint_t uiLine, uint_t uiAppCode, uint_t uiSystemCode, uint_t uiReserved);
	/// Standard constructor that takes non-const ptr to a buffer as the description
	exception(tchar_t* pszDesc, const tchar_t* pszFilename, const tchar_t* pszFunction, uint_t uiLine, uint_t uiAppCode, uint_t uiSystemCode, uint_t uiReserved);
	/// Copy constructor
	exception(const exception& rSrc);
	/// Standard destructor
	~exception();

	/// Assignment operator
	exception& operator=(const exception& eSrc);
/**@}*/
	
/** \name Outputting */
/**@{*/
	const tchar_t* get_info(tchar_t* pszInfo, size_t stMaxLen);	///< Retrieves the exception information to a specified string buffer
	const tchar_t* get_desc() const { return m_pszDesc; };
	const tchar_t* get_filename() const { return m_pszFilename; };
	const tchar_t* get_function() const { return m_pszFunction; };
	uint_t get_line() const { return m_uiLine; };
	uint_t get_appcode() const { return m_uiAppCode; };
	uint_t get_syscode() const { return m_uiSystemCode; };

	void log(const tchar_t* pszDesc, log_file* plog);		///< Logs the exception information to the log file
	void log(const tchar_t* pszDesc, const tchar_t* pszDesc2, log_file* plog);	///< Logs the exception to the log file with an additional description
/**@}*/

/** \name Formatting */
/**@{*/
	static tchar_t* format(const tchar_t* pszFormat, ...);	///< Description formatting function
/**@}*/

protected:
	void set_string(tchar_t** pszOut, const tchar_t* pszIn) const;	///< Makes a copy of an input string

protected:
	tchar_t* m_pszDesc;			///< Exception description
	tchar_t* m_pszFilename;		///< Source file in which the exception has been thrown
	tchar_t* m_pszFunction;		///< Function name in the source file in which the exception has been thrown
	uint_t m_uiLine;			///< Line in the source file in which the exception has been thrown
	uint_t m_uiAppCode;			///< Application error code
	uint_t m_uiSystemCode;		///< System error code (platform dependent)
	uint_t m_uiReserved;		///< Reserved code - currently unused and should be 0
};

END_ICPF_NAMESPACE

#endif
